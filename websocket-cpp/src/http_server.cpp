#include "http_server.h"

#include <spdlog/spdlog.h>
#include <boost/beast.hpp>

#include "http_session.h"

HttpServer::HttpServer(boost::asio::io_context& ioc,
                       boost::asio::ip::tcp::endpoint endpoint)
    : ioc_(ioc), acceptor_(ioc) {
  boost::system::error_code ec;

  // Open the acceptor
  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    SPDLOG_ERROR("open: {}", ec.message());
    return;
  }

  // Allow address reuse
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    SPDLOG_ERROR("set_option: {}", ec.message());
    return;
  }

  // Bind to the server address
  acceptor_.bind(endpoint, ec);
  if (ec) {
    SPDLOG_ERROR("bind: {}", ec.message());
    return;
  }

  // Start listening for connections
  acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    SPDLOG_ERROR("listen: {}", ec.message());
    return;
  }
}

void HttpServer::run() {
  if (!acceptor_.is_open()) return;
  doAccept();
}

void HttpServer::doAccept() {
  acceptor_.async_accept(boost::asio::make_strand(ioc_),
                         boost::beast::bind_front_handler(&HttpServer::onAccept,
                                                          shared_from_this()));
}

void HttpServer::onAccept(boost::system::error_code ec,
                          boost::asio::ip::tcp::socket socket) {
  if (ec) {
    SPDLOG_ERROR("accept: {}", ec.message());
  } else {
    std::make_shared<HttpSession>(std::move(socket))->run();
  }

  doAccept();
}
