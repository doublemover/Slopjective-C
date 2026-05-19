#pragma once

#include <cstdint>
#include <string>

#include "ast/objc3_ast.h"

bool BlockLiteralUsesPointerCaptureStorage(const Expr &expr);
std::string BuildBlockDescriptorType();
std::string BuildBlockStorageType(const Expr &expr);
std::string BuildBlockDescriptorSymbol(const Expr &expr);
std::string BuildBlockInvokeSymbol(const Expr &expr);
std::uint32_t BuildBlockDescriptorFlags(const Expr &expr);
bool BlockLiteralRequiresEscapingRuntimeHooks(const Expr &expr);
bool BlockLiteralSupportsScalarRuntimePromotion(const Expr &expr);
bool BlockLiteralSupportsEscapingRuntimeHookLowering(const Expr &expr);
bool BlockLiteralRequiresFutureRuntimeLanes(const Expr &expr);
std::string BuildBlockCopyHelperSymbol(const Expr &expr);
std::string BuildBlockDisposeHelperSymbol(const Expr &expr);
std::uint64_t AlignBlockStorageBytes(std::uint64_t value,
                                     std::uint64_t alignment);
std::uint64_t BlockStorageStaticSizeBytes(const Expr &expr);
