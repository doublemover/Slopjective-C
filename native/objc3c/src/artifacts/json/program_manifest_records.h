#pragma once

#include <iosfwd>

#include "ast/objc3_ast.h"

namespace objc3::artifacts::json {

void WriteProgramGlobalManifestRecord(std::ostream &out,
                                      const GlobalDecl &global,
                                      int resolved_value);
void WriteProgramFunctionManifestRecord(std::ostream &out,
                                        const FunctionDecl &function);

}  // namespace objc3::artifacts::json
