  summary.diagnostics_hardening_consistent =
      summary.canonical_reference_includes_sel &&
      summary.canonical_message_scalars_include_i32 &&
      summary.canonical_message_scalars_include_bool &&
      summary.canonical_forms_exclude_unknown &&
      summary.canonical_bridge_top_matches_reference_without_sel;
  summary.diagnostics_hardening_key = std::string("type-form-diagnostics-hardening;ref-sel=") +
                                      BoolKey(summary.canonical_reference_includes_sel) +
                                      ";msg-i32=" + BoolKey(summary.canonical_message_scalars_include_i32) +
                                      ";msg-bool=" + BoolKey(summary.canonical_message_scalars_include_bool) +
                                      ";exclude-unknown=" + BoolKey(summary.canonical_forms_exclude_unknown) +
                                      ";bridge-eq-ref-minus-sel=" +
                                      BoolKey(summary.canonical_bridge_top_matches_reference_without_sel);
  summary.diagnostics_hardening_ready =
      summary.diagnostics_hardening_consistent && !summary.diagnostics_hardening_key.empty();
  summary.recovery_determinism_consistent =
      summary.diagnostics_hardening_consistent &&
      summary.diagnostics_hardening_ready &&
      !summary.diagnostics_hardening_key.empty();
  summary.recovery_determinism_key = std::string("type-form-recovery-determinism;diagnostics-consistent=") +
                                     BoolKey(summary.diagnostics_hardening_consistent) +
                                     ";diagnostics-ready=" + BoolKey(summary.diagnostics_hardening_ready) +
                                     ";diagnostics-key-ready=" +
                                     BoolKey(!summary.diagnostics_hardening_key.empty());
  summary.recovery_determinism_ready =
      summary.recovery_determinism_consistent &&
      !summary.recovery_determinism_key.empty();
