#include "ir/objc3_ir_module_emission_surface.h"

#include "ir/objc3_ir_surface_serialization.h"

void EmitObjc3IRRuntimeDispatchDeclarationSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state, std::ostringstream &out) {
  out << SerializeObjc3IRRuntimeDispatchDeclarationSurface(boundary, state);
}

void EmitObjc3IRSynthesizedAccessorEmissionSurface(
    const Objc3IRSynthesizedAccessorEmissionStats &stats,
    std::ostringstream &out) {
  out << SerializeObjc3IRSynthesizedAccessorEmissionSurface(stats);
}

void EmitObjc3IRMethodDispatchEmissionSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state,
    const Objc3IRMethodDispatchEmissionStats &stats, std::ostringstream &out) {
  out << SerializeObjc3IRMethodDispatchEmissionSurface(boundary, state, stats);
}
