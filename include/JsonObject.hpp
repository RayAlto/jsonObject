#pragma once
#ifndef __JSONOBJECT_HPP__
#define __JSONOBJECT_HPP__

#include <cstddef>
#include <fstream>
#include <string>
#include <sstream>
#include <list>

class JsonObject {
private:
    enum class ObjType : uint8_t { ERROR, INT, BOOL, STR, DOUBLE, NUL, LIST, DICT };

    union ObjPtr {
        int* _int;
        bool* _bool;
        double* _double;
        std::string* _str;
        std::list<JsonObject*>* _list;
        std::list<std::pair<JsonObject*, JsonObject*>>* _dict;
    };

private:
    ObjType _type = ObjType::ERROR;
    ObjPtr _ptr;

protected:
    static inline void addIndent(std::ostream&, const int&, const int&);
    static inline std::size_t countDigit(const std::string&, std::size_t);
    static inline std::size_t countCharacter(const std::string&, std::size_t);
    static inline char parseEscapeCharacter(const char&);
    static inline std::string unicodeToU8String(const std::string&);
    bool parse(const std::string&, std::size_t&);
    static bool parseNumber(JsonObject*, const std::string&, std::size_t&);
    static bool parseEtc(JsonObject*, const std::string&, std::size_t&);
    static bool parseString(JsonObject*, const std::string&, std::size_t&);
    static bool parseList(JsonObject*, const std::string&, std::size_t&);
    static bool parseDict(JsonObject*, const std::string&, std::size_t&);
    static bool format(const JsonObject&, std::ostream&, int floor, const int& indent, const bool& ensureAscii);
    static JsonObject* generateJsonObject(const int&);
    static JsonObject* generateJsonObject(const double&);
    static JsonObject* generateJsonObject(const std::string&);

public:
    ~JsonObject();
    JsonObject();
    JsonObject(const JsonObject&);
    JsonObject(const std::string&);
    std::string type() const;
    int toInt() const;
    bool toBool() const;
    std::string toString() const;
    double toDouble() const;
    int& getInt();
    bool& getBool();
    std::string& getString();
    double& getDouble();
    int& operator=(const int&);
    bool& operator=(const bool&);
    std::string& operator=(const std::string&);
    double& operator=(const double&);
    JsonObject& operator[](const int&);
    JsonObject& operator[](const double&);
    JsonObject& operator[](const std::string&);
    bool empty() const;
    std::size_t size() const;
    void clear();
    static JsonObject& loads(const std::string&);
    static JsonObject& load(const std::string&);
    static std::string dumps(const JsonObject&, int, bool);
    static bool dump(const std::string&, const JsonObject&, int, bool);
};

JsonObject& operator""_json(const char*, std::size_t);

#endif // !__JSONOBJECT_HPP__