#include "lower/contracts/typed_sema_lowering_boundary_counts.h"

#include "ast/objc3_ast_type_surface.h"

#include <cstddef>
#include <vector>

namespace {

void AddValueTypeBoundarySite(ValueType type,
                              Objc3TypedSemaToLoweringBoundary &boundary) {
  if (Objc3ValueTypeIsObjectReference(type)) {
    ++boundary.object_reference_type_sites;
  }
  if (Objc3ValueTypeIsRuntimeMetadataReference(type)) {
    ++boundary.runtime_metadata_reference_type_sites;
  }
}

void AddParamBoundarySites(const std::vector<FuncParam> &params,
                           Objc3TypedSemaToLoweringBoundary &boundary,
                           std::size_t &slot_count) {
  slot_count += params.size();
  for (const FuncParam &param : params) {
    if (Objc3FuncParamHasConcreteTypeSurface(param)) {
      ++boundary.concrete_param_type_surfaces;
    } else {
      ++boundary.unknown_param_type_surfaces;
    }
    AddValueTypeBoundarySite(param.type, boundary);
  }
}

void AddMethodBoundary(const Objc3MethodDecl &method, bool has_body,
                       Objc3TypedSemaToLoweringBoundary &boundary) {
  ++boundary.method_signature_sites;
  if (has_body) {
    ++boundary.method_body_sites;
  }
  AddValueTypeBoundarySite(method.return_type, boundary);
  AddParamBoundarySites(method.params, boundary, boundary.method_param_slots);
  if (method.return_type == ValueType::Unknown) {
    boundary.all_callable_returns_typed = false;
  }
}

void AddPropertyBoundary(const std::vector<Objc3PropertyDecl> &properties,
                         Objc3TypedSemaToLoweringBoundary &boundary) {
  boundary.property_sites += properties.size();
  for (const Objc3PropertyDecl &property : properties) {
    AddValueTypeBoundarySite(property.type, boundary);
  }
}

}  // namespace

void Objc3PopulateTypedSemaToLoweringBoundaryCounts(
    const Objc3Program &program,
    Objc3TypedSemaToLoweringBoundary &boundary) {
  boundary.global_value_sites = program.globals.size();
  boundary.interface_sites = program.interfaces.size();
  boundary.protocol_sites = program.protocols.size();
  boundary.implementation_sites = program.implementations.size();
  boundary.frontend_diagnostic_sites = program.diagnostics.size();
  boundary.all_callable_returns_typed = true;

  for (const FunctionDecl &function : program.functions) {
    ++boundary.function_signature_sites;
    if (!function.is_prototype) {
      ++boundary.function_body_sites;
    }
    AddValueTypeBoundarySite(function.return_type, boundary);
    AddParamBoundarySites(function.params, boundary,
                          boundary.function_param_slots);
    if (function.return_type == ValueType::Unknown) {
      boundary.all_callable_returns_typed = false;
    }
  }

  for (const Objc3ProtocolDecl &protocol : program.protocols) {
    AddPropertyBoundary(protocol.properties, boundary);
    for (const Objc3MethodDecl &method : protocol.methods) {
      AddMethodBoundary(method, false, boundary);
    }
  }
  for (const Objc3InterfaceDecl &interface_decl : program.interfaces) {
    AddPropertyBoundary(interface_decl.properties, boundary);
    for (const Objc3MethodDecl &method : interface_decl.methods) {
      AddMethodBoundary(method, false, boundary);
    }
  }
  for (const Objc3ImplementationDecl &implementation : program.implementations) {
    AddPropertyBoundary(implementation.properties, boundary);
    for (const Objc3MethodDecl &method : implementation.methods) {
      AddMethodBoundary(method, method.has_body, boundary);
    }
  }
}
