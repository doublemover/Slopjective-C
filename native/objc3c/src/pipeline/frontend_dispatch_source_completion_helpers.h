#pragma once

#include <string>
#include <unordered_map>

#include "ast/objc3_ast_declarations.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

inline Objc3FrontendDispatchDispatchIntentSourceClosureSummary
BuildDispatchDispatchIntentSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendDispatchDispatchIntentSourceClosureSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_direct_declared) {
      ++summary.direct_callable_sites;
    }
    if (decl.objc_final_declared) {
      ++summary.final_callable_sites;
    }
    if (decl.objc_dynamic_declared) {
      ++summary.dynamic_callable_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.objc_direct_members_declared) {
      ++summary.direct_members_container_sites;
    }
    if (interface_decl.objc_final_declared) {
      ++summary.final_container_sites;
    }
    if (interface_decl.objc_sealed_declared) {
      ++summary.sealed_container_sites;
    }
    if (interface_decl.is_actor) {
      ++summary.actor_container_sites;
    }
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
  summary.container_annotation_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key =
      BuildDispatchDispatchIntentSourceClosureReplayKey(summary);
  return summary;
}

inline Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
BuildDispatchDispatchIntentSourceCompletionSummary(const Objc3Program &program) {
  Objc3FrontendDispatchDispatchIntentSourceCompletionSummary summary;

  std::unordered_map<std::string, bool> direct_members_by_container;
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.prefixed_dispatch_control_attributes_declared) {
      ++summary.prefixed_container_attribute_sites;
    }
    if (interface_decl.objc_direct_members_declared) {
      ++summary.direct_members_container_sites;
    }
    if (interface_decl.objc_final_declared) {
      ++summary.final_container_sites;
    }
    if (interface_decl.objc_sealed_declared) {
      ++summary.sealed_container_sites;
    }
    direct_members_by_container.emplace(
        interface_decl.name, interface_decl.objc_direct_members_declared);

    for (const auto &method : interface_decl.methods) {
      const bool effective_direct =
          method.objc_direct_declared ||
          (interface_decl.objc_direct_members_declared &&
           !method.objc_dynamic_declared);
      if (effective_direct) {
        ++summary.effective_direct_member_sites;
      }
      if (interface_decl.objc_direct_members_declared &&
          !method.objc_direct_declared && !method.objc_dynamic_declared) {
        ++summary.direct_members_defaulted_method_sites;
      }
      if (interface_decl.objc_direct_members_declared &&
          method.objc_dynamic_declared) {
        ++summary.direct_members_dynamic_opt_out_sites;
      }
    }
  }

  for (const auto &implementation : program.implementations) {
    if (implementation.has_category) {
      continue;
    }
    const auto found = direct_members_by_container.find(implementation.name);
    const bool direct_members_enabled =
        found != direct_members_by_container.end() && found->second;
    for (const auto &method : implementation.methods) {
      const bool effective_direct =
          method.objc_direct_declared ||
          (direct_members_enabled && !method.objc_dynamic_declared);
      if (effective_direct) {
        ++summary.effective_direct_member_sites;
      }
      if (direct_members_enabled && !method.objc_direct_declared &&
          !method.objc_dynamic_declared) {
        ++summary.direct_members_defaulted_method_sites;
      }
      if (direct_members_enabled && method.objc_dynamic_declared) {
        ++summary.direct_members_dynamic_opt_out_sites;
      }
    }
  }

  summary.prefixed_attribute_source_supported = true;
  summary.defaulting_source_supported = true;
  summary.deterministic_handoff =
      summary.direct_members_defaulted_method_sites +
              summary.direct_members_dynamic_opt_out_sites <=
          summary.effective_direct_member_sites +
              summary.direct_members_dynamic_opt_out_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildDispatchDispatchIntentSourceCompletionReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
