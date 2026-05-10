#include "artifacts/interop/interop_bridge_artifacts.h"

#include <sstream>

#include "io/json/json_writer.h"

namespace objc3::artifacts::interop {

std::string BuildInteropBridgeArtifactJson(
    const Objc3InteropBridgeArtifactInputs &inputs) {
  std::ostringstream callables;
  objc3::io::json::JsonArrayWriter callable_array(callables);
  for (std::size_t index = 0; index < inputs.foreign_callables.size();
       ++index) {
    const Objc3InteropBridgeCallableArtifact &callable =
        inputs.foreign_callables[index];
    std::ostringstream callable_json;
    objc3::io::json::JsonObjectWriter callable_object(callable_json);
    callable_object.StringField("name", callable.name);
    callable_object.StringField("return_type", callable.return_type);
    callable_object.SizeField("parameter_count", callable.parameters.size());
    callable_object.StringField("objc_import_module_name",
                                callable.objc_import_module_name);
    callable_object.StringField("objc_header_name", callable.objc_header_name);
    callable_object.StringField("objc_cxx_name", callable.objc_cxx_name);
    callable_object.StringField("objc_swift_name", callable.objc_swift_name);
    callable_object.StringField("objc_export_header_name",
                                callable.objc_export_header_name);
    callable_object.SizeField("objc_abi_alignment_bytes",
                              callable.objc_abi_alignment_bytes);
    callable_object.StringField("objc_foreign_type_name",
                                callable.objc_foreign_type_name);
    callable_object.StringField("objc_mixed_image_name",
                                callable.objc_mixed_image_name);
    callable_object.StringField("objc_package_entry_name",
                                callable.objc_package_entry_name);
    callable_object.End();
    callable_array.RawJsonValue(callable_json.str());
  }
  callable_array.End();

  std::ostringstream out;
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("contract_id", inputs.contract_id);
  object.StringField("module_name", inputs.module_name);
  object.StringField("header_artifact_relative_path",
                     inputs.header_artifact_relative_path);
  object.StringField("module_artifact_relative_path",
                     inputs.module_artifact_relative_path);
  object.StringField("bridge_artifact_relative_path",
                     inputs.bridge_artifact_relative_path);
  object.BoolField("runtime_generation_ready",
                   inputs.runtime_generation_ready);
  object.BoolField("cross_module_packaging_ready",
                   inputs.cross_module_packaging_ready);
  object.BoolField("deterministic", inputs.deterministic);
  object.StringField("replay_key", inputs.replay_key);
  object.RawJsonField("foreign_callables", callables.str());
  object.End();
  return out.str();
}

}  // namespace objc3::artifacts::interop
