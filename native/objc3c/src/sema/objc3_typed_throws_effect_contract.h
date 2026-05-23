#pragma once

#include <string>

inline std::string Objc3TypedThrowsKind(bool throws_declared,
                                        bool typed_throws_declared) {
  if (typed_throws_declared) {
    return "typed";
  }
  return throws_declared ? "untyped" : "none";
}

inline std::string Objc3TypedThrowsDeclaredErrorType(
    bool throws_declared,
    bool typed_throws_declared,
    const std::string &typed_error_type_spelling) {
  if (typed_throws_declared) {
    return typed_error_type_spelling;
  }
  return throws_declared ? "id<Error>" : "";
}

inline std::string Objc3BuildTypedThrowsEffectSignatureKey(
    bool throws_declared,
    bool typed_throws_declared,
    const std::string &typed_error_type_spelling) {
  const std::string throws_kind =
      Objc3TypedThrowsKind(throws_declared, typed_throws_declared);
  if (throws_kind == "none") {
    return "throws:none";
  }
  return "throws:" + throws_kind + ":" +
         Objc3TypedThrowsDeclaredErrorType(throws_declared,
                                           typed_throws_declared,
                                           typed_error_type_spelling);
}

inline std::string Objc3BuildTypedThrowsCallableCompatibilityPolicy(
    bool throws_declared,
    bool typed_throws_declared) {
  if (typed_throws_declared) {
    return "typed-throws-exact-payload-match-lowering-deferred";
  }
  return throws_declared ? "untyped-throws-id-error-carrier"
                         : "nonthrowing-only";
}

inline bool Objc3TypedThrowsCallableEffectsCompatible(
    bool lhs_throws_declared,
    bool lhs_typed_throws_declared,
    const std::string &lhs_typed_error_type_spelling,
    bool lhs_typed_throws_abi_lowering_ready,
    bool rhs_throws_declared,
    bool rhs_typed_throws_declared,
    const std::string &rhs_typed_error_type_spelling,
    bool rhs_typed_throws_abi_lowering_ready) {
  return Objc3BuildTypedThrowsEffectSignatureKey(
             lhs_throws_declared,
             lhs_typed_throws_declared,
             lhs_typed_error_type_spelling) ==
             Objc3BuildTypedThrowsEffectSignatureKey(
                 rhs_throws_declared,
                 rhs_typed_throws_declared,
                 rhs_typed_error_type_spelling) &&
         Objc3BuildTypedThrowsCallableCompatibilityPolicy(
             lhs_throws_declared,
             lhs_typed_throws_declared) ==
             Objc3BuildTypedThrowsCallableCompatibilityPolicy(
                 rhs_throws_declared,
                 rhs_typed_throws_declared) &&
         lhs_typed_throws_abi_lowering_ready ==
             rhs_typed_throws_abi_lowering_ready;
}
