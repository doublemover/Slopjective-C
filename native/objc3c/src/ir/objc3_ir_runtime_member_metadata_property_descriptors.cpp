#include "ir/objc3_ir_runtime_member_metadata_emission.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_runtime_member_metadata_symbols.h"
#include "ir/objc3_ir_symbol_model.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

#include "ir/objc3_ir_runtime_member_metadata_property_descriptors_symbols.inc"
#include "ir/objc3_ir_runtime_member_metadata_property_descriptors_constants.inc"
#include "ir/objc3_ir_runtime_member_metadata_property_descriptors_implementation_bindings.inc"
#include "ir/objc3_ir_runtime_member_metadata_property_descriptors_records.inc"
#include "ir/objc3_ir_runtime_member_metadata_property_descriptors_aggregate.inc"

bool EmitObjc3IRRuntimePropertyDescriptorSection(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    std::string &error) {
  Objc3IRRuntimePropertyDescriptorSectionState state;
  state.descriptor_symbols.reserve(
      options.frontend_metadata.runtime_metadata_property_bundles_lexicographic
          .size());

  if (!EmitObjc3IRRuntimePropertyDescriptorRecords(options, family, out,
                                                   retained_globals, state,
                                                   error)) {
    return false;
  }
  EmitObjc3IRRuntimePropertyDescriptorAggregate(
      options.layout_policy, family, state, out, retained_globals);
  return true;
}
