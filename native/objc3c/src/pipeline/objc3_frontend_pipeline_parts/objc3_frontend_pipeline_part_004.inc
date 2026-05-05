    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendClassProtocolCategoryLinkingSummary summary;
  summary.declared_class_interfaces = interface_implementation_summary.declared_interfaces;
  summary.declared_class_implementations = interface_implementation_summary.declared_implementations;
  summary.resolved_class_interfaces = interface_implementation_summary.resolved_interfaces;
  summary.resolved_class_implementations = interface_implementation_summary.resolved_implementations;
  summary.linked_class_method_symbols = interface_implementation_summary.linked_implementation_symbols;
  summary.linked_category_method_symbols = protocol_category_summary.linked_category_symbols;

  const Objc3ProtocolCategoryCompositionSummary &integration_composition_summary =
      integration_surface.protocol_category_composition_summary;
  const Objc3ProtocolCategoryCompositionSummary &type_metadata_composition_summary =
      type_metadata_handoff.protocol_category_composition_summary;
  const auto select_composition_value = [&](std::size_t integration_value, std::size_t type_metadata_value) {
    if (integration_surface.built) {
      return integration_value;
    }
    return type_metadata_value;
  };

  summary.protocol_composition_sites =
      select_composition_value(integration_composition_summary.protocol_composition_sites,
                               type_metadata_composition_summary.protocol_composition_sites);
  summary.protocol_composition_symbols =
      select_composition_value(integration_composition_summary.protocol_composition_symbols,
                               type_metadata_composition_summary.protocol_composition_symbols);
  summary.category_composition_sites =
      select_composition_value(integration_composition_summary.category_composition_sites,
                               type_metadata_composition_summary.category_composition_sites);
  summary.category_composition_symbols =
      select_composition_value(integration_composition_summary.category_composition_symbols,
                               type_metadata_composition_summary.category_composition_symbols);
  summary.invalid_protocol_composition_sites =
      select_composition_value(integration_composition_summary.invalid_protocol_composition_sites,
                               type_metadata_composition_summary.invalid_protocol_composition_sites);

  const bool composition_fields_match =
      integration_composition_summary.protocol_composition_sites ==
          type_metadata_composition_summary.protocol_composition_sites &&
      integration_composition_summary.protocol_composition_symbols ==
          type_metadata_composition_summary.protocol_composition_symbols &&
      integration_composition_summary.category_composition_sites ==
          type_metadata_composition_summary.category_composition_sites &&
      integration_composition_summary.category_composition_symbols ==
          type_metadata_composition_summary.category_composition_symbols &&
      integration_composition_summary.invalid_protocol_composition_sites ==
          type_metadata_composition_summary.invalid_protocol_composition_sites;

  summary.deterministic_class_protocol_category_linking_handoff =
      interface_implementation_summary.deterministic &&
      protocol_category_summary.deterministic_protocol_category_handoff &&
      integration_composition_summary.deterministic &&
      type_metadata_composition_summary.deterministic &&
      composition_fields_match &&
      summary.resolved_class_interfaces <= summary.declared_class_interfaces &&
      summary.resolved_class_implementations <= summary.declared_class_implementations &&
      summary.linked_class_method_symbols <= interface_implementation_summary.interface_method_symbols &&
      summary.linked_class_method_symbols <= interface_implementation_summary.implementation_method_symbols &&
      summary.linked_category_method_symbols <= protocol_category_summary.category_method_symbols &&
      summary.category_composition_sites <= summary.protocol_composition_sites &&
      summary.category_composition_symbols <= summary.protocol_composition_symbols &&
      summary.invalid_protocol_composition_sites <=
          summary.protocol_composition_sites + summary.category_composition_sites;
  return summary;
}

template <typename Container>
void AccumulateSelectorNormalizationSummary(const Container &declarations,
                                           Objc3FrontendSelectorNormalizationSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &method : declaration.methods) {
      ++summary.method_declaration_entries;
      summary.selector_piece_entries += method.selector_pieces.size();

      std::size_t method_parameter_links = 0;
      bool method_parameter_names_complete = true;
      for (const auto &piece : method.selector_pieces) {
        if (!piece.has_parameter) {
          continue;
        }
        ++method_parameter_links;
        ++summary.selector_piece_parameter_links;
        if (piece.parameter_name.empty()) {
          method_parameter_names_complete = false;
        }
      }

      if (method.selector_is_normalized) {
        ++summary.normalized_method_declarations;
      }

      summary.deterministic_selector_normalization_handoff =
          summary.deterministic_selector_normalization_handoff &&
          (!method.selector_pieces.empty() || method.selector.empty()) &&
          (method.selector_is_normalized || method.selector_pieces.empty()) &&
          method_parameter_names_complete &&
          method_parameter_links <= method.params.size() &&
          method.params.size() <= method.selector_pieces.size();
    }
  }
}

Objc3FrontendSelectorNormalizationSummary BuildSelectorNormalizationSummary(const Objc3Program &program) {
  Objc3FrontendSelectorNormalizationSummary summary;
  AccumulateSelectorNormalizationSummary(program.protocols, summary);
  AccumulateSelectorNormalizationSummary(program.interfaces, summary);
  AccumulateSelectorNormalizationSummary(program.implementations, summary);
  summary.deterministic_selector_normalization_handoff =
      summary.deterministic_selector_normalization_handoff &&
      summary.normalized_method_declarations <= summary.method_declaration_entries &&
      summary.selector_piece_parameter_links <= summary.selector_piece_entries;
  return summary;
}

template <typename Container>
void AccumulatePropertyAttributeSummary(const Container &declarations,
                                        Objc3FrontendPropertyAttributeSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      ++summary.property_declaration_entries;
      summary.property_attribute_entries += property.attributes.size();

      std::size_t accessor_modifier_entries = 0;
      if (property.is_readonly) {
        ++accessor_modifier_entries;
      }
      if (property.is_readwrite) {
        ++accessor_modifier_entries;
      }
      if (property.is_atomic) {
        ++accessor_modifier_entries;
      }
      if (property.is_nonatomic) {
        ++accessor_modifier_entries;
      }
      if (property.is_copy) {
        ++accessor_modifier_entries;
      }
      if (property.is_retain) {
        ++accessor_modifier_entries;
      }
      if (property.is_strong) {
        ++accessor_modifier_entries;
      }
      if (property.is_weak) {
        ++accessor_modifier_entries;
      }
      if (property.is_unowned) {
        ++accessor_modifier_entries;
      }
      if (property.is_unsafe_unretained) {
        ++accessor_modifier_entries;
      }
      if (property.is_assign) {
        ++accessor_modifier_entries;
      }
      if (property.is_nullable) {
        ++accessor_modifier_entries;
      }
      if (property.is_nonnull) {
        ++accessor_modifier_entries;
      }
      if (property.is_null_resettable) {
        ++accessor_modifier_entries;
      }
      if (property.is_class) {
        ++accessor_modifier_entries;
      }
      if (property.is_direct) {
        ++accessor_modifier_entries;
      }
      if (property.has_getter) {
        ++accessor_modifier_entries;
        ++summary.property_getter_selector_entries;
      }
      if (property.has_setter) {
        ++accessor_modifier_entries;
        ++summary.property_setter_selector_entries;
      }
      summary.property_accessor_modifier_entries += accessor_modifier_entries;

      bool attribute_names_complete = true;
      bool attribute_values_complete = true;
      for (const auto &attribute : property.attributes) {
        if (attribute.name.empty()) {
          attribute_names_complete = false;
        }
        if (attribute.has_value) {
          ++summary.property_attribute_value_entries;
          if (attribute.value.empty()) {
            attribute_values_complete = false;
          }
        }
      }

      summary.deterministic_property_attribute_handoff =
          summary.deterministic_property_attribute_handoff &&
          !property.name.empty() &&
          (!property.is_readonly || !property.is_readwrite) &&
          (!property.is_atomic || !property.is_nonatomic) &&
          (!property.is_weak || (!property.is_unowned && !property.is_unsafe_unretained)) &&
          (!property.is_nullable || !property.is_nonnull) &&
          (!property.is_null_resettable || (!property.is_nullable && !property.is_nonnull)) &&
          (!property.has_getter || !property.getter_selector.empty()) &&
          (!property.has_setter || !property.setter_selector.empty()) &&
          attribute_names_complete &&
          attribute_values_complete &&
          summary.property_getter_selector_entries <= summary.property_declaration_entries &&
          summary.property_setter_selector_entries <= summary.property_declaration_entries;
    }
  }
}

Objc3FrontendPropertyAttributeSummary BuildPropertyAttributeSummary(const Objc3Program &program) {
  Objc3FrontendPropertyAttributeSummary summary;
  AccumulatePropertyAttributeSummary(program.protocols, summary);
  AccumulatePropertyAttributeSummary(program.interfaces, summary);
  AccumulatePropertyAttributeSummary(program.implementations, summary);
  summary.deterministic_property_attribute_handoff =
      summary.deterministic_property_attribute_handoff &&
      summary.property_attribute_value_entries <= summary.property_attribute_entries &&
      summary.property_accessor_modifier_entries >= summary.property_getter_selector_entries &&
      summary.property_accessor_modifier_entries >= summary.property_setter_selector_entries;
  return summary;
}

void AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    bool has_pointer_declarator,
    unsigned pointer_declarator_depth,
    const std::vector<Objc3SemaTokenMetadata> &pointer_declarator_tokens,
    const std::vector<Objc3SemaTokenMetadata> &nullability_suffix_tokens,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  if (object_pointer_type_spelling) {
    ++summary.object_pointer_type_spellings;
  }
  summary.pointer_declarator_depth_total += pointer_declarator_depth;
  summary.pointer_declarator_token_entries += pointer_declarator_tokens.size();
  summary.nullability_suffix_entries += nullability_suffix_tokens.size();

  if (has_pointer_declarator) {
    ++summary.pointer_declarator_entries;
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff && pointer_declarator_depth > 0;
  } else {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff && pointer_declarator_depth == 0;
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      (!object_pointer_type_spelling || !object_pointer_type_name.empty()) &&
      pointer_declarator_tokens.size() == static_cast<std::size_t>(pointer_declarator_depth);

  for (const auto &token : pointer_declarator_tokens) {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        token.kind == Objc3SemaTokenKind::PointerDeclarator && !token.text.empty();
  }
  for (const auto &token : nullability_suffix_tokens) {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        token.kind == Objc3SemaTokenKind::NullabilitySuffix && !token.text.empty();
  }

  if (has_generic_suffix) {
    ++summary.generic_suffix_entries;
    if (generic_suffix_terminated) {
      ++summary.terminated_generic_suffix_entries;
    } else {
      ++summary.unterminated_generic_suffix_entries;
    }
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        !generic_suffix_text.empty() && generic_suffix_text.front() == '<' &&
        (!generic_suffix_terminated || generic_suffix_text.back() == '>');
    return;
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      generic_suffix_terminated && generic_suffix_text.empty();
}

void AccumulateObjectPointerNullabilityGenericsForMethod(
    const Objc3MethodDecl &method,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  AccumulateObjectPointerNullabilityGenericsTypeAnnotation(method.return_object_pointer_type_spelling,
                                                           method.return_object_pointer_type_name,
                                                           method.has_return_pointer_declarator,
                                                           method.return_pointer_declarator_depth,
                                                           method.return_pointer_declarator_tokens,
                                                           method.return_nullability_suffix_tokens,
                                                           method.has_return_generic_suffix,
                                                           method.return_generic_suffix_terminated,
                                                           method.return_generic_suffix_text,
                                                           summary);
  for (const auto &param : method.params) {
    AccumulateObjectPointerNullabilityGenericsTypeAnnotation(param.object_pointer_type_spelling,
                                                             param.object_pointer_type_name,
                                                             param.has_pointer_declarator,
                                                             param.pointer_declarator_depth,
                                                             param.pointer_declarator_tokens,
                                                             param.nullability_suffix_tokens,
                                                             param.has_generic_suffix,
                                                             param.generic_suffix_terminated,
                                                             param.generic_suffix_text,
                                                             summary);
  }
}

template <typename Container>
void AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
    const Container &declarations,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateObjectPointerNullabilityGenericsTypeAnnotation(property.object_pointer_type_spelling,
                                                               property.object_pointer_type_name,
                                                               property.has_pointer_declarator,
                                                               property.pointer_declarator_depth,
                                                               property.pointer_declarator_tokens,
                                                               property.nullability_suffix_tokens,
                                                               property.has_generic_suffix,
                                                               property.generic_suffix_terminated,
                                                               property.generic_suffix_text,
                                                               summary);
    }
    for (const auto &method : declaration.methods) {
      AccumulateObjectPointerNullabilityGenericsForMethod(method, summary);
    }
  }
}

Objc3FrontendObjectPointerNullabilityGenericsSummary BuildObjectPointerNullabilityGenericsSummary(
    const Objc3Program &program) {
  Objc3FrontendObjectPointerNullabilityGenericsSummary summary;
  for (const auto &fn : program.functions) {
    AccumulateObjectPointerNullabilityGenericsTypeAnnotation(fn.return_object_pointer_type_spelling,
                                                             fn.return_object_pointer_type_name,
                                                             fn.has_return_pointer_declarator,
                                                             fn.return_pointer_declarator_depth,
                                                             fn.return_pointer_declarator_tokens,
                                                             fn.return_nullability_suffix_tokens,
                                                             fn.has_return_generic_suffix,
                                                             fn.return_generic_suffix_terminated,
                                                             fn.return_generic_suffix_text,
                                                             summary);
    for (const auto &param : fn.params) {
      AccumulateObjectPointerNullabilityGenericsTypeAnnotation(param.object_pointer_type_spelling,
                                                               param.object_pointer_type_name,
                                                               param.has_pointer_declarator,
                                                               param.pointer_declarator_depth,
                                                               param.pointer_declarator_tokens,
                                                               param.nullability_suffix_tokens,
                                                               param.has_generic_suffix,
                                                               param.generic_suffix_terminated,
                                                               param.generic_suffix_text,
                                                               summary);
    }
  }
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.protocols, summary);
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.interfaces, summary);
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.implementations, summary);

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      summary.terminated_generic_suffix_entries + summary.unterminated_generic_suffix_entries ==
          summary.generic_suffix_entries &&
      summary.pointer_declarator_entries <= summary.pointer_declarator_depth_total &&
      summary.pointer_declarator_entries <= summary.pointer_declarator_token_entries;
  return summary;
}

std::string BuildTypeSystemTypeSourceClosureReplayKey(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";protocol_required_methods=" << summary.protocol_required_method_count
      << ";protocol_optional_methods=" << summary.protocol_optional_method_count
      << ";protocol_required_properties=" << summary.protocol_required_property_count
      << ";protocol_optional_properties=" << summary.protocol_optional_property_count
      << ";object_pointer_type_spelling_sites=" << summary.object_pointer_type_spelling_sites
      << ";pointer_declarator_entries=" << summary.pointer_declarator_entries
      << ";nullability_suffix_entries=" << summary.nullability_suffix_entries
      << ";generic_suffix_entries=" << summary.generic_suffix_entries
      << ";optional_sites="
      << summary.optional_binding_sites << ":" << summary.guard_binding_sites << ":"
      << summary.optional_send_sites << ":" << summary.nil_coalescing_sites << ":"
      << summary.typed_keypath_literal_sites
      << ";optional_member_access_sites=" << summary.optional_member_access_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildControlFlowControlFlowSourceClosureReplayKey(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";guard_binding_sites=" << summary.guard_binding_sites
      << ";guard_binding_clause_sites=" << summary.guard_binding_clause_sites
      << ";guard_boolean_condition_sites="
      << summary.guard_boolean_condition_sites
      << ";switch_pattern_sites=" << summary.switch_case_pattern_sites << ":"
      << summary.switch_default_pattern_sites
      << ";match_surface_sites=" << summary.match_statement_sites << ":"
      << summary.match_case_pattern_sites << ":" << summary.match_default_sites
      << ":" << summary.match_wildcard_pattern_sites << ":"
      << summary.match_literal_pattern_sites << ":"
      << summary.match_binding_pattern_sites << ":"
      << summary.match_result_case_pattern_sites
      << ";reserved_keyword_sites=" << summary.defer_keyword_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildErrorHandlingErrorSourceClosureReplayKey(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";throws_sites=" << summary.function_throws_declaration_sites << ":"
      << summary.method_throws_declaration_sites
      << ";result_sites=" << summary.result_like_sites << ":"
      << summary.result_success_sites << ":" << summary.result_failure_sites
      << ":" << summary.result_branch_sites << ":"
      << summary.result_payload_sites
      << ";nserror_sites=" << summary.ns_error_bridging_sites << ":"
      << summary.ns_error_out_parameter_sites << ":"
      << summary.ns_error_bridge_path_sites
      << ";bridge_marker_sites=" << summary.objc_nserror_attribute_sites << ":"
      << summary.objc_status_code_attribute_sites << ":"
      << summary.status_code_success_clause_sites << ":"
      << summary.status_code_error_type_clause_sites << ":"
      << summary.status_code_mapping_clause_sites
      << ";reserved_keyword_sites=" << summary.try_keyword_sites << ":"
      << summary.throw_keyword_sites << ":" << summary.catch_keyword_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildConcurrencyAsyncSourceClosureReplayKey(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";async_sites=" << summary.async_keyword_sites << ":"
      << summary.async_function_sites << ":" << summary.async_method_sites
      << ";await_sites=" << summary.await_keyword_sites << ":"
      << summary.await_expression_sites
      << ";executor_sites=" << summary.executor_attribute_sites << ":"
      << summary.executor_main_sites << ":" << summary.executor_global_sites
      << ":" << summary.executor_named_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildConcurrencyActorMemberIsolationSourceClosureReplayKey(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";actor_sites=" << summary.actor_interface_sites << ":"
      << summary.actor_method_sites << ":" << summary.actor_property_sites
      << ";nonisolated_sites=" << summary.objc_nonisolated_annotation_sites
      << ";executor_sites=" << summary.actor_member_executor_annotation_sites
      << ";async_sites=" << summary.actor_async_method_sites
      << ";metadata_sites=" << summary.actor_member_metadata_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildConcurrencyTaskGroupCancellationSourceClosureReplayKey(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";async_callable_sites=" << summary.async_callable_sites
      << ";executor_sites=" << summary.executor_attribute_sites
      << ";task_creation_sites=" << summary.task_creation_sites
      << ";task_group_sites=" << summary.task_group_scope_sites << ":"
      << summary.task_group_add_task_sites << ":"
      << summary.task_group_wait_next_sites << ":"
      << summary.task_group_cancel_all_sites
      << ";cancellation_sites=" << summary.cancellation_check_sites << ":"
      << summary.cancellation_handler_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildOwnershipSystemExtensionSourceClosureReplayKey(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";resource_sites=" << summary.resource_attribute_sites << ":"
      << summary.resource_close_clause_sites << ":"
      << summary.resource_invalid_clause_sites
      << ";borrowed_sites=" << summary.borrowed_pointer_sites
      << ";returns_borrowed_sites=" << summary.returns_borrowed_attribute_sites
      << ";capture_sites=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_weak_sites << ":"
      << summary.explicit_capture_unowned_sites << ":"
      << summary.explicit_capture_move_sites << ":"
      << summary.explicit_capture_plain_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildOwnershipCleanupResourceCaptureSourceCompletionReplayKey(
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";cleanup_sites=" << summary.cleanup_attribute_sites << ":"
      << summary.cleanup_sugar_sites
      << ";resource_sites=" << summary.resource_attribute_sites << ":"
      << summary.resource_sugar_sites << ":"
      << summary.resource_close_clause_sites << ":"
      << summary.resource_invalid_clause_sites
      << ";capture_sites=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_weak_sites << ":"
      << summary.explicit_capture_unowned_sites << ":"
      << summary.explicit_capture_move_sites << ":"
      << summary.explicit_capture_plain_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildOwnershipRetainableCFamilySourceCompletionReplayKey(
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";family_sites=" << summary.family_retain_sites << ":"
      << summary.family_release_sites << ":"
      << summary.family_autorelease_sites
      << ";compat_sites=" << summary.compatibility_returns_retained_sites
      << ":" << summary.compatibility_returns_not_retained_sites
      << ":" << summary.compatibility_consumed_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildDispatchDispatchIntentSourceClosureReplayKey(
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";callable_sites=" << summary.direct_callable_sites << ":"
      << summary.final_callable_sites << ":" << summary.dynamic_callable_sites
      << ";container_sites=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites
      << ":" << summary.actor_container_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildDispatchDispatchIntentSourceCompletionReplayKey(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";prefixed_container_sites="
      << summary.prefixed_container_attribute_sites
      << ";container_sites=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites
      << ";defaulting_sites=" << summary.effective_direct_member_sites << ":"
      << summary.direct_members_defaulted_method_sites << ":"
      << summary.direct_members_dynamic_opt_out_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildMetaprogrammingMetaprogrammingSourceClosureReplayKey(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.derive_marker_sites << ":"
      << summary.macro_marker_sites << ":"
      << summary.property_behavior_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildMetaprogrammingMacroPackageProvenanceSourceCompletionReplayKey(
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.macro_marker_sites << ":"
      << summary.macro_package_sites << ":"
      << summary.macro_provenance_sites << ":"
      << summary.macro_cache_key_sites << ":"
      << summary.macro_sandbox_policy_sites << ":"
      << summary.expansion_visible_macro_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildMetaprogrammingPropertyBehaviorSourceCompletionReplayKey(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.property_behavior_sites << ":"
      << summary.interface_property_behavior_sites << ":"
      << summary.implementation_property_behavior_sites << ":"
      << summary.protocol_property_behavior_sites << ":"
      << summary.synthesized_binding_visible_sites << ":"
      << summary.synthesized_getter_visible_sites << ":"
      << summary.synthesized_setter_visible_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildInteropForeignImportSourceClosureReplayKey(
    const Objc3FrontendInteropForeignImportSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.foreign_callable_sites << ":"
      << summary.extern_foreign_callable_sites << ":"
      << summary.import_module_annotation_sites << ":"
      << summary.imported_module_name_sites << ":"
      << summary.export_header_annotation_sites << ":"
      << summary.export_header_name_sites << ":"
      << summary.mixed_image_annotation_sites << ":"
      << summary.mixed_image_name_sites << ":"
      << summary.package_entry_annotation_sites << ":"
      << summary.package_entry_name_sites << ":"
      << summary.interop_annotation_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildInteropCppSwiftInteropAnnotationSourceCompletionReplayKey(
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.swift_name_annotation_sites << ":"
      << summary.swift_private_annotation_sites << ":"
      << summary.cpp_name_annotation_sites << ":"
      << summary.header_name_annotation_sites << ":"
      << summary.abi_alignment_annotation_sites << ":"
      << summary.foreign_type_annotation_sites << ":"
      << summary.interop_metadata_annotation_sites << ":"
      << summary.named_annotation_payload_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildToolingDiagnosticsMigratorSourceInventoryReplayKey(
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &summary) {
  std::ostringstream out;
  out << "families=" << summary.advanced_feature_family_count
      << ";dependencies=" << summary.dependency_surface_count
      << ";claims=" << summary.aggregated_source_only_claim_count
      << ";fail-closed=" << summary.fail_closed_construct_count
      << ";sites=" << summary.diagnostic_surface_sites << ":"
      << summary.fixit_surface_sites << ":" << summary.migrator_surface_sites
      << ":" << summary.canonicalization_hint_sites
      << ";parts=" << summary.error_surface_sites << ":"
      << summary.concurrency_surface_sites << ":"
      << summary.system_surface_sites << ":" << summary.dispatch_surface_sites
      << ":" << summary.metaprogramming_surface_sites << ":"
      << summary.interop_surface_sites
      << ";supported="
      << (summary.diagnostics_inventory_source_supported ? "true" : "false")
      << ":" << (summary.fixit_inventory_source_supported ? "true" : "false")
      << ":" << (summary.migrator_inventory_source_supported ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildToolingMigrationCanonicalizationSourceCompletionReplayKey(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "compat=" << summary.language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.legacy_literal_diagnostics_enabled ? "true" : "false")
      << ";legacy=" << summary.legacy_yes_sites << ":" << summary.legacy_no_sites
      << ":" << summary.legacy_null_sites << ":" << summary.legacy_total_sites
      << ";canonical=" << summary.canonical_true_rewrite_sites << ":"
      << summary.canonical_false_rewrite_sites << ":"
      << summary.canonical_nil_rewrite_sites
      << ";candidates=" << summary.canonicalization_candidate_sites << ":"
      << summary.fixit_candidate_sites << ":"
      << summary.migrator_candidate_sites
      << ";ready="
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildToolingDiagnosticTaxonomyPortabilityContractReplayKey(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary &summary) {
  std::ostringstream out;
  out << "portability-dependencies=" << summary.portability_dependency_count
      << ";diagnostics=" << summary.diagnostics_total << ":"
      << summary.diagnostics_after_pass_final << ":"
      << summary.diagnostics_emitted_total
      << ";arc-fixit=" << summary.ownership_arc_diagnostic_candidate_sites << ":"
      << summary.ownership_arc_fixit_available_sites << ":"
      << summary.ownership_arc_profiled_sites << ":"
      << summary.ownership_arc_weak_unowned_conflict_diagnostic_sites << ":"
      << summary.ownership_arc_empty_fixit_hint_sites << ":"
      << summary.ownership_arc_contract_violation_sites
      << ";migration-candidates="
      << summary.migration_canonicalization_candidate_sites
      << ";ready="
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildToolingFeatureSpecificFixitSynthesisReplayKey(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &summary) {
  std::ostringstream out;
  out << "families=" << summary.fixit_family_count
      << ";migration=" << summary.migration_fixit_candidate_sites << ":"
      << summary.migrator_candidate_sites
      << ";ownership-arc=" << summary.ownership_arc_fixit_available_sites << ":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ";ready="
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildConcurrencyLowercaseProfileToken(std::string token) {
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char value) {
                   return static_cast<char>(std::tolower(value));
                 });
  return token;
}

bool IsConcurrencyTaskCreationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("task_spawn") != std::string::npos ||
         lowered.find("spawn_task") != std::string::npos ||
         lowered.find("detached_task") != std::string::npos ||
         lowered.find("task_detach") != std::string::npos;
}

bool IsConcurrencyTaskGroupScopeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("with_task_group") != std::string::npos ||
         lowered.find("task_group_scope") != std::string::npos;
}

bool IsConcurrencyTaskGroupAddTaskSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("task_group_add_task") != std::string::npos ||
         lowered.find("group_add_task") != std::string::npos;
}

bool IsConcurrencyTaskGroupWaitNextSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("task_group_wait_next") != std::string::npos ||
         lowered.find("group_wait_next") != std::string::npos ||
         lowered.find("wait_next") != std::string::npos;
}

bool IsConcurrencyTaskGroupCancelAllSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("task_group_cancel_all") != std::string::npos ||
         lowered.find("group_cancel_all") != std::string::npos ||
         lowered.find("cancel_all") != std::string::npos;
}

bool IsConcurrencyCancellationCheckSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("cancelled") != std::string::npos ||
         lowered.find("is_cancelled") != std::string::npos ||
         lowered.find("cancellation") != std::string::npos;
}

bool IsConcurrencyCancellationHandlerSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("on_cancel") != std::string::npos ||
         lowered.find("cancel_handler") != std::string::npos ||
         lowered.find("with_cancellation_handler") != std::string::npos;
}

void CollectConcurrencyAsyncSourceClosureExprSites(
    const Expr *expr, Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  if (expr->await_expression_enabled) {
    ++summary.await_expression_sites;
  }
  CollectConcurrencyAsyncSourceClosureExprSites(expr->receiver.get(), summary);
  CollectConcurrencyAsyncSourceClosureExprSites(expr->left.get(), summary);
  CollectConcurrencyAsyncSourceClosureExprSites(expr->right.get(), summary);
  CollectConcurrencyAsyncSourceClosureExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectConcurrencyAsyncSourceClosureExprSites(arg.get(), summary);
  }
}

void CollectConcurrencyAsyncSourceClosureStmtSites(
    const Stmt *stmt, Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->if_stmt->condition.get(), summary);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(then_stmt.get(), summary);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(else_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->for_stmt->init.value.get(), summary);
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->for_stmt->condition.get(), summary);
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectConcurrencyAsyncSourceClosureStmtSites(case_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectConcurrencyAsyncSourceClosureExprSites(stmt->expr_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

void CollectConcurrencyTaskGroupCancellationExprSites(
    const Expr *expr,
    Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  const auto collect_symbol = [&summary](const std::string &symbol) {
    if (IsConcurrencyTaskCreationSymbol(symbol)) {
      ++summary.task_creation_sites;
    }
    if (IsConcurrencyTaskGroupScopeSymbol(symbol)) {
      ++summary.task_group_scope_sites;
    }
    if (IsConcurrencyTaskGroupAddTaskSymbol(symbol)) {
      ++summary.task_group_add_task_sites;
    }
    if (IsConcurrencyTaskGroupWaitNextSymbol(symbol)) {
      ++summary.task_group_wait_next_sites;
    }
    if (IsConcurrencyTaskGroupCancelAllSymbol(symbol)) {
      ++summary.task_group_cancel_all_sites;
    }
    if (IsConcurrencyCancellationCheckSymbol(symbol)) {
      ++summary.cancellation_check_sites;
    }
    if (IsConcurrencyCancellationHandlerSymbol(symbol)) {
      ++summary.cancellation_handler_sites;
    }
  };

  switch (expr->kind) {
  case Expr::Kind::Call:
    collect_symbol(expr->ident);
    break;
  case Expr::Kind::MessageSend:
    collect_symbol(expr->selector);
    break;
  default:
    break;
  }

  CollectConcurrencyTaskGroupCancellationExprSites(expr->receiver.get(), summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->left.get(), summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->right.get(), summary);
  CollectConcurrencyTaskGroupCancellationExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectConcurrencyTaskGroupCancellationExprSites(arg.get(), summary);
  }
}

void CollectConcurrencyTaskGroupCancellationStmtSites(
    const Stmt *stmt,
    Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->if_stmt->condition.get(), summary);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(then_stmt.get(), summary);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(else_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectConcurrencyTaskGroupCancellationStmtSites(case_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectConcurrencyTaskGroupCancellationExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

void CollectTypeSystemTypeSourceClosureExprSites(
    const Expr *expr, Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::Binary && expr->op == "??") {
    ++summary.nil_coalescing_sites;
  }
  if (expr->kind == Expr::Kind::MessageSend && expr->optional_send_enabled) {
    ++summary.optional_send_sites;
  }
  if (expr->kind == Expr::Kind::MessageSend &&
      expr->optional_member_access_enabled) {
    ++summary.optional_member_access_sites;
  }
  if (expr->typed_keypath_literal_enabled) {
    ++summary.typed_keypath_literal_sites;
  }
  CollectTypeSystemTypeSourceClosureExprSites(expr->receiver.get(), summary);
  CollectTypeSystemTypeSourceClosureExprSites(expr->left.get(), summary);
  CollectTypeSystemTypeSourceClosureExprSites(expr->right.get(), summary);
  CollectTypeSystemTypeSourceClosureExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectTypeSystemTypeSourceClosureExprSites(arg.get(), summary);
  }
  for (const auto &stmt : expr->block_body) {
    if (stmt != nullptr) {
      const Stmt *nested = stmt.get();
      switch (nested->kind) {
      case Stmt::Kind::Let:
        if (nested->let_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(nested->let_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Assign:
        if (nested->assign_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(nested->assign_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Return:
        if (nested->return_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(nested->return_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Expr:
        if (nested->expr_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(nested->expr_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::If:
      case Stmt::Kind::DoWhile:
      case Stmt::Kind::For:
      case Stmt::Kind::Switch:
      case Stmt::Kind::While:
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        break;
      }
    }
  }
}

void CollectTypeSystemTypeSourceClosureStmtSites(
    const Stmt *stmt, Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->let_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->assign_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->return_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->expr_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->if_stmt->condition.get(), summary);
      for (const auto &child : stmt->if_stmt->then_body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
      for (const auto &child : stmt->if_stmt->else_body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->do_while_stmt->condition.get(), summary);
      for (const auto &child : stmt->do_while_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->for_stmt->init.value.get(), summary);
      CollectTypeSystemTypeSourceClosureExprSites(stmt->for_stmt->condition.get(), summary);
      CollectTypeSystemTypeSourceClosureExprSites(stmt->for_stmt->step.value.get(), summary);
      for (const auto &child : stmt->for_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->switch_stmt->condition.get(), summary);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &child : case_stmt.body) {
          CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
        }
      }
    }
    break;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(stmt->while_stmt->condition.get(), summary);
      for (const auto &child : stmt->while_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &child : stmt->block_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    break;
  }
}

Objc3FrontendTypeSystemTypeSourceClosureSummary BuildTypeSystemTypeSourceClosureSummary(
    const Objc3Program &program,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_summary) {
  Objc3FrontendTypeSystemTypeSourceClosureSummary summary;
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      if (method_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++summary.protocol_optional_method_count;
      } else {
        ++summary.protocol_required_method_count;
      }
    }
    for (const auto &property_decl : protocol_decl.properties) {
      if (property_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++summary.protocol_optional_property_count;
      } else {
        ++summary.protocol_required_property_count;
      }
    }
  }

  summary.object_pointer_type_spelling_sites =
      object_pointer_summary.object_pointer_type_spellings;
  summary.pointer_declarator_entries =
      object_pointer_summary.pointer_declarator_entries;
  summary.nullability_suffix_entries =
      object_pointer_summary.nullability_suffix_entries;
  summary.generic_suffix_entries =
      object_pointer_summary.generic_suffix_entries;
  summary.protocol_optional_partition_source_supported = true;
  summary.object_pointer_nullability_source_supported = true;
  summary.pragmatic_generic_suffix_source_supported = true;
  summary.optional_binding_source_supported = true;
  summary.optional_send_source_supported = true;
  summary.nil_coalescing_source_supported = true;
  summary.typed_keypath_literal_source_supported = true;
  summary.optional_member_access_fail_closed = false;
  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      CollectTypeSystemTypeSourceClosureStmtSites(stmt.get(), summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        CollectTypeSystemTypeSourceClosureStmtSites(stmt.get(), summary);
      }
    }
  }
  summary.deterministic_handoff =
      object_pointer_summary
          .deterministic_object_pointer_nullability_generics_handoff &&
      summary.protocol_required_method_count +
              summary.protocol_optional_method_count >=
          summary.protocol_optional_method_count &&
      summary.protocol_required_property_count +
              summary.protocol_optional_property_count >=
          summary.protocol_optional_property_count;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildTypeSystemTypeSourceClosureReplayKey(summary);
  return summary;
}

void CollectControlFlowControlFlowSourceClosureStmtSites(
    const Stmt *stmt,
    Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    break;
  case Stmt::Kind::Assign:
  case Stmt::Kind::Return:
  case Stmt::Kind::Expr:
    break;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      if (stmt->if_stmt->guard_binding_surface_enabled) {
        ++summary.guard_binding_sites;
        summary.guard_binding_clause_sites +=
            stmt->if_stmt->optional_binding_clause_count;
      }
      if (stmt->if_stmt->guard_condition_list_surface_enabled) {
        summary.guard_boolean_condition_sites +=
            stmt->if_stmt->guard_boolean_condition_clause_count;
      }
      for (const auto &child : stmt->if_stmt->then_body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(child.get(), summary);
      }
      for (const auto &child : stmt->if_stmt->else_body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &child : stmt->do_while_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      for (const auto &child : stmt->for_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      if (stmt->switch_stmt->match_surface_enabled) {
        ++summary.match_statement_sites;
      } else {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          if (case_stmt.is_default) {
            ++summary.switch_default_pattern_sites;
          } else {
            ++summary.switch_case_pattern_sites;
          }
          for (const auto &child : case_stmt.body) {
            CollectControlFlowControlFlowSourceClosureStmtSites(child.get(), summary);
          }
        }
        break;
      }
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        if (case_stmt.is_default) {
          ++summary.match_default_sites;
        } else {
          ++summary.match_case_pattern_sites;
          switch (case_stmt.match_pattern_kind) {
          case MatchPatternKind::Wildcard:
            ++summary.match_wildcard_pattern_sites;
            break;
          case MatchPatternKind::LiteralInteger:
          case MatchPatternKind::LiteralBool:
          case MatchPatternKind::LiteralNil:
            ++summary.match_literal_pattern_sites;
            break;
          case MatchPatternKind::Binding:
            ++summary.match_binding_pattern_sites;
            break;
          case MatchPatternKind::ResultCase:
            ++summary.match_result_case_pattern_sites;
            break;
          case MatchPatternKind::None:
            break;
          }
        }
        for (const auto &child : case_stmt.body) {
          CollectControlFlowControlFlowSourceClosureStmtSites(child.get(), summary);
        }
      }
    }
    break;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      for (const auto &child : stmt->while_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &child : stmt->block_stmt->body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    break;
  }
}

Objc3FrontendControlFlowControlFlowSourceClosureSummary
BuildControlFlowControlFlowSourceClosureSummary(
    const Objc3Program &program,
