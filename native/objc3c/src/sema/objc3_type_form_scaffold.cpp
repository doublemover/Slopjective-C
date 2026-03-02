#include "sema/objc3_type_form_scaffold.h"

#include <array>

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

}  // namespace

Objc3TypeFormScaffoldSummary BuildObjc3TypeFormScaffoldSummary() {
  Objc3TypeFormScaffoldSummary summary;
  summary.canonical_reference_form_count = kObjc3CanonicalReferenceTypeForms.size();
  summary.canonical_message_scalar_form_count = kObjc3CanonicalScalarMessageSendTypeForms.size();
  summary.canonical_bridge_top_form_count = kObjc3CanonicalBridgeTopReferenceTypeForms.size();
  summary.canonical_reference_forms_unique = HasUniqueValueTypes(kObjc3CanonicalReferenceTypeForms);
  summary.canonical_message_scalar_forms_unique = HasUniqueValueTypes(kObjc3CanonicalScalarMessageSendTypeForms);
  summary.canonical_bridge_top_forms_unique = HasUniqueValueTypes(kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.canonical_bridge_top_subset_of_reference =
      IsSubsetOfCanonicalReferenceForms(kObjc3CanonicalBridgeTopReferenceTypeForms);
  summary.deterministic = summary.canonical_reference_form_count > 0 &&
                          summary.canonical_message_scalar_form_count > 0 &&
                          summary.canonical_bridge_top_form_count > 0 &&
                          summary.canonical_reference_forms_unique &&
                          summary.canonical_message_scalar_forms_unique &&
                          summary.canonical_bridge_top_forms_unique &&
                          summary.canonical_bridge_top_subset_of_reference;
  return summary;
}

bool IsReadyObjc3TypeFormScaffoldSummary(const Objc3TypeFormScaffoldSummary &summary) {
  return summary.canonical_reference_form_count > 0 &&
         summary.canonical_message_scalar_form_count > 0 &&
         summary.canonical_bridge_top_form_count > 0 &&
         summary.canonical_reference_forms_unique &&
         summary.canonical_message_scalar_forms_unique &&
         summary.canonical_bridge_top_forms_unique &&
         summary.canonical_bridge_top_subset_of_reference &&
         summary.deterministic;
}
