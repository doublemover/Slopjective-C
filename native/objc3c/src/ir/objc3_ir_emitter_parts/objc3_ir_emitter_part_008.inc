                << super_bundle_symbol << ", ptr "
                << instance_method_list_ref_symbol << ", ptr "
                << adopted_protocol_refs_symbol << ", i1 "
                << (bundle.objc_final_declared ? 1 : 0) << ", i1 "
                << (bundle.objc_sealed_declared ? 1 : 0)
                << " }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } { ptr "
                << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
                << metaclass_object_identity_symbol << ", ptr "
                << metaclass_super_object_identity_symbol << ", ptr "
                << super_bundle_symbol << ", ptr "
                << metaclass_method_list_ref_symbol << ", ptr "
                << adopted_protocol_refs_symbol << ", i1 "
                << (bundle.objc_final_declared ? 1 : 0) << ", i1 "
                << (bundle.objc_sealed_declared ? 1 : 0)
                << " } }, section \""
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
        };

    const auto emit_protocol_bundle_section =
        [&](const Objc3RuntimeMetadataLayoutPolicyFamily &family) {
          // protocol/category data emission anchor: protocol records
          // now materialize as real descriptor bundles with inherited
          // protocol-ref lists inside the protocol descriptor section.
          std::vector<std::string> descriptor_symbols;
          descriptor_symbols.reserve(
              frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic
                  .size());
          for (std::size_t i = 0;
               i <
               frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic
                   .size();
               ++i) {
            const auto &bundle =
                frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic
                    [i];
            const std::string descriptor_symbol =
                BuildRuntimeMetadataDescriptorSymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind, i);
            const std::string name_symbol = BuildRuntimeMetadataAuxiliarySymbol(
                layout_policy.descriptor_symbol_prefix, family.kind, "name", i);
            const std::string owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "owner_identity", i);
            const std::string inherited_refs_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "inherited_protocol_refs", i);
            const std::string instance_method_list_ref_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "instance_method_list_ref", i);
            const std::string class_method_list_ref_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "class_method_list_ref", i);
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
            std::string class_method_list_symbol = "null";
            const auto class_method_list_it = method_list_symbols_by_key.find(
                build_method_list_key(family.kind, bundle.owner_identity,
                                      "class"));
            if (class_method_list_it != method_list_symbols_by_key.end()) {
              class_method_list_symbol = class_method_list_it->second;
            }
            std::size_t class_method_list_entry_count = 0;
            const auto class_method_count_it =
                method_list_entry_counts_by_key.find(build_method_list_key(
                    family.kind, bundle.owner_identity, "class"));
            if (class_method_count_it != method_list_entry_counts_by_key.end()) {
              class_method_list_entry_count = class_method_count_it->second;
            }
            const std::size_t name_storage_len = bundle.protocol_name.size() + 1u;
            const std::size_t owner_identity_storage_len =
                bundle.owner_identity.size() + 1u;
            descriptor_symbols.push_back(descriptor_symbol);

            out << name_symbol << " = private constant [" << name_storage_len
                << " x i8] c\"" << EscapeCStringLiteral(bundle.protocol_name)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << owner_identity_symbol << " = private constant ["
                << owner_identity_storage_len << " x i8] c\""
                << EscapeCStringLiteral(bundle.owner_identity) << "\\00\", section \""
                << family.emitted_section_name << "\", align 1\n";
            out << inherited_refs_symbol << " = private global ";
            if (bundle.inherited_protocol_owner_identities_lexicographic.empty()) {
              out << "{ i64 } { i64 0 }";
            } else {
              out << "{ i64, ["
                  << bundle.inherited_protocol_owner_identities_lexicographic.size()
                  << " x ptr] } { i64 "
                  << bundle.inherited_protocol_owner_identities_lexicographic.size()
                  << ", ["
                  << bundle.inherited_protocol_owner_identities_lexicographic.size()
                  << " x ptr] [";
              for (std::size_t inherited_index = 0;
                   inherited_index <
                   bundle.inherited_protocol_owner_identities_lexicographic.size();
                   ++inherited_index) {
                if (inherited_index != 0) {
                  out << ", ";
                }
                std::string inherited_protocol_symbol = "null";
                const auto inherited_it =
                    protocol_descriptor_symbols_by_owner_identity.find(
                        bundle.inherited_protocol_owner_identities_lexicographic
                            [inherited_index]);
                if (inherited_it !=
                    protocol_descriptor_symbols_by_owner_identity.end()) {
                  inherited_protocol_symbol = inherited_it->second;
                }
                out << "ptr " << inherited_protocol_symbol;
              }
              out << "] }";
            }
            out << ", section \"" << family.emitted_section_name
                << "\", align 8\n";
            out << instance_method_list_ref_symbol
                << " = private global { i64, ptr, ptr } { i64 "
                << instance_method_list_entry_count << ", ptr "
                << owner_identity_symbol
                << ", ptr " << instance_method_list_symbol << " }, section \""
                << family.emitted_section_name << "\", align 8\n";
            out << class_method_list_ref_symbol
                << " = private global { i64, ptr, ptr } { i64 "
                << class_method_list_entry_count << ", ptr "
                << owner_identity_symbol << ", ptr " << class_method_list_symbol
                << " }, section \"" << family.emitted_section_name
                << "\", align 8\n";
            // executable realization-record expansion anchor:
            // protocol records now preserve split instance/class method counts
            // alongside inherited protocol edges so runtime conformance checks
            // do not need to reconstruct that split from sidecar summaries.
            out << descriptor_symbol
                << " = private global { ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i1 } { ptr "
                << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
                << inherited_refs_symbol << ", ptr "
                << instance_method_list_ref_symbol << ", ptr "
                << class_method_list_ref_symbol << ", i64 "
                << bundle.property_count << ", i64 " << bundle.method_count
                << ", i64 " << instance_method_list_entry_count << ", i64 "
                << class_method_list_entry_count
                << ", i1 "
                << (bundle.is_forward_declaration ? 1 : 0) << " }, section \""
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
        };

    const auto emit_category_bundle_section =
        [&](const Objc3RuntimeMetadataLayoutPolicyFamily &family) {
          // protocol/category data emission anchor: category records
          // now materialize as interface/implementation descriptor bundles with
          // attachment lists and adopted protocol-ref lists inside the category
          // descriptor section.
          std::vector<std::string> descriptor_symbols;
          descriptor_symbols.reserve(
              frontend_metadata_.runtime_metadata_category_bundles_lexicographic
                  .size());
          for (std::size_t i = 0;
               i <
               frontend_metadata_.runtime_metadata_category_bundles_lexicographic
                   .size();
               ++i) {
            const auto &bundle =
                frontend_metadata_.runtime_metadata_category_bundles_lexicographic
                    [i];
            const std::string descriptor_symbol =
                BuildRuntimeMetadataDescriptorSymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind, i);
            const std::string class_name_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "class_name", i);
            const std::string category_name_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "category_name", i);
            const std::string owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "owner_identity", i);
            const std::string record_kind_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "record_kind", i);
            const std::string category_owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "category_owner_identity", i);
            const std::string class_owner_identity_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "class_owner_identity", i);
            const std::string attachment_list_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "attachments", i);
            const std::string adopted_protocol_refs_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "adopted_protocol_refs", i);
            const std::string instance_method_list_ref_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "instance_method_list_ref", i);
            const std::string class_method_list_ref_symbol =
                BuildRuntimeMetadataAuxiliarySymbol(
                    layout_policy.descriptor_symbol_prefix, family.kind,
                    "class_method_list_ref", i);
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
            std::string class_method_list_symbol = "null";
            const auto class_method_list_it = method_list_symbols_by_key.find(
                build_method_list_key(family.kind, bundle.owner_identity,
                                      "class"));
            if (class_method_list_it != method_list_symbols_by_key.end()) {
              class_method_list_symbol = class_method_list_it->second;
            }
            std::size_t class_method_list_entry_count = 0;
            const auto class_method_count_it =
                method_list_entry_counts_by_key.find(build_method_list_key(
                    family.kind, bundle.owner_identity, "class"));
            if (class_method_count_it != method_list_entry_counts_by_key.end()) {
              class_method_list_entry_count = class_method_count_it->second;
            }
            descriptor_symbols.push_back(descriptor_symbol);

            out << class_name_symbol << " = private constant ["
                << (bundle.class_name.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.class_name) << "\\00\", section \""
                << family.emitted_section_name << "\", align 1\n";
            out << category_name_symbol << " = private constant ["
                << (bundle.category_name.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.category_name)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << owner_identity_symbol << " = private constant ["
                << (bundle.owner_identity.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << record_kind_symbol << " = private constant ["
                << (bundle.record_kind.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.record_kind)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << category_owner_identity_symbol << " = private constant ["
                << (bundle.category_owner_identity.size() + 1u)
                << " x i8] c\""
                << EscapeCStringLiteral(bundle.category_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";
            out << class_owner_identity_symbol << " = private constant ["
                << (bundle.class_owner_identity.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(bundle.class_owner_identity)
                << "\\00\", section \"" << family.emitted_section_name
                << "\", align 1\n";

            out << attachment_list_symbol << " = private global ";
            const std::size_t attachment_count = 3u;
            out << "{ i64, [" << attachment_count << " x ptr] } { i64 "
                << attachment_count << ", [" << attachment_count << " x ptr] [";
            out << "ptr " << class_owner_identity_symbol << ", ptr "
                << category_owner_identity_symbol << ", ptr "
                << owner_identity_symbol;
            out << "] }, section \"" << family.emitted_section_name
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
            out << instance_method_list_ref_symbol
                << " = private global { i64, ptr, ptr } { i64 "
                << instance_method_list_entry_count << ", ptr "
                << owner_identity_symbol << ", ptr "
                << instance_method_list_symbol << " }, section \""
                << family.emitted_section_name << "\", align 8\n";
            out << class_method_list_ref_symbol
                << " = private global { i64, ptr, ptr } { i64 "
                << class_method_list_entry_count << ", ptr "
                << owner_identity_symbol << ", ptr "
                << class_method_list_symbol << " }, section \""
                << family.emitted_section_name << "\", align 8\n";

            // executable realization-record expansion anchor:
            // category records now preserve explicit class/category owner
            // identities in-line while retaining the earlier attachment and
            // adopted-protocol aggregates for backward-compatible proofing.
            out << descriptor_symbol
                << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64 } "
                   "{ ptr "
                << class_name_symbol << ", ptr " << category_name_symbol
                << ", ptr " << record_kind_symbol << ", ptr "
                << owner_identity_symbol << ", ptr "
                << class_owner_identity_symbol << ", ptr "
                << category_owner_identity_symbol << ", ptr "
                << attachment_list_symbol << ", ptr "
                << adopted_protocol_refs_symbol << ", ptr "
                << instance_method_list_ref_symbol << ", ptr "
                << class_method_list_ref_symbol << ", i64 "
                << bundle.property_count << ", i64 "
                << bundle.instance_method_count << ", i64 "
                << bundle.class_method_count
                << " }, section \"" << family.emitted_section_name
                << "\", align 8\n";
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
        };

    const auto emit_canonical_pool_section =
        [&](const std::map<std::string, std::string> &pool_globals,
            const std::string &aggregate_symbol,
            const std::string &logical_section) {
          const std::string emitted_section_name =
              Objc3RuntimeMetadataHostSectionForLogicalName(logical_section);
          for (const auto &entry : pool_globals) {
            const std::string &value = entry.first;
            const std::string &symbol = entry.second;
            out << symbol << " = private unnamed_addr constant ["
                << (value.size() + 1u) << " x i8] c\""
                << EscapeCStringLiteral(value) << "\\00\", section \""
                << emitted_section_name << "\", align 1\n";
          }
          out << aggregate_symbol << " = internal "
              << (pool_globals.empty() ? "constant " : "global ");
          if (pool_globals.empty()) {
            out << "{ i64 } { i64 0 }";
          } else {
            out << "{ i64, [" << pool_globals.size()
                << " x ptr] } { i64 " << pool_globals.size() << ", ["
                << pool_globals.size() << " x ptr] [";
            std::size_t index = 0;
            for (const auto &entry : pool_globals) {
              if (index++ != 0) {
                out << ", ";
              }
              out << "ptr " << entry.second;
            }
            out << "] }";
          }
          out << ", section \"" << emitted_section_name << "\", align 8\n";
          emit_retained(aggregate_symbol);
        };

    for (const auto &family : layout_policy.families) {
      if (emit_member_table_payloads &&
          (family.kind == kObjc3RuntimeMetadataLayoutPolicyClassFamily ||
           family.kind == kObjc3RuntimeMetadataLayoutPolicyProtocolFamily ||
           family.kind == kObjc3RuntimeMetadataLayoutPolicyCategoryFamily)) {
        if (!emit_method_list_bundles_for_family(family)) {
          if (!unsupported_fail_closed_path_triggered_) {
            unsupported_fail_closed_path_triggered_ = true;
            unsupported_fail_closed_path_reason_ =
                executable_method_binding_error.empty()
                    ? "runtime metadata method-body binding failed"
                    : executable_method_binding_error;
          }
          return;
        }
      }
      if (emit_class_metaclass_bundle_payloads &&
          family.kind == kObjc3RuntimeMetadataLayoutPolicyClassFamily) {
        emit_class_metaclass_bundle_section(family);
      } else if (emit_protocol_category_bundle_payloads &&
                 family.kind == kObjc3RuntimeMetadataLayoutPolicyProtocolFamily) {
        emit_protocol_bundle_section(family);
      } else if (emit_protocol_category_bundle_payloads &&
                 family.kind == kObjc3RuntimeMetadataLayoutPolicyCategoryFamily) {
        emit_category_bundle_section(family);
      } else if (emit_member_table_payloads &&
                 family.kind == kObjc3RuntimeMetadataLayoutPolicyPropertyFamily) {
        emit_property_descriptor_section(family);
      } else if (emit_member_table_payloads &&
                 family.kind == kObjc3RuntimeMetadataLayoutPolicyIvarFamily) {
        emit_ivar_descriptor_section(family);
      } else {
        emit_descriptor_section(family.kind, family.emitted_section_name,
                                family.aggregate_symbol_name,
                                family.descriptor_count);
      }
    }

    const bool emit_selector_string_pools =
        !selector_pool_globals_.empty() || !runtime_string_pool_globals_.empty();
    if (emit_selector_string_pools) {
      emit_canonical_pool_section(selector_pool_globals_,
                                  "@__objc3_sec_selector_pool",
                                  kObjc3RuntimeSelectorPoolLogicalSection);
      emit_canonical_pool_section(runtime_string_pool_globals_,
                                  "@__objc3_sec_string_pool",
                                  kObjc3RuntimeStringPoolLogicalSection);
    }

    const bool emit_typed_keypath_artifacts = !typed_keypath_artifacts_.empty();
    if (emit_typed_keypath_artifacts) {
      const std::string emitted_section_name =
          Objc3RuntimeMetadataHostSectionForLogicalName(
              kObjc3RuntimeKeypathDescriptorLogicalSection);
      const std::string generic_metadata_replay_key_symbol =
          frontend_metadata_.lowering_generic_metadata_abi_replay_key.empty()
              ? "null"
              : runtime_string_pool_globals_.find(
                        frontend_metadata_.lowering_generic_metadata_abi_replay_key)
                        ->second;
      std::vector<std::string> descriptor_symbols;
      descriptor_symbols.reserve(typed_keypath_artifacts_.size());
      for (const auto &entry : typed_keypath_artifacts_) {
        const TypedKeyPathArtifact &artifact = entry.second;
        const auto root_it = runtime_string_pool_globals_.find(artifact.root_name);
        const auto component_it =
            runtime_string_pool_globals_.find(artifact.component_path);
        const auto profile_it = runtime_string_pool_globals_.find(artifact.profile);
        if (root_it == runtime_string_pool_globals_.end() ||
            component_it == runtime_string_pool_globals_.end() ||
            profile_it == runtime_string_pool_globals_.end()) {
          if (!unsupported_fail_closed_path_triggered_) {
            unsupported_fail_closed_path_triggered_ = true;
            unsupported_fail_closed_path_reason_ =
                "typed key-path artifact string-pool registration failed";
          }
          return;
        }
        descriptor_symbols.push_back(artifact.descriptor_symbol);
        out << artifact.descriptor_symbol
            << " = private global { i64, ptr, ptr, ptr, ptr, i1 } { i64 "
            << static_cast<unsigned long long>(artifact.ordinal + 1u)
            << ", ptr " << root_it->second << ", ptr " << component_it->second
            << ", ptr " << profile_it->second << ", ptr "
            << generic_metadata_replay_key_symbol << ", i1 "
            << (artifact.root_is_self ? 1 : 0) << " }, section \""
            << emitted_section_name << "\", align 8\n";
      }
      out << "@__objc3_sec_keypath_descriptors = internal global { i64, ["
          << descriptor_symbols.size() << " x ptr] } { i64 "
          << descriptor_symbols.size() << ", [" << descriptor_symbols.size()
          << " x ptr] [";
      for (std::size_t i = 0; i < descriptor_symbols.size(); ++i) {
        if (i != 0) {
          out << ", ";
        }
        out << "ptr " << descriptor_symbols[i];
      }
      out << "] }, section \"" << emitted_section_name << "\", align 8\n";
      emit_retained("@__objc3_sec_keypath_descriptors");
    }

    std::vector<std::string> discovery_root_targets;
    discovery_root_targets.reserve(layout_policy.families.size() + 3);
    discovery_root_targets.push_back(image_info_symbol);
    for (const auto &family : layout_policy.families) {
      discovery_root_targets.push_back("@" + family.aggregate_symbol_name);
    }
    if (emit_selector_string_pools) {
      discovery_root_targets.push_back("@__objc3_sec_selector_pool");
      discovery_root_targets.push_back("@__objc3_sec_string_pool");
    }
    if (emit_typed_keypath_artifacts) {
      discovery_root_targets.push_back("@__objc3_sec_keypath_descriptors");
    }

    const std::string discovery_root_symbol =
        "@" + RuntimeMetadataDiscoveryRootSymbol();
    const std::string linker_anchor_symbol =
        "@" + RuntimeMetadataLinkerAnchorSymbol();
    out << discovery_root_symbol << " = dso_local constant { i64, ["
        << discovery_root_targets.size() << " x ptr] } { i64 "
        << discovery_root_targets.size() << ", ["
        << discovery_root_targets.size() << " x ptr] [";
    for (std::size_t i = 0; i < discovery_root_targets.size(); ++i) {
      if (i != 0) {
        out << ", ";
      }
      out << "ptr " << discovery_root_targets[i];
    }
    out << "] }, section \""
        << Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeLinkerDiscoveryRootLogicalSection)
        << "\", align 8\n";
    out << linker_anchor_symbol << " = dso_local global ptr "
        << discovery_root_symbol << ", section \""
        << Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeLinkerAnchorLogicalSection)
        << "\", align 8\n";
    emit_retained(discovery_root_symbol);
    emit_retained(linker_anchor_symbol);

    if (ShouldEmitRuntimeBootstrapLowering()) {
      const std::string module_name_symbol =
          "@" + RuntimeBootstrapModuleNameGlobalSymbol();
      const std::string translation_unit_identity_symbol =
          "@" + RuntimeBootstrapTranslationUnitIdentityGlobalSymbol();
      const std::string image_descriptor_symbol =
          "@" + RuntimeBootstrapImageDescriptorSymbol();
      const std::string registration_descriptor_name_symbol =
          ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()
              ? "@" + RuntimeBootstrapRegistrationDescriptorNameGlobalSymbol()
              : std::string();
      const std::string image_root_name_symbol =
          ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()
              ? "@" + RuntimeBootstrapImageRootNameGlobalSymbol()
              : std::string();
      const std::string registration_descriptor_symbol =
          ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()
              ? "@" + RuntimeBootstrapRegistrationDescriptorSymbol()
              : std::string();
      const std::string image_root_symbol =
          ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()
              ? "@" + RuntimeBootstrapImageRootSymbol()
              : std::string();
      const std::string registration_table_symbol =
          "@" + RuntimeBootstrapRegistrationTableSymbol();
      const std::string image_local_init_state_symbol =
          "@" + RuntimeBootstrapImageLocalInitStateSymbol();
      const std::string constructor_root_symbol =
          "@" +
          frontend_metadata_.runtime_bootstrap_lowering_constructor_root_symbol;
      const std::string class_section_root_symbol =
          "@__objc3_sec_class_descriptors";
      const std::string protocol_section_root_symbol =
          "@__objc3_sec_protocol_descriptors";
      const std::string category_section_root_symbol =
          "@__objc3_sec_category_descriptors";
      const std::string property_section_root_symbol =
          "@__objc3_sec_property_descriptors";
      const std::string ivar_section_root_symbol =
          "@__objc3_sec_ivar_descriptors";
      const std::string selector_pool_symbol =
          emit_selector_string_pools ? "@__objc3_sec_selector_pool" : "null";
      const std::string string_pool_symbol =
          emit_selector_string_pools ? "@__objc3_sec_string_pool" : "null";
      const std::string keypath_descriptor_root_symbol =
          emit_typed_keypath_artifacts ? "@__objc3_sec_keypath_descriptors"
                                       : "null";
      const std::string module_name =
          program_.module_name.empty() ? "objc3_module" : program_.module_name;
      const std::string &translation_unit_identity_key =
          frontend_metadata_
              .runtime_metadata_archive_static_link_translation_unit_identity_key;
      const std::string &registration_descriptor_identifier =
          frontend_metadata_.runtime_bootstrap_registration_descriptor_identifier;
      const std::string &image_root_identifier =
          frontend_metadata_.runtime_bootstrap_image_root_identifier;
      out << module_name_symbol << " = private unnamed_addr constant ["
          << (module_name.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(module_name) << "\\00\", align 1\n";
      out << translation_unit_identity_symbol
          << " = private unnamed_addr constant ["
          << (translation_unit_identity_key.size() + 1u) << " x i8] c\""
          << EscapeCStringLiteral(translation_unit_identity_key)
          << "\\00\", align 1\n";
      if (ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()) {
        out << registration_descriptor_name_symbol
            << " = private unnamed_addr constant ["
            << (registration_descriptor_identifier.size() + 1u)
            << " x i8] c\""
            << EscapeCStringLiteral(registration_descriptor_identifier)
            << "\\00\", align 1\n";
        out << image_root_name_symbol << " = private unnamed_addr constant ["
            << (image_root_identifier.size() + 1u) << " x i8] c\""
            << EscapeCStringLiteral(image_root_identifier)
            << "\\00\", align 1\n";
      }
      const std::uint64_t registration_order_ordinal =
          frontend_metadata_.runtime_bootstrap_registration_order_ordinal == 0
              ? kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal
              : frontend_metadata_.runtime_bootstrap_registration_order_ordinal;
      out << image_descriptor_symbol << " = internal constant "
          << RuntimeBootstrapImageDescriptorType()
          << " { ptr getelementptr inbounds (["
          << (module_name.size() + 1u) << " x i8], ptr " << module_name_symbol
          << ", i32 0, i32 0), ptr getelementptr inbounds (["
          << (translation_unit_identity_key.size() + 1u)
          << " x i8], ptr " << translation_unit_identity_symbol
          << ", i32 0, i32 0)"
          << ", i64 " << registration_order_ordinal << ", i64 "
          << frontend_metadata_.runtime_metadata_section_publication_class_descriptor_count
          << ", i64 "
          << frontend_metadata_
                 .runtime_metadata_section_publication_protocol_descriptor_count
          << ", i64 "
          << frontend_metadata_
                 .runtime_metadata_section_publication_category_descriptor_count
          << ", i64 "
          << frontend_metadata_
                 .runtime_metadata_section_publication_property_descriptor_count
          << ", i64 "
          << frontend_metadata_
                 .runtime_metadata_section_publication_ivar_descriptor_count
          << " }, align 8\n";
      out << image_local_init_state_symbol
          << " = internal global i8 0, align 1\n";
      out << registration_table_symbol
          << " = internal constant " << RuntimeBootstrapRegistrationTableType()
          << " { i64 "
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_table_abi_version
          << ", i64 "
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_table_pointer_field_count
          << ", ptr " << image_descriptor_symbol << ", ptr "
          << discovery_root_symbol << ", ptr " << linker_anchor_symbol
          << ", ptr " << class_section_root_symbol << ", ptr "
          << protocol_section_root_symbol << ", ptr "
          << category_section_root_symbol << ", ptr "
          << property_section_root_symbol << ", ptr "
          << ivar_section_root_symbol << ", ptr " << selector_pool_symbol
          << ", ptr " << string_pool_symbol << ", ptr "
          << keypath_descriptor_root_symbol << ", ptr "
          << image_local_init_state_symbol << " }, align 8\n";
      if (ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()) {
        // emits first-class image-root/registration-descriptor
        // globals into dedicated sections, keyed by the authoritative source
        // identifiers from the frontend closure rather than sidecar-only JSON.
        out << image_root_symbol << " = internal constant { ptr, ptr, ptr, ptr, ptr }"
            << " { ptr getelementptr inbounds (["
            << (image_root_identifier.size() + 1u) << " x i8], ptr "
            << image_root_name_symbol << ", i32 0, i32 0), ptr getelementptr inbounds (["
            << (module_name.size() + 1u) << " x i8], ptr " << module_name_symbol
            << ", i32 0, i32 0), ptr " << image_descriptor_symbol
            << ", ptr " << registration_table_symbol << ", ptr "
            << discovery_root_symbol << " }, section \""
            << Objc3RuntimeMetadataHostSectionForLogicalName(
                   kObjc3RuntimeBootstrapImageRootLogicalSection)
            << "\", align 8\n";
        out << registration_descriptor_symbol
            << " = internal constant { ptr, ptr, ptr, ptr, ptr, ptr }"
            << " { ptr getelementptr inbounds (["
            << (registration_descriptor_identifier.size() + 1u)
            << " x i8], ptr " << registration_descriptor_name_symbol
            << ", i32 0, i32 0), ptr " << image_root_symbol << ", ptr "
            << image_descriptor_symbol << ", ptr " << registration_table_symbol
            << ", ptr " << linker_anchor_symbol << ", ptr "
            << image_local_init_state_symbol << " }, section \""
            << Objc3RuntimeMetadataHostSectionForLogicalName(
                   kObjc3RuntimeBootstrapRegistrationDescriptorLogicalSection)
            << "\", align 8\n";
      }
      const std::uint32_t global_ctor_priority =
          registration_order_ordinal >
                  static_cast<std::uint64_t>(
                      std::numeric_limits<std::uint32_t>::max())
              ? std::numeric_limits<std::uint32_t>::max()
              : static_cast<std::uint32_t>(registration_order_ordinal);
      out << "@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] "
             "[{ i32, ptr, ptr } { i32 "
          << global_ctor_priority << ", ptr "
          << constructor_root_symbol << ", ptr " << registration_table_symbol
          << " }]\n";
      emit_retained(image_descriptor_symbol);
      emit_retained(registration_table_symbol);
      emit_retained(image_local_init_state_symbol);
      if (ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()) {
        emit_retained(image_root_symbol);
        emit_retained(registration_descriptor_symbol);
      }
    }

    out << "@llvm.used = appending global [" << retained_globals.size()
        << " x ptr] [";
    for (std::size_t i = 0; i < retained_globals.size(); ++i) {
      if (i != 0) {
        out << ", ";
      }
      out << "ptr " << retained_globals[i];
    }
    out << "], section \"llvm.metadata\"\n\n";
  }

  std::string NewTemp(FunctionContext &ctx) const { return "%t" + std::to_string(ctx.temp_counter++); }

  std::string NewLabel(FunctionContext &ctx, const std::string &prefix) const {
    return prefix + std::to_string(ctx.label_counter++);
  }

  void EmitAutoreleasepoolUnwindToDepth(FunctionContext &ctx,
                                        std::size_t target_depth) const {
    while (ctx.autoreleasepool_scope_symbols.size() > target_depth) {
      ctx.code_lines.push_back("  call void @" +
                               std::string(
                                   kObjc3RuntimePopAutoreleasepoolScopeSymbol) +
                               "()");
      ctx.autoreleasepool_scope_symbols.pop_back();
    }
  }

  void PushScope(FunctionContext &ctx) const {
    ctx.scopes.push_back({});
    ctx.pending_defer_scope_blocks.push_back({});
    ctx.pending_block_dispose_scope_depths.push_back(
        ctx.pending_block_dispose_calls.size());
    ctx.pending_ownership_cleanup_scope_depths.push_back(
        ctx.pending_ownership_cleanup_calls.size());
    ctx.arc_cleanup_scope_depths.push_back(ctx.arc_owned_cleanup_ptrs.size());
  }

  void EmitDeferredCleanupBlock(const BlockStmt *block_stmt,
                                FunctionContext &ctx) const {
    // defer/guard lowering anchor: defer bodies now lower into
    // explicit LIFO scope cleanups that execute inside the same lexical cleanup
    // pipeline as existing block-dispose and ARC-owned teardown, rather than
    // running eagerly at the original statement site.
    if (block_stmt == nullptr) {
      return;
    }
    const bool outer_terminated = ctx.terminated;
    ctx.terminated = false;
    const std::size_t autoreleasepool_depth =
        ctx.autoreleasepool_scope_symbols.size();
    if (block_stmt->is_autoreleasepool_scope) {
      ctx.code_lines.push_back(
          "  call void @" +
          std::string(kObjc3RuntimePushAutoreleasepoolScopeSymbol) + "()");
      ctx.autoreleasepool_scope_symbols.push_back(
          block_stmt->autoreleasepool_scope_symbol);
    }
    PushScope(ctx);
    for (const auto &nested_stmt : block_stmt->body) {
      EmitStatement(nested_stmt.get(), ctx);
    }
    const bool block_terminated = ctx.terminated;
    PopScope(ctx, !block_terminated);
    if (!block_terminated) {
      EmitAutoreleasepoolUnwindToDepth(ctx, autoreleasepool_depth);
    }
    ctx.terminated = outer_terminated;
  }

  void EmitDeferredCleanupForScopeBucket(
      const std::vector<const BlockStmt *> &bucket, FunctionContext &ctx) const {
    // Defer cleanup execution pushes/pops lexical scopes while replaying each
    // deferred block. Copy the bucket first so those scope-vector mutations do
    // not invalidate the source bucket reference mid-iteration when one scope
    // owns multiple defer blocks.
    const std::vector<const BlockStmt *> stable_bucket = bucket;
    for (std::size_t index = stable_bucket.size(); index > 0; --index) {
      EmitDeferredCleanupBlock(stable_bucket[index - 1u], ctx);
    }
  }

  void EmitDeferredCleanupTerminalToDepth(FunctionContext &ctx,
                                          std::size_t target_scope_depth) const {
    for (std::size_t index = ctx.pending_defer_scope_blocks.size();
         index > target_scope_depth; --index) {
      EmitDeferredCleanupForScopeBucket(ctx.pending_defer_scope_blocks[index - 1u],
                                        ctx);
    }
  }

  void EmitPendingBlockDisposeUnwindToDepth(FunctionContext &ctx,
                                            std::size_t target_depth) const {
    while (ctx.pending_block_dispose_calls.size() > target_depth) {
      const PendingBlockDisposeCall call = ctx.pending_block_dispose_calls.back();
      ctx.pending_block_dispose_calls.pop_back();
      if (call.helper_symbol.empty() || call.storage_ptr.empty()) {
        continue;
      }
      ctx.code_lines.push_back("  call void @" + call.helper_symbol + "(ptr " +
                               call.storage_ptr + ")");
    }
  }

  void EmitPendingBlockDisposeTerminalCleanupToDepth(
      const FunctionContext &ctx, std::size_t target_depth,
      std::vector<std::string> &out_lines) const {
    for (std::size_t index = ctx.pending_block_dispose_calls.size();
         index > target_depth; --index) {
      const PendingBlockDisposeCall &call =
          ctx.pending_block_dispose_calls[index - 1u];
      if (call.helper_symbol.empty() || call.storage_ptr.empty()) {
        continue;
      }
      out_lines.push_back("  call void @" + call.helper_symbol + "(ptr " +
                          call.storage_ptr + ")");
    }
  }

  void EmitOwnershipCleanupCall(const PendingOwnershipCleanupCall &call,
                            FunctionContext &ctx) const {
    if (!call.active || call.storage_ptr.empty()) {
      return;
    }
    const std::string loaded_value = NewTemp(ctx);
    ctx.code_lines.push_back("  " + loaded_value + " = load i32, ptr " +
                             call.storage_ptr + ", align 4");
    if (!call.resource_close_symbol.empty()) {
      if (call.has_resource_invalid_value) {
        const std::string resource_live = NewTemp(ctx);
        const std::string resource_close_label =
            NewLabel(ctx, "ownership_resource_close_");
        const std::string resource_skip_label =
            NewLabel(ctx, "ownership_resource_skip_");
        ctx.code_lines.push_back("  " + resource_live + " = icmp ne i32 " +
                                 loaded_value + ", " +
                                 std::to_string(call.resource_invalid_value));
        ctx.code_lines.push_back("  br i1 " + resource_live + ", label %" +
                                 resource_close_label + ", label %" +
                                 resource_skip_label);
        ctx.code_lines.push_back(resource_close_label + ":");
        ctx.code_lines.push_back("  call void @" + call.resource_close_symbol +
                                 "(i32 " + loaded_value + ")");
        ctx.code_lines.push_back("  br label %" + resource_skip_label);
        ctx.code_lines.push_back(resource_skip_label + ":");
      } else {
        ctx.code_lines.push_back("  call void @" + call.resource_close_symbol +
                                 "(i32 " + loaded_value + ")");
      }
    }
    if (!call.cleanup_function_symbol.empty()) {
      ctx.code_lines.push_back("  call void @" + call.cleanup_function_symbol +
                               "(i32 " + loaded_value + ")");
    }
  }

  void EmitOwnershipCleanupTerminalCall(const PendingOwnershipCleanupCall &call,
                                    std::vector<std::string> &out_lines,
                                    int &temp_counter) const {
    if (!call.active || call.storage_ptr.empty()) {
      return;
    }
    const std::string loaded_value = "%t" + std::to_string(temp_counter++);
    out_lines.push_back("  " + loaded_value + " = load i32, ptr " +
                        call.storage_ptr + ", align 4");
    if (!call.resource_close_symbol.empty()) {
      if (call.has_resource_invalid_value) {
        const std::string resource_live = "%t" + std::to_string(temp_counter++);
        const std::string resource_close_label =
            "ownership_resource_close_" + std::to_string(temp_counter++);
        const std::string resource_skip_label =
            "ownership_resource_skip_" + std::to_string(temp_counter++);
        out_lines.push_back("  " + resource_live + " = icmp ne i32 " +
                            loaded_value + ", " +
                            std::to_string(call.resource_invalid_value));
        out_lines.push_back("  br i1 " + resource_live + ", label %" +
                            resource_close_label + ", label %" +
                            resource_skip_label);
        out_lines.push_back(resource_close_label + ":");
        out_lines.push_back("  call void @" + call.resource_close_symbol +
                            "(i32 " + loaded_value + ")");
        out_lines.push_back("  br label %" + resource_skip_label);
        out_lines.push_back(resource_skip_label + ":");
      } else {
        out_lines.push_back("  call void @" + call.resource_close_symbol +
                            "(i32 " + loaded_value + ")");
      }
    }
    if (!call.cleanup_function_symbol.empty()) {
      out_lines.push_back("  call void @" + call.cleanup_function_symbol +
                          "(i32 " + loaded_value + ")");
    }
  }

  void EmitOwnershipCleanupUnwindToDepth(FunctionContext &ctx,
                                     std::size_t target_depth) const {
    while (ctx.pending_ownership_cleanup_calls.size() > target_depth) {
      const PendingOwnershipCleanupCall call = ctx.pending_ownership_cleanup_calls.back();
      if (!call.binding_name.empty()) {
        ctx.ownership_cleanup_call_indices.erase(call.binding_name);
      }
      ctx.pending_ownership_cleanup_calls.pop_back();
      EmitOwnershipCleanupCall(call, ctx);
    }
  }

  void EmitOwnershipCleanupTerminalCleanupToDepth(const FunctionContext &ctx,
                                              std::size_t target_depth,
                                              std::vector<std::string> &out_lines,
                                              int &temp_counter) const {
    for (std::size_t index = ctx.pending_ownership_cleanup_calls.size();
         index > target_depth; --index) {
      EmitOwnershipCleanupTerminalCall(ctx.pending_ownership_cleanup_calls[index - 1u],
                                   out_lines, temp_counter);
    }
  }

  void PopScope(FunctionContext &ctx, bool emit_cleanup) const {
    if (ctx.scopes.empty()) {
      return;
    }
    const auto scope_bindings = std::move(ctx.scopes.back());
    ctx.scopes.pop_back();
    const auto deferred_blocks =
        ctx.pending_defer_scope_blocks.empty()
            ? std::vector<const BlockStmt *>{}
            : std::move(ctx.pending_defer_scope_blocks.back());
    if (!ctx.pending_defer_scope_blocks.empty()) {
      ctx.pending_defer_scope_blocks.pop_back();
    }
    const std::size_t target_block_dispose_depth =
        ctx.pending_block_dispose_scope_depths.empty()
            ? 0u
            : ctx.pending_block_dispose_scope_depths.back();
    if (!ctx.pending_block_dispose_scope_depths.empty()) {
      ctx.pending_block_dispose_scope_depths.pop_back();
    }
    const std::size_t target_ownership_cleanup_depth =
        ctx.pending_ownership_cleanup_scope_depths.empty()
            ? 0u
            : ctx.pending_ownership_cleanup_scope_depths.back();
    if (!ctx.pending_ownership_cleanup_scope_depths.empty()) {
      ctx.pending_ownership_cleanup_scope_depths.pop_back();
    }
    const std::size_t target_arc_cleanup_depth =
        ctx.arc_cleanup_scope_depths.empty() ? 0u : ctx.arc_cleanup_scope_depths.back();
    if (!ctx.arc_cleanup_scope_depths.empty()) {
      ctx.arc_cleanup_scope_depths.pop_back();
    }
    if (emit_cleanup) {
      EmitDeferredCleanupForScopeBucket(deferred_blocks, ctx);
      EmitOwnershipCleanupUnwindToDepth(ctx, target_ownership_cleanup_depth);
      EmitPendingBlockDisposeUnwindToDepth(ctx, target_block_dispose_depth);
      EmitArcOwnedCleanupUnwindToDepth(ctx, target_arc_cleanup_depth);
    }
    for (const auto &binding : scope_bindings) {
      ctx.block_bindings.erase(binding.first);
      ctx.ownership_cleanup_call_indices.erase(binding.first);
    }
  }

  std::string LookupVarPtr(const FunctionContext &ctx, const std::string &name) const {
    for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }
    if (globals_.find(name) != globals_.end()) {
      return "@" + name;
    }
    return "";
  }

  int LookupImmediateIdentifierValue(const FunctionContext &ctx,
                                     const std::string &name) const {
    const auto value_it = ctx.immediate_identifiers.find(name);
    if (value_it == ctx.immediate_identifiers.end()) {
      return 0;
    }
    return value_it->second;
  }

  static bool SortedStringListContains(const std::vector<std::string> &entries,
                                       const std::string &needle) {
    return std::binary_search(entries.begin(), entries.end(), needle);
  }

  static bool BlockLiteralUsesPointerCaptureStorage(const Expr &expr) {
    return expr.block_storage_byref_slot_count > 0u ||
           expr.block_runtime_owned_object_capture_count > 0u ||
           expr.block_runtime_weak_object_capture_count > 0u ||
           expr.block_runtime_unowned_object_capture_count > 0u ||
           expr.block_runtime_copy_helper_required ||
           expr.block_runtime_dispose_helper_required;
  }

  static std::string BuildBlockStorageType(const Expr &expr) {
    std::ostringstream out;
    if (!BlockLiteralUsesPointerCaptureStorage(expr)) {
      out << "{ ptr, ["
          << static_cast<unsigned long long>(
                 expr.block_capture_names_lexicographic.size())
          << " x i32] }";
      return out.str();
    }
    out << "{ ptr, ptr, ptr, ["
        << static_cast<unsigned long long>(
               expr.block_capture_names_lexicographic.size())
        << " x ptr] }";
    return out.str();
  }

  static std::string BuildBlockInvokeSymbol(const Expr &expr) {
    return expr.block_invoke_trampoline_symbol.empty()
               ? std::string("objc3_block_invoke_missing_symbol")
               : expr.block_invoke_trampoline_symbol;
  }

  static bool BlockLiteralRequiresEscapingRuntimeHooks(const Expr &expr) {
    return expr.block_escape_shape_symbol == "global-initializer" ||
           expr.block_escape_shape_symbol == "assignment-value" ||
           expr.block_escape_shape_symbol == "return-value" ||
           expr.block_escape_shape_symbol == "call-argument" ||
           expr.block_escape_shape_symbol == "message-argument";
  }

  static bool BlockLiteralSupportsScalarRuntimePromotion(const Expr &expr) {
    return expr.block_source_model_is_normalized &&
           expr.block_runtime_capture_ownership_is_normalized &&
           (!expr.block_storage_requires_byref_cells ||
            !expr.block_storage_byref_layout_symbol.empty()) &&
           ((!expr.block_runtime_copy_helper_required &&
             !expr.block_copy_helper_required) ||
            !expr.block_copy_helper_symbol.empty()) &&
           ((!expr.block_runtime_dispose_helper_required &&
             !expr.block_dispose_helper_required) ||
            !expr.block_dispose_helper_symbol.empty());
  }

  static bool BlockLiteralSupportsEscapingRuntimeHookLowering(const Expr &expr) {
    return expr.block_storage_escape_to_heap &&
           BlockLiteralSupportsScalarRuntimePromotion(expr);
  }

  static bool BlockLiteralRequiresFutureRuntimeLanes(const Expr &expr) {
    return !BlockLiteralSupportsScalarRuntimePromotion(expr);
  }

  static std::string BuildBlockCopyHelperSymbol(const Expr &expr) {
    return expr.block_copy_helper_symbol.empty()
               ? std::string("objc3_block_copy_helper_missing_symbol")
               : expr.block_copy_helper_symbol;
  }

  static std::string BuildBlockDisposeHelperSymbol(const Expr &expr) {
    return expr.block_dispose_helper_symbol.empty()
               ? std::string("objc3_block_dispose_helper_missing_symbol")
               : expr.block_dispose_helper_symbol;
  }

  static std::uint64_t AlignBlockStorageBytes(std::uint64_t value,
                                              std::uint64_t alignment) {
    if (alignment == 0u) {
      return value;
    }
    const std::uint64_t remainder = value % alignment;
    return remainder == 0u ? value : value + (alignment - remainder);
  }

  static std::uint64_t BlockStorageStaticSizeBytes(const Expr &expr) {
    const std::uint64_t capture_count = static_cast<std::uint64_t>(
        expr.block_capture_names_lexicographic.size());
    if (!BlockLiteralUsesPointerCaptureStorage(expr)) {
      return AlignBlockStorageBytes(
          static_cast<std::uint64_t>(sizeof(void *)) +
              capture_count * static_cast<std::uint64_t>(sizeof(std::int32_t)),
          static_cast<std::uint64_t>(alignof(void *)));
    }
    return static_cast<std::uint64_t>(sizeof(void *) * 3u) +
           capture_count * static_cast<std::uint64_t>(sizeof(void *));
  }

  void EmitPendingBlockDisposeHelpers(FunctionContext &ctx) const {
    EmitPendingBlockDisposeUnwindToDepth(ctx, 0u);
  }

  void EmitArcOwnedCleanupUnwindToDepth(FunctionContext &ctx,
                                        std::size_t target_depth) const {
    while (ctx.arc_owned_cleanup_ptrs.size() > target_depth) {
      const std::string ptr = ctx.arc_owned_cleanup_ptrs.back();
      ctx.arc_owned_cleanup_ptrs.pop_back();
      ctx.arc_owned_cleanup_ptr_set.erase(ptr);
      ctx.arc_owned_storage_ptrs.erase(ptr);
      if (ptr.empty()) {
        continue;
      }
      const std::string loaded_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + loaded_value + " = load i32, ptr " +
                               ptr + ", align 4");
      const std::string released_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                               std::string(kObjc3RuntimeReleaseI32Symbol) +
                               "(i32 " + loaded_value + ")");
      (void)released_value;
    }
  }

  void EmitArcOwnedTerminalCleanupToDepth(const FunctionContext &ctx,
                                          std::size_t target_depth,
                                          std::vector<std::string> &out_lines,
                                          int &temp_counter) const {
    for (std::size_t index = ctx.arc_owned_cleanup_ptrs.size();
         index > target_depth; --index) {
      const std::string &ptr = ctx.arc_owned_cleanup_ptrs[index - 1u];
      if (ptr.empty()) {
        continue;
      }
      const std::string loaded_value =
          "%t" + std::to_string(temp_counter++);
      out_lines.push_back("  " + loaded_value + " = load i32, ptr " + ptr +
                          ", align 4");
      const std::string released_value =
          "%t" + std::to_string(temp_counter++);
      out_lines.push_back("  " + released_value + " = call i32 @" +
                          std::string(kObjc3RuntimeReleaseI32Symbol) +
                          "(i32 " + loaded_value + ")");
      (void)released_value;
    }
  }

  void EmitBlockCopyHelper(const Expr &expr) const {
    if (!BlockLiteralUsesPointerCaptureStorage(expr) ||
        !expr.block_runtime_copy_helper_required) {
      return;
    }
    const std::string symbol = BuildBlockCopyHelperSymbol(expr);
    if (symbol.empty() ||
        !emitted_block_copy_helper_symbols_.insert(symbol).second) {
      return;
    }

    const std::string storage_type = BuildBlockStorageType(expr);
    std::ostringstream out;
    out << "define internal void @" << symbol << "(ptr %block) {\n";
    out << "entry:\n";
    int temp_counter = 0;
    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size();
         ++i) {
      const std::string &capture_name =
          expr.block_capture_names_lexicographic[i];
      if (!SortedStringListContains(
              expr.block_runtime_owned_object_capture_names_lexicographic,
              capture_name)) {
        continue;
      }
      const std::string slot_ptr =
          "%block.copy.slot." + std::to_string(temp_counter++);
      const std::string capture_ptr =
          "%block.copy.capture." + std::to_string(temp_counter++);
      const std::string loaded_value =
          "%block.copy.value." + std::to_string(temp_counter++);
      const std::string retained_value =
          "%block.copy.retained." + std::to_string(temp_counter++);
      out << "  " << slot_ptr << " = getelementptr inbounds " << storage_type
          << ", ptr %block, i32 0, i32 3, i32 " << i << "\n";
      out << "  " << capture_ptr << " = load ptr, ptr " << slot_ptr
          << ", align 8\n";
      out << "  " << loaded_value << " = load i32, ptr " << capture_ptr
          << ", align 4\n";
      out << "  " << retained_value << " = call i32 @"
          << kObjc3RuntimeRetainI32Symbol << "(i32 " << loaded_value << ")\n";
      out << "  store i32 " << retained_value << ", ptr " << capture_ptr
          << ", align 4\n";
    }
    out << "  ret void\n";
    out << "}\n";
    block_function_definitions_.push_back(out.str());
  }

  void EmitBlockDisposeHelper(const Expr &expr,
                              const FunctionContext &ctx) const {
    if (!BlockLiteralUsesPointerCaptureStorage(expr) ||
        !expr.block_runtime_dispose_helper_required) {
      return;
    }
    const std::string symbol = BuildBlockDisposeHelperSymbol(expr);
    if (symbol.empty() ||
        !emitted_block_dispose_helper_symbols_.insert(symbol).second) {
      return;
    }

    const std::string storage_type = BuildBlockStorageType(expr);
    std::ostringstream out;
    out << "define internal void @" << symbol << "(ptr %block) {\n";
    out << "entry:\n";
    int temp_counter = 0;
    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size();
         ++i) {
      const std::string &capture_name =
          expr.block_capture_names_lexicographic[i];
      const auto moved_capture_it = std::find_if(
          expr.block_explicit_capture_items_source_order.begin(),
          expr.block_explicit_capture_items_source_order.end(),
          [&capture_name](const Expr::ExplicitBlockCaptureItem &item) {
            return item.name == capture_name && item.mode == "move";
          });
      const bool moved_capture =
          moved_capture_it != expr.block_explicit_capture_items_source_order.end();
      const auto cleanup_it = ctx.ownership_cleanup_call_indices.find(capture_name);
      if (!SortedStringListContains(
              expr.block_runtime_owned_object_capture_names_lexicographic,
              capture_name) &&
          (!moved_capture || cleanup_it == ctx.ownership_cleanup_call_indices.end())) {
        continue;
      }
      const std::string slot_ptr =
          "%block.dispose.slot." + std::to_string(temp_counter++);
      const std::string capture_ptr =
          "%block.dispose.capture." + std::to_string(temp_counter++);
      const std::string loaded_value =
          "%block.dispose.value." + std::to_string(temp_counter++);
      const std::string released_value =
          "%block.dispose.released." + std::to_string(temp_counter++);
      out << "  " << slot_ptr << " = getelementptr inbounds " << storage_type
          << ", ptr %block, i32 0, i32 3, i32 " << i << "\n";
      out << "  " << capture_ptr << " = load ptr, ptr " << slot_ptr
          << ", align 8\n";
      out << "  " << loaded_value << " = load i32, ptr " << capture_ptr
          << ", align 4\n";
      if (SortedStringListContains(
              expr.block_runtime_owned_object_capture_names_lexicographic,
              capture_name)) {
        out << "  " << released_value << " = call i32 @"
            << kObjc3RuntimeReleaseI32Symbol << "(i32 " << loaded_value
            << ")\n";
      }
      if (moved_capture && cleanup_it != ctx.ownership_cleanup_call_indices.end()) {
        const PendingOwnershipCleanupCall &cleanup_call =
            ctx.pending_ownership_cleanup_calls[cleanup_it->second];
        if (!cleanup_call.resource_close_symbol.empty()) {
          if (cleanup_call.has_resource_invalid_value) {
            const std::string resource_live =
                "%block.dispose.resource.live." + std::to_string(temp_counter++);
            const std::string resource_close_label =
                "block.dispose.resource.close." + std::to_string(temp_counter++);
            const std::string resource_skip_label =
                "block.dispose.resource.skip." + std::to_string(temp_counter++);
            out << "  " << resource_live << " = icmp ne i32 " << loaded_value
                << ", " << cleanup_call.resource_invalid_value << "\n";
            out << "  br i1 " << resource_live << ", label %"
                << resource_close_label << ", label %" << resource_skip_label
                << "\n";
            out << resource_close_label << ":\n";
            out << "  call void @" << cleanup_call.resource_close_symbol
                << "(i32 " << loaded_value << ")\n";
            out << "  br label %" << resource_skip_label << "\n";
            out << resource_skip_label << ":\n";
          } else {
            out << "  call void @" << cleanup_call.resource_close_symbol
                << "(i32 " << loaded_value << ")\n";
          }
        }
        if (!cleanup_call.cleanup_function_symbol.empty()) {
          out << "  call void @" << cleanup_call.cleanup_function_symbol
              << "(i32 " << loaded_value << ")\n";
        }
      }
      (void)released_value;
    }
    out << "  ret void\n";
    out << "}\n";
    block_function_definitions_.push_back(out.str());
  }

  std::string EmitPromotedBlockHandle(const Expr &expr,
                                      const std::string &storage_ptr,
                                      FunctionContext &ctx,
                                      bool allow_nonescaping_scalar_promotion =
                                          false) const {
    // escaping-block runtime-hook anchor: readonly-scalar escaping
    // block values now lower through a private runtime promotion hook instead
    // of failing closed at the first escaping expression use.
    const bool supported = allow_nonescaping_scalar_promotion
                               ? BlockLiteralSupportsScalarRuntimePromotion(expr)
                               : BlockLiteralSupportsEscapingRuntimeHookLowering(
                                     expr);
    if (!supported) {
      return EmitUnsupportedI32Value(
          "escaping block value still requires normalized block runtime helper metadata that lands in later runtime work");
    }
    // lowering-implementation anchor: actual promotion of move-based
    // cleanup/resource captures remains fail-closed until runtime ownership
    // transfer exists. Plain stack/local helper lowering is implemented now.
    if (expr.block_explicit_capture_move_count > 0u) {
      return EmitUnsupportedI32Value(
          "escaping move captures for cleanup/resource-backed locals still require later Part 8 runtime ownership transfer support");
    }
    const bool pointer_capture_storage =
        BlockLiteralUsesPointerCaptureStorage(expr);
    const std::string promoted = NewTemp(ctx);
    ctx.code_lines.push_back(
        "  " + promoted + " = call i32 @" +
        std::string(kObjc3RuntimePromoteBlockI32Symbol) + "(ptr " + storage_ptr +
        ", i64 " + std::to_string(BlockStorageStaticSizeBytes(expr)) +
        ", i32 " + std::string(pointer_capture_storage ? "1" : "0") + ")");
    return promoted;
  }

  std::string EmitPromotedBlockHandleLoad(BlockBinding &binding,
                                          FunctionContext &ctx) const {
    if (!binding.promoted_handle_ptr.empty()) {
      const std::string loaded = NewTemp(ctx);
      ctx.code_lines.push_back("  " + loaded + " = load i32, ptr " +
                               binding.promoted_handle_ptr + ", align 4");
      return loaded;
    }
    if (binding.literal == nullptr || binding.storage_ptr.empty()) {
      return EmitUnsupportedI32Value(
          "missing block literal metadata for escaping block-handle lowering");
