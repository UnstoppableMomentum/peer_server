/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#ifndef OPTIONS_CMD_LINE_OPTIONS_H_
#define OPTIONS_CMD_LINE_OPTIONS_H_

#include <string>

class CommandLineOptions {
 public:
  CommandLineOptions(int ac, char *av[]);

  static void ShowHelp();
  void ShowOptions() const;
  const std::string& GetPathConfig() const { return m_sPathConfig; }
  const std::string& GetServer() const { return m_sServer; }
  const std::string& GetDbgLevel() const { return m_sDbgLevel; }
  const std::string& GetPathLog() const { return m_sPathLog; }
  const std::string& GetPathSslSrt() const { return m_sPathSslSrt; }
  const std::string& GetPathSslKey() const { return m_sPathSslKey; }
  std::uint16_t GetPort() const { return m_iPort; }
  std::uint32_t GetMaxNumConnections() const { return m_iMaxNumConnections; }
  bool HelpMode() const { return m_bHelp; }
  bool GetRunAsService() const { return m_runAsService; }

 private:
  void Init(int ac, char *av[]);

  std::uint16_t m_iPort;
  std::uint32_t m_iMaxNumConnections;
  std::string m_sPathConfig;
  std::string m_sServer;
  std::string m_sDbgLevel;
  std::string m_sPathLog;
  std::string m_sPathSslSrt;
  std::string m_sPathSslKey;
  bool m_runAsService;
  bool m_bHelp;
};

#endif  // OPTIONS_CMD_LINE_OPTIONS_H_
