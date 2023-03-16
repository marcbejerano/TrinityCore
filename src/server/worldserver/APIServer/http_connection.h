#pragma once

//
// Copyright (c) 2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include "route.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
    http_connection(tcp::socket socket, const std::list<route*> &routes) : socket_(std::move(socket)), routes_(routes) { }

    // Initiate the asynchronous operations associated with the connection.
    void start()
    {
        read_request();
        check_deadline();
    }

private:
    // The socket for the currently connected client.
    tcp::socket socket_;

    const std::list<route*> &routes_;

    // The buffer for performing reads.
    beast::flat_buffer buffer_{8192};

    // The request message.
    http::request<http::dynamic_body> request_;

    // The response message.
    http::response<http::dynamic_body> response_;

    // The timer for putting a deadline on connection processing.
    net::steady_timer deadline_{
        socket_.get_executor(), std::chrono::seconds(60)};

    // Asynchronously receive a complete request message.
    void read_request()
    {
        auto self = shared_from_this();

        http::async_read(socket_, buffer_, request_, [self](beast::error_code ec, std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);
                if(!ec)
                    self->process_request();
            });
    }

    // Determine what needs to be done with the request message.
    void process_request()
    {
        for (auto route : routes_)
        {
            if (route->method() == request_.method())
            {
                boost::regex expr{route->path()};
                if (boost::regex_match(std::string(request_.target()), expr))
                {
                    response_ = route->process(request_);
                    write_response();
                    return;
                }
            }
        }
            
        response_.result(http::status::not_found);
        response_.set(http::field::content_type, "text/plain");
        beast::ostream(response_.body())
            << "Invalid request: "
            << std::string(request_.method_string())
            << " "
            << request_.target();
        write_response();
    }

    // Asynchronously transmit the response message.
    void write_response()
    {
        auto self = shared_from_this();

        response_.content_length(response_.body().size());

        http::async_write(socket_, response_, [self](beast::error_code ec, std::size_t)
            {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                self->deadline_.cancel();
            });
    }

    // Check whether we have spent enough time on this connection.
    void check_deadline()
    {
        auto self = shared_from_this();

        deadline_.async_wait([self](beast::error_code ec) {
            if(!ec) {
                // Close socket to cancel any outstanding operation.
                self->socket_.close(ec);
            }
        });
    }
};

