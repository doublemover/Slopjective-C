#include "pipeline/frontend_runtime_metadata_source_record_helpers.h"

#include "pipeline/frontend_runtime_metadata_source_record_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

Objc3RuntimeMetadataSourceRecordSet BuildRuntimeMetadataSourceRecordSet(
    const Objc3Program &program) {
  Objc3RuntimeMetadataSourceRecordSet records;

  RuntimeMetadataPropertySynthesisIndex property_synthesis_index;
  BuildRuntimeMetadataPropertySynthesisIndex(program, property_synthesis_index);
  const RuntimeMetadataClassDispatchProfiles class_dispatch_profiles =
      BuildRuntimeMetadataClassDispatchProfiles(program);

  AppendRuntimeMetadataProtocolRecords(program, property_synthesis_index,
                                       records);
  AppendRuntimeMetadataInterfaceRecords(program, property_synthesis_index,
                                        records);
  AppendRuntimeMetadataImplementationRecords(
      program, property_synthesis_index, class_dispatch_profiles, records);

  SortRuntimeMetadataSourceRecordSet(records);
  records.deterministic =
      IsRuntimeMetadataSourceRecordSetDeterministic(records);
  return records;
}

}  // namespace objc3c::pipeline::orchestration
