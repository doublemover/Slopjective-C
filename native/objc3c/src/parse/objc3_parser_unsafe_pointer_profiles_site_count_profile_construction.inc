Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromCounts(
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites) {
  Objc3UnsafePointerExtensionProfile profile;
  profile.unsafe_keyword_sites = unsafe_keyword_sites;
  profile.pointer_arithmetic_sites = pointer_arithmetic_sites;
  profile.raw_pointer_type_sites = raw_pointer_type_sites;
  profile.unsafe_operation_sites =
      pointer_arithmetic_sites + raw_pointer_type_sites;
  profile.unsafe_pointer_extension_sites =
      profile.unsafe_keyword_sites + profile.pointer_arithmetic_sites +
      profile.raw_pointer_type_sites;

  const bool gate_open = unsafe_keyword_sites > 0u;
  profile.gate_blocked_sites =
      gate_open ? 0u
                : (profile.pointer_arithmetic_sites +
                   profile.raw_pointer_type_sites);
  profile.normalized_sites =
      profile.unsafe_pointer_extension_sites - profile.gate_blocked_sites;

  if (profile.unsafe_keyword_sites > profile.unsafe_pointer_extension_sites ||
      profile.pointer_arithmetic_sites >
          profile.unsafe_pointer_extension_sites ||
      profile.raw_pointer_type_sites > profile.unsafe_pointer_extension_sites ||
      profile.unsafe_operation_sites >
          profile.unsafe_pointer_extension_sites ||
      profile.normalized_sites > profile.unsafe_pointer_extension_sites ||
      profile.gate_blocked_sites > profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (!gate_open && profile.normalized_sites != profile.unsafe_keyword_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (gate_open && profile.gate_blocked_sites != 0u) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_unsafe_pointer_extension_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}
