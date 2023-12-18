#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include "tcp_client.hpp"


int main(int args, char** argv)
{
    try
    {
        boost::asio::io_context io_context;
        boost::shared_ptr<tcp_client> client(new tcp_client(io_context,argv[1]));
        io_context.run();
    }   
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}