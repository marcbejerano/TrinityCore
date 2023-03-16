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

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class route
{
public:
    route(const http::verb& method,
          const std::string& path,
          const std::string& produces = "text/plain",
          const std::string& consumes = "application/json")
    : method_(method), path_(path), produces_(produces), consumes_(consumes) { }

    virtual http::response<http::dynamic_body> process(http::request<http::dynamic_body> request) = 0;
    
    std::vector<std::string> args(const http::request<http::dynamic_body> &request) const
    {
        std::vector<std::string> result;

        int q = path().find_first_of("?");
        std::string args(std::string(request.target()).substr(q));
        std::string r(path().substr(q + 1));
        boost::regex expr{r};
        boost::smatch what;

        if (boost::regex_search(args, what, expr))
        {
            for (auto a : what)
            {
                result.push_back(a.str());
            }
        }
        return result;
    }

    std::vector<std::string> vars(const http::request<http::dynamic_body> &request) const
    {
        std::vector<std::string> result;

        std::string args(std::string(request.target()));
        std::string r(path());
        boost::regex expr{r};
        boost::smatch what;

        if (boost::regex_search(args, what, expr))
        {
            for (auto a : what)
            {
                result.push_back(a.str());
            }
        }
        return result;
    }

    const http::verb method() const { return method_; }
    const std::string path() const { return path_; }
    const std::string produces() const { return produces_; }
    const std::string consumes() const { return consumes_; }

protected:
    void build_response(http::response<http::dynamic_body> &response, const http::request<http::dynamic_body> &request)
    {
        response.version(request.version());
        response.keep_alive(false);
        response.result(http::status::ok);
        response.set(http::field::server, "TrinityCore-API");
        response.set(http::field::content_type, produces());
    }

private:
    http::verb method_;
    std::string path_;
    std::string produces_;
    std::string consumes_;
};

