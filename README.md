# BoostAsio-Chat-Server
This is a Chat-Server developed with Boost.Asio library, it works in a non-blocking asynchronous and multi-threaded manner.
Boost dependecy is required in order for this application to work, you can follow the steps provided [here](https://www.boost.org/doc/libs/1_83_0/more/getting_started/unix-variants.html).

**Build instructions:**

  - mkdir build && cd build
  - cmake ..
  - make

  **How to run:**
  
  After you have built your project, you can follow the next steps:
    1. Start the server with ./server, now it can start listening for incoming connections
    
    2. Start one or more clients with ./client {your_username}
   
