#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <sstream>

#include "artifacts/objc3_frontend_runtime_metadata_source_graph_json_families.h"

namespace objc3::artifacts::frontend {

std::string BuildExecutableMetadataSourceGraphJson(
    const Objc3ExecutableMetadataSourceGraph &graph) {
  std::ostringstream out;
  out << "{";
  runtime_metadata_source_graph_json::WriteSourceGraphIdentityJsonFields(out,
                                                                        graph);
  runtime_metadata_source_graph_json::WriteSourceGraphNodeCountJsonFields(out,
                                                                         graph);
  runtime_metadata_source_graph_json::WriteSourceGraphClosureJsonFields(out,
                                                                       graph);
  runtime_metadata_source_graph_json::WriteSourceGraphClassMetaclassNodeJsonFields(
      out, graph);
  runtime_metadata_source_graph_json::WriteSourceGraphProtocolCategoryNodeJsonFields(
      out, graph);
  runtime_metadata_source_graph_json::WriteSourceGraphMemberNodeJsonFields(out,
                                                                          graph);
  runtime_metadata_source_graph_json::WriteSourceGraphOwnerEdgeReadinessJsonFields(
      out, graph);
  out << "}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
