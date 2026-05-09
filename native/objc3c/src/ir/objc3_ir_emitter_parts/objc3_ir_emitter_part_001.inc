#include "ir/objc3_ir_emitter.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <iomanip>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emission_helpers.h"
#include "parse/objc3_parse_support.h"
#include "support/objc3_identifier_safe_suffix.h"

bool ResolveGlobalInitializerValues(const std::vector<GlobalDecl> &globals, std::vector<int> &values);

class Objc3IREmitter {
 public:
  enum class SyntheticMethodKind {
    None,
    PropertyGetter,
    PropertySetter,
    MetaprogrammingDerivedEquality,
    MetaprogrammingDerivedHash,
    MetaprogrammingDerivedDebugDescription,
  };

  struct MethodDefinition {
    std::string symbol;
    std::string implementation_name;
    std::string method_owner_identity;
    std::string superclass_name;
    const Objc3MethodDecl *method = nullptr;
    SyntheticMethodKind synthetic_method_kind = SyntheticMethodKind::None;
    ValueType synthesized_value_type = ValueType::Unknown;
    std::size_t synthesized_parameter_count = 0;
    std::string synthesized_ownership_lifetime_profile;
    std::string synthesized_ownership_runtime_hook_profile;
    std::string synthesized_accessor_ownership_profile;
  };

  struct MetaprogrammingGlobalArtifact {
    std::string symbol;
    std::string payload;
  };

  static bool IsImplementationOwnedPropertyBundle(
      const Objc3IRRuntimeMetadataPropertyBundle &bundle) {
    return bundle.synthesizes_executable_accessors;
  }

  Objc3IREmitter(const Objc3Program &program,
                 const Objc3LoweringContract &lowering_contract,
                 const Objc3IRFrontendMetadata &frontend_metadata)
      : program_(program), frontend_metadata_(frontend_metadata) {
    if (!TryBuildObjc3LoweringIRBoundary(lowering_contract, lowering_ir_boundary_, boundary_error_)) {
      return;
    }
    vector_signature_function_count_ = CountVectorSignatureFunctions(program_);
    for (const auto &global : program_.globals) {
      globals_.insert(global.name);
    }
    for (const auto &fn : program_.functions) {
      function_arity_[fn.name] = fn.params.size();
      if (fn.is_pure) {
        declared_pure_functions_.insert(fn.name);
      }
      if (!fn.is_prototype && defined_functions_.insert(fn.name).second) {
        function_definitions_.push_back(&fn);
      }
    }
    std::unordered_map<std::string, std::string> implementation_superclass_names;
    std::unordered_map<std::string, bool> interface_direct_members_by_name;
    for (const auto &interface_decl : program_.interfaces) {
      if (interface_decl.has_category) {
        continue;
      }
      implementation_superclass_names.emplace(interface_decl.name,
                                              interface_decl.super_name);
      interface_direct_members_by_name.emplace(
          interface_decl.name, interface_decl.objc_direct_members_declared);
    }
    std::unordered_map<std::string, std::size_t> method_symbol_counts;
    std::unordered_set<std::string> method_owner_identities;
    for (const auto &implementation : program_.implementations) {
      const auto direct_members_it =
          interface_direct_members_by_name.find(implementation.name);
      const bool direct_members_declared =
          !implementation.has_category &&
          direct_members_it != interface_direct_members_by_name.end() &&
          direct_members_it->second;
      for (const auto &method : implementation.methods) {
        if (!method.has_body) {
          continue;
        }
        const std::string base_symbol =
            BuildImplementationMethodFunctionSymbol(implementation.name, method.selector, method.is_class_method);
        const std::size_t ordinal = method_symbol_counts[base_symbol]++;
        const std::string symbol =
            ordinal == 0 ? base_symbol : base_symbol + "_" + std::to_string(ordinal + 1u);
        const auto super_it =
            implementation_superclass_names.find(implementation.name);
        const std::string superclass_name =
            super_it == implementation_superclass_names.end() ? std::string()
                                                              : super_it->second;
        method_definitions_.push_back(
            MethodDefinition{symbol,
                             implementation.name,
                             method.scope_path_symbol,
                             superclass_name,
                             &method,
                             SyntheticMethodKind::None,
                             ValueType::Unknown,
                             0u,
                             std::string{},
                             std::string{},
                             std::string{}});
        const bool effective_direct_dispatch =
            !implementation.has_category &&
            (method.objc_direct_declared ||
             (direct_members_declared && !method.objc_dynamic_declared));
        if (effective_direct_dispatch) {
          direct_dispatch_symbols_by_key_.emplace(
              BuildDirectDispatchMethodKey(implementation.name, method.selector,
                                           method.is_class_method),
              "@" + symbol);
        }
        if (!method.scope_path_symbol.empty()) {
          method_owner_identities.insert(method.scope_path_symbol);
        }
      }
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_property_bundles_lexicographic) {
      if (!IsImplementationOwnedPropertyBundle(bundle) ||
          bundle.declaration_owner_identity.empty() || bundle.owner_name.empty() ||
          bundle.type_name.empty() || bundle.effective_getter_selector.empty() ||
          bundle.executable_synthesized_binding_kind.empty() ||
          bundle.executable_synthesized_binding_symbol.empty()) {
        continue;
      }
      const ValueType property_type = RuntimeMetadataValueType(bundle.type_name);
      if (property_type == ValueType::Unknown || property_type == ValueType::Void) {
        boundary_error_ =
            "unsupported synthesized property accessor type '" +
            bundle.type_name + "' for owner '" + bundle.declaration_owner_identity +
            "'";
        return;
      }
      const auto append_synthesized_method =
          [&](const std::string &selector, SyntheticMethodKind kind) {
            if (selector.empty()) {
              boundary_error_ =
                  "missing synthesized property accessor selector for owner '" +
                  bundle.declaration_owner_identity + "'";
              return;
            }
            const std::string method_owner_identity =
                BuildSynthesizedInstanceMethodOwnerIdentity(
                    bundle.declaration_owner_identity, selector);
            if (!method_owner_identities.insert(method_owner_identity).second) {
              return;
            }
            const std::string base_symbol =
                BuildImplementationMethodFunctionSymbol(bundle.owner_name,
                                                        selector, false);
            const std::size_t ordinal = method_symbol_counts[base_symbol]++;
            const std::string symbol =
                ordinal == 0
                    ? base_symbol
                    : base_symbol + "_" + std::to_string(ordinal + 1u);
            method_definitions_.push_back(MethodDefinition{
                symbol,
                bundle.owner_name,
                method_owner_identity,
                std::string{},
                nullptr,
                kind,
                property_type,
                kind == SyntheticMethodKind::PropertySetter ? 1u : 0u,
                bundle.ownership_lifetime_profile,
                bundle.ownership_runtime_hook_profile,
                bundle.accessor_ownership_profile,
            });
            const auto direct_members_it =
                interface_direct_members_by_name.find(bundle.owner_name);
            const bool effective_direct_dispatch =
                bundle.owner_kind == "class-implementation" &&
                direct_members_it != interface_direct_members_by_name.end() &&
                direct_members_it->second;
            if (effective_direct_dispatch) {
              direct_dispatch_symbols_by_key_.emplace(
                  BuildDirectDispatchMethodKey(bundle.owner_name, selector,
                                               false),
                  "@" + symbol);
            }
            ++synthesized_property_accessor_count_;
          };
      append_synthesized_method(bundle.effective_getter_selector,
                                SyntheticMethodKind::PropertyGetter);
      if (!boundary_error_.empty()) {
        return;
      }
      if (bundle.effective_setter_available) {
        append_synthesized_method(bundle.effective_setter_selector,
                                  SyntheticMethodKind::PropertySetter);
        if (!boundary_error_.empty()) {
          return;
        }
      }
    }
    for (const auto &bundle :
         frontend_metadata_.metaprogramming_derived_method_bundles_lexicographic) {
      if (bundle.implementation_name.empty() ||
          bundle.declaration_owner_identity.empty() || bundle.selector.empty() ||
          bundle.emitted_symbol.empty()) {
        boundary_error_ =
            "incomplete Part 10 derived method lowering bundle";
        return;
      }
      const std::string method_owner_identity =
          BuildSynthesizedInstanceMethodOwnerIdentity(
              bundle.declaration_owner_identity, bundle.selector);
      if (!method_owner_identities.insert(method_owner_identity).second) {
        continue;
      }
      SyntheticMethodKind synthetic_kind = SyntheticMethodKind::None;
      if (bundle.derive_name == "Equality") {
        synthetic_kind = SyntheticMethodKind::MetaprogrammingDerivedEquality;
      } else if (bundle.derive_name == "Hash") {
        synthetic_kind = SyntheticMethodKind::MetaprogrammingDerivedHash;
      } else if (bundle.derive_name == "DebugDescription") {
        synthetic_kind = SyntheticMethodKind::MetaprogrammingDerivedDebugDescription;
      } else {
        boundary_error_ = "unsupported Part 10 derive kind '" +
                          bundle.derive_name + "'";
        return;
      }
      method_definitions_.push_back(MethodDefinition{
          bundle.emitted_symbol,
          bundle.implementation_name,
          method_owner_identity,
          std::string{},
          nullptr,
          synthetic_kind,
          ValueType::I32,
          bundle.parameter_count,
          std::string{},
          std::string{},
          std::string{},
      });
      ++metaprogramming_derived_method_count_;
      metaprogramming_global_artifacts_.push_back(
          MetaprogrammingGlobalArtifact{BuildSynthesizedPropertyStorageSymbol(
                                   bundle.emitted_symbol + "_selector"),
                               "derive=" + bundle.derive_name + ";owner=" +
                                   bundle.implementation_name + ";selector=" +
                                   bundle.selector + ";symbol=" +
                                   bundle.emitted_symbol});
    }
    for (const auto &bundle :
         frontend_metadata_.metaprogramming_macro_artifact_bundles_lexicographic) {
      if (bundle.emitted_symbol.empty()) {
        boundary_error_ = "incomplete Part 10 macro artifact lowering bundle";
        return;
      }
      metaprogramming_global_artifacts_.push_back(
          MetaprogrammingGlobalArtifact{bundle.emitted_symbol,
                               "function=" + bundle.function_name +
                                   ";macro=" + bundle.macro_name +
                                   ";package=" + bundle.package_name +
                                   ";provenance=" + bundle.provenance_name +
                                   ";cache_key=" + bundle.cache_key_name +
                                   ";sandbox_policy=" +
                                   bundle.sandbox_policy_name});
    }
    for (const auto &bundle :
         frontend_metadata_.metaprogramming_property_behavior_artifact_bundles_lexicographic) {
      if (bundle.emitted_symbol.empty()) {
        boundary_error_ =
            "incomplete Part 10 property behavior lowering bundle";
        return;
      }
      metaprogramming_global_artifacts_.push_back(
          MetaprogrammingGlobalArtifact{bundle.emitted_symbol,
                               "owner_kind=" + bundle.owner_kind + ";owner=" +
                                   bundle.owner_name + ";property=" +
                                   bundle.property_name + ";behavior=" +
                                   bundle.behavior_name + ";binding=" +
                                   bundle.binding_symbol});
    }
    function_signatures_ = BuildLoweredFunctionSignatures(program_);
    CollectKnownClassReceiverConstants();
    CollectCanonicalPoolLiterals();
    CollectMutableGlobalSymbols();
    CollectFunctionEffects();
  }

  bool Emit(std::string &ir, std::string &error) {
    runtime_dispatch_call_emitted_ = false;
    direct_dispatch_call_sites_emitted_ = 0;
    runtime_dispatch_call_sites_emitted_ = 0;
    selector_pool_gep_sites_emitted_ = 0;
    synthesized_getter_definition_count_ = 0;
    synthesized_setter_definition_count_ = 0;
    current_property_read_helper_call_count_ = 0;
    current_property_write_helper_call_count_ = 0;
    current_property_exchange_helper_call_count_ = 0;
    weak_current_property_load_helper_call_count_ = 0;
    weak_current_property_store_helper_call_count_ = 0;
    retain_helper_call_count_ = 0;
    release_helper_call_count_ = 0;
    autorelease_helper_call_count_ = 0;
    runtime_dispatch_symbols_used_.clear();
    unsupported_fail_closed_path_triggered_ = false;
    unsupported_fail_closed_path_reason_.clear();
    block_function_definitions_.clear();
    emitted_block_invoke_symbols_.clear();
    emitted_block_copy_helper_symbols_.clear();
    emitted_block_dispose_helper_symbols_.clear();

    if (!boundary_error_.empty()) {
      error = boundary_error_;
      return false;
    }
    if (!ValidateMessageSendArityContract(error)) {
      return false;
    }

    std::ostringstream body;

    std::vector<int> resolved_global_values;
    if (!ResolveGlobalInitializerValues(program_.globals, resolved_global_values) ||
        resolved_global_values.size() != program_.globals.size()) {
      error = "global initializer failed const evaluation";
      return false;
    }
    global_const_values_.clear();
    global_nil_proven_symbols_.clear();
    for (std::size_t i = 0; i < program_.globals.size(); ++i) {
      if (mutable_global_symbols_.find(program_.globals[i].name) == mutable_global_symbols_.end()) {
        global_const_values_[program_.globals[i].name] = resolved_global_values[i];
      }
      body << "@" << program_.globals[i].name << " = global i32 " << resolved_global_values[i] << ", align 4\n";
    }
    for (const auto &global : program_.globals) {
      if (mutable_global_symbols_.find(global.name) != mutable_global_symbols_.end()) {
        continue;
      }
      if (IsCompileTimeGlobalNilExpr(global.value.get())) {
        global_nil_proven_symbols_.insert(global.name);
      }
    }
    if (!program_.globals.empty()) {
      body << "\n";
    }

    for (const auto &artifact : metaprogramming_global_artifacts_) {
      std::string payload = artifact.payload;
      payload.push_back('\0');
      body << "@" << artifact.symbol << " = private constant ["
           << payload.size() << " x i8] c\""
           << EscapeCStringLiteral(payload) << "\", align 1\n";
    }
    if (!metaprogramming_global_artifacts_.empty()) {
      body << "\n";
    }

    EmitRuntimeMetadataSectionScaffold(body);

    EmitPrototypeDeclarations(body);

    EmitRuntimeBootstrapLoweringFunctions(body);

    for (const FunctionDecl *fn : function_definitions_) {
      EmitFunction(*fn, body);
      body << "\n";
    }
    for (const MethodDefinition &method_def : method_definitions_) {
      EmitMethod(method_def, body);
      body << "\n";
    }
    for (const std::string &definition : block_function_definitions_) {
      body << definition << "\n";
    }

    EmitEntryPoint(body);
    EmitRuntimeDispatchDeclarations(body);

    if (unsupported_fail_closed_path_triggered_) {
      error = "lowering encountered unsupported fail-closed path: " + unsupported_fail_closed_path_reason_;
      return false;
    }

    std::ostringstream out;
    out << "; objc3c native frontend IR\n";
    out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";
    out << "; runtime_dispatch_decl = " << Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_) << "\n";
    out << "; simd_vector_lowering = " << Objc3SimdVectorTypeLoweringReplayKey() << "\n";
    if (!frontend_metadata_.lowering_property_synthesis_ivar_binding_replay_key.empty()) {
      // property-ivar executable-source-closure anchor:
      // preserve the source freeze as a lowering-only replay marker.
      out << "; property_synthesis_ivar_binding_lowering = "
          << frontend_metadata_.lowering_property_synthesis_ivar_binding_replay_key << "\n";
      out << "; dispatch_and_synthesized_accessor_lowering_surface = "
          << "contract_id="
          << kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId
          << ";runtime_dispatch_symbol="
          << frontend_metadata_.runtime_link_host_link_runtime_dispatch_symbol
          << ";runtime_dispatch_arg_slots="
          << frontend_metadata_.runtime_link_host_link_runtime_dispatch_arg_slots
          << ";runtime_dispatch_symbol_matches_lowering="
          << ((frontend_metadata_.runtime_link_host_link_runtime_dispatch_symbol ==
                       lowering_ir_boundary_.runtime_dispatch_symbol &&
               frontend_metadata_.runtime_link_host_link_runtime_dispatch_symbol ==
                       frontend_metadata_
                           .runtime_support_library_link_wiring_runtime_dispatch_symbol)
                  ? "true"
                  : "false")
          << ";live_runtime_dispatch_sites="
          << (frontend_metadata_.dispatch_surface_classification_instance_sites +
              frontend_metadata_.dispatch_surface_classification_class_sites +
              frontend_metadata_.dispatch_surface_classification_super_sites +
              frontend_metadata_.dispatch_surface_classification_dynamic_sites)
          << ";direct_dispatch_sites="
          << frontend_metadata_.dispatch_surface_classification_direct_sites
          << ";property_synthesis_sites="
          << frontend_metadata_.lowering_property_synthesis_sites
          << ";property_synthesis_default_ivar_bindings="
          << frontend_metadata_.lowering_property_synthesis_default_ivar_bindings
          << ";property_synthesis_explicit_ivar_bindings="
          << frontend_metadata_.lowering_property_synthesis_explicit_ivar_bindings
          << ";interface_owned_property_synthesis_sites="
          << frontend_metadata_.lowering_interface_owned_property_synthesis_sites
          << ";implementation_property_redeclaration_sites="
          << frontend_metadata_
                 .lowering_implementation_property_redeclaration_sites
          << ";ivar_binding_resolved="
          << frontend_metadata_.lowering_property_synthesis_ivar_binding_resolved
          << ";property_descriptor_count="
          << frontend_metadata_.runtime_metadata_section_publication_property_descriptor_count
          << ";ivar_descriptor_count="
          << frontend_metadata_.runtime_metadata_section_publication_ivar_descriptor_count
          << ";member_table_emission_ready="
          << (frontend_metadata_.runtime_metadata_member_table_emission_ready ? "true"
                                                                              : "false")
          << ";deterministic_handoff="
          << (frontend_metadata_.lowering_property_synthesis_deterministic_handoff ? "true"
                                                                                   : "false")
          << "\n";
    }
    if (!frontend_metadata_.executable_property_ivar_source_model_replay_key.empty()) {
      // property-ivar source-model completion anchor:
      // publish the completed property attribute/accessor ownership/layout
      // handoff without reopening the legacy descriptor shapes from the earlier runtime step.
      // property-ivar executable semantics anchor:
      // runtime-meaningful synthesis/accessor/layout semantics now flow
      // through emitted getter/setter bodies plus realized runtime slot
      // allocation instead of remaining metadata-only.
      // property-synthesis implementation anchor:
      // sema now resolves authoritative default ivar bindings from
      // interface-declared class properties even when the implementation does
      // not redeclare those properties.
      // accessor legality expansion anchor:
      // IR only consumes sema-approved effective accessor selectors and
      // ownership/atomicity profiles after duplicate selector and unsupported
      // storage interaction diagnostics have already fail-closed source.
      out << "; property_ivar_source_model_completion = "
          << frontend_metadata_.executable_property_ivar_source_model_replay_key
          << "\n";
      // accessor/layout lowering freeze anchor:
      // emitted IR now carries the current property descriptor, ivar layout,
      // and synthesized binding handoff together with real synthesized
      // accessor bodies on the runtime current-property helper path.
      out << "; executable_property_accessor_layout_lowering = "
          << Objc3ExecutablePropertyAccessorLayoutLoweringSummary()
          << ";property_metadata_entries="
          << frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                 .size()
          << ";ivar_metadata_entries="
          << frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                 .size()
          << ";property_attribute_profiles="
          << frontend_metadata_.executable_property_attribute_profile_entries
          << ";accessor_ownership_profiles="
          << frontend_metadata_.executable_accessor_ownership_profile_entries
          << ";synthesized_binding_entries="
          << frontend_metadata_.executable_synthesized_binding_entries
          << ";ivar_layout_entries="
          << frontend_metadata_.executable_ivar_layout_entries << "\n";
      std::size_t ownership_lifetime_profile_entries = 0u;
      std::size_t ownership_runtime_hook_profile_entries = 0u;
      std::size_t runtime_backed_ownership_surface_entries = 0u;
      for (const auto &bundle :
           frontend_metadata_.runtime_metadata_property_bundles_lexicographic) {
        const bool carries_runtime_backed_ownership_surface =
            !bundle.property_attribute_profile.empty() ||
            !bundle.ownership_lifetime_profile.empty() ||
            !bundle.ownership_runtime_hook_profile.empty() ||
            !bundle.accessor_ownership_profile.empty();
        if (carries_runtime_backed_ownership_surface) {
          ++runtime_backed_ownership_surface_entries;
        }
        if (!bundle.ownership_lifetime_profile.empty()) {
          ++ownership_lifetime_profile_entries;
        }
        if (!bundle.ownership_runtime_hook_profile.empty()) {
          ++ownership_runtime_hook_profile_entries;
        }
      }
      // runtime-backed object ownership attribute surface anchor:
      // ownership-bearing property/member profiles now survive the IR/object
      // lowering boundary in emitted descriptor payloads instead of being
      // observable only through manifests and replay summaries.
      out << "; runtime_backed_object_ownership_attribute_surface = "
          << Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary()
          << ";property_descriptor_entries="
          << frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                 .size()
          << ";runtime_backed_surface_entries="
          << runtime_backed_ownership_surface_entries
          << ";property_attribute_profiles="
          << frontend_metadata_.executable_property_attribute_profile_entries
          << ";ownership_lifetime_profiles="
          << ownership_lifetime_profile_entries
          << ";ownership_runtime_hook_profiles="
          << ownership_runtime_hook_profile_entries
          << ";accessor_ownership_profiles="
          << frontend_metadata_.executable_accessor_ownership_profile_entries
          << "\n";
      if (frontend_metadata_.executable_ivar_layout_emission_ready) {
        // ivar offset/layout emission anchor: lane-C materializes
        // retained offset globals and per-owner layout tables inside the ivar
        // descriptor section, and the live runtime consumes those records for
        // per-instance slot allocation.
        out << "; executable_ivar_layout_emission = "
            << Objc3ExecutableIvarLayoutEmissionSummary()
            << ";offset_global_entries="
            << frontend_metadata_.executable_ivar_offset_global_entries
            << ";layout_table_entries="
            << frontend_metadata_.executable_ivar_layout_table_entries
            << ";layout_owner_entries="
            << frontend_metadata_.executable_ivar_layout_owner_entries << "\n";
      }
      if (synthesized_property_accessor_count_ > 0u) {
        // synthesized accessor/property lowering anchor: lane-C now
        // materializes missing implementation-owned property accessors as real
        // method bodies on the live runtime current-property helper path and
        // republishes the widened property-descriptor surface in emitted IR.
        out << "; executable_synthesized_accessor_property_lowering = "
            << Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary()
            << ";synthesized_accessor_entries="
            << synthesized_property_accessor_count_ << "\n";
        // runtime property/layout consumption freeze anchor: the live
        // runtime consumes emitted accessor implementation pointers and
        // property/layout attachment identities, hands alloc/new through the
        // realized class graph, and executes accessors against per-instance
        // slot storage rather than synthetic lowering-era globals.
        out << "; runtime_property_layout_consumption = "
            << Objc3RuntimePropertyLayoutConsumptionSummary()
            << ";property_descriptor_entries="
            << frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                   .size()
            << ";ivar_layout_owner_entries="
            << frontend_metadata_.executable_ivar_layout_owner_entries
            << ";synthesized_accessor_entries="
            << synthesized_property_accessor_count_ << "\n";
        // instance-allocation-layout-runtime anchor: lane-D now
        // upgrades the emitted property/layout runtime surface from the D001
        // freeze into true per-instance allocation backed by emitted ivar
        // offsets and realized class layout tables.
        out << "; runtime_instance_allocation_layout_support = "
            << Objc3RuntimeInstanceAllocationLayoutSupportSummary()
            << ";property_descriptor_entries="
            << frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                   .size()
            << ";ivar_layout_owner_entries="
            << frontend_metadata_.executable_ivar_layout_owner_entries
            << ";synthesized_accessor_entries="
            << synthesized_property_accessor_count_ << "\n";
        std::size_t writable_property_entries = 0u;
        for (const auto &bundle :
             frontend_metadata_.runtime_metadata_property_bundles_lexicographic) {
          if (IsImplementationOwnedPropertyBundle(bundle) &&
              !bundle.executable_synthesized_binding_symbol.empty()) {
            writable_property_entries += bundle.effective_setter_available ? 1u : 0u;
          }
        }
        // property-metadata-reflection anchor: lane-D now exposes a
        // private reflective helper surface over the realized property graph so
        // runtime probes can query property/accessor/layout facts directly.
        // property-ivar-execution gate anchor: lane-E freezes the
        // A002/B003/C003/D003 property proof chain over this same emitted
        // surface before runnable sample expansion is allowed.
        // runnable property-ivar execution-matrix anchor: lane-E's
        // live property probe links against this emitted surface rather than
        // inventing a parallel runtime path.
        out << "; runtime_property_metadata_reflection = "
            << Objc3RuntimePropertyMetadataReflectionSummary()
            << ";reflectable_property_entries="
            << frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                   .size()
            << ";writable_property_entries=" << writable_property_entries
            << ";synthesized_accessor_entries="
            << synthesized_property_accessor_count_ << "\n";
      }
    }
    if (!frontend_metadata_.runtime_metadata_source_ownership_contract_id.empty()) {
      out << "; runtime_metadata_source_ownership = "
          << frontend_metadata_.runtime_metadata_source_ownership_contract_id << "\n";
    }
    if (!frontend_metadata_.runtime_export_legality_contract_id.empty()) {
      out << "; runtime_export_legality = "
          << frontend_metadata_.runtime_export_legality_contract_id << "\n";
    }
    if (!frontend_metadata_.runtime_export_enforcement_contract_id.empty()) {
      out << "; runtime_export_enforcement = "
          << frontend_metadata_.runtime_export_enforcement_contract_id << "\n";
    }
    if (!frontend_metadata_.runtime_metadata_section_abi_contract_id.empty()) {
      out << "; runtime_metadata_section_abi = "
          << frontend_metadata_.runtime_metadata_section_abi_contract_id
          << "\n";
    }
    if (!frontend_metadata_.runtime_metadata_section_publication_contract_id.empty()) {
      out << "; runtime_metadata_section_publication = "
          << frontend_metadata_.runtime_metadata_section_publication_contract_id
          << "\n";
    }
    if (!frontend_metadata_.runtime_metadata_object_inspection_contract_id.empty()) {
      out << "; runtime_metadata_object_inspection = "
          << frontend_metadata_.runtime_metadata_object_inspection_contract_id
          << "\n";
    }
    out << "; runtime_selector_lookup_tables = contract="
        << kObjc3RuntimeSelectorLookupTablesContractId
        << ", interning_model="
        << kObjc3RuntimeSelectorLookupTablesInterningModel
        << ", merge_model="
        << kObjc3RuntimeSelectorLookupTablesMergeModel
        << ", dynamic_miss_model="
        << kObjc3RuntimeSelectorLookupTablesDynamicMissModel << "\n";
    out << "; runtime_method_cache_slow_path_lookup = contract="
        << kObjc3RuntimeMethodCacheSlowPathContractId
        << ", receiver_normalization_model="
        << kObjc3RuntimeMethodCacheSlowPathReceiverNormalizationModel
        << ", resolution_model="
        << kObjc3RuntimeMethodCacheSlowPathResolutionModel
        << ", cache_model="
        << kObjc3RuntimeMethodCacheSlowPathCacheModel
        << ", strict_error_model="
        << kObjc3RuntimeMethodCacheSlowPathStrictErrorModel << "\n";
    out << "; runtime_protocol_category_method_resolution = contract="
        << kObjc3RuntimeProtocolCategoryMethodResolutionContractId
        << ", category_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionCategoryModel
        << ", protocol_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionProtocolModel
        << ", strict_error_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionStrictErrorModel << "\n";
    if (!frontend_metadata_
             .executable_metadata_debug_projection_contract_id.empty()) {
      out << "; executable_metadata_debug_projection = "
          << frontend_metadata_.executable_metadata_debug_projection_contract_id
          << "\n";
    }
    if (!frontend_metadata_.runtime_support_library_contract_id.empty()) {
      out << "; runtime_support_library = "
          << frontend_metadata_.runtime_support_library_contract_id << "\n";
    }
    if (!frontend_metadata_.runtime_support_library_core_feature_contract_id
             .empty()) {
      out << "; runtime_support_library_core_feature = "
          << frontend_metadata_.runtime_support_library_core_feature_contract_id
          << "\n";
    }
    if (!frontend_metadata_.runtime_support_library_link_wiring_contract_id
             .empty()) {
      out << "; runtime_support_library_link_wiring = "
          << frontend_metadata_.runtime_support_library_link_wiring_contract_id
          << "\n";
    }
    if (!frontend_metadata_.lowering_id_class_sel_object_pointer_typecheck_replay_key.empty()) {
      out << "; id_class_sel_object_pointer_typecheck_lowering = "
          << frontend_metadata_.lowering_id_class_sel_object_pointer_typecheck_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_message_send_selector_lowering_replay_key.empty()) {
      out << "; message_send_selector_lowering = "
          << frontend_metadata_.lowering_message_send_selector_lowering_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_dispatch_abi_marshalling_replay_key.empty()) {
      out << "; dispatch_abi_marshalling_lowering = "
          << frontend_metadata_.lowering_dispatch_abi_marshalling_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_nil_receiver_semantics_foldability_replay_key.empty()) {
      out << "; nil_receiver_semantics_foldability_lowering = "
          << frontend_metadata_.lowering_nil_receiver_semantics_foldability_replay_key << "\n";
    }
    out << "; type_system_optional_keypath_lowering = "
        << Objc3TypeSystemOptionalKeypathLoweringSummary() << "\n";
    // control-flow safety lowering freeze anchor: the frontend now
    // publishes one lowering-owned packet for admitted Part 5 control-flow
    // sites while native IR lowering still fails closed on guard/match/defer
    // execution until later lowering/runtime work materializes it.
    out << "; control_flow_control_flow_safety_lowering = "
        << Objc3ControlFlowControlFlowSafetyLoweringSummary() << "\n";
    out << "; type_system_optional_keypath_runtime_helper_contract = "
        << Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary() << "\n";
    if (!frontend_metadata_.lowering_super_dispatch_method_family_replay_key.empty()) {
      out << "; super_dispatch_method_family_lowering = "
          << frontend_metadata_.lowering_super_dispatch_method_family_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_runtime_link_host_link_replay_key.empty()) {
      out << "; runtime_link_host_link_lowering = "
          << frontend_metadata_.lowering_runtime_link_host_link_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_ownership_qualifier_replay_key.empty()) {
      out << "; ownership_qualifier_lowering = "
          << frontend_metadata_.lowering_ownership_qualifier_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_retain_release_operation_replay_key.empty()) {
      out << "; retain_release_operation_lowering = "
          << frontend_metadata_.lowering_retain_release_operation_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_autoreleasepool_scope_replay_key.empty()) {
      out << "; autoreleasepool_scope_lowering = "
          << frontend_metadata_.lowering_autoreleasepool_scope_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_weak_unowned_semantics_replay_key.empty()) {
      out << "; weak_unowned_semantics_lowering = "
          << frontend_metadata_.lowering_weak_unowned_semantics_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_arc_diagnostics_fixit_replay_key.empty()) {
      out << "; arc_diagnostics_fixit_lowering = "
          << frontend_metadata_.lowering_arc_diagnostics_fixit_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_block_literal_capture_replay_key.empty()) {
      out << "; block_literal_capture_lowering = "
          << frontend_metadata_.lowering_block_literal_capture_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_block_abi_invoke_trampoline_replay_key.empty()) {
      out << "; block_abi_invoke_trampoline_lowering = "
          << frontend_metadata_.lowering_block_abi_invoke_trampoline_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_block_storage_escape_replay_key.empty()) {
      out << "; block_storage_escape_lowering = "
          << frontend_metadata_.lowering_block_storage_escape_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_block_copy_dispose_replay_key.empty()) {
      out << "; block_copy_dispose_lowering = "
          << frontend_metadata_.lowering_block_copy_dispose_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_block_determinism_perf_baseline_replay_key.empty()) {
      out << "; block_determinism_perf_baseline_lowering = "
          << frontend_metadata_.lowering_block_determinism_perf_baseline_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_lightweight_generic_constraint_replay_key.empty()) {
      out << "; lightweight_generic_constraint_lowering = "
          << frontend_metadata_.lowering_lightweight_generic_constraint_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_nullability_flow_warning_precision_replay_key.empty()) {
      out << "; nullability_flow_warning_precision_lowering = "
          << frontend_metadata_.lowering_nullability_flow_warning_precision_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_protocol_qualified_object_type_replay_key.empty()) {
      out << "; protocol_qualified_object_type_lowering = "
          << frontend_metadata_.lowering_protocol_qualified_object_type_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_variance_bridge_cast_replay_key.empty()) {
      out << "; variance_bridge_cast_lowering = "
          << frontend_metadata_.lowering_variance_bridge_cast_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_generic_metadata_abi_replay_key.empty()) {
      out << "; generic_metadata_abi_lowering = "
          << frontend_metadata_.lowering_generic_metadata_abi_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_module_import_graph_replay_key.empty()) {
      // runtime-aware import/module surface anchor: the frontend
      // still preserves only the local translation-unit module-import graph
      // profile in emitted IR.
      // runtime-aware import/module surface anchor: the
      // frontend now emits a canonical runtime-import surface artifact for
      // later cross-translation-unit consumers.
      // cross-module semantic preservation anchor: imported runtime metadata semantics are not lowered into IR; the lane-B surface freezes
      // semantic preservation requirements without landing imported metadata
      // semantic equivalence yet.
      // imported metadata semantic rules anchor: imported runtime
      // surface artifacts may now be consumed and validated before IR
      // emission, but the imported runtime metadata payloads themselves still
      // are not lowered into IR in this lane.
      // serialized metadata import/lowering anchor: imported
      // runtime surface artifacts now freeze the semantic handoff boundary,
      // but serialized imported metadata payloads still are not rehydrated,
      // reused incrementally, or lowered into IR in this lane.
      // serialized metadata artifact reuse anchor: emitted
      // runtime-import-surface artifacts may now carry a transitive serialized
      // runtime-metadata payload for downstream frontend reuse, but imported
      // payloads still are not lowered directly into IR in this lane.
      // cross-module build/runtime orchestration anchor:
      // cross-module link-plan packaging and aggregated runtime-registration
      // orchestration remain outside the IR emitter; lane D only freezes the
      // boundary between emitted import-surface reuse payloads and the local
      // runtime registration manifest here.
      // cross-module runtime packaging anchor: lane D now consumes
      // those emitted object-local artifacts to publish an ordered cross-module
      // link plan and runtime-registration proof, but the IR emitter remains
      // object-local and does not directly orchestrate multi-image packaging.
      // cross-module object-model gate anchor: the truthful gate
      // still lives in the emitted evidence chain and not in any new
      // cross-module IR emitter surface.
      // runnable import/module execution-matrix anchor: the IR
      // emitter still stays object-local while lane-E proves the integrated
      // multi-image path above it.
      // Imported runtime-owned declarations and foreign metadata references
      // therefore
      // remain fail-closed in IR until the later lowering/runtime milestones.
      out << "; module_import_graph_lowering = "
          << frontend_metadata_.lowering_module_import_graph_replay_key << "\n";
    }
    if (!frontend_metadata_
             .lowering_namespace_collision_shadowing_replay_key.empty()) {
      out << "; namespace_collision_shadowing_lowering = "
          << frontend_metadata_
                 .lowering_namespace_collision_shadowing_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .lowering_public_private_api_partition_replay_key.empty()) {
      out << "; public_private_api_partition_lowering = "
          << frontend_metadata_
                 .lowering_public_private_api_partition_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .lowering_incremental_module_cache_invalidation_replay_key
             .empty()) {
      out << "; incremental_module_cache_invalidation_lowering = "
          << frontend_metadata_
                 .lowering_incremental_module_cache_invalidation_replay_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_cross_module_conformance_replay_key
             .empty()) {
      out << "; cross_module_conformance_lowering = "
          << frontend_metadata_.lowering_cross_module_conformance_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .lowering_error_handling_throws_abi_propagation_replay_key.empty()) {
      out << "; error_handling_throws_abi_propagation_lowering = "
          << frontend_metadata_
                 .lowering_error_handling_throws_abi_propagation_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .lowering_error_handling_result_and_bridging_artifact_replay_key.empty()) {
      out << "; error_handling_result_and_bridging_artifact_replay = "
          << frontend_metadata_
                 .lowering_error_handling_result_and_bridging_artifact_replay_key
          << "\n";
    }
    // error-runtime/bridge-helper anchor: publish the private
    // runtime helper ABI boundary consumed by the current runnable Part 6
    // lowering so the emitted IR truthfully advertises helper-backed storage,
    // bridge normalization, and catch dispatch instead of raw local-slot
    // traffic.
    out << "; error_handling_error_runtime_bridge_helper = "
        << Objc3ErrorHandlingErrorRuntimeBridgeHelperSummary() << "\n";
    // live catch/bridge/runtime integration anchor: publish the
    // executable Part 6 runtime-support boundary so linked object probes can
    // prove the private helper ABI is live rather than a contract-only marker.
    out << "; error_handling_live_error_runtime_integration = "
        << Objc3ErrorHandlingLiveErrorRuntimeIntegrationSummary() << "\n";
    // continuation/runtime-helper anchor: publish the first private
    // Part 7 helper ABI boundary for logical continuation allocation,
    // scheduler handoff, and resume traffic. The current async lowering slice
    // remains direct-call only, so this line freezes the helper contract
    // without claiming live suspension or executor scheduling yet.
    out << "; concurrency_continuation_runtime_helper = "
        << Objc3ConcurrencyContinuationRuntimeHelperSummary() << "\n";
    // live continuation/runtime integration anchor: publish the
    // runnable Part 7 helper-execution boundary so supported async fixtures can
    // prove the direct-call await path now executes through the helper cluster.
    out << "; concurrency_live_continuation_runtime_integration = "
        << Objc3ConcurrencyLiveContinuationRuntimeIntegrationSummary() << "\n";
    // ABI/artifact completion anchor: publish the dedicated Part 7
    // task-group/runtime ABI packet so later runtime freeze work consumes a
    // stable helper list and scheduler-visible proof surface.
    out << "; concurrency_task_runtime_abi_completion = contract="
        << kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId
        << ";surface=" << kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath
        << ";helper_count=8;task_group_helper_count=4;runtime_snapshot="
        << "objc3_runtime_copy_task_runtime_state_for_testing"
        << ";follow_on_surface=objc3c.concurrency.taskruntime.helpersurface.v1\n";
    // scheduler/executor runtime freeze anchor: publish the private
    // task-runtime helper ABI and snapshot boundary as the truthful runtime
    // contract above the C003 ABI packet without claiming broader scheduler
    // execution semantics yet.
    out << "; concurrency_scheduler_executor_runtime_contract = "
        << Objc3ConcurrencySchedulerExecutorRuntimeSummary() << "\n";
    // live task runtime anchor: publish the runnable helper-backed
    // execution boundary so supported task-runtime probes can prove that the
    // existing private runtime cluster is live, while broader metadata-export
    // gating remains intentionally deferred.
    out << "; concurrency_live_task_runtime_integration = "
        << Objc3ConcurrencyLiveTaskRuntimeIntegrationSummary() << "\n";
    // hardening anchor: publish the reset/autorelease/cancellation
    // stability packet above the live task runtime boundary so issue-local
    // probes can prove deterministic replay across scope and reset edges.
    out << "; concurrency_task_runtime_hardening = "
        << Objc3ConcurrencyTaskRuntimeHardeningSummary() << "\n";
    if (!frontend_metadata_.lowering_throws_propagation_replay_key.empty()) {
      out << "; throws_propagation_lowering = "
          << frontend_metadata_.lowering_throws_propagation_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_ns_error_bridging_replay_key.empty()) {
      out << "; ns_error_bridging_lowering = "
          << frontend_metadata_.lowering_ns_error_bridging_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_unwind_cleanup_replay_key.empty()) {
      out << "; unwind_cleanup_lowering = "
          << frontend_metadata_.lowering_unwind_cleanup_replay_key << "\n";
    }
    if (!frontend_metadata_
             .lowering_error_diagnostics_recovery_replay_key.empty()) {
      out << "; error_diagnostics_recovery_lowering = "
          << frontend_metadata_
                 .lowering_error_diagnostics_recovery_replay_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_async_continuation_replay_key.empty()) {
      out << "; async_continuation_lowering = "
          << frontend_metadata_.lowering_async_continuation_replay_key << "\n";
    }
    if (!frontend_metadata_
             .lowering_await_lowering_suspension_state_replay_key.empty()) {
      out << "; await_lowering_suspension_state_lowering = "
          << frontend_metadata_
                 .lowering_await_lowering_suspension_state_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .lowering_actor_isolation_sendability_replay_key.empty()) {
      out << "; actor_isolation_sendability_lowering = "
          << frontend_metadata_
                 .lowering_actor_isolation_sendability_replay_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_actor_lowering_metadata_replay_key
             .empty()) {
      out << "; actor_lowering_metadata_contract = "
          << frontend_metadata_.lowering_actor_lowering_metadata_replay_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_dispatch_dispatch_control_replay_key.empty()) {
      out << "; dispatch_control_lowering_contract = "
          << frontend_metadata_.lowering_dispatch_dispatch_control_replay_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_interop_interop_replay_key.empty()) {
      out << "; interop_interop_lowering_abi_contract = "
          << frontend_metadata_.lowering_interop_interop_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_interop_foreign_call_lifetime_replay_key
             .empty()) {
      out << "; interop_foreign_call_and_lifetime_lowering = "
          << frontend_metadata_
                 .lowering_interop_foreign_call_lifetime_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .lowering_interop_ffi_metadata_interface_preservation_key.empty()) {
      out << "; interop_ffi_metadata_interface_preservation = "
          << frontend_metadata_
                 .lowering_interop_ffi_metadata_interface_preservation_key
          << "\n";
      out << "; interop_bridge_packaging_toolchain_contract = "
          << Objc3InteropBridgePackagingToolchainSummary() << "\n";
    }
    if (!frontend_metadata_
             .lowering_interop_header_module_bridge_generation_key.empty()) {
      out << "; interop_header_module_and_bridge_generation = "
          << frontend_metadata_
                 .lowering_interop_header_module_bridge_generation_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_metaprogramming_expansion_replay_key.empty()) {
      out << "; metaprogramming_expansion_lowering_contract = "
          << frontend_metadata_.lowering_metaprogramming_expansion_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_metaprogramming_synthesized_emission_replay_key.empty()) {
      out << "; metaprogramming_synthesized_ast_ir_emission = "
          << frontend_metadata_.lowering_metaprogramming_synthesized_emission_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .lowering_metaprogramming_module_interface_replay_preservation_key.empty()) {
      out << "; metaprogramming_module_interface_replay_preservation = "
          << frontend_metadata_
                 .lowering_metaprogramming_module_interface_replay_preservation_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_metaprogramming_synthesized_emission_replay_key.empty() ||
        !frontend_metadata_
             .lowering_metaprogramming_module_interface_replay_preservation_key.empty()) {
      out << "; metaprogramming_expansion_host_runtime_boundary = "
          << Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary() << "\n";
    }
    if (!frontend_metadata_
             .lowering_dispatch_dispatch_metadata_interface_preservation_key.empty()) {
      out << "; dispatch_dispatch_metadata_interface_preservation = "
          << frontend_metadata_
                 .lowering_dispatch_dispatch_metadata_interface_preservation_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_ownership_system_extension_replay_key.empty()) {
      out << "; system_extension_lowering_contract = "
          << frontend_metadata_.lowering_ownership_system_extension_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .ownership_borrowed_retainable_abi_completion_replay_key.empty()) {
      // ABI/artifact completion anchor: publish the dedicated Part 8
      // borrowed/retainable ABI packet above the frozen lowering contract so
      // lane-D runtime work consumes stable borrowed-return and family-call
      // inventories instead of rediscovering them from raw IR.
      out << "; ownership_borrowed_retainable_abi_completion = contract="
          << kObjc3OwnershipBorrowedRetainableAbiCompletionContractId
          << ";surface="
          << kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath
          << ";replay_key="
          << frontend_metadata_.ownership_borrowed_retainable_abi_completion_replay_key
          << ";returns_borrowed_attribute_sites="
          << frontend_metadata_
                 .ownership_borrowed_retainable_returns_borrowed_attribute_sites
          << ";family_retain_sites="
          << frontend_metadata_.ownership_borrowed_retainable_family_retain_sites
          << ";family_release_sites="
          << frontend_metadata_.ownership_borrowed_retainable_family_release_sites
          << ";family_autorelease_sites="
          << frontend_metadata_
                 .ownership_borrowed_retainable_family_autorelease_sites
          << ";compatibility_returns_retained_sites="
          << frontend_metadata_
                 .ownership_borrowed_retainable_compatibility_returns_retained_sites
          << ";compatibility_returns_not_retained_sites="
          << frontend_metadata_
                 .ownership_borrowed_retainable_compatibility_returns_not_retained_sites
          << ";compatibility_consumed_sites="
          << frontend_metadata_
                 .ownership_borrowed_retainable_compatibility_consumed_sites
          << ";follow_on_surface=objc3c.ownership.helperruntime.surface.v1\n";
      // runtime/helper-freeze anchor: publish the private Part 8
      // runtime/helper contract above the existing lowering and ABI packets so
      // emitted IR truthfully advertises that cleanup/resource proof and
      // retainable-family interop still reuse the existing ARC/autorelease
      // helper cluster rather than a new runtime subsystem.
      out << "; ownership_system_helper_runtime_contract = "
          << Objc3OwnershipSystemHelperRuntimeContractSummary() << "\n";
      // live runtime-integration anchor: publish the executable Part
      // 8 runtime boundary so linked probes can prove cleanup/resource and
      // retainable-family helper traffic through the emitted function body
      // rather than stopping at the frozen contract summary.
      out << "; ownership_live_cleanup_retainable_runtime_integration = "
          << Objc3OwnershipLiveCleanupRetainableIntegrationSummary() << "\n";
    }
    if (!frontend_metadata_
             .lowering_task_runtime_interop_cancellation_replay_key.empty()) {
      out << "; task_runtime_interop_cancellation_lowering = "
          << frontend_metadata_
                 .lowering_task_runtime_interop_cancellation_replay_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_concurrency_replay_race_guard_replay_key
             .empty()) {
      out << "; concurrency_replay_race_guard_lowering = "
          << frontend_metadata_
                 .lowering_concurrency_replay_race_guard_replay_key
          << "\n";
    }
    if (!frontend_metadata_.lowering_unsafe_pointer_extension_replay_key
             .empty()) {
      out << "; unsafe_pointer_extension_lowering = "
          << frontend_metadata_.lowering_unsafe_pointer_extension_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .lowering_inline_asm_intrinsic_governance_replay_key.empty()) {
      out << "; inline_asm_intrinsic_governance_lowering = "
          << frontend_metadata_
                 .lowering_inline_asm_intrinsic_governance_replay_key
          << "\n";
    }
    if (!frontend_metadata_
             .ownership_aware_lowering_core_feature_expansion_key.empty()) {
      out << "; ownership_aware_lowering_core_feature_expansion = "
          << frontend_metadata_
                 .ownership_aware_lowering_core_feature_expansion_key
          << "\n";
    }
    out << "; ownership_aware_lowering_core_feature_expansion_ready = "
        << (frontend_metadata_
                    .ownership_aware_lowering_core_feature_expansion_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ownership_aware_lowering_performance_quality_guardrails_key.empty()) {
      out << "; ownership_aware_lowering_performance_quality_guardrails = "
          << frontend_metadata_
                 .ownership_aware_lowering_performance_quality_guardrails_key
          << "\n";
    }
    out << "; ownership_aware_lowering_performance_quality_guardrails_ready = "
        << (frontend_metadata_
                    .ownership_aware_lowering_performance_quality_guardrails_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ownership_aware_lowering_cross_lane_integration_key.empty()) {
      out << "; ownership_aware_lowering_cross_lane_integration = "
          << frontend_metadata_
                 .ownership_aware_lowering_cross_lane_integration_key
          << "\n";
    }
    out << "; ownership_aware_lowering_cross_lane_integration_ready = "
        << (frontend_metadata_
                    .ownership_aware_lowering_cross_lane_integration_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.lowering_pass_graph_core_feature_key.empty()) {
      out << "; lowering_pass_graph_core_feature = "
          << frontend_metadata_.lowering_pass_graph_core_feature_key << "\n";
    }
    out << "; lowering_pass_graph_core_feature_ready = "
        << (frontend_metadata_.lowering_pass_graph_core_feature_ready ? "true"
                                                                      : "false")
        << "\n";
    if (!frontend_metadata_.lowering_pass_graph_core_feature_expansion_key
             .empty()) {
      out << "; lowering_pass_graph_core_feature_expansion = "
          << frontend_metadata_
                 .lowering_pass_graph_core_feature_expansion_key
          << "\n";
    }
    out << "; lowering_pass_graph_core_feature_expansion_ready = "
        << (frontend_metadata_
                    .lowering_pass_graph_core_feature_expansion_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.lowering_pass_graph_edge_case_compatibility_key
             .empty()) {
      out << "; lowering_pass_graph_edge_case_compatibility = "
          << frontend_metadata_
                 .lowering_pass_graph_edge_case_compatibility_key
          << "\n";
    }
    out << "; lowering_pass_graph_edge_case_compatibility_ready = "
        << (frontend_metadata_
                    .lowering_pass_graph_edge_case_compatibility_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.lowering_pass_graph_edge_case_robustness_key.empty()) {
      out << "; lowering_pass_graph_edge_case_robustness = "
          << frontend_metadata_.lowering_pass_graph_edge_case_robustness_key
          << "\n";
    }
    out << "; lowering_pass_graph_edge_case_robustness_ready = "
        << (frontend_metadata_.lowering_pass_graph_edge_case_robustness_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .lowering_pass_graph_diagnostics_hardening_key.empty()) {
      out << "; lowering_pass_graph_diagnostics_hardening = "
          << frontend_metadata_
                 .lowering_pass_graph_diagnostics_hardening_key
          << "\n";
    }
    out << "; lowering_pass_graph_diagnostics_hardening_ready = "
        << (frontend_metadata_.lowering_pass_graph_diagnostics_hardening_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.lowering_pass_graph_recovery_determinism_key
             .empty()) {
      out << "; lowering_pass_graph_recovery_determinism = "
          << frontend_metadata_.lowering_pass_graph_recovery_determinism_key
          << "\n";
    }
    out << "; lowering_pass_graph_recovery_determinism_ready = "
        << (frontend_metadata_.lowering_pass_graph_recovery_determinism_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.lowering_pass_graph_conformance_matrix_key
             .empty()) {
      out << "; lowering_pass_graph_conformance_matrix = "
          << frontend_metadata_.lowering_pass_graph_conformance_matrix_key
          << "\n";
    }
    out << "; lowering_pass_graph_conformance_matrix_ready = "
        << (frontend_metadata_.lowering_pass_graph_conformance_matrix_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.lowering_pass_graph_conformance_corpus_key.empty()) {
      out << "; lowering_pass_graph_conformance_corpus = "
          << frontend_metadata_.lowering_pass_graph_conformance_corpus_key
          << "\n";
    }
    out << "; lowering_pass_graph_conformance_corpus_ready = "
        << (frontend_metadata_.lowering_pass_graph_conformance_corpus_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .lowering_pass_graph_performance_quality_guardrails_key.empty()) {
      out << "; lowering_pass_graph_performance_quality_guardrails = "
          << frontend_metadata_
                 .lowering_pass_graph_performance_quality_guardrails_key
          << "\n";
    }
    out << "; lowering_pass_graph_performance_quality_guardrails_ready = "
        << (frontend_metadata_
                    .lowering_pass_graph_performance_quality_guardrails_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.ir_emission_completeness_modular_split_key.empty()) {
      out << "; ir_emission_completeness_modular_split = "
          << frontend_metadata_.ir_emission_completeness_modular_split_key
          << "\n";
    }
    out << "; ir_emission_completeness_modular_split_ready = "
        << (frontend_metadata_
                    .ir_emission_completeness_modular_split_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.ir_emission_core_feature_impl_key.empty()) {
      out << "; ir_emission_core_feature_impl = "
          << frontend_metadata_.ir_emission_core_feature_impl_key
          << "\n";
    }
    out << "; ir_emission_core_feature_impl_ready = "
        << (frontend_metadata_.ir_emission_core_feature_impl_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.ir_emission_core_feature_expansion_key.empty()) {
      out << "; ir_emission_core_feature_expansion = "
          << frontend_metadata_.ir_emission_core_feature_expansion_key
          << "\n";
    }
    out << "; ir_emission_core_feature_expansion_ready = "
        << (frontend_metadata_.ir_emission_core_feature_expansion_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_edge_case_compatibility_key.empty()) {
      out << "; ir_emission_core_feature_edge_case_compatibility = "
          << frontend_metadata_
                 .ir_emission_core_feature_edge_case_compatibility_key
          << "\n";
    }
    out << "; ir_emission_core_feature_edge_case_compatibility_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_edge_case_compatibility_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_edge_case_robustness_key.empty()) {
      out << "; ir_emission_core_feature_edge_case_robustness = "
          << frontend_metadata_
                 .ir_emission_core_feature_edge_case_robustness_key
          << "\n";
    }
    out << "; ir_emission_core_feature_edge_case_robustness_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_edge_case_robustness_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_diagnostics_hardening_key.empty()) {
      out << "; ir_emission_core_feature_diagnostics_hardening = "
          << frontend_metadata_
                 .ir_emission_core_feature_diagnostics_hardening_key
          << "\n";
    }
    out << "; ir_emission_core_feature_diagnostics_hardening_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_diagnostics_hardening_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_recovery_determinism_key.empty()) {
      out << "; ir_emission_core_feature_recovery_determinism = "
          << frontend_metadata_
                 .ir_emission_core_feature_recovery_determinism_key
          << "\n";
    }
    out << "; ir_emission_core_feature_recovery_determinism_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_recovery_determinism_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_conformance_matrix_key.empty()) {
      out << "; ir_emission_core_feature_conformance_matrix = "
          << frontend_metadata_
                 .ir_emission_core_feature_conformance_matrix_key
          << "\n";
    }
    out << "; ir_emission_core_feature_conformance_matrix_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_conformance_matrix_ready
