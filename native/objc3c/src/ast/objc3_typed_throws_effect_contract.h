#pragma once

#include <cctype>
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
    return "typed-throws-exact-payload-match-error-out-abi";
  }
  return throws_declared ? "untyped-throws-id-error-carrier"
                         : "nonthrowing-only";
}

inline bool Objc3TypedThrowsAbiLoweringReady(
    bool throws_declared,
    bool typed_throws_declared,
    const std::string &typed_error_type_spelling) {
  if (!throws_declared) {
    return false;
  }
  if (!typed_throws_declared) {
    return true;
  }
  return !typed_error_type_spelling.empty();
}

inline std::string Objc3TypedThrowsAbiStatus(
    bool throws_declared,
    bool typed_throws_declared,
    const std::string &typed_error_type_spelling) {
  if (!throws_declared) {
    return "none";
  }
  if (!typed_throws_declared) {
    return "untyped-error-out-abi";
  }
  return Objc3TypedThrowsAbiLoweringReady(throws_declared,
                                          typed_throws_declared,
                                          typed_error_type_spelling)
             ? "typed-error-out-abi"
             : "typed-error-abi-unavailable";
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

inline std::string Objc3TypedThrowsPayloadIdentity(
    const std::string &type_spelling) {
  std::string normalized;
  normalized.reserve(type_spelling.size());
  for (unsigned char ch : type_spelling) {
    if (std::isspace(ch) || ch == '*' || ch == '?') {
      continue;
    }
    normalized.push_back(
        static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
  }
  return normalized;
}

inline bool Objc3TypedThrowsIsIdErrorCarrier(
    const std::string &type_spelling) {
  return Objc3TypedThrowsPayloadIdentity(type_spelling) == "id<error>";
}

inline bool Objc3TypedThrowsIsUnsupportedForeignCarrier(
    const std::string &type_spelling) {
  const std::string identity = Objc3TypedThrowsPayloadIdentity(type_spelling);
  return identity.find("foreign") != std::string::npos ||
         identity.find("cxx") != std::string::npos ||
         identity.find("swift") != std::string::npos ||
         identity.find("exception") != std::string::npos;
}

inline bool Objc3TypedThrowsPayloadsExactlyMatch(
    const std::string &throw_type_spelling,
    const std::string &catch_type_spelling) {
  const std::string throw_identity =
      Objc3TypedThrowsPayloadIdentity(throw_type_spelling);
  const std::string catch_identity =
      Objc3TypedThrowsPayloadIdentity(catch_type_spelling);
  return !throw_identity.empty() && throw_identity == catch_identity;
}

inline std::string Objc3TypedThrowsCatchMatchStatus(
    const std::string &throw_type_spelling,
    const std::string &catch_type_spelling,
    bool bridge_to_id_error_allowed) {
  if (Objc3TypedThrowsIsUnsupportedForeignCarrier(throw_type_spelling) ||
      Objc3TypedThrowsIsUnsupportedForeignCarrier(catch_type_spelling)) {
    return "unsupported-foreign-carrier-fail-closed";
  }
  if (Objc3TypedThrowsPayloadsExactlyMatch(throw_type_spelling,
                                           catch_type_spelling)) {
    return "typed-catch-exact-payload-match";
  }
  if (bridge_to_id_error_allowed &&
      Objc3TypedThrowsIsIdErrorCarrier(catch_type_spelling)) {
    return "untyped-catch-allowed-via-id-error-bridge";
  }
  return "incompatible-catch-rejected";
}

inline std::string Objc3TypedThrowsCatchBridgeStatus(
    const std::string &throw_type_spelling,
    const std::string &catch_type_spelling,
    bool bridge_to_id_error_allowed) {
  const std::string match_status = Objc3TypedThrowsCatchMatchStatus(
      throw_type_spelling, catch_type_spelling, bridge_to_id_error_allowed);
  if (match_status == "typed-catch-exact-payload-match") {
    return "no-bridge-needed";
  }
  if (match_status == "untyped-catch-allowed-via-id-error-bridge") {
    return "bridge-to-id<Error>-allowed";
  }
  if (match_status == "unsupported-foreign-carrier-fail-closed") {
    return "unsupported-foreign-carrier-rejected";
  }
  return "bridge-rejected";
}

inline int Objc3TypedThrowsRuntimeCatchKind(
    const std::string &throw_type_spelling,
    const std::string &catch_type_spelling,
    bool bridge_to_id_error_allowed) {
  const std::string match_status = Objc3TypedThrowsCatchMatchStatus(
      throw_type_spelling, catch_type_spelling, bridge_to_id_error_allowed);
  if (match_status == "typed-catch-exact-payload-match") {
    return 4;
  }
  if (match_status == "untyped-catch-allowed-via-id-error-bridge") {
    return 2;
  }
  return 0;
}
