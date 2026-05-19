#include "pipeline/frontend_runtime_metadata_source_record_helpers_owners.h"

#include <string>

#include "pipeline/frontend_metadata_handoff_helpers.h"

namespace objc3c::pipeline::orchestration {

RuntimeMetadataClassDispatchProfiles BuildRuntimeMetadataClassDispatchProfiles(
    const Objc3Program &program) {
  RuntimeMetadataClassDispatchProfiles class_dispatch_profiles;
  class_dispatch_profiles.reserve(program.interfaces.size());
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      continue;
    }
    class_dispatch_profiles[interface_decl.name] =
        RuntimeMetadataClassDispatchProfile{
            interface_decl.objc_direct_members_declared,
            interface_decl.objc_final_declared,
            interface_decl.objc_sealed_declared};
  }
  return class_dispatch_profiles;
}

void AppendRuntimeMetadataProtocolRecords(
    const Objc3Program &program,
    const RuntimeMetadataPropertySynthesisIndex &property_synthesis_index,
    Objc3RuntimeMetadataSourceRecordSet &records) {
  for (const auto &protocol : program.protocols) {
    Objc3RuntimeMetadataProtocolSourceRecord record;
    record.name = protocol.name;
    record.inherited_protocols_lexicographic =
        protocol.inherited_protocols_lexicographic;
    record.is_forward_declaration = protocol.is_forward_declaration;
    record.property_count = protocol.properties.size();
    record.method_count = protocol.methods.size();
    record.line = protocol.line;
    record.column = protocol.column;
    records.protocols_lexicographic.push_back(record);
    AppendRuntimeMetadataPropertyRecords(protocol.properties, "protocol",
                                         protocol.name,
                                         property_synthesis_index, records);
    AppendRuntimeMetadataMethodRecords(protocol.methods, "protocol",
                                       protocol.name, false, records);
  }
}

void AppendRuntimeMetadataInterfaceRecords(
    const Objc3Program &program,
    const RuntimeMetadataPropertySynthesisIndex &property_synthesis_index,
    Objc3RuntimeMetadataSourceRecordSet &records) {
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "interface";
      record.class_name = interface_decl.name;
      record.category_name = interface_decl.category_name;
      record.adopted_protocols_lexicographic =
          interface_decl.adopted_protocols_lexicographic;
      record.property_count = interface_decl.properties.size();
      record.method_count = interface_decl.methods.size();
      record.line = interface_decl.line;
      record.column = interface_decl.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(interface_decl.name,
                                 interface_decl.category_name);
      AppendRuntimeMetadataPropertyRecords(interface_decl.properties,
                                           "category-interface", owner_name,
                                           property_synthesis_index, records);
      AppendRuntimeMetadataMethodRecords(interface_decl.methods,
                                         "category-interface", owner_name,
                                         false, records);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    record.record_kind = "interface";
    record.name = interface_decl.name;
    record.super_name = interface_decl.super_name;
    record.adopted_protocols_lexicographic =
        interface_decl.adopted_protocols_lexicographic;
    record.has_super = !interface_decl.super_name.empty();
    record.objc_final_declared = interface_decl.objc_final_declared;
    record.objc_sealed_declared = interface_decl.objc_sealed_declared;
    record.property_count = interface_decl.properties.size();
    record.method_count = interface_decl.methods.size();
    record.line = interface_decl.line;
    record.column = interface_decl.column;
    records.classes_lexicographic.push_back(record);
    AppendRuntimeMetadataPropertyRecords(interface_decl.properties,
                                         "class-interface",
                                         interface_decl.name,
                                         property_synthesis_index, records);
    AppendRuntimeMetadataMethodRecords(
        interface_decl.methods, "class-interface", interface_decl.name,
        interface_decl.objc_direct_members_declared, records);
  }
}

void AppendRuntimeMetadataImplementationRecords(
    const Objc3Program &program,
    const RuntimeMetadataPropertySynthesisIndex &property_synthesis_index,
    const RuntimeMetadataClassDispatchProfiles &class_dispatch_profiles,
    Objc3RuntimeMetadataSourceRecordSet &records) {
  for (const auto &implementation : program.implementations) {
    if (implementation.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "implementation";
      record.class_name = implementation.name;
      record.category_name = implementation.category_name;
      record.property_count = implementation.properties.size();
      record.method_count = implementation.methods.size();
      record.line = implementation.line;
      record.column = implementation.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(implementation.name,
                                 implementation.category_name);
      AppendRuntimeMetadataPropertyRecords(
          implementation.properties, "category-implementation", owner_name,
          property_synthesis_index, records);
      AppendRuntimeMetadataMethodRecords(implementation.methods,
                                         "category-implementation",
                                         owner_name, false, records);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    const auto profile_it = class_dispatch_profiles.find(implementation.name);
    record.record_kind = "implementation";
    record.name = implementation.name;
    if (profile_it != class_dispatch_profiles.end()) {
      record.objc_final_declared = profile_it->second.objc_final_declared;
      record.objc_sealed_declared = profile_it->second.objc_sealed_declared;
    }
    record.property_count = implementation.properties.size();
    record.method_count = implementation.methods.size();
    record.line = implementation.line;
    record.column = implementation.column;
    records.classes_lexicographic.push_back(record);
    AppendRuntimeMetadataPropertyRecords(
        implementation.properties, "class-implementation", implementation.name,
        property_synthesis_index, records);
    AppendRuntimeMetadataMethodRecords(
        implementation.methods, "class-implementation", implementation.name,
        profile_it != class_dispatch_profiles.end() &&
            profile_it->second.objc_direct_members_declared,
        records);
  }
}

}  // namespace objc3c::pipeline::orchestration
