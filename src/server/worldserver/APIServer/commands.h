#pragma once

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

extern std::list<route*> command_routes;
extern void init_routes();

class getHello : public route
{
public:
    getHello() : route(http::verb::get, "/hello", "text/html") { }

    virtual http::response<http::dynamic_body> process(http::request<http::dynamic_body> request)
    {
        http::response<http::dynamic_body> response_;
        build_response(response_, request);

        beast::ostream(response_.body())
            << "<html>\n"
            <<  "<head><title>TrinityCore REST API</title></head>\n"
            <<  "<body>\n"
            <<  "<h1>Hello, world!</h1>\n"
            <<  "</body>\n"
            <<  "</html>\n";

        return response_;
    }
};

class getHelloParams : public route
{
public:
    getHelloParams() : route(http::verb::get, "/hello\\?org=(\\d+)", "text/html") { }

    virtual http::response<http::dynamic_body> process(http::request<http::dynamic_body> request)
    {
        std::vector<std::string> params(args(request));
        http::response<http::dynamic_body> response_;
        build_response(response_, request);

        int org = std::atoi(params[1].c_str());

        beast::ostream(response_.body())
            << "<html>\n"
            <<  "<head><title>TrinityCore REST API</title></head>\n"
            <<  "<body>\n"
            <<  "<h1>Hello, world!</h1>\n"
            <<  "org = " << org << "\n"
            <<  "</body>\n"
            <<  "</html>\n";

        return response_;
    }
};

class getHelloPathVariables : public route
{
public:
    getHelloPathVariables() : route(http::verb::get, "/hello/(\\d+)", "text/html") { }

    virtual http::response<http::dynamic_body> process(http::request<http::dynamic_body> request)
    {
        std::vector<std::string> params(vars(request));
        http::response<http::dynamic_body> response_;
        build_response(response_, request);

        int org = std::atoi(params[1].c_str());

        beast::ostream(response_.body())
            << "<html>\n"
            <<  "<head><title>TrinityCore REST API</title></head>\n"
            <<  "<body>\n"
            <<  "<h1>Hello, world!</h1>\n"
            <<  "org = " << org << "\n"
            <<  "</body>\n"
            <<  "</html>\n";

        return response_;
    }
};

