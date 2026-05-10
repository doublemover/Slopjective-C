#pragma once

#include <cstddef>

#include "sema/objc3_sema_contract_semantic_type_metadata_handoff.h"

struct Objc3SemanticValidationOptions {
  std::size_t max_message_send_args = 4;
  bool allow_source_only_block_literals = false;
  bool allow_source_only_defer_statements = false;
  bool allow_source_only_error_runtime_surface = false;
  bool arc_mode_enabled = false;
};

bool IsDeterministicSemanticTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff);
