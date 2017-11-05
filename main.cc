#include <iostream>

#include "httpd/Server.hh"
#include "httpd/Response.hh"
#include "httpd/RequestCallback.hh"

int main()
{
	std::cout << "Hello, World! " << std::endl;

	boost::asio::io_service ios;

	http::Server s{ios, "0.0.0.0", "8080"};
	s.SetDefaultHandler([](auto&& conn)
	{
		boost::asio::streambuf buf;
		std::ostream os{&buf};
		os << conn->URL() << " not found!";
		
		return http::Response{}.SetContent(buf);
	});
	
	class EchoContent : public http::ContentHandler
	{
	public:
		BrightFuture::future<http::Response> OnContent(const char *data, std::size_t size) override
		{
			m_buf.sputn(data, size);
			return {};
		}
		BrightFuture::future<http::Response> Finish() override
		{
			http::Response rep{HTTP_STATUS_OK};
			rep.SetContent(m_buf);
			
			BrightFuture::promise<http::Response> promise;
			promise.set_value(std::move(rep));
			return promise.get_future();
		}
		
	private:
		boost::asio::streambuf m_buf;
	};
	s.AddHandler("echo", [](auto&&){return std::make_unique<EchoContent>();});
	ios.run();
	return 0;
}
