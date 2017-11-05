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
		http::Response rep{http::ResponseStatus::ok};

		boost::asio::streambuf buf;
		std::ostream os{&buf};
		os << "hello promise!";
		
		rep.SetContent(buf);
		
/*		BrightFuture::promise<http::Response> promise;
		promise.set_value(std::move(rep));
		return promise.get_future();*/
		return rep;
	});
/*	s.AddHandler("upload", [](auto&& conn)
	{
		class SaveContent : public http::ContentHandler
		{
		public:
			void OnContent(const char *data, std::size_t size) ;
			virtual void Finish() = 0;
	
			virtual BrightFuture::future<http::Response> Response() = 0;
		private:
		boost::asio::streambuf buf;
		std::ostream os{&buf};
		};
	});*/

	ios.run();
	return 0;
}
