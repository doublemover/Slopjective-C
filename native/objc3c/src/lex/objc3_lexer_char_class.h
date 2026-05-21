#pragma once

#include <string>

bool IsObjc3IdentifierStart(char c);
bool IsObjc3IdentifierBody(char c);
bool IsObjc3HorizontalWhitespace(char c);
bool TryCountObjc3Utf8Scalars(const std::string &value, int &unit_count);
std::string EscapeObjc3StringTokenText(const std::string &value);
bool TryDecodeObjc3StringTokenText(const std::string &token_text,
                                   std::string &value);
