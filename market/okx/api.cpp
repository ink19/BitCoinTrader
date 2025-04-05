#include "api.h"

#include "utils.h"

namespace Market {
namespace Okx {

std::string API::genSingature(const std::string& timestamp, const std::string& method,
                                                    const std::string& request_path) {
  // 拼接字符串
  std::string str_to_sign = timestamp + method + request_path;

  // 计算SHA-256哈希值
  std::string hash = Common::sha256_hash_base64(str_to_sign, m_secret_key);

  // 返回Base64编码的哈希值
  return hash;
}

API::API(const std::string& api_key, const std::string& secret_key, const std::string& passphrase)
    : m_api_key(api_key), m_secret_key(secret_key), m_passphrase(passphrase) {}

}  // namespace Okx
}  // namespace Market
