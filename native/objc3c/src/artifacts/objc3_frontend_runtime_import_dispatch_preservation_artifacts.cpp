#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <algorithm>
#include <sstream>

#include "io/objc3_json.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::size_t CountDirectCallableRuntimeMethodRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return static_cast<std::size_t>(std::count_if(
      records.methods_lexicographic.begin(),
      records.methods_lexicographic.end(),
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.effective_direct_dispatch;
      }));
}

std::size_t CountFinalCallableRuntimeMethodRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return static_cast<std::size_t>(std::count_if(
      records.methods_lexicographic.begin(),
      records.methods_lexicographic.end(),
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.objc_final_declared;
      }));
}

std::size_t CountFinalRuntimeClassRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return static_cast<std::size_t>(std::count_if(
      records.classes_lexicographic.begin(),
      records.classes_lexicographic.end(),
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.objc_final_declared;
      }));
}

std::size_t CountSealedRuntimeClassRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return static_cast<std::size_t>(std::count_if(
      records.classes_lexicographic.begin(),
      records.classes_lexicographic.end(),
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.objc_sealed_declared;
      }));
}

}  // namespace

Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
BuildDispatchDispatchMetadataInterfacePreservationSummary(
    const Objc3RuntimeMetadataSourceRecordSet
        &local_runtime_metadata_source_records,
    const std::string &lowering_replay_key,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary summary;
  summary.contract_id =
      kObjc3DispatchDispatchMetadataInterfacePreservationContractId;
  summary.source_contract_id = kObjc3DispatchDispatchControlLoweringContractId;
  summary.surface_path =
      kObjc3DispatchDispatchMetadataInterfacePreservationSurfacePath;
  summary.import_artifact_member_name =
      kObjc3DispatchDispatchMetadataInterfacePreservationImportArtifactMemberName;
  summary.source_model =
      kObjc3DispatchDispatchMetadataInterfacePreservationSourceModel;
  summary.preservation_model =
      kObjc3DispatchDispatchMetadataInterfacePreservationModel;
  summary.fail_closed_model =
      kObjc3DispatchDispatchMetadataInterfacePreservationFailClosedModel;
  summary.lowering_replay_key = lowering_replay_key;
  summary.local_direct_callable_record_count =
      CountDirectCallableRuntimeMethodRecords(local_runtime_metadata_source_records);
  summary.local_final_callable_record_count =
      CountFinalCallableRuntimeMethodRecords(local_runtime_metadata_source_records);
  summary.local_final_container_record_count =
      CountFinalRuntimeClassRecords(local_runtime_metadata_source_records);
  summary.local_sealed_container_record_count =
      CountSealedRuntimeClassRecords(local_runtime_metadata_source_records);
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      IsReadyObjc3RuntimeMetadataSourceRecordSet(
          local_runtime_metadata_source_records);
  summary.deterministic = local_runtime_metadata_source_records.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.dispatch_dispatch_metadata_interface_preservation_present) {
      continue;
    }
    ++summary.imported_module_count;
    summary.imported_direct_callable_record_count +=
        CountDirectCallableRuntimeMethodRecords(
            surface.runtime_metadata_source_records);
    summary.imported_final_callable_record_count +=
        CountFinalCallableRuntimeMethodRecords(
            surface.runtime_metadata_source_records);
    summary.imported_final_container_record_count +=
        CountFinalRuntimeClassRecords(surface.runtime_metadata_source_records);
    summary.imported_sealed_container_record_count +=
        CountSealedRuntimeClassRecords(surface.runtime_metadata_source_records);
    summary.deterministic =
        summary.deterministic && surface.dispatch_deterministic;
  }
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready;
  std::ostringstream replay_key;
  replay_key << Objc3DispatchDispatchMetadataInterfacePreservationSummary()
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";lowering_replay_key=" << lowering_replay_key
             << ";local_direct_callable_record_count="
             << summary.local_direct_callable_record_count
             << ";local_final_callable_record_count="
             << summary.local_final_callable_record_count
             << ";local_final_container_record_count="
             << summary.local_final_container_record_count
             << ";local_sealed_container_record_count="
             << summary.local_sealed_container_record_count
             << ";imported_direct_callable_record_count="
             << summary.imported_direct_callable_record_count
             << ";imported_final_callable_record_count="
             << summary.imported_final_callable_record_count
             << ";imported_final_container_record_count="
             << summary.imported_final_container_record_count
             << ";imported_sealed_container_record_count="
             << summary.imported_sealed_container_record_count;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(summary.lowering_replay_key)
      << "\",\"local_direct_callable_record_count\":"
      << summary.local_direct_callable_record_count
      << ",\"local_final_callable_record_count\":"
      << summary.local_final_callable_record_count
      << ",\"local_final_container_record_count\":"
      << summary.local_final_container_record_count
      << ",\"local_sealed_container_record_count\":"
      << summary.local_sealed_container_record_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_direct_callable_record_count\":"
      << summary.imported_direct_callable_record_count
      << ",\"imported_final_callable_record_count\":"
      << summary.imported_final_callable_record_count
      << ",\"imported_final_container_record_count\":"
      << summary.imported_final_container_record_count
      << ",\"imported_sealed_container_record_count\":"
      << summary.imported_sealed_container_record_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
