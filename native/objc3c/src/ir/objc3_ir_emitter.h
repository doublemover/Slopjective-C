#pragma once

#include <string>

#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_parser_contract.h"

bool EmitObjc3IRText(const Objc3ParsedProgram &program,
                     const Objc3LoweringContract &lowering_contract,
                     std::string &ir,
                     std::string &error);
