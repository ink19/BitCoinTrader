#ifndef __COMMON_UTILS_COMPONENT_HPP__
#define __COMMON_UTILS_COMPONENT_HPP__

#include <glog/logging.h>

#include <boost/json.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/pfr.hpp>
#include <boost/pfr/core_name.hpp>
#include <type_traits>
#include "type_traits.hpp"

namespace Common {

using dec_float = boost::multiprecision::cpp_dec_float_100;

template <typename T>
class DataPrinter {
public:
  DataPrinter(const T& data, int depth = 1) : data_(data), depth_(depth) {}

  template<typename U>
  static U& print(U& os, const T& data, int depth = 1) {
    if constexpr (is_shared_v<T>) {
      using BaseFieldType = remove_shared_t<T>;
      os << DataPrinter<BaseFieldType>(*data, depth + 1);
    } else {
      boost::pfr::for_each_field(data, [&](auto&& field, auto index) {
        using FieldType = std::decay_t<decltype(field)>;
        if constexpr (is_vector_v<FieldType>) {
          os << boost::pfr::get_name<index, T>() << ": [";
          for (size_t i = 0; i < field.size(); ++i) {
            os << DataPrinter<typename FieldType::value_type>(field[i], depth + 1);
            if (i != field.size() - 1) {
              os << ", ";
            }
          }
          os << "]; ";
        } else if constexpr (std::is_same_v<FieldType, dec_float>) {
          os << boost::pfr::get_name<index, T>() << ": " << field << "; ";
        } else if constexpr (std::is_base_of_v<FieldType, std::string>) {
          os << boost::pfr::get_name<index, T>() << ": " << field << "; ";
        } else if constexpr (std::is_integral_v<FieldType>) {
          os << boost::pfr::get_name<index, T>() << ": " << field << "; ";
        } else if constexpr (std::is_object_v<FieldType>) {
          os << boost::pfr::get_name<index, T>() << ": {";
          os << DataPrinter<FieldType>(field, depth + 1);
          os << "}; ";
        } else {
          os << boost::pfr::get_name<index, T>() << ": " << field << "; ";
        }
      });
    }
    return os;
  }

  template <typename U>
  friend U& operator<<(U& os, const DataPrinter<T>& data) {
    return print(os, data.data_, data.depth_ + 1);
  }

private:
  const T& data_;
  int depth_;
};

template <typename T>
class DataReader {
 public:
  static T read(const boost::json::object& obj) {
    T data;
    if constexpr (is_shared_v<T>) {
      using BaseFieldType = remove_shared_t<T>;
      data = std::make_shared<BaseFieldType>();
      DataReader<BaseFieldType>::read(obj, *data);
    } else {
      read(obj, data);
    }
    return data;
  }

  static T read(const boost::json::object& obj, T &data) {
    boost::pfr::for_each_field(data, [&](auto&& field, auto index) {
      using FieldType = std::decay_t<decltype(field)>;
      if (obj.contains(boost::pfr::get_name<index, T>())) {
        if constexpr (std::is_same_v<FieldType, std::string>) {
          field = obj.at(boost::pfr::get_name<index, T>()).as_string().c_str();
        } else if constexpr (std::is_integral_v<FieldType>) {
          if (obj.at(boost::pfr::get_name<index, T>()).is_string()) {
            auto ps = std::string(obj.at(boost::pfr::get_name<index, T>()).as_string());
            if (ps.empty()) {
              field = 0;
            } else {
              field = std::stoll(ps);
            }
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_int64()) {
            field = obj.at(boost::pfr::get_name<index, T>()).as_int64();
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_uint64()) {
            field = obj.at(boost::pfr::get_name<index, T>()).as_uint64();
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_double()) {
            field = obj.at(boost::pfr::get_name<index, T>()).as_double();
          }
        } else if constexpr (std::is_floating_point_v<FieldType>) {
          if (obj.at(boost::pfr::get_name<index, T>()).is_int64()) {
            field = static_cast<FieldType>(obj.at(boost::pfr::get_name<index, T>()).as_int64());
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_double()) {
            field = static_cast<FieldType>(obj.at(boost::pfr::get_name<index, T>()).as_double());
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_string()) {
            auto value = obj.at(boost::pfr::get_name<index, T>()).as_string();
            if (value.empty()) {
              field = static_cast<FieldType>(0);
            } else {
              field = std::stof(std::string(value));
            }
          }
        } else if constexpr (std::is_same_v<FieldType, bool>) {
          if (obj.at(boost::pfr::get_name<index, T>()).is_string()) {
            std::string fv = obj.at(boost::pfr::get_name<index, T>()).as_string().c_str();
            if (!fv.empty()) {
              field = fv == "true" ? true : false;
            } else {
              field = false;
            }
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_bool()) {
            field = obj.at(boost::pfr::get_name<index, T>()).as_bool();
          } else if (obj.at(boost::pfr::get_name<index, T>()).is_int64()) {
            field = obj.at(boost::pfr::get_name<index, T>()).as_int64() != 0;
          }
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
            field = DataReader<FieldType>::read(field_json.as_array());
          } else {
            LOG(ERROR) << "Expected array for field: " << boost::pfr::get_name<index, T>();
          }
        } else if constexpr (std::is_object_v<FieldType>) {
          if (obj.at(boost::pfr::get_name<index, T>()).is_object()) {
            field = DataReader<FieldType>::read(obj.at(boost::pfr::get_name<index, T>()).as_object());
          } else {
            LOG(ERROR) << "Expected object for field: " << boost::pfr::get_name<index, T>();
          }
        } else {
          LOG(ERROR) << "Unsupported type: " << typeid(FieldType).name();
        }
      }
    });
    return data;
  }

  static T read(const boost::json::array& arr) {
    T data;
    if constexpr (is_vector_v<T>) {
      for (size_t i = 0; i < arr.size(); ++i) {
        auto p_value = DataReader<typename T::value_type>::read(arr.at(i).as_object());
        data.push_back(p_value);
      }
    }
    return data;
  }

  static T read(const boost::json::value& obj) {
    if (obj.is_object()) {
      return read(obj.as_object());
    } else if (obj.is_array()) {
      return read(obj.as_array());
    } else {
      LOG(ERROR) << "Expected object";
      return T();
    }
  }
};

// typedef std::string StringEnum;

class StringEnum : public std::string {
public:
  StringEnum() = default;
  explicit StringEnum(const std::string& str) : std::string(str) {}
  const std::string& ToString() {
    return *this;
  }

  template<typename T>
  static T FromString(const std::string& str) {
    return T(str);
  }

  StringEnum& operator=(const std::string& str) {
    return *this = StringEnum(str);
  }
};

}  // namespace Common

#endif
