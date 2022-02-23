//
// Copyright (c) 2022 QAZ
//

#ifndef LISTENER_HPP_
#define LISTENER_HPP_

#include <memory>
#include <string>

#include "beast.hpp"
#include "net.hpp"

#include <boost/beast/ssl.hpp>
#include <boost/smart_ptr.hpp>

namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>

// Forward declaration
class shared_state;

// Accepts incoming connections and launches the sessions
class listener : public boost::enable_shared_from_this<listener> {
    net::io_context& ioc_;
    ssl::context& ctx_;
    tcp::acceptor acceptor_;
    boost::shared_ptr<shared_state> state_;

    void fail(beast::error_code ec, char const* what);
    void on_accept(beast::error_code ec, tcp::socket socket);
    void do_accept();

 public:
    listener(
        std::string host,
        std::uint16_t port,
        net::io_context& ioc,
        ssl::context& ctx,
    //    tcp::endpoint endpoint,
        boost::shared_ptr<shared_state> const& state);

    // Start accepting incoming connections
    void run();
};

#endif  // LISTENER_HPP_

