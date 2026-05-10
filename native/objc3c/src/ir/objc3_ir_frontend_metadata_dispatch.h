#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_dispatch_execution.h"

struct Objc3IRFrontendDispatchMetadata
    : Objc3IRFrontendDispatchExecutionMetadata {
  std::string lowering_property_synthesis_ivar_binding_replay_key;
  std::size_t lowering_property_synthesis_sites = 0;
  std::size_t lowering_property_synthesis_explicit_ivar_bindings = 0;
  std::size_t lowering_property_synthesis_default_ivar_bindings = 0;
  std::size_t lowering_interface_owned_property_synthesis_sites = 0;
  std::size_t lowering_implementation_property_redeclaration_sites = 0;
  std::size_t lowering_property_synthesis_ivar_binding_resolved = 0;
  bool lowering_property_synthesis_deterministic_handoff = false;
  std::string lowering_id_class_sel_object_pointer_typecheck_replay_key;
  std::size_t id_typecheck_sites = 0;
  std::size_t class_typecheck_sites = 0;
  std::size_t sel_typecheck_sites = 0;
  std::size_t object_pointer_typecheck_sites = 0;
  std::size_t id_class_sel_object_pointer_typecheck_sites_total = 0;
  bool deterministic_id_class_sel_object_pointer_typecheck_handoff = false;
};
