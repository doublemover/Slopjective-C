Objc3OwnershipSystemExtensionSemanticModelSummary
BuildOwnershipSystemExtensionSemanticModelSummary(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
        &source_summary,
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &completion_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary) {
  Objc3OwnershipSystemExtensionSemanticModelSummary summary;
  summary.cleanup_attribute_sites = completion_summary.cleanup_attribute_sites;
  summary.cleanup_sugar_sites = completion_summary.cleanup_sugar_sites;
  summary.resource_attribute_sites = completion_summary.resource_attribute_sites;
  summary.resource_sugar_sites = completion_summary.resource_sugar_sites;
  summary.borrowed_pointer_sites = source_summary.borrowed_pointer_sites;
  summary.returns_borrowed_attribute_sites =
      source_summary.returns_borrowed_attribute_sites;
  summary.explicit_capture_list_sites =
      completion_summary.explicit_capture_list_sites;
  summary.explicit_capture_item_sites =
      completion_summary.explicit_capture_item_sites;
  summary.retainable_family_annotation_sites =
      retainable_summary.family_retain_sites +
      retainable_summary.family_release_sites +
      retainable_summary.family_autorelease_sites;
  summary.retainable_family_compatibility_alias_sites =
      retainable_summary.compatibility_returns_retained_sites +
      retainable_summary.compatibility_returns_not_retained_sites +
      retainable_summary.compatibility_consumed_sites;

  summary.source_dependency_required = true;
  summary.cleanup_resource_semantic_model_frozen =
      completion_summary.cleanup_attribute_source_supported &&
      completion_summary.resource_sugar_source_supported &&
      summary.resource_attribute_sites >= summary.resource_sugar_sites;
  summary.borrowed_pointer_semantic_model_frozen =
      source_summary.borrowed_pointer_source_supported &&
      source_summary.returns_borrowed_source_supported &&
      summary.borrowed_pointer_sites >= summary.returns_borrowed_attribute_sites;
  summary.capture_legality_semantic_model_frozen =
      completion_summary.explicit_capture_list_source_supported &&
      summary.explicit_capture_item_sites >= summary.explicit_capture_list_sites;
  summary.retainable_family_semantic_model_frozen =
      retainable_summary.callable_annotation_source_supported &&
      retainable_summary.compatibility_alias_source_supported &&
      summary.retainable_family_annotation_sites > 0u;
  summary.resource_move_semantics_deferred = true;
  summary.borrowed_escape_semantics_deferred = true;
  summary.retainable_family_legality_deferred = true;
  summary.deterministic =
      source_summary.deterministic_handoff &&
      completion_summary.deterministic_handoff &&
      retainable_summary.deterministic_handoff &&
      summary.cleanup_resource_semantic_model_frozen &&
      summary.borrowed_pointer_semantic_model_frozen &&
      summary.capture_legality_semantic_model_frozen &&
      summary.retainable_family_semantic_model_frozen;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";source=" << source_summary.contract_id
      << ";completion=" << completion_summary.contract_id
      << ";retainable=" << retainable_summary.contract_id
      << ";cleanup=" << summary.cleanup_attribute_sites << ":"
      << summary.cleanup_sugar_sites
      << ";resource=" << summary.resource_attribute_sites << ":"
      << summary.resource_sugar_sites
      << ";borrowed=" << summary.borrowed_pointer_sites << ":"
      << summary.returns_borrowed_attribute_sites
      << ";capture=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites
      << ";retainable=" << summary.retainable_family_annotation_sites << ":"
      << summary.retainable_family_compatibility_alias_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
