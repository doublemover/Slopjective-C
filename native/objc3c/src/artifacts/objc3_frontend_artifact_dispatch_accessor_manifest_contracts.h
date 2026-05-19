#pragma once

#include <cstddef>
#include <string>

namespace objc3::artifacts::frontend {

inline constexpr const char *kDispatchAccessorLoweringSurfaceContractId =
    "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1";
inline constexpr const char *kDispatchAccessorRuntimeAbiSurfaceContractId =
    "objc3c.runtime.dispatch_accessor.abi.surface.v1";
inline constexpr const char *kDispatchAccessorRuntimeAbiBoundaryModel =
    "public-dispatch-entrypoint-plus-private-testing-snapshot-and-property-helper-surface";

inline constexpr const char *kDispatchAccessorRuntimeRetainI32Symbol =
    "objc3_runtime_retain_i32";
inline constexpr const char *kDispatchAccessorRuntimeReleaseI32Symbol =
    "objc3_runtime_release_i32";
inline constexpr const char *kDispatchAccessorRuntimeAutoreleaseI32Symbol =
    "objc3_runtime_autorelease_i32";

struct Objc3DispatchAndSynthesizedAccessorLoweringFields {
  std::string runtime_dispatch_symbol;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  bool runtime_dispatch_symbol_matches_lowering = false;
  std::size_t live_runtime_dispatch_sites = 0;
  std::size_t direct_dispatch_sites = 0;
  std::size_t message_send_sites = 0;
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t interface_owned_property_synthesis_sites = 0;
  std::size_t implementation_property_redeclaration_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  bool deterministic_handoff = false;
};

struct Objc3DispatchAccessorRuntimeAbiFields {
  std::string runtime_dispatch_symbol;
  bool deterministic = false;
};

}  // namespace objc3::artifacts::frontend
