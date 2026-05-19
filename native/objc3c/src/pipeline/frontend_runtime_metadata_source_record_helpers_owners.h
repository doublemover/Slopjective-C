#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "pipeline/frontend_runtime_metadata_source_record_helpers.h"

namespace objc3c::pipeline::orchestration {

struct RuntimeMetadataPropertySynthesisIndex {
  std::unordered_set<std::string> class_implementation_names;
  std::unordered_set<std::string> implementation_property_keys;
};

struct RuntimeMetadataClassDispatchProfile {
  bool objc_direct_members_declared = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
};

using RuntimeMetadataClassDispatchProfiles =
    std::unordered_map<std::string, RuntimeMetadataClassDispatchProfile>;

void BuildRuntimeMetadataPropertySynthesisIndex(
    const Objc3Program &program,
    RuntimeMetadataPropertySynthesisIndex &property_synthesis_index);

void AppendRuntimeMetadataPropertyRecords(
    const std::vector<Objc3PropertyDecl> &properties,
    const std::string &owner_kind,
    const std::string &owner_name,
    const RuntimeMetadataPropertySynthesisIndex &property_synthesis_index,
    Objc3RuntimeMetadataSourceRecordSet &records);

void AppendRuntimeMetadataMethodRecords(
    const std::vector<Objc3MethodDecl> &methods,
    const std::string &owner_kind,
    const std::string &owner_name,
    bool direct_members_declared,
    Objc3RuntimeMetadataSourceRecordSet &records);

RuntimeMetadataClassDispatchProfiles BuildRuntimeMetadataClassDispatchProfiles(
    const Objc3Program &program);

void AppendRuntimeMetadataProtocolRecords(
    const Objc3Program &program,
    const RuntimeMetadataPropertySynthesisIndex &property_synthesis_index,
    Objc3RuntimeMetadataSourceRecordSet &records);

void AppendRuntimeMetadataInterfaceRecords(
    const Objc3Program &program,
    const RuntimeMetadataPropertySynthesisIndex &property_synthesis_index,
    Objc3RuntimeMetadataSourceRecordSet &records);

void AppendRuntimeMetadataImplementationRecords(
    const Objc3Program &program,
    const RuntimeMetadataPropertySynthesisIndex &property_synthesis_index,
    const RuntimeMetadataClassDispatchProfiles &class_dispatch_profiles,
    Objc3RuntimeMetadataSourceRecordSet &records);

void SortRuntimeMetadataSourceRecordSet(
    Objc3RuntimeMetadataSourceRecordSet &records);

bool IsRuntimeMetadataSourceRecordSetDeterministic(
    const Objc3RuntimeMetadataSourceRecordSet &records);

}  // namespace objc3c::pipeline::orchestration
