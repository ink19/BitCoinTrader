#include <boost/asio/io_context.hpp>
#include <fmt/core.h>
#include <boost/asio.hpp>
#include <glog/logging.h>
#include "WebSocket.h"
#include "websocket_api.h"

int main(int argc, char* argv[])
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_minloglevel = google::INFO;
    FLAGS_logtostderr = true;
    

    LOG(INFO) << Market::Okx::sha256_hash_base64("Hello, World!");

    // boost::asio::io_context io_context;
    
    // LOG(INFO) << "Starting WebSocket client";
    // boost::asio::co_spawn(io_context, []() -> boost::asio::awaitable<void> {
    //     try {
    //         Common::WebSocket ws("wss://ws.okx.com:8443/ws/v5/public");
    //         co_await ws.connect();
    //         LOG(INFO) << "Connected to WebSocket server";
    //         co_await ws.write("{\"op\": \"subscribe\", \"args\": [{\"channel\": \"tickers\", \"instId\": \"ETH-USDT\"}]}");
    //         // co_await ws.write("ping");
    //         LOG(INFO) << "Subscribed to channel";

    //         while (true) {
    //             auto msg = co_await ws.read();
    //             fmt::print("Received message: {}\n", msg);
    //         }
    //     } catch (const std::exception& e) {
    //         LOG(ERROR) << "Error: " << e.what();
    //     }
    // }, boost::asio::detached);

    // io_context.run();
    // google::ShutdownGoogleLogging();
    return 0;
}
