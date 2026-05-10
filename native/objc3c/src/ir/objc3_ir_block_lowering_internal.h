#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "ir/objc3_ir_emitter_context.h"

inline std::string NewObjc3IRBlockTemp(FunctionContext &ctx) {
  return "%t" + std::to_string(ctx.temp_counter++);
}

inline bool Objc3IRBlockSortedStringListContains(
    const std::vector<std::string> &entries, const std::string &needle) {
  return std::binary_search(entries.begin(), entries.end(), needle);
}
