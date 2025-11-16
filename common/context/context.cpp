#include "context.h"


namespace common::context {

Context::Context(std::shared_ptr<const GlobalMetaData> global_meta_data,
  std::shared_ptr<CoroutineMetaData> coroutine_meta_data,
  std::shared_ptr<const Context> father)
  : global_meta_data_(global_meta_data), coroutine_meta_data_(coroutine_meta_data), father(father) {}

// 创建子协程上下文，生成层次化的协程名称
std::shared_ptr<Context> Context::fork(std::string child_name) {
  // 如果没有指定名称，使用计数器生成
  if (child_name.empty()) {
    child_name = fmt::format("{}", coroutine_meta_data_->child_count_++);
  } else {
    // 如果指定了名称，添加计数器后缀以保证唯一性
    child_name = fmt::format("{}-{}", child_name, coroutine_meta_data_->child_count_++);
  }

  // 创建子上下文，继承全局元数据，生成层次化名称（父名.子名）
  return std::make_shared<Context>(global_meta_data_, std::make_shared<CoroutineMetaData>(
    fmt::format("{}.{}", name(), child_name)  // 生成层次化的协程名称
  ), shared_from_this());
}


}
