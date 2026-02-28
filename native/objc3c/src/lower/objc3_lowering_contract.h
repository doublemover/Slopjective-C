#pragma once

#include <cstddef>
#include <string>

inline constexpr std::size_t kObjc3RuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kObjc3RuntimeDispatchMaxArgs = 16;
inline constexpr const char *kObjc3RuntimeDispatchSymbol = "objc3_msgsend_i32";

struct Objc3LoweringContract {
  std::size_t max_message_send_args = kObjc3RuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
};

bool IsValidRuntimeDispatchSymbol(const std::string &symbol);
bool TryGetCompoundAssignmentBinaryOpcode(const std::string &op, std::string &opcode);
