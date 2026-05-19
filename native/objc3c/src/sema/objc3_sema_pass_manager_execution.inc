#include "sema/objc3_sema_pass_manager.h"

#include "sema/objc3_parser_sema_handoff_scaffold.h"
#include "sema/objc3_sema_control_flow_equivalence.h"
#include "sema/objc3_sema_dispatch_ownership_equivalence.h"
#include "sema/objc3_sema_parser_handoff_publication.h"
#include "sema/objc3_sema_pass_block_equivalence.h"
#include "sema/objc3_sema_pass_diagnostics_driver.h"
#include "sema/objc3_sema_pass_flow_publication.h"
#include "sema/objc3_sema_pass_flow_scaffold.h"
#include "sema/objc3_sema_symbol_binding_equivalence.h"
#include "sema/objc3_sema_type_summary_equivalence.h"
#include "sema/objc3_semantic_passes.h"

Objc3SemaPassManagerResult RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input) {
#include "sema/objc3_sema_pass_manager_execution_setup.inc"
#include "sema/objc3_sema_pass_manager_execution_diagnostics.inc"
#include "sema/objc3_sema_pass_manager_execution_type_metadata.inc"
#include "sema/objc3_sema_pass_manager_execution_module_error_concurrency.inc"
