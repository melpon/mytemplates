#ifndef HTTP_SERVER_H_INCLUDED
#define HTTP_SERVER_H_INCLUDED

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>

class HttpServer : public std::enable_shared_from_this<HttpServer> {
  boost::asio::io_context& ioc_;
  boost::asio::ip::tcp::acceptor acceptor_;

 public:
  HttpServer(boost::asio::io_context& ioc,
             boost::asio::ip::tcp::endpoint endpoint);

  void run();

 private:
  void doAccept();
  void onAccept(boost::system::error_code ec,
                boost::asio::ip::tcp::socket socket);
};

#endif  // HTTP_SERVER_H_INCLUDED
