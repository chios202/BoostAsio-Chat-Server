#include <boost/asio.hpp>
#include <iostream>
#include "tcp_server.hpp"

int main()
{
    try
    {
        boost::asio::io_context io_context{};
        tcp_server server{io_context};
        io_context.run();
        

    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}