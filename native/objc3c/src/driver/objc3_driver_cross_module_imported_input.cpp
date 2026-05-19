#include "driver/objc3_driver_cross_module_imported_input.h"

#include <utility>

#include "driver/objc3_driver_cross_module_imported_input_block.h"
#include "driver/objc3_driver_cross_module_imported_input_core.h"
#include "driver/objc3_driver_cross_module_imported_input_error_concurrency.h"
#include "driver/objc3_driver_cross_module_imported_input_interop.h"
#include "driver/objc3_driver_cross_module_imported_input_metaprogramming.h"
#include "driver/objc3_driver_cross_module_imported_input_storage.h"

void AppendObjc3DriverCrossModuleRuntimeImportedInput(
    Objc3CrossModuleRuntimeLinkPlanArtifactInputs &link_plan_inputs,
    const Objc3ImportedRuntimeModuleSurface &imported_surface,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &peer_artifacts) {
  link_plan_inputs.direct_import_surface_artifact_paths.push_back(
      imported_surface.source_path.generic_string());

  Objc3CrossModuleRuntimeLinkPlanImportedInput imported_input =
      BuildObjc3DriverCrossModuleRuntimeImportedInputCore(imported_surface,
                                                         peer_artifacts);
  PopulateObjc3DriverCrossModuleRuntimeImportedInputErrorAndConcurrency(
      imported_input, imported_surface);
  PopulateObjc3DriverCrossModuleRuntimeImportedInputInterop(imported_input,
                                                           imported_surface);
  PopulateObjc3DriverCrossModuleRuntimeImportedInputMetaprogramming(
      imported_input, imported_surface);
  PopulateObjc3DriverCrossModuleRuntimeImportedInputBlockOwnership(
      imported_input, imported_surface);
  PopulateObjc3DriverCrossModuleRuntimeImportedInputStorageReflection(
      imported_input, imported_surface);

  link_plan_inputs.imported_inputs.push_back(std::move(imported_input));
}
