/**
 * @file http_client.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef TCP_DRIVER__HTTP_CLIENT_HPP_
#define TCP_DRIVER__HTTP_CLIENT_HPP_

#include <array>
#include <string>
#include <vector>

#include <boost/beast.hpp>
//#include "asio.hpp" // Inconvenience occurs with transport_driver's own asio
//using drivers::common::IoContext;

using boost::asio::ip::tcp;

namespace beast = boost::beast;
namespace http = beast::http;

namespace drivers
{
  namespace tcp_driver
  {
    /**
     * @brief Functor for HTTP processing
     * 
     */
    using Functor = std::function<void(const std::string &)>;

    class HttpClient : public std::enable_shared_from_this<HttpClient>
    {
    public:
      /**
       * @brief HttpClient constructor
       * 
       * @param ctx io_context for this client
       * @param ip Device IP address
       * @param port Device HTTP port number
       * @param http_ver HTTP version
       */
      HttpClient(
          std::shared_ptr<boost::asio::io_context> ctx,
          const std::string &ip, uint16_t port, uint16_t http_ver);

      /**
       * @brief HttpClient constructor (with ver. 11)
       * 
       * @param ctx io_context for this client
       * @param ip Device IP address
       * @param port Device HTTP port number
       */
      HttpClient(
          std::shared_ptr<boost::asio::io_context> ctx,
//          boost::asio::io_context ctx,
          const std::string &ip, uint16_t port);

      /**
       * @brief HttpClient destructor
       * 
       */
      ~HttpClient();

      /**
       * @brief Disable copy constructor
       * 
       */
      HttpClient(const HttpClient &) = delete;

      /**
       * @brief Disable copy assignment operator
       * 
       * @return HttpClient& (disabled)
       */
      HttpClient &operator=(const HttpClient &) = delete;

      /**
       * @brief Get Device IP address
       * 
       * @return std::string Device IP address
       */
      std::string remote_host() const;

      /**
       * @brief Get Device HTTP port number
       * 
       * @return uint16_t Device HTTP port number
       */
      uint16_t remote_port() const;
      uint16_t http_version() const;

      /**
       * @brief Open the http connection with set parameters
       * 
       */
      void open();

      /**
       * @brief Close the http connection
       * 
       */
      void close();

      /**
       * @brief Check the socket open/close
       * 
       * @return true if the socket is open
       * @return false if the socket is closed
       */
      bool isOpen() const;

      /**
       * @brief Blocking Write Operation
       * 
       * @param target Target of HTTP request
       * @param method Method of HTTP request
       * @param body Body of HTTP request
       * @return std::size_t Length of response data
       */
      std::size_t write(std::string target, http::verb method, std::string body = "");

      /**
       * @brief Blocking Get Operation
       * 
       * @param target Target of HTTP request
       * @return std::size_t Length of response data
       */
      std::size_t get(std::string target);

      /**
       * @brief Blocking Post Operation
       * 
       * @param target Target of HTTP request
       * @param body Body of HTTP request
       * @return std::size_t Length of response data
       */
      std::size_t post(std::string target, std::string body);

      /**
       * @brief Get http response buffer
       * 
       * @return std::string Http response buffer
       */
      std::string read_result();

      /**
       * @brief Get http response data
       * 
       * @return std::string Http response
       */
      std::string read_response();

      /**
       * @brief NonBlocking Write Operation
       * 
       * @param func Callback after async_read
       * @param target Target of HTTP request
       * @param method Method of HTTP request
       * @param body Body of HTTP request
       */
      void asyncWrite(Functor func, std::string target, http::verb method, std::string body = "");

      /**
       * @brief NonBlocking Get Operation
       * 
       * @param func Callback after async_read
       * @param target Target of HTTP request
       */
      void asyncGet(Functor func, std::string target);

      /**
       * @brief NonBlocking Post Operation
       * 
       * @param func Callback after async_read
       * @param target Target of HTTP request
       * @param body Body of HTTP request
       */
      void asyncPost(Functor func, std::string target, std::string body);

      // put, delete are not implemented

    private:
      /**
       * @brief Callback for async_resolve
       * 
       * @param ec error_code
       * @param results Resolving results
       */
      void asyncOnResolve(
          beast::error_code ec,
          tcp::resolver::results_type results);
      
      /**
       * @brief Callback for async_connect
       * 
       * @param ec error_code
       */
      void asyncOnConnect(boost::system::error_code ec);

      /**
       * @brief Callback for async_write
       * 
       * @param ec error_code
       * @param bytes_transferred # of transferred data
       */
      void asyncOnWrite(
          boost::system::error_code ec,
          std::size_t bytes_transferred);

      /**
       * @brief Callback after async_read
       * 
       * @param ec error_code
       * @param bytes_transferred # of transferred data
       */
      void asyncOnRead(
          boost::system::error_code ec,
          std::size_t bytes_transferred);

    private:
      /**
       * @brief Device IP address
       * 
       */
      const std::string m_remote_host;

      /**
       * @brief Device HTTP port number
       * 
       */
      const uint16_t m_remote_port;

      /**
       * @brief HTTP version
       * 
       */
      const uint16_t m_http_ver;

      /**
       * @brief io_context for HttpClient
       * 
       */
      std::shared_ptr<boost::asio::io_context> m_ctx;

      /**
       * @brief TCP resolver
       * 
       */
      tcp::resolver m_resolver;

      /**
       * @brief TCP socket for HttpClient
       * 
       */
      std::shared_ptr<tcp::socket> m_socket;
      //  beast::tcp_stream m_stream;//not worked...

      // for async
      /**
       * @brief Http request
       * 
       */
      http::request<http::string_body> m_req;
      
      /**
       * @brief Http response
       * 
       */
      http::response<http::string_body> m_res;

      /**
       * @brief Message buffer
       * 
       */
      boost::beast::flat_buffer m_recv_buffer;
      
      /**
       * @brief Callback
       * 
       */
      Functor m_func;

      /**
       * @brief String from http response
       * 
       */
      std::string m_res_string;
      
      /**
       * @brief String from received data
       * 
       */
      std::string m_recv_string;



      // not working well
      bool stopped_ = false;
      // not working well
      boost::asio::steady_timer deadline_;

      /**
       * @brief milliseconds timeout on http connection (blocked and not working well)
       * 
       */
      int timeout_msec;

      /**
       * @brief Not working well...
       * https://www.boost.org/doc/libs/1_63_0/doc/html/boost_asio/example/cpp03/timeouts/async_tcp_client.cpp
       * https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/example/cpp11/timeouts/async_tcp_client.cpp
       */
      void check_deadline();

    };

  } // namespace tcp_driver
} // namespace drivers

#endif // TCP_DRIVER__HTTP_CLIENT_HPP_
