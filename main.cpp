//
// Copyright (c) 2022 QAZ
//

//------------------------------------------------------------------------------
/*
    Peer connection server, multi-threaded

    This implements a peer connection server using WebSocket. 
    The `io_context` runs on any number of threads, specified at
    the command line.
*/
//------------------------------------------------------------------------------

#include <iostream>
#include <vector>

#include <boost/asio/signal_set.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/smart_ptr.hpp>

#include "config/logging.h"
#include "listener.hpp"
#include "logging/logging.h"
#include "shared_state.hpp"


namespace ssl = boost::asio::ssl;  // from <boost/asio/ssl.hpp>

int main(int argc, char *argv[]) {
    osv::logging::set_level("info");
    LOG_INFO() << "Peer Server";

    // Check command line arguments.
    if (argc != 7) {
        LOG_ERROR()
            << "Usage:  peer_server <address> <port> <doc_root> <threads>"
                " <path SSL cert> <path SSL key>\n"
            << "Example:\n"
            << "   ./peer_server 127.0.0.1 8080 . 5"
                " /home/qaz/work/config/server.crt"
                " /home/qaz/work/config/server.key\n";
        return EXIT_FAILURE;
    }
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<std::uint16_t>(std::atoi(argv[2]));
    auto const doc_root = argv[3];
    auto const threads = std::max<int>(1, std::atoi(argv[4]));
    const std::string path_ssl_crt = argv[5];
    const std::string path_ssl_key = argv[6];

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12};

    if (!path_ssl_crt.empty() && !path_ssl_key.empty()) {
        ctx.use_certificate_chain_file(path_ssl_crt);
        ctx.use_private_key_file(path_ssl_key, boost::asio::ssl::context::pem);
        //     if(verify_file.size()>0)
        // context.load_verify_file(verify_file);

        ctx.set_password_callback(
            [](std::size_t,
               boost::asio::ssl::context_base::password_purpose) {
                return "test";
            });

        ctx.set_options(
            boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::single_dh_use);
    }
    // The io_context is required for all I/O
    net::io_context ioc;

    // Create and launch a listening port
    boost::make_shared<listener>(
        ioc,
        ctx,
        tcp::endpoint{address, port},
        boost::make_shared<shared_state>(doc_root))
        ->run();

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](boost::system::error_code const &, int) {
            // Stop the io_context. This will cause run()
            // to return immediately, eventually destroying the
            // io_context and any remaining handlers in it.
            ioc.stop();
        });

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back([&ioc] {
            ioc.run();
        });

    ioc.run();

    // (If we get here, it means we got a SIGINT or SIGTERM)

    // Block until all the threads exit
    for (auto &t : v)
        t.join();

    return EXIT_SUCCESS;
}
