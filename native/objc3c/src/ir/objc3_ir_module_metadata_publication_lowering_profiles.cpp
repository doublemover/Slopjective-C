#include "ir/objc3_ir_module_metadata_publication_lowering_profiles.h"

#include <sstream>

#include "ir/objc3_ir_module_metadata_publication_lowering_profiles_block.h"
#include "ir/objc3_ir_module_metadata_publication_lowering_profiles_error.h"
#include "ir/objc3_ir_module_metadata_publication_lowering_profiles_ownership.h"
#include "ir/objc3_ir_module_metadata_publication_lowering_profiles_type_module.h"

void EmitObjc3IRModuleMetadataLoweringProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata,
    std::ostringstream &out) {
  EmitObjc3IRModuleMetadataOwnershipLoweringProfilePublication(
      frontend_metadata, out);
  EmitObjc3IRModuleMetadataBlockLoweringProfilePublication(
      frontend_metadata, out);
  EmitObjc3IRModuleMetadataTypeModuleLoweringProfilePublication(
      frontend_metadata, out);
  EmitObjc3IRModuleMetadataErrorLoweringProfilePublication(
      frontend_metadata, out);
}
