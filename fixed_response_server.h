// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef FIXED_RESPONSE_SERVER_H
#define FIXED_RESPONSE_SERVER_H

#include <arpc++/arpc++.h>

#include <string>
#include <string_view>

#include <flower/protocol/server.ad.h>

class Logger;

class FixedResponseServer : public flower::protocol::server::Server::Service {
 public:
  FixedResponseServer(std::string_view html_response, Logger* logger)
      : html_response_(html_response), logger_(logger) {
  }

  arpc::Status Connect(
      arpc::ServerContext* context,
      const flower::protocol::server::ConnectRequest* request,
      flower::protocol::server::ConnectResponse* response) override;

 private:
  const std::string html_response_;
  Logger* const logger_;
};

#endif
