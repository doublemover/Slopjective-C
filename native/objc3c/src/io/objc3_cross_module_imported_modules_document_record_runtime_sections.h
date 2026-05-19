#pragma once

#include <iosfwd>

#include "io/objc3_process.h"

void EmitObjc3CrossModuleImportedModuleRecordRuntimePreludeJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input);

void EmitObjc3CrossModuleImportedModuleRecordMetaprogrammingSectionJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input);
