#include <iostream>

#include "httpd/Server.hh"
#include "httpd/Response.hh"

int main()
{
	std::cout << "Hello, World! " << std::endl;

	boost::asio::io_service ios;

	http::Server s{ios, "0.0.0.0", "8080"};
	s.SetDefaultHandler([](auto&& request)
	{
		boost::asio::streambuf buf;
		std::ostream os{&buf};
		os << request.URL() << " not found!";
		
		return http::Response{HTTP_STATUS_NOT_FOUND}.SetContent(buf).SetContentType("text/plain");
	});
	
	class EchoContent : public http::ContentHandler
	{
	public:
		BrightFuture::future<http::Response> OnContent(http::Request&, const char *data, std::size_t size) override
		{
			m_buf.sputn(data, size);
			return {};
		}
		BrightFuture::future<http::Response> Finish(http::Request&) override
		{
			http::Response rep{HTTP_STATUS_OK};
			rep.SetContent(m_buf).SetContentType("text/plain");;
			
			return BrightFuture::make_ready_future(std::move(rep));
		}
		
	private:
		boost::asio::streambuf m_buf;
	};
	s.AddHandler("echo", [](auto&&){return std::make_unique<EchoContent>();});
	
	s.AddHandler("promise", [](auto&& request)
	{
		boost::asio::streambuf buf;
		std::ostream os{&buf};
		os << request.URL() << " is requested!";
		
		return BrightFuture::make_ready_future(std::move(http::Response{}.SetContent(buf).SetContentType("text/plain")));
	});
	s.AddHandler("bad", http::Response{HTTP_STATUS_BAD_REQUEST});
	
	ios.run();
	return 0;
}
