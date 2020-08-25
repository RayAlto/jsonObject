#include "JsonObject.hpp"
#include <iostream>

int main(int argc, char const* argv[]) {
    // std::string testString = R"+*+*( true )+*+*";
    // std::cout << "testString: " << testString << std::endl;
    // JsonObject a = JsonObject::loads(testString);
    JsonObject a = "true"_json;
    std::cout << a.type() << std::endl;
    std::cout << "a: " << (a ? "true" : "false") << std::endl;
    a = false;
    std::cout << "a: " << (a ? "true" : "false") << std::endl;
    return 0;
}