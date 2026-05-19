#include "sema/objc3_type_form_scaffold.h"

#include <array>
#include <string>

namespace {

#include "sema/objc3_type_form_scaffold_helpers.inc"

}  // namespace

Objc3TypeFormScaffoldSummary BuildObjc3TypeFormScaffoldSummary() {
  Objc3TypeFormScaffoldSummary summary;
#include "sema/objc3_type_form_scaffold_canonical_forms.inc"
#include "sema/objc3_type_form_scaffold_diagnostics_recovery.inc"
#include "sema/objc3_type_form_scaffold_conformance.inc"
#include "sema/objc3_type_form_scaffold_performance.inc"
  return summary;
}

bool IsReadyObjc3TypeFormScaffoldSummary(const Objc3TypeFormScaffoldSummary &summary) {
#include "sema/objc3_type_form_scaffold_readiness.inc"
}
