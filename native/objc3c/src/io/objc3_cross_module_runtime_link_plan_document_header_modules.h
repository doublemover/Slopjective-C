#pragma once

#include <iosfwd>

#include "io/objc3_cross_module_runtime_link_plan_sections.h"

void EmitObjc3CrossModuleRuntimeLinkPlanHeaderModuleSections(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanSections &sections);
