#ifndef __COMMON_UTILS_COMPONENT_HPP__
#define __COMMON_UTILS_COMPONENT_HPP__

#include <glog/logging.h>

#include <boost/json.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/pfr.hpp>
#include <boost/pfr/core_name.hpp>
#include <type_traits>

namespace Common {

using dec_float = boost::multiprecision::cpp_dec_float_100;

// 基础模板：默认非 vector 类型
template <typename T>
struct is_vector : std::false_type {};

// 偏特化：匹配所有 std::vector<T, Alloc> 类型
template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename _Tp>
  inline constexpr bool is_vector_v = is_vector<_Tp>::value;


template <typename T>
class DataPrinter {
public:
  DataPrinter(const T& data) : data_(data) {}

  template<typename U>
  static U& print(U& os, const T& data) {
    boost::pfr::for_each_field(data, [&](auto&& field, auto index) {
      using FieldType = std::decay_t<decltype(field)>;
      if constexpr (is_vector_v<FieldType>) {
        os << boost::pfr::get_name<index, T>() << ": [";
        for (size_t i = 0; i < field.size(); ++i) {
          os << DataPrinter<typename FieldType::value_type>(field[i]);
          if (i != field.size() - 1) {
            os << ", ";
          }
        }
        os << "];";
      } else {
        os << boost::pfr::get_name<index, T>() << ": " << field << ";";
      }
    });
    return os;
  }

  template <typename U>
  friend U& operator<<(U& os, const DataPrinter<T>& data) {
    return print(os, data.data_);
  }

private:
  const T& data_;
};

template <typename T>
class DataReader {
 public:
  static T read(const boost::json::object& obj) {
    T data;
    boost::pfr::for_each_field(data, [&](auto&& field, auto index) {
      using FieldType = std::decay_t<decltype(field)>;
      if (obj.contains(boost::pfr::get_name<index, T>())) {
        if constexpr (std::is_same_v<FieldType, std::string>) {
          field = obj.at(boost::pfr::get_name<index, T>()).as_string().c_str();
        } else if constexpr (std::is_integral_v<FieldType>) {
          if (obj.at(boost::pfr::get_name<index, T>()).is_string()) {
            std::string ps = std::string(obj.at(boost::pfr::get_name<index, T>()).as_string());
            field = std::stoll(ps);
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_int64()) {
            field = obj.at(boost::pfr::get_name<index, T>()).as_int64();
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_uint64()) {
            field = obj.at(boost::pfr::get_name<index, T>()).as_uint64();
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_double()) {
            field = static_cast<FieldType>(obj.at(boost::pfr::get_name<index, T>()).as_double());
          }
        } else if constexpr (std::is_floating_point_v<FieldType>) {
          field = obj.at(boost::pfr::get_name<index, T>()).as_double();
        } else if constexpr (std::is_same_v<FieldType, dec_float>) {
          if (obj.at(boost::pfr::get_name<index, T>()).is_string()) {
            std::string fv = obj.at(boost::pfr::get_name<index, T>()).as_string().c_str();
            if (!fv.empty()) {
              field = dec_float(fv);
            } else {
              field = dec_float(0);
            }
          }
        } else if constexpr (is_vector_v<FieldType>) {
          auto field_json = obj.at(boost::pfr::get_name<index, T>());
          if (field_json.is_array()) {
            for (size_t i = 0; i < field.size(); ++i) {
              auto p_value = DataReader<typename FieldType::value_type>::read(field_json.at(i).as_object());
              field.push_back(p_value);
            }
          } else {
            LOG(ERROR) << "Expected array for field: " << boost::pfr::get_name<index, T>();
          }
        } else {
          LOG(ERROR) << "Unsupported type: " << typeid(FieldType).name();
        }
      }
    });
    return data;
  }
};

}  // namespace Common

#endif
