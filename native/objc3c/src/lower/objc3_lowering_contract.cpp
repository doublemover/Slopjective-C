#include "lower/objc3_lowering_contract.h"

#include <cctype>
#include <string>

namespace {

bool IsRuntimeDispatchSymbolStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

bool IsRuntimeDispatchSymbolBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

const char *AtomicMemoryOrderToken(Objc3AtomicMemoryOrder order) {
  switch (order) {
    case Objc3AtomicMemoryOrder::Relaxed:
      return kObjc3AtomicMemoryOrderRelaxed;
    case Objc3AtomicMemoryOrder::Acquire:
      return kObjc3AtomicMemoryOrderAcquire;
    case Objc3AtomicMemoryOrder::Release:
      return kObjc3AtomicMemoryOrderRelease;
    case Objc3AtomicMemoryOrder::AcqRel:
      return kObjc3AtomicMemoryOrderAcqRel;
    case Objc3AtomicMemoryOrder::SeqCst:
    default:
      return kObjc3AtomicMemoryOrderSeqCst;
  }
}

}  // namespace

bool IsValidRuntimeDispatchSymbol(const std::string &symbol) {
  if (symbol.empty() || !IsRuntimeDispatchSymbolStart(symbol[0])) {
    return false;
  }
  for (std::size_t i = 1; i < symbol.size(); ++i) {
    if (!IsRuntimeDispatchSymbolBody(symbol[i])) {
      return false;
    }
  }
  return true;
}

bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,
                                       Objc3LoweringContract &normalized,
                                       std::string &error) {
  if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {
    error = "invalid lowering contract max_message_send_args: " + std::to_string(input.max_message_send_args) +
            " (expected <= " + std::to_string(kObjc3RuntimeDispatchMaxArgs) + ")";
    return false;
  }
  if (!IsValidRuntimeDispatchSymbol(input.runtime_dispatch_symbol)) {
    error = "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " +
            input.runtime_dispatch_symbol;
    return false;
  }
  normalized.max_message_send_args = input.max_message_send_args;
  normalized.runtime_dispatch_symbol = input.runtime_dispatch_symbol;
  return true;
}

bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,
                                     Objc3LoweringIRBoundary &boundary,
                                     std::string &error) {
  Objc3LoweringContract normalized;
  if (!TryNormalizeObjc3LoweringContract(input, normalized, error)) {
    return false;
  }
  boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;
  boundary.runtime_dispatch_symbol = normalized.runtime_dispatch_symbol;
  boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;
  return true;
}

std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary) {
  return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +
         ";runtime_dispatch_arg_slots=" + std::to_string(boundary.runtime_dispatch_arg_slots) +
         ";selector_global_ordering=" + boundary.selector_global_ordering;
}

bool TryGetCompoundAssignmentBinaryOpcode(const std::string &op, std::string &opcode) {
  if (op == "+=") {
    opcode = "add";
    return true;
  }
  if (op == "-=") {
    opcode = "sub";
    return true;
  }
  if (op == "*=") {
    opcode = "mul";
    return true;
  }
  if (op == "/=") {
    opcode = "sdiv";
    return true;
  }
  if (op == "%=") {
    opcode = "srem";
    return true;
  }
  if (op == "&=") {
    opcode = "and";
    return true;
  }
  if (op == "|=") {
    opcode = "or";
    return true;
  }
  if (op == "^=") {
    opcode = "xor";
    return true;
  }
  if (op == "<<=") {
    opcode = "shl";
    return true;
  }
  if (op == ">>=") {
    opcode = "ashr";
    return true;
  }
  return false;
}

bool TryParseObjc3AtomicMemoryOrder(const std::string &token, Objc3AtomicMemoryOrder &order) {
  if (token == kObjc3AtomicMemoryOrderRelaxed) {
    order = Objc3AtomicMemoryOrder::Relaxed;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderAcquire) {
    order = Objc3AtomicMemoryOrder::Acquire;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderRelease) {
    order = Objc3AtomicMemoryOrder::Release;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderAcqRel || token == "acquire_release") {
    order = Objc3AtomicMemoryOrder::AcqRel;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderSeqCst) {
    order = Objc3AtomicMemoryOrder::SeqCst;
    return true;
  }
  return false;
}

const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order) {
  switch (order) {
    case Objc3AtomicMemoryOrder::Relaxed:
      return "monotonic";
    case Objc3AtomicMemoryOrder::Acquire:
      return "acquire";
    case Objc3AtomicMemoryOrder::Release:
      return "release";
    case Objc3AtomicMemoryOrder::AcqRel:
      return "acq_rel";
    case Objc3AtomicMemoryOrder::SeqCst:
    default:
      return "seq_cst";
  }
}

std::string Objc3AtomicMemoryOrderMappingReplayKey() {
  return std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Relaxed)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Relaxed) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Acquire)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Acquire) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Release)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Release) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::AcqRel)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::AcqRel) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::SeqCst)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::SeqCst);
}
