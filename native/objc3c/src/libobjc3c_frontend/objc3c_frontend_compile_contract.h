#pragma once

/*
 * Internal C++ validation and normalization owner for public compile options.
 * Error text produced here is copied through the public context/result APIs.
 */
#include <filesystem>
#include <string>

#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "libobjc3c_frontend/objc3c_frontend_options.h"

namespace objc3c::frontend {

bool IsMissingFrontendBorrowedText(objc3c_frontend_borrowed_text_t text);

bool IsMissingFrontendBorrowedPath(objc3c_frontend_borrowed_path_t path);

bool ValidateFrontendEmitOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error);

bool ValidateFrontendCompileFileOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error);

bool ValidateFrontendCompileSourceOptions(
    const objc3c_frontend_compile_options_t &options,
    std::string &error);

bool ValidateSupportedFrontendLanguageVersion(uint8_t requested_language_version,
                                              std::string &error);

std::filesystem::path BorrowedFrontendPathToFilesystemPath(
    objc3c_frontend_borrowed_path_t path);

std::filesystem::path OptionalBorrowedFrontendFilesystemPath(
    objc3c_frontend_borrowed_path_t path);

std::filesystem::path ResolveFrontendInputPath(
    const objc3c_frontend_compile_options_t &options);

std::filesystem::path ResolveFrontendOutputDir(
    const objc3c_frontend_compile_options_t &options);

std::string ResolveFrontendEmitPrefix(
    const objc3c_frontend_compile_options_t &options,
    const std::filesystem::path &input_path);

Objc3FrontendOptions BuildFrontendPipelineOptions(
    const objc3c_frontend_compile_options_t &options);

}  // namespace objc3c::frontend
