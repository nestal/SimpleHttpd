#include <iostream>

#include "httpd/Server.hh"
#include "httpd/Response.hh"

int main()
{
	std::cout << "Hello, World! " << std::endl;

	boost::asio::io_service ios;

	http::Server s{ios, "0.0.0.0", "8080"};
	s.SetDefaultHandler([](const http::ConnectionPtr& conn){
		conn->Response().SetStatus(http::Response::Status::ok);

		std::ostream os{&conn->Response().Content()};
		os << "hello";
		conn->Reply();
	});

	ios.run();
	return 0;
}
