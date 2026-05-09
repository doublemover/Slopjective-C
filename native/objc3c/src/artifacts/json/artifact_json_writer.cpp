#include "artifacts/json/artifact_json_writer.h"

#include <sstream>

#include "io/json/json_writer.h"

namespace objc3::artifacts::json {

std::string RenderArtifactJson(const ArtifactJsonDocument &document) {
  std::ostringstream out;
  objc3::io::json::JsonObjectWriter root(out);
  root.RawJsonField("payload", objc3::io::json::RenderJson(document.payload));
  root.StringField("schema_id", document.schema_id);
  root.End();
  return out.str();
}

}  // namespace objc3::artifacts::json
