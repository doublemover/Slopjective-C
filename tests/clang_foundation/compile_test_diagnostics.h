#pragma once
#include "clang_foundation/compile_session.h"
#include <iostream>

namespace objc3::clang_foundation::test {
inline void ReportFailure(const CompileResult &result) {
  if(result.success)return;
  std::cerr << result.failure << '\n';
  for(const auto &diagnostic:result.diagnostics)
    std::cerr << diagnostic.spelling.file << ':' << diagnostic.spelling.line
              << ": " << diagnostic.message << '\n';
  for(const auto &diagnostic:result.interface_import_diagnostics)
    std::cerr << diagnostic.code << ": " << diagnostic.message << '\n';
  if(result.interface_artifact)
    for(const auto &diagnostic:result.interface_artifact->diagnostics)
      std::cerr << diagnostic.code << ": " << diagnostic.message << " [" << diagnostic.subject << "]\n";
}
} // namespace objc3::clang_foundation::test
