#ifndef __COMMON_OPTIONS_H__
#define __COMMON_OPTIONS_H__

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>
#include <errcode.h>
#include "utils.h"

namespace Config {

namespace po = boost::program_options;

class OptionsImpl {
public:
  OptionsImpl() = default;

  int operator()(int argc, char* argv[]) {
    init_desc();
    try {
      po::store(po::parse_command_line(argc, argv, m_desc), m_vm);
      po::notify(m_vm);
    } catch (const po::error& e) {
      std::cerr << "Error: " << e.what() << std::endl;
      std::cerr << m_desc << std::endl;
      return ErrCode_Invalid_Param;
    }
    return ErrCode_OK;
  }

  void init_desc() {
    m_desc.add_options()
        ("help,h", "Show help message")
        ("config,c", po::value<std::string>()->default_value("config.ini"), "Path to config file")
        ("log,l", po::value<std::string>(), "Path to log file");
  }

  std::string config_file() {
    return m_vm["config"].as<std::string>();
  }

private:
  po::options_description m_desc;
  po::variables_map m_vm;
};

using Options = Common::Singleton<OptionsImpl>;

}

#define AppOptions Config::Options::getInstance()

#endif