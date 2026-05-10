  if (!std::is_sorted(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end())) {
    return false;
  }
  if (!std::is_sorted(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),
                       [](const Objc3SemanticFunctionTypeMetadata &lhs, const Objc3SemanticFunctionTypeMetadata &rhs) {
                         return lhs.name < rhs.name;
                       })) {
    return false;
  }
  // Legacy extraction anchor retained for contract tests:
  // return std::all_of(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),
  const bool functions_metadata_rows_named =
      std::all_of(handoff.functions_lexicographic.begin(),
                  handoff.functions_lexicographic.end(),
                  [](const Objc3SemanticFunctionTypeMetadata &fn) { return !fn.name.empty(); });
  if (!functions_metadata_rows_named) {
    return false;
  }
  if (!std::is_sorted(handoff.interfaces_lexicographic.begin(), handoff.interfaces_lexicographic.end(),
                      [](const Objc3SemanticInterfaceTypeMetadata &lhs, const Objc3SemanticInterfaceTypeMetadata &rhs) {
                        return lhs.name < rhs.name;
                      })) {
    return false;
  }
  if (!std::is_sorted(
          handoff.implementations_lexicographic.begin(),
          handoff.implementations_lexicographic.end(),
          [](const Objc3SemanticImplementationTypeMetadata &lhs, const Objc3SemanticImplementationTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return false;
  }
  if (!std::is_sorted(handoff.message_send_selector_lowering_sites_lexicographic.begin(),
                      handoff.message_send_selector_lowering_sites_lexicographic.end(),
                      IsMessageSendSelectorLoweringSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_literal_capture_sites_lexicographic.begin(),
                      handoff.block_literal_capture_sites_lexicographic.end(),
                      IsBlockLiteralCaptureSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_abi_invoke_trampoline_sites_lexicographic.begin(),
                      handoff.block_abi_invoke_trampoline_sites_lexicographic.end(),
                      IsBlockAbiInvokeTrampolineSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_storage_escape_sites_lexicographic.begin(),
                      handoff.block_storage_escape_sites_lexicographic.end(),
                      IsBlockStorageEscapeSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_copy_dispose_sites_lexicographic.begin(),
                      handoff.block_copy_dispose_sites_lexicographic.end(),
                      IsBlockCopyDisposeSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_determinism_perf_baseline_sites_lexicographic.begin(),
                      handoff.block_determinism_perf_baseline_sites_lexicographic.end(),
                      IsBlockDeterminismPerfBaselineSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.autoreleasepool_scope_sites_lexicographic.begin(),
                      handoff.autoreleasepool_scope_sites_lexicographic.end(),
                      IsAutoreleasePoolScopeSiteMetadataLess)) {
    return false;
  }
