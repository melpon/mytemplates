#include "websocket_session.h"

#include <spdlog/spdlog.h>
#include <boost/asio/bind_executor.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/websocket/error.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <nlohmann/json.hpp>

#include "util.h"

using json = nlohmann::json;

WebsocketSession::WebsocketSession(boost::asio::ip::tcp::socket socket)
    : ws_(std::move(socket)) {
  SPDLOG_DEBUG("constructed WebsocketSession");
}

WebsocketSession::~WebsocketSession() {
  SPDLOG_DEBUG("destructed WebsocketSession");
}

void WebsocketSession::run(
    boost::beast::http::request<boost::beast::http::string_body> req) {
  doAccept(std::move(req));
}

void WebsocketSession::doAccept(
    boost::beast::http::request<boost::beast::http::string_body> req) {
  // Accept the websocket handshake
  ws_.async_accept(std::move(req),
                   boost::beast::bind_front_handler(&WebsocketSession::onAccept,
                                                    shared_from_this()));
}

void WebsocketSession::onAccept(boost::system::error_code ec) {
  if (ec) {
    SPDLOG_ERROR("onAccept: {}", ec.message());
    return;
  }

  doRead();
}

void WebsocketSession::doRead() {
  // WebSocket での読み込みを開始
  ws_.async_read(read_buffer_,
                 boost::beast::bind_front_handler(&WebsocketSession::onRead,
                                                  shared_from_this()));
}

void WebsocketSession::onRead(boost::system::error_code ec,
                              std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  const auto recv_text = boost::beast::buffers_to_string(read_buffer_.data());
  read_buffer_.consume(read_buffer_.size());

  if (ec == boost::beast::websocket::error::closed) {
    SPDLOG_INFO("websocket closed");
    return;
  }

  if (ec) {
    SPDLOG_ERROR("onRead: {}", ec.message());
    return;
  }

  SPDLOG_INFO("received recv_text: {}", recv_text);

  json recv_json;

  try {
    recv_json = json::parse(recv_text);
  } catch (json::parse_error& e) {
    SPDLOG_ERROR("invalid JSON format: {}", e.what());
    return;
  }

  if (recv_json.find("message") == recv_json.end()) {
    SPDLOG_ERROR("message not found");
    return;
  }

  std::string message = recv_json.at("message");
  std::transform(message.begin(), message.end(), message.begin(),
                 [](const char c) {
                   if ('a' <= c && c <= 'z') {
                     return (char)((((c - 'a') + 13) % 26) + 'a');
                   }
                   if ('A' <= c && c <= 'Z') {
                     return (char)((((c - 'A') + 13) % 26) + 'A');
                   }
                   return c;
                 });

  sendText(nlohmann::json::object({{"message", message}}).dump());

  doRead();
}

void WebsocketSession::sendText(std::string text) {
  SPDLOG_DEBUG("send text: {}", text);

  bool empty = write_buffer_.empty();
  boost::beast::flat_buffer buffer;

  const auto n = boost::asio::buffer_copy(buffer.prepare(text.size()),
                                          boost::asio::buffer(text));
  buffer.commit(n);

  write_buffer_.push_back(std::move(buffer));

  if (empty) {
    doWrite();
  }
}

void WebsocketSession::doWrite() {
  auto& buffer = write_buffer_.front();

  ws_.text(true);
  ws_.async_write(buffer.data(),
                  boost::beast::bind_front_handler(&WebsocketSession::onWrite,
                                                   shared_from_this()));
}

void WebsocketSession::onWrite(boost::system::error_code ec,
                               std::size_t bytes_transferred) {
  if (ec == boost::asio::error::operation_aborted) {
    return;
  }

  if (ec) {
    SPDLOG_ERROR("onWrite: {}", ec.message());
    return;
  }

  write_buffer_.erase(write_buffer_.begin());

  if (!write_buffer_.empty()) {
    doWrite();
  }
}
