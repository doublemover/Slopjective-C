#pragma once

#include <string>
#include <string_view>

namespace objc3::artifacts::frontend {

struct RuntimeStatePublicationPaths {
  std::string emit_prefix = "module";
  std::string compile_manifest_artifact = "module.manifest.json";
  std::string object_artifact = "module.obj";
  std::string backend_artifact = "module.ll";
};

RuntimeStatePublicationPaths BuildRuntimeStatePublicationPaths(
    std::string_view registration_manifest_artifact);

}  // namespace objc3::artifacts::frontend
