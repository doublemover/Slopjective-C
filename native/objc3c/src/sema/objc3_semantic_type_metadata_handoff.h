#pragma once

#include <string>

#include "sema/objc3_sema_contract.h"

Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(
    const Objc3SemanticIntegrationSurface &surface);

bool IsDeterministicSemanticTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff);

std::string ExplainNonDeterministicSemanticTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff);
