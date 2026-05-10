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
