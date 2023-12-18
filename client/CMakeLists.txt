cmake_minimum_required(VERSION 3.22.1)
project("client")


add_executable("${PROJECT_NAME}" "src/main.cpp" "src/tcp_client.hpp")


find_package(Boost COMPONENTS thread REQUIRED)


target_link_libraries("${PROJECT_NAME}"
    ${Boost_LIBRARIES}
)
