#ifndef UTILITIES_H
#define UTILITIES_H

#include <optional>
#include <string>

#define REMOVE_QUOTES(str) ([](const std::string& s) { \
    std::string result = s; \
    result.erase(std::remove(result.begin(), result.end(), '\"'), result.end()); \
    return result; })(str)

std::optional<int> to_int(const std::string& str);

int select_by_id(int count_id);

bool is_valid_IP(const std::string& ip);

std::string remove_ansi(const std::string& input);

#endif // UTILITIES_H