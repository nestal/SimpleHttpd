#include <iostream>

#include "httpd/Server.hh"
#include "httpd/Response.hh"

int main()
{
	boost::asio::io_service ios;

	http::Server s{ios, "0.0.0.0", "8080"};
	s.SetDefaultHandler([](auto&& request)
	{
		auto sc = std::make_shared<http::StreamContent>();
		*sc << request.URL() << " not found!";
		
		return http::Response{http::status_NOT_FOUND}.SetContent(std::move(sc), "text/plain");
	});
	
	class EchoContent : public http::ContentHandler
	{
	public:
		boost::optional<http::Response> OnContent(http::Request&, const char *data, std::size_t size) override
		{
			m_buf->rdbuf()->sputn(data, size);
			return {};
		}
		boost::optional<http::Response> ReplyNow(http::Request&) override
		{
			return http::Response{http::status_OK}.SetContent(m_buf, "text/plain");
		}
		BrightFuture::future<http::Response> ReplyLater(http::Request&) override
		{
			return {};
		}
		
	private:
		std::shared_ptr<http::StreamContent> m_buf{std::make_shared<http::StreamContent>()};
	};
	s.AddHandler("echo", [](auto&& request)
	{
		request.HandleContent(std::make_unique<EchoContent>());
		return http::Response{};
	});
	
	s.AddHandler("promise", [](auto&& request)
	{
		auto sc = std::make_shared<http::StreamContent>();
		*sc << request.URL() << " is requested!";
		
		return http::Response{}.SetContent(std::move(sc), "text/plain");
	});
	s.AddHandler("bad", http::status_BAD_REQUEST);
	s.AddHandler("exception", [](auto&&)->http::Response{throw -1;});
	
	ios.run();
	return 0;
}
