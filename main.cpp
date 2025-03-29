#include <boost/asio/io_context.hpp>
#include <fmt/core.h>
#include <boost/asio.hpp>
#include <glog/logging.h>
#include "WebSocket.h"

int main(int argc, char* argv[])
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_minloglevel = google::INFO;
    FLAGS_logtostderr = true;

    Common::WebSocket WebSocket("wss://ws.okx.com:8443/ws/v5/public");

    google::ShutdownGoogleLogging();
    return 0;
}
