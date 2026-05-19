#include "pipeline/runtime_import_preservation_owners.h"

#include <string>
#include <utility>

#include "pipeline/runtime_import_json_helpers.h"
#include "support/objc3_file_reading.h"

namespace {

using JsonParser = objc3c::pipeline::RuntimeImportJsonParser;
using JsonValue = objc3c::pipeline::RuntimeImportJsonValue;
using objc3c::pipeline::AsObject;

}  // namespace

bool TryLoadObjc3ImportedRuntimeModuleSurface(
    const std::filesystem::path &path,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  std::string io_error;
  std::string payload;
  if (!objc3c::support::TryReadTextFile(path, payload, io_error, "unable to open file", "failed to read file")) {
    error = path.generic_string() + ": " + io_error;
    return false;
  }

  JsonParser parser(payload);
  JsonValue root_value;
  std::string parse_error;
  if (!parser.Parse(root_value, parse_error)) {
    error = path.generic_string() + ": " + parse_error;
    return false;
  }
  const JsonValue::Object *root_object = AsObject(root_value);
  if (root_object == nullptr) {
    error = path.generic_string() + ": import surface payload must be a JSON object";
    return false;
  }

  Objc3ImportedRuntimeModuleSurface parsed_surface;
  parsed_surface.source_path = path;
  if (!objc3c::pipeline::runtime_import_preservation::
          ParseImportedRuntimeModuleSurface(*root_object, parsed_surface,
                                            parse_error)) {
    error = path.generic_string() + ": " + parse_error;
    return false;
  }
  if (!objc3c::pipeline::runtime_import_preservation::
          PublishImportedRuntimeModuleSurfaceReadiness(path, parsed_surface,
                                                       error)) {
    return false;
  }
  surface = std::move(parsed_surface);
  return true;
}

bool TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  return objc3c::pipeline::runtime_import_preservation::
      LoadImportedRuntimeModulePackagingPeerArtifacts(surface, artifacts, error);
}
