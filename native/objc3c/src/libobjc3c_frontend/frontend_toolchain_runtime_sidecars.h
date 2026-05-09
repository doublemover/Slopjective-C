#pragma once

#include <filesystem>
#include <string>

namespace objc3c::frontend {

bool WriteBackendSidecar(const std::filesystem::path &path,
                         const std::string &contents,
                         std::string &backend_output_error);

}  // namespace objc3c::frontend
