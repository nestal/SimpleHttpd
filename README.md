# SimpleHttp: a Simple Asynchronous HTTP Server Framework

SimpleHttp is a framework for writing web servers in C++. It allows the
developer to quickly implement web services by writing a few lines of
C++ code.

All network I/O operations in SimpleHttp is performed asynchronously
using Boost::Asio. Custom [request handlers](@ref http::RequestHandler) can
also perform I/O asynchronously (i.e. forwarding the request to
another web server).