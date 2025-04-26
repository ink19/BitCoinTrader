#ifndef __RECORD_H
#define __RECORD_H

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace Record {

namespace asio = boost::asio;
class Record {
public:
  Record(const std::string& filename);

  asio::awaitable<int> write(const std::string& buffer);

  template<typename T>
  asio::awaitable<int> write(const T& buffer) {
    std::stringstream sb;
    boost::archive::text_oarchive oa(sb);
    oa << buffer;
    return write(sb.str());
  }

private:
  asio::awaitable<int> open_file(const std::string& filename);

  std::string m_filename;
  std::shared_ptr<asio::stream_file> m_fp;
  uint64_t m_writed_size;
  int index;
};


}  // namespace Record

#endif  // RECORD_H
