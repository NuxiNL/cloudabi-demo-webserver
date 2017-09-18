// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <program.h>

#include <cstdlib>
#include <memory>

#include <arpc++/arpc++.h>
#include <flower/protocol/switchboard.ad.h>

#include "configuration.ad.h"
#include "fixed_response_server.h"

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

  // Start a server through the switchboard.
  ServerStartResponse response;
  {
    const auto& http_switchboard = configuration.http_switchboard();
    if (!http_switchboard)
      std::exit(1);
    auto stub = Switchboard::NewStub(CreateChannel(http_switchboard));
    ClientContext context;
    ServerStartRequest request;
    if (Status status = stub->ServerStart(&context, request, &response);
        !status.ok())
      std::exit(1);
  }

  // Process incoming requests.
  ServerBuilder builder(response.server());
  FixedResponseServer fixed_response_server(configuration.html_response());
  builder.RegisterService(&fixed_response_server);
  for (auto server = builder.Build(); server->HandleRequest() == 0;) {
  }
  std::exit(0);
}
