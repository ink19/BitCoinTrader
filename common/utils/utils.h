#ifndef __COMMON_UTILS_H
#define __COMMON_UTILS_H

#include <cstdint>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/asio.hpp>
#include <fmt/format.h>
#include <jsoncpp/jsoncpp.hpp>

namespace asio = boost::asio;


using dec_float = boost::multiprecision::cpp_dec_float_50;
namespace jsoncpp {

template<>
struct transform<dec_float> {
    static void trans(const bj::value &jv, dec_float &t) {
        if (jv.is_string()) {
            t = dec_float(jv.as_string().c_str());
        } else if (jv.is_double()) {
            t = dec_float(jv.as_double());
        } else if (jv.is_int64()) {
            t = dec_float(jv.as_int64());
        } else {
            throw std::runtime_error("invalid type");
        }
    }
};
}


namespace Common {

extern std::string sha256_hash_base64(const std::string& input, const std::string& key);
extern int64_t get_current_time_s();
extern std::string time_format_iso(const int64_t& time);

template<typename T>
class SingletonPtr {
 public:
  static std::shared_ptr<T> get_instance() {
    static std::shared_ptr<T> instance = std::make_shared<T>();  // C++11后保证线程安全
    return instance;
  }

 private:
  SingletonPtr() {}  // 构造函数私有化
  SingletonPtr(const SingletonPtr&) = delete;
  SingletonPtr& operator=(const SingletonPtr&) = delete;
};

}  // namespace Common

#endif
