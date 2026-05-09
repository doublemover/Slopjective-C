#pragma once

#include <string>

bool IsObjc3IdentifierStart(char c);
bool IsObjc3IdentifierBody(char c);
bool IsObjc3HorizontalWhitespace(char c);
std::string EscapeObjc3StringTokenText(const std::string &value);
