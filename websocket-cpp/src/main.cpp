#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>
#include <boost/asio.hpp>

#include "http_server.h"

int main(int argc, char* argv[]) {
  CLI::App app("websocket-server");

  int log_level = spdlog::level::info;
  int port = 3000;

  auto log_level_map = std::vector<std::pair<std::string, int>>(
      {{"trace", (int)spdlog::level::trace},
       {"debug", (int)spdlog::level::debug},
       {"info", (int)spdlog::level::info},
       {"warn", (int)spdlog::level::warn},
       {"error", (int)spdlog::level::err},
       {"critical", (int)spdlog::level::critical},
       {"off", (int)spdlog::level::off}});
  app.add_option("--log-level", log_level, "ログレベル")
      ->transform(CLI::CheckedTransformer(log_level_map, CLI::ignore_case));
  app.add_option("--port", port, "ポート番号(デフォルトは3000)")
      ->check(CLI::Range(0, 65535));

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  spdlog::set_level((spdlog::level::level_enum)log_level);

  boost::asio::io_context ioc{1};
  const boost::asio::ip::tcp::endpoint endpoint{
      boost::asio::ip::make_address("0.0.0.0"), static_cast<uint16_t>(port)};
  std::make_shared<HttpServer>(ioc, endpoint)->run();
  ioc.run();
}
