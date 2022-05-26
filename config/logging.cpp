/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "./logging.h"

namespace selenika {
namespace logging {
    void set_level(const std::string& level) {
        boost::log::trivial::severity_level severity_level
            = boost::log::trivial::trace;
        if ("debug" == level) {
            severity_level = boost::log::trivial::debug;
        } else if ("info" == level) {
            severity_level = boost::log::trivial::info;
        } else if ("warning" == level) {
            severity_level = boost::log::trivial::warning;
        } else if ("error" == level) {
            severity_level = boost::log::trivial::error;
        } else if ("fatal" == level) {
            severity_level = boost::log::trivial::fatal;
        }

        boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= severity_level);
    }
}  // namespace logging
}  // namespace selenika
