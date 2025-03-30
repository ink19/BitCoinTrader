#ifndef __CONFIG_COMMON_CONFIG_H__
#define __CONFIG_COMMON_CONFIG_H__

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "utils.h"

namespace Config {

using ptree = boost::property_tree::ptree;

class ConfigTree {
public:
  ConfigTree(const std::string& prefix, std::shared_ptr<ptree> pt) : m_prefix(prefix), m_ptree(pt) {};

  template<typename T>
  T get(const std::string& key) {
    return m_ptree->get<T>(m_prefix + "." + key);
  }
private:
  std::string m_prefix;
  std::shared_ptr<ptree> m_ptree;
};

class ConfigOkx : public ConfigTree {
public:
  ConfigOkx(const std::string& prefix, std::shared_ptr<ptree> pt) : ConfigTree(prefix, pt) {
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
      m_okx_config = std::make_shared<ConfigOkx>("okx", m_ptree);
    }
    return m_okx_config;
  }
private:
  std::shared_ptr<ptree> m_ptree;
  std::shared_ptr<ConfigOkx> m_okx_config;
};

using Config = Common::Singleton<ConfigImpl>;

};

#define AppConfig ::Config::Config::getInstance()

#endif