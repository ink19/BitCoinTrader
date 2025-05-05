#ifndef __CONFIG_COMMON_CONFIG_H__
#define __CONFIG_COMMON_CONFIG_H__

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <memory>
#include <string>

#include "utils.h"

namespace Config {

using ptree = boost::property_tree::ptree;
using dec_float = boost::multiprecision::cpp_dec_float_100;

class ConfigTree {
 public:
  ConfigTree(const std::string& prefix, std::shared_ptr<ptree> pt) : m_prefix(prefix), m_ptree(pt){};

  template <typename T>
  T get(const std::string& key) {
    return m_ptree->get<T>(m_prefix + "." + key);
  }

 private:
  std::string m_prefix;
  std::shared_ptr<ptree> m_ptree;
};

class ConfigOkx : public ConfigTree {
 public:
  ConfigOkx(std::shared_ptr<ptree> pt) : ConfigTree({"okx"}, pt) {
    m_api_key = this->get<std::string>("api_key");
    m_secret_key = this->get<std::string>("secret_key");
    m_passphrase = this->get<std::string>("passphrase");
  };

  std::string api_key() const { return m_api_key; }
  std::string secret_key() const { return m_secret_key; }
  std::string passphrase() const { return m_passphrase; }

 private:
  std::string m_api_key;
  std::string m_secret_key;
  std::string m_passphrase;
};

class ConfigWeWork : public ConfigTree {
 public:
  ConfigWeWork(std::shared_ptr<ptree> pt) : ConfigTree({"wework"}, pt) { m_key = this->get<std::string>("key"); };

  std::string key() const { return m_key; }

 private:
  std::string m_key;
};

class ConfigCommon : public ConfigTree {
 public:
  ConfigCommon(std::shared_ptr<ptree> pt) : ConfigTree("common", pt) {
    m_timeout_ms = this->get<uint32_t>("timeout_ms");
  };

  uint32_t timeout_ms() const { return m_timeout_ms; }

 private:
  uint32_t m_timeout_ms;
};

class ConfigCompare : public ConfigTree {
 public:
  ConfigCompare(std::shared_ptr<ptree> pt) : ConfigTree("compare", pt) {
    m_min = dec_float(this->get<std::string>("min_diff"));
    if (m_min == 0) {
      m_min = dec_float("0.5");
    }
    
    m_report_time_s = this->get<int64_t>("report_time");
  };

  dec_float min_diff() const { return m_min; }
  int64_t report_time_s() const { return m_report_time_s; }
 private:
  dec_float m_min;
  int64_t m_report_time_s = 0;
};

class ConfigImpl {
 public:
  ConfigImpl(const std::string& config_file = "") {
    if (!config_file.empty()) {
      init(config_file);
    }
  };

  void init(const std::string& config_file = "config.ini") {
    m_ptree = std::make_shared<ptree>();
    boost::property_tree::read_ini(config_file, *m_ptree);
  }

  std::shared_ptr<ConfigOkx> okx() {
    if (!m_okx_config) {
      m_okx_config = std::make_shared<ConfigOkx>(m_ptree);
    }
    return m_okx_config;
  }

  std::shared_ptr<ConfigCommon> common() {
    if (!m_common_config) {
      m_common_config = std::make_shared<ConfigCommon>(m_ptree);
    }
    return m_common_config;
  }

  std::shared_ptr<ConfigWeWork> wework() {
    if (!m_wework_config) {
      m_wework_config = std::make_shared<ConfigWeWork>(m_ptree);
    }
    return m_wework_config;
  }

  std::shared_ptr<ConfigCompare> compare() {
    if (!m_compare_config) {
      m_compare_config = std::make_shared<ConfigCompare>(m_ptree);
    }
    return m_compare_config;
  }

 private:
  std::shared_ptr<ptree> m_ptree;
  std::shared_ptr<ConfigOkx> m_okx_config;
  std::shared_ptr<ConfigCommon> m_common_config;
  std::shared_ptr<ConfigWeWork> m_wework_config;
  std::shared_ptr<ConfigCompare> m_compare_config;
};

using Config = Common::Singleton<ConfigImpl>;

};  // namespace Config

#define AppConfig ::Config::Config::getInstance()

#endif
