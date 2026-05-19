#pragma once

#include "sema/objc3_sema_parser_handoff_publication.h"
#include "sema/objc3_sema_pass_diagnostics_driver.h"
#include "sema/objc3_sema_pass_manager_contract.h"

void PublishObjc3SemaPassFlowSummary(
    const Objc3SemaParserHandoffPublication &handoff_publication,
    const Objc3SemaPassDiagnosticsRun &diagnostics_run,
    Objc3SemaPassManagerResult &result);
