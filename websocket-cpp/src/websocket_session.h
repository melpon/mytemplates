#ifndef WEBSOCKET_SESSION_H_INCLUDED
#define WEBSOCKET_SESSION_H_INCLUDED

#include <memory>
#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/system/error_code.hpp>

class WebsocketSession : public std::enable_shared_from_this<WebsocketSession> {
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws_;
  boost::beast::multi_buffer read_buffer_;
  std::vector<boost::beast::flat_buffer> write_buffer_;

 public:
  WebsocketSession(boost::asio::ip::tcp::socket socket);
  ~WebsocketSession();
  void run(boost::beast::http::request<boost::beast::http::string_body> req);

 private:
  void doAccept(
      boost::beast::http::request<boost::beast::http::string_body> req);
  void onAccept(boost::system::error_code ec);

  void doRead();
  void onRead(boost::system::error_code ec, std::size_t bytes_transferred);

  void sendText(std::string text);
  void doWrite();
  void onWrite(boost::system::error_code ec, std::size_t bytes_transferred);
};

#endif  // WEBSOCKET_SESSION_H_INCLUDED
