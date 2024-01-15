#ifndef TCP_CLIENT
#define TCP_CLIENT

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <functional>
#include <mutex>
#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>
#include <csignal>
#include <iostream>

using boost::asio::ip::tcp;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
     using pointer = boost::shared_ptr<tcp_connection>;

     static pointer create(boost::asio::io_context &io_context)
     {
          return pointer(new tcp_connection(io_context));
     }

     tcp::socket &socket()
     {
          return socket_;
     }

     void startWriting(const std::string &username_)
     {
          std::string msg;
          std::getline(std::cin >> std::ws, msg);
          std::string final = username_ + ": " + msg + "\n";

          boost::asio::async_write(socket_, boost::asio::buffer(final),
                                   boost::bind(&tcp_connection::writeHandler, shared_from_this(), boost::asio::placeholders::error, username_));
     }

     void writeHandler(const boost::system::error_code &error, const std::string &username_)
     {
          if (!error)
          {
               startWriting(username_);
          }
     }

     void readMessages()
     {
          boost::asio::async_read_until(socket_, buffer_, "\n",
                                        boost::bind(&tcp_connection::handleRead, shared_from_this(),
                                                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
     }

     void handleRead(const boost::system::error_code &error, size_t bytes_transfered)
     {
          if (!error)
          {
               std::string message{buffers_begin(buffer_.data()),
                                   buffers_begin(buffer_.data()) + bytes_transfered};
               std::cout << message;
               buffer_.consume(bytes_transfered);
               readMessages();
          }
     }

private:
     tcp_connection(boost::asio::io_context &io_context)
         : socket_{io_context}
     {
     }

     boost::asio::streambuf buffer_;
     tcp::socket socket_;
};

class tcp_client
{
public:
     tcp_client(boost::asio::io_context &io_context, std::string &&username)
         : io_context_{io_context}, username_{std::move(username)}, resolver_{io_context},
           query_{"localhost", "1500"},
           socket_{io_context}
     {

          resolveToServer();
     }

     void resolveToServer()
     {
          resolver_.async_resolve(query_, boost::bind(&tcp_client::resolverHandler, this, boost::asio::placeholders::error,
                                                      boost::asio::placeholders::results));
     }

     void resolverHandler(const boost::system::error_code &error, tcp::resolver::results_type results)
     {
          if (!error)
          {

               tcp_connection::pointer new_connection = tcp_connection::create(io_context_);

               boost::asio::async_connect(new_connection->socket(), results,
                                          boost::bind(&tcp_client::handleConnection, this, new_connection, boost::asio::placeholders::error));
          }
     }

     void handleConnection(tcp_connection::pointer new_connection, const boost::system::error_code &error)
     {
          if (!error)
          {

               boost::thread_group threadpool;

               boost::asio::io_context::work work(io_context_);

               threadpool.create_thread(
                   [this]()
                   { io_context_.run(); });
               threadpool.create_thread(
                   [this]()
                   { io_context_.run(); });

               io_context_.post([this, &new_connection]()
                                { new_connection->readMessages(); });

               io_context_.post([this, &new_connection]()
                                { new_connection->startWriting(this->username_); });

               threadpool.join_all();
          }
     }

private:
     boost::asio::io_context &io_context_;
     std::string username_;
     tcp::resolver resolver_;
     tcp::resolver::query query_;
     tcp::socket socket_;
};

#endif