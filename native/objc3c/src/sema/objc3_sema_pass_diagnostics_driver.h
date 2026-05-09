#pragma once

#include <cstddef>

#include "sema/objc3_parser_sema_handoff_scaffold.h"
#include "sema/objc3_sema_pass_manager_contract.h"

struct Objc3SemaPassDiagnosticsRun {
  bool pass_order_matches_contract = true;
  std::size_t pass_iteration_count = 0;
  bool diagnostics_after_pass_monotonic = false;
};

Objc3SemaPassDiagnosticsRun RunObjc3SemaDiagnosticsPasses(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffScaffold &handoff,
    Objc3SemaPassManagerResult &result);
