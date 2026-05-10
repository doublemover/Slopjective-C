#include "pipeline/objc3_runtime_import_surface.h"

bool IsReadyObjc3ImportedRuntimeModuleSurfaceCrossModuleContract(
    const Objc3ImportedRuntimeModuleSurface &surface) {
  return !surface.source_path.empty() &&
         IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
             surface.frontend_closure_summary) &&
         surface.frontend_closure_summary.ready_for_frontend_module_consumption &&
         surface.frontend_closure_summary.runtime_metadata_source_records_ready;
}
