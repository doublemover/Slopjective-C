#pragma once

#include <string>

#include "ast/objc3_ast_declarations.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

inline Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
BuildOwnershipRetainableCFamilySourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    for (const std::string &attribute_name :
         decl.retainable_c_family_callable_attributes) {
      if (attribute_name == "objc_family_retain") {
        ++summary.family_retain_sites;
      } else if (attribute_name == "objc_family_release") {
        ++summary.family_release_sites;
      } else if (attribute_name == "objc_family_autorelease") {
        ++summary.family_autorelease_sites;
      } else if (attribute_name == "os_returns_retained" ||
                 attribute_name == "cf_returns_retained" ||
                 attribute_name == "ns_returns_retained") {
        ++summary.compatibility_returns_retained_sites;
      } else if (attribute_name == "os_returns_not_retained" ||
                 attribute_name == "cf_returns_not_retained" ||
                 attribute_name == "ns_returns_not_retained") {
        ++summary.compatibility_returns_not_retained_sites;
      } else if (attribute_name == "os_consumed" ||
                 attribute_name == "cf_consumed" ||
                 attribute_name == "ns_consumed") {
        ++summary.compatibility_consumed_sites;
      }
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
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.callable_annotation_source_supported = true;
  summary.compatibility_alias_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key =
      BuildOwnershipRetainableCFamilySourceCompletionReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
