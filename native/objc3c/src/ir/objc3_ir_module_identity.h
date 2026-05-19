#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

std::string JoinStringParts(const std::vector<std::string> &parts,
                            const std::string &delimiter);
std::uint64_t StableRuntimeMetadataLinkerAnchorHash(
    const std::string &text);
std::string LowerHex64(std::uint64_t value);
std::string MakeModuleIdentifierSafeSuffix(const std::string &text);
std::string EncodeBoundaryTokenValueHex(const std::string &text);
