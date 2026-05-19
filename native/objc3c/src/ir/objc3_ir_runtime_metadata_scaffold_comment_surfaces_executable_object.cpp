#include "ir/objc3_ir_runtime_metadata_scaffold_comment_surfaces_executable_object.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_set>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_metadata_scaffold_emission.h"
#include "lower/contracts/executable_object_artifact_layout_contracts.h"
#include "lower/contracts/runtime_object_executable_support_contracts.h"
#include "lower/contracts/runtime_object_realization_contracts.h"
#include "lower/contracts/runtime_object_sample_support_contracts.h"

void EmitObjc3IRRuntimeMetadataScaffoldExecutableObjectCommentSurfaces(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    bool emit_class_metaclass_bundle_payloads,
    bool emit_protocol_category_bundle_payloads,
    bool emit_member_table_payloads, std::ostringstream &out) {
  const Objc3IRFrontendMetadata &frontend_metadata = options.frontend_metadata;
  if (!emit_class_metaclass_bundle_payloads ||
      !emit_protocol_category_bundle_payloads || !emit_member_table_payloads) {
    return;
  }

  std::unordered_set<std::string> implementation_method_owner_identities;
  implementation_method_owner_identities.reserve(
      options.method_definitions.size());
  for (const Objc3IRMethodDefinition &method_def : options.method_definitions) {
    if (method_def.method_owner_identity.empty()) {
      continue;
    }
    implementation_method_owner_identities.insert(
        method_def.method_owner_identity);
  }
  std::size_t executable_method_entry_count = 0;
  std::size_t bound_method_entry_count = 0;
  for (const auto &bundle :
       frontend_metadata.runtime_metadata_method_list_bundles_lexicographic) {
    const bool implementation_owned =
        bundle.owner_kind == "class-implementation" ||
        bundle.owner_kind == "category-implementation";
    if (!implementation_owned) {
      continue;
    }
    for (const auto &entry : bundle.entries_lexicographic) {
      if (!entry.has_body) {
        continue;
      }
      ++executable_method_entry_count;
      if (implementation_method_owner_identities.find(entry.owner_identity) !=
          implementation_method_owner_identities.end()) {
        ++bound_method_entry_count;
      }
    }
  }
  out << "; executable_object_artifact_lowering = "
      << Objc3ExecutableObjectArtifactLoweringSummary()
      << ";class_realization_record_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";category_realization_record_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << ";implementation_method_definition_count="
      << options.method_definitions.size()
      << ";executable_method_entry_count=" << executable_method_entry_count
      << ";bound_method_entry_count=" << bound_method_entry_count
      << ";class_handoff_replay="
      << frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key
      << ";protocol_category_handoff_replay="
      << frontend_metadata.runtime_metadata_protocol_category_typed_handoff_replay_key
      << ";member_table_handoff_replay="
      << frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key
      << "\n";
  out << "; executable_method_body_binding = "
      << Objc3ExecutableMethodBodyBindingSummary()
      << ";implementation_method_definition_count="
      << options.method_definitions.size()
      << ";executable_method_entry_count=" << executable_method_entry_count
      << ";bound_method_entry_count=" << bound_method_entry_count << "\n";
  out << "; executable_realization_records = "
      << Objc3ExecutableRealizationRecordsSummary()
      << ";class_record_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";protocol_record_count="
      << frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size()
      << ";category_record_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << "\n";
  out << "; runtime_class_realization = "
      << Objc3RuntimeClassRealizationSummary()
      << ";class_bundle_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";protocol_record_count="
      << frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size()
      << ";category_record_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << "\n";
  out << "; runtime_metaclass_graph_root_class_baseline = "
      << Objc3RuntimeMetaclassGraphRootClassSummary()
      << ";class_bundle_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";receiver_binding_candidate_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << "\n";
  std::size_t class_protocol_ref_count = 0;
  for (const auto &bundle :
       frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic) {
    class_protocol_ref_count +=
        bundle.adopted_protocol_owner_identities_lexicographic.size();
  }
  std::size_t category_protocol_ref_count = 0;
  for (const auto &bundle :
       frontend_metadata.runtime_metadata_category_bundles_lexicographic) {
    category_protocol_ref_count +=
        bundle.adopted_protocol_owner_identities_lexicographic.size();
  }
  out << "; runtime_category_attachment_protocol_conformance = "
      << Objc3RuntimeCategoryAttachmentProtocolConformanceSummary()
      << ";attached_category_candidate_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << ";class_protocol_ref_count=" << class_protocol_ref_count
      << ";category_protocol_ref_count=" << category_protocol_ref_count
      << "\n";
  out << "; runtime_canonical_runnable_object_sample_support = "
      << Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary()
      << ";class_bundle_count="
      << frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
             .size()
      << ";attached_category_candidate_count="
      << frontend_metadata.runtime_metadata_category_bundles_lexicographic.size()
      << ";builtin_object_sample_selector_count=3\n";
}
