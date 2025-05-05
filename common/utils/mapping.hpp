#ifndef __COMMON_UTILS_MAPPING_HPP__
#define __COMMON_UTILS_MAPPING_HPP__

#include <string>
#include <map>
#include <tuple>
#include <boost/pfr.hpp>
#include <array>

namespace Common {

namespace pfr = boost::pfr;

#define MAPPER_TYPE(SRC_TYPE, DST_TYPE) \
  inline std::shared_ptr<DST_TYPE> mapping(std::shared_ptr<SRC_TYPE> src_data) { \
    auto dst_data = std::make_shared<DST_TYPE>();

#define MAPPER_TYPE_END(SRC_TYPE, DST_TYPE) \
    return dst_data; \
  }

#define MAPPER_TYPE_ITEM_WITH_FUNC(SRC_NAME, DST_NAME, MAP_FUNC) \
  dst_data->DST_NAME = MAP_FUNC(src_data->SRC_NAME);

#define MAPPER_TYPE_ITEM(SRC_NAME, DST_NAME) \
  dst_data->DST_NAME = src_data->SRC_NAME;


}  // namespace Common

#endif
