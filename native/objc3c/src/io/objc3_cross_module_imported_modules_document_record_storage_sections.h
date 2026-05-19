#pragma once

#include <iosfwd>

#include "io/objc3_process.h"

void EmitObjc3CrossModuleImportedModuleRecordStorageSectionsJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input);
