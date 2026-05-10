#pragma once

#include <string>

struct Objc3IRFrontendMetadata;
struct Objc3LoweringContract;
struct Objc3Program;

bool EmitObjc3IRText(const Objc3Program &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error);
