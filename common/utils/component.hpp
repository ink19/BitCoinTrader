#ifndef __COMMON_UTILS_COMPONENT_HPP__
#define __COMMON_UTILS_COMPONENT_HPP__

/**
 * @file component.hpp
 * @brief 数据处理工具类
 * 
 * 提供了通用的数据处理功能：
 * - DataPrinter: 数据打印工具，支持复杂结构的格式化输出
 * - DataReader: JSON反序列化工具，将JSON转换为C++对象
 * - DataSerializer: JSON序列化工具，将C++对象转换为JSON
 * 
 * 使用Boost.PFR实现编译期反射，无需手动编写序列化代码。
 */

#include <glog/logging.h>

#include <boost/json.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/pfr.hpp>
#include <boost/pfr/core_name.hpp>
#include <type_traits>
#include "type_traits.hpp"

namespace Common {

/// 高精度浮点数类型，100位精度
using dec_float = boost::multiprecision::cpp_dec_float_100;

/**
 * @brief 数据打印工具类
 * 
 * 支持打印各种类型的数据，包括：
 * - 基本类型（int, float, string等）
 * - 容器类型（vector等）
 * - 智能指针类型
 * - 自定义结构体（通过Boost.PFR反射）
 * 
 * @tparam T 数据类型
 */
template <typename T>
class DataPrinter {
public:
  DataPrinter(const T& data, int depth = 1) : data_(data), depth_(depth) {}

  /**
   * @brief 打印数据到输出流
   * @tparam U 输出流类型
   * @param os 输出流
   * @param data 要打印的数据
   * @param depth 当前深度（用于嵌套结构）
   * @return U& 输出流引用
   */
  template<typename U>
  static U& print(U& os, const T& data, int depth = 1) {
    if constexpr (is_shared_v<T>) {
      using BaseFieldType = remove_shared_t<T>;
      os << DataPrinter<BaseFieldType>(*data, depth + 1);
    } else if constexpr (is_vector_v<T>) {
      os  << "[";
      for (size_t i = 0; i < data.size(); ++i) {
        os << DataPrinter<typename T::value_type>(data[i], depth + 1);
        if (i != data.size() - 1) {
          os << ", ";
        }
      }
      os << "]; ";
    } else if constexpr (std::is_same_v<T, dec_float>) {
      os  << data << "; ";
    } else if constexpr (std::is_base_of_v<T, std::string>) {
      os  << data << "; ";
    } else if constexpr (std::is_integral_v<T>) {
      os  << data << "; ";
    } else if constexpr (std::is_object_v<T>) {
      os  << "{";
      boost::pfr::for_each_field(data, [&](auto&& field, auto index) {
        using FieldType = std::decay_t<decltype(field)>;
        os << boost::pfr::get_name<index, T>() << ": " <<  DataPrinter<FieldType>(field, depth + 1) << "; ";
      });
      os << "}; ";
    } else {
      os << data << "; ";
    } 
    return os;
  }

  template<typename U>
  static U& print(U& os, const T& data) {

  }

  template <typename U>
  friend U& operator<<(U& os, const DataPrinter<T>& data) {
    return print(os, data.data_, data.depth_ + 1);
  }

private:
  const T& data_;
  int depth_;
};

/**
 * @brief JSON反序列化工具类
 * 
 * 将JSON数据转换为C++对象，支持：
 * - 基本类型转换
 * - 容器类型转换
 * - 智能指针类型转换
 * - 自定义结构体转换（通过Boost.PFR反射）
 * 
 * @tparam T 目标数据类型
 */
template <typename T>
class DataReader {
 public:
  /**
   * @brief 从JSON值读取数据
   * @param obj JSON值
   * @return T 转换后的数据
   */
  static T read(const boost::json::value& obj) {
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
        DataReader<FieldType>::read(obj.at(boost::pfr::get_name<index, T>()), field);
      }
    });
    return data;
  }

  static void read(const boost::json::array& arr, T &data) {
    if constexpr (is_vector_v<T>) {
      for (size_t i = 0; i < arr.size(); ++i) {
        auto p_value = DataReader<typename T::value_type>::read(arr.at(i));
        data.push_back(p_value);
      }
    }
  }

  static void read(const boost::json::value& jvalue, T &data) {
    if constexpr (is_shared_v<T>) {
      using BaseFieldType = remove_shared_t<T>;
      data = std::make_shared<BaseFieldType>();
      DataReader<BaseFieldType>::read(jvalue, *data);
    } else if constexpr (std::is_same_v<T, std::string>) {
      data = jvalue.as_string().c_str();
    } else if constexpr (std::is_integral_v<T>) {
      if (jvalue.is_string()) {
        auto ps = std::string(jvalue.as_string());
        if (ps.empty()) {
          data = 0;
        } else {
          data = std::stoll(ps);
        }
      } else if (jvalue.is_int64()) {
        data = jvalue.as_int64();
      } else if (jvalue.is_uint64()) {
        data = jvalue.as_uint64();
      } else if (jvalue.is_double()) {
        data = jvalue.as_double();
      }
    } else if constexpr (std::is_floating_point_v<T>) {
      if (jvalue.is_int64()) {
        data = static_cast<T>(jvalue.as_int64());
      } else if (jvalue.is_double()) {
        data = static_cast<T>(jvalue.as_double());
      } else if (jvalue.is_string()) {
        auto value = jvalue.as_string();
        if (value.empty()) {
          data = static_cast<T>(0);
        } else {
          data = std::stof(std::string(value));
        }
      }
    } else if constexpr (std::is_same_v<T, bool>) {
      if (jvalue.is_string()) {
        std::string fv = jvalue.as_string().c_str();
        if (!fv.empty()) {
          data = fv == "true" ? true : false;
        } else {
          data = false;
        }
      } else if (jvalue.is_bool()) {
        data = jvalue.as_bool();
      } else if (jvalue.is_int64()) {
        data = jvalue.as_int64() != 0;
      }
    } else if constexpr (std::is_same_v<T, dec_float>) {
      if (jvalue.is_string()) {
        std::string fv = jvalue.as_string().c_str();
        if (!fv.empty()) {
          data = dec_float(fv);
        } else {
          data = dec_float(0);
        }
      } else {
        data = dec_float(0);
      }
    } else if constexpr (is_vector_v<T>) {
      if (jvalue.is_array()) {
        DataReader<T>::read(jvalue.as_array(), data);
      } else {
        LOG(ERROR) << "Expected array for field";
      }
    } else if constexpr (std::is_object_v<T>) {
      if (jvalue.is_object()) {
        DataReader<T>::read(jvalue.as_object(), data);
      } else {
        LOG(ERROR) << "Expected object for field";
      }
    } else {
      LOG(ERROR) << "Unsupported type: " << typeid(T).name();
    }
  }

  static std::shared_ptr<T> read_shared_ptr(const boost::json::value& obj) {
    return DataReader<std::shared_ptr<T>>::read(obj);
  }
};

/**
 * @brief JSON序列化工具类
 * 
 * 将C++对象转换为JSON数据，支持：
 * - 基本类型序列化
 * - 容器类型序列化
 * - 智能指针类型序列化
 * - 自定义结构体序列化（通过Boost.PFR反射）
 * 
 * @tparam T 源数据类型
 */
template <typename T>
class DataSerializer {
public:
  /**
   * @brief 将数据序列化为JSON
   * @param data 要序列化的数据
   * @return boost::json::value JSON值
   */
  static boost::json::value write(const T& data) {
    boost::json::value val;
    if constexpr (is_shared_v<T>) {
      using BaseFieldType = remove_shared_t<T>;
      val = DataSerializer<BaseFieldType>::write(*data);
    } else if constexpr (is_vector_v<T>) {
      boost::json::array arr;
      for (auto& item : data) {
        arr.push_back(DataSerializer<typename T::value_type>::write(item));
      }
      val = arr;
    } else if constexpr (std::is_same_v<T, dec_float>) {
      val = boost::json::value(data.str());
    } else if constexpr (std::is_base_of_v<T, std::string>) {
      val = boost::json::value(data);
    } else if constexpr (std::is_integral_v<T>) {
      if constexpr (std::is_same_v<T, bool>) {
        val = boost::json::value(data ? "true" : "false");
      } else {
        val = boost::json::value(data);
      }
    } else if constexpr (std::is_object_v<T>) {
      boost::json::object obj;
      boost::pfr::for_each_field(data, [&](auto&& field, auto index) {
        using FieldType = std::decay_t<decltype(field)>;
        obj[boost::pfr::get_name<index, T>()] = DataSerializer<FieldType>::write(field);
      });
      val = obj;
    } else {
      LOG(ERROR) << "Unsupported type: " << typeid(T).name();
    }
    return val;
  }
  
  static boost::json::value operator()(const T& data) {
    return write(data);
  }
};

/**
 * @brief 快捷序列化函数
 * @tparam T 数据类型
 * @param data 要序列化的数据
 * @return boost::json::value JSON值
 */
template <typename T>
inline boost::json::value JsonSerialize(const T& data) {
  return DataSerializer<T>()(data);
}

/**
 * @brief 字符串枚举类型
 * 
 * 继承自std::string，提供枚举类型的字符串表示。
 */
class StringEnum : public std::string {
public:
  StringEnum() = default;
  explicit StringEnum(const std::string& str) : std::string(str) {}
  
  /**
   * @brief 转换为字符串
   * @return const std::string& 字符串引用
   */
  const std::string& ToString() {
    return *this;
  }

  /**
   * @brief 从字符串创建枚举
   * @tparam T 枚举类型
   * @param str 字符串
   * @return T 枚举值
   */
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
