#ifndef __COMMON_CONTEXT_CONTEXT_H__
#define __COMMON_CONTEXT_CONTEXT_H__

#include <memory>
#include <fmt/format.h>
#include <atomic>
#include <functional>
#include <set>
#include "utils/utils.h"

namespace common::context {


class CoroutineManager : public std::enable_shared_from_this<CoroutineManager> {
public:
  void add_running_coroutine(const std::string& coroutine_name) {
    running_coroutines_.insert(coroutine_name);
  }

  void remove_running_coroutine(const std::string& coroutine_name) {
    running_coroutines_.erase(coroutine_name);
  }
private:
  std::set<std::string> running_coroutines_;
};

class GlobalMetaData : public std::enable_shared_from_this<GlobalMetaData> {
public:
  GlobalMetaData(std::shared_ptr<CoroutineManager> manager) : manager_(manager) {}

  friend class Context;
private:
  std::shared_ptr<CoroutineManager> manager_;
};

class CoroutineMetaData : public std::enable_shared_from_this<CoroutineMetaData> {
public:
  CoroutineMetaData(const std::string& name) : coroutine_name_(name) {}
  
  std::atomic_int32_t child_count_{0};
  friend class Context;
private:
  const std::string coroutine_name_;
};

class Context : public std::enable_shared_from_this<Context> {
public:
  Context(std::shared_ptr<const GlobalMetaData> global_meta_data,
    std::shared_ptr<CoroutineMetaData> coroutine_meta_data,
    std::shared_ptr<const Context> father);

  std::shared_ptr<Context> fork(std::string child_name = "");

  std::shared_ptr<CoroutineManager> coroutine_manager() const {
    return global_meta_data_->manager_;
  }

  const std::string name() const {
    return coroutine_meta_data_->coroutine_name_;
  }
private:
  std::shared_ptr<const GlobalMetaData> global_meta_data_;
  std::shared_ptr<CoroutineMetaData> coroutine_meta_data_;
  std::shared_ptr<const Context> father;
};

using ContextPtr = std::shared_ptr<Context>;

template <typename ReturnType, typename ...Args>
void co_spawn_deteched(asio::any_io_executor& exec, ContextPtr ctx, const std::string& child_name, std::function<asio::awaitable<ReturnType>(ContextPtr, Args...)> func, Args &&...args) {
  auto child_ctx = ctx->fork(child_name);

  child_ctx->coroutine_manager()->add_running_coroutine(child_ctx->name());

  asio::co_spawn(exec, [child_ctx, func, args...]() mutable -> asio::awaitable<ReturnType> {
    auto& res = co_await func(child_ctx, std::forward<Args>(args)...);
    child_ctx->coroutine_manager()->remove_running_coroutine(child_ctx->name());
    co_return res;
  }, asio::detached);
}

template <typename ReturnType, typename ...Args>
void co_spawn_deteched(asio::any_io_executor& exec, ContextPtr ctx, std::function<asio::awaitable<ReturnType>(ContextPtr, Args...)> func, Args &&...args) {
  co_spawn_deteched(exec, ctx, "", func, std::forward<Args>(args)...);
}



}

#endif
