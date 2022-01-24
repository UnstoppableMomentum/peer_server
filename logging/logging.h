/*
* Copyright 2021 <QQQ>
*/

#ifndef LOGGING_LOGGING_H_
#define LOGGING_LOGGING_H_

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#define LOG_TRACE() BOOST_LOG_TRIVIAL(trace) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "
#define LOG_DEBUG() BOOST_LOG_TRIVIAL(debug) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "
#define LOG_INFO() BOOST_LOG_TRIVIAL(info)
#define LOG_WARN() BOOST_LOG_TRIVIAL(warning) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "
#define LOG_ERROR() BOOST_LOG_TRIVIAL(error) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "
#define LOG_FATAL() BOOST_LOG_TRIVIAL(fatal) << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] "

#endif  // LOGGING_LOGGING_H_
