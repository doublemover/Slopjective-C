#include "sema/objc3_semantic_type_predicates.h"

#include "sema/objc3_type_form_scaffold.h"
#include "support/objc3_type_profile_helpers.h"

bool IsCanonicalObjc3TypeFormScaffoldReady() {
  static const Objc3TypeFormScaffoldSummary summary =
      BuildObjc3TypeFormScaffoldSummary();
  return IsReadyObjc3TypeFormScaffoldSummary(summary);
}

bool IsUnknownSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector && info.type == ValueType::Unknown;
}

bool IsCallableSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector && info.type == ValueType::Function && info.is_callable;
}

bool IsScalarSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector;
}

bool IsScalarBoolCompatibleType(const SemanticTypeInfo &info) {
  return !info.is_vector &&
         (info.type == ValueType::Bool || info.type == ValueType::I32);
}

bool IsScalarI32CompatibleType(const SemanticTypeInfo &info) {
  return !info.is_vector &&
         (info.type == ValueType::I32 ||
          objc3c::support::IsObjCReferenceAliasValueType(info.type));
}

bool AreScalarI32AliasCompatible(const SemanticTypeInfo &lhs,
                                 const SemanticTypeInfo &rhs) {
  return IsScalarI32CompatibleType(lhs) && IsScalarI32CompatibleType(rhs);
}

bool IsObjCReferenceValueType(ValueType type) {
  if (!IsCanonicalObjc3TypeFormScaffoldReady()) {
    return false;
  }
  return IsObjc3CanonicalReferenceTypeForm(type);
}

bool IsObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector &&
         (IsObjCReferenceValueType(info.type) ||
          objc3c::support::IsObjCReferenceAliasValueType(info.type));
}

bool IsNullableObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         !info.is_refined_nonnull_reference &&
         (info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::Nullable ||
          info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::ImplicitlyUnwrapped ||
          (info.has_nullability_suffix &&
           info.canonical_type.nullability ==
               Objc3SemanticCanonicalNullability::Unspecified));
}

bool IsNonnullDestinationObjCReferenceSemanticType(
    const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         (info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::Nonnull ||
          info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::ImplicitlyUnwrapped ||
          info.canonical_type.nullability ==
              Objc3SemanticCanonicalNullability::NullResettable);
}

bool IsVoidSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector && info.type == ValueType::Void;
}

SemanticTypeInfo MakeNonnullRefinedSemanticType(
    const SemanticTypeInfo &info) {
  SemanticTypeInfo refined = info;
  if (IsObjCReferenceSemanticType(refined)) {
    refined.has_nullability_suffix = false;
    refined.is_refined_nonnull_reference = true;
    refined.canonical_type.nullability =
        Objc3SemanticCanonicalNullability::Nonnull;
  }
  return refined;
}

bool IsOwnedObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         info.ownership_kind == SemanticOwnershipKind::Retained;
}

bool IsWeakObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         info.ownership_kind == SemanticOwnershipKind::Weak;
}

bool IsUnownedObjCReferenceSemanticType(const SemanticTypeInfo &info) {
  return IsObjCReferenceSemanticType(info) &&
         info.ownership_kind == SemanticOwnershipKind::Unowned;
}

bool IsMessageCompatibleType(const SemanticTypeInfo &info) {
  if (!IsCanonicalObjc3TypeFormScaffoldReady()) {
    return false;
  }
  return !info.is_vector && IsObjc3CanonicalMessageSendTypeForm(info.type);
}
