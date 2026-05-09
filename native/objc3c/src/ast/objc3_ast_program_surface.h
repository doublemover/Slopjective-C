#pragma once

#include <cstddef>
#include <string>

#include "ast/objc3_ast_declarations.h"

std::size_t Objc3ProgramDeclarationCount(const Objc3Program &program);
std::string Objc3ProgramLoweringReplayKey(const Objc3Program &program);
