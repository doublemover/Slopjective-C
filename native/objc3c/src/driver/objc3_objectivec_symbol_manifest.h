#pragma once

#include <clang-c/Index.h>

#include <filesystem>
#include <string>
#include <vector>

std::vector<std::string> CollectObjectiveCTranslationUnitDiagnostics(
    CXTranslationUnit translation_unit,
    bool &has_errors);
std::string BuildObjectiveCSymbolManifest(
    const std::filesystem::path &input,
    CXTranslationUnit translation_unit);
