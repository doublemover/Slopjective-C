Objc3WeakUnownedLifetimeProfile BuildWeakUnownedLifetimeProfile(
    const std::string &spelling,
    bool prefer_safe_unowned) {
  Objc3WeakUnownedLifetimeProfile profile;
  if (spelling == "__weak") {
    profile.is_weak_reference = true;
    profile.lifetime_profile = "weak";
    profile.runtime_hook_profile = "objc-weak-side-table";
  } else if (spelling == "__unsafe_unretained") {
    profile.is_unowned_reference = true;
    profile.is_unowned_safe_reference = prefer_safe_unowned;
    profile.lifetime_profile =
        prefer_safe_unowned ? "unowned-safe" : "unowned-unsafe";
    profile.runtime_hook_profile = prefer_safe_unowned
                                       ? "objc-unowned-safe-guard"
                                       : "objc-unowned-unsafe-direct";
  } else if (spelling == "__strong") {
    profile.lifetime_profile = "strong-owned";
  } else if (spelling == "__autoreleasing") {
    profile.lifetime_profile = "autoreleasing";
  }
  return profile;
}

Objc3WeakUnownedLifetimeProfile BuildPropertyWeakUnownedLifetimeProfile(
    const Objc3PropertyDecl &property) {
  if (property.is_weak) {
    return BuildWeakUnownedLifetimeProfile("__weak", false);
  }
  if (property.is_unowned) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", true);
  }
  if (property.is_unsafe_unretained) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", false);
  }
  if (!property.ownership_qualifier_spelling.empty()) {
    return BuildWeakUnownedLifetimeProfile(
        property.ownership_qualifier_spelling,
        false);
  }
  if (property.is_assign) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", false);
  }
  return Objc3WeakUnownedLifetimeProfile{};
}
