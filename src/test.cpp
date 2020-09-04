#include "JsonObject.hpp"
#include <iostream>

int main(int argc, char const* argv[]) {
    JsonObject a = R"+*+*(
{
    "widget": {
        "debug": "我abc是~\t456傻逼",
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
    std::cout << JsonObject::dumps(JsonObject::loads(JsonObject::dumps(a, 4, true)), 4, true) << std::endl;
    return 0;
}