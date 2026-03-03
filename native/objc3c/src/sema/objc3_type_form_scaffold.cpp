#include "sema/objc3_type_form_scaffold.h"

#include <array>
#include <string>

namespace {

template <std::size_t N>
bool HasUniqueValueTypes(const std::array<ValueType, N> &forms) {
  for (std::size_t i = 0; i < forms.size(); ++i) {
    for (std::size_t j = i + 1; j < forms.size(); ++j) {
      if (forms[i] == forms[j]) {
        return false;
      }
    }
  }
  return true;
}

template <std::size_t N>
bool IsSubsetOfCanonicalReferenceForms(const std::array<ValueType, N> &forms) {
  for (const ValueType form : forms) {
    if (!IsObjc3CanonicalReferenceTypeForm(form)) {
      return false;
    }
  }
  return true;
}

template <std::size_t N>
bool ContainsValueType(const std::array<ValueType, N> &forms, ValueType type) {
  for (const ValueType candidate : forms) {
    if (candidate == type) {
      return true;
    }
  }
  return false;
}

template <std::size_t N>
bool AreDisjointFromCanonicalReferenceForms(const std::array<ValueType, N> &forms) {
  for (const ValueType form : forms) {
    if (IsObjc3CanonicalReferenceTypeForm(form)) {
      return false;
    }
  }
  return true;
}

template <std::size_t N>
bool ExcludesUnknownValueType(const std::array<ValueType, N> &forms) {
  for (const ValueType form : forms) {
    if (form == ValueType::Unknown) {
      return false;
    }
  }
  return true;
}

template <std::size_t ReferenceN, std::size_t BridgeTopN>
bool IsCanonicalBridgeTopEquivalentToReferenceWithoutSel(const std::array<ValueType, ReferenceN> &reference_forms,
                                                         const std::array<ValueType, BridgeTopN> &bridge_top_forms) {
  std::size_t reference_without_sel_count = 0;
  for (const ValueType reference_form : reference_forms) {
    if (reference_form == ValueType::ObjCSel) {
      continue;
    }
    ++reference_without_sel_count;
    if (!ContainsValueType(bridge_top_forms, reference_form)) {
      return false;
    }
  }
  return reference_without_sel_count == bridge_top_forms.size();
}

inline const char *BoolKey(bool value) {
  return value ? "1" : "0";
}

}  // namespace

Objc3TypeFormScaffoldSummary BuildObjc3TypeFormScaffoldSummary() {
  Objc3TypeFormScaffoldSummary summary;
  summary.canonical_reference_form_count = kObjc3CanonicalReferenceTypeForms.size();
  summary.canonical_message_scalar_form_count = kObjc3CanonicalScalarMessageSendTypeForms.size();
  summary.canonical_bridge_top_form_count = kObjc3CanonicalBridgeTopReferenceTypeForms.size();
  summary.canonical_reference_forms_unique = HasUniqueValueTypes(kObjc3CanonicalReferenceTypeForms);
  summary.canonical_message_scalar_forms_unique = HasUniqueValueTypes(kObjc3CanonicalScalarMessageSendTypeForms);
  summary.canonical_bridge_top_forms_unique = HasUniqueValueTypes(kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.canonical_message_scalars_disjoint_from_reference =
      AreDisjointFromCanonicalReferenceForms(kObjc3CanonicalScalarMessageSendTypeForms);
  summary.canonical_bridge_top_subset_of_reference =
      IsSubsetOfCanonicalReferenceForms(kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.canonical_bridge_top_excludes_sel =
      !ContainsValueType(kObjc3CanonicalBridgeTopReferenceTypeForms, ValueType::ObjCSel);
  summary.canonical_reference_includes_sel =
      ContainsValueType(kObjc3CanonicalReferenceTypeForms, ValueType::ObjCSel);
  summary.canonical_message_scalars_include_i32 =
      ContainsValueType(kObjc3CanonicalScalarMessageSendTypeForms, ValueType::I32);
  summary.canonical_message_scalars_include_bool =
      ContainsValueType(kObjc3CanonicalScalarMessageSendTypeForms, ValueType::Bool);
  summary.canonical_forms_exclude_unknown =
      ExcludesUnknownValueType(kObjc3CanonicalReferenceTypeForms) &&
      ExcludesUnknownValueType(kObjc3CanonicalScalarMessageSendTypeForms) &&
      ExcludesUnknownValueType(kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.canonical_bridge_top_matches_reference_without_sel =
      IsCanonicalBridgeTopEquivalentToReferenceWithoutSel(kObjc3CanonicalReferenceTypeForms,
                                                          kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.diagnostics_hardening_consistent =
      summary.canonical_reference_includes_sel &&
      summary.canonical_message_scalars_include_i32 &&
      summary.canonical_message_scalars_include_bool &&
      summary.canonical_forms_exclude_unknown &&
      summary.canonical_bridge_top_matches_reference_without_sel;
  summary.diagnostics_hardening_key = std::string("type-form-diagnostics-hardening;ref-sel=") +
                                      BoolKey(summary.canonical_reference_includes_sel) +
                                      ";msg-i32=" + BoolKey(summary.canonical_message_scalars_include_i32) +
                                      ";msg-bool=" + BoolKey(summary.canonical_message_scalars_include_bool) +
                                      ";exclude-unknown=" + BoolKey(summary.canonical_forms_exclude_unknown) +
                                      ";bridge-eq-ref-minus-sel=" +
                                      BoolKey(summary.canonical_bridge_top_matches_reference_without_sel);
  summary.diagnostics_hardening_ready =
      summary.diagnostics_hardening_consistent && !summary.diagnostics_hardening_key.empty();
  summary.recovery_determinism_consistent =
      summary.diagnostics_hardening_consistent &&
      summary.diagnostics_hardening_ready &&
      !summary.diagnostics_hardening_key.empty();
  summary.recovery_determinism_key = std::string("type-form-recovery-determinism;diagnostics-consistent=") +
                                     BoolKey(summary.diagnostics_hardening_consistent) +
                                     ";diagnostics-ready=" + BoolKey(summary.diagnostics_hardening_ready) +
                                     ";diagnostics-key-ready=" +
                                     BoolKey(!summary.diagnostics_hardening_key.empty());
  summary.recovery_determinism_ready =
      summary.recovery_determinism_consistent &&
      !summary.recovery_determinism_key.empty();
  summary.conformance_matrix_consistent =
      summary.recovery_determinism_consistent &&
      summary.recovery_determinism_ready &&
      !summary.recovery_determinism_key.empty() &&
      summary.canonical_reference_form_count == kObjc3CanonicalReferenceTypeForms.size() &&
      summary.canonical_message_scalar_form_count == kObjc3CanonicalScalarMessageSendTypeForms.size() &&
      summary.canonical_bridge_top_form_count == kObjc3CanonicalBridgeTopReferenceTypeForms.size() &&
      summary.canonical_reference_forms_unique &&
      summary.canonical_message_scalar_forms_unique &&
      summary.canonical_bridge_top_forms_unique &&
      summary.canonical_message_scalars_disjoint_from_reference &&
      summary.canonical_bridge_top_subset_of_reference &&
      summary.canonical_bridge_top_excludes_sel &&
      summary.canonical_reference_includes_sel &&
      summary.canonical_message_scalars_include_i32 &&
      summary.canonical_message_scalars_include_bool &&
      summary.canonical_forms_exclude_unknown &&
      summary.canonical_bridge_top_matches_reference_without_sel;
  summary.conformance_matrix_key = std::string("type-form-conformance-matrix;recovery-consistent=") +
                                   BoolKey(summary.recovery_determinism_consistent) +
                                   ";recovery-ready=" + BoolKey(summary.recovery_determinism_ready) +
                                   ";recovery-key-ready=" + BoolKey(!summary.recovery_determinism_key.empty()) +
                                   ";reference-forms=" + std::to_string(summary.canonical_reference_form_count) +
                                   ";message-scalars=" + std::to_string(summary.canonical_message_scalar_form_count) +
                                   ";bridge-top-forms=" + std::to_string(summary.canonical_bridge_top_form_count);
  summary.conformance_matrix_ready =
      summary.conformance_matrix_consistent &&
      !summary.conformance_matrix_key.empty();
  summary.deterministic = summary.canonical_reference_form_count > 0 &&
                          summary.canonical_message_scalar_form_count > 0 &&
                          summary.canonical_bridge_top_form_count > 0 &&
                          summary.canonical_reference_forms_unique &&
                          summary.canonical_message_scalar_forms_unique &&
                          summary.canonical_bridge_top_forms_unique &&
                          summary.canonical_message_scalars_disjoint_from_reference &&
                          summary.canonical_bridge_top_subset_of_reference &&
                          summary.canonical_bridge_top_excludes_sel &&
                          summary.canonical_reference_includes_sel &&
                          summary.canonical_message_scalars_include_i32 &&
                          summary.canonical_message_scalars_include_bool &&
                          summary.canonical_forms_exclude_unknown &&
                          summary.canonical_bridge_top_matches_reference_without_sel &&
                          summary.diagnostics_hardening_consistent &&
                          summary.diagnostics_hardening_ready &&
                          !summary.diagnostics_hardening_key.empty() &&
                          summary.recovery_determinism_consistent &&
                          summary.recovery_determinism_ready &&
                          !summary.recovery_determinism_key.empty() &&
                          summary.conformance_matrix_consistent &&
                          summary.conformance_matrix_ready &&
                          !summary.conformance_matrix_key.empty();
  return summary;
}

bool IsReadyObjc3TypeFormScaffoldSummary(const Objc3TypeFormScaffoldSummary &summary) {
  return summary.canonical_reference_form_count > 0 &&
         summary.canonical_message_scalar_form_count > 0 &&
         summary.canonical_bridge_top_form_count > 0 &&
         summary.canonical_reference_forms_unique &&
         summary.canonical_message_scalar_forms_unique &&
         summary.canonical_bridge_top_forms_unique &&
         summary.canonical_message_scalars_disjoint_from_reference &&
         summary.canonical_bridge_top_subset_of_reference &&
         summary.canonical_bridge_top_excludes_sel &&
         summary.canonical_reference_includes_sel &&
         summary.canonical_message_scalars_include_i32 &&
         summary.canonical_message_scalars_include_bool &&
         summary.canonical_forms_exclude_unknown &&
         summary.canonical_bridge_top_matches_reference_without_sel &&
         summary.diagnostics_hardening_consistent &&
         summary.diagnostics_hardening_ready &&
         !summary.diagnostics_hardening_key.empty() &&
         summary.recovery_determinism_consistent &&
         summary.recovery_determinism_ready &&
         !summary.recovery_determinism_key.empty() &&
         summary.conformance_matrix_consistent &&
         summary.conformance_matrix_ready &&
         !summary.conformance_matrix_key.empty() &&
         summary.deterministic;
}
