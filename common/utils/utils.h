#ifndef __COMMON_UTILS_H
#define __COMMON_UTILS_H

#include <cstdint>
#include <string>

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

}  // namespace Common

#endif