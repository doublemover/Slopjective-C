#pragma once

#include <filesystem>
#include <string>

bool TryDeriveObjc3DriverConformanceEmitPrefix(
    const std::filesystem::path &report_path,
    std::string &emit_prefix);

bool TryDeriveObjc3DriverConformancePublicationPath(
    const std::filesystem::path &report_path,
    std::filesystem::path &publication_path);
