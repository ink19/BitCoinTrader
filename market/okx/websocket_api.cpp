#include "websocket_api.h"
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>

extern std::string Market::Okx::sha256_hash_base64(const std::string &input) {
    std::string hash;
    CryptoPP::SHA256 sha256;
    
    // 计算SHA-256哈希值
    CryptoPP::StringSource stringSrc(input, true,
        new CryptoPP::HashFilter(sha256,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(hash)
            )
        )
    );
    
    // 将哈希值转换为字节数组
    std::string hash_str = hash;
    std::vector<CryptoPP::byte> hash_bytes(hash_str.begin(), hash_str.end());
    
    // Base64编码
    std::string encoded;
    CryptoPP::Base64Encoder encoder(
        new CryptoPP::StringSink(encoded),
        false // 不添加换行符
    );
    encoder.Put(hash_bytes.data(), hash_bytes.size());
    encoder.MessageEnd();
    
    return encoded;
}
