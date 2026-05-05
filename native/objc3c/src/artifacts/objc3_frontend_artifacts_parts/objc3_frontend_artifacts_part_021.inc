          const auto append_category_bundle =
              [&](const std::string &record_kind,
                  const std::string &record_owner_identity,
                  std::size_t property_count,
                  std::size_t instance_method_count,
                  std::size_t class_method_count) {
                if (record_owner_identity.empty() ||
                    !category_owner_identities.insert(record_owner_identity)
                         .second) {
                  protocol_category_payload_complete = false;
                  return;
                }
                Objc3IRRuntimeMetadataCategoryBundle bundle;
                bundle.record_kind = record_kind;
                bundle.class_name = category_node.class_name;
                bundle.category_name = category_node.category_name;
                bundle.owner_identity = record_owner_identity;
                bundle.category_owner_identity = category_node.owner_identity;
                bundle.class_owner_identity = category_node.class_owner_identity;
                bundle.adopted_protocol_owner_identities_lexicographic =
                    category_node.adopted_protocol_owner_identities_lexicographic;
                bundle.property_count = property_count;
                bundle.instance_method_count = instance_method_count;
                bundle.class_method_count = class_method_count;
                category_bundles.push_back(std::move(bundle));
              };
          if (category_node.has_interface) {
            append_category_bundle(
                "interface", category_node.interface_owner_identity,
                category_node.interface_property_count,
                category_node.interface_method_count,
                category_node.interface_class_method_count);
          }
          if (protocol_category_payload_complete &&
              category_node.has_implementation) {
            append_category_bundle(
                "implementation", category_node.implementation_owner_identity,
                category_node.implementation_property_count,
                category_node.implementation_method_count,
                category_node.implementation_class_method_count);
          }
          if (!protocol_category_payload_complete) {
            break;
          }
        }
      }

      if (protocol_category_payload_complete) {
        for (const auto &bundle : protocol_bundles) {
          for (const auto &inherited_owner_identity :
               bundle.inherited_protocol_owner_identities_lexicographic) {
            if (protocol_owner_identities.find(inherited_owner_identity) ==
                protocol_owner_identities.end()) {
              protocol_category_payload_complete = false;
              break;
            }
          }
          if (!protocol_category_payload_complete) {
            break;
          }
        }
      }

      protocol_category_payload_complete =
          protocol_category_payload_complete &&
          protocol_bundles.size() ==
              runtime_metadata_section_publication.protocol_descriptor_count &&
          category_bundles.size() ==
              runtime_metadata_section_publication.category_descriptor_count;
      if (protocol_category_payload_complete) {
        ir_frontend_metadata.runtime_metadata_protocol_bundles_lexicographic =
            std::move(protocol_bundles);
        ir_frontend_metadata.runtime_metadata_category_bundles_lexicographic =
            std::move(category_bundles);
      }
      ir_frontend_metadata.runtime_metadata_protocol_category_emission_ready =
          protocol_category_payload_complete;
      ir_frontend_metadata
          .runtime_metadata_protocol_category_emission_fail_closed =
          protocol_category_payload_complete;

      // member-table data emission anchor: the typed lowering
      // handoff now also projects real owner-scoped method tables plus real
      // property/ivar descriptor payload records without reopening the earlier
      // class/protocol/category descriptor bundle shapes from C002/C003.
      bool member_table_payload_complete = protocol_category_payload_complete;
      std::vector<Objc3IRRuntimeMetadataPropertyBundle> property_bundles;
      property_bundles.reserve(source_graph.property_nodes_lexicographic.size());
      std::unordered_set<std::string> property_owner_identities;
      property_owner_identities.reserve(
          source_graph.property_nodes_lexicographic.size());
      for (const auto &property_node : source_graph.property_nodes_lexicographic) {
        if (property_node.owner_kind.empty() || property_node.owner_name.empty() ||
            property_node.owner_identity.empty() ||
            property_node.declaration_owner_identity.empty() ||
            property_node.export_owner_identity.empty() ||
            property_node.property_name.empty() || property_node.type_name.empty() ||
            !property_owner_identities.insert(property_node.owner_identity).second ||
            (property_node.has_getter && property_node.getter_selector.empty()) ||
            (property_node.has_setter && property_node.setter_selector.empty()) ||
            property_node.property_attribute_profile.empty() ||
            property_node.effective_getter_selector.empty() ||
            property_node.accessor_ownership_profile.empty() ||
            (property_node.effective_setter_available &&
             property_node.effective_setter_selector.empty())) {
          member_table_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataPropertyBundle bundle;
        bundle.owner_kind = property_node.owner_kind;
        bundle.owner_name = property_node.owner_name;
        bundle.owner_identity = property_node.owner_identity;
        bundle.declaration_owner_identity =
            property_node.declaration_owner_identity;
        bundle.export_owner_identity = property_node.export_owner_identity;
        bundle.property_name = property_node.property_name;
        bundle.type_name = property_node.type_name;
        bundle.has_getter = property_node.has_getter;
        bundle.getter_selector = property_node.getter_selector;
        bundle.has_setter = property_node.has_setter;
        bundle.setter_selector = property_node.setter_selector;
        bundle.ivar_binding_symbol = property_node.ivar_binding_symbol;
        bundle.executable_synthesized_binding_kind =
            property_node.executable_synthesized_binding_kind;
        bundle.executable_synthesized_binding_symbol =
            property_node.executable_synthesized_binding_symbol;
        bundle.property_attribute_profile =
            property_node.property_attribute_profile;
        bundle.ownership_lifetime_profile =
            property_node.ownership_lifetime_profile;
        bundle.ownership_runtime_hook_profile =
            property_node.ownership_runtime_hook_profile;
        bundle.effective_getter_selector =
            property_node.effective_getter_selector;
        bundle.effective_setter_available =
            property_node.effective_setter_available;
        bundle.effective_setter_selector =
            property_node.effective_setter_selector;
        bundle.accessor_ownership_profile =
            property_node.accessor_ownership_profile;
        bundle.synthesizes_executable_accessors =
            property_node.synthesizes_executable_accessors;
        bundle.executable_ivar_layout_symbol =
            property_node.executable_ivar_layout_symbol;
        bundle.executable_ivar_layout_slot_index =
            property_node.executable_ivar_layout_slot_index;
        bundle.executable_ivar_layout_size_bytes =
            property_node.executable_ivar_layout_size_bytes;
        bundle.executable_ivar_layout_alignment_bytes =
            property_node.executable_ivar_layout_alignment_bytes;
        bundle.executable_ivar_layout_offset_bytes =
            property_node.executable_ivar_layout_offset_bytes;
        bundle.executable_ivar_layout_padding_bytes =
            property_node.executable_ivar_layout_padding_bytes;
        bundle.executable_ivar_layout_inherited_slot_count =
            property_node.executable_ivar_layout_inherited_slot_count;
        bundle.executable_ivar_layout_inherited_size_bytes =
            property_node.executable_ivar_layout_inherited_size_bytes;
        bundle.executable_ivar_layout_owner_size_bytes =
            property_node.executable_ivar_layout_owner_size_bytes;
        bundle.executable_ivar_init_order_index =
            property_node.executable_ivar_init_order_index;
        bundle.executable_ivar_destroy_order_index =
            property_node.executable_ivar_destroy_order_index;
        bundle.executable_ivar_layout_valid =
            property_node.executable_ivar_layout_valid;
        bundle.executable_ivar_layout_replay_key =
            property_node.executable_ivar_layout_replay_key;
        property_bundles.push_back(std::move(bundle));
      }
      std::sort(
          property_bundles.begin(), property_bundles.end(),
          [](const Objc3IRRuntimeMetadataPropertyBundle &lhs,
             const Objc3IRRuntimeMetadataPropertyBundle &rhs) {
            return std::tie(lhs.declaration_owner_identity, lhs.property_name,
                            lhs.owner_identity) <
                   std::tie(rhs.declaration_owner_identity, rhs.property_name,
                            rhs.owner_identity);
          });

      std::vector<Objc3IRRuntimeMetadataIvarBundle> ivar_bundles;
      ivar_bundles.reserve(source_graph.ivar_nodes_lexicographic.size());
      std::unordered_set<std::string> ivar_owner_identities;
      ivar_owner_identities.reserve(source_graph.ivar_nodes_lexicographic.size());
      if (member_table_payload_complete) {
        for (const auto &ivar_node : source_graph.ivar_nodes_lexicographic) {
          if (ivar_node.owner_kind.empty() || ivar_node.owner_name.empty() ||
              ivar_node.owner_identity.empty() ||
              ivar_node.declaration_owner_identity.empty() ||
              ivar_node.export_owner_identity.empty() ||
              ivar_node.property_owner_identity.empty() ||
              ivar_node.property_name.empty() ||
              ivar_node.ivar_binding_symbol.empty() ||
              ivar_node.executable_synthesized_binding_kind.empty() ||
              ivar_node.executable_ivar_layout_symbol.empty() ||
              !ivar_owner_identities.insert(ivar_node.owner_identity).second) {
            member_table_payload_complete = false;
            break;
          }

          Objc3IRRuntimeMetadataIvarBundle bundle;
          bundle.owner_kind = ivar_node.owner_kind;
          bundle.owner_name = ivar_node.owner_name;
          bundle.owner_identity = ivar_node.owner_identity;
          bundle.declaration_owner_identity = ivar_node.declaration_owner_identity;
          bundle.export_owner_identity = ivar_node.export_owner_identity;
          bundle.property_owner_identity = ivar_node.property_owner_identity;
          bundle.property_name = ivar_node.property_name;
          bundle.ivar_binding_symbol = ivar_node.ivar_binding_symbol;
          bundle.executable_synthesized_binding_kind =
              ivar_node.executable_synthesized_binding_kind;
          bundle.executable_synthesized_binding_symbol =
              ivar_node.executable_synthesized_binding_symbol;
          bundle.executable_ivar_layout_symbol =
              ivar_node.executable_ivar_layout_symbol;
          bundle.executable_ivar_layout_slot_index =
              ivar_node.executable_ivar_layout_slot_index;
          bundle.executable_ivar_layout_size_bytes =
              ivar_node.executable_ivar_layout_size_bytes;
          bundle.executable_ivar_layout_alignment_bytes =
              ivar_node.executable_ivar_layout_alignment_bytes;
          bundle.executable_ivar_layout_offset_bytes =
              ivar_node.executable_ivar_layout_offset_bytes;
          bundle.executable_ivar_layout_padding_bytes =
              ivar_node.executable_ivar_layout_padding_bytes;
          bundle.executable_ivar_layout_inherited_slot_count =
              ivar_node.executable_ivar_layout_inherited_slot_count;
          bundle.executable_ivar_layout_inherited_size_bytes =
              ivar_node.executable_ivar_layout_inherited_size_bytes;
          bundle.executable_ivar_layout_owner_size_bytes =
              ivar_node.executable_ivar_layout_owner_size_bytes;
          bundle.executable_ivar_init_order_index =
              ivar_node.executable_ivar_init_order_index;
          bundle.executable_ivar_destroy_order_index =
              ivar_node.executable_ivar_destroy_order_index;
          bundle.executable_ivar_layout_valid =
              ivar_node.executable_ivar_layout_valid;
          bundle.executable_ivar_layout_replay_key =
              ivar_node.executable_ivar_layout_replay_key;
          ivar_bundles.push_back(std::move(bundle));
        }
      }
      std::sort(
          ivar_bundles.begin(), ivar_bundles.end(),
          [](const Objc3IRRuntimeMetadataIvarBundle &lhs,
             const Objc3IRRuntimeMetadataIvarBundle &rhs) {
            return std::tie(lhs.declaration_owner_identity, lhs.property_name,
                            lhs.owner_identity) <
                   std::tie(rhs.declaration_owner_identity, rhs.property_name,
                            rhs.owner_identity);
          });

      std::vector<Objc3IRRuntimeMetadataMethodListBundle> method_list_bundles;
      method_list_bundles.reserve(source_graph.method_nodes_lexicographic.size());
      std::unordered_map<std::string, std::size_t> method_bundle_indexes;
      method_bundle_indexes.reserve(source_graph.method_nodes_lexicographic.size());
      std::unordered_set<std::string> method_owner_identities;
      method_owner_identities.reserve(source_graph.method_nodes_lexicographic.size());
      const auto determine_owner_family_kind = [](const std::string &owner_kind)
          -> std::string {
        if (owner_kind == "protocol") {
          return "protocol";
        }
        if (owner_kind == "class-interface" ||
            owner_kind == "class-implementation") {
          return "class";
        }
        if (owner_kind == "category-interface" ||
            owner_kind == "category-implementation") {
          return "category";
        }
        return {};
      };
      if (member_table_payload_complete) {
        for (const auto &method_node : source_graph.method_nodes_lexicographic) {
          const std::string list_kind =
              method_node.is_class_method ? "class" : "instance";
          const std::string owner_family_kind =
              determine_owner_family_kind(method_node.owner_kind);
          if (method_node.owner_kind.empty() || method_node.owner_name.empty() ||
              owner_family_kind.empty() || method_node.owner_identity.empty() ||
              method_node.declaration_owner_identity.empty() ||
              method_node.export_owner_identity.empty() ||
              method_node.selector.empty() ||
              method_node.return_type_name.empty()) {
            member_table_payload_complete = false;
            break;
          }

          const std::string bundle_key =
              method_node.declaration_owner_identity + "|" + list_kind;
          auto bundle_it = method_bundle_indexes.find(bundle_key);
          if (bundle_it == method_bundle_indexes.end()) {
            Objc3IRRuntimeMetadataMethodListBundle bundle;
            bundle.owner_kind = method_node.owner_kind;
            bundle.owner_name = method_node.owner_name;
            bundle.owner_family_kind = owner_family_kind;
            bundle.declaration_owner_identity =
                method_node.declaration_owner_identity;
            bundle.export_owner_identity = method_node.export_owner_identity;
            bundle.list_kind = list_kind;
            method_list_bundles.push_back(std::move(bundle));
            bundle_it = method_bundle_indexes
                            .emplace(bundle_key, method_list_bundles.size() - 1u)
                            .first;
          }

          auto &bundle = method_list_bundles[bundle_it->second];
          if (bundle.owner_kind != method_node.owner_kind ||
              bundle.owner_name != method_node.owner_name ||
              bundle.owner_family_kind != owner_family_kind ||
              bundle.export_owner_identity != method_node.export_owner_identity ||
              bundle.list_kind != list_kind) {
            member_table_payload_complete = false;
            break;
          }

          Objc3IRRuntimeMetadataMethodEntry entry;
          entry.owner_identity = method_node.owner_identity;
          entry.selector = method_node.selector;
          entry.return_type_name = method_node.return_type_name;
          entry.parameter_count = method_node.parameter_count;
          entry.has_body = method_node.has_body;
          entry.effective_direct_dispatch =
              method_node.effective_direct_dispatch;
          entry.objc_final_declared = method_node.objc_final_declared;
          bundle.entries_lexicographic.push_back(std::move(entry));
          method_owner_identities.insert(method_node.owner_identity);
        }
      }
      const auto build_instance_method_owner_identity =
          [](const std::string &declaration_owner_identity,
             const std::string &selector) {
            return declaration_owner_identity + "::instance_method:" + selector;
          };
      if (member_table_payload_complete) {
        for (const auto &property_bundle : property_bundles) {
          if (!property_bundle.synthesizes_executable_accessors ||
              property_bundle.owner_name.empty() ||
              property_bundle.declaration_owner_identity.empty() ||
              property_bundle.export_owner_identity.empty() ||
              property_bundle.property_name.empty() ||
              property_bundle.type_name.empty() ||
              property_bundle.executable_synthesized_binding_kind.empty() ||
              property_bundle.executable_synthesized_binding_symbol.empty()) {
            continue;
          }

          const std::string owner_family_kind =
              determine_owner_family_kind(property_bundle.owner_kind);
          if (owner_family_kind.empty()) {
            member_table_payload_complete = false;
            break;
          }

          const auto append_synthesized_accessor =
              [&](const std::string &selector,
                  const std::string &return_type_name,
                  std::size_t parameter_count) {
                if (selector.empty() || return_type_name.empty()) {
                  member_table_payload_complete = false;
                  return;
                }
                const std::string method_owner_identity =
                    build_instance_method_owner_identity(
                        property_bundle.declaration_owner_identity, selector);
                if (!method_owner_identities.insert(method_owner_identity).second) {
                  return;
                }

                const std::string bundle_key =
                    property_bundle.declaration_owner_identity + "|instance";
                auto bundle_it = method_bundle_indexes.find(bundle_key);
                if (bundle_it == method_bundle_indexes.end()) {
                  Objc3IRRuntimeMetadataMethodListBundle bundle;
                  bundle.owner_kind = property_bundle.owner_kind;
                  bundle.owner_name = property_bundle.owner_name;
                  bundle.owner_family_kind = owner_family_kind;
                  bundle.declaration_owner_identity =
                      property_bundle.declaration_owner_identity;
                  bundle.export_owner_identity =
                      property_bundle.export_owner_identity;
                  bundle.list_kind = "instance";
                  method_list_bundles.push_back(std::move(bundle));
                  bundle_it = method_bundle_indexes
                                  .emplace(bundle_key,
                                           method_list_bundles.size() - 1u)
                                  .first;
                }

                auto &bundle = method_list_bundles[bundle_it->second];
                if (bundle.owner_kind != property_bundle.owner_kind ||
                    bundle.owner_name != property_bundle.owner_name ||
                    bundle.owner_family_kind != owner_family_kind ||
                    bundle.export_owner_identity !=
                        property_bundle.export_owner_identity ||
                    bundle.list_kind != "instance") {
                  member_table_payload_complete = false;
                  return;
                }

                Objc3IRRuntimeMetadataMethodEntry entry;
                entry.owner_identity = method_owner_identity;
                entry.selector = selector;
                entry.return_type_name = return_type_name;
                entry.parameter_count = parameter_count;
                entry.has_body = true;
                entry.effective_direct_dispatch = false;
                entry.objc_final_declared = false;
                bundle.entries_lexicographic.push_back(std::move(entry));
              };

          append_synthesized_accessor(
              property_bundle.effective_getter_selector, property_bundle.type_name,
              0u);
          if (!member_table_payload_complete) {
            break;
          }
          if (property_bundle.effective_setter_available) {
            append_synthesized_accessor(
                property_bundle.effective_setter_selector, "void", 1u);
            if (!member_table_payload_complete) {
              break;
            }
          }
        }
      }
      if (member_table_payload_complete) {
        for (const auto &derive_bundle :
             ir_frontend_metadata.metaprogramming_derived_method_bundles_lexicographic) {
          if (derive_bundle.implementation_name.empty() ||
              derive_bundle.declaration_owner_identity.empty() ||
              derive_bundle.export_owner_identity.empty() ||
              derive_bundle.selector.empty() || derive_bundle.emitted_symbol.empty()) {
            member_table_payload_complete = false;
            break;
          }
          const std::string method_owner_identity =
              build_instance_method_owner_identity(
                  derive_bundle.declaration_owner_identity,
                  derive_bundle.selector);
          if (!method_owner_identities.insert(method_owner_identity).second) {
            continue;
          }
          const std::string bundle_key =
              derive_bundle.declaration_owner_identity + "|instance";
          auto bundle_it = method_bundle_indexes.find(bundle_key);
          if (bundle_it == method_bundle_indexes.end()) {
            Objc3IRRuntimeMetadataMethodListBundle bundle;
            bundle.owner_kind = "class-implementation";
            bundle.owner_name = derive_bundle.implementation_name;
            bundle.owner_family_kind = "class";
            bundle.declaration_owner_identity =
                derive_bundle.declaration_owner_identity;
            bundle.export_owner_identity = derive_bundle.export_owner_identity;
            bundle.list_kind = "instance";
            method_list_bundles.push_back(std::move(bundle));
            bundle_it = method_bundle_indexes
                            .emplace(bundle_key, method_list_bundles.size() - 1u)
                            .first;
          }

          auto &bundle = method_list_bundles[bundle_it->second];
          if (bundle.owner_kind != "class-implementation" ||
              bundle.owner_name != derive_bundle.implementation_name ||
              bundle.owner_family_kind != "class" ||
              bundle.export_owner_identity != derive_bundle.export_owner_identity ||
              bundle.list_kind != "instance") {
            member_table_payload_complete = false;
            break;
          }

          Objc3IRRuntimeMetadataMethodEntry entry;
          entry.owner_identity = method_owner_identity;
          entry.selector = derive_bundle.selector;
          entry.return_type_name = "i32";
          entry.parameter_count = derive_bundle.parameter_count;
          entry.has_body = true;
          entry.effective_direct_dispatch = false;
          entry.objc_final_declared = false;
          bundle.entries_lexicographic.push_back(std::move(entry));
        }
      }
      std::sort(
          method_list_bundles.begin(), method_list_bundles.end(),
          [](const Objc3IRRuntimeMetadataMethodListBundle &lhs,
             const Objc3IRRuntimeMetadataMethodListBundle &rhs) {
            return std::tie(lhs.owner_family_kind,
                            lhs.declaration_owner_identity, lhs.list_kind) <
                   std::tie(rhs.owner_family_kind,
                            rhs.declaration_owner_identity, rhs.list_kind);
          });
      for (auto &bundle : method_list_bundles) {
        std::sort(bundle.entries_lexicographic.begin(),
                  bundle.entries_lexicographic.end(),
                  [](const Objc3IRRuntimeMetadataMethodEntry &lhs,
                     const Objc3IRRuntimeMetadataMethodEntry &rhs) {
                    return std::tie(lhs.selector, lhs.owner_identity,
                                    lhs.parameter_count, lhs.return_type_name,
                                    lhs.has_body,
                                    lhs.effective_direct_dispatch,
                                    lhs.objc_final_declared) <
                           std::tie(rhs.selector, rhs.owner_identity,
                                    rhs.parameter_count, rhs.return_type_name,
                                    rhs.has_body,
                                    rhs.effective_direct_dispatch,
                                    rhs.objc_final_declared);
                  });
      }

      member_table_payload_complete =
          member_table_payload_complete &&
          property_bundles.size() ==
              runtime_metadata_section_publication.property_descriptor_count &&
          ivar_bundles.size() ==
              runtime_metadata_section_publication.ivar_descriptor_count;
      if (member_table_payload_complete) {
        std::size_t property_attribute_profiles = 0;
        std::size_t accessor_ownership_profiles = 0;
        std::size_t synthesized_binding_entries = 0;
        for (const auto &bundle : property_bundles) {
          if (!bundle.property_attribute_profile.empty()) {
            ++property_attribute_profiles;
          }
          if (!bundle.accessor_ownership_profile.empty()) {
            ++accessor_ownership_profiles;
          }
          if (!bundle.executable_synthesized_binding_kind.empty()) {
            ++synthesized_binding_entries;
          }
        }
        ir_frontend_metadata.runtime_metadata_method_list_bundles_lexicographic =
            std::move(method_list_bundles);
        ir_frontend_metadata.executable_property_attribute_profile_entries =
            property_attribute_profiles;
        ir_frontend_metadata.executable_accessor_ownership_profile_entries =
            accessor_ownership_profiles;
        ir_frontend_metadata.executable_synthesized_binding_entries =
            synthesized_binding_entries;
        ir_frontend_metadata.executable_ivar_layout_entries =
            ivar_bundles.size();
        ir_frontend_metadata.executable_property_ivar_source_model_replay_key =
            "property_attribute_profiles=" +
            std::to_string(property_attribute_profiles) +
            ";accessor_ownership_profiles=" +
            std::to_string(accessor_ownership_profiles) +
            ";synthesized_bindings=" +
            std::to_string(synthesized_binding_entries) +
            ";ivar_layout_entries=" +
            std::to_string(ivar_bundles.size()) +
            ";deterministic=true;lane_contract=objc3c.property.ivar.source.model.v1";
        ir_frontend_metadata.executable_ivar_layout_emission_contract_id =
            kObjc3ExecutableIvarLayoutEmissionContractId;
        ir_frontend_metadata.executable_ivar_layout_descriptor_model =
            kObjc3ExecutableIvarLayoutDescriptorModel;
        ir_frontend_metadata.executable_ivar_offset_global_model =
            kObjc3ExecutableIvarOffsetGlobalModel;
        ir_frontend_metadata.executable_ivar_layout_table_model =
            kObjc3ExecutableIvarLayoutTableModel;
        std::set<std::string> ivar_layout_owner_identities;
        bool ivar_layout_emission_complete = true;
        std::size_t ivar_offset_global_entries = 0;
        for (const auto &bundle : ivar_bundles) {
          if (bundle.declaration_owner_identity.empty() ||
              bundle.executable_ivar_layout_symbol.empty() ||
              bundle.executable_ivar_layout_alignment_bytes == 0u ||
              bundle.executable_ivar_layout_size_bytes == 0u ||
              !bundle.executable_ivar_layout_valid ||
              bundle.executable_ivar_layout_replay_key.empty() ||
              bundle.ivar_binding_symbol.empty()) {
            ivar_layout_emission_complete = false;
            break;
          }
          ivar_layout_owner_identities.insert(bundle.declaration_owner_identity);
          ++ivar_offset_global_entries;
        }
        ir_frontend_metadata.executable_ivar_offset_global_entries =
            ivar_offset_global_entries;
        ir_frontend_metadata.executable_ivar_layout_table_entries =
            ivar_layout_owner_identities.size();
        ir_frontend_metadata.executable_ivar_layout_owner_entries =
            ivar_layout_owner_identities.size();
        ir_frontend_metadata.executable_ivar_layout_emission_ready =
            ivar_layout_emission_complete;
        ir_frontend_metadata.executable_ivar_layout_emission_fail_closed =
            ivar_layout_emission_complete;
        if (ivar_layout_emission_complete) {
          ir_frontend_metadata.executable_ivar_layout_emission_replay_key =
              "offset_globals=" +
              std::to_string(ivar_offset_global_entries) +
              ";layout_tables=" +
              std::to_string(ivar_layout_owner_identities.size()) +
              ";owner_entries=" +
              std::to_string(ivar_layout_owner_identities.size()) +
              ";deterministic=true;lane_contract=objc3c.ivar.layout.emission.v1";
        } else {
          ir_frontend_metadata.executable_ivar_layout_emission_replay_key.clear();
        }
        ir_frontend_metadata.runtime_metadata_property_bundles_lexicographic =
            std::move(property_bundles);
        ir_frontend_metadata.runtime_metadata_ivar_bundles_lexicographic =
            std::move(ivar_bundles);
      }
      ir_frontend_metadata.runtime_metadata_member_table_emission_ready =
          member_table_payload_complete;
      ir_frontend_metadata.runtime_metadata_member_table_emission_fail_closed =
          member_table_payload_complete;
    }
  }
  ir_frontend_metadata.runtime_metadata_object_inspection_contract_id =
      runtime_metadata_object_inspection.contract_id;
  ir_frontend_metadata.runtime_metadata_object_inspection_publication_contract_id =
      runtime_metadata_object_inspection.publication_contract_id;
  ir_frontend_metadata.runtime_metadata_object_inspection_matrix_published =
      runtime_metadata_object_inspection.matrix_published;
  ir_frontend_metadata.runtime_metadata_object_inspection_fail_closed =
      runtime_metadata_object_inspection.fail_closed;
  ir_frontend_metadata.runtime_metadata_object_inspection_uses_llvm_readobj =
      runtime_metadata_object_inspection.uses_llvm_readobj;
  ir_frontend_metadata.runtime_metadata_object_inspection_uses_llvm_objdump =
      runtime_metadata_object_inspection.uses_llvm_objdump;
  ir_frontend_metadata.runtime_metadata_object_inspection_matrix_row_count =
      runtime_metadata_object_inspection.matrix_row_count;
  ir_frontend_metadata.runtime_metadata_object_inspection_fixture_path =
      runtime_metadata_object_inspection.fixture_path;
  ir_frontend_metadata.runtime_metadata_object_inspection_emit_prefix =
      runtime_metadata_object_inspection.emit_prefix;
  ir_frontend_metadata.runtime_metadata_object_inspection_object_relative_path =
      runtime_metadata_object_inspection.object_relative_path;
  ir_frontend_metadata.runtime_metadata_object_inspection_section_inventory_row_key =
      runtime_metadata_object_inspection.section_inventory_row_key;
  ir_frontend_metadata.runtime_metadata_object_inspection_section_inventory_command =
      runtime_metadata_object_inspection.section_inventory_command;
  ir_frontend_metadata.runtime_metadata_object_inspection_symbol_inventory_row_key =
      runtime_metadata_object_inspection.symbol_inventory_row_key;
  ir_frontend_metadata.runtime_metadata_object_inspection_symbol_inventory_command =
      runtime_metadata_object_inspection.symbol_inventory_command;
  ir_frontend_metadata.executable_metadata_debug_projection_contract_id =
      executable_metadata_debug_projection.contract_id;
  ir_frontend_metadata
      .executable_metadata_debug_projection_typed_handoff_contract_id =
      executable_metadata_debug_projection.typed_lowering_handoff_contract_id;
  ir_frontend_metadata
      .executable_metadata_debug_projection_source_graph_contract_id =
      executable_metadata_debug_projection.source_graph_contract_id;
  ir_frontend_metadata.executable_metadata_debug_projection_named_metadata_name =
      executable_metadata_debug_projection.named_metadata_name;
  ir_frontend_metadata
      .executable_metadata_debug_projection_manifest_surface_path =
      executable_metadata_debug_projection.manifest_surface_path;
  ir_frontend_metadata
      .executable_metadata_debug_projection_typed_handoff_surface_path =
      executable_metadata_debug_projection.typed_handoff_surface_path;
  ir_frontend_metadata
      .executable_metadata_debug_projection_source_graph_surface_path =
      executable_metadata_debug_projection.source_graph_surface_path;
  ir_frontend_metadata.executable_metadata_debug_projection_matrix_published =
      executable_metadata_debug_projection.matrix_published;
  ir_frontend_metadata.executable_metadata_debug_projection_fail_closed =
      executable_metadata_debug_projection.fail_closed;
  ir_frontend_metadata
      .executable_metadata_debug_projection_manifest_debug_surface_published =
      executable_metadata_debug_projection.manifest_debug_surface_published;
  ir_frontend_metadata
      .executable_metadata_debug_projection_ir_named_metadata_published =
      executable_metadata_debug_projection.ir_named_metadata_published;
  ir_frontend_metadata
      .executable_metadata_debug_projection_replay_anchor_deterministic =
      executable_metadata_debug_projection.replay_anchor_deterministic;
  ir_frontend_metadata
      .executable_metadata_debug_projection_active_typed_handoff_ready =
      executable_metadata_debug_projection.active_typed_handoff_ready;
  ir_frontend_metadata.executable_metadata_debug_projection_matrix_row_count =
      executable_metadata_debug_projection.matrix_row_count;
  ir_frontend_metadata.executable_metadata_debug_projection_replay_key =
      executable_metadata_debug_projection.replay_key;
  ir_frontend_metadata
      .executable_metadata_debug_projection_active_typed_handoff_replay_key =
      executable_metadata_debug_projection.active_typed_handoff_replay_key;
  ir_frontend_metadata.executable_metadata_debug_projection_row0_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[0]);
  ir_frontend_metadata.executable_metadata_debug_projection_row1_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[1]);
  ir_frontend_metadata.executable_metadata_debug_projection_row2_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[2]);
  ir_frontend_metadata.runtime_support_library_contract_id =
      runtime_support_library.contract_id;
  ir_frontend_metadata.runtime_support_library_metadata_publication_contract_id =
      runtime_support_library.metadata_publication_contract_id;
  ir_frontend_metadata.runtime_support_library_boundary_frozen =
      runtime_support_library.boundary_frozen;
  ir_frontend_metadata.runtime_support_library_fail_closed =
      runtime_support_library.fail_closed;
  ir_frontend_metadata.runtime_support_library_target_name_frozen =
      runtime_support_library.target_name_frozen;
  ir_frontend_metadata.runtime_support_library_exported_entrypoints_frozen =
      runtime_support_library.exported_entrypoints_frozen;
  ir_frontend_metadata.runtime_support_library_ownership_boundaries_frozen =
      runtime_support_library.ownership_boundaries_frozen;
  ir_frontend_metadata.runtime_support_library_build_constraints_frozen =
      runtime_support_library.build_constraints_frozen;
  ir_frontend_metadata
      .runtime_support_library_test_dispatch_surface_remains_non_authoritative =
      runtime_support_library.test_dispatch_surface_remains_non_authoritative;
  ir_frontend_metadata.runtime_support_library_native_library_present =
      runtime_support_library.native_runtime_library_present;
  ir_frontend_metadata.runtime_support_library_driver_link_wiring_pending =
      runtime_support_library.driver_link_wiring_pending;
  ir_frontend_metadata.runtime_support_library_ready_for_skeleton =
      runtime_support_library.ready_for_runtime_library_skeleton;
  ir_frontend_metadata.runtime_support_library_target_name =
      runtime_support_library.cmake_target_name;
  ir_frontend_metadata.runtime_support_library_public_header_path =
      runtime_support_library.public_header_path;
  ir_frontend_metadata.runtime_support_library_source_root =
      runtime_support_library.source_root;
  ir_frontend_metadata.runtime_support_library_library_kind =
      runtime_support_library.library_kind;
  ir_frontend_metadata.runtime_support_library_archive_basename =
      runtime_support_library.archive_basename;
  ir_frontend_metadata.runtime_support_library_register_image_symbol =
      runtime_support_library.register_image_symbol;
  ir_frontend_metadata.runtime_support_library_lookup_selector_symbol =
      runtime_support_library.lookup_selector_symbol;
  ir_frontend_metadata.runtime_support_library_dispatch_i32_symbol =
      runtime_support_library.dispatch_i32_symbol;
  ir_frontend_metadata.runtime_support_library_reset_for_testing_symbol =
      runtime_support_library.reset_for_testing_symbol;
  ir_frontend_metadata.runtime_support_library_driver_link_mode =
      runtime_support_library.driver_link_mode;
  ir_frontend_metadata.runtime_support_library_compiler_ownership_boundary =
      runtime_support_library.compiler_ownership_boundary;
  ir_frontend_metadata.runtime_support_library_runtime_ownership_boundary =
      runtime_support_library.runtime_ownership_boundary;
  ir_frontend_metadata.runtime_support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  ir_frontend_metadata
      .runtime_support_library_core_feature_support_library_contract_id =
      runtime_support_library_core_feature.support_library_contract_id;
  ir_frontend_metadata
      .runtime_support_library_core_feature_metadata_publication_contract_id =
      runtime_support_library_core_feature.metadata_publication_contract_id;
  ir_frontend_metadata.runtime_support_library_core_feature_fail_closed =
      runtime_support_library_core_feature.fail_closed;
  ir_frontend_metadata.runtime_support_library_core_feature_sources_present =
      runtime_support_library_core_feature
          .native_runtime_library_sources_present;
  ir_frontend_metadata.runtime_support_library_core_feature_header_present =
      runtime_support_library_core_feature.native_runtime_library_header_present;
  ir_frontend_metadata
      .runtime_support_library_core_feature_archive_build_enabled =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  ir_frontend_metadata
      .runtime_support_library_core_feature_entrypoints_implemented =
      runtime_support_library_core_feature
          .native_runtime_library_entrypoints_implemented;
  ir_frontend_metadata
      .runtime_support_library_core_feature_selector_lookup_stateful =
      runtime_support_library_core_feature.selector_lookup_stateful;
  ir_frontend_metadata
      .runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper =
      runtime_support_library_core_feature
          .deterministic_dispatch_formula_matches_runtime_test_helper;
  ir_frontend_metadata
      .runtime_support_library_core_feature_reset_for_testing_supported =
      runtime_support_library_core_feature.reset_for_testing_supported;
  ir_frontend_metadata
      .runtime_support_library_core_feature_test_dispatch_surface_remains_non_authoritative =
      runtime_support_library_core_feature
          .test_dispatch_surface_remains_non_authoritative;
  ir_frontend_metadata
      .runtime_support_library_core_feature_driver_link_wiring_pending =
      runtime_support_library_core_feature.driver_link_wiring_pending;
  ir_frontend_metadata
      .runtime_support_library_core_feature_ready_for_driver_link_wiring =
      runtime_support_library_core_feature.ready_for_driver_link_wiring;
  ir_frontend_metadata.runtime_support_library_core_feature_target_name =
      runtime_support_library_core_feature.cmake_target_name;
  ir_frontend_metadata.runtime_support_library_core_feature_public_header_path =
      runtime_support_library_core_feature.public_header_path;
  ir_frontend_metadata.runtime_support_library_core_feature_source_root =
      runtime_support_library_core_feature.source_root;
  ir_frontend_metadata
      .runtime_support_library_core_feature_implementation_source_path =
      runtime_support_library_core_feature.implementation_source_path;
  ir_frontend_metadata.runtime_support_library_core_feature_library_kind =
      runtime_support_library_core_feature.library_kind;
  ir_frontend_metadata.runtime_support_library_core_feature_archive_basename =
      runtime_support_library_core_feature.archive_basename;
  ir_frontend_metadata.runtime_support_library_core_feature_archive_relative_path =
      runtime_support_library_core_feature.archive_relative_path;
  ir_frontend_metadata.runtime_support_library_core_feature_probe_source_path =
      runtime_support_library_core_feature.probe_source_path;
  ir_frontend_metadata.runtime_support_library_core_feature_register_image_symbol =
      runtime_support_library_core_feature.register_image_symbol;
  ir_frontend_metadata
      .runtime_support_library_core_feature_lookup_selector_symbol =
      runtime_support_library_core_feature.lookup_selector_symbol;
  ir_frontend_metadata.runtime_support_library_core_feature_dispatch_i32_symbol =
      runtime_support_library_core_feature.dispatch_i32_symbol;
  ir_frontend_metadata
      .runtime_support_library_core_feature_reset_for_testing_symbol =
      runtime_support_library_core_feature.reset_for_testing_symbol;
  ir_frontend_metadata.runtime_support_library_core_feature_driver_link_mode =
      runtime_support_library_core_feature.driver_link_mode;
  ir_frontend_metadata.runtime_support_library_link_wiring_contract_id =
      runtime_support_library_link_wiring.contract_id;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_core_feature_contract_id =
      runtime_support_library_link_wiring
          .support_library_core_feature_contract_id;
  ir_frontend_metadata.runtime_support_library_link_wiring_fail_closed =
      runtime_support_library_link_wiring.fail_closed;
  ir_frontend_metadata.runtime_support_library_link_wiring_archive_available =
      runtime_support_library_link_wiring.runtime_library_archive_available;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_compatibility_dispatch_alias_exported =
      runtime_support_library_link_wiring
          .compatibility_dispatch_alias_exported;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_driver_emits_runtime_link_contract =
      runtime_support_library_link_wiring.driver_emits_runtime_link_contract;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library =
      runtime_support_library_link_wiring
          .execution_smoke_consumes_runtime_library;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_test_dispatch_surface_remains_non_authoritative =
      runtime_support_library_link_wiring
          .test_dispatch_surface_remains_non_authoritative;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_ready_for_runtime_library_consumption =
      runtime_support_library_link_wiring
          .ready_for_runtime_library_consumption;
  ir_frontend_metadata.runtime_support_library_link_wiring_archive_relative_path =
      runtime_support_library_link_wiring.archive_relative_path;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_compatibility_dispatch_symbol =
      runtime_support_library_link_wiring.compatibility_dispatch_symbol;
  ir_frontend_metadata.runtime_support_library_link_wiring_runtime_dispatch_symbol =
      runtime_support_library_link_wiring.runtime_dispatch_symbol;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_execution_smoke_script_path =
      runtime_support_library_link_wiring.execution_smoke_script_path;
  ir_frontend_metadata.runtime_support_library_link_wiring_driver_link_mode =
      runtime_support_library_link_wiring.driver_link_mode;
  ir_frontend_metadata.deterministic_id_class_sel_object_pointer_typecheck_handoff =
      id_class_sel_object_pointer_typecheck_contract.deterministic;
  ir_frontend_metadata.deterministic_dispatch_surface_classification_handoff =
      dispatch_surface_classification_contract.deterministic;
  ir_frontend_metadata.deterministic_message_send_selector_lowering_handoff =
      message_send_selector_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_dispatch_abi_marshalling_handoff =
      dispatch_abi_marshalling_contract.deterministic;
  ir_frontend_metadata.deterministic_nil_receiver_semantics_foldability_handoff =
      nil_receiver_semantics_foldability_contract.deterministic;
  ir_frontend_metadata.deterministic_super_dispatch_method_family_handoff =
      super_dispatch_method_family_contract.deterministic;
  ir_frontend_metadata.deterministic_runtime_link_host_link_handoff =
      runtime_link_host_link_contract.deterministic;
  ir_frontend_metadata.deterministic_ownership_qualifier_lowering_handoff =
      ownership_qualifier_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_retain_release_operation_lowering_handoff =
      retain_release_operation_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_autoreleasepool_scope_lowering_handoff =
      autoreleasepool_scope_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_object_pointer_nullability_generics_handoff =
      object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_handoff =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff;
  ir_frontend_metadata.deterministic_scope_resolution_handoff =
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_scope_resolution_handoff_key =
      symbol_graph_scope_resolution_summary.deterministic_handoff_key;
  ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_ready =
      ownership_aware_lowering_behavior_scaffold.expansion_ready;
  ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_key =
      ownership_aware_lowering_behavior_scaffold.expansion_key;
  ir_frontend_metadata.ownership_aware_lowering_performance_quality_guardrails_ready =
      ownership_aware_lowering_behavior_scaffold.performance_quality_guardrails_ready;
  ir_frontend_metadata.ownership_aware_lowering_performance_quality_guardrails_key =
      ownership_aware_lowering_behavior_scaffold.performance_quality_guardrails_key;
  ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_ready =
      ownership_aware_lowering_behavior_scaffold.cross_lane_integration_ready;
  ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_key =
      ownership_aware_lowering_behavior_scaffold.cross_lane_integration_key;
  ir_frontend_metadata.lowering_pass_graph_core_feature_ready =
      pipeline_result.ir_emission_completeness_scaffold.core_feature_ready;
  ir_frontend_metadata.lowering_pass_graph_core_feature_key =
      pipeline_result.ir_emission_completeness_scaffold.core_feature_key;
  ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_ready =
      pipeline_result.ir_emission_completeness_scaffold.expansion_ready;
  ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_key =
      pipeline_result.ir_emission_completeness_scaffold.expansion_key;
  ir_frontend_metadata.lowering_pass_graph_edge_case_compatibility_ready =
      pipeline_result.ir_emission_completeness_scaffold
          .edge_case_compatibility_ready;
  ir_frontend_metadata.lowering_pass_graph_edge_case_compatibility_key =
      pipeline_result.ir_emission_completeness_scaffold
          .edge_case_compatibility_key;
  ir_frontend_metadata.lowering_pass_graph_edge_case_robustness_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_ready;
  ir_frontend_metadata.lowering_pass_graph_edge_case_robustness_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_key;
  ir_frontend_metadata.lowering_pass_graph_diagnostics_hardening_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_ready;
  ir_frontend_metadata.lowering_pass_graph_diagnostics_hardening_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_key;
  ir_frontend_metadata.lowering_pass_graph_recovery_determinism_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_ready;
  ir_frontend_metadata.lowering_pass_graph_recovery_determinism_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_key;
  ir_frontend_metadata.lowering_pass_graph_conformance_matrix_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_matrix_ready;
  ir_frontend_metadata.lowering_pass_graph_conformance_matrix_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_matrix_key;
  ir_frontend_metadata.lowering_pass_graph_conformance_corpus_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_corpus_ready;
  ir_frontend_metadata.lowering_pass_graph_conformance_corpus_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_corpus_key;
  ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_ready;
  ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_key;
  ir_frontend_metadata.ir_emission_completeness_modular_split_ready =
      pipeline_result.ir_emission_completeness_scaffold.modular_split_ready;
  ir_frontend_metadata.ir_emission_completeness_modular_split_key =
      pipeline_result.ir_emission_completeness_scaffold.scaffold_key;
  ir_frontend_metadata.ir_emission_core_feature_impl_ready =
      ir_emission_core_feature_impl_surface.core_feature_impl_ready;
  ir_frontend_metadata.ir_emission_core_feature_impl_key =
      ir_emission_core_feature_impl_surface.core_feature_key;
  ir_frontend_metadata.ir_emission_core_feature_expansion_ready =
      ir_emission_core_feature_impl_surface.core_feature_expansion_ready;
  ir_frontend_metadata.ir_emission_core_feature_expansion_key =
      ir_emission_core_feature_impl_surface.expansion_key;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_edge_case_compatibility_ready;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_key =
      ir_emission_core_feature_impl_surface.edge_case_compatibility_key;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_ready =
      ir_emission_core_feature_impl_surface.core_feature_edge_case_robustness_ready;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_key =
      ir_emission_core_feature_impl_surface.edge_case_robustness_key;
  ir_frontend_metadata.ir_emission_core_feature_diagnostics_hardening_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_diagnostics_hardening_ready;
  ir_frontend_metadata.ir_emission_core_feature_diagnostics_hardening_key =
      ir_emission_core_feature_impl_surface.diagnostics_hardening_key;
  ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_recovery_determinism_ready;
  ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_key =
      ir_emission_core_feature_impl_surface.recovery_determinism_key;
  ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_ready =
      ir_emission_core_feature_impl_surface.core_feature_conformance_matrix_ready;
  ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_key =
      ir_emission_core_feature_impl_surface.conformance_matrix_key;
  ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_ready =
      ir_emission_core_feature_impl_surface.core_feature_conformance_corpus_ready;
  ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_key =
      ir_emission_core_feature_impl_surface.conformance_corpus_key;
  ir_frontend_metadata.ir_emission_core_feature_performance_quality_guardrails_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_performance_quality_guardrails_ready;
  ir_frontend_metadata.ir_emission_core_feature_performance_quality_guardrails_key =
      ir_emission_core_feature_impl_surface.performance_quality_guardrails_key;
  ir_frontend_metadata.ir_emission_core_feature_cross_lane_integration_sync_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_cross_lane_integration_sync_ready;
  ir_frontend_metadata.ir_emission_core_feature_cross_lane_integration_sync_key =
      ir_emission_core_feature_impl_surface.cross_lane_integration_sync_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_core_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_core_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_core_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_core_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_edge_compatibility_shard1_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_advanced_edge_compatibility_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_edge_compatibility_shard1_key =
      ir_emission_core_feature_impl_surface
          .advanced_edge_compatibility_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_diagnostics_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_diagnostics_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_diagnostics_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_diagnostics_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_conformance_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_conformance_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_conformance_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_conformance_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_integration_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_integration_shard1_key;
  std::string ir_error;
  // Historical extraction contract marker:
  // EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)
  // if (!EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {
  if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return bundle;
  }
  bundle.ir_text =
      std::string("; runtime_dispatch_lowering_abi_boundary = ") +
      Objc3RuntimeDispatchLoweringAbiBoundarySummary(
          runtime_dispatch_lowering_abi_contract) +
      "\n" + bundle.ir_text;

  if (IsSuspiciousNativeIRTruthGap(bundle.ir_text, program,
                                   message_send_selector_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, "O3L330",
                 "LLVM IR emission failed: emitted native IR is suspiciously trivial for runtime-bearing executable surface")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return bundle;
  }

  return bundle;
}
