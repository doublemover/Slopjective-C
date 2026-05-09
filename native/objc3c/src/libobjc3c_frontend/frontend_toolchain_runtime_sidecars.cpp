#include "libobjc3c_frontend/frontend_toolchain_runtime_sidecars.h"

#include "libobjc3c_frontend/objc3c_frontend_file_output.h"

namespace objc3c::frontend {

bool WriteBackendSidecar(const std::filesystem::path &path,
                         const std::string &contents,
                         std::string &backend_output_error) {
  return WriteFrontendTextFile(path, contents, backend_output_error);
}

}  // namespace objc3c::frontend
