#include "pipeline/runtime_import_link_plan.h"

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline {

bool RuntimeImportPathEndsWith(const std::string &text,
                               const std::string &suffix) {
  return text.size() >= suffix.size() &&
         text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool TryResolveRuntimeImportEmitPrefixFromSurfacePath(
    const std::filesystem::path &path,
    std::string &emit_prefix,
    std::string &error) {
  const std::string filename = path.filename().generic_string();
  const std::string suffix =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactSuffix;
  if (!RuntimeImportPathEndsWith(filename, suffix) ||
      filename.size() <= suffix.size()) {
    error = "import surface path does not end with the canonical artifact suffix";
    return false;
  }
  emit_prefix = filename.substr(0, filename.size() - suffix.size());
  if (emit_prefix.empty()) {
    error = "import surface path does not contain an emit prefix";
    return false;
  }
  return true;
}

}  // namespace objc3c::pipeline
