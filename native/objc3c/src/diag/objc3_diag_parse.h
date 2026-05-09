#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "diag/objc3_diag_record.h"

bool TryParseUnsignedSegment(std::string_view text,
                             std::size_t begin,
                             std::size_t delimiter_offset,
                             unsigned &value);
bool TryParseRenderedDiagnostic(std::string_view diag_text,
                                Objc3DiagnosticPayload &payload);
bool TryParseDiagnosticCoordinateAndCode(std::string_view diag_text,
                                         unsigned &line,
                                         unsigned &column,
                                         std::string &code);
