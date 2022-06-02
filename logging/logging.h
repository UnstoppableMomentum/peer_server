/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////


#ifndef LOGGING_LOGGING_H_
#define LOGGING_LOGGING_H_

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#define SLNK_LOG_TRACE() BOOST_LOG_TRIVIAL(trace) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "
#define SLNK_LOG_DEBUG() BOOST_LOG_TRIVIAL(debug) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "
#define SLNK_LOG_INFO() BOOST_LOG_TRIVIAL(info)
#define SLNK_LOG_WARN() BOOST_LOG_TRIVIAL(warning) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "
#define SLNK_LOG_ERROR() BOOST_LOG_TRIVIAL(error) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "
#define SLNK_LOG_FATAL() BOOST_LOG_TRIVIAL(fatal) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "

#endif  // LOGGING_LOGGING_H_
