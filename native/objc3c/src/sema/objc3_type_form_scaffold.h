#pragma once

#include <cstddef>

#include "sema/objc3_sema_contract.h"

struct Objc3TypeFormScaffoldSummary {
  std::size_t canonical_reference_form_count = 0;
  std::size_t canonical_message_scalar_form_count = 0;
  std::size_t canonical_bridge_top_form_count = 0;
  bool canonical_reference_forms_unique = false;
  bool canonical_message_scalar_forms_unique = false;
  bool canonical_bridge_top_forms_unique = false;
  bool canonical_bridge_top_subset_of_reference = false;
  bool deterministic = false;
};

Objc3TypeFormScaffoldSummary BuildObjc3TypeFormScaffoldSummary();
bool IsReadyObjc3TypeFormScaffoldSummary(const Objc3TypeFormScaffoldSummary &summary);
