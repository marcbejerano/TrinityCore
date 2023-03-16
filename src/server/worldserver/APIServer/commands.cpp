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
#include "http_connection.h"
#include "commands.h"

#include "Chat.h"
#include "ChatCommand.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

std::list<route*> command_routes;
getHello route_getHello;
getHelloParams route_getHelloParams;
getHelloPathVariables route_getHelloPathVariables;

void init_routes()
{
    if (routes.empty())
    {
        routes.push_back(&route_getHello);
        routes.push_back(&route_getHelloParams);
        routes.push_back(&route_getHelloPathVariables);
    }
}

