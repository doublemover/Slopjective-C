#pragma once

#include <cstddef>
#include <string>

struct Objc3IdClassSelObjectPointerTypeCheckingSummary {
  std::size_t canonical_reference_form_count = 0;
  std::size_t canonical_message_scalar_form_count = 0;
  std::size_t canonical_bridge_top_form_count = 0;
  std::size_t param_type_sites = 0;
  std::size_t param_id_spelling_sites = 0;
  std::size_t param_class_spelling_sites = 0;
  std::size_t param_sel_spelling_sites = 0;
  std::size_t param_instancetype_spelling_sites = 0;
  std::size_t param_object_pointer_type_sites = 0;
  std::size_t return_type_sites = 0;
  std::size_t return_id_spelling_sites = 0;
  std::size_t return_class_spelling_sites = 0;
  std::size_t return_sel_spelling_sites = 0;
  std::size_t return_instancetype_spelling_sites = 0;
  std::size_t return_object_pointer_type_sites = 0;
  std::size_t property_type_sites = 0;
  std::size_t property_id_spelling_sites = 0;
  std::size_t property_class_spelling_sites = 0;
  std::size_t property_sel_spelling_sites = 0;
  std::size_t property_instancetype_spelling_sites = 0;
  std::size_t property_object_pointer_type_sites = 0;
  bool canonical_reference_forms_unique = false;
  bool canonical_message_scalar_forms_unique = false;
  bool canonical_bridge_top_forms_unique = false;
  bool canonical_bridge_top_subset_of_reference = false;
  bool canonical_type_form_scaffold_ready = false;
  bool canonical_type_form_diagnostics_hardening_consistent = false;
  bool canonical_type_form_diagnostics_hardening_ready = false;
  std::string canonical_type_form_diagnostics_hardening_key;
  bool canonical_type_form_recovery_determinism_consistent = false;
  bool canonical_type_form_recovery_determinism_ready = false;
  std::string canonical_type_form_recovery_determinism_key;
  bool canonical_type_form_conformance_matrix_consistent = false;
  bool canonical_type_form_conformance_matrix_ready = false;
  std::string canonical_type_form_conformance_matrix_key;
  std::size_t canonical_type_form_conformance_corpus_case_count = 0;
  std::size_t canonical_type_form_conformance_corpus_passed_case_count = 0;
  std::size_t canonical_type_form_conformance_corpus_failed_case_count = 0;
  bool canonical_type_form_conformance_corpus_consistent = false;
  bool canonical_type_form_conformance_corpus_ready = false;
  std::string canonical_type_form_conformance_corpus_key;
  std::size_t canonical_type_form_performance_quality_required_guardrail_count = 0;
  std::size_t canonical_type_form_performance_quality_passed_guardrail_count = 0;
  std::size_t canonical_type_form_performance_quality_failed_guardrail_count = 0;
  bool canonical_type_form_performance_quality_guardrails_consistent = false;
  bool canonical_type_form_performance_quality_guardrails_ready = false;
  std::string canonical_type_form_performance_quality_guardrails_key;
  bool deterministic = true;
};
