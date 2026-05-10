bool IsNSErrorTypeSpelling(const FuncParam &param) {
  if (!param.object_pointer_type_spelling) {
    return false;
  }
  return BuildLowercaseProfileToken(param.object_pointer_type_name) ==
         "nserror";
}

bool IsNSErrorOutParameterSite(const FuncParam &param) {
  if (!IsNSErrorTypeSpelling(param)) {
    return false;
  }
  const std::string lowered_name = BuildLowercaseProfileToken(param.name);
  return param.has_pointer_declarator ||
         lowered_name.find("error") != std::string::npos;
}

Objc3NSErrorBridgingProfile BuildNSErrorBridgingProfileFromParameters(
    const std::vector<FuncParam> &params,
    std::size_t raw_failable_call_sites) {
  Objc3NSErrorBridgingProfile profile;
  for (const auto &param : params) {
    if (IsNSErrorTypeSpelling(param)) {
      profile.ns_error_parameter_sites += 1u;
      if (IsNSErrorOutParameterSite(param)) {
        profile.ns_error_out_parameter_sites += 1u;
      }
    }
  }

  profile.ns_error_bridge_path_sites =
      std::min(profile.ns_error_out_parameter_sites, raw_failable_call_sites);
  profile.normalized_sites =
      profile.ns_error_parameter_sites + profile.ns_error_out_parameter_sites;
  profile.bridge_boundary_sites = profile.ns_error_bridge_path_sites;
  profile.ns_error_bridging_sites =
      profile.normalized_sites + profile.bridge_boundary_sites;
  profile.failable_call_sites =
      std::min(raw_failable_call_sites, profile.ns_error_bridging_sites);

  if (profile.ns_error_out_parameter_sites > profile.ns_error_parameter_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.ns_error_bridge_path_sites >
          profile.ns_error_out_parameter_sites ||
      profile.ns_error_bridge_path_sites > profile.failable_call_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.bridge_boundary_sites !=
      profile.ns_error_bridging_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.ns_error_parameter_sites > profile.ns_error_bridging_sites ||
      profile.ns_error_out_parameter_sites > profile.ns_error_bridging_sites ||
      profile.ns_error_bridge_path_sites > profile.ns_error_bridging_sites ||
      profile.failable_call_sites > profile.ns_error_bridging_sites ||
      profile.normalized_sites > profile.ns_error_bridging_sites ||
      profile.bridge_boundary_sites > profile.ns_error_bridging_sites) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_ns_error_bridging_lowering_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}
