#pragma once

#include <string_view>

#ifndef OM_DECLSPEC
#define OM_DECLSPEC
#endif

/// print to stdout "hello, {user_name}" and return true on success
OM_DECLSPEC bool greetings(std::string_view user_name);
