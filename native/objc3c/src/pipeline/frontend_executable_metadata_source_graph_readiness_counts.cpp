#include "pipeline/frontend_executable_metadata_source_graph_readiness_owners.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"

namespace objc3c::pipeline::orchestration {

ExecutableMetadataSourceGraphReadinessCounts
BuildExecutableMetadataSourceGraphReadinessCounts(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const Objc3ExecutableMetadataSourceGraph &graph) {
  const std::size_t expected_class_interface_count = static_cast<std::size_t>(
      std::count_if(program.interfaces.begin(), program.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  const std::size_t expected_class_implementation_count =
      static_cast<std::size_t>(
          std::count_if(program.implementations.begin(),
                        program.implementations.end(),
                        [](const Objc3ImplementationDecl &decl) {
                          return !decl.has_category;
                        }));

  std::vector<std::string> category_record_owner_names;
  category_record_owner_names.reserve(
      runtime_metadata_source_records.categories_lexicographic.size());
  for (const auto &record :
       runtime_metadata_source_records.categories_lexicographic) {
    category_record_owner_names.push_back(
        BuildCategoryOwnerName(record.class_name, record.category_name));
  }
  std::sort(category_record_owner_names.begin(),
            category_record_owner_names.end());
  category_record_owner_names.erase(
      std::unique(category_record_owner_names.begin(),
                  category_record_owner_names.end()),
      category_record_owner_names.end());

  ExecutableMetadataSourceGraphReadinessCounts readiness_counts;
  readiness_counts.interface_count_aligned =
      graph.interface_nodes_lexicographic.size() ==
      expected_class_interface_count;
  readiness_counts.implementation_count_aligned =
      graph.implementation_nodes_lexicographic.size() ==
      expected_class_implementation_count;
  readiness_counts.metaclass_count_aligned =
      graph.metaclass_nodes_lexicographic.size() ==
      graph.interface_nodes_lexicographic.size();
  readiness_counts.class_node_floor_satisfied =
      graph.class_nodes_lexicographic.size() >=
          graph.interface_nodes_lexicographic.size() &&
      graph.class_nodes_lexicographic.size() >=
          graph.implementation_nodes_lexicographic.size();
  readiness_counts.protocol_count_aligned =
      graph.protocol_nodes_lexicographic.size() ==
      runtime_metadata_source_records.protocols_lexicographic.size();
  readiness_counts.category_count_aligned =
      graph.category_nodes_lexicographic.size() ==
      category_record_owner_names.size();
  readiness_counts.property_count_aligned =
      graph.property_nodes_lexicographic.size() ==
      runtime_metadata_source_records.properties_lexicographic.size();
  readiness_counts.method_count_aligned =
      graph.method_nodes_lexicographic.size() ==
      runtime_metadata_source_records.methods_lexicographic.size();
  readiness_counts.ivar_count_aligned =
      graph.ivar_nodes_lexicographic.size() ==
      runtime_metadata_source_records.ivars_lexicographic.size();
  return readiness_counts;
}

}  // namespace objc3c::pipeline::orchestration
