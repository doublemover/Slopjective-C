           frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic) {
        total_method_entries += bundle.entries_lexicographic.size();
      }
      out << "; runtime_metadata_member_table_emission = "
          << Objc3RuntimeMetadataMemberTableEmissionSummary()
          << ";method_list_bundle_count="
          << frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic
                 .size()
          << ";method_entry_count=" << total_method_entries
          << ";property_descriptor_count="
          << frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                 .size()
          << ";ivar_descriptor_count="
          << frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic.size()
          << ";typed_handoff_replay="
          << frontend_metadata_.runtime_metadata_member_table_typed_handoff_replay_key
          << "\n";
    }
    if (emit_class_metaclass_bundle_payloads &&
        emit_protocol_category_bundle_payloads &&
        emit_member_table_payloads) {
      std::unordered_set<std::string> implementation_method_owner_identities;
      implementation_method_owner_identities.reserve(method_definitions_.size());
      for (const MethodDefinition &method_def : method_definitions_) {
        if (method_def.method_owner_identity.empty()) {
          continue;
        }
        implementation_method_owner_identities.insert(
            method_def.method_owner_identity);
      }
      std::size_t executable_method_entry_count = 0;
      std::size_t bound_method_entry_count = 0;
      for (const auto &bundle :
           frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic) {
        const bool implementation_owned =
            bundle.owner_kind == "class-implementation" ||
            bundle.owner_kind == "category-implementation";
        if (!implementation_owned) {
          continue;
        }
        for (const auto &entry : bundle.entries_lexicographic) {
          if (!entry.has_body) {
            continue;
          }
          ++executable_method_entry_count;
          if (implementation_method_owner_identities.find(entry.owner_identity) !=
              implementation_method_owner_identities.end()) {
            ++bound_method_entry_count;
          }
        }
      }
      out << "; executable_object_artifact_lowering = "
          << Objc3ExecutableObjectArtifactLoweringSummary()
          << ";class_realization_record_count="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_bundles_lexicographic.size()
          << ";category_realization_record_count="
          << frontend_metadata_.runtime_metadata_category_bundles_lexicographic
                 .size()
          << ";implementation_method_definition_count="
          << method_definitions_.size()
          << ";executable_method_entry_count="
          << executable_method_entry_count
          << ";bound_method_entry_count=" << bound_method_entry_count
          << ";class_handoff_replay="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_typed_handoff_replay_key
          << ";protocol_category_handoff_replay="
          << frontend_metadata_
                 .runtime_metadata_protocol_category_typed_handoff_replay_key
          << ";member_table_handoff_replay="
          << frontend_metadata_
                 .runtime_metadata_member_table_typed_handoff_replay_key
          << "\n";
      out << "; executable_method_body_binding = "
          << Objc3ExecutableMethodBodyBindingSummary()
          << ";implementation_method_definition_count="
          << method_definitions_.size()
          << ";executable_method_entry_count="
          << executable_method_entry_count
          << ";bound_method_entry_count=" << bound_method_entry_count
          << "\n";
      // executable realization-record expansion anchor: lane-C now
      // publishes the realization-ready class/protocol/category record contract
      // separately from the older C001/C002 boundary lines so later runtime
      // work can consume the preserved owner/super/adoption edges directly.
      out << "; executable_realization_records = "
          << Objc3ExecutableRealizationRecordsSummary()
          << ";class_record_count="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_bundles_lexicographic.size()
          << ";protocol_record_count="
          << frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic
                 .size()
          << ";category_record_count="
          << frontend_metadata_.runtime_metadata_category_bundles_lexicographic
                 .size()
          << "\n";
      // class-realization-runtime freeze anchor: lane-D consumes the
      // already-emitted realization records through one explicit runtime-owned
      // boundary instead of rediscovering class/metaclass/category/protocol
      // relationships from source or manifests.
      out << "; runtime_class_realization = "
          << Objc3RuntimeClassRealizationSummary()
          << ";class_bundle_count="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_bundles_lexicographic.size()
          << ";protocol_record_count="
          << frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic
                 .size()
          << ";category_record_count="
          << frontend_metadata_.runtime_metadata_category_bundles_lexicographic
                 .size()
          << "\n";
      // metaclass-graph-root-class anchor: lane-D now republishes
      // the realized class/metaclass graph and root-class baseline over the
      // same emitted realization records without widening the IR object
      // surface beyond preserved receiver identities and owner/super edges.
      out << "; runtime_metaclass_graph_root_class_baseline = "
          << Objc3RuntimeMetaclassGraphRootClassSummary()
          << ";class_bundle_count="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_bundles_lexicographic.size()
          << ";receiver_binding_candidate_count="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_bundles_lexicographic.size()
          << "\n";
      std::size_t class_protocol_ref_count = 0;
      for (const auto &bundle :
           frontend_metadata_.runtime_metadata_class_metaclass_bundles_lexicographic) {
        class_protocol_ref_count +=
            bundle.adopted_protocol_owner_identities_lexicographic.size();
      }
      std::size_t category_protocol_ref_count = 0;
      for (const auto &bundle :
           frontend_metadata_.runtime_metadata_category_bundles_lexicographic) {
        category_protocol_ref_count +=
            bundle.adopted_protocol_owner_identities_lexicographic.size();
      }
      // category-attachment-protocol-conformance anchor: emitted
      // realization records now carry direct class protocol refs and category
      // attachment protocol refs so runtime queries can consume the realized
      // graph without falling back to manifest-only summaries.
      out << "; runtime_category_attachment_protocol_conformance = "
          << Objc3RuntimeCategoryAttachmentProtocolConformanceSummary()
          << ";attached_category_candidate_count="
          << frontend_metadata_.runtime_metadata_category_bundles_lexicographic
                 .size()
          << ";class_protocol_ref_count=" << class_protocol_ref_count
          << ";category_protocol_ref_count=" << category_protocol_ref_count
          << "\n";
      // canonical-runnable-object-sample anchor: emitted
      // realization records now publish the truthful object-sample execution
      // boundary above the D003 realized graph. The builtin selector count is
      // fixed to alloc/new/init, while metadata-rich category/protocol shapes
      // remain observable through the paired library/probe evidence path.
      // class-protocol-category conformance gate anchor:
      // A003/B004/C003/D004 remain the canonical proof surface that lane-E
      // consumes to decide whether classes/protocols/categories are executable
      // rather than merely modeled.
      // runnable class-protocol-category execution-matrix anchor:
      // the next lane-E matrix reuses those same emitted realization/runtime
      // boundaries and adds one linked inheritance executable instead of
      // broadening lowering with matrix-only metadata.
      out << "; runtime_canonical_runnable_object_sample_support = "
          << Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary()
          << ";class_bundle_count="
          << frontend_metadata_.runtime_metadata_class_metaclass_bundles_lexicographic
                 .size()
          << ";attached_category_candidate_count="
          << frontend_metadata_.runtime_metadata_category_bundles_lexicographic
                 .size()
          << ";builtin_object_sample_selector_count=3\n";
    }
    if (!selector_pool_globals_.empty() || !runtime_string_pool_globals_.empty()) {
      out << "; runtime_metadata_selector_string_pool_emission = "
          << Objc3RuntimeMetadataSelectorStringPoolEmissionSummary()
          << ";selector_pool_count=" << selector_pool_globals_.size()
          << ";string_pool_count=" << runtime_string_pool_globals_.size()
          << ";selector_section="
          << Objc3RuntimeMetadataHostSectionForLogicalName(
                 kObjc3RuntimeSelectorPoolLogicalSection)
          << ";string_section="
          << Objc3RuntimeMetadataHostSectionForLogicalName(
                 kObjc3RuntimeStringPoolLogicalSection)
          << "\n";
    }
    if (!typed_keypath_artifacts_.empty()) {
      // runtime-helper freeze anchor: the emitted descriptor
      // aggregate plus stable handle ordinals are the current runtime-facing
      // key-path boundary; full runtime evaluation helpers remain deferred to
      // the next lane-D issue.
      out << "; typed_keypath_artifact_emission = "
          << "contract=objc3c.type_system.typed.keypath.artifact.emission.v1"
          << ";keypath_count=" << typed_keypath_artifacts_.size()
          << ";section="
          << Objc3RuntimeMetadataHostSectionForLogicalName(
                 kObjc3RuntimeKeypathDescriptorLogicalSection)
          << ";aggregate_symbol=@__objc3_sec_keypath_descriptors"
          << ";generic_metadata_abi_replay_key="
          << (frontend_metadata_.lowering_generic_metadata_abi_replay_key.empty()
                  ? "none"
                  : frontend_metadata_.lowering_generic_metadata_abi_replay_key)
          << "\n";
    }
    out << "; runtime_metadata_binary_inspection_harness = "
        << Objc3RuntimeMetadataBinaryInspectionHarnessSummary()
        << ";positive_case_count=4"
        << ";negative_case_count=1"
        << ";section_inventory_command="
        << kObjc3RuntimeBinaryInspectionSectionCommand
        << ";symbol_inventory_command="
        << kObjc3RuntimeBinaryInspectionSymbolCommand << "\n";
    out << "; runtime_metadata_object_packaging_retention = "
        << Objc3RuntimeMetadataObjectPackagingRetentionSummary()
        << ";section_inventory_command="
        << kObjc3RuntimeBinaryInspectionSectionCommand
        << ";symbol_inventory_command="
        << kObjc3RuntimeBinaryInspectionSymbolCommand << "\n";
    out << "; runtime_metadata_linker_retention = "
        << Objc3RuntimeMetadataLinkerRetentionSummary()
        << ";linker_anchor_symbol=" << RuntimeMetadataLinkerAnchorSymbol()
        << ";discovery_root_symbol=" << RuntimeMetadataDiscoveryRootSymbol()
        << ";linker_anchor_logical_section="
        << kObjc3RuntimeLinkerAnchorLogicalSection
        << ";discovery_root_logical_section="
        << kObjc3RuntimeLinkerDiscoveryRootLogicalSection
        << ";linker_response_artifact_suffix="
        << kObjc3RuntimeLinkerResponseArtifactSuffix
        << ";discovery_artifact_suffix="
        << kObjc3RuntimeLinkerDiscoveryArtifactSuffix
        << ";translation_unit_identity_key="
        << frontend_metadata_
               .runtime_metadata_archive_static_link_translation_unit_identity_key
        << ";translation_unit_identity_key_hex="
        << EncodeBoundaryTokenValueHex(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_translation_unit_identity_key)
        << "\n";
    out << "; runtime_metadata_archive_static_link_discovery = "
        << Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary()
        << ";translation_unit_identity_key="
        << frontend_metadata_
               .runtime_metadata_archive_static_link_translation_unit_identity_key
        << ";translation_unit_identity_key_hex="
        << EncodeBoundaryTokenValueHex(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_translation_unit_identity_key)
        << ";merged_linker_response_artifact_suffix="
        << frontend_metadata_
               .runtime_metadata_archive_static_link_response_artifact_suffix
        << ";merged_discovery_artifact_suffix="
        << frontend_metadata_
               .runtime_metadata_archive_static_link_discovery_artifact_suffix
        << "\n";
    out << "; runtime_metadata_emission_gate = "
        << Objc3RuntimeMetadataEmissionGateSummary()
        << ";source_to_section_matrix_surface=objc3c.runtime.metadata.sourcetosectionmatrix.v1"
        << ";object_format_policy_surface=objc3c.runtime.metadata.objectformatpolicy.v1"
        << ";binary_inspection_surface=objc3c.runtime.metadata.binaryinspection.v1"
        << ";archive_static_link_surface=objc3c.runtime.metadata.archivestaticlink.v1"
        << "\n";
    out << "; runtime_metadata_object_emission_closeout = "
        << Objc3RuntimeMetadataObjectEmissionCloseoutSummary()
        << ";dependency_gate_surface=objc3c.runtime.metadata.dependencygate.v1"
        << ";class_object_case_surface=objc3c.runtime.metadata.sourcetosectionmatrix.v1"
        << ";binary_object_case_surface=objc3c.runtime.metadata.binaryinspection.v1"
        << ";linker_fanin_surface=objc3c.runtime.metadata.archivestaticlink.v1"
        << "\n";
    // manifest/object/IR truth gate anchor: publish the single artifact truth
    // boundary that the manifest, LLVM IR, native object, registration sidecars,
    // and release-claim sidecars must all satisfy together.
    out << "; manifest_object_ir_truth_gate = "
        << Objc3ManifestObjectIrTruthGateSummary()
        << ";manifest_artifact=module.manifest.json"
        << ";ir_artifact=module.ll"
        << ";object_artifact=module.obj"
        << ";registration_descriptor_artifact=module.runtime-registration-descriptor.json"
        << ";registration_manifest_artifact=module.runtime-registration-manifest.json"
        << ";conformance_report_artifact=module.objc3-conformance-report.json"
        << "\n";
    out << "; runtime metadata section publication globals\n";

    std::vector<std::string> retained_globals;
    retained_globals.reserve(layout_policy.total_retained_global_count);

    const auto emit_retained = [&](const std::string &symbol) {
      retained_globals.push_back(symbol);
    };

    std::unordered_map<std::string, std::string>
        protocol_descriptor_symbols_by_owner_identity;
    if (emit_protocol_category_bundle_payloads) {
      protocol_descriptor_symbols_by_owner_identity.reserve(
          frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic
              .size());
      for (std::size_t i = 0;
           i <
           frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic
               .size();
           ++i) {
        const auto &bundle =
            frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic[i];
        protocol_descriptor_symbols_by_owner_identity.emplace(
            bundle.owner_identity,
            BuildRuntimeMetadataDescriptorSymbol(
                layout_policy.descriptor_symbol_prefix,
                kObjc3RuntimeMetadataLayoutPolicyProtocolFamily, i));
      }
    }
    const auto build_method_list_key = [](const std::string &owner_family_kind,
                                          const std::string &owner_identity,
                                          const std::string &list_kind) {
      return owner_family_kind + "|" + owner_identity + "|" + list_kind;
    };
    std::unordered_map<std::string, std::string> method_list_symbols_by_key;
    std::unordered_map<std::string, std::size_t> method_list_entry_counts_by_key;
    std::unordered_map<std::string, std::string>
        implementation_method_symbols_by_owner_identity;
    std::string executable_method_binding_error;
    if (emit_member_table_payloads) {
      method_list_symbols_by_key.reserve(
          frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic
              .size());
      method_list_entry_counts_by_key.reserve(
          frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic
              .size());
      implementation_method_symbols_by_owner_identity.reserve(
          method_definitions_.size());
      for (std::size_t i = 0;
           i <
           frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic
               .size();
           ++i) {
        const auto &bundle =
            frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic
                [i];
        method_list_symbols_by_key.emplace(
            build_method_list_key(bundle.owner_family_kind,
                                  bundle.declaration_owner_identity,
                                  bundle.list_kind),
            BuildRuntimeMetadataAuxiliarySymbol(
                layout_policy.descriptor_symbol_prefix, bundle.owner_family_kind,
                bundle.list_kind + "_methods", i));
        method_list_entry_counts_by_key.emplace(
            build_method_list_key(bundle.owner_family_kind,
                                  bundle.declaration_owner_identity,
                                  bundle.list_kind),
            bundle.entries_lexicographic.size());
      }
      for (const MethodDefinition &method_def : method_definitions_) {
        if (method_def.method_owner_identity.empty()) {
          continue;
        }
        const std::string implementation_symbol = "@" + method_def.symbol;
        const auto [binding_it, inserted] =
            implementation_method_symbols_by_owner_identity.emplace(
                method_def.method_owner_identity, implementation_symbol);
        if (!inserted && binding_it->second != implementation_symbol) {
          executable_method_binding_error =
              "duplicate executable method-body binding for owner identity '" +
              method_def.method_owner_identity + "'";
          break;
        }
      }
      if (!executable_method_binding_error.empty()) {
        if (!fail_open_fallback_triggered_) {
          fail_open_fallback_triggered_ = true;
          fail_open_fallback_reason_ = executable_method_binding_error;
        }
        return;
      }
    }

    const std::string image_info_symbol = "@" + layout_policy.image_info_symbol;
    out << image_info_symbol << " = " << layout_policy.aggregate_linkage
        << " global { i32, i32 } zeroinitializer, section \""
        << layout_policy.emitted_image_info_section << "\", align 4\n";
    emit_retained(image_info_symbol);

    const auto emit_descriptor_section =
        [&](const std::string &kind,
            const std::string &section_name,
            const std::string &aggregate_symbol_name,
            std::size_t descriptor_count) {
          std::vector<std::string> descriptor_symbols;
          descriptor_symbols.reserve(descriptor_count);
          for (std::size_t i = 0; i < descriptor_count; ++i) {
            const std::string descriptor_symbol =
                BuildRuntimeMetadataDescriptorSymbol(
                    layout_policy.descriptor_symbol_prefix, kind, i);
            descriptor_symbols.push_back(descriptor_symbol);
            out << descriptor_symbol << " = " << layout_policy.descriptor_linkage
                << " global [1 x i8] zeroinitializer, section \""
                << section_name << "\", align 1\n";
            emit_retained(descriptor_symbol);
          }

          const std::string aggregate_symbol = "@" + aggregate_symbol_name;
          out << aggregate_symbol << " = " << layout_policy.aggregate_linkage
              << (descriptor_symbols.empty() ? " constant " : " global ");
          if (descriptor_symbols.empty()) {
            out << "{ i64 } { i64 0 }";
          } else {
            out << "{ i64, [" << descriptor_symbols.size()
                << " x ptr] } { i64 " << descriptor_symbols.size() << ", ["
                << descriptor_symbols.size() << " x ptr] [";
            for (std::size_t i = 0; i < descriptor_symbols.size(); ++i) {
              if (i != 0) {
                out << ", ";
              }
              out << "ptr " << descriptor_symbols[i];
            }
            out << "] }";
          }
          out << ", section \"" << section_name << "\", align 8\n";
          emit_retained(aggregate_symbol);
        };

    const auto emit_method_list_bundles_for_family =
        [&](const Objc3RuntimeMetadataLayoutPolicyFamily &family) {
          if (!emit_member_table_payloads) {
            return true;
          }
          for (std::size_t bundle_index = 0;
               bundle_index <
               frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic
                   .size();
               ++bundle_index) {
            const auto &bundle =
                frontend_metadata_
                    .runtime_metadata_method_list_bundles_lexicographic[bundle_index];
            if (bundle.owner_family_kind != family.kind) {
              continue;
            }

            const auto method_list_it = method_list_symbols_by_key.find(
                build_method_list_key(bundle.owner_family_kind,
                                      bundle.declaration_owner_identity,
                                      bundle.list_kind));
            if (method_list_it == method_list_symbols_by_key.end()) {
              continue;
            }

            const std::string list_symbol = method_list_it->second;
            const std::string owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    bundle.list_kind + "_methods_owner_identity", bundle_index);
            const std::string export_owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    bundle.list_kind + "_methods_export_owner_identity",
                    bundle_index);

            out << owner_identity_symbol << " = private constant ["
                << (bundle.declaration_owner_identity.size() + 1u)
                << " x i8] c\""
                << EscapeCStringLiteral(bundle.declaration_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << export_owner_identity_symbol << " = private constant ["
                << (bundle.export_owner_identity.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.export_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            std::vector<std::string> entry_initializers;
            entry_initializers.reserve(bundle.entries_lexicographic.size());
            for (std::size_t entry_index = 0;
                 entry_index < bundle.entries_lexicographic.size();
                 ++entry_index) {
              const auto &entry = bundle.entries_lexicographic[entry_index];
              const std::string bundle_ordinal =
                  FormatRuntimeMetadataDescriptorOrdinal(bundle_index);
              const std::string entry_ordinal =
                  FormatRuntimeMetadataDescriptorOrdinal(entry_index);
              const std::string selector_symbol =
                  "@" + layout_policy.descriptor_symbol_prefix + family.kind +
                  "_" + bundle.list_kind + "_method_selector_" +
                  bundle_ordinal + "_" + entry_ordinal;
              const std::string entry_owner_identity_symbol =
                  "@" + layout_policy.descriptor_symbol_prefix + family.kind +
                  "_" + bundle.list_kind + "_method_owner_identity_" +
                  bundle_ordinal + "_" + entry_ordinal;
              const std::string return_type_symbol =
                  "@" + layout_policy.descriptor_symbol_prefix + family.kind +
                  "_" + bundle.list_kind + "_method_return_type_" +
                  bundle_ordinal + "_" + entry_ordinal;
              std::string implementation_symbol = "null";
              if (entry.has_body &&
                  (bundle.owner_kind == "class-implementation" ||
                   bundle.owner_kind == "category-implementation")) {
                // executable object artifact lowering freeze anchor:
                // object emission binds implementation-owned method entries by
                // canonical owner identity to concrete LLVM definition symbols;
                // later executable-runtime work must extend this binding
                // surface instead of rediscovering bodies from source.
                // executable method-body binding anchor: the
                // implementation pointer is now a fail-closed requirement for
                // every implementation-owned executable method entry. If the
                // canonical method owner identity cannot resolve to exactly one
                // emitted LLVM body symbol, IR/object emission aborts instead
                // of silently leaving a null implementation slot.
                // slow-path anchor: class and category
                // implementation method-table entries now carry callable
                // implementation pointers from registered metadata.
                // slow-path anchor: class-implementation method-table
                // entries now carry callable implementation pointers so the
                // runtime can resolve live class/metaclass bodies from
                // registered metadata.
                // slow-path anchor: category-implementation
                // method-table entries now carry callable implementation
                // pointers so the runtime can resolve live category bodies from
                // registered metadata.
                const auto implementation_it =
                    implementation_method_symbols_by_owner_identity.find(
                        entry.owner_identity);
                if (implementation_it ==
                    implementation_method_symbols_by_owner_identity.end()) {
                  executable_method_binding_error =
                      "missing executable method-body binding for owner identity '" +
                      entry.owner_identity + "' in emitted " +
                      bundle.owner_kind + " " + bundle.list_kind +
                      " method list";
                  return false;
                }
                implementation_symbol = implementation_it->second;
              }

              out << selector_symbol << " = private constant ["
                  << (entry.selector.size() + 1u) << " x i8] c\""
                  << EscapeCStringLiteral(entry.selector)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
              out << entry_owner_identity_symbol << " = private constant ["
                  << (entry.owner_identity.size() + 1u) << " x i8] c\""
                  << EscapeCStringLiteral(entry.owner_identity)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
              out << return_type_symbol << " = private constant ["
                  << (entry.return_type_name.size() + 1u) << " x i8] c\""
                  << EscapeCStringLiteral(entry.return_type_name)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
              std::ostringstream entry_initializer;
              // dispatch-control lowering anchor: method entries now
              // preserve effective-direct and objc_final intent bits alongside
              // implementation bindings so emitted metadata matches the live
              // direct-call lowering surface.
              entry_initializer << "{ ptr, ptr, ptr, i64, ptr, i64, i1, i1 } { ptr "
                                << selector_symbol << ", ptr "
                                << entry_owner_identity_symbol << ", ptr "
                                << return_type_symbol << ", i64 "
                                << entry.parameter_count << ", ptr "
                                << implementation_symbol << ", i64 "
                                << (entry.has_body ? 1 : 0) << ", i1 "
                                << (entry.effective_direct_dispatch ? 1 : 0)
                                << ", i1 "
                                << (entry.objc_final_declared ? 1 : 0) << " }";
              entry_initializers.push_back(entry_initializer.str());
            }
            out << list_symbol << " = private global ";
            if (entry_initializers.empty()) {
              out << "{ i64, ptr, ptr } { i64 0, ptr " << owner_identity_symbol
                  << ", ptr " << export_owner_identity_symbol << " }";
            } else {
              out << "{ i64, ptr, ptr, [" << entry_initializers.size()
                  << " x { ptr, ptr, ptr, i64, ptr, i64, i1, i1 }] } { i64 "
                  << entry_initializers.size() << ", ptr "
                  << owner_identity_symbol << ", ptr "
                  << export_owner_identity_symbol << ", ["
                  << entry_initializers.size()
                  << " x { ptr, ptr, ptr, i64, ptr, i64, i1, i1 }] [";
              for (std::size_t entry_index = 0;
                   entry_index < entry_initializers.size(); ++entry_index) {
                if (entry_index != 0) {
                  out << ", ";
                }
                out << entry_initializers[entry_index];
              }
              out << "] }";
            }
            out << ", section \"" << family.emitted_section_name
                << "\", align 8\n";
            emit_retained(list_symbol);
          }
          return true;
        };

    const auto emit_property_descriptor_section =
        [&](const Objc3RuntimeMetadataLayoutPolicyFamily &family) {
          std::vector<std::string> descriptor_symbols;
          descriptor_symbols.reserve(
              frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                  .size());
          for (std::size_t i = 0;
               i <
               frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                   .size();
               ++i) {
            const auto &bundle =
                frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                    [i];
            const std::string descriptor_symbol =
                BuildRuntimeMetadataDescriptorSymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind, i);
            const std::string property_name_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "name", i);
            const std::string type_name_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "type", i);
            const std::string owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "owner_identity", i);
            const std::string declaration_owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "declaration_owner_identity", i);
            const std::string export_owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "export_owner_identity", i);
            const std::string getter_symbol = bundle.has_getter
                                                  ? BuildRuntimeMetadataAuxiliarySymbol(
                                                        layout_policy
                                                            .descriptor_symbol_prefix,
                                                        family.kind,
                                                        "getter_selector", i)
                                                  : std::string{"null"};
            const std::string setter_symbol = bundle.has_setter
                                                  ? BuildRuntimeMetadataAuxiliarySymbol(
                                                        layout_policy
                                                            .descriptor_symbol_prefix,
                                                        family.kind,
                                                        "setter_selector", i)
                                                  : std::string{"null"};
            const std::string effective_getter_symbol =
                bundle.effective_getter_selector.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "effective_getter_selector", i);
            const std::string effective_setter_symbol =
                bundle.effective_setter_available
                    ? BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "effective_setter_selector", i)
                    : std::string{"null"};
            const std::string ivar_binding_symbol =
                bundle.ivar_binding_symbol.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "ivar_binding", i);
            const std::string synthesized_binding_symbol =
                bundle.executable_synthesized_binding_symbol.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "synthesized_binding", i);
            const std::string ivar_layout_symbol =
                bundle.executable_ivar_layout_symbol.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "ivar_layout", i);
            const std::string ivar_layout_replay_key_symbol =
                bundle.executable_ivar_layout_replay_key.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "ivar_layout_replay", i);
            const std::string property_attribute_profile_symbol =
                bundle.property_attribute_profile.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "property_attribute_profile", i);
            const std::string ownership_lifetime_profile_symbol =
                bundle.ownership_lifetime_profile.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "ownership_lifetime_profile", i);
            const std::string ownership_runtime_hook_profile_symbol =
                bundle.ownership_runtime_hook_profile.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "ownership_runtime_hook_profile", i);
            const std::string accessor_ownership_profile_symbol =
                bundle.accessor_ownership_profile.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "accessor_ownership_profile", i);
            std::string getter_implementation_symbol = "null";
            std::string setter_implementation_symbol = "null";
            descriptor_symbols.push_back(descriptor_symbol);

            out << property_name_symbol << " = private constant ["
                << (bundle.property_name.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.property_name)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << type_name_symbol << " = private constant ["
                << (bundle.type_name.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.type_name)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << owner_identity_symbol << " = private constant ["
                << (bundle.owner_identity.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << declaration_owner_identity_symbol << " = private constant ["
                << (bundle.declaration_owner_identity.size() + 1u)
                << " x i8] c\""
                << EscapeCStringLiteral(bundle.declaration_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << export_owner_identity_symbol << " = private constant ["
                << (bundle.export_owner_identity.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.export_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            if (bundle.has_getter) {
              out << getter_symbol << " = private constant ["
                  << (bundle.getter_selector.size() + 1u) << " x i8] c\""
                  << EscapeCStringLiteral(bundle.getter_selector)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (bundle.has_setter) {
              out << setter_symbol << " = private constant ["
                  << (bundle.setter_selector.size() + 1u) << " x i8] c\""
                  << EscapeCStringLiteral(bundle.setter_selector)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.effective_getter_selector.empty()) {
              out << effective_getter_symbol << " = private constant ["
                  << (bundle.effective_getter_selector.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.effective_getter_selector)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (bundle.effective_setter_available) {
              out << effective_setter_symbol << " = private constant ["
                  << (bundle.effective_setter_selector.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.effective_setter_selector)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.ivar_binding_symbol.empty()) {
              out << ivar_binding_symbol << " = private constant ["
                  << (bundle.ivar_binding_symbol.size() + 1u) << " x i8] c\""
                  << EscapeCStringLiteral(bundle.ivar_binding_symbol)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.executable_synthesized_binding_symbol.empty()) {
              out << synthesized_binding_symbol << " = private constant ["
                  << (bundle.executable_synthesized_binding_symbol.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(
                         bundle.executable_synthesized_binding_symbol)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.executable_ivar_layout_symbol.empty()) {
              out << ivar_layout_symbol << " = private constant ["
                  << (bundle.executable_ivar_layout_symbol.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.executable_ivar_layout_symbol)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.executable_ivar_layout_replay_key.empty()) {
              out << ivar_layout_replay_key_symbol
                  << " = private constant ["
                  << (bundle.executable_ivar_layout_replay_key.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(
                         bundle.executable_ivar_layout_replay_key)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.property_attribute_profile.empty()) {
              out << property_attribute_profile_symbol
                  << " = private constant ["
                  << (bundle.property_attribute_profile.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.property_attribute_profile)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.ownership_lifetime_profile.empty()) {
              out << ownership_lifetime_profile_symbol
                  << " = private constant ["
                  << (bundle.ownership_lifetime_profile.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.ownership_lifetime_profile)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.ownership_runtime_hook_profile.empty()) {
              out << ownership_runtime_hook_profile_symbol
                  << " = private constant ["
                  << (bundle.ownership_runtime_hook_profile.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(
                         bundle.ownership_runtime_hook_profile)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.accessor_ownership_profile.empty()) {
              out << accessor_ownership_profile_symbol
                  << " = private constant ["
                  << (bundle.accessor_ownership_profile.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.accessor_ownership_profile)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (IsImplementationOwnedPropertyBundle(bundle)) {
              const auto getter_implementation_it =
                  implementation_method_symbols_by_owner_identity.find(
                      BuildSynthesizedInstanceMethodOwnerIdentity(
                          bundle.declaration_owner_identity,
                          bundle.effective_getter_selector));
              if (getter_implementation_it ==
                  implementation_method_symbols_by_owner_identity.end()) {
                executable_method_binding_error =
                    "missing synthesized accessor getter binding for property '" +
                    bundle.property_name + "' in owner '" +
                    bundle.declaration_owner_identity + "'";
                return false;
              }
              getter_implementation_symbol = getter_implementation_it->second;
              if (bundle.effective_setter_available) {
                const auto setter_implementation_it =
                    implementation_method_symbols_by_owner_identity.find(
                        BuildSynthesizedInstanceMethodOwnerIdentity(
                            bundle.declaration_owner_identity,
                            bundle.effective_setter_selector));
                if (setter_implementation_it ==
                    implementation_method_symbols_by_owner_identity.end()) {
                  executable_method_binding_error =
                      "missing synthesized accessor setter binding for property '" +
                      bundle.property_name + "' in owner '" +
                      bundle.declaration_owner_identity + "'";
                  return false;
                }
                setter_implementation_symbol =
                    setter_implementation_it->second;
              }
            }

            out << descriptor_symbol
                << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1, i1, i1, i1 } "
                   "{ ptr "
                << property_name_symbol << ", ptr " << type_name_symbol
                << ", ptr " << owner_identity_symbol << ", ptr "
                << declaration_owner_identity_symbol << ", ptr "
                << export_owner_identity_symbol << ", ptr " << getter_symbol
                << ", ptr " << setter_symbol << ", ptr "
                << effective_getter_symbol << ", ptr "
                << effective_setter_symbol << ", ptr " << ivar_binding_symbol
                << ", ptr " << synthesized_binding_symbol << ", ptr "
                << ivar_layout_symbol << ", ptr "
                << property_attribute_profile_symbol << ", ptr "
                << ownership_lifetime_profile_symbol << ", ptr "
                << ownership_runtime_hook_profile_symbol << ", ptr "
                << accessor_ownership_profile_symbol << ", ptr "
                << getter_implementation_symbol << ", ptr "
                << setter_implementation_symbol << ", ptr "
                << ivar_layout_replay_key_symbol << ", i64 "
                << bundle.executable_ivar_layout_slot_index << ", i64 "
                << bundle.executable_ivar_layout_size_bytes << ", i64 "
                << bundle.executable_ivar_layout_alignment_bytes << ", i64 "
                << bundle.executable_ivar_layout_offset_bytes << ", i64 "
                << bundle.executable_ivar_layout_padding_bytes << ", i64 "
                << bundle.executable_ivar_layout_inherited_slot_count
                << ", i64 "
                << bundle.executable_ivar_layout_inherited_size_bytes
                << ", i64 "
                << bundle.executable_ivar_layout_owner_size_bytes << ", i64 "
                << bundle.executable_ivar_init_order_index << ", i64 "
                << bundle.executable_ivar_destroy_order_index << ", i1 "
                << (bundle.executable_ivar_layout_valid ? 1 : 0) << ", i1 "
                << (bundle.has_getter ? 1 : 0) << ", i1 "
                << (bundle.has_setter ? 1 : 0) << ", i1 "
                << (bundle.effective_setter_available ? 1 : 0)
                << " }, section \""
                << family.emitted_section_name << "\", align 8\n";
            emit_retained(descriptor_symbol);
          }

          const std::string aggregate_symbol = "@" + family.aggregate_symbol_name;
          out << aggregate_symbol << " = " << layout_policy.aggregate_linkage
              << (descriptor_symbols.empty() ? " constant " : " global ");
          if (descriptor_symbols.empty()) {
            out << "{ i64 } { i64 0 }";
          } else {
            out << "{ i64, [" << descriptor_symbols.size()
                << " x ptr] } { i64 " << descriptor_symbols.size() << ", ["
                << descriptor_symbols.size() << " x ptr] [";
            for (std::size_t i = 0; i < descriptor_symbols.size(); ++i) {
              if (i != 0) {
                out << ", ";
              }
              out << "ptr " << descriptor_symbols[i];
            }
            out << "] }";
          }
          out << ", section \"" << family.emitted_section_name
              << "\", align 8\n";
          emit_retained(aggregate_symbol);
          return true;
        };

    const auto emit_ivar_descriptor_section =
        [&](const Objc3RuntimeMetadataLayoutPolicyFamily &family) {
          std::map<std::string, std::size_t> instance_size_by_owner_identity;

          std::vector<std::string> descriptor_symbols;
          descriptor_symbols.reserve(
              frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                  .size());
          std::map<std::string,
                   std::vector<std::pair<std::size_t, std::string>>>
              descriptor_symbols_by_declaration_owner_identity;
          for (std::size_t i = 0;
               i <
               frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                   .size();
               ++i) {
            const auto &bundle =
                frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                    [i];
            const std::string descriptor_symbol =
                BuildRuntimeMetadataDescriptorSymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind, i);
            const std::string owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "owner_identity", i);
            const std::string declaration_owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "declaration_owner_identity", i);
            const std::string export_owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "export_owner_identity", i);
            const std::string property_owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "property_owner_identity", i);
            const std::string property_name_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "property_name", i);
            const std::string ivar_binding_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "binding", i);
            const std::string layout_symbol =
                bundle.executable_ivar_layout_symbol.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "layout_symbol", i);
            const std::string offset_global_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "offset", i);
            const std::string layout_record_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "layout_record", i);
            const std::string layout_replay_key_symbol =
                bundle.executable_ivar_layout_replay_key.empty()
                    ? std::string{"null"}
                    : BuildRuntimeMetadataAuxiliarySymbol(
                          layout_policy.descriptor_symbol_prefix, family.kind,
                          "layout_replay", i);
            descriptor_symbols.push_back(descriptor_symbol);
            descriptor_symbols_by_declaration_owner_identity
                [bundle.declaration_owner_identity]
                    .push_back(std::make_pair(
                        bundle.executable_ivar_layout_slot_index,
                        descriptor_symbol));
            instance_size_by_owner_identity[bundle.declaration_owner_identity] =
                std::max(instance_size_by_owner_identity
                             [bundle.declaration_owner_identity],
                         bundle.executable_ivar_layout_owner_size_bytes);

            out << owner_identity_symbol << " = private constant ["
                << (bundle.owner_identity.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << declaration_owner_identity_symbol << " = private constant ["
                << (bundle.declaration_owner_identity.size() + 1u)
                << " x i8] c\""
                << EscapeCStringLiteral(bundle.declaration_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << export_owner_identity_symbol << " = private constant ["
                << (bundle.export_owner_identity.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.export_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << property_owner_identity_symbol << " = private constant ["
                << (bundle.property_owner_identity.size() + 1u)
                << " x i8] c\""
                << EscapeCStringLiteral(bundle.property_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << property_name_symbol << " = private constant ["
                << (bundle.property_name.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.property_name)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << ivar_binding_symbol << " = private constant ["
                << (bundle.ivar_binding_symbol.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.ivar_binding_symbol)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            if (!bundle.executable_ivar_layout_symbol.empty()) {
              out << layout_symbol << " = private constant ["
                  << (bundle.executable_ivar_layout_symbol.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.executable_ivar_layout_symbol)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            if (!bundle.executable_ivar_layout_replay_key.empty()) {
              out << layout_replay_key_symbol << " = private constant ["
                  << (bundle.executable_ivar_layout_replay_key.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(
                         bundle.executable_ivar_layout_replay_key)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            out << offset_global_symbol << " = private global i64 "
                << bundle.executable_ivar_layout_offset_bytes << ", section \""
                << family.emitted_section_name << "\", align 8\n";
            emit_retained(offset_global_symbol);
            out << layout_record_symbol
                << " = private global { ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1 } { ptr "
                << layout_symbol << ", ptr " << layout_replay_key_symbol
                << ", i64 "
                << bundle.executable_ivar_layout_slot_index << ", i64 "
                << bundle.executable_ivar_layout_offset_bytes << ", i64 "
                << bundle.executable_ivar_layout_size_bytes << ", i64 "
                << bundle.executable_ivar_layout_alignment_bytes
                << ", i64 " << bundle.executable_ivar_layout_padding_bytes
                << ", i64 "
                << bundle.executable_ivar_layout_inherited_slot_count
                << ", i64 "
                << bundle.executable_ivar_layout_inherited_size_bytes
                << ", i64 "
                << bundle.executable_ivar_layout_owner_size_bytes
                << ", i64 " << bundle.executable_ivar_init_order_index
                << ", i64 " << bundle.executable_ivar_destroy_order_index
                << ", i1 "
                << (bundle.executable_ivar_layout_valid ? 1 : 0)
                << " }, section \"" << family.emitted_section_name
                << "\", align 8\n";
            emit_retained(layout_record_symbol);

            out << descriptor_symbol
                << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i1 } { ptr "
                << owner_identity_symbol << ", ptr "
                << declaration_owner_identity_symbol << ", ptr "
                << export_owner_identity_symbol << ", ptr "
                << property_owner_identity_symbol << ", ptr "
                << property_name_symbol << ", ptr " << ivar_binding_symbol
                << ", ptr " << layout_record_symbol << ", ptr "
                << offset_global_symbol << ", ptr "
                << layout_replay_key_symbol << ", i64 "
                << bundle.executable_ivar_layout_slot_index << ", i64 "
                << bundle.executable_ivar_layout_offset_bytes << ", i64 "
                << bundle.executable_ivar_layout_size_bytes << ", i64 "
                << bundle.executable_ivar_layout_alignment_bytes
                << ", i64 " << bundle.executable_ivar_layout_padding_bytes
                << ", i64 "
                << bundle.executable_ivar_layout_inherited_slot_count
                << ", i64 "
                << bundle.executable_ivar_layout_inherited_size_bytes
                << ", i64 "
                << bundle.executable_ivar_layout_owner_size_bytes
                << ", i64 " << bundle.executable_ivar_init_order_index
                << ", i64 " << bundle.executable_ivar_destroy_order_index
                << ", i1 "
                << (bundle.executable_ivar_layout_valid ? 1 : 0)
                << " }, section \"" << family.emitted_section_name
                << "\", align 8\n";
            emit_retained(descriptor_symbol);
          }

          std::size_t layout_table_ordinal = 0u;
          for (auto &[owner_identity, owner_descriptor_symbols] :
               descriptor_symbols_by_declaration_owner_identity) {
            std::stable_sort(
                owner_descriptor_symbols.begin(),
                owner_descriptor_symbols.end(),
                [](const auto &lhs, const auto &rhs) {
                  return lhs.first < rhs.first ||
                         (lhs.first == rhs.first && lhs.second < rhs.second);
                });
            const std::string owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "layout_owner", layout_table_ordinal);
            const std::string layout_table_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "layout_table", layout_table_ordinal);
            out << owner_identity_symbol << " = private constant ["
                << (owner_identity.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << layout_table_symbol << " = private global ";
            if (owner_descriptor_symbols.empty()) {
              out << "{ ptr, i64, i64 } { ptr " << owner_identity_symbol
                  << ", i64 0, i64 0 }";
            } else {
              out << "{ ptr, i64, [" << owner_descriptor_symbols.size()
                  << " x ptr], i64 } { ptr " << owner_identity_symbol
                  << ", i64 " << owner_descriptor_symbols.size() << ", ["
                  << owner_descriptor_symbols.size() << " x ptr] [";
              for (std::size_t symbol_index = 0u;
                   symbol_index < owner_descriptor_symbols.size();
                   ++symbol_index) {
                if (symbol_index != 0u) {
                  out << ", ";
                }
                out << "ptr " << owner_descriptor_symbols[symbol_index].second;
              }
              out << "], i64 "
                  << instance_size_by_owner_identity[owner_identity] << " }";
            }
            out << ", section \"" << family.emitted_section_name
                << "\", align 8\n";
            emit_retained(layout_table_symbol);
            ++layout_table_ordinal;
          }

          const std::string aggregate_symbol = "@" + family.aggregate_symbol_name;
          out << aggregate_symbol << " = " << layout_policy.aggregate_linkage
              << (descriptor_symbols.empty() ? " constant " : " global ");
          if (descriptor_symbols.empty()) {
            out << "{ i64 } { i64 0 }";
          } else {
            out << "{ i64, [" << descriptor_symbols.size()
                << " x ptr] } { i64 " << descriptor_symbols.size() << ", ["
                << descriptor_symbols.size() << " x ptr] [";
            for (std::size_t i = 0; i < descriptor_symbols.size(); ++i) {
              if (i != 0) {
                out << ", ";
              }
              out << "ptr " << descriptor_symbols[i];
            }
            out << "] }";
          }
          out << ", section \"" << family.emitted_section_name
              << "\", align 8\n";
          emit_retained(aggregate_symbol);
        };

    const auto emit_class_metaclass_bundle_section =
        [&](const Objc3RuntimeMetadataLayoutPolicyFamily &family) {
          std::unordered_map<std::string, std::string> descriptor_symbols_by_owner_identity;
          descriptor_symbols_by_owner_identity.reserve(
              frontend_metadata_
                  .runtime_metadata_class_metaclass_bundles_lexicographic.size());
          for (std::size_t i = 0;
               i < frontend_metadata_
                       .runtime_metadata_class_metaclass_bundles_lexicographic
                           .size();
               ++i) {
            const auto &bundle =
                frontend_metadata_
                    .runtime_metadata_class_metaclass_bundles_lexicographic[i];
            descriptor_symbols_by_owner_identity.emplace(
                bundle.owner_identity,
                BuildRuntimeMetadataDescriptorSymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind, i));
          }

          std::vector<std::string> descriptor_symbols;
          descriptor_symbols.reserve(
              frontend_metadata_
                  .runtime_metadata_class_metaclass_bundles_lexicographic.size());
          for (std::size_t i = 0;
               i < frontend_metadata_
                       .runtime_metadata_class_metaclass_bundles_lexicographic
                           .size();
               ++i) {
            const auto &bundle =
                frontend_metadata_
                    .runtime_metadata_class_metaclass_bundles_lexicographic[i];
            const std::string descriptor_symbol =
                BuildRuntimeMetadataDescriptorSymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind, i);
            const std::string name_symbol = BuildRuntimeMetadataAuxiliarySymbol(
                layout_policy.descriptor_symbol_prefix, family.kind, "name", i);
            const std::string owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "owner_identity", i);
            const std::string class_object_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "class_object_identity", i);
            const std::string metaclass_object_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "metaclass_object_identity", i);
            const std::string instance_method_list_ref_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "instance_method_list_ref", i);
            const std::string metaclass_method_list_ref_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, "metaclass",
                    "method_list_ref", i);
            const std::string adopted_protocol_refs_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "adopted_protocol_refs", i);
            std::string class_super_object_identity_symbol = "null";
            std::string metaclass_super_object_identity_symbol = "null";
            if (bundle.has_super) {
              class_super_object_identity_symbol =
                  BuildRuntimeMetadataAuxiliarySymbol(
                      layout_policy.descriptor_symbol_prefix, family.kind,
                      "super_class_object_identity", i);
              metaclass_super_object_identity_symbol =
                  BuildRuntimeMetadataAuxiliarySymbol(
                      layout_policy.descriptor_symbol_prefix, family.kind,
                      "super_metaclass_object_identity", i);
            }
            std::string instance_method_list_symbol = "null";
            const auto instance_method_list_it = method_list_symbols_by_key.find(
                build_method_list_key(family.kind, bundle.owner_identity,
                                      "instance"));
            if (instance_method_list_it != method_list_symbols_by_key.end()) {
              instance_method_list_symbol = instance_method_list_it->second;
            }
            std::size_t instance_method_list_entry_count = 0;
            const auto instance_method_count_it =
                method_list_entry_counts_by_key.find(build_method_list_key(
                    family.kind, bundle.owner_identity, "instance"));
            if (instance_method_count_it != method_list_entry_counts_by_key.end()) {
              instance_method_list_entry_count =
                  instance_method_count_it->second;
            }
            std::string metaclass_method_list_symbol = "null";
            const auto metaclass_method_list_it = method_list_symbols_by_key.find(
                build_method_list_key(family.kind, bundle.owner_identity,
                                      "class"));
            if (metaclass_method_list_it != method_list_symbols_by_key.end()) {
              metaclass_method_list_symbol = metaclass_method_list_it->second;
            }
            std::size_t metaclass_method_list_entry_count = 0;
            const auto metaclass_method_count_it =
                method_list_entry_counts_by_key.find(build_method_list_key(
                    family.kind, bundle.owner_identity, "class"));
            if (metaclass_method_count_it !=
                method_list_entry_counts_by_key.end()) {
              metaclass_method_list_entry_count =
                  metaclass_method_count_it->second;
            }
            std::string super_bundle_symbol = "null";
            if (bundle.has_super) {
              const auto super_it =
                  descriptor_symbols_by_owner_identity.find(
                      bundle.super_bundle_owner_identity);
              if (super_it != descriptor_symbols_by_owner_identity.end()) {
                super_bundle_symbol = super_it->second;
              }
            }
            const std::size_t storage_len = bundle.class_name.size() + 1u;
            const std::size_t owner_identity_storage_len =
                bundle.owner_identity.size() + 1u;
            const std::size_t class_object_identity_storage_len =
                bundle.class_owner_identity.size() + 1u;
            const std::size_t metaclass_object_identity_storage_len =
                bundle.metaclass_owner_identity.size() + 1u;
            descriptor_symbols.push_back(descriptor_symbol);

            out << name_symbol << " = private constant [" << storage_len
                << " x i8] c\""
                << EscapeCStringLiteral(bundle.class_name)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << owner_identity_symbol << " = private constant ["
                << owner_identity_storage_len << " x i8] c\""
                << EscapeCStringLiteral(bundle.owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << class_object_identity_symbol << " = private constant ["
                << class_object_identity_storage_len << " x i8] c\""
                << EscapeCStringLiteral(bundle.class_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << metaclass_object_identity_symbol << " = private constant ["
                << metaclass_object_identity_storage_len << " x i8] c\""
                << EscapeCStringLiteral(bundle.metaclass_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            if (bundle.has_super) {
              out << class_super_object_identity_symbol
                  << " = private constant ["
                  << (bundle.super_class_owner_identity.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.super_class_owner_identity)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
              out << metaclass_super_object_identity_symbol
                  << " = private constant ["
                  << (bundle.super_metaclass_owner_identity.size() + 1u)
                  << " x i8] c\""
                  << EscapeCStringLiteral(bundle.super_metaclass_owner_identity)
                  << "\\00\", section \"" << family.emitted_section_name
                  << "\", align 1\n";
            }
            out << instance_method_list_ref_symbol
                << " = private global { i64, ptr, ptr } { i64 "
                << instance_method_list_entry_count << ", ptr "
                << owner_identity_symbol << ", ptr "
                << instance_method_list_symbol
                << " }, section \"" << family.emitted_section_name
                << "\", align 8\n";
            out << metaclass_method_list_ref_symbol
                << " = private global { i64, ptr, ptr } { i64 "
                << metaclass_method_list_entry_count << ", ptr "
                << owner_identity_symbol << ", ptr "
                << metaclass_method_list_symbol
                << " }, section \"" << family.emitted_section_name
                << "\", align 8\n";
            out << adopted_protocol_refs_symbol << " = private global ";
            if (bundle.adopted_protocol_owner_identities_lexicographic.empty()) {
              out << "{ i64 } { i64 0 }";
            } else {
              out << "{ i64, ["
                  << bundle.adopted_protocol_owner_identities_lexicographic.size()
                  << " x ptr] } { i64 "
                  << bundle.adopted_protocol_owner_identities_lexicographic.size()
                  << ", ["
                  << bundle.adopted_protocol_owner_identities_lexicographic.size()
                  << " x ptr] [";
              for (std::size_t adopted_index = 0;
                   adopted_index <
                   bundle.adopted_protocol_owner_identities_lexicographic.size();
                   ++adopted_index) {
                if (adopted_index != 0) {
                  out << ", ";
                }
                std::string adopted_protocol_symbol = "null";
                const auto adopted_it =
                    protocol_descriptor_symbols_by_owner_identity.find(
                        bundle.adopted_protocol_owner_identities_lexicographic
                            [adopted_index]);
                if (adopted_it !=
                    protocol_descriptor_symbols_by_owner_identity.end()) {
                  adopted_protocol_symbol = adopted_it->second;
                }
                out << "ptr " << adopted_protocol_symbol;
              }
              out << "] }";
            }
            out << ", section \"" << family.emitted_section_name
                << "\", align 8\n";
            // executable realization-record expansion anchor: each
            // class/metaclass record now preserves bundle-owner, object-owner,
            // and super-object identities in-line with the realized bundle
            // pointer plus the existing owner-scoped method-list ref.
            // dispatch-control lowering anchor: realized
            // class/metaclass descriptor bundles now preserve objc_final and
            // objc_sealed container intent as explicit metadata bits.
            out << descriptor_symbol
                << " = private global { { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } } "
                   "{ { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } { ptr "
                << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
                << class_object_identity_symbol << ", ptr "
                << class_super_object_identity_symbol << ", ptr "
