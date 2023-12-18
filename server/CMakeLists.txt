cmake_minimum_required(VERSION 3.22.1)
project("server")
add_executable("${PROJECT_NAME}" "src/main.cpp" "src/tcp_server.hpp")

