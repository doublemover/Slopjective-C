Objc3ArcDiagnosticFixitProfile BuildArcDiagnosticFixitProfile(
    const std::string &spelling,
    bool is_return_type,
    bool is_property_type,
    bool weak_unowned_conflict) {
  Objc3ArcDiagnosticFixitProfile profile;
  if (weak_unowned_conflict) {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = "arc-weak-unowned-conflict";
    profile.fixit_hint = "remove-weak-or-unowned-attribute";
    return profile;
  }

  if (spelling == "__unsafe_unretained") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile =
        is_return_type ? "arc-return-unsafe-unretained"
                       : "arc-unsafe-unretained";
    profile.fixit_hint = is_property_type
                             ? "replace-with-weak-or-strong-attribute"
                             : "replace-with-__weak-or-__strong";
    return profile;
  }

  if (spelling == "__autoreleasing") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile =
        is_return_type ? "arc-return-autoreleasing-transfer"
                       : "arc-autoreleasing-misuse";
    profile.fixit_hint = is_return_type
                             ? "replace-return-qualifier-with-__strong"
                             : "replace-with-__strong-or-out-parameter";
    return profile;
  }

  if (is_return_type && spelling == "__weak") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = "arc-return-weak-escape";
    profile.fixit_hint = "replace-return-qualifier-with-__strong";
  }
  return profile;
}
