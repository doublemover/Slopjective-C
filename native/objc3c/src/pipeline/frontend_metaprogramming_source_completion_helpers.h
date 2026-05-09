#pragma once

#include <string>

#include "ast/objc3_ast_declarations.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

inline Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
BuildMetaprogrammingMetaprogrammingSourceClosureSummary(
    const Objc3Program &program) {
  Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary summary;

  for (const auto &fn : program.functions) {
    if (fn.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.objc_derive_declared) {
      ++summary.derive_marker_sites;
    }
    for (const auto &property : interface_decl.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : interface_decl.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property : protocol_decl.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : protocol_decl.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &property : implementation.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : implementation.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }

  summary.derive_marker_source_supported = true;
  summary.macro_marker_source_supported = true;
  summary.property_behavior_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key =
      BuildMetaprogrammingMetaprogrammingSourceClosureReplayKey(summary);
  return summary;
}

inline Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
      summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
    if (decl.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (decl.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (decl.objc_macro_cache_key_declared) {
      ++summary.macro_cache_key_sites;
    }
    if (decl.objc_macro_sandbox_declared) {
      ++summary.macro_sandbox_policy_sites;
    }
    if (decl.objc_macro_declared && decl.objc_macro_package_declared &&
        decl.objc_macro_provenance_declared) {
      ++summary.expansion_visible_macro_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.macro_package_source_supported = true;
  summary.macro_provenance_source_supported = true;
  summary.macro_cache_key_source_supported = true;
  summary.macro_sandbox_policy_source_supported = true;
  summary.expansion_visible_source_supported = true;
  summary.deterministic_handoff =
      summary.expansion_visible_macro_sites <= summary.macro_marker_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_package_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_provenance_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildMetaprogrammingMacroPackageProvenanceSourceCompletionReplayKey(
          summary);
  return summary;
}

inline Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary summary;

  const auto accumulate_property = [&summary](
      const Objc3PropertyDecl &property, const char *owner_kind) {
    if (!property.property_behavior_declared) {
      return;
    }
    ++summary.property_behavior_sites;
    const std::string kind(owner_kind);
    if (kind == "interface") {
      ++summary.interface_property_behavior_sites;
    } else if (kind == "implementation") {
      ++summary.implementation_property_behavior_sites;
    } else if (kind == "protocol") {
      ++summary.protocol_property_behavior_sites;
    }
    if (property.executable_synthesized_binding_kind == "implicit-ivar" &&
        !property.executable_synthesized_binding_symbol.empty()) {
      ++summary.synthesized_binding_visible_sites;
    }
    if (!property.effective_getter_selector.empty()) {
      ++summary.synthesized_getter_visible_sites;
    }
    if (property.effective_setter_available &&
        !property.effective_setter_selector.empty()) {
      ++summary.synthesized_setter_visible_sites;
    }
  };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &property : interface_decl.properties) {
      accumulate_property(property, "interface");
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property : protocol_decl.properties) {
      accumulate_property(property, "protocol");
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &property : implementation.properties) {
      accumulate_property(property, "implementation");
    }
  }

  summary.property_behavior_source_supported = true;
  summary.synthesized_declaration_visibility_supported = true;
  summary.deterministic_handoff =
      summary.interface_property_behavior_sites +
              summary.implementation_property_behavior_sites +
              summary.protocol_property_behavior_sites ==
          summary.property_behavior_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildMetaprogrammingPropertyBehaviorSourceCompletionReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
