#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_contract.h"

struct Objc3TypeFormScaffoldSummary {
  std::size_t canonical_reference_form_count = 0;
  std::size_t canonical_message_scalar_form_count = 0;
  std::size_t canonical_bridge_top_form_count = 0;
  bool canonical_reference_forms_unique = false;
  bool canonical_message_scalar_forms_unique = false;
  bool canonical_bridge_top_forms_unique = false;
  bool canonical_message_scalars_disjoint_from_reference = false;
  bool canonical_bridge_top_subset_of_reference = false;
  bool canonical_bridge_top_excludes_sel = false;
  bool canonical_reference_includes_sel = false;
  bool canonical_message_scalars_include_i32 = false;
  bool canonical_message_scalars_include_bool = false;
  bool canonical_forms_exclude_unknown = false;
  bool canonical_bridge_top_matches_reference_without_sel = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  std::string diagnostics_hardening_key;
  bool deterministic = false;
};

Objc3TypeFormScaffoldSummary BuildObjc3TypeFormScaffoldSummary();
bool IsReadyObjc3TypeFormScaffoldSummary(const Objc3TypeFormScaffoldSummary &summary);
