/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "options/cmd_line_options.h"
#include "options/definitions.h"

CommandLineOptions::CommandLineOptions(int ac, char *av[])
  : m_iPort(kDefaultPort)
  , m_iMaxNumConnections(kDefaultMaxNumConnections)
  , m_sPathConfig(kDefaultPathConfig)
  , m_sServer(kDefaultServer)
  , m_sDbgLevel(kDefaultDbgLevel)
  , m_sPathLog(kDefaultLog)
  , m_sPathSslSrt(kDefaultPathSslSrt)
  , m_sPathSslKey(kDefaultPathSslKey)
  , m_runAsService(kDefaultRunAsService)
  , m_bHelp(false) {
  Init(ac, av);
}

void CommandLineOptions::ShowHelp() {
  std::cout << kHelpHeader << std::endl
    << "--" << kCmdLineOptHelp << " " << kHelpHelp << std::endl
    << "--" << kCmdLineOptConfig << "=" << kHelpPathConfig << std::endl
    << "--" << kCmdLineLog << "=" << kHelpLog << std::endl
    << "--" << kCmdLineOptSslSrt << "=" << kHelpPathSslSrt << std::endl
    << "--" << kCmdLineOptSslKey << "=" << kHelpPathSslKey << std::endl
    << "--" << kCmdLineOptServer << "=" << kHelpServer << std::endl
    << "--" << kCmdLineOptPort << "=" << kHelpPort << std::endl
    << "--" << kCmdLineMaxNumConnections << "=" << kHelpMaxNumConnections << std::endl
    << "--" << kCmdLineDbgLevel << "=" << kHelpDbgLevel << std::endl
    << "--" << kCmdLineRunAsService << "=" << kHelpRunAsService << std::endl;
}

void CommandLineOptions::Init(int ac, char *av[]) {
  namespace po = boost::program_options;
  po::options_description desc(kHelpHeader);

  desc.add_options()
  (kCmdLineOptHelp, kHelpHelp)
  (kCmdLineOptConfig, po::value<std::string>(), kHelpPathConfig)
  (kCmdLineOptServer, po::value<std::string>(), kHelpServer)
  (kCmdLineDbgLevel, po::value<std::string>(), kHelpDbgLevel)
  (kCmdLineLog, po::value<std::string>(), kHelpLog)
  (kCmdLineOptSslSrt, po::value<std::string>(), kHelpPathSslSrt)
  (kCmdLineOptSslKey, po::value<std::string>(), kHelpPathSslKey)
  (kCmdLineOptPort, po::value<std::uint16_t>(), kHelpPort)
  (kCmdLineMaxNumConnections, po::value<std::uint32_t>(), kHelpMaxNumConnections)
  (kCmdLineRunAsService, po::value<bool>(), kHelpRunAsService);

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);

  try {
    if (vm.count(kCmdLineOptHelp))
      m_bHelp = true;

    if (vm.count(kCmdLineOptConfig))
      m_sPathConfig = vm[kCmdLineOptConfig].as<std::string>();

    if (vm.count(kCmdLineOptServer))
      m_sServer = vm[kCmdLineOptServer].as<std::string>();

    if (vm.count(kCmdLineDbgLevel))
      m_sDbgLevel = vm[kCmdLineDbgLevel].as<std::string>();

    if (vm.count(kCmdLineLog))
      m_sPathLog = vm[kCmdLineLog].as<std::string>();

    if (vm.count(kCmdLineOptSslSrt))
      m_sPathSslSrt = vm[kCmdLineOptSslSrt].as<std::string>();

    if (vm.count(kCmdLineOptSslKey))
      m_sPathSslKey = vm[kCmdLineOptSslKey].as<std::string>();

    if (vm.count(kCmdLineOptPort))
      m_iPort = vm[kCmdLineOptPort].as<std::uint16_t>();

    if (vm.count(kCmdLineMaxNumConnections))
      m_iMaxNumConnections = vm[kCmdLineMaxNumConnections].as<std::uint32_t>();

    if (vm.count(kCmdLineRunAsService))
      m_runAsService = vm[kCmdLineRunAsService].as<bool>();
  } catch (std::exception &ex) {
    std::cout << desc << std::endl;
    ShowHelp();
  }
}

void CommandLineOptions::ShowOptions() const {
  std::cout << "Command line options:" << std::endl
    << kCmdLineOptHelp << ": " << (HelpMode() ? "true" : "false") << std::endl
    << kCmdLineOptConfig << ": " << GetPathConfig() << std::endl
    << kCmdLineOptServer << ": " << GetServer() << std::endl
    << kCmdLineOptPort << ": " << GetPort() << std::endl
    << kCmdLineMaxNumConnections << ": " << GetMaxNumConnections() << std::endl
    << kCmdLineRunAsService << ": " << GetRunAsService() << std::endl
    << kCmdLineLog << ": " << GetPathLog() << std::endl
    << kCmdLineOptSslSrt << ": " << GetPathSslSrt() << std::endl
    << kCmdLineOptSslKey << ": " << GetPathSslKey() << std::endl
    << kCmdLineDbgLevel << ": " << GetDbgLevel() << std::endl;
}

