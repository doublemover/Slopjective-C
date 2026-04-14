#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "diag/objc3_diag_types.h"

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message);
bool StartsWith(std::string_view value, std::string_view prefix);
bool TryParseUnsignedSegment(std::string_view text, std::size_t begin, std::size_t delimiter_offset, unsigned &value);
bool TryParseDiagnosticCoordinateAndCode(std::string_view diag_text, unsigned &line, unsigned &column, std::string &code);
std::string ToLower(std::string value);
DiagSortKey ParseDiagSortKey(const std::string &diag);
void NormalizeDiagnostics(std::vector<std::string> &diagnostics);
