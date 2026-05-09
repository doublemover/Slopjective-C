#pragma once

#include <string>
#include <vector>

#include "diag/objc3_diag_types.h"

DiagSortKey ParseDiagSortKey(const std::string &diag);
void NormalizeDiagnostics(std::vector<std::string> &diagnostics);
