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
    
    google::ShutdownGoogleLogging();
    return 0;
}
