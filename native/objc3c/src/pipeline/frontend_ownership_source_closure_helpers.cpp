#include "pipeline/frontend_ownership_source_closure_helpers.h"

#include "pipeline/frontend_source_closure_replay_keys.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
BuildOwnershipSystemExtensionSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendOwnershipSystemExtensionSourceClosureSummary summary;

  auto count_borrowed_in_params =
      [&summary](const std::vector<FuncParam> &params) {
        for (const auto &param : params) {
          if (param.borrowed_pointer_qualified) {
            ++summary.borrowed_pointer_sites;
          }
        }
      };

  for (const auto &fn : program.functions) {
    count_borrowed_in_params(fn.params);
    if (fn.return_borrowed_pointer_qualified) {
      ++summary.borrowed_pointer_sites;
    }
    if (fn.objc_returns_borrowed_declared) {
      ++summary.returns_borrowed_attribute_sites;
    }
    for (const auto &stmt : fn.body) {
      detail::CollectOwnershipSystemExtensionStmtSites(stmt.get(), summary);
    }
  }

  auto count_method =
      [&summary, &count_borrowed_in_params](const Objc3MethodDecl &method) {
        count_borrowed_in_params(method.params);
        if (method.return_borrowed_pointer_qualified) {
          ++summary.borrowed_pointer_sites;
        }
        if (method.objc_returns_borrowed_declared) {
          ++summary.returns_borrowed_attribute_sites;
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      count_method(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      count_method(method);
      for (const auto &stmt : method.body) {
        detail::CollectOwnershipSystemExtensionStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.resource_attribute_source_supported = true;
  summary.borrowed_pointer_source_supported = true;
  summary.returns_borrowed_source_supported = true;
  summary.explicit_capture_list_source_supported = true;
  summary.deterministic_handoff =
      summary.resource_close_clause_sites <= summary.resource_attribute_sites &&
      summary.resource_invalid_clause_sites <= summary.resource_attribute_sites &&
      summary.explicit_capture_weak_sites +
              summary.explicit_capture_unowned_sites +
              summary.explicit_capture_move_sites +
              summary.explicit_capture_plain_sites <=
          summary.explicit_capture_item_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildOwnershipSystemExtensionSourceClosureReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
