#include "utils.h"

#include <ctime>
#include <string>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/hmac.h>

std::string Common::sha256_hash_base64(const std::string& input, const std::string& key) {
  std::string hash;
  CryptoPP::HMAC<CryptoPP::SHA256> hmac((const CryptoPP::byte*)key.data(), key.size());

  // 计算SHA-256哈希值
  CryptoPP::StringSource stringSrc(
      input, true, new CryptoPP::HashFilter(hmac, new CryptoPP::StringSink(hash)));

  // 将哈希值转换为字节数组
  std::string hash_str = hash;
  std::vector<CryptoPP::byte> hash_bytes(hash_str.begin(), hash_str.end());

  // Base64编码
  std::string encoded;
  CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(encoded),
                                  false  // 不添加换行符
  );
  encoder.Put(hash_bytes.data(), hash_bytes.size());
  encoder.MessageEnd();

  return encoded;
}

extern int64_t Common::get_current_time_s() {
  return std::time(nullptr);
}

extern std::string Common::time_format_iso(const int64_t& time) {
  std::time_t t = static_cast<std::time_t>(time);
  char buf[sizeof "2011-10-08T07:07:09.000Z"];
  strftime(buf, sizeof buf, "%FT%T.000Z", gmtime(&t));
  return std::string(buf);
}