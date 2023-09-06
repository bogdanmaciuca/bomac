#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

void GenericError(const std::string &message) {
	std::cout << "Error: " << message << "\n";
}
void ErrorRT(u16 line, const std::string &message) {
	std::cout << "Runtime error on line " << line << ": " << message << "\n";
	exit(0);
}

using Object = std::variant<bool, float, std::string>;
enum {
	TYPE_BOOLEAN = 0,
	TYPE_NUMBER,
	TYPE_STRING
};
std::string ObjToStr(const Object& obj) {
	switch (obj.index()) {
	case TYPE_BOOLEAN:
		return (std::get<bool>(obj) ? "true" : "false");
	case TYPE_NUMBER:
		return std::to_string(std::get<float>(obj));
	case TYPE_STRING:
		return std::get<std::string>(obj);
	default:
		return "Internal error in ObjToStr.\n";
	}
}

#endif