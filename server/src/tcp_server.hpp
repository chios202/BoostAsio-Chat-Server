#ifndef TCP_SERVER
#define TCP_SERVER


#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <thread>

using boost::asio::ip::tcp;
class tcp_connection;

class distribute_over
{
public:

    void addToList(std::shared_ptr<tcp_connection>& Connection_)
    {
        connections_.push_back(Connection_);
    }

    std::vector<std::shared_ptr<tcp_connection>>& getConnections()
    {
        return connections_;
    }


private:

    std::vector<std::shared_ptr<tcp_connection>> connections_;
};

class tcp_connection: public std::enable_shared_from_this<tcp_connection>
{

public:
    using createConnection = std::shared_ptr<tcp_connection>;


    static createConnection create(boost::asio::io_context& io_context,distribute_over& distribute)
    {
        return createConnection(new tcp_connection(io_context,distribute));
    }

    //Getter for socket streaming messages and accepting incoming requests
    tcp::socket& socket()
    {
        return socket_;
    }

    //Member function for reading messages from socket streams
    void readMessagages()
    {
        boost::asio::async_read_until(socket_,buffer_, '\n',
            boost::bind(&tcp_connection::handleReadMessages,shared_from_this(),
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

private:


    //Handler function for previous member function
    void handleReadMessages(const boost::system::error_code& error, size_t bytes_transfered)
    {
        if (!error)
        {
            std::string message{buffers_begin(buffer_.data()), buffers_begin(buffer_.data()) + bytes_transfered};
            buffer_.consume(bytes_transfered);
            
            std::vector<std::shared_ptr<tcp_connection>>& connections_ = distribute_.getConnections();
            
    
            for (auto& client: connections_)
            {
                if (client != shared_from_this())
                {
                    client->writeMessages(message);
                    
                }
            }
            readMessagages();
        }
    }
    

    void writeMessages(const std::string& message)
    {
        boost::asio::async_write(socket_, boost::asio::buffer(message),
            boost::bind(&tcp_connection::handleWrite, shared_from_this(), boost::asio::placeholders::error));
    }

    void handleWrite(const boost::system::error_code& error)
    {
        if (!error)
        {
            readMessagages();
        }
    }
    

    
    tcp_connection(boost::asio::io_context& io_context,distribute_over& distribute)
    : socket_{io_context}, distribute_{distribute}
    {

    }


    distribute_over& distribute_;
    tcp::socket socket_;
    boost::asio::streambuf buffer_;

};


class tcp_server
{
public:

    tcp_server(boost::asio::io_context& io_context)
    : io_context_{io_context}, acceptor_{io_context, tcp::endpoint(tcp::v4(), 1500)}
    {
        acceptIncomingConnecitons();
    }



private:


    //Member function for accepting Incoming Connections
    void acceptIncomingConnecitons()
    {
        tcp_connection::createConnection new_connection{tcp_connection::create(io_context_,distribute)};

        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&tcp_server::handleIncomingConnections,this,new_connection,
            boost::asio::placeholders::error));


    }


    //Handler for previous member function
    void handleIncomingConnections(tcp_connection::createConnection new_connection, 
        const boost::system::error_code& error)
    {
        if (!error)
        {
            distribute.addToList(new_connection);
            new_connection->readMessagages();
        }
        acceptIncomingConnecitons();
    }


    
    distribute_over distribute;
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    

};


#endif