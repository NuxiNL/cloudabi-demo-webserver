// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <program.h>

#include <cstdlib>
#include <memory>

#include <arpc++/arpc++.h>
#include <flower/protocol/switchboard.ad.h>

#include "configuration.ad.h"
#include "fd_streambuf.h"
#include "fixed_response_server.h"
#include "logger.h"

using arpc::ArgdataParser;
using arpc::ClientContext;
using arpc::CreateChannel;
using arpc::ServerBuilder;
using arpc::Status;
using flower::protocol::switchboard::ServerStartRequest;
using flower::protocol::switchboard::ServerStartResponse;
using flower::protocol::switchboard::Switchboard;
using webserver::Configuration;

void program_main(const argdata_t* ad) {
  // Parse the configuration provided on startup.
  Configuration configuration;
  {
    ArgdataParser argdata_parser;
    configuration.Parse(*ad, &argdata_parser);
  }

  // Make logging work.
  const auto& logger_output = configuration.logger_output();
  if (!logger_output)
    std::exit(1);
  fd_streambuf logger_streambuf(configuration.logger_output());
  Logger logger(&logger_streambuf);

  // Start a server through the switchboard.
  ServerStartResponse response;
  {
    const auto& http_switchboard = configuration.http_switchboard();
    if (!http_switchboard) {
      logger.Log() << "Cannot start without a switchboard socket";
      std::exit(1);
    }
    auto stub = Switchboard::NewStub(CreateChannel(http_switchboard));
    ClientContext context;
    ServerStartRequest request;
    if (Status status = stub->ServerStart(&context, request, &response);
        !status.ok()) {
      logger.Log() << status.error_message();
      std::exit(1);
    }
  }

  // Process incoming requests.
  ServerBuilder builder(response.server());
  FixedResponseServer fixed_response_server(configuration.html_response(),
                                            &logger);
  builder.RegisterService(&fixed_response_server);
  auto server = builder.Build();
  int error;
  while ((error = server->HandleRequest()) == 0) {
  }
  if (error > 0) {
    logger.Log() << "Failed to process incoming request: "
                 << std::strerror(errno);
    std::exit(1);
  }
  std::exit(0);
}
