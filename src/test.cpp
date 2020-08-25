#include "JsonObject.hpp"
#include <iostream>
#include <bitset>

int main(int argc, char const* argv[]) {
    // std::string testString = R"+*+*( true )+*+*";
    // std::cout << "testString: " << testString << std::endl;
    // JsonObject a = JsonObject::loads(testString);
    JsonObject a = "\"\\u662f\""_json;
    std::cout << a.type() << std::endl;
    std::cout << a.toString() << std::endl;
    return 0;
}