#ifndef __COMMON_UTILS_H
#define __COMMON_UTILS_H

#include <cstdint>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/asio.hpp>
#include <fmt/format.h>

using dec_float = boost::multiprecision::cpp_dec_float<50>;
namespace asio = boost::asio;


namespace Common {

extern std::string sha256_hash_base64(const std::string& input, const std::string& key);
extern int64_t get_current_time_s();
extern std::string time_format_iso(const int64_t& time);

template<typename T>
class Singleton {
 public:
  static T& getInstance() {
    static T instance;  // C++11后保证线程安全
    return instance;
  }

 private:
  Singleton() {}  // 构造函数私有化
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;
};

#define ELOG(LEVEL, FORMAT, ...) \
  co_await on_log(std::make_shared<engine::LogData>(LEVEL, fmt::format(FORMAT, ##__VA_ARGS__)))

#define ELOG_INFO(FORMAT, ...) ELOG(engine::LogLevel::kInfo, FORMAT, ##__VA_ARGS__)
#define ELOG_WARN(FORMAT, ...) ELOG(engine::LogLevel::kWarning, FORMAT, ##__VA_ARGS__)
#define ELOG_ERROR(FORMAT, ...) ELOG(engine::LogLevel::kError, FORMAT, ##__VA_ARGS__)
#define ELOG_DEBUG(FORMAT, ...) ELOG(engine::LogLevel::kDebug, FORMAT, ##__VA_ARGS__)

}  // namespace Common

#endif
