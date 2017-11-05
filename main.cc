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
		return rep;
	});
	
	class SaveContent : public http::ContentHandler
	{
	public:
		BrightFuture::future<http::Response> OnContent(const char *data, std::size_t size) override
		{
			return {};
		}
		virtual BrightFuture::future<http::Response> Finish() override
		{
			boost::asio::streambuf buf;
			std::ostream os{&buf};
			
			os << "this is a ContentHandler!";
			
			http::Response rep{http::ResponseStatus::ok};
			rep.SetContent(buf);
			
			BrightFuture::promise<http::Response> promise;
			promise.set_value(std::move(rep));
			return promise.get_future();
		}
	};
	s.AddHandler("upload", [](auto&&){return std::make_unique<SaveContent>();});
	ios.run();
	return 0;
}
