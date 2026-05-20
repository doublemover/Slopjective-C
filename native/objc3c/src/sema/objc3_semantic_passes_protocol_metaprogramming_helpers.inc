struct ProtocolCompositionInfo {
  bool has_protocol_composition = false;
  std::vector<std::string> names_lexicographic;
  bool has_invalid_protocol_composition = false;
};

static std::string NormalizeMetaprogrammingRawDeriveToken(std::string token) {
  token = objc3c::support::TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string NormalizeMetaprogrammingRawMacroToken(std::string token) {
  token = objc3c::support::TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string NormalizeMetaprogrammingRawPropertyBehaviorToken(std::string token) {
  token = objc3c::support::TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string BuildMetaprogrammingLowercaseDeriveToken(std::string token) {
  token = NormalizeMetaprogrammingRawDeriveToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static std::string NormalizeMetaprogrammingSupportedDeriveName(
    const std::string &derive_name) {
  const std::string lowered = BuildMetaprogrammingLowercaseDeriveToken(derive_name);
  if (lowered == "equatable" || lowered == "equality") {
    return "Equality";
  }
  if (lowered == "hash") {
    return "Hash";
  }
  if (lowered == "debugdescription") {
    return "DebugDescription";
  }
  return "";
}

static std::string BuildMetaprogrammingLowercasePropertyBehaviorToken(std::string token) {
  token = NormalizeMetaprogrammingRawPropertyBehaviorToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static std::string NormalizeMetaprogrammingSupportedPropertyBehaviorName(
    const std::string &behavior_name) {
  const std::string lowered =
      BuildMetaprogrammingLowercasePropertyBehaviorToken(behavior_name);
  if (lowered == "observed") {
    return "Observed";
  }
  if (lowered == "projected") {
    return "Projected";
  }
  return "";
}

static std::string BuildMetaprogrammingLowercaseMacroToken(std::string token) {
  token = NormalizeMetaprogrammingRawMacroToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static bool IsSupportedMetaprogrammingMacroPackageName(const std::string &package_name) {
  const std::string lowered = BuildMetaprogrammingLowercaseMacroToken(package_name);
  return lowered == "std.metaprogramming" ||
         lowered.rfind("std.metaprogramming.", 0) == 0u;
}

static bool IsSupportedMetaprogrammingMacroProvenanceToken(
    const std::string &provenance_name) {
  const std::string normalized =
      NormalizeMetaprogrammingRawMacroToken(provenance_name);
  const std::string lowered = BuildMetaprogrammingLowercaseMacroToken(provenance_name);
  if (normalized != lowered) {
    return false;
  }
  constexpr std::string_view prefix = "sha256:";
  if (lowered.rfind(prefix, 0) != 0u) {
    return false;
  }
  const std::string_view digest = std::string_view(lowered).substr(prefix.size());
  if (digest.size() < 6u || digest.size() > 64u) {
    return false;
  }
  return std::all_of(digest.begin(), digest.end(), [](char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
  });
}

static bool IsSupportedMetaprogrammingMacroCacheKeyToken(
    const std::string &cache_key_name) {
  const std::string normalized =
      NormalizeMetaprogrammingRawMacroToken(cache_key_name);
  if (normalized.size() < 3u || normalized.size() > 160u ||
      normalized.find("..") != std::string::npos) {
    return false;
  }
  return std::all_of(normalized.begin(), normalized.end(), [](char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') || c == '.' || c == ':' || c == '_' ||
           c == '-';
  });
}

static bool IsSupportedMetaprogrammingMacroSandboxPolicy(
    const std::string &sandbox_policy_name) {
  return BuildMetaprogrammingLowercaseMacroToken(sandbox_policy_name) ==
         "deterministic";
}

static std::string BuildMetaprogrammingDerivedSelectorForInventoryRow(
    const std::string &canonical_derive_name) {
  if (canonical_derive_name == "Equality") {
    return "isEqual:";
  }
  if (canonical_derive_name == "Hash") {
    return "hash";
  }
  if (canonical_derive_name == "DebugDescription") {
    return "debugDescription";
  }
  return "";
}
