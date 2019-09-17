# C++ Websocket テンプレート

Boost.Beast を使った Websocket サーバのテンプレートです。

以下のライブラリを利用しています。

- [Boost](https://www.boost.org/)
- [nlohmann/json](https://github.com/nlohmann/json)
- [CLI11](https://github.com/CLIUtils/CLI11)
- [spdlog](https://github.com/gabime/spdlog)

## 環境構築

```bash
./install.sh
```

## ビルド

ビルドには CMake を利用しているので、あらかじめインストールしておく必要があります。

```bash
./build.sh
```

## 実行

```bash
./websocket-server
```

`/ws` に繋ぐと Websocket に繋がります。それ以外は静的ファイルのリクエストになります。

Websocket につないだ状態で、`{"message": "string"}` のような形式の JSON データを送ると、ROT13 された値が受信できます。

`{"message": "az"}` を送信する → `{"message": "nm"}` を受信する
