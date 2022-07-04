/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#ifndef CONFIG_LOGGING_H_
#define CONFIG_LOGGING_H_

#include <string>

// Logging configuration.

namespace selenika {
namespace logging {
    void set_level(const std::string& level);
}  // namespace logging
}  // namespace selenika

#endif  // CONFIG_LOGGING_H_
