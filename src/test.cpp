#include "JsonObject.hpp"
#include <iostream>
#include <bitset>

int main(int argc, char const* argv[]) {
    // std::string testString = R"+*+*( true )+*+*";
    // std::cout << "testString: " << testString << std::endl;
    // JsonObject a = JsonObject::loads(testString);
    JsonObject a = R"+*+*(
{
    "widget": {
        "debug": "on",
        "window": {
            "title": "Sample Konfabulator Widget",
            "name": "main_window",
            "width": 500,
            "height": 500
        },
        "image": {
            "src": "Images/Sun.png",
            "name": "sun1",
            "hOffset": 250,
            "vOffset": 250,
            "alignment": "center"
        },
        "text": {
            "data": "Click Here",
            "size": 36,
            "style": "bold",
            "name": "text1",
            "hOffset": 250,
            "vOffset": 100,
            "alignment": "center",
            "onMouseUp": "sun1.opacity = (sun1.opacity / 100) * 90;"
        }
    }
}
)+*+*"_json;
    std::cout << a.type() << std::endl;
    std::cout << a.size() << std::endl;
    std::cout << (a["widget"]["window"]["height"]).toInt() << std::endl;

    return 0;
}