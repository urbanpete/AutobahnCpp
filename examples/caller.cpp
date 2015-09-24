///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2014 Tavendo GmbH
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////
#include "parameters.hpp"

#include <autobahn/autobahn.hpp>
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>

int main(int argc, char** argv)
{
    std::cerr << "Boost: " << BOOST_VERSION << std::endl;

    try {
        auto parameters = get_parameters(argc, argv);

        boost::asio::io_service io;
        auto transport = std::make_shared<autobahn::wamp_tcp_transport>(
                io, parameters->rawsocket_endpoint());

        bool debug = parameters->debug();
        auto session = std::make_shared<autobahn::wamp_session>(
                io, transport, transport, debug);

        // Make sure the continuation futures we use do not run out of scope prematurely.
        // Since we are only using one thread here this can cause the io service to block
        // as a future generated by a continuation will block waiting for its promise to be
        // fulfilled when it goes out of scope. This would prevent the session from receiving
        // responses from the router.
        boost::future<void> start_future;
        boost::future<void> join_future;
        boost::future<void> call_future;
        boost::future<void> leave_future;
        boost::future<void> stop_future;

        transport->async_connect([&](boost::system::error_code ec) {
                if (!ec) {
                    std::cerr << "connected to server" << std::endl;

                    start_future = session->start().then([&](boost::future<bool> started) {
                        if (started.get()) {
                            join_future = session->join(parameters->realm()).then([&](boost::future<uint64_t> s) {
                                std::cerr << "joined realm: " << s.get() << std::endl;

                                autobahn::wamp_call_options call_options;
                                call_options.set_timeout(std::chrono::seconds(10));

                                std::tuple<uint64_t, uint64_t> arguments(23, 777);
                                call_future = session->call("com.examples.calculator.add", arguments, call_options).then(
                                [&](boost::future<autobahn::wamp_call_result> result) {
                                    try {
                                        uint64_t sum = result.get().argument<uint64_t>(0);
                                        std::cerr << "call result: " << sum << std::endl;
                                    } catch (const std::exception& e) {
                                        std::cerr << "call failed: " << e.what() << std::endl;
                                        io.stop();
                                    }

                                    leave_future = session->leave().then([&](boost::future<std::string> reason) {
                                        std::cerr << "left session (" << reason.get() << ")" << std::endl;
                                        stop_future = session->stop().then([&](boost::future<void> stopped) {
                                            std::cerr << "stopped session" << std::endl;
                                            io.stop();
                                        });
                                    });
                                });
                            });
                        } else {
                            std::cerr << "failed to start session" << std::endl;
                            io.stop();
                        }
                    });
                } else {
                    std::cerr << "connect failed: " << ec.message() << std::endl;
                    io.stop();
                }
            }
        );

        std::cerr << "starting io service" << std::endl;
        io.run();
        std::cerr << "stopped io service" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
