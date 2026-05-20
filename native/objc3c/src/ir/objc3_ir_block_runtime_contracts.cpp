#include "ir/objc3_ir_block_runtime_contracts.h"

#include <cstddef>
#include <cstdint>
#include <sstream>

bool BlockLiteralUsesPointerCaptureStorage(const Expr &expr) {
  return expr.block_storage_byref_slot_count > 0u ||
         expr.block_runtime_owned_object_capture_count > 0u ||
         expr.block_runtime_weak_object_capture_count > 0u ||
         expr.block_runtime_unowned_object_capture_count > 0u ||
         expr.block_runtime_copy_helper_required ||
         expr.block_runtime_dispose_helper_required;
}

std::string BuildBlockDescriptorType() {
  return "{ i64, i64, i32, i32, i32, ptr }";
}

std::string BuildBlockStorageType(const Expr &expr) {
  std::ostringstream out;
  if (!BlockLiteralUsesPointerCaptureStorage(expr)) {
    out << "{ ptr, ["
        << static_cast<unsigned long long>(
               expr.block_capture_names_lexicographic.size())
        << " x i32] }";
    return out.str();
  }
  out << "{ ptr, ptr, ptr, ["
      << static_cast<unsigned long long>(
             expr.block_capture_names_lexicographic.size())
      << " x ptr] }";
  return out.str();
}

std::string BuildBlockDescriptorSymbol(const Expr &expr) {
  return expr.block_abi_descriptor_symbol.empty()
             ? std::string("objc3_block_descriptor_missing_symbol")
             : expr.block_abi_descriptor_symbol;
}

std::string BuildBlockInvokeSymbol(const Expr &expr) {
  return expr.block_invoke_trampoline_symbol.empty()
             ? std::string("objc3_block_invoke_missing_symbol")
             : expr.block_invoke_trampoline_symbol;
}

std::uint32_t BuildBlockDescriptorFlags(const Expr &expr) {
  std::uint32_t flags = 0u;
  if (BlockLiteralUsesPointerCaptureStorage(expr)) {
    flags |= 1u << 0u;
  }
  if (expr.block_runtime_copy_helper_required) {
    flags |= 1u << 1u;
  }
  if (expr.block_runtime_dispose_helper_required) {
    flags |= 1u << 2u;
  }
  return flags;
}

bool BlockLiteralRequiresEscapingRuntimeHooks(const Expr &expr) {
  return expr.block_escape_shape_symbol == "global-initializer" ||
         expr.block_escape_shape_symbol == "assignment-value" ||
         expr.block_escape_shape_symbol == "return-value" ||
         expr.block_escape_shape_symbol == "call-argument" ||
         expr.block_escape_shape_symbol == "message-argument";
}

bool BlockLiteralSupportsScalarRuntimePromotion(const Expr &expr) {
  return expr.block_source_model_is_normalized &&
         expr.block_runtime_capture_ownership_is_normalized &&
         (!expr.block_storage_requires_byref_cells ||
          !expr.block_storage_byref_layout_symbol.empty()) &&
         ((!expr.block_runtime_copy_helper_required &&
           !expr.block_copy_helper_required) ||
          !expr.block_copy_helper_symbol.empty()) &&
         ((!expr.block_runtime_dispose_helper_required &&
           !expr.block_dispose_helper_required) ||
          !expr.block_dispose_helper_symbol.empty());
}

bool BlockLiteralSupportsEscapingRuntimeHookLowering(const Expr &expr) {
  return expr.block_storage_escape_to_heap &&
         BlockLiteralSupportsScalarRuntimePromotion(expr);
}

bool BlockLiteralRequiresFutureRuntimeLanes(const Expr &expr) {
  return !BlockLiteralSupportsScalarRuntimePromotion(expr);
}

std::string BuildBlockCopyHelperSymbol(const Expr &expr) {
  return expr.block_copy_helper_symbol.empty()
             ? std::string("objc3_block_copy_helper_missing_symbol")
             : expr.block_copy_helper_symbol;
}

std::string BuildBlockDisposeHelperSymbol(const Expr &expr) {
  return expr.block_dispose_helper_symbol.empty()
             ? std::string("objc3_block_dispose_helper_missing_symbol")
             : expr.block_dispose_helper_symbol;
}

std::uint64_t AlignBlockStorageBytes(std::uint64_t value,
                                     std::uint64_t alignment) {
  if (alignment == 0u) {
    return value;
  }
  const std::uint64_t remainder = value % alignment;
  return remainder == 0u ? value : value + (alignment - remainder);
}

std::uint64_t BlockStorageStaticSizeBytes(const Expr &expr) {
  const std::uint64_t capture_count = static_cast<std::uint64_t>(
      expr.block_capture_names_lexicographic.size());
  if (!BlockLiteralUsesPointerCaptureStorage(expr)) {
    return AlignBlockStorageBytes(
        static_cast<std::uint64_t>(sizeof(void *)) +
            capture_count * static_cast<std::uint64_t>(sizeof(std::int32_t)),
        static_cast<std::uint64_t>(alignof(void *)));
  }
  return static_cast<std::uint64_t>(sizeof(void *) * 3u) +
         capture_count * static_cast<std::uint64_t>(sizeof(void *));
}
