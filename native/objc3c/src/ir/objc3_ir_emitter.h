#pragma once

#include <string>

#include "ir/objc3_ir_frontend_metadata.h"

struct Objc3LoweringContract;
struct Objc3Program;

bool EmitObjc3IRText(const Objc3Program &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error);
