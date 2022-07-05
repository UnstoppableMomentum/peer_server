/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#include <iostream>
#include <utility>

#include "logging/logging.h"
#include "listener.hpp"
#include "websocket_session.hpp"

listener::listener(
    std::string host,
    std::uint16_t port,
    net::io_context &ioc,
    ssl::context &ctx,
    boost::shared_ptr<shared_state> const &state)
    : ioc_(ioc), ctx_(ctx), acceptor_(ioc), state_(state) {
    beast::error_code ec;

    tcp::resolver resolver{ioc};

    boost::system::error_code error;

    std::stringstream ss;
    ss << port;

    tcp::resolver::results_type results = resolver.resolve(host, ss.str(), error);

    for (tcp::endpoint const& endpoint : results) {
        SLNK_LOG_INFO() << endpoint << " try to utilize ...";

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            fail(ec, "open");
            continue;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) {
            fail(ec, "set_option");
            continue;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec) {
            fail(ec, "bind");
            continue;
        }

        // Start listening for connections
        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if (ec) {
            fail(ec, "listen");
            continue;
        }
        SLNK_LOG_INFO() << endpoint << " start listening";
    }
}

void listener::run() {
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &listener::on_accept,
            shared_from_this()));
}

// Report a failure
void listener::fail(beast::error_code ec, char const *what) {
    // Don't report on canceled operations
    if (ec == net::error::operation_aborted)
        return;
     SLNK_LOG_ERROR() << what << ": " << ec.message();
}

// // Handle a connection
void listener::on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec)
        return fail(ec, "accept");
    else
        // Launch a new session for this connection
        boost::make_shared<websocket_session>(
            std::move(socket),
            ctx_,
            state_)
            ->run();
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &listener::on_accept,
            shared_from_this()));
}

void listener::do_accept() {
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &listener::on_accept,
            shared_from_this()));
}
