/*
* Copyright 2022 <QQQ>
*/

#ifndef OPTIONS_DEFINITIONS_H_
#define OPTIONS_DEFINITIONS_H_

constexpr char kCmdLineOptHelp[] = "help";
constexpr char kCmdLineOptConfig[] = "config";
constexpr char kCmdLineOptServer[] = "server";
constexpr char kCmdLineDbgLevel[] = "dbg_level";
constexpr char kCmdLineOptSslSrt[] = "ssl_srt";
constexpr char kCmdLineOptSslKey[] = "ssl_key";
constexpr char kCmdLineOptPort[] = "port";
constexpr char kCmdLineMaxNumConnections[] = "max_num_connections";

constexpr char kDefaultPathConfig[] = "config.json";
constexpr char kDefaultServer[] = "localhost";
constexpr char kDefaultDbgLevel[] = "info";
constexpr char kDefaultPathSslSrt[] = "server.crt";
constexpr char kDefaultPathSslKey[] = "server.key";
constexpr std::uint16_t kDefaultPort = 8080;
constexpr std::uint32_t kDefaultMaxNumConnections = 1024;

constexpr char kHelpHeader[] = "Command line options:";
constexpr char kHelpHelp[] = "Show help";
constexpr char kHelpPathConfig[] = "\"path to configuration file\"";
constexpr char kHelpServer[] = "\"peer server IP or domain\"";
constexpr char kHelpDbgLevel[] = "\"debug level: 'debug', 'info', 'warning', 'error' or 'fatal' \"";
constexpr char kHelpPathSslSrt[] = "\"path to SSH sert file\"";
constexpr char kHelpPathSslKey[] = "\"path to SSH key file\"";
constexpr char kHelpPort[] = "\"Port\"";
constexpr char kHelpMaxNumConnections[] = "\"max number of connections\"";

#endif  // OPTIONS_DEFINITIONS_H_
