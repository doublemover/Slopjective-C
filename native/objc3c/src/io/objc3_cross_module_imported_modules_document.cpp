#include "io/objc3_cross_module_imported_modules_document.h"

#include <cstddef>
#include <sstream>

#include "io/objc3_cross_module_imported_modules_document_records.h"

std::string BuildObjc3CrossModuleImportedModulesJson(
    const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
        &imported_inputs) {
  std::ostringstream imported_modules_json;
  imported_modules_json << "[\n";
  for (std::size_t index = 0; index < imported_inputs.size(); ++index) {
    EmitObjc3CrossModuleImportedModuleRecordJson(imported_modules_json,
                                                 imported_inputs[index]);
    imported_modules_json << "\n    }";
    if (index + 1u < imported_inputs.size()) {
      imported_modules_json << ",";
    }
    imported_modules_json << "\n";
  }
  imported_modules_json << "  ]";
  return imported_modules_json.str();
}
