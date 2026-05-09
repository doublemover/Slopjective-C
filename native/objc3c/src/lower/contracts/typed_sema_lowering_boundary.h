#pragma once

#include "ast/objc3_ast_declarations.h"
#include "lower/contracts/lowering_ownership_contracts.h"

#include <cstddef>
#include <string>

struct Objc3TypedSemaToLoweringBoundary {
  std::string module_name;
  std::size_t global_value_sites = 0;
  std::size_t function_signature_sites = 0;
  std::size_t function_body_sites = 0;
  std::size_t function_param_slots = 0;
  std::size_t method_signature_sites = 0;
  std::size_t method_body_sites = 0;
  std::size_t method_param_slots = 0;
  std::size_t property_sites = 0;
  std::size_t interface_sites = 0;
  std::size_t protocol_sites = 0;
  std::size_t implementation_sites = 0;
  std::size_t concrete_param_type_surfaces = 0;
  std::size_t unknown_param_type_surfaces = 0;
  std::size_t object_reference_type_sites = 0;
  std::size_t runtime_metadata_reference_type_sites = 0;
  std::size_t frontend_diagnostic_sites = 0;
  bool deterministic = false;
  bool all_callable_returns_typed = false;
  bool all_params_have_concrete_type_surface = false;
  bool diagnostics_clear = false;
  std::string typed_semantic_handoff_owner = kObjc3TypedSemanticHandoffOwner;
  std::string strict_contract_owner_model = kObjc3LoweringNoFallbackOwnerModel;
  bool owner_split_explicit = false;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool ready = false;
  std::string replay_key;
};

Objc3TypedSemaToLoweringBoundary Objc3BuildTypedSemaToLoweringBoundary(
    const Objc3Program &program);
bool Objc3TypedSemaToLoweringBoundaryIsReady(
    const Objc3TypedSemaToLoweringBoundary &boundary);
std::string Objc3TypedSemaToLoweringBoundaryReplayKey(
    const Objc3TypedSemaToLoweringBoundary &boundary);
