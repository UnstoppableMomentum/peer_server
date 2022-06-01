/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

// Peer connection server, multi-threaded
// This implements a peer connection server using WebSocket.
// The `io_context` runs on any number of threads, specified at the command line.

#include <iostream>
#include <vector>

#include <boost/asio/signal_set.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/smart_ptr.hpp>

#include "config/logging.h"
#include "listener.hpp"
#include "logging/logging.h"
#include "options/cmd_line_options.h"
#include "shared_state.hpp"

int main(int argc, char *argv[]) {
    const CommandLineOptions clo(argc, argv);
    if (clo.HelpMode()) {
        clo.ShowHelp();
        return EXIT_SUCCESS;
    }

    LOG_INFO() << "Start Peer Server";

    if ("debug" == clo.GetDbgLevel()) {
        clo.ShowOptions();
    }

    try {
        LOG_DEBUG() << "Apply debug level: " << clo.GetDbgLevel();
        selenika::logging::set_level(clo.GetDbgLevel());

        LOG_DEBUG() << "Apply parameter address:" << clo.GetServer();
        std::string host = clo.GetServer();
        // net::ip::make_address(clo.GetServer());

        LOG_DEBUG() << "Listen port:" << clo.GetPort();
        LOG_DEBUG() << "Maximum number of connections:" << clo.GetMaxNumConnections();

        LOG_DEBUG() << "Apply parameter doc_root: '.'";
        auto const doc_root = ".";

        LOG_DEBUG() << "Apply parameter threads: 1";
        auto const threads = 1;

        LOG_DEBUG() << "Apply parameter PathSslSrt:" << clo.GetPathSslSrt();
        const std::string path_ssl_crt = clo.GetPathSslSrt();

        LOG_DEBUG() << "Apply parameter PathSslKey:" << clo.GetPathSslKey();
        const std::string path_ssl_key = clo.GetPathSslKey();

        LOG_DEBUG() << "Init SSL";
        // The SSL context is required, and holds certificates
        boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv13};

        if (!path_ssl_crt.empty() && !path_ssl_key.empty()) {
            ctx.use_certificate_chain_file(path_ssl_crt);
            ctx.use_private_key_file(path_ssl_key, boost::asio::ssl::context::pem);
            ctx.set_password_callback([](std::size_t, boost::asio::ssl::context_base::password_purpose) {
                // TODO(serg): check
                return "test";
            });
            ctx.set_options(
                boost::asio::ssl::context::default_workarounds |
                boost::asio::ssl::context::no_sslv2 |
                boost::asio::ssl::context::single_dh_use);
        }

        LOG_DEBUG() << "Create and launch a listening port";
        // The io_context is required for all I/O
        net::io_context ioc;

        // Create and launch a listening port
        boost::make_shared<listener>(
            host,
            clo.GetPort(),
            ioc,
            ctx,
            boost::make_shared<shared_state>(doc_root, clo.GetMaxNumConnections()))->run();

        LOG_DEBUG() << "Start waiting SIGINT, SIGTERM signals";

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](boost::system::error_code const &, int) {
                // Stop the io_context. This will cause run()
                // to return immediately, eventually destroying the
                // io_context and any remaining handlers in it.
                ioc.stop();
        });

        LOG_DEBUG() << "Set up threads";
        // Run the I/O service on the requested number of threads
        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for (auto i = threads - 1; i > 0; --i) {
            v.emplace_back([&ioc] { ioc.run(); });
        }

        LOG_DEBUG() << "Run the I/O service, seems OK";
        ioc.run();

        // (If we get here, it means we got a SIGINT or SIGTERM)
        LOG_DEBUG() << "got a SIGINT or SIGTERM";

        // Block until all the threads exit
        for (auto &t : v) {
            t.join();
        }
    } catch (const boost::system::system_error &ex) {
        LOG_ERROR() << "Peer Server failed (boost::system::system_error): " << ex.what() << std::endl;
    } catch (const std::exception &ex) {
        LOG_ERROR() << "Peer Server failed (std::exception): " << ex.what() << std::endl;
    } catch (...) {
        LOG_ERROR() << "Peer Server failed (unknown exception)";
    }

    return EXIT_SUCCESS;
}

