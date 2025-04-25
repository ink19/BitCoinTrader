#ifndef __RECORD_H
#define __RECORD_H

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>

namespace Record {

namespace asio = boost::asio;

class Record {
public:
  Record();

  asio::awaitable<int> write(const std::string& buffer);
private:
};

}  // namespace Record

#endif  // RECORD_H
