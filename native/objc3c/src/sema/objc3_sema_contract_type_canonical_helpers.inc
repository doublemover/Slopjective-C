inline bool IsObjc3CanonicalReferenceTypeForm(ValueType type) {
  for (const ValueType candidate : kObjc3CanonicalReferenceTypeForms) {
    if (candidate == type) {
      return true;
    }
  }
  return false;
}

inline bool IsObjc3CanonicalMessageSendTypeForm(ValueType type) {
  for (const ValueType candidate : kObjc3CanonicalScalarMessageSendTypeForms) {
    if (candidate == type) {
      return true;
    }
  }
  return IsObjc3CanonicalReferenceTypeForm(type);
}

inline bool IsObjc3CanonicalBridgeTopReferenceTypeForm(ValueType type) {
  for (const ValueType candidate : kObjc3CanonicalBridgeTopReferenceTypeForms) {
    if (candidate == type) {
      return true;
    }
  }
  return false;
}
