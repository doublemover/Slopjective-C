#include "io/objc3_cross_module_imported_modules_document_records.h"

#include <ostream>

#include "io/objc3_cross_module_imported_modules_document_record_header.h"
#include "io/objc3_cross_module_imported_modules_document_record_interop_sections.h"
#include "io/objc3_cross_module_imported_modules_document_record_runtime_sections.h"
#include "io/objc3_cross_module_imported_modules_document_record_storage_sections.h"

void EmitObjc3CrossModuleImportedModuleRecordJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input) {
  EmitObjc3CrossModuleImportedModuleRecordHeaderJson(out, imported_input);
  EmitObjc3CrossModuleImportedModuleRecordRuntimePreludeJson(out,
                                                             imported_input);
  EmitObjc3CrossModuleImportedModuleRecordInteropSectionsJson(out,
                                                              imported_input);
  EmitObjc3CrossModuleImportedModuleRecordMetaprogrammingSectionJson(
      out, imported_input);
  EmitObjc3CrossModuleImportedModuleRecordStorageSectionsJson(out,
                                                              imported_input);
}
