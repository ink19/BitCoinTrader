#ifndef __RECORD_H
#define __RECORD_H

#include <glog/logging.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/pfr.hpp>
#include <boost/endian.hpp>
#include <cstddef>

#include "type_traits.hpp"

namespace Record {

namespace asio = boost::asio;
using dec_float = boost::multiprecision::cpp_dec_float_100;

// 序列化库，2bit类型+数据
enum SerializeTypeEnum {
  SerializeTypeEnum_None = 0,
  SerializeTypeEnum_Vector = 1,  // 4bit长度 + item + item + ...
  SerializeTypeEnum_Struct = 2,  // 4bit长度 + item + item + item
  SerializeTypeEnum_Integer = 3,
  SerializeTypeEnum_String = 4, // 4bit长度 + 字符串
};

#pragma pack(push, 1)
struct RecordProtocol {
  uint16_t type;
  union {
    struct {
      uint32_t length;
    } vector;

    struct {
      uint32_t length;
    } struct_;
    
    uint64_t integer;

    struct {
      uint32_t length;
    } string;
  } data;
  uint8_t *ext_data;


  void serialize() {
    switch (type) {
      case SerializeTypeEnum_Vector:
        boost::endian::native_to_big_inplace(this->data.vector.length);
        break;
      case SerializeTypeEnum_Struct:
        boost::endian::native_to_big_inplace(this->data.struct_.length);
        break;
      case SerializeTypeEnum_Integer:
        boost::endian::native_to_big_inplace(this->data.integer);
        break;
      case SerializeTypeEnum_String:
        boost::endian::native_to_big_inplace(this->data.string.length);
        break;
      default:
        break;
    }
    boost::endian::native_to_big_inplace(this->type);
  }
};
#pragma pack(pop)

template <typename N>
uint32_t serialize(asio::mutable_buffer& s, N& o) {
  if constexpr (Common::is_shared_v<N>) {
    return serialize(s, *o.get());
  } else {
    uint32_t dlen = 0;
    boost::pfr::for_each_field(o, [&](auto&& field, auto index) {
      using FieldType = std::decay_t<decltype(field)>;
      RecordProtocol *buff = (RecordProtocol *)((uint8_t *)s.data() + dlen);
      if constexpr (Common::is_vector_v<FieldType>) {
        buff->type = SerializeTypeEnum_Vector;
        uint32_t length = 0;
        for (int i = 0; i < field.size(); ++i) {
          length += serialize(asio::buffer(
            (uint8_t *)(&(buff->ext_data)) + length, s.size() - offsetof(RecordProtocol, ext_data) - length), field[i]
          );
        }
        buff->data.vector.length = length;
        dlen += offsetof(RecordProtocol, ext_data) + length;
      } else if constexpr (std::is_same_v<FieldType, dec_float>) {
        auto val = field.str();
        buff->type = SerializeTypeEnum_String;
        buff->data.string.length = val.size();
        uint8_t *ext_data = (uint8_t *)(&(buff->ext_data));
        for (int i = 0; i < val.size(); ++i) {
          ext_data[i] = val[i];
        }
        dlen += offsetof(RecordProtocol, ext_data) + val.size();
      } else if constexpr (std::is_base_of_v<FieldType, std::string>) {
        buff->type = SerializeTypeEnum_String;
        buff->data.string.length = field.size();
        uint8_t *ext_data = (uint8_t *)(&(buff->ext_data));
        for (int i = 0; i < field.size(); ++i) {
          ext_data[i] = field[i];
        }
        dlen += offsetof(RecordProtocol, ext_data) + field.size();
      } else if constexpr (std::is_integral_v<FieldType>) {
        buff->type = SerializeTypeEnum_Integer;
        buff->data.integer = field;
        dlen += offsetof(RecordProtocol, ext_data);
      } else if constexpr (std::is_object_v<FieldType>) {
        buff->type = SerializeTypeEnum_Struct;
        uint32_t length = serialize(asio::buffer(
          (uint8_t *)(&(buff->ext_data)), s.size() - offsetof(RecordProtocol, ext_data)), field
        );
        dlen += offsetof(RecordProtocol, ext_data) + length;
      } else {
        LOG(ERROR) << "not support type";
      }
      buff->serialize();
    });
    return dlen;
  }
}

class Record {
 public:
  Record(const std::string& filename);

  asio::awaitable<int> write_buffer(const asio::const_buffer& buffer);

  template <typename T>
  asio::awaitable<int> write(std::shared_ptr<T> data) {
    uint8_t buffer[1024];
    asio::mutable_buffer s = asio::buffer(buffer, 1024);
    int len = serialize(s, data);
    if (len > 1024) {
      LOG(ERROR) << "serialize data error";
      co_return -1;
    }
    co_return co_await write_buffer(asio::buffer(buffer, len));
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
