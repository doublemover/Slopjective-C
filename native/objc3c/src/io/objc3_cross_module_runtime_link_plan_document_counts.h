#pragma once

#include <cstddef>
#include <iosfwd>

#include "io/objc3_cross_module_runtime_link_plan_sections.h"
#include "io/objc3_process.h"

void EmitObjc3CrossModuleRuntimeLinkPlanDocumentCounts(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const Objc3CrossModuleRuntimeLinkPlanSections &sections,
    std::size_t imported_input_count);
