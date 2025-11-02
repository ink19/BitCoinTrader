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

  asio::awaitable<void> run();
};

}  // namespace testing
}  // namespace strage

#endif  // __STRAGE_TESTING_TESTING_H__
