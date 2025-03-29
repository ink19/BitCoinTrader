#include "WebSocket.h"
#include <boost/url.hpp>
#include <boost/url/parse.hpp>

namespace Common {

WebSocket::WebSocket(const std::string& uri) {
    boost::urls::parse_uri(uri);
}
    

}
