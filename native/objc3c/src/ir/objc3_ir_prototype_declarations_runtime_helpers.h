#pragma once

#include <iosfwd>
#include <string>
#include <unordered_set>

struct Objc3IRPrototypeDeclarationOptions;

bool EmitObjc3IRDeclarationOnce(std::unordered_set<std::string> &declared_symbols,
                                bool &emitted, std::ostringstream &out,
                                const std::string &symbol,
                                const std::string &declaration);

bool Objc3IRRequiresRuntimeHelperDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options);

void EmitObjc3IRRuntimeHelperDeclarations(
    std::unordered_set<std::string> &declared_symbols, bool &emitted,
    std::ostringstream &out);
