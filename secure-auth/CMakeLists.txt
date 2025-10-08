cmake_minimum_required(VERSION 4.0)
project(secure-auth)

set(CMAKE_CXX_STANDARD 20)

add_executable(secure-auth
        src/service/JwtService.h
        src/SecureAuth.h
)
