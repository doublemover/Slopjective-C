#pragma once

#include <string>

#include "sema/objc3_semantic_integration_surface.h"

std::string NormalizeGenericArgumentTypeSpelling(const std::string &argument);

bool InterfaceAdoptsProtocolDirectly(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &protocol_name);

bool IsSameOrDerivedInterface(const Objc3SemanticIntegrationSurface &surface,
                              const std::string &target_interface_name,
                              const std::string &value_interface_name);
