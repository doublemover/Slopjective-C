#include "ir/objc3_ir_module_metadata_publication_advanced_profiles.h"

#include <sstream>

#include "ir/objc3_ir_module_metadata_publication_advanced_profiles_async_actor.h"
#include "ir/objc3_ir_module_metadata_publication_advanced_profiles_interop_meta.h"
#include "ir/objc3_ir_module_metadata_publication_advanced_profiles_ownership.h"
#include "ir/objc3_ir_module_metadata_publication_advanced_profiles_safety.h"

void EmitObjc3IRModuleMetadataAdvancedProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata,
    std::ostringstream &out) {
  EmitObjc3IRModuleMetadataAsyncActorAdvancedProfilePublication(
      frontend_metadata, out);
  EmitObjc3IRModuleMetadataInteropMetaprogrammingAdvancedProfilePublication(
      frontend_metadata, out);
  EmitObjc3IRModuleMetadataOwnershipAdvancedProfilePublication(
      frontend_metadata, out);
  EmitObjc3IRModuleMetadataSafetyAdvancedProfilePublication(
      frontend_metadata, out);
}
