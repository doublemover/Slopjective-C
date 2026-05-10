#pragma once

#include "sema/objc3_sema_contract_semantic_type_metadata_handoff.h"

struct Objc3SemanticIntegrationSurface;

Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(
    const Objc3SemanticIntegrationSurface &surface);
