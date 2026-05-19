void AppendObjc3FrontendArtifactManifestLoweringHeader(
    std::ostream &manifest,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactManifestLoweringHeaderFields
        &lowering_header_fields) {
  manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\""
           << options.lowering.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":"
           << options.lowering.max_message_send_args
           << ",\"selector_global_ordering\":\"lexicographic\"},\n";
  manifest << "  \"lowering_vector_abi\":{\"replay_key\":\""
           << kObjc3ArtifactSimdVectorTypeLoweringReplayKey
           << "\",\"lane_contract\":\"" << kObjc3ArtifactSimdVectorLaneContract
           << "\",\"vector_signature_functions\":"
           << lowering_header_fields.vector_signature_functions
           << "},\n";
  manifest << "  \"lowering_property_synthesis_ivar_binding\":{\"replay_key\":\""
           << lowering_header_fields.property_synthesis_ivar_binding_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ArtifactPropertySynthesisIvarBindingLaneContract
           << "\",\"deterministic_handoff\":"
           << (lowering_header_fields.property_synthesis_ivar_binding_deterministic
                   ? "true"
                   : "false")
           << "},\n";
}
