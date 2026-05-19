  result.type_metadata_mapping_readiness_record =
      BuildObjc3SemaTypeMetadataMappingReadinessRecord(
          input, result.parity_surface);
  result.deterministic_type_metadata_mapping_readiness_record =
      IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
          result.type_metadata_mapping_readiness_record);
  result.parity_surface.type_metadata_mapping_readiness_record =
      result.type_metadata_mapping_readiness_record;
  result.parity_surface.deterministic_type_metadata_mapping_readiness_record =
      result.deterministic_type_metadata_mapping_readiness_record;
  result.typed_semantic_handoff_record =
      BuildObjc3SemaTypedSemanticHandoffRecord(input, result.parity_surface);
  result.deterministic_typed_semantic_handoff_record =
      IsReadyObjc3SemaTypedSemanticHandoffRecord(
          result.typed_semantic_handoff_record);
  result.parity_surface.typed_semantic_handoff_record =
      result.typed_semantic_handoff_record;
  result.parity_surface.deterministic_typed_semantic_handoff_record =
      result.deterministic_typed_semantic_handoff_record;
  result.parser_sema_conformance_evidence_record =
      BuildObjc3ParserSemaConformanceEvidenceRecord(input, result.parity_surface);
  result.deterministic_parser_sema_conformance_evidence_record =
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          result.parser_sema_conformance_evidence_record);
  result.parity_surface.parser_sema_conformance_evidence_record =
      result.parser_sema_conformance_evidence_record;
  result.parity_surface.deterministic_parser_sema_conformance_evidence_record =
      result.deterministic_parser_sema_conformance_evidence_record;
  result.parser_sema_contract_readiness_record =
      BuildObjc3ParserSemaContractReadinessRecord(input, result.parity_surface);
  result.deterministic_parser_sema_contract_readiness_record =
      IsReadyObjc3ParserSemaContractReadinessRecord(
          result.parser_sema_contract_readiness_record);
  result.parity_surface.parser_sema_contract_readiness_record =
      result.parser_sema_contract_readiness_record;
  result.parity_surface.deterministic_parser_sema_contract_readiness_record =
      result.deterministic_parser_sema_contract_readiness_record;
  result.parity_closeout_publication_readiness_record =
      BuildObjc3SemaParityCloseoutPublicationReadinessRecord(
          input, result.executed, result.parity_surface);
  result.deterministic_parity_closeout_publication_readiness_record =
      IsReadyObjc3SemaParityCloseoutPublicationReadinessRecord(
          result.parity_closeout_publication_readiness_record);
  result.parity_surface.parity_closeout_publication_readiness_record =
      result.parity_closeout_publication_readiness_record;
  result.parity_surface
      .deterministic_parity_closeout_publication_readiness_record =
      result.deterministic_parity_closeout_publication_readiness_record;
  result.parity_validation_record = BuildObjc3SemaParityValidationRecord(
      input,
      result.parity_closeout_publication_readiness_record);
  result.deterministic_parity_validation_record =
      IsReadyObjc3SemaParityValidationRecord(result.parity_validation_record);
  result.parity_surface.parity_validation_record =
      result.parity_validation_record;
  result.parity_surface.deterministic_parity_validation_record =
      result.deterministic_parity_validation_record;
