#include <boost/asio/io_context.hpp>
#include <fmt/core.h>
#include <boost/asio.hpp>


int main()
{
    boost::asio::io_context ctx;
    fmt::print("Hello World!\n");
    return 0;
}
