#pragma once

#include <string>

#include "ir/objc3_ir_module_emission_surface.h"

std::string SerializeObjc3IRRuntimeDispatchDeclarationSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state);

std::string SerializeObjc3IRSynthesizedAccessorEmissionSurface(
    const Objc3IRSynthesizedAccessorEmissionStats &stats);

std::string SerializeObjc3IRMethodDispatchEmissionSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state,
    const Objc3IRMethodDispatchEmissionStats &stats);
