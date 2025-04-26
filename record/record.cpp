#include "record.h"

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/stream_file.hpp>
#include <boost/endian/conversion.hpp>

const int MAX_FILE_SIZE = 1024 * 1024 * 50;

namespace Record {

namespace asio = boost::asio;

asio::awaitable<int> Record::write(const std::string& buffer) {
  auto ctx = co_await asio::this_coro::executor;
  if (!m_fp || m_writed_size >= MAX_FILE_SIZE) {
    co_await open_file(m_filename);
  }
  auto buffer_size = buffer.size();
  int32_t netending_buffer_size = boost::endian::native_to_big(buffer_size);

  co_await m_fp->async_write_some(asio::buffer(&netending_buffer_size, sizeof(netending_buffer_size)));
  co_await m_fp->async_write_some(buffer);

  m_writed_size += buffer_size + sizeof(netending_buffer_size);

  co_return 0;
}

Record::Record(const std::string& filename) : m_filename(filename), index(0) {}

asio::awaitable<int> Record::open_file(const std::string& filename) {
  auto ctx = co_await asio::this_coro::executor;
  if (m_fp) {
    m_fp->close();
  }
  const std::string full_path = filename + "." + std::to_string(index);
  m_fp = std::make_shared<asio::stream_file>(ctx, full_path, asio::file_base::create | asio::file_base::write_only);
  m_writed_size = 0;

  co_return 0;
}

}
