#ifndef __STRAGE_TESTING_TESTING_H__
#define __STRAGE_TESTING_TESTING_H__

#include <string>
#include "base/stragy.h"

namespace stragy {
namespace testing {

class Testing : public base::Stragy {
public:
  Testing(engine::EnginePtr engine);
  ~Testing();

  asio::awaitable<void> run() override;

  asio::awaitable<void> recv_account(engine::AccountDataPtr account) override;
  asio::awaitable<void> recv_position(engine::PositionDataPtr position) override;
  asio::awaitable<void> recv_book(engine::BookPtr order) override;
};

}  // namespace testing
}  // namespace strage

#endif  // __STRAGE_TESTING_TESTING_H__
