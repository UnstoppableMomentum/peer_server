/*
* Copyright 2021 <QQQ>
*/

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
  const std::string& GetPathSslSrt() const { return m_sPathSslSrt; }
  const std::string& GetPathSslKey() const { return m_sPathSslKey; }
  std::uint16_t GetPort() const { return m_iPort; }
  bool HelpMode() const { return m_bHelp; }

 private:
  void Init(int ac, char *av[]);

  std::uint16_t m_iPort;
  std::string m_sPathConfig;
  std::string m_sServer;
  std::string m_sDbgLevel;
  std::string m_sPathSslSrt;
  std::string m_sPathSslKey;
  bool m_bHelp;
};

#endif  // OPTIONS_CMD_LINE_OPTIONS_H_