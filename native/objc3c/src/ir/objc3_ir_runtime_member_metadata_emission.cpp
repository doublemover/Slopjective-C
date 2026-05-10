#include "ir/objc3_ir_runtime_member_metadata_emission.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <sstream>
#include <utility>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_member_metadata_symbols.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

#include "ir/objc3_ir_runtime_member_metadata_method_lists.inc"
#include "ir/objc3_ir_runtime_member_metadata_ivar_records.inc"
#include "ir/objc3_ir_runtime_member_metadata_ivar_tables.inc"

bool EmitObjc3IRRuntimeMethodListBundlesForFamily(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    std::string &error) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  for (std::size_t bundle_index = 0;
       bundle_index <
       frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
           .size();
       ++bundle_index) {
    if (!EmitObjc3IRRuntimeMemberMetadataMethodListBundle(
            options, family, bundle_index, out, retained_globals, error)) {
      return false;
    }
  }
  return true;
}

void EmitObjc3IRRuntimeIvarDescriptorSection(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  Objc3IRRuntimeIvarDescriptorSectionState state;
  state.descriptor_symbols.reserve(
      options.frontend_metadata.runtime_metadata_ivar_bundles_lexicographic
          .size());

  EmitObjc3IRRuntimeIvarDescriptorRecords(options, family, out,
                                          retained_globals, state);
  EmitObjc3IRRuntimeIvarLayoutTables(options.layout_policy, family, out,
                                     retained_globals, state);
  EmitObjc3IRRuntimeIvarAggregate(options.layout_policy, family, out,
                                  retained_globals, state);
}
