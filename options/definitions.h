/*
* Copyright 2021 <QQQ>
*/

#ifndef OPTIONS_DEFINITIONS_H_
#define OPTIONS_DEFINITIONS_H_

const char kCmdLineOptHelp[] = "help";
const char kCmdLineOptConfig[] = "config";
const char kCmdLineOptServer[] = "server";
const char kCmdLineDbgLevel[] = "dbg_level";
const char kCmdLineOptSslSrt[] = "ssl_srt";
const char kCmdLineOptSslKey[] = "ssl_key";
const char kCmdLineOptPort[] = "port";

const char kDefaultPathConfig[] = "config.json";
const char kDefaultServer[] = "localhost";
const char kDefaultDbgLevel[] = "info";
const char kDefaultPathSslSrt[] = "server.crt";
const char kDefaultPathSslKey[] = "server.key";
const std::uint16_t kDefaultPort = 8080;

const char kHelpHeader[] = "Command line options:";
const char kHelpHelp[] = "Show help";
const char kHelpPathConfig[] = "\"path to configuration file\"";
const char kHelpServer[] = "\"peer server IP or domain\"";
const char kHelpDbgLevel[] = "\"debug level: 'debug', 'info', 'warning', 'error' or 'fatal' \"";
const char kHelpPathSslSrt[] = "\"path to SSH sert file\"";
const char kHelpPathSslKey[] = "\"path to SSH key file\"";
const char kHelpPort[] = "\"Port\"";

#endif  // OPTIONS_DEFINITIONS_H_
