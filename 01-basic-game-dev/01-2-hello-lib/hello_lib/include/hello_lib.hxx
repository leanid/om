#pragma once

#include <string_view>

/// print to stdout "hello, {user_name}" and return true on success
bool greetings(std::string_view user_name);
