#include "record.h"

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/stream_file.hpp>

namespace Record {

namespace asio = boost::asio;

asio::awaitable<int> write(const std::string& buffer) {
  auto ctx = co_await asio::this_coro::executor;
  auto fp = asio::stream_file(ctx, 
    "record.txt",
    asio::file_base::create | asio::file_base::write_only);
  
  co_await fp.async_write_some(asio::buffer(buffer), asio::use_awaitable);

  co_return 0;
}

}
