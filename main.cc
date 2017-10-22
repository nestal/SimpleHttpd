#include <iostream>

#include "httpd/Server.hh"
#include "httpd/Response.hh"

int main()
{
	std::cout << "Hello, World! " << std::endl;

	boost::asio::io_service ios;

	http::Server s{ios, "0.0.0.0", "8080"};
	s.SetDefaultHandler([](auto&& conn)
	{
		http::Response rep;
		rep.SetStatus(http::ResponseStatus::ok);

		boost::asio::streambuf buf;
		std::ostream os{&buf};
		os << "hello promise!";
		
		std::vector<char> vec(buf.size());
		buffer_copy(boost::asio::buffer(vec), buf.data());
		rep.SetContent(std::move(vec));
		
		BrightFuture::promise<http::Response> promise;
		promise.set_value(std::move(rep));
		return promise.get_future();
	});

	ios.run();
	return 0;
}
