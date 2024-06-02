#include "utils.hpp"

#include <iostream>


int main() {
    assert(ip_helper::urlDecode("Hello%20World%21") == "Hello World!");
    assert(ip_helper::urlDecode("%7B%22key%22%3A%20%22value%22%7D") == "{\"key\": \"value\"}");

    assert(ip_helper::extractIPAddress("ipv4:192.168.1.1:11312") == "192.168.1.1");
    assert(ip_helper::extractIPAddress("ipv6:%5B2001:0db8:85a3:0000:0000:8a2e:0370%5D:7334") ==
           "2001:0db8:85a3:0000:0000:8a2e:0370");

    std::cout << "All tests passed." << std::endl;
    return 0;
}
