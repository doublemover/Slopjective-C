#pragma once

#include <iosfwd>
#include <vector>

#include "ast/objc3_ast.h"

namespace objc3::artifacts::json {

void WriteProgramGlobalsManifestArray(
    std::ostream &out, const std::vector<GlobalDecl> &globals,
    const std::vector<int> &resolved_global_values);
void WriteFunctionDeclarationsManifestArray(
    std::ostream &out, const std::vector<const FunctionDecl *> &functions);

}  // namespace objc3::artifacts::json
