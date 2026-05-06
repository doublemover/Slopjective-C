#include "artifacts/json/artifact_json_writer.h"

#include <utility>

#include "io/json/json_writer.h"

namespace objc3::artifacts::json {

std::string RenderArtifactJson(const ArtifactJsonDocument &document) {
  objc3::io::json::JsonValue::Object root;
  root.emplace("payload", document.payload);
  root.emplace("schema_id", objc3::io::json::JsonValue::String(document.schema_id));
  return objc3::io::json::RenderJson(objc3::io::json::JsonValue::ObjectValue(std::move(root)));
}

}  // namespace objc3::artifacts::json
