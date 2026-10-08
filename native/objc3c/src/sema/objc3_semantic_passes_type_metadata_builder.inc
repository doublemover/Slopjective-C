                                           container_name +
                                           "' must define a body"));
      }

      Objc3MethodInfo method_info =
          BuildMethodInfo(method_decl, selector_contract, arc_mode_enabled,
                          &generic_interface_names);
      const auto method_insert =
          implementation_info.methods.emplace(selector, std::move(method_info));
      if (!method_insert.second) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S200",
                                       "duplicate " + container_label +
                                           " selector '" + selector + "' in " +
                                           container_label + " '" +
                                           container_name + "'"));
        continue;
      }

      if (!is_category_implementation) {
        ++interface_implementation_summary.implementation_method_symbols;
      }
      if (matched_interface == nullptr) {
        continue;
      }

      const auto interface_method_it = matched_interface->methods.find(selector);
      if (interface_method_it == matched_interface->methods.end()) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: implementation selector '" + selector + "' in '" +
                                           implementation_decl.name + "' is not declared in interface"));
        continue;
      }

      if (!IsCompatibleMethodSignature(interface_method_it->second, method_insert.first->second)) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: incompatible method signature for selector '" + selector +
                                           "' in implementation '" + implementation_decl.name + "'"));
        continue;
      }

      if (!is_category_implementation) {
        ++interface_implementation_summary.linked_implementation_symbols;
      }
    }

    if (!is_category_implementation) {
      surface.implementations.emplace(implementation_decl.name,
                                      std::move(implementation_info));
    } else {
      const std::string category_owner_identity =
          BuildCategoryOwnerIdentity(implementation_decl.name,
                                     implementation_decl.category_name);
      if (!surface.category_implementations
               .emplace(category_owner_identity, std::move(implementation_info))
               .second) {
        diagnostics.push_back(MakeDiag(implementation_decl.line,
                                       implementation_decl.column,
                                       "O3S200",
                                       "duplicate category implementation '" +
                                           container_name + "'"));
      }
    }
  }

  // protocol-conformance enforcement anchor: parser-owned
  // @required/@optional partitions are now enforced here against class and
  // category adoption before runtime metadata consumption can proceed.
  ValidateAndBuildCategoryMergeSurfaces(ast, surface, diagnostics);
  ValidateDeclaredProtocolConformance(ast, surface, protocol_definitions,
                                      surface.category_interfaces,
                                      surface.category_implementations,
                                      diagnostics);
  ValidateInteropInteropRuntimeParity(ast, diagnostics);
  ValidateInteropCppInteropInteractions(ast, diagnostics);
  ValidateInteropSwiftInteropIsolation(ast, diagnostics);
  ValidateInheritanceOverrideAndRealizationLegality(ast, surface, diagnostics);
  ValidateDispatchDispatchIntentCompatibility(ast, diagnostics);

  interface_implementation_summary.resolved_interfaces = surface.interfaces.size();
  interface_implementation_summary.resolved_implementations = surface.implementations.size();
  interface_implementation_summary.deterministic =
      interface_implementation_summary.linked_implementation_symbols <=
          interface_implementation_summary.implementation_method_symbols &&
      interface_implementation_summary.linked_implementation_symbols <=
          interface_implementation_summary.interface_method_symbols;
  surface.interface_implementation_summary = interface_implementation_summary;
#include "sema/objc3_semantic_passes_type_metadata_integration_surface_summaries.inc"
  // unsupported-feature enforcement anchor: accepted source sites
  // that would over-claim unsupported runtime-backed features must fail closed
  // in sema before lowering/runtime handoff proceeds.
  const Objc3UnsupportedFeatureClaimEnforcementStats
      unsupported_feature_enforcement =
          DiagnoseUnsupportedFeatureClaimSources(
              ast,
              allow_source_only_block_literals,
              allow_source_only_defer_statements,
              allow_source_only_error_runtime_surface,
              arc_mode_enabled,
              diagnostics);
  // semantic freeze anchor: sema owns the single fail-closed
  // legality summary that classifies live compatibility/migration selections,
  // source-only claim downgrades, and unsupported strictness/macro claim
  // rejections before later lowering/runtime/reporting lanes consume them.
  surface.compatibility_strictness_claim_semantics_summary =
      BuildCompatibilityStrictnessClaimSemanticsSummaryFromIntegrationSurface(
          surface,
          language_profile,
          unsupported_feature_enforcement);
  surface.built = true;
  return surface;
}

Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {
  Objc3SemanticTypeMetadataHandoff handoff;
  handoff.global_names_lexicographic.reserve(surface.globals.size());
  for (const auto &global : surface.globals) {
    handoff.global_names_lexicographic.push_back(global.first);
  }
  std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());

#include "sema/objc3_semantic_passes_type_metadata_function_builder.inc"

#include "sema/objc3_semantic_passes_type_metadata_interface_handoff_builder.inc"

#include "sema/objc3_semantic_passes_type_metadata_implementation_handoff_builder.inc"

#include "sema/objc3_semantic_passes_type_metadata_interface_implementation_summary.inc"

#include "sema/objc3_semantic_passes_type_metadata_selector_summary.inc"
