#pragma once

#include <cstddef>
#include <string>
#include <vector>

std::string BuildBlockLiteralCaptureProfile(
    const std::vector<std::string> &capture_names_lexicographic);
std::string BuildBlockCaptureInventoryEntry(const std::string &capture_name);
std::vector<std::string> BuildBlockCaptureInventoryEntriesLexicographic(
    const std::vector<std::string> &capture_names_lexicographic);
std::string BuildBlockCaptureInventoryProfile(
    std::size_t capture_count,
    std::size_t byvalue_readonly_capture_count);
