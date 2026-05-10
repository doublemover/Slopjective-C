  snapshot.parser_diagnostic_count = parser_diagnostic_count;
  snapshot.diagnostic_owner_contract_id = std::string(kObjc3DiagnosticOwnerContractId);
  snapshot.diagnostic_stage_owner = std::string(kObjc3ParserDiagnosticStageOwner);
  snapshot.diagnostic_fixit_owner = std::string(kObjc3ParserDiagnosticFixitOwner);
  snapshot.diagnostic_recovery_owner = std::string(kObjc3ParserDiagnosticRecoveryOwner);
  snapshot.diagnostic_owner_split_explicit =
      Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kParser) &&
      Objc3DiagnosticOwnerIsExplicit(snapshot.diagnostic_stage_owner) &&
      Objc3DiagnosticOwnerIsExplicit(snapshot.diagnostic_fixit_owner) &&
      Objc3DiagnosticOwnerIsExplicit(snapshot.diagnostic_recovery_owner);
  snapshot.diagnostic_recovery_counts_as_success = false;
  snapshot.deterministic_handoff =
      snapshot.diagnostic_owner_split_explicit &&
      !snapshot.diagnostic_recovery_counts_as_success;
  snapshot.parser_recovery_replay_ready = true;
