#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"

std::string BuildBlockLiteralAbiLayoutProfile(std::size_t parameter_count,
                                              std::size_t capture_count,
                                              std::size_t body_statement_count);
std::string BuildBlockLiteralAbiDescriptorSymbol(unsigned line,
                                                 unsigned column,
                                                 std::size_t parameter_count,
                                                 std::size_t capture_count);
std::string BuildBlockLiteralInvokeTrampolineSymbol(unsigned line,
                                                    unsigned column,
                                                    std::size_t parameter_count,
                                                    std::size_t capture_count);
std::vector<std::string> BuildBlockInvokeSurfaceEntriesLexicographic(
    const Expr &block);
std::string BuildBlockInvokeSurfaceProfile(const Expr &block);
std::string BuildBlockSourceModelReplayKey(const Expr &block);
