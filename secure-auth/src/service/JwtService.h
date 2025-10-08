#ifndef SECURE_AUTH_JWTSERVICE_H
#define SECURE_AUTH_JWTSERVICE_H
#include <string>

class JwtService {
    static constexpr std::string JWT_SECRET;

public:
    JwtService();

private:

};


#endif