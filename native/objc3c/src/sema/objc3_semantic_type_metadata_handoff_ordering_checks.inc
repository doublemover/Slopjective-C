  if (!std::is_sorted(handoff.global_names_lexicographic.begin(),
                      handoff.global_names_lexicographic.end())) {
    return "global names are not lexicographically sorted";
  }
  if (!std::is_sorted(
          handoff.functions_lexicographic.begin(),
          handoff.functions_lexicographic.end(),
          [](const Objc3SemanticFunctionTypeMetadata &lhs,
             const Objc3SemanticFunctionTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return "function metadata rows are not lexicographically sorted";
  }
  if (!std::all_of(handoff.functions_lexicographic.begin(),
                   handoff.functions_lexicographic.end(),
                   [](const Objc3SemanticFunctionTypeMetadata &fn) {
                     return !fn.name.empty();
                   })) {
    return "function metadata row has an empty name";
  }
  if (!std::is_sorted(
          handoff.interfaces_lexicographic.begin(),
          handoff.interfaces_lexicographic.end(),
          [](const Objc3SemanticInterfaceTypeMetadata &lhs,
             const Objc3SemanticInterfaceTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return "interface metadata rows are not lexicographically sorted";
  }
  if (!std::is_sorted(
          handoff.implementations_lexicographic.begin(),
          handoff.implementations_lexicographic.end(),
          [](const Objc3SemanticImplementationTypeMetadata &lhs,
             const Objc3SemanticImplementationTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return "implementation metadata rows are not lexicographically sorted";
  }
  if (!std::is_sorted(handoff.message_send_selector_lowering_sites_lexicographic.begin(),
                      handoff.message_send_selector_lowering_sites_lexicographic.end(),
                      IsMessageSendSelectorLoweringSiteMetadataLess)) {
    return "message-send selector lowering sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_literal_capture_sites_lexicographic.begin(),
                      handoff.block_literal_capture_sites_lexicographic.end(),
                      IsBlockLiteralCaptureSiteMetadataLess)) {
    return "block literal capture sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_abi_invoke_trampoline_sites_lexicographic.begin(),
                      handoff.block_abi_invoke_trampoline_sites_lexicographic.end(),
                      IsBlockAbiInvokeTrampolineSiteMetadataLess)) {
    return "block ABI invoke-trampoline sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_storage_escape_sites_lexicographic.begin(),
                      handoff.block_storage_escape_sites_lexicographic.end(),
                      IsBlockStorageEscapeSiteMetadataLess)) {
    return "block storage-escape sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_copy_dispose_sites_lexicographic.begin(),
                      handoff.block_copy_dispose_sites_lexicographic.end(),
                      IsBlockCopyDisposeSiteMetadataLess)) {
    return "block copy/dispose sites are not sorted";
  }
  if (!std::is_sorted(handoff.block_determinism_perf_baseline_sites_lexicographic.begin(),
                      handoff.block_determinism_perf_baseline_sites_lexicographic.end(),
                      IsBlockDeterminismPerfBaselineSiteMetadataLess)) {
    return "block determinism/perf baseline sites are not sorted";
  }
  if (!std::is_sorted(handoff.autoreleasepool_scope_sites_lexicographic.begin(),
                      handoff.autoreleasepool_scope_sites_lexicographic.end(),
                      IsAutoreleasePoolScopeSiteMetadataLess)) {
    return "autoreleasepool scope sites are not sorted";
  }
