#include "ir/objc3_ir_runtime_object_metadata_emission.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_object_metadata_symbols.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

#include "objc3_ir_runtime_object_metadata_class_record_state.inc"
#include "objc3_ir_runtime_object_metadata_class_record_method_refs.inc"
#include "objc3_ir_runtime_object_metadata_class_record_edges.inc"
#include "objc3_ir_runtime_object_metadata_class_record_serialization.inc"
#include "objc3_ir_runtime_object_metadata_class_record_aggregate.inc"

void EmitObjc3IRRuntimeClassMetaclassBundleSection(
    const Objc3IRRuntimeObjectMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;

  const auto descriptor_symbols_by_owner_identity =
      BuildObjc3IRRuntimeClassMetaclassDescriptorSymbolsByOwnerIdentity(
          frontend_metadata, layout_policy, family);

  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(
      frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
          .size());
  for (std::size_t i = 0;
       i < frontend_metadata
               .runtime_metadata_class_metaclass_bundles_lexicographic.size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
            [i];
    const Objc3IRRuntimeClassMetaclassRecordSymbols symbols =
        BuildObjc3IRRuntimeClassMetaclassRecordSymbols(layout_policy, family,
                                                       bundle, i);
    const Objc3IRRuntimeClassMetaclassMethodListReferences method_refs =
        ResolveObjc3IRRuntimeClassMetaclassMethodListReferences(options, family,
                                                                bundle);
    const Objc3IRRuntimeClassMetaclassEdgeReferences edge_refs =
        ResolveObjc3IRRuntimeClassMetaclassEdgeReferences(
            bundle, descriptor_symbols_by_owner_identity);

    descriptor_symbols.push_back(symbols.descriptor_symbol);
    EmitObjc3IRRuntimeClassMetaclassIdentityStrings(bundle, family, symbols,
                                                    out);
    EmitObjc3IRRuntimeClassMetaclassMethodListReferences(
        family, symbols, method_refs, out);
    EmitObjc3IRRuntimeClassMetaclassAdoptedProtocolRefs(options, family, bundle,
                                                        symbols, out);
    EmitObjc3IRRuntimeClassMetaclassDescriptorRecord(bundle, family, symbols,
                                                     edge_refs, out);
    RetainObjc3IRRuntimeObjectMetadataGlobal(retained_globals,
                                             symbols.descriptor_symbol);
  }

  EmitObjc3IRRuntimeClassMetaclassAggregate(layout_policy, family,
                                            descriptor_symbols, out,
                                            retained_globals);
}
