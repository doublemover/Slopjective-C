#pragma once

#include <string>

#include "ast/objc3_ast.h"
#include "lower/objc3_lowering_contract.h"

bool EmitObjc3IRText(const Objc3Program &program,
                     const Objc3LoweringContract &lowering_contract,
                     std::string &ir,
                     std::string &error);
