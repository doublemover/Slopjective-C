#pragma once

#include <memory>
#include "clang/AST/ASTConsumer.h"
#include "clang_foundation/compile_session.h"

namespace objc3::clang_foundation {
std::unique_ptr<clang::ASTConsumer> MakeContractConsumer(
    const CompileRequest &request, CompileResult &result);
}
