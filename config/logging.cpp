/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "config/definitions.h"
#include "config/logging.h"

namespace selenika {
namespace logging {
    void set_level(const std::string& level) {
        boost::log::trivial::severity_level severity_level
            = boost::log::trivial::trace;
        if (kLogLevelDebug == level) {
            severity_level = boost::log::trivial::debug;
        } else if (kLogLevelInfo == level) {
            severity_level = boost::log::trivial::info;
        } else if (kLogLevelWarning == level) {
            severity_level = boost::log::trivial::warning;
        } else if (kLogLevelError == level) {
            severity_level = boost::log::trivial::error;
        } else if (kLogLevelFatal == level) {
            severity_level = boost::log::trivial::fatal;
        }

        boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= severity_level);
    }
}  // namespace logging
}  // namespace selenika
