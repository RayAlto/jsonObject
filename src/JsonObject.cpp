#include "JsonObject.hpp"

void JsonObject::addIndent(std::ostream& jsonStream, const int& floor, const int& indent) {
    if (indent < 0)
        return;
    for (size_t i = 0; i < floor; i++)
        for (size_t j = 0; j < indent; j++)
            jsonStream << ' ';
}

std::size_t JsonObject::countDigit(const std::string& text, std::size_t startPosition = 0) {
    std::size_t count = 0;
    while (std::isdigit(text[startPosition]) && startPosition < text.size()) {
        count++;
        startPosition++;
    }
    return count;
}

std::size_t JsonObject::countCharacter(const std::string& text, std::size_t startPosition = 0) {
    std::size_t count = 0;
    while (std::isalpha(text[startPosition]) && startPosition < text.size()) {
        count++;
        startPosition++;
    }
    return count;
}

char JsonObject::parseEscapeCharacter(const char& character) {
    switch (character) {
    case '"':
    case '\\':
    case '/':
        return character;
        break;
    case 'b':
        return '\b';
        break;
    case 'f':
        return '\f';
        break;
    case 'n':
        return '\n';
        break;
    case 'r':
        return '\r';
        break;
    case 't':
        return '\t';
        break;
    default:
        return '?';
        break;
    }
}

std::string JsonObject::unicodeToU8String(const std::string& unicodeDigits) {
    int order = std::stoi(unicodeDigits, 0, 16);
    std::ostringstream u8StringStream;
    u8StringStream << char((((order & 0xF000) >> 12) | 0x00E0) - 256);
    u8StringStream << char((((order & 0x0FC0) >> 6) | 0x0080) - 256);
    u8StringStream << char(((order & 0x003F) | 0x0080) - 256);
    return u8StringStream.str();
}

bool JsonObject::parse(const std::string& jsonText, std::size_t& startPosition) {
    clear();
    startPosition = jsonText.find_first_not_of(" \r\n\t", startPosition);
    if (startPosition == std::string::npos)
        return false;
    if (std::isdigit(jsonText[startPosition]) | jsonText[startPosition] == '-' | jsonText[startPosition] == '+')
        parseNumber(this, jsonText, startPosition);
    else if (std::isalpha(jsonText[startPosition]))
        parseEtc(this, jsonText, startPosition);
    else if (jsonText[startPosition] == '"')
        parseString(this, jsonText, startPosition);
    else if (jsonText[startPosition] == '[')
        parseList(this, jsonText, startPosition);
    else if (jsonText[startPosition] == '{')
        parseDict(this, jsonText, startPosition);
    return true;
}

bool JsonObject::parseNumber(JsonObject* jsonObject, const std::string& jsonText, std::size_t& startPosition) {
    bool isDouble = false;
    std::size_t numberLength = 0;
    if (jsonText[startPosition] == '-' | jsonText[startPosition + numberLength] == '+')
        numberLength++;
    numberLength += countDigit(jsonText, startPosition + numberLength);
    if (jsonText[startPosition + numberLength] == '.') {
        isDouble = true;
        numberLength++;
        numberLength += countDigit(jsonText, startPosition + numberLength);
    }
    if (jsonText[startPosition + numberLength] == 'e' | jsonText[startPosition + numberLength] == 'E') {
        isDouble = true;
        numberLength++;
        if (jsonText[startPosition + numberLength] == '-' | jsonText[startPosition + numberLength] == '+')
            numberLength++;
        numberLength += countDigit(jsonText, startPosition + numberLength);
    }
    if (isDouble) {
        jsonObject->_type = ObjType::DOUBLE;
        jsonObject->_ptr._double = new double(std::stod(jsonText.substr(startPosition, numberLength)));
    }
    else {
        jsonObject->_type = ObjType::INT;
        jsonObject->_ptr._int = new int(std::stoi(jsonText.substr(startPosition, numberLength)));
    }
    startPosition += numberLength;
    return true;
}

bool JsonObject::parseEtc(JsonObject* jsonObject, const std::string& jsonText, std::size_t& startPosition) {
    std::size_t etcLength = countCharacter(jsonText, startPosition);
    std::string etcString = jsonText.substr(startPosition, etcLength);
    if (etcString == "true") {
        jsonObject->_type = ObjType::BOOL;
        jsonObject->_ptr._bool = new bool(true);
    }
    else if (etcString == "false") {
        jsonObject->_type = ObjType::BOOL;
        jsonObject->_ptr._bool = new bool(false);
    }
    else if (etcString == "null") {
        jsonObject->_type = ObjType::NUL;
    }
    else
        return false;
    return true;
}

bool JsonObject::parseString(JsonObject* jsonObject, const std::string& jsonText, std::size_t& startPosition) {
    std::ostringstream parsedStringSteam;
    if (jsonText[startPosition] == '"')
        startPosition++;
    while (jsonText[startPosition] != '"' && startPosition < jsonText.size()) {
        if (jsonText[startPosition] == '\\') {
            startPosition++;
            if (startPosition >= jsonText.size())
                break;
            if (jsonText[startPosition] == 'u') { // It is an Unicode Character
                if (startPosition + 4 >= jsonText.size())
                    break;
                parsedStringSteam << unicodeToU8String(jsonText.substr(startPosition + 1, 4));
                startPosition += 4;
            }
            else
                parsedStringSteam << parseEscapeCharacter(jsonText[startPosition]);
        }
        else {
            parsedStringSteam << jsonText[startPosition];
        }
        startPosition++;
    }
    startPosition++;
    jsonObject->_type = ObjType::STR;
    jsonObject->_ptr._str = new std::string(parsedStringSteam.str());
    return true;
}

bool JsonObject::parseList(JsonObject* jsonObject, const std::string& jsonText, std::size_t& startPosition) {
    jsonObject->_type = ObjType::LIST;
    jsonObject->_ptr._list = new std::list<JsonObject*>();
    if (jsonText[startPosition] == '[')
        startPosition++;
    startPosition = jsonText.find_first_not_of(" \r\n\t", startPosition);
    while (startPosition < jsonText.size()) {
        JsonObject* newItem = new JsonObject();
        newItem->parse(jsonText, startPosition);
        jsonObject->_ptr._list->emplace_back(newItem);
        startPosition = jsonText.find_first_not_of(" \r\n\t", startPosition);
        if (jsonText[startPosition] == ',') {
            startPosition++;
        }
        else if (jsonText[startPosition] == ']') {
            startPosition++;
            return true;
        }
        else {
            return false;
        }
    }
    return true;
}

bool JsonObject::parseDict(JsonObject* jsonObject, const std::string& jsonText, std::size_t& startPosition) {
    jsonObject->_type = ObjType::DICT;
    jsonObject->_ptr._dict = new std::list<std::pair<JsonObject*, JsonObject*>>();
    if (jsonText[startPosition] == '{')
        startPosition++;
    startPosition = jsonText.find_first_not_of(" \r\n\t", startPosition);
    while (startPosition < jsonText.size()) {
        JsonObject* newItemKey = new JsonObject();
        JsonObject* newItemValue = new JsonObject();
        newItemKey->parse(jsonText, startPosition);
        startPosition = jsonText.find_first_not_of(" \r\n\t", startPosition);
        if (jsonText[startPosition] == ':') {
            startPosition++;
            startPosition = jsonText.find_first_not_of(" \r\n\t", startPosition);
        }
        else {
            delete newItemKey;
            delete newItemValue;
            return false;
        }
        newItemValue->parse(jsonText, startPosition);
        jsonObject->_ptr._dict->emplace_back(std::pair<JsonObject*, JsonObject*>(newItemKey, newItemValue));
        startPosition = jsonText.find_first_not_of(" \r\n\t", startPosition);
        if (jsonText[startPosition] == ',') {
            startPosition++;
        }
        else if (jsonText[startPosition] == '}') {
            startPosition++;
            return true;
        }
        else {
            return false;
        }
    }
    return true;
}

bool JsonObject::format(const JsonObject& jsonObject, std::ostream& jsonStream, int floor, const int& indent, const bool& ensureAscii) {
    switch (jsonObject._type) {
    case ObjType::INT:
        jsonStream << *jsonObject._ptr._int;
        break;
    case ObjType::BOOL:
        jsonStream << std::boolalpha << *jsonObject._ptr._bool << std::noboolalpha;
        break;
    case ObjType::STR:
        jsonStream << '"';
        for (const char& c : *jsonObject._ptr._str) {
            switch (c) {
            case '"':
                jsonStream << "\\\"";
                break;
            case '\\':
                jsonStream << "\\\\";
                break;
            case '\b':
                jsonStream << "\\b";
                break;
            case '\f':
                jsonStream << "\\f";
                break;
            case '\n':
                jsonStream << "\\n";
                break;
            case '\r':
                jsonStream << "\\r";
                break;
            case '\t':
                jsonStream << "\\t";
                break;
            default:
                jsonStream << c;
                break;
            }
        }
        jsonStream << '"';
        break;
    case ObjType::DOUBLE:
        jsonStream << *jsonObject._ptr._double;
        break;
    case ObjType::LIST: {
        jsonStream << '[';
        if (indent >= 0)
            jsonStream << '\n';
        std::list<JsonObject*>::iterator listIterator = jsonObject._ptr._list->begin();
        while (listIterator != --jsonObject._ptr._list->end()) {
            addIndent(jsonStream, floor + 1, indent);
            format(**listIterator, jsonStream, floor + 1, indent, ensureAscii);
            jsonStream << ',';
            jsonStream << (indent >= 0 ? '\n' : ' ');
            listIterator++;
        }
        addIndent(jsonStream, floor + 1, indent);
        format(**listIterator, jsonStream, floor + 1, indent, ensureAscii);
        if (indent >= 0)
            jsonStream << '\n';
        break;
        addIndent(jsonStream, floor, indent);
        jsonStream << '[';
    }
    case ObjType::DICT: {
        jsonStream << '{';
        if (indent >= 0)
            jsonStream << '\n';
        std::list<std::pair<JsonObject*, JsonObject*>>::iterator dictIterator = jsonObject._ptr._dict->begin();
        while (dictIterator != --jsonObject._ptr._dict->end()) {
            addIndent(jsonStream, floor + 1, indent);
            format(*(*dictIterator).first, jsonStream, floor + 1, indent, ensureAscii);
            jsonStream << ": ";
            format(*(*dictIterator).second, jsonStream, floor + 1, indent, ensureAscii);
            jsonStream << ',';
            jsonStream << (indent >= 0 ? '\n' : ' ');
            dictIterator++;
        }
        addIndent(jsonStream, floor + 1, indent);
        format(*(*dictIterator).first, jsonStream, floor + 1, indent, ensureAscii);
        jsonStream << ": ";
        format(*(*dictIterator).second, jsonStream, floor + 1, indent, ensureAscii);
        if (indent >= 0)
            jsonStream << "\n";
        addIndent(jsonStream, floor, indent);
        jsonStream << '}';
        break;
    }
    case ObjType::NUL:
        jsonStream << "null";
        break;
    default:
        return false;
        break;
    }
    return true;
}

JsonObject* JsonObject::generateJsonObject(const int& val) {
    JsonObject* object = new JsonObject();
    object->_type = ObjType::INT;
    object->_ptr._int = new int(val);
    return object;
}

JsonObject* JsonObject::generateJsonObject(const double& val) {
    JsonObject* object = new JsonObject();
    object->_type = ObjType::DOUBLE;
    object->_ptr._double = new double(val);
    return object;
}

JsonObject* JsonObject::generateJsonObject(const std::string& val) {
    JsonObject* object = new JsonObject();
    object->_type = ObjType::STR;
    object->_ptr._str = new std::string(val);
    return object;
}

JsonObject::~JsonObject() {
    switch (_type) {
    case ObjType::INT:
        delete _ptr._int;
        break;
    case ObjType::BOOL:
        delete _ptr._bool;
        break;
    case ObjType::STR:
        delete _ptr._str;
        break;
    case ObjType::DOUBLE:
        delete _ptr._double;
        break;
    case ObjType::LIST:
        for (JsonObject* i : *_ptr._list) {
            delete i;
        }
        delete _ptr._list;
        break;
    case ObjType::DICT:
        for (std::pair<JsonObject*, JsonObject*>& i : *_ptr._dict) {
            delete i.first;
            delete i.second;
        }
        delete _ptr._dict;
        break;
    }
}

JsonObject::JsonObject() : _ptr {nullptr} {}

JsonObject::JsonObject(const JsonObject& jsonObject) : JsonObject() {
    _type = jsonObject._type;
    switch (_type) {
    case ObjType::INT:
        _ptr._int = new int(*jsonObject._ptr._int);
        break;
    case ObjType::BOOL:
        _ptr._bool = new bool(*jsonObject._ptr._bool);
        break;
    case ObjType::STR:
        _ptr._str = new std::string(*jsonObject._ptr._str);
        break;
    case ObjType::DOUBLE:
        _ptr._double = new double(*jsonObject._ptr._double);
        break;
    case ObjType::LIST:
        _ptr._list = new std::list<JsonObject*>();
        for (const JsonObject* i : *jsonObject._ptr._list) {
            _ptr._list->emplace_back(new JsonObject(*i));
        }
        break;
    case ObjType::DICT:
        _ptr._dict = new std::list<std::pair<JsonObject*, JsonObject*>>();
        for (const std::pair<JsonObject*, JsonObject*>& i : *jsonObject._ptr._dict) {
            _ptr._dict->emplace_back(std::pair<JsonObject*, JsonObject*>(new JsonObject(*i.first), new JsonObject(*i.second)));
        }
        break;
    }
}

JsonObject::JsonObject(const std::string& jsonText) : JsonObject() {
    std::size_t position = 0;
    parse(jsonText, position);
}

std::string JsonObject::type() const {
    switch (_type) {
    case ObjType::INT:
        return "int";
    case ObjType::BOOL:
        return "bool";
    case ObjType::STR:
        return "string";
    case ObjType::DOUBLE:
        return "double";
    case ObjType::LIST:
        return "list";
    case ObjType::DICT:
        return "dict";
    case ObjType::NUL:
        return "null";
    }
    return "";
}

int JsonObject::toInt() const {
    return _type == ObjType::INT ? *_ptr._int : 0;
}

bool JsonObject::toBool() const {
    return _type == ObjType::BOOL ? *_ptr._bool : false;
}

std::string JsonObject::toString() const {
    return _type == ObjType::STR ? *_ptr._str : "";
}

double JsonObject::toDouble() const {
    return _type == ObjType::DOUBLE ? *_ptr._double : 0;
}

int& JsonObject::getInt() {
    return *_ptr._int;
}

bool& JsonObject::getBool() {
    return *_ptr._bool;
}

std::string& JsonObject::getString() {
    return *_ptr._str;
}

double& JsonObject::getDouble() {
    return *_ptr._double;
}

int& JsonObject::operator=(const int& rv) {
    *_ptr._int = rv;
    return *_ptr._int;
}

bool& JsonObject::operator=(const bool& rv) {
    *_ptr._bool = rv;
    return *_ptr._bool;
}

std::string& JsonObject::operator=(const std::string& rv) {
    *_ptr._str = rv;
    return *_ptr._str;
}

double& JsonObject::operator=(const double& rv) {
    *_ptr._double = rv;
    return *_ptr._double;
}

JsonObject& JsonObject::operator[](const int& rv) {
    if (_type == ObjType::LIST) {
        if (rv >= _ptr._list->size())
            return *this;
        std::list<JsonObject*>::iterator objectIterator = _ptr._list->begin();
        std::size_t index = rv;
        while (index > 0 && objectIterator != _ptr._list->end()) {
            objectIterator++;
            index--;
        }
        return **objectIterator;
    }
    else if (_type == ObjType::DICT) {
        std::list<std::pair<JsonObject*, JsonObject*>>::iterator objectPairIterator = _ptr._dict->begin();
        while (objectPairIterator != _ptr._dict->end()) {
            if (objectPairIterator->first->_type == ObjType::INT && *objectPairIterator->first->_ptr._int == rv)
                return *(objectPairIterator->second);
            objectPairIterator++;
        }
        return *this;
    }
    return *this;
}

JsonObject& JsonObject::operator[](const double& rv) {
    if (_type == ObjType::DICT) {
        std::list<std::pair<JsonObject*, JsonObject*>>::iterator objectPairIterator = _ptr._dict->begin();
        while (objectPairIterator != _ptr._dict->end()) {
            if (objectPairIterator->first->_type == ObjType::DOUBLE && *objectPairIterator->first->_ptr._double == rv)
                return *(objectPairIterator->second);
            objectPairIterator++;
        }
    }
    return *this;
}

JsonObject& JsonObject::operator[](const std::string& rv) {
    if (_type == ObjType::DICT) {
        std::list<std::pair<JsonObject*, JsonObject*>>::iterator objectPairIterator = _ptr._dict->begin();
        while (objectPairIterator != _ptr._dict->end()) {
            if (objectPairIterator->first->_type == ObjType::STR && *objectPairIterator->first->_ptr._str == rv)
                return *(objectPairIterator->second);
            objectPairIterator++;
        }
    }
    return *this;
}

bool JsonObject::empty() const {
    switch (_type) {
    case ObjType::STR:
        return _ptr._str->empty();
    case ObjType::LIST:
        return _ptr._list->empty();
    case ObjType::DICT:
        return _ptr._dict->empty();
    case ObjType::ERROR:
        return true;
    }
    return false;
}

std::size_t JsonObject::size() const {
    switch (_type) {
    case ObjType::STR:
        return _ptr._str->size();
    case ObjType::LIST:
        return _ptr._list->size();
    case ObjType::DICT:
        return _ptr._dict->size();
    }
    return 0;
}

void JsonObject::clear() {
    if (_type == ObjType::ERROR)
        return;
    switch (_type) {
    case ObjType::INT:
        *_ptr._int = 0;
        break;
    case ObjType::BOOL:
        *_ptr._bool = false;
        break;
    case ObjType::STR:
        _ptr._str->clear();
        break;
    case ObjType::DOUBLE:
        *_ptr._double = 0.0;
        break;
    case ObjType::LIST:
        for (JsonObject* i : *_ptr._list) {
            delete i;
        }
        _ptr._list->clear();
        break;
    case ObjType::DICT:
        for (std::pair<JsonObject*, JsonObject*> i : *_ptr._dict) {
            delete i.first;
            delete i.second;
        }
        _ptr._dict->clear();
        break;
    }
}

JsonObject& JsonObject::loads(const std::string& jsonText) {
    return *(new JsonObject(jsonText));
}

JsonObject& JsonObject::load(const std::string& jsonFile) {
    std::ifstream jsonFileStream(jsonFile, std::ios::in);
    std::ostringstream jsonTextStringStream;
    std::string jsonTextTemp;
    while (!jsonFileStream.eof()) {
        std::getline(jsonFileStream, jsonTextTemp);
        jsonTextStringStream << jsonTextTemp;
    }
    return *(new JsonObject(jsonTextStringStream.str()));
}

std::string JsonObject::dumps(const JsonObject& jsonObject, int indent = -1, bool ensureAscii = false) {
    std::ostringstream jsonStringStream;
    format(jsonObject, jsonStringStream, 0, indent, ensureAscii);
    return jsonStringStream.str();
}

bool JsonObject::dump(const std::string& fileName, const JsonObject& jsonObject, int indent = -1, bool ensureAscii = false) {
    std::ofstream jsonFsteam(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    if (jsonFsteam.bad())
        return false;
    format(jsonObject, jsonFsteam, 0, indent, ensureAscii);
    return true;
}

JsonObject& operator""_json(const char* str, std::size_t len) {
    return *(new JsonObject(str));
}