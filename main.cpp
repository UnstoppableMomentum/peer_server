/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

// Peer connection server, multi-threaded
// This implements a peer connection server using WebSocket.
// The `io_context` runs on any number of threads, specified at the command line.

#include <syslog.h>

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

    SLNK_LOG_INFO() << "Start Peer Server";

    if ("debug" == clo.GetDbgLevel()) {
        clo.ShowOptions();
    }

    try {
        SLNK_LOG_DEBUG() << "Apply debug level: " << clo.GetDbgLevel();
        selenika::logging::set_level(clo.GetDbgLevel());

        SLNK_LOG_DEBUG() << "Apply parameter address:" << clo.GetServer();
        std::string host = clo.GetServer();

        SLNK_LOG_DEBUG() << "Listen port:" << clo.GetPort();
        SLNK_LOG_DEBUG() << "Maximum number of connections:" << clo.GetMaxNumConnections();

        SLNK_LOG_DEBUG() << "Apply parameter doc_root: '.'";
        auto const doc_root = ".";

        SLNK_LOG_DEBUG() << "Apply parameter threads: 1";
        auto const threadsNum = 1;

        SLNK_LOG_DEBUG() << "Apply parameter PathSslSrt:" << clo.GetPathSslSrt();
        const std::string path_ssl_crt = clo.GetPathSslSrt();

        SLNK_LOG_DEBUG() << "Apply parameter PathSslKey:" << clo.GetPathSslKey();
        const std::string path_ssl_key = clo.GetPathSslKey();

        SLNK_LOG_DEBUG() << "Init SSL";
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

        SLNK_LOG_DEBUG() << "Create and launch a listening port";
        // The io_context is required for all I/O
        net::io_context ioc;

        // Create and launch a listening port
        boost::make_shared<listener>(
            host,
            clo.GetPort(),
            ioc,
            ctx,
            boost::make_shared<shared_state>(doc_root, clo.GetMaxNumConnections()))->run();

        SLNK_LOG_DEBUG() << "Start waiting SIGINT, SIGTERM signals";

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](boost::system::error_code const &, int) {
            // Stop the io_context. This will cause run()
            // to return immediately, eventually destroying the
            // io_context and any remaining handlers in it.
            ioc.stop();
        });

        SLNK_LOG_DEBUG() << "Set up threads";
        // Run the I/O service on the requested number of threads
        std::vector<std::thread> threads;
        threads.reserve(threadsNum - 1);
        for (auto i = threadsNum - 1; i > 0; --i) {
          threads.emplace_back([&ioc] { ioc.run(); });
        }

        SLNK_LOG_DEBUG() << "Initialization of the I/O service is OK";

        if (clo.GetRunAsService()) {
          SLNK_LOG_DEBUG() << "Becoming a daemon...";

          // Register signal handlers so that the daemon may be shut down. You may
          // also want to register for other signals, such as SIGHUP to trigger a
          // re-read of a configuration file.
             // boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
          signals.async_wait(
            [&](boost::system::error_code /*ec*/, int /*signo*/) {
              ioc.stop();
          });

          SLNK_LOG_DEBUG() << "First fork...";

          // Inform the io_context that we are about to become a daemon. The
          // io_context cleans up any internal resources, such as threads, that may
          // interfere with forking.
          ioc.notify_fork(boost::asio::io_context::fork_prepare);

          // Fork the process and have the parent exit. If the process was started
          // from a shell, this returns control to the user. Forking a new process is
          // also a prerequisite for the subsequent call to setsid().
          if (pid_t pid = fork()) {
            if (pid > 0) {
              // We're in the parent process and need to exit.
              //
              // When the exit() function is used, the program terminates without
              // invoking local variables' destructors. Only global variables are
              // destroyed. As the io_context object is a local variable, this means
              // we do not have to call:
              //
              //   io_context.notify_fork(boost::asio::io_context::fork_parent);
              //
              // However, this line should be added before each call to exit() if
              // using a global io_context object. An additional call:
              //
              //   io_context.notify_fork(boost::asio::io_context::fork_prepare);
              //
              // should also precede the second fork().
              exit(0);
            } else {
              syslog(LOG_ERR | LOG_USER, "First fork failed: %m");
              SLNK_LOG_ERROR() << "First fork failed";
              return EXIT_FAILURE;
            }
          }

          // Make the process a new session leader. This detaches it from the
          // terminal.
          setsid();

          // A process inherits its working directory from its parent. This could be
          // on a mounted filesystem, which means that the running daemon would
          // prevent this filesystem from being unmounted. Changing to the root
          // directory avoids this problem.
          chdir("/");

          // The file mode creation mask is also inherited from the parent process.
          // We don't want to restrict the permissions on files created by the
          // daemon, so the mask is cleared.
          umask(0);

          SLNK_LOG_DEBUG() << "Second fork...";
          // A second fork ensures the process cannot acquire a controlling terminal.
          if (pid_t pid = fork()) {
            if (pid > 0) {
              exit(0);
            } else {
              syslog(LOG_ERR | LOG_USER, "Second fork failed: %m");
              SLNK_LOG_ERROR() << "Second fork failed";
              return EXIT_FAILURE;
            }
          }

          // Close the standard streams. This decouples the daemon from the terminal
          // that started it.
          close(0);
          close(1);
          close(2);

          // We don't want the daemon to have any standard input.

          SLNK_LOG_DEBUG() << "Detauch standard input";
          if (open("/dev/null", O_RDONLY) < 0) {
            syslog(LOG_ERR | LOG_USER, "Unable to open /dev/null: %m");
            SLNK_LOG_ERROR() << "Unable to open /dev/null";
            return EXIT_FAILURE;
          }

          // Send standard output to a log file.
          SLNK_LOG_DEBUG() << "Send standard output to a log file";
          const char* output = clo.GetPathLog().c_str();
          const int flags = O_WRONLY | O_CREAT | O_APPEND;
          const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
          if (open(output, flags, mode) < 0) {
            syslog(LOG_ERR | LOG_USER, "Unable to open output file %s: %m", output);
            return EXIT_FAILURE;
          }

          // Also send standard error to the same log file.
          if (dup(1) < 0) {
            syslog(LOG_ERR | LOG_USER, "Unable to dup output descriptor: %m");
            SLNK_LOG_ERROR() << "Unable to dup output descriptor";
            return EXIT_FAILURE;
          }

          // Inform the io_context that we have finished becoming a daemon. The
          // io_context uses this opportunity to create any internal file descriptors
          // that need to be private to the new process.
          ioc.notify_fork(boost::asio::io_context::fork_child);

          // The io_context can now be used normally.
          syslog(LOG_INFO | LOG_USER, "Peer server daemon is ready");
        }

        SLNK_LOG_DEBUG() << "run server";
        ioc.run();

        // (If we get here, it means we got a SIGINT or SIGTERM)
        SLNK_LOG_DEBUG() << "got a SIGINT or SIGTERM";

        // Block until all the threads exit
        for (auto &t : threads) {
            t.join();
        }
    } catch (const boost::system::system_error &ex) {
        SLNK_LOG_ERROR() << "Peer Server failed (boost::system::system_error): " << ex.what() << std::endl;
    } catch (const std::exception &ex) {
        SLNK_LOG_ERROR() << "Peer Server failed (std::exception): " << ex.what() << std::endl;
    } catch (...) {
        SLNK_LOG_ERROR() << "Peer Server failed (unknown exception)";
    }

    return EXIT_SUCCESS;
}

