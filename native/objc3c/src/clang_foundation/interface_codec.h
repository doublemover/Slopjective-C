#pragma once

#include <string>

#include "clang_foundation/interface_contract.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"

namespace objc3::clang_foundation {

// Encodes the sidecar as deterministic JSON. The canonical Clang-generated
// source remains a separate <module>.objc3i file.
[[nodiscard]] llvm::Expected<std::string> EncodeSemanticInterfaceSidecar(
    const SemanticInterfaceSidecar &sidecar,
    llvm::ArrayRef<llvm::StringRef> supported_required_semantics);

// Decoding binds the sidecar to the exact UTF-8 bytes in the paired .objc3i
// source. Unknown schema keys and required semantics fail closed.
[[nodiscard]] llvm::Expected<SemanticInterfaceSidecar>
DecodeSemanticInterfaceSidecar(
    llvm::StringRef json_bytes,
    llvm::StringRef generated_source_bytes,
    llvm::ArrayRef<llvm::StringRef> supported_required_semantics);

[[nodiscard]] std::string ComputeInterfaceSourceSha256(
    llvm::StringRef generated_source_bytes);

}  // namespace objc3::clang_foundation
