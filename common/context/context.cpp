#include "context.h"


namespace common::context {

Context::Context(std::shared_ptr<const GlobalMetaData> global_meta_data,
  std::shared_ptr<CoroutineMetaData> coroutine_meta_data,
  std::shared_ptr<const Context> father)
  : global_meta_data_(global_meta_data), coroutine_meta_data_(coroutine_meta_data), father(father) {}

std::shared_ptr<Context> Context::fork(std::string child_name) {
  if (child_name.empty()) {
    child_name = fmt::format("{}", coroutine_meta_data_->child_count_++);
  } else {
    child_name = fmt::format("{}-{}", child_name, coroutine_meta_data_->child_count_++);
  }

  return std::make_shared<Context>(global_meta_data_, std::make_shared<CoroutineMetaData>(
    fmt::format("{}.{}", name(), child_name)
  ), shared_from_this());
}


}
