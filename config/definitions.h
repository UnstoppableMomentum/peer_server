/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#ifndef CONFIG_DEFINITIONS_H_
#define CONFIG_DEFINITIONS_H_

#include <string>

// Logging configuration.

namespace selenika {
namespace logging {
    constexpr char kLogLevelDebug[] = "debug";
    constexpr char kLogLevelInfo[] = "info";
    constexpr char kLogLevelWarning[] = "warning";
    constexpr char kLogLevelError[] = "error";
    constexpr char kLogLevelFatal[] = "fatal";
}  // namespace logging
}  // namespace selenika

#endif  // CONFIG_DEFINITIONS_H_
