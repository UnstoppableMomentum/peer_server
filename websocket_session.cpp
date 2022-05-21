/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#include <iostream>
#include <utility>

#include "logging/logging.h"

#include "websocket_session.hpp"

websocket_session::websocket_session(
    tcp::socket&& socket,
    ssl::context& ctx,
    boost::shared_ptr<shared_state> const& state)
    : ws_(std::move(socket), ctx)
    , state_(state) {
}

websocket_session::~websocket_session() {
    // Remove this session from the list of active sessions
    state_->leave(this);
}

void websocket_session::fail(beast::error_code ec, char const* what) {
    // Don't report these
    if ( ec == net::error::operation_aborted ||
        ec == websocket::error::closed)
        return;

    LOG_DEBUG() << " what: "<< what << ": " << ec.message() << "\n";
}

void websocket_session::on_accept(beast::error_code ec) {
    LOG_DEBUG() << " this:" << this << std::endl;
    // Handle the error, if any
    if (ec) {
        return fail(ec, "accept");
    }

    // Add this session to the list of active sessions
    state_->join(this);

    // Read a message
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &websocket_session::on_read,
            shared_from_this()));
}

void websocket_session::on_read(beast::error_code ec, std::size_t) {
    // Handle the error, if any
    if (ec) {
        return fail(ec, "read");
    }

    std::string response = state_->handle_message(
        this, beast::buffers_to_string(buffer_.data()));

    auto const ss = boost::make_shared<std::string const>(std::move(response));
    send(ss);

    // Send to all connections
    // state_->send(beast::buffers_to_string(buffer_.data()));

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Read another message
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &websocket_session::on_read,
            shared_from_this()));
}

void websocket_session::send(boost::shared_ptr<std::string const> const& ss) {
    // Post our work to the strand, this ensures
    // that the members of `this` will not be
    // accessed concurrently.

    LOG_DEBUG() << " this:" << this << std::endl;

    net::post(
        ws_.get_executor(),
        beast::bind_front_handler(
            &websocket_session::on_send,
            shared_from_this(),
            ss));
}

void websocket_session::on_send(
    boost::shared_ptr<std::string const> const& ss) {
    LOG_DEBUG() << " this:" << this << std::endl;
    // Always add to queue
    queue_.push_back(ss);

    // Are we already writing?
    if (queue_.size() > 1)
        return;

    // We are not currently writing, so send this immediately
    ws_.async_write(
        net::buffer(*queue_.front()),
        beast::bind_front_handler(
            &websocket_session::on_write,
            shared_from_this()));
}

void websocket_session::on_write(beast::error_code ec, std::size_t) {
    LOG_DEBUG() << " this:" << this << std::endl;
    // Handle the error, if any
    if (ec)
        return fail(ec, "write");

    // Remove the string from the queue
    queue_.erase(queue_.begin());

    // Send the next message if any
    if (!queue_.empty())
        ws_.async_write(
            net::buffer(*queue_.front()),
            beast::bind_front_handler(
                &websocket_session::on_write,
                shared_from_this()));
}

void websocket_session::on_handshake(beast::error_code ec) {
    if (ec) {
        LOG_DEBUG()  << "error code:" << ec << std::endl;
        return fail(ec, "handshake");
    }

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws_).expires_never();

    // Set suggested timeout settings for the websocket
    ws_.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::response_type& res) {
            res.set(http::field::server,
                std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-chat-multi");
        }));

    // Accept the websocket handshake
     ws_.async_accept(
        // req,
         beast::bind_front_handler(
             &websocket_session::on_accept,
             shared_from_this()));
}

    // Get on the correct executor
    void
    websocket_session::run() {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        net::dispatch(ws_.get_executor(),
            beast::bind_front_handler(
                &websocket_session::on_run,
                shared_from_this()));
    }

    // Start the asynchronous operation
    void
    websocket_session::on_run() {
        // Set the timeout.
        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

         // Perform the SSL handshake
        ws_.next_layer().async_handshake(
            ssl::stream_base::server,
            beast::bind_front_handler(
                &websocket_session::on_handshake,
                shared_from_this()));
    }
