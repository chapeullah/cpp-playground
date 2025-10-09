cmake_minimum_required(VERSION 4.0)
project(secure-auth)

set(CMAKE_CXX_STANDARD 20)

add_library(secure-auth INTERFACE)
target_include_directories(secure-auth INTERFACE src)
target_sources(secure-auth INTERFACE
        FILE_SET HEADERS
        BASE_DIRS src
        FILES
            src/service/JwtService.h
            src/SecureAuth.h
)