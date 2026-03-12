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

bool ResolveGlobalInitializerValues(const std::vector<GlobalDecl> &globals, std::vector<int> &values);

class Objc3IREmitter {
 public:
  enum class SynthesizedAccessorKind {
    None,
    Getter,
    Setter,
  };

  struct MethodDefinition {
    std::string symbol;
    std::string implementation_name;
    std::string method_owner_identity;
    std::string superclass_name;
    const Objc3MethodDecl *method = nullptr;
    SynthesizedAccessorKind synthesized_accessor_kind =
        SynthesizedAccessorKind::None;
    std::string synthesized_storage_symbol;
    ValueType synthesized_value_type = ValueType::Unknown;
    std::string synthesized_ownership_lifetime_profile;
    std::string synthesized_ownership_runtime_hook_profile;
    std::string synthesized_accessor_ownership_profile;
  };

  struct SynthesizedPropertyStorage {
    std::string symbol;
    std::string binding_symbol;
  };

  static bool IsImplementationOwnedPropertyBundle(
      const Objc3IRRuntimeMetadataPropertyBundle &bundle) {
    return bundle.owner_kind == "class-implementation" ||
           bundle.owner_kind == "category-implementation";
  }

  static std::string BuildSynthesizedInstanceMethodOwnerIdentity(
      const std::string &declaration_owner_identity,
      const std::string &selector) {
    return declaration_owner_identity + "::instance_method:" + selector;
  }

  static std::string BuildSynthesizedPropertyStorageSymbol(
      const std::string &binding_symbol) {
    std::string out = "objc3_property_storage_";
    out.reserve(out.size() + binding_symbol.size());
    for (unsigned char ch : binding_symbol) {
      if (std::isalnum(ch) != 0) {
        out.push_back(static_cast<char>(ch));
      } else {
        out.push_back('_');
      }
    }
    if (out == "objc3_property_storage_") {
      out += "anonymous";
    }
    return out;
  }

  static ValueType RuntimeMetadataValueType(const std::string &type_name) {
    if (type_name == "void") {
      return ValueType::Void;
    }
    if (type_name == "bool") {
      return ValueType::Bool;
    }
    if (type_name.empty()) {
      return ValueType::Unknown;
    }
    return ValueType::I32;
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
    for (const auto &interface_decl : program_.interfaces) {
      if (interface_decl.has_category) {
        continue;
      }
      implementation_superclass_names.emplace(interface_decl.name,
                                              interface_decl.super_name);
    }
    std::unordered_map<std::string, std::size_t> method_symbol_counts;
    std::unordered_set<std::string> method_owner_identities;
    for (const auto &implementation : program_.implementations) {
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
                             SynthesizedAccessorKind::None,
                             std::string{},
                             ValueType::Unknown,
                             std::string{},
                             std::string{},
                             std::string{}});
        if (!method.scope_path_symbol.empty()) {
          method_owner_identities.insert(method.scope_path_symbol);
        }
      }
    }
    std::unordered_map<std::string, std::string>
        synthesized_storage_symbols_by_binding;
    synthesized_storage_symbols_by_binding.reserve(
        frontend_metadata_
            .runtime_metadata_property_bundles_lexicographic.size());
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
      const auto [storage_it, inserted_storage] =
          synthesized_storage_symbols_by_binding.emplace(
              bundle.executable_synthesized_binding_symbol,
              BuildSynthesizedPropertyStorageSymbol(
                  bundle.executable_synthesized_binding_symbol));
      if (inserted_storage) {
        synthesized_property_storages_.push_back(
            SynthesizedPropertyStorage{storage_it->second,
                                       bundle.executable_synthesized_binding_symbol});
      }
      const auto append_synthesized_method =
          [&](const std::string &selector, SynthesizedAccessorKind kind) {
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
                storage_it->second,
                property_type,
                bundle.ownership_lifetime_profile,
                bundle.ownership_runtime_hook_profile,
                bundle.accessor_ownership_profile,
            });
            ++synthesized_property_accessor_count_;
          };
      append_synthesized_method(bundle.effective_getter_selector,
                                SynthesizedAccessorKind::Getter);
      if (!boundary_error_.empty()) {
        return;
      }
      if (bundle.effective_setter_available) {
        append_synthesized_method(bundle.effective_setter_selector,
                                  SynthesizedAccessorKind::Setter);
        if (!boundary_error_.empty()) {
          return;
        }
      }
    }
    function_signatures_ = BuildLoweredFunctionSignatures(program_);
    CollectKnownClassReceiverConstants();
    CollectCanonicalPoolLiterals();
    CollectMutableGlobalSymbols();
    CollectFunctionEffects();
  }

  bool Emit(std::string &ir, std::string &error) {
    runtime_dispatch_call_emitted_ = false;
    fail_open_fallback_triggered_ = false;
    fail_open_fallback_reason_.clear();
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

    for (const auto &storage : synthesized_property_storages_) {
      body << "@" << storage.symbol << " = private global i32 0, align 4\n";
    }
    if (!synthesized_property_storages_.empty()) {
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

    if (fail_open_fallback_triggered_) {
      error = "lowering encountered unsupported fail-closed path: " + fail_open_fallback_reason_;
      return false;
    }

    std::ostringstream out;
    out << "; objc3c native frontend IR\n";
    out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";
    out << "; runtime_dispatch_decl = " << Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_) << "\n";
    out << "; simd_vector_lowering = " << Objc3SimdVectorTypeLoweringReplayKey() << "\n";
    if (!frontend_metadata_.lowering_property_synthesis_ivar_binding_replay_key.empty()) {
      // M257-A001 property-ivar executable-source-closure anchor:
      // preserve the source freeze as a lowering-only replay marker.
      out << "; property_synthesis_ivar_binding_lowering = "
          << frontend_metadata_.lowering_property_synthesis_ivar_binding_replay_key << "\n";
    }
    if (!frontend_metadata_.executable_property_ivar_source_model_replay_key.empty()) {
      // M257-A002 property-ivar source-model completion anchor:
      // publish the completed property attribute/accessor ownership/layout
      // handoff without reopening the legacy descriptor shapes from M253-C004.
      // M257-B001 property-ivar executable semantics anchor:
      // runtime-meaningful synthesis/accessor/storage semantics stay frozen on
      // this replay marker until accessor bodies and storage realization land.
      // M257-B002 property-synthesis implementation anchor:
      // sema now resolves authoritative default ivar bindings from
      // interface-declared class properties even when the implementation does
      // not redeclare those properties.
      // M257-B003 accessor legality expansion anchor:
      // IR only consumes sema-approved effective accessor selectors and
      // ownership/atomicity profiles after duplicate selector and unsupported
      // storage interaction diagnostics have already fail-closed source.
      out << "; property_ivar_source_model_completion = "
          << frontend_metadata_.executable_property_ivar_source_model_replay_key
          << "\n";
      // M257-C001 accessor/layout lowering freeze anchor:
      // lane-C now republishes the current property descriptor, ivar layout,
      // and synthesized binding handoff directly in emitted IR without yet
      // synthesizing accessor bodies or realizing runtime storage/layout.
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
      // M260-A002 runtime-backed object ownership attribute surface anchor:
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
        // M257-C002 ivar offset/layout emission anchor: lane-C now materializes
        // real retained offset globals and per-owner layout tables inside the
        // ivar descriptor section, but runtime allocation and accessor
        // execution remain deferred to later issues.
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
        // M257-C003 synthesized accessor/property lowering anchor: lane-C now
        // materializes missing implementation-owned property accessors as real
        // method bodies backed by deterministic storage globals and republishes
        // the widened property-descriptor surface in emitted IR.
        out << "; executable_synthesized_accessor_property_lowering = "
            << Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary()
            << ";synthesized_accessor_entries="
            << synthesized_property_accessor_count_
            << ";synthesized_storage_globals="
            << synthesized_property_storages_.size() << "\n";
        // M257-D001 runtime property/layout consumption freeze anchor: lane-D
        // now freezes the truthful runtime surface above C003. Runtime
        // consumes emitted accessor implementation pointers and
        // property/layout attachment identities, but repeated alloc/new still
        // normalize onto one canonical realized instance identity per class
        // until D002 introduces per-instance slot allocation.
        out << "; runtime_property_layout_consumption = "
            << Objc3RuntimePropertyLayoutConsumptionSummary()
            << ";property_descriptor_entries="
            << frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                   .size()
            << ";ivar_layout_owner_entries="
            << frontend_metadata_.executable_ivar_layout_owner_entries
            << ";synthesized_accessor_entries="
            << synthesized_property_accessor_count_ << "\n";
        // M257-D002 instance-allocation-layout-runtime anchor: lane-D now
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
        // M257-D003 property-metadata-reflection anchor: lane-D now exposes a
        // private reflective helper surface over the realized property graph so
        // runtime probes can query property/accessor/layout facts directly.
        // M257-E001 property-ivar-execution gate anchor: lane-E freezes the
        // A002/B003/C003/D003 property proof chain over this same emitted
        // surface before runnable sample expansion is allowed.
        // M257-E002 runnable property-ivar execution-matrix anchor: lane-E's
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
    if (!frontend_metadata_.runtime_metadata_section_scaffold_contract_id.empty()) {
      out << "; runtime_metadata_section_scaffold = "
          << frontend_metadata_.runtime_metadata_section_scaffold_contract_id
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
        << ", fallback_model="
        << kObjc3RuntimeSelectorLookupTablesFallbackModel << "\n";
    out << "; runtime_method_cache_slow_path_lookup = contract="
        << kObjc3RuntimeMethodCacheSlowPathContractId
        << ", receiver_normalization_model="
        << kObjc3RuntimeMethodCacheSlowPathReceiverNormalizationModel
        << ", resolution_model="
        << kObjc3RuntimeMethodCacheSlowPathResolutionModel
        << ", cache_model="
        << kObjc3RuntimeMethodCacheSlowPathCacheModel
        << ", fallback_model="
        << kObjc3RuntimeMethodCacheSlowPathFallbackModel << "\n";
    out << "; runtime_protocol_category_method_resolution = contract="
        << kObjc3RuntimeProtocolCategoryMethodResolutionContractId
        << ", category_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionCategoryModel
        << ", protocol_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionProtocolModel
        << ", fallback_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionFallbackModel << "\n";
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
    out << "; part3_optional_keypath_lowering = "
        << Objc3Part3OptionalKeypathLoweringSummary() << "\n";
    // M266-C001 control-flow safety lowering freeze anchor: the frontend now
    // publishes one lowering-owned packet for admitted Part 5 control-flow
    // sites while native IR lowering still fails closed on guard/match/defer
    // execution until later M266 lowering/runtime issues materialize it.
    out << "; part5_control_flow_safety_lowering = "
        << Objc3Part5ControlFlowSafetyLoweringSummary() << "\n";
    out << "; part3_optional_keypath_runtime_helper_contract = "
        << Objc3Part3OptionalKeypathRuntimeHelperContractSummary() << "\n";
    if (!frontend_metadata_.lowering_super_dispatch_method_family_replay_key.empty()) {
      out << "; super_dispatch_method_family_lowering = "
          << frontend_metadata_.lowering_super_dispatch_method_family_replay_key << "\n";
    }
    if (!frontend_metadata_.lowering_runtime_shim_host_link_replay_key.empty()) {
      out << "; runtime_shim_host_link_lowering = "
          << frontend_metadata_.lowering_runtime_shim_host_link_replay_key << "\n";
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
      // M258-A001 runtime-aware import/module surface anchor: the frontend
      // still preserves only the local translation-unit module-import graph
      // profile in emitted IR.
      // M258-A001/A002 runtime-aware import/module surface anchor: the
      // frontend now emits a canonical runtime-import surface artifact for
      // later cross-translation-unit consumers.
      // M258-B001 cross-module semantic preservation anchor: imported runtime metadata semantics are not lowered into IR; the lane-B surface freezes
      // semantic preservation requirements without landing imported metadata
      // semantic equivalence yet.
      // M258-B002 imported metadata semantic rules anchor: imported runtime
      // surface artifacts may now be consumed and validated before IR
      // emission, but the imported runtime metadata payloads themselves still
      // are not lowered into IR in this lane.
      // M258-C001 serialized metadata import/lowering anchor: imported
      // runtime surface artifacts now freeze the semantic handoff boundary,
      // but serialized imported metadata payloads still are not rehydrated,
      // reused incrementally, or lowered into IR in this lane.
      // M258-C002 serialized metadata artifact reuse anchor: emitted
      // runtime-import-surface artifacts may now carry a transitive serialized
      // runtime-metadata payload for downstream frontend reuse, but imported
      // payloads still are not lowered directly into IR in this lane.
      // M258-D001 cross-module build/runtime orchestration anchor:
      // cross-module link-plan packaging and aggregated runtime-registration
      // orchestration remain outside the IR emitter; lane D only freezes the
      // boundary between emitted import-surface reuse payloads and the local
      // runtime registration manifest here.
      // M258-D002 cross-module runtime packaging anchor: lane D now consumes
      // those emitted object-local artifacts to publish an ordered cross-module
      // link plan and runtime-registration proof, but the IR emitter remains
      // object-local and does not directly orchestrate multi-image packaging.
      // M258-E001 cross-module object-model gate anchor: the truthful gate
      // still lives in the emitted evidence chain and not in any new
      // cross-module IR emitter surface.
      // M258-E002 runnable import/module execution-matrix anchor: the IR
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
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_conformance_corpus_key.empty()) {
      out << "; ir_emission_core_feature_conformance_corpus = "
          << frontend_metadata_
                 .ir_emission_core_feature_conformance_corpus_key
          << "\n";
    }
    out << "; ir_emission_core_feature_conformance_corpus_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_conformance_corpus_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_performance_quality_guardrails_key.empty()) {
      out << "; ir_emission_core_feature_performance_quality_guardrails = "
          << frontend_metadata_
                 .ir_emission_core_feature_performance_quality_guardrails_key
          << "\n";
    }
    out << "; ir_emission_core_feature_performance_quality_guardrails_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_performance_quality_guardrails_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_cross_lane_integration_sync_key.empty()) {
      out << "; ir_emission_core_feature_cross_lane_integration_sync = "
          << frontend_metadata_
                 .ir_emission_core_feature_cross_lane_integration_sync_key
          << "\n";
    }
    out << "; ir_emission_core_feature_cross_lane_integration_sync_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_cross_lane_integration_sync_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.ir_emission_core_feature_advanced_core_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_core_shard1 = "
          << frontend_metadata_.ir_emission_core_feature_advanced_core_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_core_shard1_ready = "
        << (frontend_metadata_.ir_emission_core_feature_advanced_core_shard1_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_advanced_edge_compatibility_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_edge_compatibility_shard1 = "
          << frontend_metadata_
                 .ir_emission_core_feature_advanced_edge_compatibility_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_edge_compatibility_shard1_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_advanced_edge_compatibility_shard1_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_advanced_diagnostics_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_diagnostics_shard1 = "
          << frontend_metadata_
                 .ir_emission_core_feature_advanced_diagnostics_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_diagnostics_shard1_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_advanced_diagnostics_shard1_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_advanced_conformance_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_conformance_shard1 = "
          << frontend_metadata_
                 .ir_emission_core_feature_advanced_conformance_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_conformance_shard1_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_advanced_conformance_shard1_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_advanced_integration_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_integration_shard1 = "
          << frontend_metadata_
                 .ir_emission_core_feature_advanced_integration_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_integration_shard1_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_advanced_integration_shard1_ready
                ? "true"
                : "false")
        << "\n";
    out << "; simd_vector_function_signatures = " << vector_signature_function_count_ << "\n";
    out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)
        << ", compatibility_mode=" << frontend_metadata_.compatibility_mode
        << ", arc_mode=" << frontend_metadata_.arc_mode
        << ", migration_assist=" << (frontend_metadata_.migration_assist ? "true" : "false")
        << ", migration_legacy_total=" << frontend_metadata_.migration_legacy_total() << "\n";
    out << "; frontend_objc_interface_implementation_profile = declared_interfaces="
        << frontend_metadata_.declared_interfaces
        << ", declared_implementations=" << frontend_metadata_.declared_implementations
        << ", resolved_interface_symbols=" << frontend_metadata_.resolved_interface_symbols
        << ", resolved_implementation_symbols=" << frontend_metadata_.resolved_implementation_symbols
        << ", interface_method_symbols=" << frontend_metadata_.interface_method_symbols
        << ", implementation_method_symbols=" << frontend_metadata_.implementation_method_symbols
        << ", linked_implementation_symbols=" << frontend_metadata_.linked_implementation_symbols
        << ", deterministic_interface_implementation_handoff="
        << (frontend_metadata_.deterministic_interface_implementation_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_protocol_category_profile = declared_protocols="
        << frontend_metadata_.declared_protocols
        << ", declared_categories=" << frontend_metadata_.declared_categories
        << ", resolved_protocol_symbols=" << frontend_metadata_.resolved_protocol_symbols
        << ", resolved_category_symbols=" << frontend_metadata_.resolved_category_symbols
        << ", protocol_method_symbols=" << frontend_metadata_.protocol_method_symbols
        << ", category_method_symbols=" << frontend_metadata_.category_method_symbols
        << ", linked_category_symbols=" << frontend_metadata_.linked_category_symbols
        << ", deterministic_protocol_category_handoff="
        << (frontend_metadata_.deterministic_protocol_category_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_class_protocol_category_linking_profile = declared_class_interfaces="
        << frontend_metadata_.declared_class_interfaces
        << ", declared_class_implementations=" << frontend_metadata_.declared_class_implementations
        << ", resolved_class_interfaces=" << frontend_metadata_.resolved_class_interfaces
        << ", resolved_class_implementations=" << frontend_metadata_.resolved_class_implementations
        << ", linked_class_method_symbols=" << frontend_metadata_.linked_class_method_symbols
        << ", linked_category_method_symbols=" << frontend_metadata_.linked_category_method_symbols
        << ", protocol_composition_sites=" << frontend_metadata_.protocol_composition_sites
        << ", protocol_composition_symbols=" << frontend_metadata_.protocol_composition_symbols
        << ", category_composition_sites=" << frontend_metadata_.category_composition_sites
        << ", category_composition_symbols=" << frontend_metadata_.category_composition_symbols
        << ", invalid_protocol_composition_sites=" << frontend_metadata_.invalid_protocol_composition_sites
        << ", deterministic_class_protocol_category_linking_handoff="
        << (frontend_metadata_.deterministic_class_protocol_category_linking_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_selector_normalization_profile = method_declaration_entries="
        << frontend_metadata_.selector_method_declaration_entries
        << ", normalized_method_declarations=" << frontend_metadata_.selector_normalized_method_declarations
        << ", selector_piece_entries=" << frontend_metadata_.selector_piece_entries
        << ", selector_piece_parameter_links=" << frontend_metadata_.selector_piece_parameter_links
        << ", deterministic_selector_normalization_handoff="
        << (frontend_metadata_.deterministic_selector_normalization_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_property_attribute_profile = property_declaration_entries="
        << frontend_metadata_.property_declaration_entries
        << ", property_attribute_entries=" << frontend_metadata_.property_attribute_entries
        << ", property_attribute_value_entries=" << frontend_metadata_.property_attribute_value_entries
        << ", property_accessor_modifier_entries=" << frontend_metadata_.property_accessor_modifier_entries
        << ", property_getter_selector_entries=" << frontend_metadata_.property_getter_selector_entries
        << ", property_setter_selector_entries=" << frontend_metadata_.property_setter_selector_entries
        << ", deterministic_property_attribute_handoff="
        << (frontend_metadata_.deterministic_property_attribute_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_id_class_sel_object_pointer_typecheck_profile = id_typecheck_sites="
        << frontend_metadata_.id_typecheck_sites
        << ", class_typecheck_sites=" << frontend_metadata_.class_typecheck_sites
        << ", sel_typecheck_sites=" << frontend_metadata_.sel_typecheck_sites
        << ", object_pointer_typecheck_sites=" << frontend_metadata_.object_pointer_typecheck_sites
        << ", total_typecheck_sites=" << frontend_metadata_.id_class_sel_object_pointer_typecheck_sites_total
        << ", deterministic_id_class_sel_object_pointer_typecheck_handoff="
        << (frontend_metadata_.deterministic_id_class_sel_object_pointer_typecheck_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_dispatch_surface_classification_profile = instance_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_instance_sites
        << ", class_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_class_sites
        << ", super_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_super_sites
        << ", direct_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_direct_sites
        << ", dynamic_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_dynamic_sites
        << ", instance_entrypoint_family="
        << frontend_metadata_
               .dispatch_surface_classification_instance_entrypoint_family
        << ", class_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_class_entrypoint_family
        << ", super_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_super_entrypoint_family
        << ", direct_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_direct_entrypoint_family
        << ", dynamic_entrypoint_family="
        << frontend_metadata_
               .dispatch_surface_classification_dynamic_entrypoint_family
        << ", deterministic_dispatch_surface_classification_handoff="
        << (frontend_metadata_.deterministic_dispatch_surface_classification_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_message_send_selector_lowering_profile = message_send_sites="
        << frontend_metadata_.message_send_selector_lowering_sites
        << ", unary_selector_sites=" << frontend_metadata_.message_send_selector_lowering_unary_sites
        << ", keyword_selector_sites=" << frontend_metadata_.message_send_selector_lowering_keyword_sites
        << ", selector_piece_sites=" << frontend_metadata_.message_send_selector_lowering_selector_piece_sites
        << ", argument_expression_sites="
        << frontend_metadata_.message_send_selector_lowering_argument_expression_sites
        << ", receiver_expression_sites=" << frontend_metadata_.message_send_selector_lowering_receiver_sites
        << ", selector_literal_entries="
        << frontend_metadata_.message_send_selector_lowering_selector_literal_entries
        << ", selector_literal_characters="
        << frontend_metadata_.message_send_selector_lowering_selector_literal_characters
        << ", deterministic_message_send_selector_lowering_handoff="
        << (frontend_metadata_.deterministic_message_send_selector_lowering_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_dispatch_abi_marshalling_profile = message_send_sites="
        << frontend_metadata_.dispatch_abi_marshalling_message_send_sites
        << ", receiver_slots_marshaled=" << frontend_metadata_.dispatch_abi_marshalling_receiver_slots_marshaled
        << ", selector_slots_marshaled=" << frontend_metadata_.dispatch_abi_marshalling_selector_slots_marshaled
        << ", argument_value_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_value_slots_marshaled
        << ", argument_padding_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_padding_slots_marshaled
        << ", argument_total_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_total_slots_marshaled
        << ", total_marshaled_slots=" << frontend_metadata_.dispatch_abi_marshalling_total_marshaled_slots
        << ", runtime_dispatch_arg_slots="
        << frontend_metadata_.dispatch_abi_marshalling_runtime_dispatch_arg_slots
        << ", deterministic_dispatch_abi_marshalling_handoff="
        << (frontend_metadata_.deterministic_dispatch_abi_marshalling_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_nil_receiver_semantics_foldability_profile = message_send_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_message_send_sites
        << ", receiver_nil_literal_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_receiver_nil_literal_sites
        << ", nil_receiver_semantics_enabled_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_enabled_sites
        << ", nil_receiver_foldable_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_foldable_sites
        << ", nil_receiver_runtime_dispatch_required_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_runtime_dispatch_required_sites
        << ", non_nil_receiver_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_non_nil_receiver_sites
        << ", contract_violation_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_contract_violation_sites
        << ", deterministic_nil_receiver_semantics_foldability_handoff="
        << (frontend_metadata_.deterministic_nil_receiver_semantics_foldability_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_super_dispatch_method_family_profile = message_send_sites="
        << frontend_metadata_.super_dispatch_method_family_message_send_sites
        << ", receiver_super_identifier_sites="
        << frontend_metadata_.super_dispatch_method_family_receiver_super_identifier_sites
        << ", super_dispatch_enabled_sites=" << frontend_metadata_.super_dispatch_method_family_enabled_sites
        << ", super_dispatch_requires_class_context_sites="
        << frontend_metadata_.super_dispatch_method_family_requires_class_context_sites
        << ", method_family_init_sites=" << frontend_metadata_.super_dispatch_method_family_init_sites
        << ", method_family_copy_sites=" << frontend_metadata_.super_dispatch_method_family_copy_sites
        << ", method_family_mutable_copy_sites="
        << frontend_metadata_.super_dispatch_method_family_mutable_copy_sites
        << ", method_family_new_sites=" << frontend_metadata_.super_dispatch_method_family_new_sites
        << ", method_family_none_sites=" << frontend_metadata_.super_dispatch_method_family_none_sites
        << ", method_family_returns_retained_result_sites="
        << frontend_metadata_.super_dispatch_method_family_returns_retained_result_sites
        << ", method_family_returns_related_result_sites="
        << frontend_metadata_.super_dispatch_method_family_returns_related_result_sites
        << ", contract_violation_sites="
        << frontend_metadata_.super_dispatch_method_family_contract_violation_sites
        << ", deterministic_super_dispatch_method_family_handoff="
        << (frontend_metadata_.deterministic_super_dispatch_method_family_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_runtime_shim_host_link_profile = message_send_sites="
        << frontend_metadata_.runtime_shim_host_link_message_send_sites
        << ", runtime_shim_required_sites=" << frontend_metadata_.runtime_shim_host_link_required_sites
        << ", runtime_shim_elided_sites=" << frontend_metadata_.runtime_shim_host_link_elided_sites
        << ", runtime_dispatch_arg_slots="
        << frontend_metadata_.runtime_shim_host_link_runtime_dispatch_arg_slots
        << ", runtime_dispatch_declaration_parameter_count="
        << frontend_metadata_.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count
        << ", runtime_dispatch_symbol=" << frontend_metadata_.runtime_shim_host_link_runtime_dispatch_symbol
        << ", default_runtime_dispatch_symbol_binding="
        << (frontend_metadata_.runtime_shim_host_link_default_runtime_dispatch_symbol_binding ? "true" : "false")
        << ", contract_violation_sites="
        << frontend_metadata_.runtime_shim_host_link_contract_violation_sites
        << ", deterministic_runtime_shim_host_link_handoff="
        << (frontend_metadata_.deterministic_runtime_shim_host_link_handoff ? "true" : "false")
        << "\n";
    // M260-A001 runtime-backed-object-ownership freeze anchor: the current
    // runnable object slice preserves ownership through property/accessor
    // metadata profiles plus these legacy ownership lowering summaries. No
    // live ARC runtime retain/release/autorelease execution hooks are emitted
    // here yet.
    // M260-B001 retainable-object semantic-rule freeze anchor: retain/release,
    // autoreleasepool, and destruction-order behavior are still represented by
    // deterministic summary lanes only; runtime-backed property/member
    // ownership metadata is live, but storage legality is not yet executable.
    out << "; retainable_object_semantic_rules_freeze = "
        << Objc3RetainableObjectSemanticRulesFreezeSummary() << "\n";
    // M260-B002 runtime-backed storage ownership legality anchor: explicit
    // object-property ownership qualifiers now participate in live semantic
    // legality, so the IR closeout surface publishes the exact owned/weak/
    // unowned contract now enforced before metadata emission.
    out << "; runtime_backed_storage_ownership_legality = "
        << Objc3RuntimeBackedStorageOwnershipLegalitySummary() << "\n";
    // M260-B003 autoreleasepool/destruction-order semantic expansion anchor:
    // lane-B still fail-closes autoreleasepool, but the IR closeout surface
    // now publishes the ownership-sensitive destruction-order contract that
    // distinguishes plain pool rejection from owned runtime-backed object
    // storage edges.
    out << "; runtime_backed_autoreleasepool_destruction_order = "
        << Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary() << "\n";
    // M260-C001 ownership-lowering baseline freeze anchor: the current IR
    // surface keeps ownership qualifier, retain/release, autoreleasepool, and
    // weak/unowned lowering on deterministic summary lanes only.
    // No live runtime ownership hooks are emitted here before M260-C002.
    out << "; ownership_lowering_baseline = "
        << Objc3OwnershipLoweringBaselineSummary() << "\n";
    if (synthesized_property_accessor_count_ > 0u) {
      // M260-C002 runtime hook emission anchor: synthesized accessors now
      // execute through runtime-owned helper entrypoints that consume the
      // current dispatch-frame property context rather than the old summary-only
      // ownership lane. The legacy storage globals stay emitted for historical
      // artifact compatibility, but owned and weak execution paths use the live
      // runtime hooks below.
      out << "; ownership_runtime_hook_emission = "
          << Objc3OwnershipRuntimeHookEmissionSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_
          << ";synthesized_storage_globals="
          << synthesized_property_storages_.size() << "\n";
      // M260-D001 runtime memory-management API freeze anchor: the public
      // runtime ABI remains narrow while lane-C emits the private helper
      // surface consumed by synthesized ownership accessors.
      out << "; runtime_memory_management_api = "
          << Objc3RuntimeMemoryManagementApiSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_
          << ";synthesized_storage_globals="
          << synthesized_property_storages_.size() << "\n";
    }
    if (synthesized_property_accessor_count_ > 0u ||
        frontend_metadata_.autoreleasepool_scope_lowering_scope_sites > 0u ||
        frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites >
            0u ||
        frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites > 0u ||
        frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites > 0u) {
      // M260-D002 runtime memory-management implementation anchor: emitted IR
      // now carries the private autoreleasepool push/pop runtime surface and
      // the live refcount/weak/autoreleasepool execution model that native mode
      // consumes for runtime-backed object programs.
      out << "; runtime_memory_management_implementation = "
          << Objc3RuntimeMemoryManagementImplementationSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_
          << ";synthesized_storage_globals="
          << synthesized_property_storages_.size()
          << ";autoreleasepool_scope_sites="
          << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
          << "\n";
    }
    // M260-E001 ownership-runtime-gate freeze anchor: lane-E freezes the
    // supported ownership runtime slice and its explicit non-goals as a
    // dedicated emitted boundary so later smoke/docs work can validate the
    // correct baseline without rediscovering it from prose alone.
    // M260-E002 ownership-smoke closeout anchor: the runnable smoke matrix
    // consumes this exact emitted gate boundary as the integrated ownership
    // proof surface for M260 closeout.
    out << "; ownership_runtime_gate = "
        << Objc3OwnershipRuntimeGateSummary() << "\n";
    // M261-A001 executable-block-source-closure anchor: emit the truthful
    // parser/AST boundary so lane-A can prove block literals entered the source
    // closure before runtime lowering remains fail closed.
    out << "; executable_block_source_closure = "
        << Objc3ExecutableBlockSourceClosureSummary() << "\n";
    // M261-A002 block-source-model-completion anchor: emit the completed
    // source-model replay boundary so source-only frontend runs and later
    // lowering work can agree on typed parameter signatures, capture storage
    // inventories, and invoke-surface symbols without reconstructing them.
    out << "; executable_block_source_model_completion = "
        << Objc3ExecutableBlockSourceModelCompletionSummary()
        << ";replay_key="
        << frontend_metadata_.lowering_block_source_model_completion_replay_key
        << "\n";
    // M261-A003 block-source-storage-annotation anchor: emit the truthful
    // byref/helper/escape-shape replay boundary so source-only manifests and
    // later runnable block lowering consume the same deterministic source
    // annotations while native block execution remains fail closed.
    out << "; executable_block_source_storage_annotations = "
        << Objc3ExecutableBlockSourceStorageAnnotationSummary()
        << ";replay_key="
        << frontend_metadata_
               .lowering_block_source_storage_annotation_replay_key
        << "\n";
    // M261-B001 block-runtime-semantic-rules freeze anchor: emit the current
    // semantic-rule boundary so block runtime follow-on work can preserve the
    // source-only admission/native fail-closed split without rediscovering it
    // from prose or historical issue packets.
    // M261-B002 capture-legality/escape/invocation implementation anchor:
    // the emitted summary line stays on the frozen runtime boundary while the
    // source-only sema path now enforces live capture legality and local
    // block-call typing ahead of runnable block-object lowering.
    // M261-B003 byref/copy-dispose/object-ownership anchor: helper-eligibility
    // totals in this emitted surface now reflect owned object captures as well as byref cells,
    // while native block execution still remains fail-closed.
    out << "; executable_block_runtime_semantic_rules = "
        << Objc3ExecutableBlockRuntimeSemanticRulesSummary() << "\n";
    // M261-C001 block-lowering-ABI/artifact-boundary freeze anchor: lane-C
    // now publishes the truthful lowering boundary required for runnable block
    // execution, while native emit still fails closed before any emitted block
    // object records, invoke thunks, byref cells, or helper bodies land.
    out << "; executable_block_lowering_abi_artifact_boundary = "
        << Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary() << "\n";
    // M261-C002 executable-block-object/invoke-thunk anchor: native lowering
    // now emits stack block objects plus internal invoke thunks for the narrow
    // readonly-scalar capture slice, while byref/helper/ownership-sensitive
    // cases remain explicitly deferred to C003.
    out << "; executable_block_object_invoke_thunk_lowering = "
        << Objc3ExecutableBlockObjectInvokeThunkLoweringSummary() << "\n";
    // M261-C003 byref-cell/copy-helper/dispose-helper anchor: native lowering
    // now widens the runnable block slice to non-escaping byref and owned
    // capture cases through stack helper emission and helper call sites, while
    // escaping heap promotion remains deferred to later M261 issues.
    out << "; executable_block_byref_helper_lowering = "
        << Objc3ExecutableBlockByrefHelperLoweringSummary() << "\n";
    // M261-C004 escaping-block runtime-hook anchor: lane-C now publishes the
    // escaping readonly-scalar block slice that lowers through runtime
    // promotion/invoke hooks while pointer-managed escaping captures remain
    // explicitly deferred to later runtime issues.
    out << "; executable_block_escape_runtime_hook_lowering = "
        << Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary() << "\n";
    // M261-D001 block-runtime API/object-layout freeze anchor: emitted IR now
    // republishes the current private helper ABI and private runtime layout
    // boundary so later runtime implementation issues preserve this exact
    // contract instead of widening it ad hoc.
    out << "; runtime_block_api_object_layout = "
        << Objc3RuntimeBlockApiObjectLayoutSummary() << "\n";
    // M261-D002 block-runtime allocation/copy-dispose/invoke anchor: emitted
    // IR now republishes the live runtime capability boundary for promoted
    // block records with helper-mediated copy/dispose support while byref and
    // ownership-interoperating escape paths remain deferred.
    out << "; runtime_block_allocation_copy_dispose_invoke_support = "
        << Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary() << "\n";
    // M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor:
    // emitted IR now republishes that escaping pointer-capture block handles
    // rewrite capture slots onto runtime-owned forwarding cells before helper
    // execution, keeping byref mutation and owned capture lifetimes live after
    // the source frame returns.
    out << "; runtime_block_byref_forwarding_heap_promotion_ownership_interop = "
        << Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary() << "\n";
    // M261-E001 runnable-block-runtime gate anchor: lane-E now freezes the
    // integrated block-runtime gate above the retained A003/B003/C004/D003
    // source, sema, lowering, and runtime proofs so later closeout work
    // cannot substitute metadata-only evidence.
    out << "; runnable_block_runtime_gate = "
        << Objc3RunnableBlockRuntimeGateSummary() << "\n";
    // M261-E002 runnable-block execution-matrix anchor: lane-E now closes the
    // current M261 slice by requiring integrated executable block probes above
    // the retained gate, without widening the public block ABI or helper
    // surface.
    out << "; runnable_block_execution_matrix = "
        << Objc3RunnableBlockExecutionMatrixSummary() << "\n";
    // M262-A001 ARC source-surface/mode-boundary anchor: emit the truthful
    // ARC-adjacent frontend/mode boundary so later ARC automation work cannot
    // silently claim a runnable `-fobjc-arc` mode before the driver and
    // executable ownership-qualified function/method path are actually live.
    out << "; arc_source_mode_boundary = "
        << Objc3ArcSourceModeBoundarySummary() << "\n";
    // M262-A002 ARC mode-handling core implementation anchor: publish the
    // explicit ARC-mode execution boundary so manifests and emitted IR stay
    // aligned on when ownership-qualified executable signatures are runnable.
    out << "; arc_mode_handling = "
        << Objc3ArcModeHandlingSummary(frontend_metadata_.arc_mode_enabled) << "\n";
    // M262-B001 ARC semantic-rule freeze anchor: publish the semantic
    // fail-closed boundary for property conflicts and deferred inference so IR
    // evidence stays aligned with semantic validation.
    out << "; arc_semantic_rules = "
        << Objc3ArcSemanticRulesSummary() << "\n";
    // M262-B002 ARC inference/lifetime implementation anchor: publish the
    // truthful semantic-upgrade boundary so emitted IR and manifests agree
    // when ARC mode has widened the supported slice from explicit-only
    // ownership spelling into inferred strong-owned retain/release activity.
    out << "; arc_inference_lifetime = "
        << Objc3ArcInferenceLifetimeSummary() << "\n";
    // M262-B003 ARC interaction-semantics expansion anchor: publish the
    // supported weak/autorelease-return/property-synthesis/block-interaction
    // semantic boundary so emitted IR stays aligned with the live ARC slice
    // instead of forcing later issues to reconstruct it out of older packets.
    out << "; arc_interaction_semantics = "
        << Objc3ArcInteractionSemanticsSummary() << "\n";
    // M262-C001 ARC lowering ABI/cleanup freeze anchor: publish the current
    // lowering/helper boundary directly into IR so later retain/release,
    // cleanup-scheduling, weak lowering, and autorelease-return work must
    // preserve one explicit contract instead of inferring it from older ARC
    // semantic packets.
    out << "; arc_lowering_abi_cleanup_model = "
        << Objc3ArcLoweringAbiCleanupModelSummary() << "\n";
    // M262-C002 ARC automatic-insertion implementation anchor: publish the
    // live param/return helper-insertion boundary so later ARC work extends a
    // real lowering surface instead of a summary-only semantic contract.
    out << "; arc_automatic_insertions = "
        << Objc3ArcAutomaticInsertionSummary() << "\n";
    // M262-C003 ARC cleanup/weak/lifetime implementation anchor: publish the
    // supported scope-exit cleanup, weak current-property helper, and block
    // lifetime cleanup boundary so later ARC/block widening extends a real
    // lowering/runtime surface rather than inferred behavior.
    out << "; arc_cleanup_weak_lifetime_hooks = "
        << Objc3ArcCleanupWeakLifetimeHooksSummary() << "\n";
    // M262-C004 ARC/block autorelease-return implementation anchor: publish
    // the supported escaping-block plus autoreleasing-return edge inventory so
    // later runtime ARC work extends a real branch-stable lowering surface
    // rather than re-deriving cleanup ordering from semantic summaries.
    out << "; arc_block_autorelease_return_lowering = "
        << Objc3ArcBlockAutoreleaseReturnLoweringSummary() << "\n";
    // M262-D001 runtime ARC helper API surface anchor: publish the private
    // helper ABI boundary that current ARC lowering already consumes so later
    // runtime implementation work extends a truthful runtime contract instead
    // of inferring helper availability from emitted calls alone.
    out << "; runtime_arc_helper_api_surface = "
        << Objc3RuntimeArcHelperApiSurfaceSummary() << "\n";
    // M262-D002 runtime ARC helper implementation anchor: publish the live
    // executable helper-runtime support boundary so later diagnostics or debug
    // instrumentation work extends a truthful runtime capability rather than
    // another summary-only marker.
    out << "; runtime_arc_helper_runtime_support = "
        << Objc3RuntimeArcHelperRuntimeSupportSummary() << "\n";
    // M262-D003 ownership-debug/runtime-validation anchor: publish the
    // private ARC debug snapshot boundary so lane-D validation can prove live
    // helper traffic without widening the public runtime ABI.
    out << "; runtime_arc_debug_instrumentation = "
        << Objc3RuntimeArcDebugInstrumentationSummary() << "\n";
    // M262-E001 runnable-arc-runtime gate anchor: lane-E freezes the supported
    // ARC slice above the existing mode-handling, interaction, lowering, and
    // runtime proof chain without claiming closeout-matrix coverage yet.
    out << "; runnable_arc_runtime_gate = "
        << Objc3RunnableArcRuntimeGateSummary() << "\n";
    // M262-E002 runnable-arc-closeout anchor: lane-E consumes the already-live
    // ARC proof chain plus integrated execution smoke as the closeout surface
    // without widening the supported ARC semantics or runtime ABI.
    out << "; runnable_arc_closeout = " << Objc3RunnableArcCloseoutSummary()
        << "\n";
    out << "; frontend_objc_ownership_qualifier_lowering_profile = ownership_qualifier_sites="
        << frontend_metadata_.ownership_qualifier_lowering_ownership_qualifier_sites
        << ", invalid_ownership_qualifier_sites="
        << frontend_metadata_.ownership_qualifier_lowering_invalid_ownership_qualifier_sites
        << ", object_pointer_type_annotation_sites="
        << frontend_metadata_.ownership_qualifier_lowering_object_pointer_type_annotation_sites
        << ", deterministic_ownership_qualifier_lowering_handoff="
        << (frontend_metadata_.deterministic_ownership_qualifier_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_retain_release_operation_lowering_profile = ownership_qualified_sites="
        << frontend_metadata_.retain_release_operation_lowering_ownership_qualified_sites
        << ", retain_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_retain_insertion_sites
        << ", release_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_release_insertion_sites
        << ", autorelease_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_autorelease_insertion_sites
        << ", contract_violation_sites="
        << frontend_metadata_.retain_release_operation_lowering_contract_violation_sites
        << ", deterministic_retain_release_operation_lowering_handoff="
        << (frontend_metadata_.deterministic_retain_release_operation_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_autoreleasepool_scope_lowering_profile = scope_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
        << ", scope_symbolized_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_symbolized_sites
        << ", max_scope_depth="
        << frontend_metadata_.autoreleasepool_scope_lowering_max_scope_depth
        << ", scope_entry_transition_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_entry_transition_sites
        << ", scope_exit_transition_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_exit_transition_sites
        << ", contract_violation_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_contract_violation_sites
        << ", deterministic_autoreleasepool_scope_lowering_handoff="
        << (frontend_metadata_.deterministic_autoreleasepool_scope_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_weak_unowned_semantics_lowering_profile = ownership_candidate_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_ownership_candidate_sites
        << ", weak_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_weak_reference_sites
        << ", unowned_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_unowned_reference_sites
        << ", unowned_safe_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_unowned_safe_reference_sites
        << ", weak_unowned_conflict_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_conflict_sites
        << ", contract_violation_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_contract_violation_sites
        << ", deterministic_weak_unowned_semantics_lowering_handoff="
        << (frontend_metadata_.deterministic_weak_unowned_semantics_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_arc_diagnostics_fixit_lowering_profile = ownership_arc_diagnostic_candidate_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites
        << ", ownership_arc_fixit_available_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites
        << ", ownership_arc_profiled_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites
        << ", ownership_arc_weak_unowned_conflict_diagnostic_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites
        << ", ownership_arc_empty_fixit_hint_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites
        << ", contract_violation_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_contract_violation_sites
        << ", deterministic_arc_diagnostics_fixit_lowering_handoff="
        << (frontend_metadata_.deterministic_arc_diagnostics_fixit_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_literal_capture_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_literal_sites
        << ", block_parameter_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_parameter_entries
        << ", block_capture_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_capture_entries
        << ", block_body_statement_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_body_statement_entries
        << ", block_empty_capture_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_empty_capture_sites
        << ", block_nondeterministic_capture_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_nondeterministic_capture_sites
        << ", block_non_normalized_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_non_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_literal_capture_lowering_contract_violation_sites
        << ", deterministic_block_literal_capture_lowering_handoff="
        << (frontend_metadata_.deterministic_block_literal_capture_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_abi_invoke_trampoline_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_block_literal_sites
        << ", invoke_argument_slots_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total
        << ", capture_word_count_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_word_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_body_statement_entries_total
        << ", descriptor_symbolized_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites
        << ", invoke_trampoline_symbolized_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites
        << ", missing_invoke_trampoline_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_missing_invoke_sites
        << ", non_normalized_layout_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_contract_violation_sites
        << ", deterministic_block_abi_invoke_trampoline_lowering_handoff="
        << (frontend_metadata_.deterministic_block_abi_invoke_trampoline_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_storage_escape_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_storage_escape_lowering_block_literal_sites
        << ", mutable_capture_count_total="
        << frontend_metadata_.block_storage_escape_lowering_mutable_capture_count_total
        << ", byref_slot_count_total="
        << frontend_metadata_.block_storage_escape_lowering_byref_slot_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_body_statement_entries_total
        << ", requires_byref_cells_sites="
        << frontend_metadata_.block_storage_escape_lowering_requires_byref_cells_sites
        << ", escape_analysis_enabled_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_analysis_enabled_sites
        << ", escape_to_heap_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites
        << ", escape_profile_normalized_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_profile_normalized_sites
        << ", byref_layout_symbolized_sites="
        << frontend_metadata_.block_storage_escape_lowering_byref_layout_symbolized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_storage_escape_lowering_contract_violation_sites
        << ", deterministic_block_storage_escape_lowering_handoff="
        << (frontend_metadata_.deterministic_block_storage_escape_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_copy_dispose_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_copy_dispose_lowering_block_literal_sites
        << ", mutable_capture_count_total="
        << frontend_metadata_.block_copy_dispose_lowering_mutable_capture_count_total
        << ", byref_slot_count_total="
        << frontend_metadata_.block_copy_dispose_lowering_byref_slot_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_body_statement_entries_total
        << ", copy_helper_required_sites="
        << frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites
        << ", dispose_helper_required_sites="
        << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites
        << ", profile_normalized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_profile_normalized_sites
        << ", copy_helper_symbolized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_copy_helper_symbolized_sites
        << ", dispose_helper_symbolized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_symbolized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_copy_dispose_lowering_contract_violation_sites
        << ", deterministic_block_copy_dispose_lowering_handoff="
        << (frontend_metadata_.deterministic_block_copy_dispose_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_determinism_perf_baseline_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_block_literal_sites
        << ", baseline_weight_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_baseline_weight_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_body_statement_entries_total
        << ", deterministic_capture_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_deterministic_capture_sites
        << ", heavy_tier_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_heavy_tier_sites
        << ", normalized_profile_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_normalized_profile_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_contract_violation_sites
        << ", deterministic_block_determinism_perf_baseline_lowering_handoff="
        << (frontend_metadata_.deterministic_block_determinism_perf_baseline_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_lightweight_generic_constraint_lowering_profile = generic_constraint_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_generic_constraint_sites
        << ", generic_suffix_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_generic_suffix_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_object_pointer_type_sites
        << ", terminated_generic_suffix_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_pointer_declarator_sites
        << ", normalized_constraint_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_normalized_constraint_sites
        << ", contract_violation_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_contract_violation_sites
        << ", deterministic_lightweight_generic_constraint_lowering_handoff="
        << (frontend_metadata_.deterministic_lightweight_generic_constraint_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_nullability_flow_warning_precision_lowering_profile = nullability_flow_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_object_pointer_type_sites
        << ", nullability_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nullability_suffix_sites
        << ", nullable_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nullable_suffix_sites
        << ", nonnull_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nonnull_suffix_sites
        << ", normalized_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_contract_violation_sites
        << ", deterministic_nullability_flow_warning_precision_lowering_handoff="
        << (frontend_metadata_.deterministic_nullability_flow_warning_precision_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_protocol_qualified_object_type_lowering_profile = protocol_qualified_object_type_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_protocol_composition_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_object_pointer_type_sites
        << ", terminated_protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_pointer_declarator_sites
        << ", normalized_protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites
        << ", contract_violation_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_contract_violation_sites
        << ", deterministic_protocol_qualified_object_type_lowering_handoff="
        << (frontend_metadata_.deterministic_protocol_qualified_object_type_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_variance_bridge_cast_lowering_profile = variance_bridge_cast_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_protocol_composition_sites
        << ", ownership_qualifier_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_ownership_qualifier_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_contract_violation_sites
        << ", deterministic_variance_bridge_cast_lowering_handoff="
        << (frontend_metadata_.deterministic_variance_bridge_cast_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_generic_metadata_abi_lowering_profile = generic_metadata_abi_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_sites
        << ", generic_suffix_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_generic_suffix_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_protocol_composition_sites
        << ", ownership_qualifier_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_ownership_qualifier_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_contract_violation_sites
        << ", deterministic_generic_metadata_abi_lowering_handoff="
        << (frontend_metadata_.deterministic_generic_metadata_abi_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_module_import_graph_lowering_profile = module_import_graph_sites="
        << frontend_metadata_.module_import_graph_lowering_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_.module_import_graph_lowering_import_edge_candidate_sites
        << ", namespace_segment_sites="
        << frontend_metadata_.module_import_graph_lowering_namespace_segment_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.module_import_graph_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.module_import_graph_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.module_import_graph_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.module_import_graph_lowering_contract_violation_sites
        << ", deterministic_module_import_graph_lowering_handoff="
        << (frontend_metadata_.deterministic_module_import_graph_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_namespace_collision_shadowing_lowering_profile = namespace_collision_shadowing_sites="
        << frontend_metadata_.namespace_collision_shadowing_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_contract_violation_sites
        << ", deterministic_namespace_collision_shadowing_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_namespace_collision_shadowing_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_public_private_api_partition_lowering_profile = public_private_api_partition_sites="
        << frontend_metadata_.public_private_api_partition_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_contract_violation_sites
        << ", deterministic_public_private_api_partition_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_public_private_api_partition_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_incremental_module_cache_invalidation_lowering_profile = incremental_module_cache_invalidation_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_contract_violation_sites
        << ", deterministic_incremental_module_cache_invalidation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_incremental_module_cache_invalidation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_cross_module_conformance_lowering_profile = cross_module_conformance_sites="
        << frontend_metadata_.cross_module_conformance_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.cross_module_conformance_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_contract_violation_sites
        << ", deterministic_cross_module_conformance_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_cross_module_conformance_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_throws_propagation_lowering_profile = throws_propagation_sites="
        << frontend_metadata_.throws_propagation_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_.throws_propagation_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .throws_propagation_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.throws_propagation_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.throws_propagation_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.throws_propagation_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .throws_propagation_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_.throws_propagation_lowering_contract_violation_sites
        << ", deterministic_throws_propagation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_throws_propagation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_ns_error_bridging_lowering_profile = ns_error_bridging_sites="
        << frontend_metadata_.ns_error_bridging_lowering_sites
        << ", ns_error_parameter_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_parameter_sites
        << ", ns_error_out_parameter_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_out_parameter_sites
        << ", ns_error_bridge_path_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_bridge_path_sites
        << ", failable_call_sites="
        << frontend_metadata_.ns_error_bridging_lowering_failable_call_sites
        << ", normalized_sites="
        << frontend_metadata_.ns_error_bridging_lowering_normalized_sites
        << ", bridge_boundary_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_bridge_boundary_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_contract_violation_sites
        << ", deterministic_ns_error_bridging_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_ns_error_bridging_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_unwind_cleanup_lowering_profile = unwind_cleanup_sites="
        << frontend_metadata_.unwind_cleanup_lowering_sites
        << ", unwind_edge_sites="
        << frontend_metadata_.unwind_cleanup_lowering_unwind_edge_sites
        << ", cleanup_scope_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_scope_sites
        << ", cleanup_emit_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_emit_sites
        << ", landing_pad_sites="
        << frontend_metadata_.unwind_cleanup_lowering_landing_pad_sites
        << ", cleanup_resume_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_resume_sites
        << ", normalized_sites="
        << frontend_metadata_.unwind_cleanup_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.unwind_cleanup_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.unwind_cleanup_lowering_contract_violation_sites
        << ", deterministic_unwind_cleanup_lowering_handoff="
        << (frontend_metadata_.deterministic_unwind_cleanup_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_error_diagnostics_recovery_lowering_profile = error_diagnostic_sites="
        << frontend_metadata_.error_diagnostics_recovery_lowering_sites
        << ", parser_diagnostic_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_parser_diagnostic_sites
        << ", semantic_diagnostic_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_semantic_diagnostic_sites
        << ", fixit_hint_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_fixit_hint_sites
        << ", recovery_candidate_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_recovery_candidate_sites
        << ", recovery_applied_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_recovery_applied_sites
        << ", normalized_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_contract_violation_sites
        << ", deterministic_error_diagnostics_recovery_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_error_diagnostics_recovery_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_async_continuation_lowering_profile = async_continuation_sites="
        << frontend_metadata_.async_continuation_lowering_sites
        << ", async_keyword_sites="
        << frontend_metadata_.async_continuation_lowering_async_keyword_sites
        << ", async_function_sites="
        << frontend_metadata_.async_continuation_lowering_async_function_sites
        << ", continuation_allocation_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_allocation_sites
        << ", continuation_resume_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_resume_sites
        << ", continuation_suspend_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_suspend_sites
        << ", async_state_machine_sites="
        << frontend_metadata_
               .async_continuation_lowering_async_state_machine_sites
        << ", normalized_sites="
        << frontend_metadata_.async_continuation_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_.async_continuation_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .async_continuation_lowering_contract_violation_sites
        << ", deterministic_async_continuation_lowering_handoff="
        << (frontend_metadata_.deterministic_async_continuation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_await_lowering_suspension_state_lowering_profile = await_suspension_sites="
        << frontend_metadata_.await_lowering_suspension_state_lowering_sites
        << ", await_keyword_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_keyword_sites
        << ", await_suspension_point_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_suspension_point_sites
        << ", await_resume_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_resume_sites
        << ", await_state_machine_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_state_machine_sites
        << ", await_continuation_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_continuation_sites
        << ", normalized_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_contract_violation_sites
        << ", deterministic_await_lowering_suspension_state_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_await_lowering_suspension_state_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_actor_isolation_sendability_lowering_profile = actor_isolation_sites="
        << frontend_metadata_.actor_isolation_sendability_lowering_sites
        << ", sendability_check_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_sendability_check_sites
        << ", cross_actor_hop_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_cross_actor_hop_sites
        << ", non_sendable_capture_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_non_sendable_capture_sites
        << ", sendable_transfer_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_sendable_transfer_sites
        << ", isolation_boundary_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_isolation_boundary_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_contract_violation_sites
        << ", deterministic_actor_isolation_sendability_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_actor_isolation_sendability_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_task_runtime_interop_cancellation_lowering_profile = task_runtime_sites="
        << frontend_metadata_.task_runtime_interop_cancellation_lowering_sites
        << ", task_runtime_interop_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_interop_sites
        << ", cancellation_probe_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_cancellation_probe_sites
        << ", cancellation_handler_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_cancellation_handler_sites
        << ", runtime_resume_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_resume_sites
        << ", runtime_cancel_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_cancel_sites
        << ", normalized_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_contract_violation_sites
        << ", deterministic_task_runtime_interop_cancellation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_task_runtime_interop_cancellation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_concurrency_replay_race_guard_lowering_profile = concurrency_replay_sites="
        << frontend_metadata_.concurrency_replay_race_guard_lowering_sites
        << ", replay_proof_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_replay_proof_sites
        << ", race_guard_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_race_guard_sites
        << ", task_handoff_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_task_handoff_sites
        << ", actor_isolation_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_actor_isolation_sites
        << ", deterministic_schedule_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_deterministic_schedule_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_contract_violation_sites
        << ", deterministic_concurrency_replay_race_guard_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_concurrency_replay_race_guard_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_unsafe_pointer_extension_lowering_profile = unsafe_pointer_extension_sites="
        << frontend_metadata_.unsafe_pointer_extension_lowering_sites
        << ", unsafe_keyword_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_unsafe_keyword_sites
        << ", pointer_arithmetic_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_pointer_arithmetic_sites
        << ", raw_pointer_type_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_raw_pointer_type_sites
        << ", unsafe_operation_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_unsafe_operation_sites
        << ", normalized_sites="
        << frontend_metadata_.unsafe_pointer_extension_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_contract_violation_sites
        << ", deterministic_unsafe_pointer_extension_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_unsafe_pointer_extension_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_inline_asm_intrinsic_governance_lowering_profile = inline_asm_intrinsic_sites="
        << frontend_metadata_.inline_asm_intrinsic_governance_lowering_sites
        << ", inline_asm_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_inline_asm_sites
        << ", intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_intrinsic_sites
        << ", governed_intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites
        << ", privileged_intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites
        << ", normalized_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_contract_violation_sites
        << ", deterministic_inline_asm_intrinsic_governance_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_inline_asm_intrinsic_governance_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_object_pointer_nullability_generics_profile = object_pointer_type_spellings="
        << frontend_metadata_.object_pointer_type_spellings
        << ", pointer_declarator_entries=" << frontend_metadata_.pointer_declarator_entries
        << ", pointer_declarator_depth_total=" << frontend_metadata_.pointer_declarator_depth_total
        << ", pointer_declarator_token_entries=" << frontend_metadata_.pointer_declarator_token_entries
        << ", nullability_suffix_entries=" << frontend_metadata_.nullability_suffix_entries
        << ", generic_suffix_entries=" << frontend_metadata_.generic_suffix_entries
        << ", terminated_generic_suffix_entries=" << frontend_metadata_.terminated_generic_suffix_entries
        << ", unterminated_generic_suffix_entries=" << frontend_metadata_.unterminated_generic_suffix_entries
        << ", deterministic_object_pointer_nullability_generics_handoff="
        << (frontend_metadata_.deterministic_object_pointer_nullability_generics_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_symbol_graph_scope_resolution_profile = global_symbol_nodes="
        << frontend_metadata_.global_symbol_nodes
        << ", function_symbol_nodes=" << frontend_metadata_.function_symbol_nodes
        << ", interface_symbol_nodes=" << frontend_metadata_.interface_symbol_nodes
        << ", implementation_symbol_nodes=" << frontend_metadata_.implementation_symbol_nodes
        << ", interface_property_symbol_nodes=" << frontend_metadata_.interface_property_symbol_nodes
        << ", implementation_property_symbol_nodes=" << frontend_metadata_.implementation_property_symbol_nodes
        << ", interface_method_symbol_nodes=" << frontend_metadata_.interface_method_symbol_nodes
        << ", implementation_method_symbol_nodes=" << frontend_metadata_.implementation_method_symbol_nodes
        << ", top_level_scope_symbols=" << frontend_metadata_.top_level_scope_symbols
        << ", nested_scope_symbols=" << frontend_metadata_.nested_scope_symbols
        << ", scope_frames_total=" << frontend_metadata_.scope_frames_total
        << ", implementation_interface_resolution_sites="
        << frontend_metadata_.implementation_interface_resolution_sites
        << ", implementation_interface_resolution_hits="
        << frontend_metadata_.implementation_interface_resolution_hits
        << ", implementation_interface_resolution_misses="
        << frontend_metadata_.implementation_interface_resolution_misses
        << ", method_resolution_sites=" << frontend_metadata_.method_resolution_sites
        << ", method_resolution_hits=" << frontend_metadata_.method_resolution_hits
        << ", method_resolution_misses=" << frontend_metadata_.method_resolution_misses
        << ", deterministic_symbol_graph_handoff="
        << (frontend_metadata_.deterministic_symbol_graph_handoff ? "true" : "false")
        << ", deterministic_scope_resolution_handoff="
        << (frontend_metadata_.deterministic_scope_resolution_handoff ? "true" : "false")
        << ", deterministic_symbol_graph_scope_resolution_handoff_key="
        << frontend_metadata_.deterministic_symbol_graph_scope_resolution_handoff_key << "\n";
    out << "source_filename = \"" << program_.module_name << ".objc3\"\n\n";
    EmitFrontendMetadata(out);
    // Historical extraction contract markers retained for fail-closed tooling:
    // out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";
    // for (std::size_t i = 0; i < lowering_ir_boundary_.runtime_dispatch_arg_slots; ++i) {
    //   out << ", i32";
    // }
    // out << ")\n\n";
    if (runtime_dispatch_call_emitted_) {
      out << "; runtime_dispatch_call_decl = "
          << Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_)
          << "\n\n";
    }
    out << body.str();
    ir = out.str();
    return true;
  }

 private:
  struct LoweredFunctionSignature {
    ValueType return_type = ValueType::I32;
    std::vector<ValueType> param_types;
  };

  struct FunctionEffectInfo {
    bool has_global_write = false;
    bool has_message_send = false;
    std::unordered_set<std::string> called_functions;
  };

  struct LoweredMessageSend {
    std::string receiver = "0";
    bool receiver_is_compile_time_zero = false;
    bool receiver_is_compile_time_nonzero = false;
    std::vector<std::string> args;
    std::string selector;
    std::string dispatch_surface_family;
    std::string dispatch_surface_entrypoint_family;
    std::string dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  };

  struct ControlLabels {
    std::string continue_label;
    std::string break_label;
    bool continue_allowed = false;
    std::size_t scope_depth = 0;
    std::size_t autoreleasepool_depth = 0;
    std::size_t pending_block_dispose_depth = 0;
    std::size_t arc_cleanup_depth = 0;
  };

  struct BlockBinding {
    std::string storage_ptr;
    const Expr *literal = nullptr;
    std::string promoted_handle_ptr;
  };

  struct TypedKeyPathArtifact {
    std::size_t ordinal = 0;
    bool root_is_self = false;
    std::string root_name;
    std::string component_path;
    std::string profile;
    std::string descriptor_symbol;
  };

  struct PendingBlockDisposeCall {
    std::string helper_symbol;
    std::string storage_ptr;
  };

  struct FunctionContext {
    std::vector<std::string> entry_lines;
    std::vector<std::string> code_lines;
    std::vector<std::unordered_map<std::string, std::string>> scopes;
    std::unordered_map<std::string, BlockBinding> block_bindings;
    std::vector<PendingBlockDisposeCall> pending_block_dispose_calls;
    std::vector<ControlLabels> control_stack;
    std::vector<std::string> autoreleasepool_scope_symbols;
    std::vector<std::vector<const BlockStmt *>> pending_defer_scope_blocks;
    std::vector<std::size_t> pending_block_dispose_scope_depths;
    std::vector<std::size_t> arc_cleanup_scope_depths;
    std::unordered_set<std::string> nil_bound_ptrs;
    std::unordered_set<std::string> nonzero_bound_ptrs;
    std::unordered_map<std::string, int> const_value_ptrs;
    std::unordered_map<std::string, int> immediate_identifiers;
    std::vector<std::string> arc_owned_cleanup_ptrs;
    std::unordered_set<std::string> arc_owned_cleanup_ptr_set;
    std::unordered_set<std::string> arc_owned_storage_ptrs;
    ValueType return_type = ValueType::I32;
    int temp_counter = 0;
    int label_counter = 0;
    bool terminated = false;
    bool global_proofs_invalidated = false;
    bool arc_return_insert_retain = false;
    bool arc_return_insert_autorelease = false;
  };

  static const char *LLVMScalarType(ValueType type) {
    if (type == ValueType::Bool) {
      return "i1";
    }
    if (type == ValueType::Void) {
      return "void";
    }
    return "i32";
  }

  static bool IsArcExecutableObjectParam(const FuncParam &param) {
    return param.id_spelling || param.instancetype_spelling ||
           param.object_pointer_type_spelling;
  }

  static bool IsArcExecutableObjectReturn(const FunctionDecl &fn) {
    return fn.return_id_spelling || fn.return_instancetype_spelling ||
           fn.return_object_pointer_type_spelling;
  }

  static bool IsArcExecutableObjectReturn(const Objc3MethodDecl &method) {
    return method.return_id_spelling || method.return_instancetype_spelling ||
           method.return_object_pointer_type_spelling;
  }

  static bool EffectiveArcParamInsertRetain(const FuncParam &param,
                                            bool arc_mode_enabled) {
    return param.ownership_insert_retain ||
           (arc_mode_enabled && !param.has_ownership_qualifier &&
            IsArcExecutableObjectParam(param));
  }

  static bool EffectiveArcParamInsertRelease(const FuncParam &param,
                                             bool arc_mode_enabled) {
    return param.ownership_insert_release ||
           (arc_mode_enabled && !param.has_ownership_qualifier &&
            IsArcExecutableObjectParam(param));
  }

  static bool EffectiveArcReturnInsertRetain(const FunctionDecl &fn,
                                             bool arc_mode_enabled) {
    return fn.return_ownership_insert_retain ||
           (arc_mode_enabled && !fn.has_return_ownership_qualifier &&
            IsArcExecutableObjectReturn(fn));
  }

  static bool EffectiveArcReturnInsertRetain(const Objc3MethodDecl &method,
                                             bool arc_mode_enabled) {
    return method.return_ownership_insert_retain ||
           (arc_mode_enabled && !method.has_return_ownership_qualifier &&
            IsArcExecutableObjectReturn(method));
  }

  static bool EffectiveArcReturnInsertAutorelease(const FunctionDecl &fn) {
    return fn.return_ownership_insert_autorelease;
  }

  static bool EffectiveArcReturnInsertAutorelease(
      const Objc3MethodDecl &method) {
    return method.return_ownership_insert_autorelease;
  }

  static std::map<std::string, LoweredFunctionSignature> BuildLoweredFunctionSignatures(const Objc3Program &program) {
    std::map<std::string, LoweredFunctionSignature> signatures;
    for (const auto &fn : program.functions) {
      LoweredFunctionSignature signature;
      signature.return_type = fn.return_type;
      signature.param_types.reserve(fn.params.size());
      for (const auto &param : fn.params) {
        signature.param_types.push_back(param.type);
      }
      auto existing = signatures.find(fn.name);
      if (existing == signatures.end()) {
        signatures.emplace(fn.name, std::move(signature));
      }
    }
    return signatures;
  }

  static std::size_t CountVectorSignatureFunctions(const Objc3Program &program) {
    std::unordered_set<std::string> vector_function_names;
    for (const auto &fn : program.functions) {
      bool has_vector_signature = fn.return_vector_spelling;
      if (!has_vector_signature) {
        for (const auto &param : fn.params) {
          if (param.vector_spelling) {
            has_vector_signature = true;
            break;
          }
        }
      }
      if (has_vector_signature) {
        vector_function_names.insert(fn.name);
      }
    }
    return vector_function_names.size();
  }

  static std::string EscapeCStringLiteral(const std::string &text) {
    std::ostringstream out;
    for (unsigned char c : text) {
      std::ostringstream byte;
      byte << std::hex << std::uppercase << static_cast<int>(c);
      std::string value = byte.str();
      if (value.size() < 2) {
        value = "0" + value;
      }
      if (c == '\\' || c == '"' || c < 32 || c > 126) {
        out << "\\" << value;
        continue;
      }
      out << static_cast<char>(c);
    }
    return out.str();
  }

  static std::string JoinStringParts(const std::vector<std::string> &parts,
                                     const std::string &delimiter) {
    std::ostringstream out;
    for (std::size_t i = 0; i < parts.size(); ++i) {
      if (i != 0) {
        out << delimiter;
      }
      out << parts[i];
    }
    return out.str();
  }

  static std::uint64_t StableRuntimeMetadataLinkerAnchorHash(
      const std::string &text) {
    std::uint64_t hash = 1469598103934665603ull;
    for (unsigned char c : text) {
      hash ^= static_cast<std::uint64_t>(c);
      hash *= 1099511628211ull;
    }
    return hash;
  }

  static std::string LowerHex64(std::uint64_t value) {
    std::ostringstream out;
    out << std::hex << std::nouppercase << value;
    return out.str();
  }

  std::string RuntimeMetadataLinkerAnchorSuffix() const {
    std::string seed = program_.module_name;
    seed += "|";
    seed += frontend_metadata_.runtime_metadata_class_metaclass_typed_handoff_replay_key;
    seed += "|";
    seed += frontend_metadata_.runtime_metadata_protocol_category_typed_handoff_replay_key;
    seed += "|";
    seed += frontend_metadata_.runtime_metadata_member_table_typed_handoff_replay_key;
    seed += "|";
    seed +=
        frontend_metadata_
            .runtime_metadata_archive_static_link_translation_unit_identity_key;
    return LowerHex64(StableRuntimeMetadataLinkerAnchorHash(seed));
  }

  std::string RuntimeMetadataLinkerAnchorSymbol() const {
    return "objc3_runtime_metadata_link_anchor_" +
           RuntimeMetadataLinkerAnchorSuffix();
  }

  std::string RuntimeMetadataDiscoveryRootSymbol() const {
    return "objc3_runtime_metadata_discovery_root_" +
           RuntimeMetadataLinkerAnchorSuffix();
  }

  static std::string MakeIdentifierSafeSuffix(const std::string &text) {
    std::string out;
    out.reserve(text.size());
    for (unsigned char ch : text) {
      if (std::isalnum(ch) != 0 || ch == '_') {
        out.push_back(static_cast<char>(ch));
      } else {
        out.push_back('_');
      }
    }
    if (out.empty()) {
      out = "module";
    }
    return out;
  }

  static std::string EncodeBoundaryTokenValueHex(const std::string &text) {
    static constexpr char kHex[] = "0123456789abcdef";
    std::string out;
    out.reserve(text.size() * 2u);
    for (unsigned char ch : text) {
      out.push_back(kHex[(ch >> 4) & 0x0f]);
      out.push_back(kHex[ch & 0x0f]);
    }
    return out;
  }

  bool ShouldEmitRuntimeBootstrapLowering() const {
    return ShouldEmitRuntimeMetadataSectionScaffold() &&
           frontend_metadata_.runtime_bootstrap_lowering_ready &&
           frontend_metadata_.runtime_bootstrap_lowering_fail_closed &&
           !frontend_metadata_.runtime_bootstrap_lowering_contract_id.empty() &&
           !frontend_metadata_.runtime_bootstrap_lowering_constructor_root_symbol
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_init_stub_symbol_prefix.empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_registration_table_symbol_prefix
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_registration_entrypoint_symbol
                .empty() &&
           !frontend_metadata_.runtime_bootstrap_lowering_global_ctor_list_model
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_registration_table_layout_model
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_lowering_image_local_initialization_model
                .empty() &&
           frontend_metadata_
                   .runtime_bootstrap_lowering_registration_table_abi_version >
               0 &&
           frontend_metadata_
                   .runtime_bootstrap_lowering_registration_table_pointer_field_count >
               0 &&
           frontend_metadata_
               .runtime_bootstrap_lowering_bootstrap_ir_materialization_landed &&
           frontend_metadata_
               .runtime_bootstrap_lowering_image_local_initialization_landed &&
           !frontend_metadata_
                .runtime_metadata_archive_static_link_translation_unit_identity_key
                .empty();
  }

  bool ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering() const {
    return ShouldEmitRuntimeBootstrapLowering() &&
           !frontend_metadata_
                .runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id
                .empty() &&
           !frontend_metadata_
                .runtime_bootstrap_registration_descriptor_identifier.empty() &&
           !frontend_metadata_.runtime_bootstrap_image_root_identifier.empty();
  }

  std::string RuntimeBootstrapSafeSuffix() const {
    return MakeIdentifierSafeSuffix(
        frontend_metadata_
            .runtime_metadata_archive_static_link_translation_unit_identity_key);
  }

  std::string RuntimeBootstrapModuleNameGlobalSymbol() const {
    return ".objc3_runtime_module_name_" + RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapTranslationUnitIdentityGlobalSymbol() const {
    return ".objc3_runtime_translation_unit_identity_" +
           RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapImageDescriptorSymbol() const {
    return "__objc3_runtime_image_descriptor_" + RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapRegistrationDescriptorIdentifierSafeSuffix() const {
    return MakeIdentifierSafeSuffix(
        frontend_metadata_.runtime_bootstrap_registration_descriptor_identifier);
  }

  std::string RuntimeBootstrapImageRootIdentifierSafeSuffix() const {
    return MakeIdentifierSafeSuffix(
        frontend_metadata_.runtime_bootstrap_image_root_identifier);
  }

  std::string RuntimeBootstrapRegistrationDescriptorNameGlobalSymbol() const {
    return ".objc3_runtime_registration_descriptor_name_" +
           RuntimeBootstrapRegistrationDescriptorIdentifierSafeSuffix();
  }

  std::string RuntimeBootstrapImageRootNameGlobalSymbol() const {
    return ".objc3_runtime_image_root_name_" +
           RuntimeBootstrapImageRootIdentifierSafeSuffix();
  }

  std::string RuntimeBootstrapRegistrationDescriptorSymbol() const {
    return std::string(kObjc3RuntimeBootstrapRegistrationDescriptorSymbolPrefix) +
           RuntimeBootstrapRegistrationDescriptorIdentifierSafeSuffix();
  }

  std::string RuntimeBootstrapImageRootSymbol() const {
    return std::string(kObjc3RuntimeBootstrapImageRootSymbolPrefix) +
           RuntimeBootstrapImageRootIdentifierSafeSuffix();
  }

  std::string RuntimeBootstrapInitStubSymbol() const {
    return frontend_metadata_.runtime_bootstrap_lowering_init_stub_symbol_prefix +
           RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapRegistrationTableSymbol() const {
    return frontend_metadata_
               .runtime_bootstrap_lowering_registration_table_symbol_prefix +
           RuntimeBootstrapSafeSuffix();
  }

  std::string RuntimeBootstrapImageLocalInitStateSymbol() const {
    return frontend_metadata_
               .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix +
           RuntimeBootstrapSafeSuffix();
  }

  static constexpr const char *RuntimeBootstrapImageDescriptorType() {
    return "{ ptr, ptr, i64, i64, i64, i64, i64, i64 }";
  }

  static constexpr const char *RuntimeBootstrapRegistrationTableType() {
    return "{ i64, i64, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr }";
  }

  void EmitFrontendMetadata(std::ostringstream &out) const {
    // M256-A001 executable source-closure freeze anchor: IR currently
    // publishes interface/protocol/category/linking metadata as the canonical
    // source-closure proof surface only. Later M256 realization issues must
    // preserve these identities while adding runnable class/category/protocol
    // behavior.
    // M256-A002 executable class/metaclass source-closure anchor: the IR
    // handoff now carries declaration-owned parent identities, method-owner
    // identities, and class/metaclass object identities so later realization
    // work consumes the same fail-closed source model.
    if (frontend_metadata_.executable_class_metaclass_source_closure_ready) {
      out << "; executable_class_metaclass_source_closure = contract="
          << frontend_metadata_
                 .executable_class_metaclass_source_closure_contract_id
          << ";parent_identity_model="
          << frontend_metadata_
                 .executable_class_metaclass_parent_identity_model
          << ";method_owner_identity_model="
          << frontend_metadata_
                 .executable_class_metaclass_method_owner_identity_model
          << ";class_object_identity_model="
          << frontend_metadata_
                 .executable_class_metaclass_object_identity_model
          << ";declaration_node_count="
          << frontend_metadata_
                 .executable_class_metaclass_declaration_node_count
          << ";parent_identity_edge_count="
          << frontend_metadata_
                 .executable_class_metaclass_parent_identity_edge_count
          << ";method_owner_identity_edge_count="
          << frontend_metadata_
                 .executable_class_metaclass_method_owner_identity_edge_count
          << ";class_object_identity_edge_count="
          << frontend_metadata_
                 .executable_class_metaclass_object_identity_edge_count
          << "\n";
    }
    // M256-A003 executable protocol/category source-closure anchor: the IR
    // handoff now carries protocol inheritance, category attachment, and
    // adopted-protocol conformance identities so later object-model semantic
    // issues consume the same fail-closed source model.
    // M256-B001 object-model semantic-rule freeze anchor: IR stays proof-only
    // for the frozen semantic boundary covering realization legality,
    // inheritance legality, override compatibility, protocol conformance, and
    // deterministic category merge behavior; executable enforcement begins in
    // later M256 lane-B issues.
    // M256-B002 protocol-conformance implementation anchor: IR remains a
    // proof-only consumer of the sema-owned protocol conformance result while
    // publishing the same protocol/category source identities after sema
    // starts enforcing required-vs-optional protocol member coverage with
    // fail-closed diagnostics.
    // M256-B003 category-merge implementation anchor: IR remains downstream of
    // the sema-owned realized-class category merge/conflict decision and must
    // not reinterpret attachment legality or concrete message resolution.
    // M256-B004 inheritance/override legality anchor: IR remains downstream of
    // the sema-owned realized-class inheritance and override legality result
    // and must not reinterpret superclass cycles, missing realization closure,
    // or inherited member compatibility.
    if (frontend_metadata_.executable_protocol_category_source_closure_ready) {
      out << "; executable_protocol_category_source_closure = contract="
          << frontend_metadata_
                 .executable_protocol_category_source_closure_contract_id
          << ";protocol_inheritance_model="
          << frontend_metadata_
                 .executable_protocol_inheritance_identity_model
          << ";category_attachment_model="
          << frontend_metadata_
                 .executable_category_attachment_identity_model
          << ";protocol_category_conformance_model="
          << frontend_metadata_
                 .executable_protocol_category_conformance_identity_model
          << ";protocol_node_count="
          << frontend_metadata_
                 .executable_protocol_category_protocol_node_count
          << ";category_node_count="
          << frontend_metadata_
                 .executable_protocol_category_category_node_count
          << ";protocol_inheritance_edge_count="
          << frontend_metadata_
                 .executable_protocol_inheritance_identity_edge_count
          << ";category_attachment_edge_count="
          << frontend_metadata_
                 .executable_category_attachment_identity_edge_count
          << ";protocol_category_conformance_edge_count="
          << frontend_metadata_
                 .executable_protocol_category_conformance_identity_edge_count
          << "\n";
    }
    out << "!objc3.frontend = !{!0}\n";
    out << "!objc3.objc_interface_implementation = !{!1}\n";
    out << "!objc3.objc_protocol_category = !{!2}\n";
    out << "!objc3.objc_class_protocol_category_linking = !{!7}\n";
    out << "!objc3.objc_selector_normalization = !{!3}\n";
    out << "!objc3.objc_property_attribute = !{!4}\n";
    out << "!objc3.objc_runtime_metadata_source_ownership = !{!45}\n";
    out << "!objc3.objc_runtime_export_legality = !{!46}\n";
    out << "!objc3.objc_runtime_export_enforcement = !{!47}\n";
    out << "!objc3.objc_runtime_metadata_section_abi = !{!48}\n";
    out << "!objc3.objc_runtime_metadata_section_scaffold = !{!49}\n";
    out << "!objc3.objc_runtime_metadata_layout_policy = !{!55}\n";
    out << "!objc3.objc_runtime_class_metaclass_emission = !{!56}\n";
    out << "!objc3.objc_runtime_protocol_category_emission = !{!57}\n";
    out << "!objc3.objc_runtime_member_table_emission = !{!58}\n";
    out << "!objc3.objc_runtime_selector_string_pool_emission = !{!59}\n";
    out << "!objc3.objc_runtime_binary_inspection_harness = !{!60}\n";
    out << "!objc3.objc_runtime_object_packaging_retention = !{!61}\n";
    out << "!objc3.objc_runtime_linker_retention = !{!62}\n";
    out << "!objc3.objc_runtime_archive_static_link_discovery = !{!63}\n";
    out << "!objc3.objc_runtime_metadata_emission_gate = !{!64}\n";
    out << "!objc3.objc_runtime_metadata_object_emission_closeout = !{!65}\n";
    out << "!objc3.objc_runtime_metadata_object_inspection = !{!50}\n";
    out << "!objc3.objc_runtime_support_library = !{!51}\n";
    out << "!objc3.objc_runtime_support_library_core_feature = !{!52}\n";
    out << "!objc3.objc_runtime_support_library_link_wiring = !{!53}\n";
    out << "!objc3.objc_executable_metadata_debug_projection = !{!54}\n";
    out << "!objc3.objc_object_pointer_nullability_generics = !{!5}\n";
    out << "!objc3.objc_symbol_graph_scope_resolution = !{!6}\n";
    out << "!objc3.objc_id_class_sel_object_pointer_typecheck = !{!8}\n";
    out << "!objc3.objc_dispatch_surface_classification = !{!66}\n";
    out << "!objc3.objc_executable_ivar_layout_emission = !{!67}\n";
    out << "!objc3.objc_executable_synthesized_accessor_property_lowering = !{!68}\n";
    out << "!objc3.objc_runtime_ownership_hook_emission = !{!69}\n";
    out << "!objc3.objc_runtime_memory_management_api = !{!70}\n";
    out << "!objc3.objc_runtime_memory_management_implementation = !{!71}\n";
    out << "!objc3.objc_ownership_runtime_gate = !{!72}\n";
    out << "!objc3.objc_runnable_block_runtime_gate = !{!73}\n";
    out << "!objc3.objc_runnable_block_execution_matrix = !{!74}\n";
    out << "!objc3.objc_arc_source_mode_boundary = !{!75}\n";
    out << "!objc3.objc_arc_mode_handling = !{!76}\n";
    out << "!objc3.objc_arc_semantic_rules = !{!77}\n";
    out << "!objc3.objc_arc_inference_lifetime = !{!78}\n";
    out << "!objc3.objc_arc_interaction_semantics = !{!79}\n";
    out << "!objc3.objc_arc_automatic_insertions = !{!80}\n";
    out << "!objc3.objc_arc_cleanup_weak_lifetime_hooks = !{!81}\n";
    out << "!objc3.objc_arc_block_autorelease_return_lowering = !{!82}\n";
    out << "!objc3.objc_runtime_arc_helper_api_surface = !{!83}\n";
    out << "!objc3.objc_runtime_arc_helper_runtime_support = !{!84}\n";
    out << "!objc3.objc_runtime_arc_debug_instrumentation = !{!85}\n";
    out << "!objc3.objc_runnable_arc_runtime_gate = !{!86}\n";
    out << "!objc3.objc_message_send_selector_lowering = !{!9}\n";
    out << "!objc3.objc_dispatch_abi_marshalling = !{!10}\n";
    out << "!objc3.objc_nil_receiver_semantics_foldability = !{!11}\n";
    out << "!objc3.objc_super_dispatch_method_family = !{!12}\n";
    out << "!objc3.objc_runtime_shim_host_link = !{!13}\n";
    out << "!objc3.objc_ownership_qualifier_lowering = !{!14}\n";
    out << "!objc3.objc_retain_release_operation_lowering = !{!15}\n";
    out << "!objc3.objc_autoreleasepool_scope_lowering = !{!16}\n";
    out << "!objc3.objc_weak_unowned_semantics_lowering = !{!17}\n";
    out << "!objc3.objc_arc_diagnostics_fixit_lowering = !{!18}\n";
    out << "!objc3.objc_block_literal_capture_lowering = !{!19}\n";
    out << "!objc3.objc_block_abi_invoke_trampoline_lowering = !{!20}\n";
    out << "!objc3.objc_block_storage_escape_lowering = !{!21}\n";
    out << "!objc3.objc_block_copy_dispose_lowering = !{!22}\n";
    out << "!objc3.objc_block_determinism_perf_baseline_lowering = !{!23}\n";
    out << "!objc3.objc_lightweight_generic_constraint_lowering = !{!24}\n";
    out << "!objc3.objc_nullability_flow_warning_precision_lowering = !{!25}\n";
    out << "!objc3.objc_protocol_qualified_object_type_lowering = !{!26}\n";
    out << "!objc3.objc_variance_bridge_cast_lowering = !{!27}\n";
    out << "!objc3.objc_generic_metadata_abi_lowering = !{!28}\n";
    out << "!objc3.objc_module_import_graph_lowering = !{!29}\n";
    out << "!objc3.objc_namespace_collision_shadowing_lowering = !{!30}\n";
    out << "!objc3.objc_public_private_api_partition_lowering = !{!31}\n";
    out << "!objc3.objc_incremental_module_cache_invalidation_lowering = !{!32}\n";
    out << "!objc3.objc_cross_module_conformance_lowering = !{!33}\n";
    out << "!objc3.objc_throws_propagation_lowering = !{!34}\n";
    out << "!objc3.objc_unwind_cleanup_lowering = !{!35}\n";
    out << "!objc3.objc_ns_error_bridging_lowering = !{!36}\n";
    out << "!objc3.objc_unsafe_pointer_extension_lowering = !{!37}\n";
    out << "!objc3.objc_inline_asm_intrinsic_governance_lowering = !{!38}\n";
    out << "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}\n";
    out << "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}\n";
    out << "!objc3.objc_actor_isolation_sendability_lowering = !{!41}\n";
    out << "!objc3.objc_await_lowering_suspension_state_lowering = !{!42}\n";
    out << "!objc3.objc_async_continuation_lowering = !{!43}\n";
    out << "!objc3.objc_error_diagnostics_recovery_lowering = !{!44}\n";
    out << "!0 = !{i32 " << static_cast<unsigned>(frontend_metadata_.language_version) << ", !\""
        << EscapeCStringLiteral(frontend_metadata_.compatibility_mode) << "\", i1 "
        << (frontend_metadata_.migration_assist ? 1 : 0) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.migration_legacy_yes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.migration_legacy_no) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.migration_legacy_null) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.migration_legacy_total()) << "}\n";
    out << "!1 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.declared_interfaces) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.declared_implementations) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_interface_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_implementation_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.interface_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.implementation_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.linked_implementation_symbols) << ", i1 "
        << (frontend_metadata_.deterministic_interface_implementation_handoff ? 1 : 0) << "}\n";
    out << "!2 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.declared_protocols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.declared_categories) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_protocol_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_category_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.protocol_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.category_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.linked_category_symbols) << ", i1 "
        << (frontend_metadata_.deterministic_protocol_category_handoff ? 1 : 0) << "}\n";
    out << "!3 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.selector_method_declaration_entries)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.selector_normalized_method_declarations)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.selector_piece_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.selector_piece_parameter_links) << ", i1 "
        << (frontend_metadata_.deterministic_selector_normalization_handoff ? 1 : 0) << "}\n";
    out << "!4 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.property_declaration_entries)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.property_attribute_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.property_attribute_value_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.property_accessor_modifier_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.property_getter_selector_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.property_setter_selector_entries) << ", i1 "
        << (frontend_metadata_.deterministic_property_attribute_handoff ? 1 : 0) << "}\n\n";
    out << "!45 = !{!\"" << EscapeCStringLiteral(frontend_metadata_.runtime_metadata_source_ownership_contract_id)
        << "\", !\"" << EscapeCStringLiteral(frontend_metadata_.runtime_metadata_source_schema) << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.runtime_metadata_ivar_source_model) << "\", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_class_record_count) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_protocol_record_count) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_category_interface_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_category_implementation_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_property_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_method_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_metadata_ivar_record_count)
        << ", i1 " << (frontend_metadata_.frontend_owns_runtime_metadata_source_records ? 1 : 0) << ", i1 "
        << (frontend_metadata_.runtime_metadata_source_records_ready_for_lowering ? 1 : 0) << ", i1 "
        << (frontend_metadata_.native_runtime_library_present ? 1 : 0) << ", i1 "
        << (frontend_metadata_.runtime_metadata_source_boundary_fail_closed ? 1 : 0) << ", i1 "
        << (frontend_metadata_.runtime_shim_test_only ? 1 : 0) << ", i1 "
        << (frontend_metadata_.deterministic_runtime_metadata_source_schema ? 1 : 0) << "}\n";
    out << "!46 = !{!\"" << EscapeCStringLiteral(frontend_metadata_.runtime_export_legality_contract_id)
        << "\", i1 " << (frontend_metadata_.runtime_export_semantic_boundary_frozen ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_metadata_export_enforcement_ready ? 1 : 0)
        << ", i1 " << (frontend_metadata_.runtime_export_fail_closed ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_duplicate_runtime_identity_enforcement_pending ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_incomplete_declaration_export_blocking_pending ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_illegal_redeclaration_mix_export_blocking_pending ? 1 : 0)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_class_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_protocol_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_category_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_method_record_count)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_export_ivar_record_count)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_invalid_protocol_composition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_attribute_invalid_entries)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_attribute_contract_violations)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_invalid_type_annotation_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_ivar_binding_missing)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_property_ivar_binding_conflicts)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_implementation_resolution_misses)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_method_resolution_misses)
        << ", i1 " << (frontend_metadata_.runtime_export_boundary_ready ? 1 : 0) << "}\n";
    out << "!47 = !{!\"" << EscapeCStringLiteral(frontend_metadata_.runtime_export_enforcement_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_export_metadata_completeness_enforced ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_duplicate_runtime_identity_suppression_enforced ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_illegal_redeclaration_mix_blocking_enforced ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_export_metadata_shape_drift_blocking_enforced ? 1 : 0)
        << ", i1 " << (frontend_metadata_.runtime_export_enforcement_fail_closed ? 1 : 0)
        << ", i1 " << (frontend_metadata_.runtime_export_ready_for_runtime_export ? 1 : 0)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_duplicate_runtime_identity_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_incomplete_declaration_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_illegal_redeclaration_mix_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_export_metadata_shape_drift_sites)
        << "}\n";
    out << "!48 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_abi_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_section_boundary_frozen ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_fail_closed ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_metadata_section_object_file_inventory_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_symbol_policy_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_visibility_model_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_retention_policy_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_ready_for_scaffold ? 1
                                                                           : 0)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_image_info_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_class_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_protocol_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_category_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_property_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_logical_ivar_descriptor_section)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_descriptor_symbol_prefix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_aggregate_symbol_prefix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_image_info_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_descriptor_linkage)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_aggregate_linkage)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_visibility)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_retention_root)
        << "\"}\n";
    out << "!49 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_section_scaffold_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_abi_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_section_scaffold_emitted ? 1
                                                                         : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_scaffold_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_section_scaffold_uses_llvm_used
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_metadata_section_scaffold_image_info_emitted
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_class_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_protocol_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_category_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_property_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_ivar_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_total_descriptor_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_total_retained_global_count)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_image_info_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_class_aggregate_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_protocol_aggregate_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_category_aggregate_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_property_aggregate_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_section_scaffold_ivar_aggregate_symbol)
        << "\"}\n";
    Objc3RuntimeMetadataLayoutPolicy runtime_metadata_layout_policy;
    std::string runtime_metadata_layout_policy_error;
    if (!TryBuildRuntimeMetadataLayoutPolicy(runtime_metadata_layout_policy,
                                             runtime_metadata_layout_policy_error) &&
        runtime_metadata_layout_policy.failure_reason.empty()) {
      runtime_metadata_layout_policy.failure_reason =
          runtime_metadata_layout_policy_error;
    }
    out << "!50 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_object_inspection_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_scaffold_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_object_inspection_matrix_published
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_object_inspection_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_object_inspection_uses_llvm_readobj
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_object_inspection_uses_llvm_objdump
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .runtime_metadata_object_inspection_matrix_row_count)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_object_inspection_fixture_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_object_inspection_emit_prefix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_object_relative_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_section_inventory_row_key)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_section_inventory_command)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_symbol_inventory_row_key)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_object_inspection_symbol_inventory_command)
        << "\"}\n";
    out << "!51 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_metadata_scaffold_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_support_library_boundary_frozen ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_fail_closed ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_target_name_frozen ? 1
                                                                          : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_exported_entrypoints_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_ownership_boundaries_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_build_constraints_frozen
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_shim_remains_test_only
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_native_library_present
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_driver_link_wiring_pending
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_ready_for_skeleton ? 1
                                                                          : 0)
        << ", !\""
        << EscapeCStringLiteral(frontend_metadata_.runtime_support_library_target_name)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_public_header_path)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.runtime_support_library_source_root)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.runtime_support_library_library_kind)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_archive_basename)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_register_image_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_lookup_selector_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_dispatch_i32_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_reset_for_testing_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_driver_link_mode)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_compiler_ownership_boundary)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_runtime_ownership_boundary)
        << "\"}\n";
    out << "!52 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_core_feature_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_support_library_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_metadata_scaffold_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_support_library_core_feature_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_core_feature_sources_present
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_support_library_core_feature_header_present
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_archive_build_enabled
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_entrypoints_implemented
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_selector_lookup_stateful
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_dispatch_formula_matches_test_shim
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_reset_for_testing_supported
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_shim_remains_test_only
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_driver_link_wiring_pending
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_core_feature_ready_for_driver_link_wiring
                ? 1
                : 0)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_core_feature_target_name)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_public_header_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_core_feature_source_root)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_implementation_source_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_support_library_core_feature_library_kind)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_archive_basename)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_archive_relative_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_probe_source_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_register_image_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_lookup_selector_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_dispatch_i32_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_reset_for_testing_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_core_feature_driver_link_mode)
        << "\"}\n";
    out << "!53 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_core_feature_contract_id)
        << "\", i1 "
        << (frontend_metadata_.runtime_support_library_link_wiring_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_archive_available
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_compatibility_dispatch_alias_exported
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_driver_emits_runtime_link_contract
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_shim_remains_test_only
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_support_library_link_wiring_ready_for_runtime_library_consumption
                ? 1
                : 0)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_archive_relative_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_compatibility_dispatch_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_runtime_dispatch_symbol)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_execution_smoke_script_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_support_library_link_wiring_driver_link_mode)
        << "\"}\n";
    out << "!54 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_typed_handoff_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_source_graph_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_named_metadata_name)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_manifest_surface_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_typed_handoff_surface_path)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_source_graph_surface_path)
        << "\", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_matrix_published
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_.executable_metadata_debug_projection_fail_closed
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_manifest_debug_surface_published
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_ir_named_metadata_published
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_replay_anchor_deterministic
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .executable_metadata_debug_projection_active_typed_handoff_ready
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .executable_metadata_debug_projection_matrix_row_count)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_metadata_debug_projection_replay_key)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_active_typed_handoff_replay_key)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_row0_descriptor)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_row1_descriptor)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .executable_metadata_debug_projection_row2_descriptor)
        << "\"}\n";
    out << "!55 = !{!\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.contract_id)
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.abi_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.scaffold_contract_id)
        << "\", i1 "
        << (runtime_metadata_layout_policy.ready ? 1 : 0) << ", i1 "
        << (runtime_metadata_layout_policy.fail_closed ? 1 : 0) << ", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.family_ordering_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.descriptor_ordering_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.aggregate_relocation_policy)
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.comdat_policy)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.visibility_spelling_policy)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.retention_ordering_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.object_format_policy_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.object_format_surface_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.object_format)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.section_spelling_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.retention_anchor_model)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.descriptor_linkage)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.aggregate_linkage)
        << "\", !\""
        << EscapeCStringLiteral(
               runtime_metadata_layout_policy.metadata_visibility)
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.retention_root)
        << "\", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_layout_policy.total_retained_global_count)
        << ", !\""
        << EscapeCStringLiteral(Objc3RuntimeMetadataLayoutPolicyReplayKey(
               runtime_metadata_layout_policy))
        << "\", !\""
        << EscapeCStringLiteral(runtime_metadata_layout_policy.failure_reason)
        << "\"}\n";
    std::size_t runtime_metadata_class_bundle_count = 0;
    std::size_t runtime_metadata_instance_method_reference_total = 0;
    std::size_t runtime_metadata_class_method_reference_total = 0;
    for (const auto &bundle :
         frontend_metadata_
             .runtime_metadata_class_metaclass_bundles_lexicographic) {
      ++runtime_metadata_class_bundle_count;
      runtime_metadata_instance_method_reference_total +=
          bundle.instance_method_count;
      runtime_metadata_class_method_reference_total += bundle.class_method_count;
    }
    out << "!56 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_emission_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_class_metaclass_name_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_super_link_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_method_list_reference_model)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_class_metaclass_emission_ready
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_metadata_class_metaclass_emission_fail_closed
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_class_bundle_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_instance_method_reference_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_class_method_reference_total)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_class_metaclass_typed_handoff_replay_key)
        << "\"}\n";
    std::size_t runtime_metadata_protocol_bundle_count = 0;
    std::size_t runtime_metadata_protocol_inherited_reference_total = 0;
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic) {
      ++runtime_metadata_protocol_bundle_count;
      runtime_metadata_protocol_inherited_reference_total +=
          bundle.inherited_protocol_owner_identities_lexicographic.size();
    }
    std::size_t runtime_metadata_category_bundle_count = 0;
    std::size_t runtime_metadata_category_adopted_reference_total = 0;
    std::size_t runtime_metadata_category_attachment_reference_total = 0;
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_category_bundles_lexicographic) {
      ++runtime_metadata_category_bundle_count;
      runtime_metadata_category_adopted_reference_total +=
          bundle.adopted_protocol_owner_identities_lexicographic.size();
      runtime_metadata_category_attachment_reference_total += 3u;
    }
    out << "!57 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_protocol_category_emission_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_protocol_emission_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_category_emission_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_protocol_reference_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_category_attachment_model)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_protocol_category_emission_ready
                ? 1
                : 0)
        << ", i1 "
        << (frontend_metadata_
                    .runtime_metadata_protocol_category_emission_fail_closed
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_protocol_bundle_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_protocol_inherited_reference_total)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_category_bundle_count)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_category_adopted_reference_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               runtime_metadata_category_attachment_reference_total)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_protocol_category_typed_handoff_replay_key)
        << "\"}\n";
    std::size_t runtime_metadata_method_list_bundle_count = 0;
    std::size_t runtime_metadata_method_entry_total = 0;
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic) {
      ++runtime_metadata_method_list_bundle_count;
      runtime_metadata_method_entry_total += bundle.entries_lexicographic.size();
    }
    out << "!58 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_member_table_emission_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_method_list_emission_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.runtime_metadata_method_list_grouping_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_property_descriptor_emission_payload_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_ivar_descriptor_emission_payload_model)
        << "\", i1 "
        << (frontend_metadata_.runtime_metadata_member_table_emission_ready ? 1
                                                                            : 0)
        << ", i1 "
        << (frontend_metadata_.runtime_metadata_member_table_emission_fail_closed
                ? 1
                : 0)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_method_list_bundle_count)
        << ", i64 "
        << static_cast<unsigned long long>(runtime_metadata_method_entry_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.runtime_metadata_property_bundles_lexicographic
                   .size())
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                   .size())
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_member_table_typed_handoff_replay_key)
        << "\"}\n";
    out << "!59 = !{!\""
        << EscapeCStringLiteral(
               kObjc3RuntimeSelectorStringPoolEmissionContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeSelectorPoolEmissionPayloadModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeStringPoolEmissionPayloadModel)
        << "\", i64 "
        << static_cast<unsigned long long>(selector_pool_globals_.size())
        << ", i64 "
        << static_cast<unsigned long long>(runtime_string_pool_globals_.size())
        << ", !\""
        << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeSelectorPoolLogicalSection))
        << "\", !\""
        << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
               kObjc3RuntimeStringPoolLogicalSection))
        << "\"}\n";
    out << "!60 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionHarnessContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionPositiveCorpusModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionNegativeCorpusModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSectionCommand)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSymbolCommand)
        << "\", i64 4, i64 1}\n";
    out << "!61 = !{!\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionBoundaryModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionAnchorModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionArtifact)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeObjectPackagingRetentionSymbolPrefix)
        << "\"}\n";
    out << "!62 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionAnchorModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerAnchorLogicalSection)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryRootLogicalSection)
        << "\", !\""
        << EscapeCStringLiteral(RuntimeMetadataLinkerAnchorSymbol())
        << "\", !\""
        << EscapeCStringLiteral(RuntimeMetadataDiscoveryRootSymbol())
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerResponseArtifactSuffix)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryArtifactSuffix)
        << "\"}\n";
    out << "!63 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_discovery_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_anchor_seed_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_translation_unit_identity_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_merge_model)
        << "\", i1 "
        << (frontend_metadata_
                    .runtime_metadata_archive_static_link_discovery_ready
                ? 1
                : 0)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_response_artifact_suffix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_discovery_artifact_suffix)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .runtime_metadata_archive_static_link_translation_unit_identity_key)
        << "\"}\n";
    out << "!64 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateFailureModel)
        << "\"}\n";
    out << "!65 = !{!\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMetadataObjectEmissionCloseoutContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel)
        << "\"}\n";
    out << "!66 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_instance_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_class_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_super_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_direct_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_surface_classification_dynamic_sites)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_instance_entrypoint_family)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_class_entrypoint_family)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_super_entrypoint_family)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_direct_entrypoint_family)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_
                   .dispatch_surface_classification_dynamic_entrypoint_family)
        << "\", i1 "
        << (frontend_metadata_.deterministic_dispatch_surface_classification_handoff
                ? 1
                : 0)
        << "}\n";
    out << "!67 = !{!\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_layout_emission_contract_id)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_layout_descriptor_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_offset_global_model)
        << "\", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_layout_table_model)
        << "\", i1 "
        << (frontend_metadata_.executable_ivar_layout_emission_ready ? 1 : 0)
        << ", i1 "
        << (frontend_metadata_.executable_ivar_layout_emission_fail_closed ? 1
                                                                           : 0)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.executable_ivar_offset_global_entries)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.executable_ivar_layout_table_entries)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.executable_ivar_layout_owner_entries)
        << ", !\""
        << EscapeCStringLiteral(
               frontend_metadata_.executable_ivar_layout_emission_replay_key)
        << "\"}\n";
    out << "!68 = !{!\""
        << EscapeCStringLiteral(
               kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel)
        << "\", i64 "
        << static_cast<unsigned long long>(synthesized_property_accessor_count_)
        << ", i64 "
        << static_cast<unsigned long long>(synthesized_property_storages_.size())
        << "}\n";
    out << "!69 = !{!\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAccessorModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipRuntimeHookEmissionPropertyContextModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3OwnershipRuntimeHookEmissionFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", i64 "
        << static_cast<unsigned long long>(synthesized_property_accessor_count_)
        << ", i64 "
        << static_cast<unsigned long long>(synthesized_property_storages_.size())
        << "}\n";
    out << "!70 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiReferenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiWeakModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiFailClosedModel)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", i64 "
        << static_cast<unsigned long long>(synthesized_property_accessor_count_)
        << ", i64 "
        << static_cast<unsigned long long>(synthesized_property_storages_.size())
        << "}\n";
    out << "!71 = !{!\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationRefcountModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationWeakModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationFailClosedModel)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", i64 "
        << static_cast<unsigned long long>(synthesized_property_accessor_count_)
        << ", i64 "
        << static_cast<unsigned long long>(synthesized_property_storages_.size())
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.autoreleasepool_scope_lowering_scope_sites)
        << "}\n";
    out << "!72 = !{!\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateSupportedModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               kObjc3RuntimeMemoryManagementImplementationContractId)
        << "\"}\n";
    out << "!73 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateActiveModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId)
        << "\"}\n";
    out << "!74 = !{!\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixContractId)
        << "\", !\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixActiveModel)
        << "\", !\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(
               Expr::kObjc3RunnableBlockExecutionMatrixFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
        << "\"}\n";
    out << "!75 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundarySourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryModeModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryFailClosedModel)
        << "\"}\n";
    out << "!76 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingModeModel)
        << "\", !\""
        << EscapeCStringLiteral(frontend_metadata_.arc_mode)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingNonGoalModel)
        << "\"}\n";
    out << "!77 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSemanticModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesNonGoalModel)
        << "\"}\n";
    out << "!78 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSemanticModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeNonGoalModel)
        << "\"}\n";
    out << "!79 = !{!\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSemanticModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsFailClosedModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsNonGoalModel)
        << "\"}\n";
    out << "!80 = !{!\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionLoweringModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionFailureModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionNonGoalModel)
        << "\"}\n";
    out << "!81 = !{!\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksLoweringModel)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksFailureModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel)
        << "\"}\n";
    out << "!82 = !{!\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
        << "\", !\""
        << EscapeCStringLiteral(Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePromoteBlockI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeInvokeBlockI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel)
        << "\"}\n";
    out << "!83 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceReferenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceWeakModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceFailClosedModel)
        << "\"}\n";
    out << "!84 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportDependencyModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportWeakModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportExecutionModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel)
        << "\"}\n";
    out << "!85 = !{!\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationDependencyModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationCoverageModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationValidationModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationFailClosedModel)
        << "\"}\n";
    out << "!86 = !{!\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateEvidenceModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateActiveModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateNonGoalModel)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcModeHandlingContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcInteractionSemanticsContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
        << "\", !\""
        << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateFailClosedModel)
        << "\"}\n";
    out << "!5 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.object_pointer_type_spellings)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.pointer_declarator_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.pointer_declarator_depth_total) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.pointer_declarator_token_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.nullability_suffix_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.generic_suffix_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.terminated_generic_suffix_entries) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.unterminated_generic_suffix_entries) << ", i1 "
        << (frontend_metadata_.deterministic_object_pointer_nullability_generics_handoff ? 1 : 0) << "}\n";
    out << "!6 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.global_symbol_nodes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.function_symbol_nodes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.interface_symbol_nodes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.implementation_symbol_nodes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.interface_property_symbol_nodes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.implementation_property_symbol_nodes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.interface_method_symbol_nodes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.implementation_method_symbol_nodes) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.top_level_scope_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.nested_scope_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.scope_frames_total) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.implementation_interface_resolution_sites) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.implementation_interface_resolution_hits) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.implementation_interface_resolution_misses) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.method_resolution_sites) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.method_resolution_hits) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.method_resolution_misses) << ", i1 "
        << (frontend_metadata_.deterministic_symbol_graph_handoff ? 1 : 0) << ", i1 "
        << (frontend_metadata_.deterministic_scope_resolution_handoff ? 1 : 0) << ", !\""
        << EscapeCStringLiteral(frontend_metadata_.deterministic_symbol_graph_scope_resolution_handoff_key)
        << "\"}\n";
    out << "!7 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.declared_class_interfaces) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.declared_class_implementations) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_class_interfaces) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.resolved_class_implementations) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.linked_class_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.linked_category_method_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.protocol_composition_sites) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.protocol_composition_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.category_composition_sites) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.category_composition_symbols) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.invalid_protocol_composition_sites) << ", i1 "
        << (frontend_metadata_.deterministic_class_protocol_category_linking_handoff ? 1 : 0) << "}\n";
    out << "!8 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.id_typecheck_sites) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.class_typecheck_sites) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.sel_typecheck_sites) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.object_pointer_typecheck_sites) << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.id_class_sel_object_pointer_typecheck_sites_total)
        << ", i1 "
        << (frontend_metadata_.deterministic_id_class_sel_object_pointer_typecheck_handoff ? 1 : 0) << "}\n";
    out << "!9 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.message_send_selector_lowering_sites)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.message_send_selector_lowering_unary_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.message_send_selector_lowering_keyword_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.message_send_selector_lowering_selector_piece_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.message_send_selector_lowering_argument_expression_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.message_send_selector_lowering_receiver_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.message_send_selector_lowering_selector_literal_entries)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.message_send_selector_lowering_selector_literal_characters)
        << ", i1 " << (frontend_metadata_.deterministic_message_send_selector_lowering_handoff ? 1 : 0) << "}\n";
    out << "!10 = !{i64 " << static_cast<unsigned long long>(frontend_metadata_.dispatch_abi_marshalling_message_send_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.dispatch_abi_marshalling_receiver_slots_marshaled)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.dispatch_abi_marshalling_selector_slots_marshaled)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_abi_marshalling_argument_value_slots_marshaled)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_abi_marshalling_argument_padding_slots_marshaled)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.dispatch_abi_marshalling_argument_total_slots_marshaled)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.dispatch_abi_marshalling_total_marshaled_slots)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.dispatch_abi_marshalling_runtime_dispatch_arg_slots)
        << ", i1 " << (frontend_metadata_.deterministic_dispatch_abi_marshalling_handoff ? 1 : 0) << "}\n";
    out << "!11 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nil_receiver_semantics_foldability_message_send_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nil_receiver_semantics_foldability_receiver_nil_literal_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.nil_receiver_semantics_foldability_enabled_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.nil_receiver_semantics_foldability_foldable_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nil_receiver_semantics_foldability_runtime_dispatch_required_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.nil_receiver_semantics_foldability_non_nil_receiver_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nil_receiver_semantics_foldability_contract_violation_sites)
        << ", i1 " << (frontend_metadata_.deterministic_nil_receiver_semantics_foldability_handoff ? 1 : 0)
        << "}\n\n";
    out << "!12 = !{i64 "
        << static_cast<unsigned long long>(frontend_metadata_.super_dispatch_method_family_message_send_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.super_dispatch_method_family_receiver_super_identifier_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.super_dispatch_method_family_enabled_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.super_dispatch_method_family_requires_class_context_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.super_dispatch_method_family_init_sites)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.super_dispatch_method_family_copy_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.super_dispatch_method_family_mutable_copy_sites)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.super_dispatch_method_family_new_sites)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.super_dispatch_method_family_none_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.super_dispatch_method_family_returns_retained_result_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.super_dispatch_method_family_returns_related_result_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.super_dispatch_method_family_contract_violation_sites)
        << ", i1 " << (frontend_metadata_.deterministic_super_dispatch_method_family_handoff ? 1 : 0)
        << "}\n\n";
    out << "!13 = !{i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_shim_host_link_message_send_sites)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_shim_host_link_required_sites)
        << ", i64 " << static_cast<unsigned long long>(frontend_metadata_.runtime_shim_host_link_elided_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_shim_host_link_runtime_dispatch_arg_slots)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count)
        << ", !\"" << EscapeCStringLiteral(frontend_metadata_.runtime_shim_host_link_runtime_dispatch_symbol)
        << "\", i1 "
        << (frontend_metadata_.runtime_shim_host_link_default_runtime_dispatch_symbol_binding ? 1 : 0)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.runtime_shim_host_link_contract_violation_sites)
        << ", i1 " << (frontend_metadata_.deterministic_runtime_shim_host_link_handoff ? 1 : 0)
        << "}\n\n";
    out << "!14 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ownership_qualifier_lowering_ownership_qualifier_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ownership_qualifier_lowering_invalid_ownership_qualifier_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ownership_qualifier_lowering_object_pointer_type_annotation_sites)
        << ", i1 " << (frontend_metadata_.deterministic_ownership_qualifier_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!15 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.retain_release_operation_lowering_ownership_qualified_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.retain_release_operation_lowering_retain_insertion_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.retain_release_operation_lowering_release_insertion_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.retain_release_operation_lowering_autorelease_insertion_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.retain_release_operation_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_retain_release_operation_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!16 = !{i64 "
        << static_cast<unsigned long long>(frontend_metadata_.autoreleasepool_scope_lowering_scope_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.autoreleasepool_scope_lowering_scope_symbolized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.autoreleasepool_scope_lowering_max_scope_depth)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.autoreleasepool_scope_lowering_scope_entry_transition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.autoreleasepool_scope_lowering_scope_exit_transition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.autoreleasepool_scope_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_autoreleasepool_scope_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!17 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.weak_unowned_semantics_lowering_ownership_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.weak_unowned_semantics_lowering_weak_reference_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.weak_unowned_semantics_lowering_unowned_reference_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.weak_unowned_semantics_lowering_unowned_safe_reference_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.weak_unowned_semantics_lowering_conflict_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.weak_unowned_semantics_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_weak_unowned_semantics_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!18 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.arc_diagnostics_fixit_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_arc_diagnostics_fixit_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!19 = !{i64 "
        << static_cast<unsigned long long>(frontend_metadata_.block_literal_capture_lowering_block_literal_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.block_literal_capture_lowering_block_parameter_entries)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.block_literal_capture_lowering_block_capture_entries)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_literal_capture_lowering_block_body_statement_entries)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.block_literal_capture_lowering_block_empty_capture_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_literal_capture_lowering_block_nondeterministic_capture_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_literal_capture_lowering_block_non_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(frontend_metadata_.block_literal_capture_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_block_literal_capture_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!20 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_block_literal_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_word_count_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_parameter_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_body_statement_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_missing_invoke_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_abi_invoke_trampoline_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_block_abi_invoke_trampoline_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!21 = !{i64 "
        << static_cast<unsigned long long>(frontend_metadata_.block_storage_escape_lowering_block_literal_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_mutable_capture_count_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_byref_slot_count_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_parameter_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_capture_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_body_statement_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_requires_byref_cells_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_escape_analysis_enabled_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_escape_profile_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_byref_layout_symbolized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_storage_escape_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_block_storage_escape_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!22 = !{i64 "
        << static_cast<unsigned long long>(frontend_metadata_.block_copy_dispose_lowering_block_literal_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_mutable_capture_count_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_byref_slot_count_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_parameter_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_capture_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_body_statement_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_profile_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_copy_helper_symbolized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_dispose_helper_symbolized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_copy_dispose_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_block_copy_dispose_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!23 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_block_literal_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_baseline_weight_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_parameter_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_capture_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_body_statement_entries_total)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_deterministic_capture_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_heavy_tier_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_normalized_profile_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.block_determinism_perf_baseline_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_block_determinism_perf_baseline_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!24 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.lightweight_generic_constraint_lowering_generic_constraint_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.lightweight_generic_constraint_lowering_generic_suffix_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.lightweight_generic_constraint_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.lightweight_generic_constraint_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.lightweight_generic_constraint_lowering_normalized_constraint_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.lightweight_generic_constraint_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_lightweight_generic_constraint_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!25 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nullability_flow_warning_precision_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nullability_flow_warning_precision_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nullability_flow_warning_precision_lowering_nullability_suffix_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nullability_flow_warning_precision_lowering_nullable_suffix_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nullability_flow_warning_precision_lowering_nonnull_suffix_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nullability_flow_warning_precision_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.nullability_flow_warning_precision_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_nullability_flow_warning_precision_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!26 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.protocol_qualified_object_type_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.protocol_qualified_object_type_lowering_protocol_composition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.protocol_qualified_object_type_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.protocol_qualified_object_type_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.protocol_qualified_object_type_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_protocol_qualified_object_type_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!27 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.variance_bridge_cast_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.variance_bridge_cast_lowering_protocol_composition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.variance_bridge_cast_lowering_ownership_qualifier_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.variance_bridge_cast_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.variance_bridge_cast_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.variance_bridge_cast_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.variance_bridge_cast_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_variance_bridge_cast_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!28 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.generic_metadata_abi_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.generic_metadata_abi_lowering_generic_suffix_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.generic_metadata_abi_lowering_protocol_composition_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.generic_metadata_abi_lowering_ownership_qualifier_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.generic_metadata_abi_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.generic_metadata_abi_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.generic_metadata_abi_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.generic_metadata_abi_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_generic_metadata_abi_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!29 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.module_import_graph_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.module_import_graph_lowering_import_edge_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.module_import_graph_lowering_namespace_segment_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.module_import_graph_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.module_import_graph_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.module_import_graph_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.module_import_graph_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_module_import_graph_lowering_handoff ? 1 : 0)
        << "}\n\n";
    out << "!30 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.namespace_collision_shadowing_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .namespace_collision_shadowing_lowering_namespace_segment_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .namespace_collision_shadowing_lowering_import_edge_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .namespace_collision_shadowing_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .namespace_collision_shadowing_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .namespace_collision_shadowing_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .namespace_collision_shadowing_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_namespace_collision_shadowing_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!31 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.public_private_api_partition_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .public_private_api_partition_lowering_namespace_segment_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .public_private_api_partition_lowering_import_edge_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .public_private_api_partition_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .public_private_api_partition_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .public_private_api_partition_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .public_private_api_partition_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_public_private_api_partition_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!32 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .incremental_module_cache_invalidation_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .incremental_module_cache_invalidation_lowering_namespace_segment_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .incremental_module_cache_invalidation_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .incremental_module_cache_invalidation_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .incremental_module_cache_invalidation_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .incremental_module_cache_invalidation_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_incremental_module_cache_invalidation_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!33 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.cross_module_conformance_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .cross_module_conformance_lowering_namespace_segment_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .cross_module_conformance_lowering_import_edge_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .cross_module_conformance_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .cross_module_conformance_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.cross_module_conformance_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .cross_module_conformance_lowering_cache_invalidation_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .cross_module_conformance_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_cross_module_conformance_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!34 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.throws_propagation_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .throws_propagation_lowering_namespace_segment_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .throws_propagation_lowering_import_edge_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .throws_propagation_lowering_object_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .throws_propagation_lowering_pointer_declarator_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.throws_propagation_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .throws_propagation_lowering_cache_invalidation_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .throws_propagation_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_throws_propagation_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!35 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_unwind_edge_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_cleanup_scope_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_cleanup_emit_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_landing_pad_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_cleanup_resume_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_guard_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unwind_cleanup_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_unwind_cleanup_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!36 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ns_error_bridging_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ns_error_bridging_lowering_ns_error_parameter_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ns_error_bridging_lowering_ns_error_out_parameter_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ns_error_bridging_lowering_ns_error_bridge_path_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ns_error_bridging_lowering_failable_call_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.ns_error_bridging_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ns_error_bridging_lowering_bridge_boundary_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .ns_error_bridging_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_ns_error_bridging_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!37 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.unsafe_pointer_extension_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .unsafe_pointer_extension_lowering_unsafe_keyword_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .unsafe_pointer_extension_lowering_pointer_arithmetic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .unsafe_pointer_extension_lowering_raw_pointer_type_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .unsafe_pointer_extension_lowering_unsafe_operation_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .unsafe_pointer_extension_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .unsafe_pointer_extension_lowering_gate_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .unsafe_pointer_extension_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_unsafe_pointer_extension_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!38 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.inline_asm_intrinsic_governance_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .inline_asm_intrinsic_governance_lowering_inline_asm_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .inline_asm_intrinsic_governance_lowering_intrinsic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .inline_asm_intrinsic_governance_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .inline_asm_intrinsic_governance_lowering_gate_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .inline_asm_intrinsic_governance_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_inline_asm_intrinsic_governance_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!39 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.concurrency_replay_race_guard_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .concurrency_replay_race_guard_lowering_replay_proof_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .concurrency_replay_race_guard_lowering_race_guard_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .concurrency_replay_race_guard_lowering_task_handoff_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .concurrency_replay_race_guard_lowering_actor_isolation_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .concurrency_replay_race_guard_lowering_deterministic_schedule_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .concurrency_replay_race_guard_lowering_guard_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .concurrency_replay_race_guard_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_concurrency_replay_race_guard_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!40 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.task_runtime_interop_cancellation_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .task_runtime_interop_cancellation_lowering_runtime_interop_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .task_runtime_interop_cancellation_lowering_cancellation_probe_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .task_runtime_interop_cancellation_lowering_cancellation_handler_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .task_runtime_interop_cancellation_lowering_runtime_resume_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .task_runtime_interop_cancellation_lowering_runtime_cancel_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .task_runtime_interop_cancellation_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .task_runtime_interop_cancellation_lowering_guard_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .task_runtime_interop_cancellation_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_task_runtime_interop_cancellation_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!41 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.actor_isolation_sendability_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .actor_isolation_sendability_lowering_sendability_check_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .actor_isolation_sendability_lowering_cross_actor_hop_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .actor_isolation_sendability_lowering_non_sendable_capture_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .actor_isolation_sendability_lowering_sendable_transfer_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .actor_isolation_sendability_lowering_isolation_boundary_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .actor_isolation_sendability_lowering_guard_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .actor_isolation_sendability_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_actor_isolation_sendability_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!42 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.await_lowering_suspension_state_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_keyword_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_suspension_point_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_resume_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_state_machine_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_await_continuation_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_gate_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .await_lowering_suspension_state_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_await_lowering_suspension_state_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!43 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_async_keyword_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_async_function_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_continuation_allocation_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_continuation_resume_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_continuation_suspend_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_async_state_machine_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.async_continuation_lowering_gate_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .async_continuation_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_.deterministic_async_continuation_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
    out << "!44 = !{i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_.error_diagnostics_recovery_lowering_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_parser_diagnostic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_semantic_diagnostic_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_fixit_hint_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_recovery_candidate_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_recovery_applied_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_normalized_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_guard_blocked_sites)
        << ", i64 "
        << static_cast<unsigned long long>(
               frontend_metadata_
                   .error_diagnostics_recovery_lowering_contract_violation_sites)
        << ", i1 "
        << (frontend_metadata_
                    .deterministic_error_diagnostics_recovery_lowering_handoff
                ? 1
                : 0)
        << "}\n\n";
  }

  void RegisterSelectorLiteral(const std::string &selector) {
    if (selector.empty() ||
        selector_pool_globals_.find(selector) != selector_pool_globals_.end()) {
      return;
    }
    selector_pool_globals_.emplace(selector, "");
  }

  void RegisterRuntimeStringLiteral(const std::string &value) {
    if (value.empty() ||
        runtime_string_pool_globals_.find(value) !=
            runtime_string_pool_globals_.end()) {
      return;
    }
    runtime_string_pool_globals_.emplace(value, "");
  }

  void AssignCanonicalPoolGlobalNames() {
    std::size_t index = 0;
    for (auto &entry : selector_pool_globals_) {
      entry.second = "@__objc3_sel_pool_" +
                     FormatRuntimeMetadataDescriptorOrdinal(index++);
    }
    index = 0;
    for (auto &entry : runtime_string_pool_globals_) {
      entry.second = "@__objc3_str_pool_" +
                     FormatRuntimeMetadataDescriptorOrdinal(index++);
    }
  }

  void RegisterTypedKeyPathLiteral(const Expr &expr) {
    if (!expr.typed_keypath_literal_enabled ||
        !expr.typed_keypath_literal_is_normalized ||
        expr.typed_keypath_components.empty()) {
      return;
    }
    const std::string profile =
        expr.typed_keypath_literal_profile.empty()
            ? std::string("typed-keypath:root=") + expr.typed_keypath_root_name
            : expr.typed_keypath_literal_profile;
    if (typed_keypath_artifacts_.find(profile) != typed_keypath_artifacts_.end()) {
      return;
    }
    TypedKeyPathArtifact artifact;
    artifact.root_is_self = expr.typed_keypath_root_is_self;
    artifact.root_name = expr.typed_keypath_root_name;
    artifact.component_path = JoinStringParts(expr.typed_keypath_components, ".");
    artifact.profile = profile;
    typed_keypath_artifacts_.emplace(profile, std::move(artifact));
    RegisterRuntimeStringLiteral(expr.typed_keypath_root_name);
    RegisterRuntimeStringLiteral(JoinStringParts(expr.typed_keypath_components, "."));
    RegisterRuntimeStringLiteral(profile);
    if (!frontend_metadata_.lowering_generic_metadata_abi_replay_key.empty()) {
      RegisterRuntimeStringLiteral(
          frontend_metadata_.lowering_generic_metadata_abi_replay_key);
    }
  }

  void AssignTypedKeyPathArtifactOrdinals() {
    std::size_t ordinal = 0;
    for (auto &entry : typed_keypath_artifacts_) {
      entry.second.ordinal = ordinal;
      entry.second.descriptor_symbol =
          "@__objc3_keypath_desc_" + FormatRuntimeMetadataDescriptorOrdinal(ordinal);
      ++ordinal;
    }
  }

  void CollectSelectorExpr(const Expr *expr) {
    if (expr == nullptr) {
      return;
    }
    if (expr->typed_keypath_literal_enabled) {
      RegisterTypedKeyPathLiteral(*expr);
    }
    switch (expr->kind) {
      case Expr::Kind::MessageSend:
        RegisterSelectorLiteral(expr->selector);
        CollectSelectorExpr(expr->receiver.get());
        for (const auto &arg : expr->args) {
          CollectSelectorExpr(arg.get());
        }
        return;
      case Expr::Kind::Binary:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        return;
      case Expr::Kind::Conditional:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        CollectSelectorExpr(expr->third.get());
        return;
      case Expr::Kind::Call:
        for (const auto &arg : expr->args) {
          CollectSelectorExpr(arg.get());
        }
        return;
      default:
        return;
    }
  }

  void CollectSelectorStmt(const Stmt *stmt) {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt != nullptr) {
          CollectSelectorExpr(stmt->let_stmt->value.get());
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt != nullptr) {
          CollectSelectorExpr(stmt->assign_stmt->value.get());
        }
        return;
      case Stmt::Kind::Return:
        if (stmt->return_stmt != nullptr) {
          CollectSelectorExpr(stmt->return_stmt->value.get());
        }
        return;
      case Stmt::Kind::Expr:
        if (stmt->expr_stmt != nullptr) {
          CollectSelectorExpr(stmt->expr_stmt->value.get());
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->if_stmt->condition.get());
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectSelectorStmt(then_stmt.get());
        }
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectSelectorStmt(else_stmt.get());
        }
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        CollectSelectorExpr(stmt->do_while_stmt->condition.get());
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->for_stmt->init.value.get());
        CollectSelectorExpr(stmt->for_stmt->condition.get());
        CollectSelectorExpr(stmt->for_stmt->step.value.get());
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->switch_stmt->condition.get());
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectSelectorStmt(case_body_stmt.get());
          }
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->while_stmt->condition.get());
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectSelectorStmt(nested_stmt.get());
        }
        return;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectRuntimeMetadataPoolLiterals() {
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_class_metaclass_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.class_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.protocol_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_category_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.class_name);
      RegisterRuntimeStringLiteral(bundle.category_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.record_kind);
      RegisterRuntimeStringLiteral(bundle.category_owner_identity);
      RegisterRuntimeStringLiteral(bundle.class_owner_identity);
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_method_list_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      for (const auto &entry : bundle.entries_lexicographic) {
        RegisterSelectorLiteral(entry.selector);
        RegisterRuntimeStringLiteral(entry.owner_identity);
        RegisterRuntimeStringLiteral(entry.return_type_name);
      }
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_property_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.property_name);
      RegisterRuntimeStringLiteral(bundle.type_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      if (bundle.has_getter) {
        RegisterSelectorLiteral(bundle.getter_selector);
      }
      if (bundle.has_setter) {
        RegisterSelectorLiteral(bundle.setter_selector);
      }
      if (!bundle.ivar_binding_symbol.empty()) {
        RegisterRuntimeStringLiteral(bundle.ivar_binding_symbol);
      }
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      RegisterRuntimeStringLiteral(bundle.property_owner_identity);
      RegisterRuntimeStringLiteral(bundle.property_name);
      RegisterRuntimeStringLiteral(bundle.ivar_binding_symbol);
    }
  }

  void CollectCanonicalPoolLiterals() {
    for (const auto &global : program_.globals) {
      CollectSelectorExpr(global.value.get());
    }
    for (const auto &fn : program_.functions) {
      for (const auto &stmt : fn.body) {
        CollectSelectorStmt(stmt.get());
      }
    }
    CollectRuntimeMetadataPoolLiterals();
    AssignCanonicalPoolGlobalNames();
    AssignTypedKeyPathArtifactOrdinals();
  }

  static bool IsNameBoundInScopes(const std::vector<std::unordered_set<std::string>> &scopes,
                                  const std::string &name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      if (it->find(name) != it->end()) {
        return true;
      }
    }
    return false;
  }

  void NotePotentialGlobalMutation(const std::string &name,
                                   const std::vector<std::unordered_set<std::string>> &scopes) {
    if (name.empty() || IsNameBoundInScopes(scopes, name)) {
      return;
    }
    if (globals_.find(name) != globals_.end()) {
      mutable_global_symbols_.insert(name);
    }
  }

  void CollectMutableGlobalSymbolsForClause(const ForClause &clause, std::vector<std::unordered_set<std::string>> &scopes) {
    switch (clause.kind) {
      case ForClause::Kind::None:
      case ForClause::Kind::Expr:
        return;
      case ForClause::Kind::Let:
        if (!scopes.empty() && !clause.name.empty()) {
          scopes.back().insert(clause.name);
        }
        return;
      case ForClause::Kind::Assign:
        NotePotentialGlobalMutation(clause.name, scopes);
        return;
    }
  }

  void CollectMutableGlobalSymbolsStmt(const Stmt *stmt, std::vector<std::unordered_set<std::string>> &scopes) {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt != nullptr && !stmt->let_stmt->name.empty() && !scopes.empty()) {
          scopes.back().insert(stmt->let_stmt->name);
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt != nullptr) {
          NotePotentialGlobalMutation(stmt->assign_stmt->name, scopes);
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectMutableGlobalSymbolsStmt(then_stmt.get(), scopes);
        }
        scopes.pop_back();
        scopes.push_back({});
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectMutableGlobalSymbolsStmt(else_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        CollectMutableGlobalSymbolsForClause(stmt->for_stmt->init, scopes);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        CollectMutableGlobalSymbolsForClause(stmt->for_stmt->step, scopes);
        scopes.pop_back();
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          scopes.push_back({});
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectMutableGlobalSymbolsStmt(case_body_stmt.get(), scopes);
          }
          scopes.pop_back();
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectMutableGlobalSymbolsStmt(nested_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Return:
      case Stmt::Kind::Expr:
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectMutableGlobalSymbols() {
    mutable_global_symbols_.clear();
    for (const FunctionDecl *fn : function_definitions_) {
      if (fn == nullptr) {
        continue;
      }
      std::vector<std::unordered_set<std::string>> scopes;
      scopes.push_back({});
      for (const auto &param : fn->params) {
        scopes.back().insert(param.name);
      }
      for (const auto &stmt : fn->body) {
        CollectMutableGlobalSymbolsStmt(stmt.get(), scopes);
      }
    }
  }

  bool IsGlobalSymbolWriteTarget(const std::string &name,
                                 const std::vector<std::unordered_set<std::string>> &scopes) const {
    if (name.empty() || IsNameBoundInScopes(scopes, name)) {
      return false;
    }
    return globals_.find(name) != globals_.end();
  }

  void CollectFunctionEffectExpr(const Expr *expr, std::vector<std::unordered_set<std::string>> &scopes,
                                 FunctionEffectInfo &info) const {
    if (expr == nullptr) {
      return;
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
      case Expr::Kind::BoolLiteral:
      case Expr::Kind::NilLiteral:
      case Expr::Kind::Identifier:
      case Expr::Kind::BlockLiteral:
        return;
      case Expr::Kind::Binary:
        CollectFunctionEffectExpr(expr->left.get(), scopes, info);
        CollectFunctionEffectExpr(expr->right.get(), scopes, info);
        return;
      case Expr::Kind::Conditional:
        CollectFunctionEffectExpr(expr->left.get(), scopes, info);
        CollectFunctionEffectExpr(expr->right.get(), scopes, info);
        CollectFunctionEffectExpr(expr->third.get(), scopes, info);
        return;
      case Expr::Kind::Call:
        info.called_functions.insert(expr->ident);
        for (const auto &arg : expr->args) {
          CollectFunctionEffectExpr(arg.get(), scopes, info);
        }
        return;
      case Expr::Kind::MessageSend:
        info.has_message_send = true;
        CollectFunctionEffectExpr(expr->receiver.get(), scopes, info);
        for (const auto &arg : expr->args) {
          CollectFunctionEffectExpr(arg.get(), scopes, info);
        }
        return;
    }
  }

  void CollectFunctionEffectForClause(const ForClause &clause, std::vector<std::unordered_set<std::string>> &scopes,
                                      FunctionEffectInfo &info) const {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        return;
      case ForClause::Kind::Let:
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        if (!scopes.empty() && !clause.name.empty()) {
          scopes.back().insert(clause.name);
        }
        return;
      case ForClause::Kind::Assign:
        if (IsGlobalSymbolWriteTarget(clause.name, scopes)) {
          info.has_global_write = true;
        }
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        return;
    }
  }

  void CollectFunctionEffectStmt(const Stmt *stmt, std::vector<std::unordered_set<std::string>> &scopes,
                                 FunctionEffectInfo &info) const {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->let_stmt->value.get(), scopes, info);
        if (!scopes.empty() && !stmt->let_stmt->name.empty()) {
          scopes.back().insert(stmt->let_stmt->name);
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt == nullptr) {
          return;
        }
        if (IsGlobalSymbolWriteTarget(stmt->assign_stmt->name, scopes)) {
          info.has_global_write = true;
        }
        CollectFunctionEffectExpr(stmt->assign_stmt->value.get(), scopes, info);
        return;
      case Stmt::Kind::Return:
        if (stmt->return_stmt != nullptr) {
          CollectFunctionEffectExpr(stmt->return_stmt->value.get(), scopes, info);
        }
        return;
      case Stmt::Kind::Expr:
        if (stmt->expr_stmt != nullptr) {
          CollectFunctionEffectExpr(stmt->expr_stmt->value.get(), scopes, info);
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->if_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectFunctionEffectStmt(then_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        scopes.push_back({});
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectFunctionEffectStmt(else_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        CollectFunctionEffectExpr(stmt->do_while_stmt->condition.get(), scopes, info);
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        CollectFunctionEffectForClause(stmt->for_stmt->init, scopes, info);
        CollectFunctionEffectExpr(stmt->for_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        CollectFunctionEffectForClause(stmt->for_stmt->step, scopes, info);
        scopes.pop_back();
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->switch_stmt->condition.get(), scopes, info);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          scopes.push_back({});
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectFunctionEffectStmt(case_body_stmt.get(), scopes, info);
          }
          scopes.pop_back();
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->while_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectFunctionEffectStmt(nested_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectFunctionEffects() {
    function_effects_.clear();
    impure_functions_.clear();

    for (const FunctionDecl *fn : function_definitions_) {
      if (fn == nullptr) {
        continue;
      }
      FunctionEffectInfo info;
      std::vector<std::unordered_set<std::string>> scopes;
      scopes.push_back({});
      for (const auto &param : fn->params) {
        scopes.back().insert(param.name);
      }
      for (const auto &stmt : fn->body) {
        CollectFunctionEffectStmt(stmt.get(), scopes, info);
      }
      function_effects_[fn->name] = std::move(info);
    }

    for (const auto &entry : function_effects_) {
      const FunctionEffectInfo &info = entry.second;
      if (info.has_global_write || info.has_message_send) {
        impure_functions_.insert(entry.first);
      }
    }

    bool changed = true;
    while (changed) {
      changed = false;
      for (const auto &entry : function_effects_) {
        const std::string &name = entry.first;
        if (impure_functions_.find(name) != impure_functions_.end()) {
          continue;
        }
        for (const std::string &callee : entry.second.called_functions) {
          const bool callee_defined = defined_functions_.find(callee) != defined_functions_.end();
          const bool callee_declared_pure = declared_pure_functions_.find(callee) != declared_pure_functions_.end();
          if ((!callee_defined && !callee_declared_pure) ||
              impure_functions_.find(callee) != impure_functions_.end()) {
            impure_functions_.insert(name);
            changed = true;
            break;
          }
        }
      }
    }
  }

  bool FunctionMayHaveGlobalSideEffects(const std::string &name) const {
    if (name.empty()) {
      return true;
    }
    if (defined_functions_.find(name) == defined_functions_.end()) {
      return declared_pure_functions_.find(name) == declared_pure_functions_.end();
    }
    return impure_functions_.find(name) != impure_functions_.end();
  }

  bool ShouldEmitRuntimeMetadataSectionScaffold() const {
    // M253-A001 emitted metadata inventory freeze anchor: the currently
    // supported emitted inventory is image-info plus class/protocol/category/
    // property/ivar descriptor sections retained via llvm.used. Separate
    // method/selector/string-pool section families remain explicit non-goals
    // until later M253 issues extend the inventory model.
    return frontend_metadata_.runtime_metadata_section_ready_for_scaffold &&
           frontend_metadata_.runtime_export_ready_for_runtime_export &&
           frontend_metadata_.runtime_metadata_section_scaffold_emitted &&
           frontend_metadata_.runtime_metadata_section_scaffold_fail_closed &&
           frontend_metadata_.runtime_metadata_section_scaffold_uses_llvm_used &&
           frontend_metadata_.runtime_metadata_section_scaffold_image_info_emitted;
  }

  static std::string FormatRuntimeMetadataDescriptorOrdinal(std::size_t ordinal) {
    std::ostringstream formatted;
    formatted << std::setw(4) << std::setfill('0') << ordinal;
    return formatted.str();
  }

  std::string BuildRuntimeMetadataDescriptorSymbol(
      const std::string &descriptor_symbol_prefix, const std::string &kind,
      std::size_t ordinal) const {
    return "@" + descriptor_symbol_prefix + kind + "_" +
           FormatRuntimeMetadataDescriptorOrdinal(ordinal);
  }

  std::string BuildRuntimeMetadataAuxiliarySymbol(
      const std::string &descriptor_symbol_prefix, const std::string &kind,
      const std::string &suffix, std::size_t ordinal) const {
    return "@" + descriptor_symbol_prefix + kind + "_" + suffix + "_" +
           FormatRuntimeMetadataDescriptorOrdinal(ordinal);
  }

  bool TryBuildRuntimeMetadataLayoutPolicy(
      Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) const {
    Objc3RuntimeMetadataLayoutPolicyInput input;
    input.abi_contract_id =
        frontend_metadata_.runtime_metadata_section_abi_contract_id;
    input.scaffold_contract_id =
        frontend_metadata_.runtime_metadata_section_scaffold_contract_id;
    input.section_boundary_ready =
        frontend_metadata_.runtime_metadata_section_ready_for_scaffold;
    input.runtime_export_ready =
        frontend_metadata_.runtime_export_ready_for_runtime_export;
    input.scaffold_emitted =
        frontend_metadata_.runtime_metadata_section_scaffold_emitted;
    input.scaffold_fail_closed =
        frontend_metadata_.runtime_metadata_section_scaffold_fail_closed;
    input.uses_llvm_used =
        frontend_metadata_.runtime_metadata_section_scaffold_uses_llvm_used;
    input.image_info_emitted =
        frontend_metadata_.runtime_metadata_section_scaffold_image_info_emitted;
    input.image_info_symbol =
        frontend_metadata_.runtime_metadata_section_scaffold_image_info_symbol;
    input.image_info_section =
        frontend_metadata_.runtime_metadata_section_logical_image_info_section;
    input.descriptor_symbol_prefix =
        frontend_metadata_.runtime_metadata_section_descriptor_symbol_prefix;
    input.descriptor_linkage =
        frontend_metadata_.runtime_metadata_section_descriptor_linkage;
    input.aggregate_linkage =
        frontend_metadata_.runtime_metadata_section_aggregate_linkage;
    input.metadata_visibility =
        frontend_metadata_.runtime_metadata_section_visibility;
    input.retention_root =
        frontend_metadata_.runtime_metadata_section_retention_root;
    input.total_retained_global_count =
        frontend_metadata_
            .runtime_metadata_section_scaffold_total_retained_global_count;
    input.families = {{
        {kObjc3RuntimeMetadataLayoutPolicyClassFamily,
         frontend_metadata_
             .runtime_metadata_section_logical_class_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_scaffold_class_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_scaffold_class_descriptor_count},
        {kObjc3RuntimeMetadataLayoutPolicyProtocolFamily,
         frontend_metadata_
             .runtime_metadata_section_logical_protocol_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_scaffold_protocol_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_scaffold_protocol_descriptor_count},
        {kObjc3RuntimeMetadataLayoutPolicyCategoryFamily,
         frontend_metadata_
             .runtime_metadata_section_logical_category_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_scaffold_category_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_scaffold_category_descriptor_count},
        {kObjc3RuntimeMetadataLayoutPolicyPropertyFamily,
         frontend_metadata_
             .runtime_metadata_section_logical_property_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_scaffold_property_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_scaffold_property_descriptor_count},
        {kObjc3RuntimeMetadataLayoutPolicyIvarFamily,
         frontend_metadata_.runtime_metadata_section_logical_ivar_descriptor_section,
         frontend_metadata_
             .runtime_metadata_section_scaffold_ivar_aggregate_symbol,
         frontend_metadata_
             .runtime_metadata_section_scaffold_ivar_descriptor_count},
    }};
    return ::TryBuildObjc3RuntimeMetadataLayoutPolicy(input, policy, error);
  }

  void EmitRuntimeMetadataSectionScaffold(std::ostringstream &out) const {
    if (!ShouldEmitRuntimeMetadataSectionScaffold()) {
      return;
    }

    Objc3RuntimeMetadataLayoutPolicy layout_policy;
    std::string layout_policy_error;
    if (!TryBuildRuntimeMetadataLayoutPolicy(layout_policy, layout_policy_error) ||
        !IsReadyObjc3RuntimeMetadataLayoutPolicy(layout_policy)) {
      return;
    }

    // M253-A002 source-to-section matrix anchor: emitted runtime metadata still
    // materializes only image-info plus class/protocol/category/property/ivar
    // descriptor sections. Interface/implementation/metaclass/method nodes
    // remain explicit no-standalone-emission matrix rows until later M253
    // payload work lands.
    // M253-B001 layout/visibility policy anchor: image-info emits first, then
    // class/protocol/category/property/ivar families. Within each family,
    // descriptor ordinals ascend before the family aggregate. Metadata globals
    // stay local-linkage only (private descriptors, internal image-info and
    // aggregates), use no COMDAT, omit explicit hidden visibility spelling,
    // and remain retained through @llvm.used in emission order.
    // M253-B002 normalized layout policy anchor: semantic finalization now
    // happens before emission. This function consumes one normalized lowering
    // policy packet and materializes exactly that plan, rather than
    // re-hardcoding family order or relocation behavior locally.
    // M253-B003 object-format policy expansion anchor: the lowering packet now
    // also decides the emitted section spellings for the host object format.
    // The emitter may not reinterpret that COFF/ELF/Mach-O mapping locally.
    out << "; runtime_metadata_layout_policy = "
        << Objc3RuntimeMetadataLayoutPolicyReplayKey(layout_policy) << "\n";
    out << "; runtime_metadata_section_emission_boundary = "
        << Objc3RuntimeMetadataSectionEmissionBoundarySummary() << "\n";
    // M263-C001 freezes this emitted boundary against the live ctor-root and
    // llvm.global_ctors path rather than the earlier deferred placeholder
    // model, so later multi-image lowering must preserve these names/shapes.
    out << "; runtime_bootstrap_lowering_boundary = "
        << Objc3RuntimeBootstrapLoweringBoundarySummary() << "\n";
    if (frontend_metadata_.versioned_conformance_report_lowering_ready &&
        !frontend_metadata_.versioned_conformance_report_lowering_replay_key
             .empty()) {
      // M264-C001 versioned conformance-report lowering anchor: the emitted
      // IR now advertises the lowered machine-readable capability report
      // boundary that mirrors the manifest sidecar publication.
      out << "; versioned_conformance_report_lowering = "
          << Objc3VersionedConformanceReportLoweringContractSummary()
          << ";replay_key="
          << frontend_metadata_.versioned_conformance_report_lowering_replay_key
          << "\n";
      // M264-C002 capability-reporting anchor: the emitted IR advertises the
      // truthful runtime/public capability payload boundary carried by the
      // conformance sidecar so later publication paths never reconstruct it.
      out << "; runtime_capability_reporting = "
          << Objc3RuntimeCapabilityReportingContractSummary()
          << ";replay_key="
          << frontend_metadata_.versioned_conformance_report_lowering_replay_key
          << "\n";
    }
    if (ShouldEmitRuntimeBootstrapLowering()) {
      out << "; runtime_bootstrap_ctor_init_emission = "
          << "contract=objc3c-runtime-constructor-init-stub-emission/m254-c002-v1"
          << ";constructor_root_symbol="
          << frontend_metadata_.runtime_bootstrap_lowering_constructor_root_symbol
          << ";constructor_init_stub_symbol=" << RuntimeBootstrapInitStubSymbol()
          << ";registration_table_symbol="
          << RuntimeBootstrapRegistrationTableSymbol()
          << ";image_descriptor_symbol="
          << RuntimeBootstrapImageDescriptorSymbol()
          << ";registration_entrypoint_symbol="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_entrypoint_symbol
          << ";global_ctor_list_model="
          << frontend_metadata_.runtime_bootstrap_lowering_global_ctor_list_model
          << ";happy_path=register-before-user-main\n";
      out << "; runtime_registration_table_image_local_initialization = "
          << "contract=objc3c-runtime-registration-table-image-local-initialization/m254-c003-v1"
          << ";registration_table_symbol="
          << RuntimeBootstrapRegistrationTableSymbol()
          << ";registration_table_layout_model="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_table_layout_model
          << ";registration_table_abi_version="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_table_abi_version
          << ";registration_table_pointer_field_count="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_registration_table_pointer_field_count
          << ";class_section_root_symbol=__objc3_sec_class_descriptors"
          << ";protocol_section_root_symbol=__objc3_sec_protocol_descriptors"
          << ";category_section_root_symbol=__objc3_sec_category_descriptors"
          << ";property_section_root_symbol=__objc3_sec_property_descriptors"
          << ";ivar_section_root_symbol=__objc3_sec_ivar_descriptors"
          << ";selector_pool_symbol="
          << (selector_pool_globals_.empty() ? "null"
                                             : "@__objc3_sec_selector_pool")
          << ";string_pool_symbol="
          << (runtime_string_pool_globals_.empty() ? "null"
                                                   : "@__objc3_sec_string_pool")
          << ";image_local_init_state_symbol="
          << RuntimeBootstrapImageLocalInitStateSymbol()
          << ";image_local_initialization_model="
          << frontend_metadata_
                 .runtime_bootstrap_lowering_image_local_initialization_model
          << ";happy_path=guarded-once-before-runtime-registration\n";
      out << "; runtime_bootstrap_registrar_image_walk = "
          << "contract=" << kObjc3RuntimeBootstrapRegistrarContractId
          << ";stage_registration_table_symbol="
          << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
          << ";image_walk_snapshot_symbol="
          << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol
          << ";image_walk_model="
          << kObjc3RuntimeBootstrapImageWalkModel
          << ";selector_pool_interning_model="
          << kObjc3RuntimeBootstrapSelectorPoolInterningModel
          << ";realization_staging_model="
          << kObjc3RuntimeBootstrapRealizationStagingModel << "\n";
    }
    if (ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()) {
      // M263-C002 registration-descriptor/image-root lowering anchor: the IR
      // boundary now advertises the concrete identifier-driven globals and
      // sections that the native bootstrap path materializes.
      out << "; runtime_registration_descriptor_image_root_lowering = "
          << Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary()
          << ";registration_descriptor_identifier="
          << frontend_metadata_
                 .runtime_bootstrap_registration_descriptor_identifier
          << ";image_root_identifier="
          << frontend_metadata_.runtime_bootstrap_image_root_identifier
          << ";registration_descriptor_symbol="
          << RuntimeBootstrapRegistrationDescriptorSymbol()
          << ";image_root_symbol=" << RuntimeBootstrapImageRootSymbol()
          << "\n";
      if (frontend_metadata_.runtime_metadata_archive_static_link_discovery_ready) {
        // M263-C003 archive/static-link bootstrap replay corpus anchor: the
        // IR boundary now advertises the retained archive replay proof surface
        // that composes the D003 merge model with the live bootstrap replay
        // runtime over emitted C002 registration-descriptor/image-root globals.
        out << "; runtime_bootstrap_archive_static_link_replay_corpus = "
            << Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary()
            << ";translation_unit_identity_key="
            << frontend_metadata_
                   .runtime_metadata_archive_static_link_translation_unit_identity_key
            << ";registration_descriptor_identifier="
            << frontend_metadata_
                   .runtime_bootstrap_registration_descriptor_identifier
            << ";image_root_identifier="
            << frontend_metadata_.runtime_bootstrap_image_root_identifier
            << ";replay_registered_images_symbol="
            << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
            << ";reset_replay_state_snapshot_symbol="
            << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol << "\n";
      }
    }
    const bool emit_class_metaclass_bundle_payloads =
        frontend_metadata_.runtime_metadata_class_metaclass_emission_ready &&
        frontend_metadata_.runtime_metadata_class_metaclass_emission_fail_closed &&
        !frontend_metadata_
             .runtime_metadata_class_metaclass_emission_contract_id.empty() &&
        frontend_metadata_
                .runtime_metadata_class_metaclass_bundles_lexicographic.size() ==
            layout_policy.families[0].descriptor_count;
    const bool emit_protocol_category_bundle_payloads =
        frontend_metadata_.runtime_metadata_protocol_category_emission_ready &&
        frontend_metadata_
            .runtime_metadata_protocol_category_emission_fail_closed &&
        !frontend_metadata_
             .runtime_metadata_protocol_category_emission_contract_id.empty() &&
        frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic.size() ==
            layout_policy.families[1].descriptor_count &&
        frontend_metadata_.runtime_metadata_category_bundles_lexicographic.size() ==
            layout_policy.families[2].descriptor_count;
    const bool emit_member_table_payloads =
        frontend_metadata_.runtime_metadata_member_table_emission_ready &&
        frontend_metadata_.runtime_metadata_member_table_emission_fail_closed &&
        !frontend_metadata_
             .runtime_metadata_member_table_emission_contract_id.empty() &&
        frontend_metadata_.runtime_metadata_property_bundles_lexicographic.size() ==
            layout_policy.families[3].descriptor_count &&
        frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic.size() ==
            layout_policy.families[4].descriptor_count;
    if (emit_class_metaclass_bundle_payloads) {
      std::size_t total_instance_method_refs = 0;
      std::size_t total_class_method_refs = 0;
      for (const auto &bundle :
           frontend_metadata_
               .runtime_metadata_class_metaclass_bundles_lexicographic) {
        total_instance_method_refs += bundle.instance_method_count;
        total_class_method_refs += bundle.class_method_count;
      }
      out << "; runtime_metadata_class_metaclass_emission = "
          << Objc3RuntimeMetadataClassMetaclassEmissionSummary()
          << ";bundle_count="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_bundles_lexicographic.size()
          << ";instance_method_refs=" << total_instance_method_refs
          << ";class_method_refs=" << total_class_method_refs
          << ";typed_handoff_replay="
          << frontend_metadata_
                 .runtime_metadata_class_metaclass_typed_handoff_replay_key
          << "\n";
    }
    if (emit_protocol_category_bundle_payloads) {
      std::size_t total_inherited_protocol_refs = 0;
      for (const auto &bundle :
           frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic) {
        total_inherited_protocol_refs +=
            bundle.inherited_protocol_owner_identities_lexicographic.size();
      }
      std::size_t total_adopted_protocol_refs = 0;
      std::size_t total_category_attachment_refs = 0;
      for (const auto &bundle :
           frontend_metadata_.runtime_metadata_category_bundles_lexicographic) {
        total_adopted_protocol_refs +=
            bundle.adopted_protocol_owner_identities_lexicographic.size();
        total_category_attachment_refs += 3u;
      }
      out << "; runtime_metadata_protocol_category_emission = "
          << Objc3RuntimeMetadataProtocolCategoryEmissionSummary()
          << ";protocol_bundle_count="
          << frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic.size()
          << ";inherited_protocol_refs=" << total_inherited_protocol_refs
          << ";category_bundle_count="
          << frontend_metadata_.runtime_metadata_category_bundles_lexicographic.size()
          << ";adopted_protocol_refs=" << total_adopted_protocol_refs
          << ";attachment_refs=" << total_category_attachment_refs
          << ";typed_handoff_replay="
          << frontend_metadata_
                 .runtime_metadata_protocol_category_typed_handoff_replay_key
          << "\n";
    }
    if (emit_member_table_payloads) {
      std::size_t total_method_entries = 0;
      for (const auto &bundle :
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
      // M256-C003 executable realization-record expansion anchor: lane-C now
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
      // M256-D001 class-realization-runtime freeze anchor: lane-D consumes the
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
      // M256-D002 metaclass-graph-root-class anchor: lane-D now republishes
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
      // M256-D003 category-attachment-protocol-conformance anchor: emitted
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
      // M256-D004 canonical-runnable-object-sample anchor: emitted
      // realization records now publish the truthful object-sample execution
      // boundary above the D003 realized graph. The builtin selector count is
      // fixed to alloc/new/init, while metadata-rich category/protocol shapes
      // remain observable through the paired library/probe proof path.
      // M256-E001 class-protocol-category conformance gate anchor:
      // A003/B004/C003/D004 remain the canonical proof surface that lane-E
      // consumes to decide whether classes/protocols/categories are executable
      // rather than merely modeled.
      // M256-E002 runnable class-protocol-category execution-matrix anchor:
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
      // M265-D001 runtime-helper freeze anchor: the emitted descriptor
      // aggregate plus stable handle ordinals are the current runtime-facing
      // key-path boundary; full runtime evaluation helpers remain deferred to
      // the next lane-D issue.
      out << "; typed_keypath_artifact_emission = "
          << "contract=objc3c-part3-typed-keypath-artifact-emission/m265-c003-v1"
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
        << ";source_to_section_matrix_issue=M253-A002"
        << ";object_format_policy_issue=M253-B003"
        << ";binary_inspection_issue=M253-C006"
        << ";archive_static_link_issue=M253-D003"
        << "\n";
    out << "; runtime_metadata_object_emission_closeout = "
        << Objc3RuntimeMetadataObjectEmissionCloseoutSummary()
        << ";dependency_gate_issue=M253-E001"
        << ";class_object_case_issue=M253-A002"
        << ";binary_object_case_issue=M253-C006"
        << ";linker_fanin_issue=M253-D003"
        << "\n";
    out << "; runtime metadata section scaffold globals\n";

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
                // M256-C001 executable object artifact lowering freeze anchor:
                // object emission binds implementation-owned method entries by
                // canonical owner identity to concrete LLVM definition symbols;
                // later executable-runtime work must extend this binding
                // surface instead of rediscovering bodies from source.
                // M256-C002 executable method-body binding anchor: the
                // implementation pointer is now a fail-closed requirement for
                // every implementation-owned executable method entry. If the
                // canonical method owner identity cannot resolve to exactly one
                // emitted LLVM body symbol, IR/object emission aborts instead
                // of silently leaving a null implementation slot.
                // M255-D003/M255-D004 slow-path anchor: class and category
                // implementation method-table entries now carry callable
                // implementation pointers from registered metadata.
                // M255-D003 slow-path anchor: class-implementation method-table
                // entries now carry callable implementation pointers so the
                // runtime can resolve live class/metaclass bodies from
                // registered metadata.
                // M255-D004 slow-path anchor: category-implementation
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
              entry_initializer << "{ ptr, ptr, ptr, i64, ptr, i64 } { ptr "
                                << selector_symbol << ", ptr "
                                << entry_owner_identity_symbol << ", ptr "
                                << return_type_symbol << ", i64 "
                                << entry.parameter_count << ", ptr "
                                << implementation_symbol << ", i64 "
                                << (entry.has_body ? 1 : 0) << " }";
              entry_initializers.push_back(entry_initializer.str());
            }
            out << list_symbol << " = private global ";
            if (entry_initializers.empty()) {
              out << "{ i64, ptr, ptr } { i64 0, ptr " << owner_identity_symbol
                  << ", ptr " << export_owner_identity_symbol << " }";
            } else {
              out << "{ i64, ptr, ptr, [" << entry_initializers.size()
                  << " x { ptr, ptr, ptr, i64, ptr, i64 }] } { i64 "
                  << entry_initializers.size() << ", ptr "
                  << owner_identity_symbol << ", ptr "
                  << export_owner_identity_symbol << ", ["
                  << entry_initializers.size()
                  << " x { ptr, ptr, ptr, i64, ptr, i64 }] [";
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
                << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i1, i1, i1 } "
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
                << setter_implementation_symbol << ", i64 "
                << bundle.executable_ivar_layout_slot_index << ", i64 "
                << bundle.executable_ivar_layout_size_bytes << ", i64 "
                << bundle.executable_ivar_layout_alignment_bytes << ", i1 "
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
          const auto align_to = [](std::size_t value, std::size_t alignment) {
            const std::size_t effective_alignment =
                std::max<std::size_t>(alignment, 1u);
            const std::size_t remainder = value % effective_alignment;
            return remainder == 0u ? value
                                   : value + (effective_alignment - remainder);
          };

          std::vector<std::size_t> descriptor_offsets(
              frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                  .size(),
              0u);
          std::map<std::string, std::vector<std::size_t>>
              ivar_indexes_by_declaration_owner_identity;
          for (std::size_t i = 0;
               i <
               frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                   .size();
               ++i) {
            const auto &bundle =
                frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                    [i];
            ivar_indexes_by_declaration_owner_identity
                [bundle.declaration_owner_identity]
                    .push_back(i);
          }
          std::map<std::string, std::size_t> instance_size_by_owner_identity;
          for (auto &[owner_identity, indexes] :
               ivar_indexes_by_declaration_owner_identity) {
            std::stable_sort(
                indexes.begin(), indexes.end(),
                [&](std::size_t lhs_index, std::size_t rhs_index) {
                  const auto &lhs =
                      frontend_metadata_
                          .runtime_metadata_ivar_bundles_lexicographic[lhs_index];
                  const auto &rhs =
                      frontend_metadata_
                          .runtime_metadata_ivar_bundles_lexicographic[rhs_index];
                  return std::tie(lhs.executable_ivar_layout_slot_index,
                                  lhs.property_name, lhs.owner_identity) <
                         std::tie(rhs.executable_ivar_layout_slot_index,
                                  rhs.property_name, rhs.owner_identity);
                });
            std::size_t running_offset = 0u;
            std::size_t max_alignment = 1u;
            for (const std::size_t index : indexes) {
              const auto &bundle =
                  frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                      [index];
              const std::size_t alignment =
                  std::max<std::size_t>(
                      bundle.executable_ivar_layout_alignment_bytes, 1u);
              running_offset = align_to(running_offset, alignment);
              descriptor_offsets[index] = running_offset;
              running_offset += bundle.executable_ivar_layout_size_bytes;
              max_alignment = std::max(max_alignment, alignment);
            }
            instance_size_by_owner_identity[owner_identity] =
                align_to(running_offset, max_alignment);
          }

          std::vector<std::string> descriptor_symbols;
          descriptor_symbols.reserve(
              frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic
                  .size());
          std::map<std::string, std::vector<std::string>>
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
            descriptor_symbols.push_back(descriptor_symbol);
            descriptor_symbols_by_declaration_owner_identity
                [bundle.declaration_owner_identity]
                    .push_back(descriptor_symbol);

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
            out << offset_global_symbol << " = private global i64 "
                << descriptor_offsets[i] << ", section \""
                << family.emitted_section_name << "\", align 8\n";
            emit_retained(offset_global_symbol);
            out << layout_record_symbol
                << " = private global { ptr, i64, i64, i64, i64 } { ptr "
                << layout_symbol << ", i64 "
                << bundle.executable_ivar_layout_slot_index << ", i64 "
                << descriptor_offsets[i] << ", i64 "
                << bundle.executable_ivar_layout_size_bytes << ", i64 "
                << bundle.executable_ivar_layout_alignment_bytes
                << " }, section \"" << family.emitted_section_name
                << "\", align 8\n";
            emit_retained(layout_record_symbol);

            out << descriptor_symbol
                << " = private global { ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64 } { ptr "
                << owner_identity_symbol << ", ptr "
                << declaration_owner_identity_symbol << ", ptr "
                << export_owner_identity_symbol << ", ptr "
                << property_owner_identity_symbol << ", ptr "
                << property_name_symbol << ", ptr " << ivar_binding_symbol
                << ", ptr " << layout_record_symbol << ", ptr "
                << offset_global_symbol << ", i64 "
                << bundle.executable_ivar_layout_slot_index << ", i64 "
                << descriptor_offsets[i] << ", i64 "
                << bundle.executable_ivar_layout_size_bytes << ", i64 "
                << bundle.executable_ivar_layout_alignment_bytes
                << " }, section \"" << family.emitted_section_name
                << "\", align 8\n";
            emit_retained(descriptor_symbol);
          }

          std::size_t layout_table_ordinal = 0u;
          for (const auto &[owner_identity, owner_descriptor_symbols] :
               descriptor_symbols_by_declaration_owner_identity) {
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
                out << "ptr " << owner_descriptor_symbols[symbol_index];
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
            // M256-C003 executable realization-record expansion anchor: each
            // class/metaclass record now preserves bundle-owner, object-owner,
            // and super-object identities in-line with the realized bundle
            // pointer plus the existing owner-scoped method-list ref.
            out << descriptor_symbol
                << " = private global { { ptr, ptr, ptr, ptr, ptr, ptr, ptr }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr } } "
                   "{ { ptr, ptr, ptr, ptr, ptr, ptr, ptr } { ptr "
                << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
                << class_object_identity_symbol << ", ptr "
                << class_super_object_identity_symbol << ", ptr "
                << super_bundle_symbol << ", ptr "
                << instance_method_list_ref_symbol << ", ptr "
                << adopted_protocol_refs_symbol
                << " }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr } { ptr "
                << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
                << metaclass_object_identity_symbol << ", ptr "
                << metaclass_super_object_identity_symbol << ", ptr "
                << super_bundle_symbol << ", ptr "
                << metaclass_method_list_ref_symbol << ", ptr "
                << adopted_protocol_refs_symbol << " } }, section \""
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
          // M253-C003 protocol/category data emission anchor: protocol records
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
            // M256-C003 executable realization-record expansion anchor:
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
          // M253-C003 protocol/category data emission anchor: category records
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

            // M256-C003 executable realization-record expansion anchor:
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
          if (!fail_open_fallback_triggered_) {
            fail_open_fallback_triggered_ = true;
            fail_open_fallback_reason_ =
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
          if (!fail_open_fallback_triggered_) {
            fail_open_fallback_triggered_ = true;
            fail_open_fallback_reason_ =
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
          << frontend_metadata_.runtime_metadata_section_scaffold_class_descriptor_count
          << ", i64 "
          << frontend_metadata_
                 .runtime_metadata_section_scaffold_protocol_descriptor_count
          << ", i64 "
          << frontend_metadata_
                 .runtime_metadata_section_scaffold_category_descriptor_count
          << ", i64 "
          << frontend_metadata_
                 .runtime_metadata_section_scaffold_property_descriptor_count
          << ", i64 "
          << frontend_metadata_
                 .runtime_metadata_section_scaffold_ivar_descriptor_count
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
        // M263-C002 emits first-class image-root/registration-descriptor
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
    ctx.arc_cleanup_scope_depths.push_back(ctx.arc_owned_cleanup_ptrs.size());
  }

  void EmitDeferredCleanupBlock(const BlockStmt *block_stmt,
                                FunctionContext &ctx) const {
    // M266-C002 defer/guard lowering anchor: defer bodies now lower into
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
    const std::size_t target_arc_cleanup_depth =
        ctx.arc_cleanup_scope_depths.empty() ? 0u : ctx.arc_cleanup_scope_depths.back();
    if (!ctx.arc_cleanup_scope_depths.empty()) {
      ctx.arc_cleanup_scope_depths.pop_back();
    }
    if (emit_cleanup) {
      EmitDeferredCleanupForScopeBucket(deferred_blocks, ctx);
      EmitPendingBlockDisposeUnwindToDepth(ctx, target_block_dispose_depth);
      EmitArcOwnedCleanupUnwindToDepth(ctx, target_arc_cleanup_depth);
    }
    for (const auto &binding : scope_bindings) {
      ctx.block_bindings.erase(binding.first);
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

  void EmitBlockDisposeHelper(const Expr &expr) const {
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
      if (!SortedStringListContains(
              expr.block_runtime_owned_object_capture_names_lexicographic,
              capture_name)) {
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
      out << "  " << released_value << " = call i32 @"
          << kObjc3RuntimeReleaseI32Symbol << "(i32 " << loaded_value << ")\n";
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
    // M261-C004 escaping-block runtime-hook anchor: readonly-scalar escaping
    // block values now lower through a private runtime promotion hook instead
    // of failing closed at the first escaping expression use.
    const bool supported = allow_nonescaping_scalar_promotion
                               ? BlockLiteralSupportsScalarRuntimePromotion(expr)
                               : BlockLiteralSupportsEscapingRuntimeHookLowering(
                                     expr);
    if (!supported) {
      return EmitUnsupportedI32Value(
          "escaping block value still requires normalized block runtime helper metadata that lands in later M261 runtime issues");
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
    }
    const std::string promoted =
        EmitPromotedBlockHandle(*binding.literal, binding.storage_ptr, ctx,
                                true);
    if (promoted == "poison") {
      return promoted;
    }
    binding.promoted_handle_ptr =
        "%block.promoted.addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + binding.promoted_handle_ptr +
                              " = alloca i32, align 4");
    ctx.code_lines.push_back("  store i32 " + promoted + ", ptr " +
                             binding.promoted_handle_ptr + ", align 4");
    return promoted;
  }

  std::string EmitIdentifierValue(const std::string &name,
                                  FunctionContext &ctx) const {
    auto block_it = ctx.block_bindings.find(name);
    if (block_it != ctx.block_bindings.end()) {
      return EmitPromotedBlockHandleLoad(block_it->second, ctx);
    }
    const std::string ptr = LookupVarPtr(ctx, name);
    if (!ptr.empty()) {
      const std::string tmp = NewTemp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = load i32, ptr " + ptr + ", align 4");
      return tmp;
    }
    if (globals_.find(name) != globals_.end()) {
      const std::string tmp = NewTemp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = load i32, ptr @" + name + ", align 4");
      return tmp;
    }
    const auto immediate_it = ctx.immediate_identifiers.find(name);
    if (immediate_it != ctx.immediate_identifiers.end()) {
      return std::to_string(immediate_it->second);
    }
    // Match bindings are now materialized by the executable Part 5 lowering
    // path when a live match arm captures the condition value. Remaining
    // unresolved identifiers still fail closed here.
    return EmitUnsupportedI32Value("unresolved identifier '" + name + "' during IR lowering");
  }

  std::string EmitTypedKeyPathLiteralValue(const Expr &expr) const {
    const std::string profile =
        expr.typed_keypath_literal_profile.empty()
            ? std::string("typed-keypath:root=") + expr.typed_keypath_root_name
            : expr.typed_keypath_literal_profile;
    const auto artifact_it = typed_keypath_artifacts_.find(profile);
    if (artifact_it == typed_keypath_artifacts_.end()) {
      return EmitUnsupportedI32Value(
          "typed key-path artifact '" + profile +
          "' was not registered before IR lowering");
    }
    return std::to_string(
        static_cast<unsigned long long>(artifact_it->second.ordinal + 1u));
  }

  void EmitBlockInvokeThunk(const Expr &expr) const {
    // M261-C003 byref-cell/copy-helper/dispose-helper anchor: each runnable
    // local block literal now receives one internal invoke thunk definition
    // that rehydrates readonly captures from snapshot cells and mutated captures
    // from stack byref-cell references.
    const std::string symbol = BuildBlockInvokeSymbol(expr);
    if (symbol.empty() || !emitted_block_invoke_symbols_.insert(symbol).second) {
      return;
    }

    std::ostringstream out;
    out << "define internal i32 @" << symbol
        << "(ptr %block, i32 %arg0, i32 %arg1, i32 %arg2, i32 %arg3) {\n";
    out << "entry:\n";

    FunctionContext ctx;
    ctx.return_type = ValueType::I32;
    PushScope(ctx);

    const std::string block_storage_type = BuildBlockStorageType(expr);
    const bool pointer_capture_storage =
        BlockLiteralUsesPointerCaptureStorage(expr);
    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size(); ++i) {
      const std::string &capture_name = expr.block_capture_names_lexicographic[i];
      const std::string slot_ptr = NewTemp(ctx);
      const std::size_t capture_field_index =
          pointer_capture_storage ? 3u : 1u;
      ctx.entry_lines.push_back("  " + slot_ptr + " = getelementptr inbounds " +
                                block_storage_type +
                                ", ptr %block, i32 0, i32 " +
                                std::to_string(capture_field_index) +
                                ", i32 " + std::to_string(i));
      if (pointer_capture_storage) {
        const std::string capture_ptr = NewTemp(ctx);
        ctx.entry_lines.push_back("  " + capture_ptr + " = load ptr, ptr " +
                                  slot_ptr + ", align 8");
        ctx.scopes.back()[capture_name] = capture_ptr;
        continue;
      }
      const std::string ptr =
          "%" + capture_name + ".addr." + std::to_string(ctx.temp_counter++);
      const std::string value = NewTemp(ctx);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.scopes.back()[capture_name] = ptr;
      ctx.entry_lines.push_back("  " + value + " = load i32, ptr " + slot_ptr +
                                ", align 4");
      ctx.entry_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                                ", align 4");
    }

    for (std::size_t i = 0; i < expr.block_parameters_source_order.size() && i < 4u; ++i) {
      const auto &parameter = expr.block_parameters_source_order[i];
      const std::string ptr =
          "%" + parameter.name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.entry_lines.push_back("  store i32 %arg" + std::to_string(i) + ", ptr " + ptr + ", align 4");
      ctx.scopes.back()[parameter.name] = ptr;
    }

    for (const auto &stmt : expr.block_body) {
      EmitStatement(stmt.get(), ctx);
      if (ctx.terminated) {
        break;
      }
    }

    if (!ctx.terminated) {
      EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
      EmitPendingBlockDisposeHelpers(ctx);
      EmitArcOwnedCleanupReleases(ctx);
      ctx.code_lines.push_back("  ret i32 0");
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }
    out << "}\n";
    block_function_definitions_.push_back(out.str());
  }

  std::string EmitBlockLiteralStorage(const Expr &expr,
                                      FunctionContext &ctx) const {
    // M261-C003 byref-cell/copy-helper/dispose-helper anchor: the current live
    // lowering slice now supports non-escaping byref and owned-capture block
    // objects through stack snapshot/byref cells plus emitted helper bodies.
    if (BlockLiteralRequiresFutureRuntimeLanes(expr)) {
      return EmitUnsupportedI32Value(
          "block literal requires escaping heap-promotion or runtime-managed copy/dispose lowering that lands in later M261 issues");
    }
    if (expr.block_parameter_count > 4u) {
      return EmitUnsupportedI32Value(
          "block literal exceeds current runnable invoke-thunk arity limit of 4");
    }

    EmitBlockInvokeThunk(expr);
    EmitBlockCopyHelper(expr);
    EmitBlockDisposeHelper(expr);

    const std::string storage_type = BuildBlockStorageType(expr);
    const bool pointer_capture_storage =
        BlockLiteralUsesPointerCaptureStorage(expr);
    const std::string storage_ptr =
        "%block.literal.addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + storage_ptr + " = alloca " + storage_type + ", align 8");

    const std::string invoke_ptr_slot = NewTemp(ctx);
    ctx.code_lines.push_back("  " + invoke_ptr_slot + " = getelementptr inbounds " +
                             storage_type + ", ptr " + storage_ptr + ", i32 0, i32 0");
    ctx.code_lines.push_back("  store ptr @" + BuildBlockInvokeSymbol(expr) + ", ptr " +
                             invoke_ptr_slot + ", align 8");

    if (pointer_capture_storage) {
      const std::string copy_helper_slot = NewTemp(ctx);
      const std::string dispose_helper_slot = NewTemp(ctx);
      ctx.code_lines.push_back("  " + copy_helper_slot +
                               " = getelementptr inbounds " + storage_type +
                               ", ptr " + storage_ptr + ", i32 0, i32 1");
      ctx.code_lines.push_back("  store ptr " +
                               std::string(expr.block_runtime_copy_helper_required
                                               ? "@" + BuildBlockCopyHelperSymbol(expr)
                                               : "null") +
                               ", ptr " + copy_helper_slot + ", align 8");
      ctx.code_lines.push_back("  " + dispose_helper_slot +
                               " = getelementptr inbounds " + storage_type +
                               ", ptr " + storage_ptr + ", i32 0, i32 2");
      ctx.code_lines.push_back("  store ptr " +
                               std::string(expr.block_runtime_dispose_helper_required
                                               ? "@" + BuildBlockDisposeHelperSymbol(expr)
                                               : "null") +
                               ", ptr " + dispose_helper_slot + ", align 8");
    }

    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size(); ++i) {
      const std::string &capture_name = expr.block_capture_names_lexicographic[i];
      const std::string capture_slot = NewTemp(ctx);
      if (pointer_capture_storage) {
        std::string capture_cell_ptr;
        if (SortedStringListContains(
                expr.block_byref_capture_names_lexicographic, capture_name)) {
          capture_cell_ptr = LookupVarPtr(ctx, capture_name);
          if (capture_cell_ptr.empty()) {
            return EmitUnsupportedI32Value(
                "block literal byref capture '" + capture_name +
                "' could not be resolved during IR lowering");
          }
        } else {
          capture_cell_ptr = "%" + capture_name + ".capture.addr." +
                             std::to_string(ctx.temp_counter++);
          ctx.entry_lines.push_back("  " + capture_cell_ptr +
                                    " = alloca i32, align 4");
          const std::string capture_value = EmitIdentifierValue(capture_name, ctx);
          ctx.code_lines.push_back("  store i32 " + capture_value + ", ptr " +
                                   capture_cell_ptr + ", align 4");
        }
        ctx.code_lines.push_back("  " + capture_slot +
                                 " = getelementptr inbounds " + storage_type +
                                 ", ptr " + storage_ptr +
                                 ", i32 0, i32 3, i32 " + std::to_string(i));
        ctx.code_lines.push_back("  store ptr " + capture_cell_ptr + ", ptr " +
                                 capture_slot + ", align 8");
        continue;
      }
      const std::string capture_value = EmitIdentifierValue(capture_name, ctx);
      ctx.code_lines.push_back("  " + capture_slot + " = getelementptr inbounds " +
                               storage_type + ", ptr " + storage_ptr + ", i32 0, i32 1, i32 " +
                               std::to_string(i));
      ctx.code_lines.push_back("  store i32 " + capture_value + ", ptr " + capture_slot +
                               ", align 4");
    }

    if (pointer_capture_storage && expr.block_runtime_copy_helper_required) {
      ctx.code_lines.push_back("  call void @" + BuildBlockCopyHelperSymbol(expr) +
                               "(ptr " + storage_ptr + ")");
    }
    if (pointer_capture_storage && expr.block_runtime_dispose_helper_required) {
      ctx.pending_block_dispose_calls.push_back(
          PendingBlockDisposeCall{BuildBlockDisposeHelperSymbol(expr),
                                  storage_ptr});
    }

    return storage_ptr;
  }

  std::string EmitBlockInvokeCall(const BlockBinding &binding,
                                  const Expr *call_expr,
                                  FunctionContext &ctx) const {
    if (binding.literal == nullptr) {
      return EmitUnsupportedI32Value(
          "missing block literal metadata for local callable invocation");
    }

    if (!binding.promoted_handle_ptr.empty()) {
      std::array<std::string, 4> args{"0", "0", "0", "0"};
      for (std::size_t i = 0; i < call_expr->args.size() && i < args.size();
           ++i) {
        args[i] = EmitExpr(call_expr->args[i].get(), ctx);
      }
      const std::string handle = NewTemp(ctx);
      ctx.code_lines.push_back("  " + handle + " = load i32, ptr " +
                               binding.promoted_handle_ptr + ", align 4");
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back(
          "  " + out + " = call i32 @" +
          std::string(kObjc3RuntimeInvokeBlockI32Symbol) + "(i32 " + handle +
          ", i32 " + args[0] + ", i32 " + args[1] + ", i32 " + args[2] +
          ", i32 " + args[3] + ")");
      return out;
    }

    const std::string storage_type = BuildBlockStorageType(*binding.literal);
    const std::string invoke_ptr_slot = NewTemp(ctx);
    const std::string invoke_ptr = NewTemp(ctx);
    ctx.code_lines.push_back("  " + invoke_ptr_slot + " = getelementptr inbounds " +
                             storage_type + ", ptr " + binding.storage_ptr + ", i32 0, i32 0");
    ctx.code_lines.push_back("  " + invoke_ptr + " = load ptr, ptr " + invoke_ptr_slot +
                             ", align 8");

    std::array<std::string, 4> args{"0", "0", "0", "0"};
    for (std::size_t i = 0; i < call_expr->args.size() && i < args.size(); ++i) {
      args[i] = EmitExpr(call_expr->args[i].get(), ctx);
    }

    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + out + " = call i32 " + invoke_ptr + "(ptr " +
                             binding.storage_ptr + ", i32 " + args[0] + ", i32 " + args[1] +
                             ", i32 " + args[2] + ", i32 " + args[3] + ")");
    return out;
  }

  static int NextNonZeroReceiverIdentityValue(std::size_t ordinal, int salt) {
    constexpr int kBase = 1024;
    constexpr int kStride = 17;
    return kBase + static_cast<int>(ordinal * kStride) + salt;
  }

  void CollectKnownClassReceiverConstants() {
    std::set<std::string> class_names;
    for (const auto &interface_decl : program_.interfaces) {
      if (!interface_decl.has_category && !interface_decl.name.empty()) {
        class_names.insert(interface_decl.name);
      }
    }
    for (const auto &implementation : program_.implementations) {
      if (!implementation.has_category && !implementation.name.empty()) {
        class_names.insert(implementation.name);
      }
    }
    std::size_t ordinal = 0;
    for (const std::string &class_name : class_names) {
      class_receiver_constants_[class_name] =
          NextNonZeroReceiverIdentityValue(ordinal++, 0);
    }
  }

  int LookupClassReceiverIdentityValue(const std::string &class_name) const {
    const auto value_it = class_receiver_constants_.find(class_name);
    if (value_it == class_receiver_constants_.end()) {
      return 0;
    }
    return value_it->second;
  }

  static int BuildInstanceReceiverIdentityValue(int class_identity) {
    return class_identity == 0 ? 0 : class_identity + 1;
  }

  static int BuildClassReceiverIdentityValue(int class_identity) {
    return class_identity == 0 ? 0 : class_identity + 2;
  }

  void SeedKnownClassReceiverBindings(FunctionContext &ctx) const {
    for (const auto &entry : class_receiver_constants_) {
      ctx.immediate_identifiers.emplace(entry.first, entry.second);
    }
  }

  std::string CoerceI32ToBoolI1(const std::string &i32_value, FunctionContext &ctx) const {
    const std::string bool_i1 = NewTemp(ctx);
    ctx.code_lines.push_back("  " + bool_i1 + " = icmp ne i32 " + i32_value + ", 0");
    return bool_i1;
  }

  std::string CoerceValueToI32(const std::string &value, ValueType value_type, FunctionContext &ctx) const {
    if (value_type != ValueType::Bool) {
      return value;
    }
    const std::string widened = NewTemp(ctx);
    ctx.code_lines.push_back("  " + widened + " = zext i1 " + value + " to i32");
    return widened;
  }

  const LoweredFunctionSignature *LookupFunctionSignature(const std::string &name) const {
    auto signature_it = function_signatures_.find(name);
    if (signature_it == function_signatures_.end()) {
      return nullptr;
    }
    return &signature_it->second;
  }

  void AppendLoweredCallArg(std::vector<std::string> &args, const std::string &arg_i32, ValueType expected_type,
                            FunctionContext &ctx) const {
    if (expected_type == ValueType::Bool) {
      const std::string arg_i1 = CoerceI32ToBoolI1(arg_i32, ctx);
      args.push_back("i1 " + arg_i1);
      return;
    }
    args.push_back("i32 " + arg_i32);
  }

  void RegisterArcOwnedCleanupPtr(const std::string &ptr,
                                  FunctionContext &ctx) const {
    if (ptr.empty() || !ctx.arc_owned_cleanup_ptr_set.insert(ptr).second) {
      return;
    }
    ctx.arc_owned_cleanup_ptrs.push_back(ptr);
    ctx.arc_owned_storage_ptrs.insert(ptr);
  }

  void EmitArcOwnedCleanupReleases(FunctionContext &ctx) const {
    EmitArcOwnedCleanupUnwindToDepth(ctx, 0u);
  }

  void EmitTerminalCleanupToDepth(FunctionContext &ctx, std::size_t scope_depth,
                                  std::size_t autoreleasepool_depth,
                                  std::size_t pending_block_dispose_depth,
                                  std::size_t arc_cleanup_depth) const {
    EmitDeferredCleanupTerminalToDepth(ctx, scope_depth);
    EmitAutoreleasepoolUnwindToDepth(ctx, autoreleasepool_depth);
    EmitPendingBlockDisposeTerminalCleanupToDepth(
        ctx, pending_block_dispose_depth, ctx.code_lines);
    EmitArcOwnedTerminalCleanupToDepth(
        ctx, arc_cleanup_depth, ctx.code_lines, ctx.temp_counter);
  }

  void EmitTypedReturn(const std::string &i32_value, FunctionContext &ctx) const {
    if (ctx.return_type == ValueType::Void) {
      EmitDeferredCleanupTerminalToDepth(ctx, 0u);
      EmitPendingBlockDisposeTerminalCleanupToDepth(ctx, 0u, ctx.code_lines);
      EmitArcOwnedTerminalCleanupToDepth(
          ctx, 0u, ctx.code_lines, ctx.temp_counter);
      ctx.code_lines.push_back("  ret void");
      return;
    }
    std::string returned_value = i32_value;
    if (ctx.arc_return_insert_retain) {
      const std::string retained_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                               std::string(kObjc3RuntimeRetainI32Symbol) +
                               "(i32 " + returned_value + ")");
      returned_value = retained_value;
    }
    if (ctx.arc_return_insert_autorelease) {
      const std::string autoreleased_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + autoreleased_value + " = call i32 @" +
                               std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                               "(i32 " + returned_value + ")");
      returned_value = autoreleased_value;
    }
    EmitDeferredCleanupTerminalToDepth(ctx, 0u);
    EmitPendingBlockDisposeTerminalCleanupToDepth(ctx, 0u, ctx.code_lines);
    EmitArcOwnedTerminalCleanupToDepth(
        ctx, 0u, ctx.code_lines, ctx.temp_counter);
    if (ctx.return_type == ValueType::Bool) {
      const std::string bool_i1 = CoerceI32ToBoolI1(returned_value, ctx);
      ctx.code_lines.push_back("  ret i1 " + bool_i1);
      return;
    }
    ctx.code_lines.push_back("  ret i32 " + returned_value);
  }

  void EmitTypedParamStore(const FuncParam &param, std::size_t index, const std::string &ptr, FunctionContext &ctx) const {
    if (param.type == ValueType::Bool) {
      const std::string widened = "%arg" + std::to_string(index) + ".zext." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + widened + " = zext i1 %arg" + std::to_string(index) + " to i32");
      ctx.entry_lines.push_back("  store i32 " + widened + ", ptr " + ptr + ", align 4");
      return;
    }
    std::string stored_value = "%arg" + std::to_string(index);
    if (EffectiveArcParamInsertRetain(param, frontend_metadata_.arc_mode_enabled)) {
      const std::string retained_value =
          "%arg" + std::to_string(index) + ".retained." +
          std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + retained_value + " = call i32 @" +
                                std::string(kObjc3RuntimeRetainI32Symbol) +
                                "(i32 " + stored_value + ")");
      stored_value = retained_value;
    }
    ctx.entry_lines.push_back("  store i32 " + stored_value + ", ptr " + ptr +
                              ", align 4");
    if (EffectiveArcParamInsertRelease(param, frontend_metadata_.arc_mode_enabled)) {
      RegisterArcOwnedCleanupPtr(ptr, ctx);
    }
  }

  void EmitAssignmentStore(const std::string &ptr, const std::string &op, const Expr *value_expr,
                           FunctionContext &ctx) const {
    if (ptr.empty()) {
      return;
    }
    int assigned_const_value = 0;
    const bool has_assigned_const_value =
        op == "=" && value_expr != nullptr && TryGetCompileTimeI32ExprInContext(value_expr, ctx, assigned_const_value);
    const bool has_assigned_nil_value = op == "=" && value_expr != nullptr && IsCompileTimeNilReceiverExprInContext(value_expr, ctx);
    // Any explicit write invalidates compile-time nil binding for this storage slot.
    ctx.nil_bound_ptrs.erase(ptr);
    // Any explicit write invalidates compile-time known non-zero binding for this storage slot.
    ctx.nonzero_bound_ptrs.erase(ptr);
    // Any explicit write invalidates tracked compile-time constant value for this storage slot.
    ctx.const_value_ptrs.erase(ptr);
    if (op == "++" || op == "--") {
      const std::string lhs = NewTemp(ctx);
      ctx.code_lines.push_back("  " + lhs + " = load i32, ptr " + ptr + ", align 4");
      const std::string out = NewTemp(ctx);
      const std::string opcode = op == "++" ? "add" : "sub";
      ctx.code_lines.push_back("  " + out + " = " + opcode + " i32 " + lhs + ", 1");
      ctx.code_lines.push_back("  store i32 " + out + ", ptr " + ptr + ", align 4");
      return;
    }
    if (op == "=") {
      if (value_expr == nullptr) {
        return;
      }
      const std::string value = EmitExpr(value_expr, ctx);
      if (ctx.arc_owned_storage_ptrs.find(ptr) != ctx.arc_owned_storage_ptrs.end()) {
        const std::string retained_value = NewTemp(ctx);
        const std::string previous_value = NewTemp(ctx);
        const std::string released_value = NewTemp(ctx);
        ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                                 std::string(kObjc3RuntimeRetainI32Symbol) +
                                 "(i32 " + value + ")");
        ctx.code_lines.push_back("  " + previous_value +
                                 " = load i32, ptr " + ptr + ", align 4");
        ctx.code_lines.push_back("  store i32 " + retained_value + ", ptr " +
                                 ptr + ", align 4");
        ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                                 std::string(kObjc3RuntimeReleaseI32Symbol) +
                                 "(i32 " + previous_value + ")");
        (void)released_value;
      } else {
        ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                                 ", align 4");
      }
      if (has_assigned_nil_value && ptr.rfind("@", 0) != 0) {
        ctx.nil_bound_ptrs.insert(ptr);
      }
      if (has_assigned_const_value) {
        ctx.const_value_ptrs[ptr] = assigned_const_value;
        if (assigned_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
      }
      return;
    }

    if (value_expr == nullptr) {
      return;
    }
    std::string binary_opcode;
    if (!TryGetCompoundAssignmentBinaryOpcode(op, binary_opcode)) {
      const std::string value = EmitExpr(value_expr, ctx);
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
      return;
    }

    const std::string lhs = NewTemp(ctx);
    ctx.code_lines.push_back("  " + lhs + " = load i32, ptr " + ptr + ", align 4");
    const std::string rhs = EmitExpr(value_expr, ctx);
    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + out + " = " + binary_opcode + " i32 " + lhs + ", " + rhs);
    ctx.code_lines.push_back("  store i32 " + out + ", ptr " + ptr + ", align 4");
  }

  void EmitForClause(const ForClause &clause, FunctionContext &ctx) const {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        if (clause.value != nullptr) {
          (void)EmitExpr(clause.value.get(), ctx);
        }
        return;
      case ForClause::Kind::Assign: {
        const std::string ptr = LookupVarPtr(ctx, clause.name);
        if (ptr.empty() && ctx.block_bindings.find(clause.name) != ctx.block_bindings.end()) {
          (void)EmitUnsupportedI32Value(
              "reassigning block values is not yet runnable in Objective-C 3 native mode");
          return;
        }
        EmitAssignmentStore(ptr, clause.op, clause.value.get(), ctx);
        return;
      }
      case ForClause::Kind::Let: {
        if (ctx.scopes.empty() || clause.value == nullptr) {
          return;
        }
        const std::string value = EmitExpr(clause.value.get(), ctx);
        const std::string ptr = "%" + clause.name + ".addr." + std::to_string(ctx.temp_counter++);
        int clause_const_value = 0;
        const bool has_clause_const_value = TryGetCompileTimeI32ExprInContext(clause.value.get(), ctx, clause_const_value);
        const bool has_clause_nil_value = IsCompileTimeNilReceiverExprInContext(clause.value.get(), ctx);
        ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
        ctx.scopes.back()[clause.name] = ptr;
        if (has_clause_nil_value) {
          ctx.nil_bound_ptrs.insert(ptr);
        }
        if (has_clause_const_value) {
          ctx.const_value_ptrs[ptr] = clause_const_value;
        }
        if (has_clause_const_value && clause_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
        ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
        return;
      }
    }
  }

  std::string EmitLocalI32Binding(const std::string &name, const Expr *value_expr,
                                  FunctionContext &ctx,
                                  bool mark_runtime_nonnull) const {
    const std::string value = EmitExpr(value_expr, ctx);
    int const_value = 0;
    const bool has_const_value =
        TryGetCompileTimeI32ExprInContext(value_expr, ctx, const_value);
    const bool has_nil_value =
        IsCompileTimeNilReceiverExprInContext(value_expr, ctx);
    const std::string ptr =
        "%" + name + ".addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
    ctx.scopes.back()[name] = ptr;
    if (has_nil_value) {
      ctx.nil_bound_ptrs.insert(ptr);
    }
    if (has_const_value) {
      ctx.const_value_ptrs[ptr] = const_value;
    }
    if ((has_const_value && const_value != 0) || mark_runtime_nonnull) {
      ctx.nonzero_bound_ptrs.insert(ptr);
    }
    ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                             ", align 4");
    return value;
  }

  void EmitOptionalBindingIfStatement(const IfStmt *if_stmt,
                                      FunctionContext &ctx) const {
    if (if_stmt == nullptr) {
      return;
    }

    const bool is_guard = if_stmt->guard_binding_surface_enabled ||
                          if_stmt->guard_condition_list_surface_enabled;
    const std::size_t binding_count =
        std::min(if_stmt->optional_binding_clause_count, if_stmt->then_body.size());
    if (binding_count == 0u && !is_guard) {
      return;
    }
    const std::string success_label =
        NewLabel(ctx, is_guard ? "guard_success_" : "if_bind_success_");
    const std::string failure_label =
        NewLabel(ctx, is_guard ? "guard_else_" : "if_bind_else_");
    const std::string merge_label =
        NewLabel(ctx, is_guard ? "guard_end_" : "if_bind_end_");

    PushScope(ctx);
    for (std::size_t index = 0; index < binding_count; ++index) {
      const Stmt *binding_stmt = if_stmt->then_body[index].get();
      if (binding_stmt == nullptr || binding_stmt->kind != Stmt::Kind::Let ||
          binding_stmt->let_stmt == nullptr) {
        (void)EmitUnsupportedI32Value(
            "optional binding lowering expected synthetic let clauses");
        PopScope(ctx, false);
        return;
      }
      const LetStmt *binding = binding_stmt->let_stmt.get();
      const std::string value =
          EmitLocalI32Binding(binding->name, binding->value.get(), ctx, true);
      const std::string is_present = NewTemp(ctx);
      const std::string next_label =
          (index + 1u == binding_count) ? success_label
                                        : NewLabel(ctx, "if_bind_clause_");
      ctx.code_lines.push_back("  " + is_present + " = icmp ne i32 " + value +
                               ", 0");
      ctx.code_lines.push_back("  br i1 " + is_present + ", label %" +
                               next_label + ", label %" + failure_label);
      if (next_label != success_label) {
        ctx.code_lines.push_back(next_label + ":");
      }
    }

    if (is_guard) {
      const std::string guard_ready_label =
          if_stmt->guard_condition_exprs.empty()
              ? success_label
              : NewLabel(ctx, "guard_ready_");
      if (binding_count == 0u) {
        ctx.code_lines.push_back("  br label %" + success_label);
      }
      ctx.code_lines.push_back(success_label + ":");
      for (const auto &guard_condition : if_stmt->guard_condition_exprs) {
        const std::string condition_value = EmitExpr(guard_condition.get(), ctx);
        const std::string condition_i1 = NewTemp(ctx);
        const bool is_last_condition =
            guard_condition.get() == if_stmt->guard_condition_exprs.back().get();
        const std::string next_label =
            is_last_condition ? guard_ready_label
                              : NewLabel(ctx, "guard_clause_");
        ctx.code_lines.push_back("  " + condition_i1 + " = icmp ne i32 " +
                                 condition_value + ", 0");
        ctx.code_lines.push_back("  br i1 " + condition_i1 + ", label %" +
                                 next_label + ", label %" + failure_label);
        if (!is_last_condition) {
          ctx.code_lines.push_back(next_label + ":");
        }
      }
      if (guard_ready_label != success_label) {
        ctx.code_lines.push_back(guard_ready_label + ":");
      }
      const auto promoted_bindings = ctx.scopes.back();
      PopScope(ctx, false);
      for (const auto &binding : promoted_bindings) {
        ctx.scopes.back()[binding.first] = binding.second;
      }
      ctx.code_lines.push_back("  br label %" + merge_label);

      ctx.code_lines.push_back(failure_label + ":");
      PushScope(ctx);
      ctx.terminated = false;
      for (const auto &s : if_stmt->else_body) {
        EmitStatement(s.get(), ctx);
      }
      const bool else_terminated = ctx.terminated;
      PopScope(ctx, !else_terminated);
      if (!else_terminated) {
        ctx.code_lines.push_back("  br label %" + merge_label);
      }

      ctx.code_lines.push_back(merge_label + ":");
      ctx.terminated = false;
      return;
    }

    ctx.code_lines.push_back(success_label + ":");
    ctx.terminated = false;
    for (std::size_t index = binding_count; index < if_stmt->then_body.size();
         ++index) {
      EmitStatement(if_stmt->then_body[index].get(), ctx);
    }
    const bool then_terminated = ctx.terminated;
    PopScope(ctx, !then_terminated);
    if (!then_terminated) {
      ctx.code_lines.push_back("  br label %" + merge_label);
    }

    ctx.code_lines.push_back(failure_label + ":");
    PushScope(ctx);
    ctx.terminated = false;
    for (const auto &s : if_stmt->else_body) {
      EmitStatement(s.get(), ctx);
    }
    const bool else_terminated = ctx.terminated;
    PopScope(ctx, !else_terminated);
    if (!else_terminated) {
      ctx.code_lines.push_back("  br label %" + merge_label);
    }

    if (then_terminated && else_terminated) {
      ctx.terminated = true;
      return;
    }
    ctx.code_lines.push_back(merge_label + ":");
    ctx.terminated = false;
  }

  bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      return true;
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return IsCompileTimeNilReceiverExprInContext(expr->right.get(), ctx);
      }
      return IsCompileTimeNilReceiverExprInContext(expr->third.get(), ctx);
    }
    if (expr->kind != Expr::Kind::Identifier) {
      return false;
    }
    const std::string ptr = LookupVarPtr(ctx, expr->ident);
    if (ptr.empty()) {
      return LookupImmediateIdentifierValue(ctx, expr->ident) == 0;
    }
    if (ctx.nil_bound_ptrs.find(ptr) != ctx.nil_bound_ptrs.end()) {
      return true;
    }
    if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
      return global_nil_proven_symbols_.find(expr->ident) != global_nil_proven_symbols_.end();
    }
    return false;
  }

  bool IsCompileTimeGlobalNilExpr(const Expr *expr) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      return true;
    }
    if (expr->kind == Expr::Kind::Identifier) {
      return global_nil_proven_symbols_.find(expr->ident) != global_nil_proven_symbols_.end();
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      const FunctionContext global_eval_ctx;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), global_eval_ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return IsCompileTimeGlobalNilExpr(expr->right.get());
      }
      return IsCompileTimeGlobalNilExpr(expr->third.get());
    }
    return false;
  }

  bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::Number) {
      value = expr->number;
      return true;
    }
    if (expr->kind == Expr::Kind::BoolLiteral) {
      value = expr->bool_value ? 1 : 0;
      return true;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      value = 0;
      return true;
    }
    if (expr->kind == Expr::Kind::Identifier) {
      const std::string ptr = LookupVarPtr(ctx, expr->ident);
      if (ptr.empty()) {
        const int immediate_value =
            LookupImmediateIdentifierValue(ctx, expr->ident);
        if (immediate_value != 0) {
          value = immediate_value;
          return true;
        }
        return false;
      }
      auto value_it = ctx.const_value_ptrs.find(ptr);
      if (value_it != ctx.const_value_ptrs.end()) {
        value = value_it->second;
        return true;
      }
      if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
        auto global_it = global_const_values_.find(expr->ident);
        if (global_it != global_const_values_.end()) {
          value = global_it->second;
          return true;
        }
      }
      return false;
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, value);
      }
      return TryGetCompileTimeI32ExprInContext(expr->third.get(), ctx, value);
    }
    if (expr->kind != Expr::Kind::Binary || expr->left == nullptr || expr->right == nullptr) {
      return false;
    }
    if (expr->op == "&&" || expr->op == "||") {
      int lhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs)) {
        return false;
      }
      if (expr->op == "&&") {
        if (lhs == 0) {
          value = 0;
          return true;
        }
        int rhs = 0;
        if (!TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
          return false;
        }
        value = rhs != 0 ? 1 : 0;
        return true;
      }
      if (lhs != 0) {
        value = 1;
        return true;
      }
      int rhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
        return false;
      }
      value = rhs != 0 ? 1 : 0;
      return true;
    }
    if (expr->op == "??") {
      int lhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs)) {
        return false;
      }
      if (lhs != 0) {
        value = lhs;
        return true;
      }
      return TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, value);
    }
    int lhs = 0;
    int rhs = 0;
    if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs) ||
        !TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
      return false;
    }
    if (expr->op == "+") {
      value = lhs + rhs;
      return true;
    }
    if (expr->op == "-") {
      value = lhs - rhs;
      return true;
    }
    if (expr->op == "*") {
      value = lhs * rhs;
      return true;
    }
    if (expr->op == "/") {
      if (rhs == 0) {
        return false;
      }
      value = lhs / rhs;
      return true;
    }
    if (expr->op == "%") {
      if (rhs == 0) {
        return false;
      }
      value = lhs % rhs;
      return true;
    }
    if (expr->op == "&") {
      value = lhs & rhs;
      return true;
    }
    if (expr->op == "|") {
      value = lhs | rhs;
      return true;
    }
    if (expr->op == "^") {
      value = lhs ^ rhs;
      return true;
    }
    if (expr->op == "<<" || expr->op == ">>") {
      if (rhs < 0 || rhs > 31) {
        return false;
      }
      value = expr->op == "<<" ? (lhs << rhs) : (lhs >> rhs);
      return true;
    }
    if (expr->op == "==") {
      value = lhs == rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "!=") {
      value = lhs != rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "<") {
      value = lhs < rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "<=") {
      value = lhs <= rhs ? 1 : 0;
      return true;
    }
    if (expr->op == ">") {
      value = lhs > rhs ? 1 : 0;
      return true;
    }
    if (expr->op == ">=") {
      value = lhs >= rhs ? 1 : 0;
      return true;
    }
    return false;
  }

  bool IsCompileTimeKnownNonNilExprInContext(const Expr *expr, const FunctionContext &ctx) const {
    int const_value = 0;
    if (!TryGetCompileTimeI32ExprInContext(expr, ctx, const_value)) {
      return false;
    }
    return const_value != 0;
  }

  bool ValidateMessageSendArityExpr(const Expr *expr, std::string &error) const {
    if (expr == nullptr) {
      return true;
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
      case Expr::Kind::BoolLiteral:
      case Expr::Kind::NilLiteral:
      case Expr::Kind::Identifier:
      case Expr::Kind::BlockLiteral:
        return true;
      case Expr::Kind::Binary:
        return ValidateMessageSendArityExpr(expr->left.get(), error) &&
               ValidateMessageSendArityExpr(expr->right.get(), error);
      case Expr::Kind::Conditional:
        return ValidateMessageSendArityExpr(expr->left.get(), error) &&
               ValidateMessageSendArityExpr(expr->right.get(), error) &&
               ValidateMessageSendArityExpr(expr->third.get(), error);
      case Expr::Kind::Call:
        for (const auto &arg : expr->args) {
          if (!ValidateMessageSendArityExpr(arg.get(), error)) {
            return false;
          }
        }
        return true;
      case Expr::Kind::MessageSend:
        if (expr->args.size() > lowering_ir_boundary_.runtime_dispatch_arg_slots) {
          error = "message send exceeds runtime dispatch arg slots: got " + std::to_string(expr->args.size()) +
                  ", max " + std::to_string(lowering_ir_boundary_.runtime_dispatch_arg_slots) + " at " +
                  std::to_string(expr->line) + ":" + std::to_string(expr->column);
          return false;
        }
        if (!ValidateMessageSendArityExpr(expr->receiver.get(), error)) {
          return false;
        }
        for (const auto &arg : expr->args) {
          if (!ValidateMessageSendArityExpr(arg.get(), error)) {
            return false;
          }
        }
        return true;
    }
    return true;
  }

  bool ValidateMessageSendArityForClause(const ForClause &clause, std::string &error) const {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return true;
      case ForClause::Kind::Expr:
      case ForClause::Kind::Let:
      case ForClause::Kind::Assign:
        return ValidateMessageSendArityExpr(clause.value.get(), error);
    }
    return true;
  }

  bool ValidateMessageSendArityStmt(const Stmt *stmt, std::string &error) const {
    if (stmt == nullptr) {
      return true;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        return stmt->let_stmt == nullptr || ValidateMessageSendArityExpr(stmt->let_stmt->value.get(), error);
      case Stmt::Kind::Assign:
        return stmt->assign_stmt == nullptr || ValidateMessageSendArityExpr(stmt->assign_stmt->value.get(), error);
      case Stmt::Kind::Return:
        return stmt->return_stmt == nullptr || ValidateMessageSendArityExpr(stmt->return_stmt->value.get(), error);
      case Stmt::Kind::Expr:
        return stmt->expr_stmt == nullptr || ValidateMessageSendArityExpr(stmt->expr_stmt->value.get(), error);
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return true;
        }
        if (!ValidateMessageSendArityExpr(stmt->if_stmt->condition.get(), error)) {
          return false;
        }
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          if (!ValidateMessageSendArityStmt(then_stmt.get(), error)) {
            return false;
          }
        }
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          if (!ValidateMessageSendArityStmt(else_stmt.get(), error)) {
            return false;
          }
        }
        return true;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return true;
        }
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          if (!ValidateMessageSendArityStmt(loop_stmt.get(), error)) {
            return false;
          }
        }
        return ValidateMessageSendArityExpr(stmt->do_while_stmt->condition.get(), error);
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return true;
        }
        if (!ValidateMessageSendArityForClause(stmt->for_stmt->init, error) ||
            !ValidateMessageSendArityExpr(stmt->for_stmt->condition.get(), error) ||
            !ValidateMessageSendArityForClause(stmt->for_stmt->step, error)) {
          return false;
        }
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          if (!ValidateMessageSendArityStmt(loop_stmt.get(), error)) {
            return false;
          }
        }
        return true;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return true;
        }
        if (!ValidateMessageSendArityExpr(stmt->switch_stmt->condition.get(), error)) {
          return false;
        }
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &case_body_stmt : case_stmt.body) {
            if (!ValidateMessageSendArityStmt(case_body_stmt.get(), error)) {
              return false;
            }
          }
        }
        return true;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return true;
        }
        if (!ValidateMessageSendArityExpr(stmt->while_stmt->condition.get(), error)) {
          return false;
        }
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          if (!ValidateMessageSendArityStmt(loop_stmt.get(), error)) {
            return false;
          }
        }
        return true;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return true;
        }
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          if (!ValidateMessageSendArityStmt(nested_stmt.get(), error)) {
            return false;
          }
        }
        return true;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return true;
    }
    return true;
  }

  bool ValidateMessageSendArityContract(std::string &error) const {
    for (const auto &global : program_.globals) {
      if (!ValidateMessageSendArityExpr(global.value.get(), error)) {
        return false;
      }
    }
    for (const auto &fn : program_.functions) {
      for (const auto &stmt : fn.body) {
        if (!ValidateMessageSendArityStmt(stmt.get(), error)) {
          return false;
        }
      }
    }
    for (const auto &implementation : program_.implementations) {
      for (const auto &method : implementation.methods) {
        if (!method.has_body) {
          continue;
        }
        for (const auto &stmt : method.body) {
          if (!ValidateMessageSendArityStmt(stmt.get(), error)) {
            return false;
          }
        }
      }
    }
    return true;
  }

  LoweredMessageSend LowerMessageSendHeader(const Expr *expr,
                                           FunctionContext &ctx) const {
    LoweredMessageSend lowered;
    lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, "0");
    if (expr == nullptr) {
      return lowered;
    }

    lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);
    lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);
    lowered.receiver = EmitExpr(expr->receiver.get(), ctx);
    lowered.selector = expr->selector;
    lowered.dispatch_surface_family = expr->dispatch_surface_family_symbol;
    lowered.dispatch_surface_entrypoint_family =
        expr->dispatch_surface_entrypoint_family_symbol;
    lowered.dispatch_symbol =
        Objc3DispatchSurfaceRuntimeEntrypointSymbol(
            lowered.dispatch_surface_family);
    return lowered;
  }

  void MaterializeMessageSendArgs(const Expr *expr, LoweredMessageSend &lowered,
                                  FunctionContext &ctx) const {
    if (expr == nullptr) {
      return;
    }
    for (std::size_t i = 0; i < expr->args.size() && i < lowered.args.size(); ++i) {
      lowered.args[i] = EmitExpr(expr->args[i].get(), ctx);
    }
  }

  LoweredMessageSend LowerMessageSendExpr(const Expr *expr, FunctionContext &ctx) const {
    LoweredMessageSend lowered = LowerMessageSendHeader(expr, ctx);
    MaterializeMessageSendArgs(expr, lowered, ctx);
    return lowered;
  }

  std::string EmitRuntimeDispatch(const LoweredMessageSend &lowered, FunctionContext &ctx) const {
    if (RequiresFailClosedObjc3RuntimeDispatchFallback(
            lowered.dispatch_surface_family)) {
      return EmitUnsupportedI32Value(
          "direct dispatch lowering remains unsupported on the live runtime path");
    }

    const bool uses_canonical_runtime_entrypoint =
        UsesCanonicalObjc3RuntimeDispatchEntrypoint(
            lowered.dispatch_surface_family);
    if (lowered.receiver_is_compile_time_zero &&
        !uses_canonical_runtime_entrypoint) {
      return "0";
    }

    auto selector_it = selector_pool_globals_.find(lowered.selector);
    if (selector_it == selector_pool_globals_.end()) {
      return EmitUnsupportedI32Value("missing selector global for message send selector '" + lowered.selector + "'");
    }

    const std::size_t selector_len = lowered.selector.size() + 1;
    const std::string selector_ptr = NewTemp(ctx);
    ctx.code_lines.push_back("  " + selector_ptr + " = getelementptr inbounds [" + std::to_string(selector_len) +
                             " x i8], ptr " + selector_it->second + ", i32 0, i32 0");

    const auto emit_dispatch_call = [&](const std::string &dispatch_value) {
      // M255-A001 dispatch-surface classification anchor: instance/class/super/dynamic
      // message sends that survive folding all route through the live runtime family;
      // direct dispatch remains an explicit non-goal for this freeze.
      // M255-B001 dispatch legality/selector-resolution freeze anchor: lowering
      // consumes normalized selector text only, preserves explicit receiver
      // legality, and does not attempt overload or ambiguity recovery beyond the
      // fail-closed frontend contract.
      // M255-B002 selector-resolution implementation anchor: once lane-B sema
      // resolves concrete self/super/known-class receivers, lowering still
      // emits the same live runtime entrypoint family and relies on the
      // fail-closed exact-signature result instead of performing its own
      // overload recovery.
      // M255-B003 super/direct/dynamic legality expansion anchor: lowering
      // continues to route admitted super/dynamic sites through the live
      // runtime family, preserves their normalized method-family metadata, and
      // never synthesizes a reserved direct-dispatch entrypoint.
      // M255-C001 dispatch lowering ABI freeze anchor: lowering still emits
      // the compatibility bridge symbol as the default call target while the
      // frozen lane-C ABI records the canonical runtime entrypoint,
      // selector-lookup surface, i32 receiver/result ABI, and fixed four-slot
      // argument vector that M255-C002 will cut over to explicitly.
      // M255-C002 runtime call ABI generation anchor: normalized instance/class
      // sends now call objc3_runtime_dispatch_i32 directly, while deferred
      // super/dynamic sites preserve objc3_msgsend_i32 until M255-C003.
      // M255-C004 live-dispatch cutover anchor: supported dynamic sends now
      // join instance/class/super on objc3_runtime_dispatch_i32, nil semantics
      // for canonical surfaces stay runtime-owned, the compatibility symbol is
      // retained only as a non-emitted alias/test surface, and reserved direct
      // dispatch surfaces still fail closed before IR emission.
      // M255-E001 live-dispatch gate anchor: supported live sends must
      // continue to emit only objc3_runtime_dispatch_i32 calls here. The
      // compatibility shim remains exported only as evidence/test surface, and
      // E002 is the first issue allowed to replace shim-based smoke/closeout
      // assumptions with the integrated live-dispatch gate.
      // M255-E002 live-dispatch smoke/replay closeout anchor: smoke and replay
      // now treat canonical runtime dispatch evidence as authoritative, so
      // emitted live sends must continue to surface objc3_runtime_dispatch_i32
      // even for nil-result paths that return 0 through the runtime.
      // M255-D001 lookup/dispatch runtime freeze anchor: emitted IR still
      // targets only the canonical lookup/dispatch boundary and does not
      // materialize runtime selector-table, method-cache, or slow-path helper
      // symbols. Those runtime-owned details stay behind the frozen
      // objc3_runtime_lookup_selector / objc3_runtime_dispatch_i32 surface
      // until later lane-D issues extend them explicitly.
      std::ostringstream call;
      call << "  " << dispatch_value << " = call i32 @" << lowered.dispatch_symbol << "(i32 "
           << lowered.receiver << ", ptr " << selector_ptr;
      for (const std::string &arg : lowered.args) {
        call << ", i32 " << arg;
      }
      call << ")";
      runtime_dispatch_call_emitted_ = true;
      ctx.code_lines.push_back(call.str());
    };

    if (uses_canonical_runtime_entrypoint ||
        lowered.receiver_is_compile_time_nonzero) {
      const std::string dispatch_value = NewTemp(ctx);
      emit_dispatch_call(dispatch_value);
      InvalidateGlobalProofState(ctx);
      return dispatch_value;
    }

    const std::string is_nil = NewTemp(ctx);
    const std::string nil_label = NewLabel(ctx, "msg_nil_");
    const std::string dispatch_label = NewLabel(ctx, "msg_dispatch_");
    const std::string merge_label = NewLabel(ctx, "msg_merge_");
    const std::string dispatch_value = NewTemp(ctx);
    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + is_nil + " = icmp eq i32 " + lowered.receiver + ", 0");
    ctx.code_lines.push_back("  br i1 " + is_nil + ", label %" + nil_label + ", label %" + dispatch_label);
    ctx.code_lines.push_back(nil_label + ":");
    ctx.code_lines.push_back("  br label %" + merge_label);
    ctx.code_lines.push_back(dispatch_label + ":");
    emit_dispatch_call(dispatch_value);
    ctx.code_lines.push_back("  br label %" + merge_label);
    ctx.code_lines.push_back(merge_label + ":");
    ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label + "], [" + dispatch_value + ", %" +
                             dispatch_label + "]");
    InvalidateGlobalProofState(ctx);
    return out;
  }

  std::string EmitMessageSendExpr(const Expr *expr, FunctionContext &ctx) const {
    if (expr != nullptr && expr->optional_send_enabled) {
      LoweredMessageSend lowered = LowerMessageSendHeader(expr, ctx);
      if (lowered.receiver_is_compile_time_zero) {
        return "0";
      }
      if (lowered.receiver_is_compile_time_nonzero) {
        MaterializeMessageSendArgs(expr, lowered, ctx);
        return EmitRuntimeDispatch(lowered, ctx);
      }

      const std::string is_nil = NewTemp(ctx);
      const std::string nil_label = NewLabel(ctx, "opt_send_nil_");
      const std::string dispatch_label = NewLabel(ctx, "opt_send_dispatch_");
      const std::string merge_label = NewLabel(ctx, "opt_send_merge_");
      const std::string out = NewTemp(ctx);

      ctx.code_lines.push_back("  " + is_nil + " = icmp eq i32 " +
                               lowered.receiver + ", 0");
      ctx.code_lines.push_back("  br i1 " + is_nil + ", label %" + nil_label +
                               ", label %" + dispatch_label);
      ctx.code_lines.push_back(nil_label + ":");
      ctx.code_lines.push_back("  br label %" + merge_label);
      ctx.code_lines.push_back(dispatch_label + ":");
      lowered.receiver_is_compile_time_zero = false;
      lowered.receiver_is_compile_time_nonzero = true;
      MaterializeMessageSendArgs(expr, lowered, ctx);
      const std::string dispatch_value = EmitRuntimeDispatch(lowered, ctx);
      ctx.code_lines.push_back("  br label %" + merge_label);
      ctx.code_lines.push_back(merge_label + ":");
      ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label +
                               "], [" + dispatch_value + ", %" +
                               dispatch_label + "]");
      return out;
    }
    const LoweredMessageSend lowered = LowerMessageSendExpr(expr, ctx);
    return EmitRuntimeDispatch(lowered, ctx);
  }

  std::string EmitExpr(const Expr *expr, FunctionContext &ctx) const {
    if (expr == nullptr) {
      return EmitUnsupportedI32Value("null expression reached IR lowering");
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
        return std::to_string(expr->number);
      case Expr::Kind::BoolLiteral:
        return expr->bool_value ? "1" : "0";
      case Expr::Kind::NilLiteral:
        return "0";
      case Expr::Kind::BlockLiteral:
        if (BlockLiteralSupportsEscapingRuntimeHookLowering(*expr)) {
          const std::string storage_ptr = EmitBlockLiteralStorage(*expr, ctx);
          if (storage_ptr == "poison") {
            return storage_ptr;
          }
          return EmitPromotedBlockHandle(*expr, storage_ptr, ctx);
        }
        return EmitUnsupportedI32Value(
            "block literal values must be bound to a local name before use");
      case Expr::Kind::Identifier: {
        if (expr->typed_keypath_literal_enabled) {
          return EmitTypedKeyPathLiteralValue(*expr);
        }
        return EmitIdentifierValue(expr->ident, ctx);
      }
      case Expr::Kind::Binary: {
        if (expr->op == "&&" || expr->op == "||") {
          const std::string lhs = EmitExpr(expr->left.get(), ctx);
          const std::string lhs_i1 = NewTemp(ctx);
          const std::string rhs_label = NewLabel(ctx, expr->op == "&&" ? "and_rhs_" : "or_rhs_");
          const std::string rhs_done_label = NewLabel(ctx, expr->op == "&&" ? "and_rhs_done_" : "or_rhs_done_");
          const std::string short_label = NewLabel(ctx, expr->op == "&&" ? "and_short_" : "or_short_");
          const std::string merge_label = NewLabel(ctx, expr->op == "&&" ? "and_merge_" : "or_merge_");
          const std::string rhs_i1 = NewTemp(ctx);
          const std::string logical_i1 = NewTemp(ctx);
          const std::string out_i32 = NewTemp(ctx);
          const std::string short_value = expr->op == "&&" ? "0" : "1";

          ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs + ", 0");
          if (expr->op == "&&") {
            ctx.code_lines.push_back(
                "  br i1 " + lhs_i1 + ", label %" + rhs_label + ", label %" + short_label);
          } else {
            ctx.code_lines.push_back(
                "  br i1 " + lhs_i1 + ", label %" + short_label + ", label %" + rhs_label);
          }

          ctx.code_lines.push_back(rhs_label + ":");
          const std::string rhs = EmitExpr(expr->right.get(), ctx);
          ctx.code_lines.push_back("  br label %" + rhs_done_label);
          ctx.code_lines.push_back(rhs_done_label + ":");
          ctx.code_lines.push_back("  " + rhs_i1 + " = icmp ne i32 " + rhs + ", 0");
          ctx.code_lines.push_back("  br label %" + merge_label);

          ctx.code_lines.push_back(short_label + ":");
          ctx.code_lines.push_back("  br label %" + merge_label);

          ctx.code_lines.push_back(merge_label + ":");
          ctx.code_lines.push_back("  " + logical_i1 + " = phi i1 [" + short_value + ", %" + short_label +
                                   "], [" + rhs_i1 + ", %" + rhs_done_label + "]");
          ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " + logical_i1 + " to i32");
          return out_i32;
        }
        if (expr->op == "??") {
          const std::string lhs = EmitExpr(expr->left.get(), ctx);
          const std::string lhs_i1 = NewTemp(ctx);
          const std::string rhs_label = NewLabel(ctx, "coalesce_rhs_");
          const std::string lhs_label = NewLabel(ctx, "coalesce_lhs_");
          const std::string merge_label = NewLabel(ctx, "coalesce_merge_");
          const std::string rhs_value_name = NewTemp(ctx);
          const std::string out_value = NewTemp(ctx);
          ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs +
                                   ", 0");
          ctx.code_lines.push_back("  br i1 " + lhs_i1 + ", label %" +
                                   lhs_label + ", label %" + rhs_label);
          ctx.code_lines.push_back(rhs_label + ":");
          const std::string rhs = EmitExpr(expr->right.get(), ctx);
          ctx.code_lines.push_back("  " + rhs_value_name + " = add i32 " + rhs +
                                   ", 0");
          ctx.code_lines.push_back("  br label %" + merge_label);
          ctx.code_lines.push_back(lhs_label + ":");
          ctx.code_lines.push_back("  br label %" + merge_label);
          ctx.code_lines.push_back(merge_label + ":");
          ctx.code_lines.push_back("  " + out_value + " = phi i32 [" + lhs +
                                   ", %" + lhs_label + "], [" + rhs_value_name +
                                   ", %" + rhs_label + "]");
          return out_value;
        }

        const std::string lhs = EmitExpr(expr->left.get(), ctx);
        const std::string rhs = EmitExpr(expr->right.get(), ctx);
        if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
          const std::string tmp = NewTemp(ctx);
          std::string op = "add";
          if (expr->op == "+") {
            op = "add";
          } else if (expr->op == "-") {
            op = "sub";
          } else if (expr->op == "*") {
            op = "mul";
          } else if (expr->op == "/") {
            op = "sdiv";
          } else if (expr->op == "%") {
            op = "srem";
          }
          ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs + ", " + rhs);
          return tmp;
        }

        if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
          const std::string tmp = NewTemp(ctx);
          std::string op = "and";
          if (expr->op == "&") {
            op = "and";
          } else if (expr->op == "|") {
            op = "or";
          } else if (expr->op == "^") {
            op = "xor";
          } else if (expr->op == "<<") {
            op = "shl";
          } else if (expr->op == ">>") {
            op = "ashr";
          }
          ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs + ", " + rhs);
          return tmp;
        }

        std::string pred;
        if (expr->op == "==") {
          pred = "eq";
        } else if (expr->op == "!=") {
          pred = "ne";
        } else if (expr->op == "<") {
          pred = "slt";
        } else if (expr->op == "<=") {
          pred = "sle";
        } else if (expr->op == ">") {
          pred = "sgt";
        } else if (expr->op == ">=") {
          pred = "sge";
        } else {
          return EmitUnsupportedI32Value("unsupported binary operator '" + expr->op + "'");
        }
        const std::string cmp_i1 = NewTemp(ctx);
        const std::string out_i32 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cmp_i1 + " = icmp " + pred + " i32 " + lhs + ", " + rhs);
        ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " + cmp_i1 + " to i32");
        return out_i32;
      }
      case Expr::Kind::Conditional: {
        const std::string cond_value = EmitExpr(expr->left.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        const std::string true_label = NewLabel(ctx, "cond_true_");
        const std::string false_label = NewLabel(ctx, "cond_false_");
        const std::string merge_label = NewLabel(ctx, "cond_merge_");
        const std::string result_ptr = "%cond.addr." + std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + result_ptr + " = alloca i32, align 4");
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond_value + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + true_label + ", label %" + false_label);

        ctx.code_lines.push_back(true_label + ":");
        const std::string true_value = EmitExpr(expr->right.get(), ctx);
        ctx.code_lines.push_back("  store i32 " + true_value + ", ptr " + result_ptr + ", align 4");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(false_label + ":");
        const std::string false_value = EmitExpr(expr->third.get(), ctx);
        ctx.code_lines.push_back("  store i32 " + false_value + ", ptr " + result_ptr + ", align 4");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(merge_label + ":");
        const std::string out_value = NewTemp(ctx);
        ctx.code_lines.push_back("  " + out_value + " = load i32, ptr " + result_ptr + ", align 4");
        return out_value;
      }
      case Expr::Kind::Call: {
        const auto local_block_it = ctx.block_bindings.find(expr->ident);
        if (local_block_it != ctx.block_bindings.end()) {
          return EmitBlockInvokeCall(local_block_it->second, expr, ctx);
        }
        const LoweredFunctionSignature *signature = LookupFunctionSignature(expr->ident);
        std::vector<std::string> args;
        args.reserve(expr->args.size());
        for (std::size_t i = 0; i < expr->args.size(); ++i) {
          const std::string arg_i32 = EmitExpr(expr->args[i].get(), ctx);
          const ValueType expected_type =
              signature != nullptr && i < signature->param_types.size() ? signature->param_types[i] : ValueType::I32;
          AppendLoweredCallArg(args, arg_i32, expected_type, ctx);
        }
        std::ostringstream arglist;
        for (std::size_t i = 0; i < args.size(); ++i) {
          if (i != 0) {
            arglist << ", ";
          }
          arglist << args[i];
        }
        const ValueType return_type = signature != nullptr ? signature->return_type : ValueType::I32;
        const std::string llvm_return_type = LLVMScalarType(return_type);
        const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);
        if (return_type == ValueType::Void) {
          ctx.code_lines.push_back("  call " + llvm_return_type + " @" + expr->ident + "(" + arglist.str() + ")");
          if (call_may_have_global_side_effects) {
            InvalidateGlobalProofState(ctx);
          }
          return "0";
        }
        const std::string tmp = NewTemp(ctx);
        ctx.code_lines.push_back("  " + tmp + " = call " + llvm_return_type + " @" + expr->ident + "(" +
                                 arglist.str() + ")");
        const std::string out = CoerceValueToI32(tmp, return_type, ctx);
        if (call_may_have_global_side_effects) {
          InvalidateGlobalProofState(ctx);
        }
        return out;
      }
      case Expr::Kind::MessageSend: {
        return EmitMessageSendExpr(expr, ctx);
      }
    }
    return EmitUnsupportedI32Value("unsupported expression kind reached IR lowering");
  }

  std::string EmitUnsupportedI32Value(const std::string &reason) const {
    if (!fail_open_fallback_triggered_) {
      fail_open_fallback_triggered_ = true;
      fail_open_fallback_reason_ = reason;
    }
    return "poison";
  }

  void EmitStatement(const Stmt *stmt, FunctionContext &ctx) const {
    if (stmt == nullptr || ctx.terminated) {
      return;
    }

    switch (stmt->kind) {
      case Stmt::Kind::Let: {
        const LetStmt *let = stmt->let_stmt.get();
        if (let == nullptr || ctx.scopes.empty()) {
          return;
        }
        if (let->value != nullptr && let->value->kind == Expr::Kind::BlockLiteral) {
          const std::string storage_ptr = EmitBlockLiteralStorage(*let->value, ctx);
          if (storage_ptr == "poison") {
            return;
          }
          ctx.block_bindings[let->name] =
              BlockBinding{storage_ptr, let->value.get(), ""};
          return;
        }
        if (let->value != nullptr && let->value->kind == Expr::Kind::Identifier) {
          const auto existing_block_it =
              ctx.block_bindings.find(let->value->ident);
          if (existing_block_it != ctx.block_bindings.end()) {
            ctx.block_bindings[let->name] = existing_block_it->second;
            return;
          }
        }
        // Evaluate the initializer against the currently visible scope first so
        // shadowing declarations can read the previous binding deterministically.
        const std::string value = EmitExpr(let->value.get(), ctx);
        int let_const_value = 0;
        const bool has_let_const_value = TryGetCompileTimeI32ExprInContext(let->value.get(), ctx, let_const_value);
        const bool has_let_nil_value = IsCompileTimeNilReceiverExprInContext(let->value.get(), ctx);
        const std::string ptr = "%" + let->name + ".addr." + std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
        ctx.scopes.back()[let->name] = ptr;
        if (has_let_nil_value) {
          ctx.nil_bound_ptrs.insert(ptr);
        }
        if (has_let_const_value) {
          ctx.const_value_ptrs[ptr] = let_const_value;
        }
        if (has_let_const_value && let_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
        ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
        return;
      }
      case Stmt::Kind::Return: {
        const ReturnStmt *ret = stmt->return_stmt.get();
        if (ret == nullptr) {
          return;
        }
        if (ret->value == nullptr) {
          EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
          EmitTypedReturn("0", ctx);
        } else {
          const std::string value = EmitExpr(ret->value.get(), ctx);
          EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
          EmitTypedReturn(value, ctx);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Assign: {
        const AssignStmt *assign = stmt->assign_stmt.get();
        if (assign == nullptr) {
          return;
        }
        const std::string ptr = LookupVarPtr(ctx, assign->name);
        if (ptr.empty() && ctx.block_bindings.find(assign->name) != ctx.block_bindings.end()) {
          (void)EmitUnsupportedI32Value(
              "reassigning block values is not yet runnable in Objective-C 3 native mode");
          return;
        }
        EmitAssignmentStore(ptr, assign->op, assign->value.get(), ctx);
        return;
      }
      case Stmt::Kind::Break: {
        if (ctx.control_stack.empty()) {
          EmitTerminalCleanupToDepth(ctx, 0u, 0u, 0u, 0u);
          ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
        } else {
          EmitTerminalCleanupToDepth(
              ctx, ctx.control_stack.back().scope_depth,
              ctx.control_stack.back().autoreleasepool_depth,
              ctx.control_stack.back().pending_block_dispose_depth,
              ctx.control_stack.back().arc_cleanup_depth);
          ctx.code_lines.push_back("  br label %" + ctx.control_stack.back().break_label);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Continue: {
        std::string continue_label;
        std::size_t continue_autoreleasepool_depth =
            ctx.autoreleasepool_scope_symbols.size();
        for (auto it = ctx.control_stack.rbegin(); it != ctx.control_stack.rend(); ++it) {
          if (it->continue_allowed) {
            continue_label = it->continue_label;
            continue_autoreleasepool_depth = it->autoreleasepool_depth;
            break;
          }
        }
        if (continue_label.empty()) {
          EmitTerminalCleanupToDepth(ctx, 0u, 0u, 0u, 0u);
          ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
        } else {
          const ControlLabels &target = *std::find_if(
              ctx.control_stack.rbegin(), ctx.control_stack.rend(),
              [&continue_label](const ControlLabels &labels) {
                return labels.continue_allowed &&
                       labels.continue_label == continue_label;
              });
          EmitTerminalCleanupToDepth(
              ctx, target.scope_depth, continue_autoreleasepool_depth,
              target.pending_block_dispose_depth, target.arc_cleanup_depth);
          ctx.code_lines.push_back("  br label %" + continue_label);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Empty:
        return;
      case Stmt::Kind::Block: {
        const BlockStmt *block_stmt = stmt->block_stmt.get();
        if (block_stmt == nullptr) {
          return;
        }
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
        PopScope(ctx, !ctx.terminated);
        if (!ctx.terminated) {
          EmitAutoreleasepoolUnwindToDepth(ctx, autoreleasepool_depth);
        }
        return;
      }
      case Stmt::Kind::Defer: {
        const BlockStmt *block_stmt = stmt->block_stmt.get();
        if (block_stmt == nullptr || ctx.pending_defer_scope_blocks.empty()) {
          return;
        }
        ctx.pending_defer_scope_blocks.back().push_back(block_stmt);
        return;
      }
      case Stmt::Kind::Expr: {
        const ExprStmt *expr_stmt = stmt->expr_stmt.get();
        if (expr_stmt != nullptr) {
          (void)EmitExpr(expr_stmt->value.get(), ctx);
        }
        return;
      }
      case Stmt::Kind::While: {
        const WhileStmt *while_stmt = stmt->while_stmt.get();
        if (while_stmt == nullptr) {
          return;
        }

        const std::string cond_label = NewLabel(ctx, "while_cond_");
        const std::string body_label = NewLabel(ctx, "while_body_");
        const std::string end_label = NewLabel(ctx, "while_end_");
        ctx.code_lines.push_back("  br label %" + cond_label);

        ctx.code_lines.push_back(cond_label + ":");
        const std::string cond = EmitExpr(while_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);

        ctx.code_lines.push_back(body_label + ":");
        PushScope(ctx);
        ctx.control_stack.push_back(
            {cond_label, end_label, true, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
        ctx.terminated = false;
        for (const auto &s : while_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        PopScope(ctx, !body_terminated);
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + cond_label);
        }
        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::DoWhile: {
        const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
        if (do_while_stmt == nullptr) {
          return;
        }

        const std::string body_label = NewLabel(ctx, "do_body_");
        const std::string cond_label = NewLabel(ctx, "do_cond_");
        const std::string end_label = NewLabel(ctx, "do_end_");
        ctx.code_lines.push_back("  br label %" + body_label);

        ctx.code_lines.push_back(body_label + ":");
        PushScope(ctx);
        ctx.control_stack.push_back(
            {cond_label, end_label, true, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
        ctx.terminated = false;
        for (const auto &s : do_while_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        PopScope(ctx, !body_terminated);
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + cond_label);
        }

        ctx.code_lines.push_back(cond_label + ":");
        const std::string cond = EmitExpr(do_while_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);

        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::For: {
        const ForStmt *for_stmt = stmt->for_stmt.get();
        if (for_stmt == nullptr) {
          return;
        }

        PushScope(ctx);
        EmitForClause(for_stmt->init, ctx);

        const std::string cond_label = NewLabel(ctx, "for_cond_");
        const std::string body_label = NewLabel(ctx, "for_body_");
        const std::string step_label = NewLabel(ctx, "for_step_");
        const std::string end_label = NewLabel(ctx, "for_end_");

        ctx.code_lines.push_back("  br label %" + cond_label);
        ctx.code_lines.push_back(cond_label + ":");
        if (for_stmt->condition == nullptr) {
          ctx.code_lines.push_back("  br label %" + body_label);
        } else {
          const std::string cond = EmitExpr(for_stmt->condition.get(), ctx);
          const std::string cond_i1 = NewTemp(ctx);
          ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
          ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);
        }

        ctx.code_lines.push_back(body_label + ":");
        PushScope(ctx);
        ctx.control_stack.push_back(
            {step_label, end_label, true, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
        ctx.terminated = false;
        for (const auto &s : for_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        PopScope(ctx, !body_terminated);
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + step_label);
        }

        ctx.code_lines.push_back(step_label + ":");
        EmitForClause(for_stmt->step, ctx);
        ctx.code_lines.push_back("  br label %" + cond_label);

        ctx.code_lines.push_back(end_label + ":");
        PopScope(ctx, true);
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::Switch: {
        const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
        if (switch_stmt == nullptr) {
          return;
        }

        const std::string condition_value = EmitExpr(switch_stmt->condition.get(), ctx);
        const std::string end_label = NewLabel(ctx, "switch_end_");

        if (switch_stmt->match_surface_enabled) {
          // M266-C003 match lowering anchor: statement-form match now lowers
          // literal/default/wildcard/binding arms as a distinct control-flow
          // carrier with case-local binding storage and no switch-style
          // fallthrough, while result-case payload matching remains fail-closed
          // until a runtime Result ABI exists.
          std::vector<std::string> arm_labels;
          arm_labels.reserve(switch_stmt->cases.size());
          for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
            const SwitchCase &case_stmt = switch_stmt->cases[i];
            arm_labels.push_back(case_stmt.is_default
                                     ? NewLabel(ctx, "match_default_")
                                     : NewLabel(ctx, "match_case_"));
          }

          if (!switch_stmt->cases.empty()) {
            std::vector<std::string> test_labels;
            test_labels.reserve(switch_stmt->cases.size());
            for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
              test_labels.push_back(NewLabel(ctx, "match_test_"));
            }
            ctx.code_lines.push_back("  br label %" + test_labels.front());
            for (std::size_t case_index = 0; case_index < switch_stmt->cases.size(); ++case_index) {
              const SwitchCase &case_stmt = switch_stmt->cases[case_index];
              const std::string next_label =
                  case_index + 1u < switch_stmt->cases.size() ? test_labels[case_index + 1u] : end_label;
              ctx.code_lines.push_back(test_labels[case_index] + ":");
              if (case_stmt.is_default ||
                  case_stmt.match_pattern_kind == MatchPatternKind::Wildcard ||
                  case_stmt.match_pattern_kind == MatchPatternKind::Binding) {
                ctx.code_lines.push_back("  br label %" + arm_labels[case_index]);
                continue;
              }
              if (case_stmt.match_pattern_kind == MatchPatternKind::LiteralInteger ||
                  case_stmt.match_pattern_kind == MatchPatternKind::LiteralBool ||
                  case_stmt.match_pattern_kind == MatchPatternKind::LiteralNil) {
                const std::string cmp = NewTemp(ctx);
                ctx.code_lines.push_back("  " + cmp + " = icmp eq i32 " + condition_value + ", " +
                                         std::to_string(case_stmt.value));
                ctx.code_lines.push_back("  br i1 " + cmp + ", label %" + arm_labels[case_index] +
                                         ", label %" + next_label);
                continue;
              }
              if (case_stmt.match_pattern_kind == MatchPatternKind::ResultCase) {
                EmitUnsupportedI32Value(
                    "result-case match lowering remains fail-closed until a runtime Result payload ABI lands");
                return;
              }
              EmitUnsupportedI32Value("unsupported match pattern reached IR lowering");
              return;
            }
          } else {
            ctx.code_lines.push_back("  br label %" + end_label);
          }

          for (std::size_t arm_index = 0; arm_index < switch_stmt->cases.size(); ++arm_index) {
            const SwitchCase &case_stmt = switch_stmt->cases[arm_index];
            ctx.code_lines.push_back(arm_labels[arm_index] + ":");
            PushScope(ctx);
            if (!case_stmt.match_binding_name.empty() &&
                (case_stmt.match_pattern_kind == MatchPatternKind::Binding)) {
              const std::string ptr =
                  "%" + case_stmt.match_binding_name + ".addr." + std::to_string(ctx.temp_counter++);
              ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
              ctx.code_lines.push_back("  store i32 " + condition_value + ", ptr " + ptr + ", align 4");
              ctx.scopes.back()[case_stmt.match_binding_name] = ptr;
            }
            ctx.control_stack.push_back(
                {"", end_label, false, ctx.scopes.size() - 1u,
                 ctx.autoreleasepool_scope_symbols.size(),
                 ctx.pending_block_dispose_calls.size(),
                 ctx.arc_owned_cleanup_ptrs.size()});
            ctx.terminated = false;
            for (const auto &case_body_stmt : case_stmt.body) {
              EmitStatement(case_body_stmt.get(), ctx);
            }
            const bool arm_terminated = ctx.terminated;
            ctx.control_stack.pop_back();
            PopScope(ctx, !arm_terminated);

            if (!arm_terminated) {
              ctx.code_lines.push_back("  br label %" + end_label);
            }
          }

          ctx.code_lines.push_back(end_label + ":");
          ctx.terminated = false;
          return;
        }

        std::vector<std::string> arm_labels;
        arm_labels.reserve(switch_stmt->cases.size());
        std::vector<std::size_t> case_clause_indices;
        case_clause_indices.reserve(switch_stmt->cases.size());
        std::size_t default_index = switch_stmt->cases.size();

        for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
          const SwitchCase &case_stmt = switch_stmt->cases[i];
          if (case_stmt.is_default) {
            arm_labels.push_back(NewLabel(ctx, "switch_default_"));
            if (default_index == switch_stmt->cases.size()) {
              default_index = i;
            }
          } else {
            arm_labels.push_back(NewLabel(ctx, "switch_case_"));
            case_clause_indices.push_back(i);
          }
        }

        const std::string default_label =
            default_index < switch_stmt->cases.size() ? arm_labels[default_index] : end_label;

        if (!case_clause_indices.empty()) {
          std::vector<std::string> test_labels;
          test_labels.reserve(case_clause_indices.size());
          for (std::size_t i = 0; i < case_clause_indices.size(); ++i) {
            test_labels.push_back(NewLabel(ctx, "switch_test_"));
          }

          ctx.code_lines.push_back("  br label %" + test_labels[0]);
          for (std::size_t test_index = 0; test_index < case_clause_indices.size(); ++test_index) {
            const std::size_t case_index = case_clause_indices[test_index];
            const std::string next_label =
                (test_index + 1 < case_clause_indices.size()) ? test_labels[test_index + 1] : default_label;

            ctx.code_lines.push_back(test_labels[test_index] + ":");
            const std::string cmp = NewTemp(ctx);
            ctx.code_lines.push_back("  " + cmp + " = icmp eq i32 " + condition_value + ", " +
                                     std::to_string(switch_stmt->cases[case_index].value));
            ctx.code_lines.push_back("  br i1 " + cmp + ", label %" + arm_labels[case_index] + ", label %" +
                                     next_label);
          }
        } else {
          ctx.code_lines.push_back("  br label %" + default_label);
        }

        for (std::size_t arm_index = 0; arm_index < switch_stmt->cases.size(); ++arm_index) {
          const SwitchCase &case_stmt = switch_stmt->cases[arm_index];
          ctx.code_lines.push_back(arm_labels[arm_index] + ":");
        PushScope(ctx);
        ctx.control_stack.push_back(
            {"", end_label, false, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
          ctx.terminated = false;
          for (const auto &case_body_stmt : case_stmt.body) {
            EmitStatement(case_body_stmt.get(), ctx);
          }
          const bool arm_terminated = ctx.terminated;
          ctx.control_stack.pop_back();
          PopScope(ctx, !arm_terminated);

          if (!arm_terminated) {
            if (arm_index + 1 < switch_stmt->cases.size()) {
              ctx.code_lines.push_back("  br label %" + arm_labels[arm_index + 1]);
            } else {
              ctx.code_lines.push_back("  br label %" + end_label);
            }
          }
        }

        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::If: {
        const IfStmt *if_stmt = stmt->if_stmt.get();
        if (if_stmt == nullptr) {
          return;
        }
        if (if_stmt->optional_binding_surface_enabled ||
            if_stmt->guard_condition_list_surface_enabled) {
          EmitOptionalBindingIfStatement(if_stmt, ctx);
          return;
        }

        const std::string cond = EmitExpr(if_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        const std::string then_label = NewLabel(ctx, "if_then_");
        const std::string else_label = NewLabel(ctx, "if_else_");
        const std::string merge_label = NewLabel(ctx, "if_end_");

        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + then_label + ", label %" + else_label);

        ctx.code_lines.push_back(then_label + ":");
        PushScope(ctx);
        ctx.terminated = false;
        for (const auto &s : if_stmt->then_body) {
          EmitStatement(s.get(), ctx);
        }
        const bool then_terminated = ctx.terminated;
        PopScope(ctx, !then_terminated);
        if (!then_terminated) {
          ctx.code_lines.push_back("  br label %" + merge_label);
        }

        ctx.code_lines.push_back(else_label + ":");
        PushScope(ctx);
        ctx.terminated = false;
        for (const auto &s : if_stmt->else_body) {
          EmitStatement(s.get(), ctx);
        }
        const bool else_terminated = ctx.terminated;
        PopScope(ctx, !else_terminated);
        if (!else_terminated) {
          ctx.code_lines.push_back("  br label %" + merge_label);
        }

        if (then_terminated && else_terminated) {
          ctx.terminated = true;
        } else {
          ctx.code_lines.push_back(merge_label + ":");
          ctx.terminated = false;
        }
        return;
      }
    }
  }

  void InvalidateGlobalProofState(FunctionContext &ctx) const {
    ctx.global_proofs_invalidated = true;
    for (auto it = ctx.nil_bound_ptrs.begin(); it != ctx.nil_bound_ptrs.end();) {
      if (it->rfind("@", 0) == 0) {
        it = ctx.nil_bound_ptrs.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = ctx.nonzero_bound_ptrs.begin(); it != ctx.nonzero_bound_ptrs.end();) {
      if (it->rfind("@", 0) == 0) {
        it = ctx.nonzero_bound_ptrs.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = ctx.const_value_ptrs.begin(); it != ctx.const_value_ptrs.end();) {
      if (it->first.rfind("@", 0) == 0) {
        it = ctx.const_value_ptrs.erase(it);
      } else {
        ++it;
      }
    }
  }

  void EmitPrototypeDeclarations(std::ostringstream &out) const {
    bool emitted = false;
    std::unordered_set<std::string> declared_symbols;
    const auto requires_arc_helper_declarations = [&]() {
      for (const auto &fn : program_.functions) {
        if (EffectiveArcReturnInsertRetain(
                fn, frontend_metadata_.arc_mode_enabled) ||
            fn.return_ownership_insert_release ||
            EffectiveArcReturnInsertAutorelease(fn)) {
          return true;
        }
        for (const auto &param : fn.params) {
          if (EffectiveArcParamInsertRetain(
                  param, frontend_metadata_.arc_mode_enabled) ||
              EffectiveArcParamInsertRelease(
                  param, frontend_metadata_.arc_mode_enabled) ||
              param.ownership_insert_autorelease) {
            return true;
          }
        }
      }
      for (const auto &method_def : method_definitions_) {
        if (method_def.method == nullptr) {
          continue;
        }
        const Objc3MethodDecl &method = *method_def.method;
        if (EffectiveArcReturnInsertRetain(
                method, frontend_metadata_.arc_mode_enabled) ||
            method.return_ownership_insert_release ||
            EffectiveArcReturnInsertAutorelease(method)) {
          return true;
        }
        for (const auto &param : method.params) {
          if (EffectiveArcParamInsertRetain(
                  param, frontend_metadata_.arc_mode_enabled) ||
              EffectiveArcParamInsertRelease(
                  param, frontend_metadata_.arc_mode_enabled) ||
              param.ownership_insert_autorelease) {
            return true;
          }
        }
      }
      return false;
    };
    const auto emit_declaration_once =
        [&](const std::string &symbol, const std::string &declaration) {
          if (symbol.empty() || !declared_symbols.insert(symbol).second) {
            return false;
          }
          out << declaration;
          emitted = true;
          return true;
        };
    if (ShouldEmitRuntimeBootstrapLowering()) {
      emit_declaration_once(
          kObjc3RuntimeBootstrapStageRegistrationTableSymbol,
          "declare void @" +
              std::string(kObjc3RuntimeBootstrapStageRegistrationTableSymbol) +
              "(ptr)\n");
      emit_declaration_once(
          frontend_metadata_
              .runtime_bootstrap_lowering_registration_entrypoint_symbol,
          "declare i32 @" +
              frontend_metadata_
                  .runtime_bootstrap_lowering_registration_entrypoint_symbol +
              "(ptr)\n");
      emit_declaration_once("abort", "declare void @abort()\n");
    }
    if (!selector_pool_globals_.empty()) {
      const auto emit_runtime_dispatch_declaration =
          [&](const std::string &symbol) {
            if (symbol.empty() ||
                !declared_symbols.insert(symbol).second) {
              return false;
            }
            out << "declare i32 @" << symbol << "(i32, ptr";
            for (std::size_t i = 0; i < lowering_ir_boundary_.runtime_dispatch_arg_slots;
                 ++i) {
              out << ", i32";
            }
            out << ")\n";
            emitted = true;
            return true;
          };
      emit_runtime_dispatch_declaration(
          kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol);
    }
    if (synthesized_property_accessor_count_ > 0u ||
        requires_arc_helper_declarations() ||
        frontend_metadata_.autoreleasepool_scope_lowering_scope_sites > 0u ||
        frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites >
            0u ||
        frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites > 0u ||
        frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites > 0u) {
      emit_declaration_once(kObjc3RuntimeReadCurrentPropertyI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeReadCurrentPropertyI32Symbol) +
                                "()\n");
      emit_declaration_once(kObjc3RuntimeWriteCurrentPropertyI32Symbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimeWriteCurrentPropertyI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeExchangeCurrentPropertyI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeExchangeCurrentPropertyI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol,
                            "declare i32 @" +
                                std::string(
                                    kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol) +
                                "()\n");
      emit_declaration_once(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeRetainI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeRetainI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeReleaseI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeReleaseI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimeAutoreleaseI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                                "(i32)\n");
      emit_declaration_once(kObjc3RuntimePromoteBlockI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimePromoteBlockI32Symbol) +
                                "(ptr, i64, i32)\n");
      emit_declaration_once(kObjc3RuntimeInvokeBlockI32Symbol,
                            "declare i32 @" +
                                std::string(kObjc3RuntimeInvokeBlockI32Symbol) +
                                "(i32, i32, i32, i32, i32)\n");
      emit_declaration_once(kObjc3RuntimePushAutoreleasepoolScopeSymbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimePushAutoreleasepoolScopeSymbol) +
                                "()\n");
      emit_declaration_once(kObjc3RuntimePopAutoreleasepoolScopeSymbol,
                            "declare void @" +
                                std::string(
                                    kObjc3RuntimePopAutoreleasepoolScopeSymbol) +
                                "()\n");
    }
    for (const auto &entry : function_signatures_) {
      if (defined_functions_.find(entry.first) != defined_functions_.end()) {
        continue;
      }
      if (!declared_symbols.insert(entry.first).second) {
        continue;
      }
      const LoweredFunctionSignature &signature = entry.second;
      std::ostringstream params;
      for (std::size_t i = 0; i < signature.param_types.size(); ++i) {
        if (i != 0) {
          params << ", ";
        }
        params << LLVMScalarType(signature.param_types[i]);
      }
      out << "declare " << LLVMScalarType(signature.return_type) << " @" << entry.first << "(" << params.str()
          << ")\n";
      emitted = true;
    }
    if (emitted) {
      out << "\n";
    }
  }

  void EmitRuntimeBootstrapLoweringFunctions(std::ostringstream &out) const {
    if (!ShouldEmitRuntimeBootstrapLowering()) {
      return;
    }

    const std::string init_stub_symbol =
        "@" + RuntimeBootstrapInitStubSymbol();
    const std::string ctor_root_symbol =
        "@" +
        frontend_metadata_.runtime_bootstrap_lowering_constructor_root_symbol;
    const std::string registration_table_symbol =
        "@" + RuntimeBootstrapRegistrationTableSymbol();
    const std::string register_image_symbol =
        "@" +
        frontend_metadata_.runtime_bootstrap_lowering_registration_entrypoint_symbol;

    out << "define internal void " << init_stub_symbol << "() {\n";
    out << "entry:\n";
    out << "  %bootstrap_state_slot = getelementptr inbounds "
        << RuntimeBootstrapRegistrationTableType() << ", ptr "
        << registration_table_symbol << ", i32 0, i32 13\n";
    out << "  %bootstrap_state_cell = load ptr, ptr %bootstrap_state_slot, align 8\n";
    out << "  %bootstrap_state = load i8, ptr %bootstrap_state_cell, align 1\n";
    out << "  %bootstrap_already_initialized = icmp ne i8 %bootstrap_state, 0\n";
    out << "  br i1 %bootstrap_already_initialized, label %bootstrap_success, label %bootstrap_register\n";
    out << "bootstrap_register:\n";
    out << "  call void @" << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
        << "(ptr " << registration_table_symbol << ")\n";
    out << "  %bootstrap_image_slot = getelementptr inbounds "
        << RuntimeBootstrapRegistrationTableType() << ", ptr "
        << registration_table_symbol << ", i32 0, i32 2\n";
    out << "  %bootstrap_image = load ptr, ptr %bootstrap_image_slot, align 8\n";
    out << "  %bootstrap_status = call i32 " << register_image_symbol
        << "(ptr %bootstrap_image)\n";
    out << "  %bootstrap_ok = icmp eq i32 %bootstrap_status, 0\n";
    out << "  br i1 %bootstrap_ok, label %bootstrap_success, label %bootstrap_fail\n";
    out << "bootstrap_fail:\n";
    out << "  call void @abort()\n";
    out << "  unreachable\n";
    out << "bootstrap_success:\n";
    out << "  store i8 1, ptr %bootstrap_state_cell, align 1\n";
    out << "  ret void\n";
    out << "}\n\n";

    out << "define internal void " << ctor_root_symbol << "() {\n";
    out << "entry:\n";
    out << "  call void " << init_stub_symbol << "()\n";
    out << "  ret void\n";
    out << "}\n\n";
  }

  void EmitFunction(const FunctionDecl &fn, std::ostringstream &out) const {
    std::ostringstream signature;
    for (std::size_t i = 0; i < fn.params.size(); ++i) {
      if (i != 0) {
        signature << ", ";
      }
      signature << LLVMScalarType(fn.params[i].type) << " %arg" << i;
    }

    out << "define " << LLVMScalarType(fn.return_type) << " @" << fn.name << "(" << signature.str() << ") {\n";
    out << "entry:\n";

    FunctionContext ctx;
    ctx.return_type = fn.return_type;
    ctx.arc_return_insert_retain =
        EffectiveArcReturnInsertRetain(fn, frontend_metadata_.arc_mode_enabled);
    ctx.arc_return_insert_autorelease =
        EffectiveArcReturnInsertAutorelease(fn);
    PushScope(ctx);
    SeedKnownClassReceiverBindings(ctx);

    for (std::size_t i = 0; i < fn.params.size(); ++i) {
      const auto &param = fn.params[i];
      const std::string ptr = "%" + param.name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      EmitTypedParamStore(param, i, ptr, ctx);
      ctx.scopes.back()[param.name] = ptr;
    }

    for (const auto &stmt : fn.body) {
      EmitStatement(stmt.get(), ctx);
      if (ctx.terminated) {
        break;
      }
    }

    if (!ctx.terminated) {
      EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
      EmitTypedReturn("0", ctx);
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }

    out << "}\n";
  }

  void EmitMethod(const MethodDefinition &method_def,
                  std::ostringstream &out) const {
    if (method_def.method == nullptr) {
      EmitSynthesizedAccessorMethod(method_def, out);
      return;
    }
    const Objc3MethodDecl &method = *method_def.method;
    std::ostringstream signature;
    for (std::size_t i = 0; i < method.params.size(); ++i) {
      if (i != 0) {
        signature << ", ";
      }
      signature << LLVMScalarType(method.params[i].type) << " %arg" << i;
    }

    out << "define " << LLVMScalarType(method.return_type) << " @"
        << method_def.symbol << "(" << signature.str() << ") {\n";
    out << "entry:\n";

    FunctionContext ctx;
    ctx.return_type = method.return_type;
    ctx.arc_return_insert_retain = EffectiveArcReturnInsertRetain(
        method, frontend_metadata_.arc_mode_enabled);
    ctx.arc_return_insert_autorelease =
        EffectiveArcReturnInsertAutorelease(method);
    PushScope(ctx);
    SeedKnownClassReceiverBindings(ctx);
    const int implementation_class_identity =
        LookupClassReceiverIdentityValue(method_def.implementation_name);
    const int self_identity =
        method.is_class_method
            ? BuildClassReceiverIdentityValue(implementation_class_identity)
            : BuildInstanceReceiverIdentityValue(implementation_class_identity);
    if (self_identity != 0) {
      ctx.immediate_identifiers["self"] = self_identity;
    }
    const int super_class_identity =
        LookupClassReceiverIdentityValue(method_def.superclass_name);
    const int super_identity =
        method.is_class_method
            ? BuildClassReceiverIdentityValue(super_class_identity)
            : BuildInstanceReceiverIdentityValue(super_class_identity);
    if (super_identity != 0) {
      ctx.immediate_identifiers["super"] = super_identity;
    }

    for (std::size_t i = 0; i < method.params.size(); ++i) {
      const auto &param = method.params[i];
      const std::string ptr = "%" + param.name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      EmitTypedParamStore(param, i, ptr, ctx);
      ctx.scopes.back()[param.name] = ptr;
    }

    for (const auto &stmt : method.body) {
      EmitStatement(stmt.get(), ctx);
      if (ctx.terminated) {
        break;
      }
    }

    if (!ctx.terminated) {
      EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
      EmitTypedReturn("0", ctx);
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }

    out << "}\n";
  }

  void EmitSynthesizedAccessorMethod(const MethodDefinition &method_def,
                                     std::ostringstream &out) const {
    if (method_def.synthesized_accessor_kind == SynthesizedAccessorKind::None ||
        method_def.synthesized_storage_symbol.empty()) {
      return;
    }
    const auto profile_contains = [](const std::string &profile,
                                     const char *needle) {
      return !profile.empty() && needle != nullptr &&
             profile.find(needle) != std::string::npos;
    };
    const bool uses_weak_runtime_hooks =
        method_def.synthesized_ownership_runtime_hook_profile ==
        "objc-weak-side-table";
    const bool uses_strong_runtime_hooks =
        method_def.synthesized_ownership_lifetime_profile == "strong-owned" ||
        profile_contains(method_def.synthesized_accessor_ownership_profile,
                         "ownership_lifetime=strong-owned");
    const char *llvm_value_type = LLVMScalarType(method_def.synthesized_value_type);
    if (method_def.synthesized_accessor_kind == SynthesizedAccessorKind::Getter) {
      out << "define " << llvm_value_type << " @" << method_def.symbol << "() {\n";
      out << "entry:\n";
      const std::string loaded_value = "%objc3_property_slot";
      out << "  " << loaded_value << " = call i32 @"
          << (uses_weak_runtime_hooks ? kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
                                      : kObjc3RuntimeReadCurrentPropertyI32Symbol)
          << "()\n";
      std::string returned_value = loaded_value;
      if (uses_strong_runtime_hooks) {
        const std::string retained_value = "%objc3_property_retained";
        const std::string autoreleased_value = "%objc3_property_autoreleased";
        out << "  " << retained_value << " = call i32 @"
            << kObjc3RuntimeRetainI32Symbol << "(i32 " << loaded_value << ")\n";
        out << "  " << autoreleased_value << " = call i32 @"
            << kObjc3RuntimeAutoreleaseI32Symbol << "(i32 " << retained_value
            << ")\n";
        returned_value = autoreleased_value;
      }
      if (method_def.synthesized_value_type == ValueType::Bool) {
        out << "  %objc3_property_value = icmp ne i32 %objc3_property_slot, 0\n";
        out << "  ret i1 %objc3_property_value\n";
      } else {
        out << "  ret i32 " << returned_value << "\n";
      }
      out << "}\n";
      return;
    }

    out << "define void @" << method_def.symbol << "(" << llvm_value_type
        << " %arg0) {\n";
    out << "entry:\n";
    std::string stored_value = "%arg0";
    if (method_def.synthesized_value_type == ValueType::Bool) {
      out << "  %objc3_property_value = zext i1 %arg0 to i32\n";
      stored_value = "%objc3_property_value";
    }
    if (uses_weak_runtime_hooks) {
      out << "  call void @" << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
          << "(i32 " << stored_value << ")\n";
    } else if (uses_strong_runtime_hooks) {
      out << "  %objc3_property_retained = call i32 @"
          << kObjc3RuntimeRetainI32Symbol << "(i32 " << stored_value << ")\n";
      out << "  %objc3_property_previous = call i32 @"
          << kObjc3RuntimeExchangeCurrentPropertyI32Symbol << "(i32 %objc3_property_retained)\n";
      out << "  %objc3_property_release = call i32 @"
          << kObjc3RuntimeReleaseI32Symbol
          << "(i32 %objc3_property_previous)\n";
    } else {
      out << "  call void @" << kObjc3RuntimeWriteCurrentPropertyI32Symbol
          << "(i32 " << stored_value << ")\n";
    }
    out << "  ret void\n";
    out << "}\n";
  }

  static std::string BuildImplementationMethodFunctionSymbol(const std::string &implementation_name,
                                                             const std::string &selector,
                                                             bool is_class_method) {
    auto sanitize = [](const std::string &text) {
      std::string out;
      out.reserve(text.size());
      for (unsigned char ch : text) {
        if (std::isalnum(ch) != 0) {
          out.push_back(static_cast<char>(ch));
        } else {
          out.push_back('_');
        }
      }
      if (out.empty()) {
        out = "anonymous";
      }
      return out;
    };
    const std::string owner = sanitize(implementation_name);
    const std::string selector_key = sanitize(selector);
    const char *dispatch_kind = is_class_method ? "class" : "instance";
    return "objc3_method_" + owner + "_" + dispatch_kind + "_" + selector_key;
  }

  void EmitEntryPoint(std::ostringstream &out) const {
    auto main_it = function_arity_.find("main");
    const bool has_zero_arity_main =
        main_it != function_arity_.end() && main_it->second == 0;
    if (has_zero_arity_main) {
      out << "define i32 @objc3c_entry() {\n";
      out << "entry:\n";
      ValueType main_return_type = ValueType::I32;
      const LoweredFunctionSignature *main_signature = LookupFunctionSignature("main");
      if (main_signature != nullptr) {
        main_return_type = main_signature->return_type;
      }
      if (main_return_type == ValueType::Void) {
        out << "  call void @main()\n";
        out << "  ret i32 0\n";
      } else {
        out << "  %call_main = call " << LLVMScalarType(main_return_type) << " @main()\n";
        if (main_return_type == ValueType::Bool) {
          out << "  %call_main_i32 = zext i1 %call_main to i32\n";
          out << "  ret i32 %call_main_i32\n";
        } else {
          out << "  ret i32 %call_main\n";
        }
      }
      out << "}\n";
      return;
    }

    out << "define internal i32 @objc3c_entry() {\n";
    out << "entry:\n";
    std::string previous = "0";
    for (std::size_t i = 0; i < program_.globals.size(); ++i) {
      const auto &global = program_.globals[i];
      const std::string load_name = "%entry_load_" + std::to_string(i);
      const std::string sum_name = "%entry_sum_" + std::to_string(i);
      out << "  " << load_name << " = load i32, ptr @" << global.name << ", align 4\n";
      out << "  " << sum_name << " = add i32 " << previous << ", " << load_name << "\n";
      previous = sum_name;
    }
    out << "  ret i32 " << previous << "\n";
    out << "}\n";
  }

  const Objc3Program &program_;
  Objc3IRFrontendMetadata frontend_metadata_;
  Objc3LoweringIRBoundary lowering_ir_boundary_;
  std::string boundary_error_;
  std::unordered_set<std::string> globals_;
  std::unordered_set<std::string> mutable_global_symbols_;
  std::unordered_map<std::string, int> global_const_values_;
  std::unordered_set<std::string> global_nil_proven_symbols_;
  std::unordered_set<std::string> defined_functions_;
  std::unordered_set<std::string> declared_pure_functions_;
  std::vector<const FunctionDecl *> function_definitions_;
  std::vector<MethodDefinition> method_definitions_;
  std::vector<SynthesizedPropertyStorage> synthesized_property_storages_;
  std::size_t synthesized_property_accessor_count_ = 0;
  std::unordered_map<std::string, FunctionEffectInfo> function_effects_;
  std::unordered_set<std::string> impure_functions_;
  std::unordered_map<std::string, std::size_t> function_arity_;
  std::map<std::string, LoweredFunctionSignature> function_signatures_;
  std::map<std::string, std::string> selector_pool_globals_;
  std::map<std::string, std::string> runtime_string_pool_globals_;
  std::map<std::string, TypedKeyPathArtifact> typed_keypath_artifacts_;
  std::unordered_map<std::string, int> class_receiver_constants_;
  std::size_t vector_signature_function_count_ = 0;
  mutable std::vector<std::string> block_function_definitions_;
  mutable std::unordered_set<std::string> emitted_block_invoke_symbols_;
  mutable std::unordered_set<std::string> emitted_block_copy_helper_symbols_;
  mutable std::unordered_set<std::string> emitted_block_dispose_helper_symbols_;
  mutable bool runtime_dispatch_call_emitted_ = false;
  mutable bool fail_open_fallback_triggered_ = false;
  mutable std::string fail_open_fallback_reason_;
};

bool EmitObjc3IRText(const Objc3Program &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error) {
  Objc3IREmitter emitter(program, lowering_contract, frontend_metadata);
  return emitter.Emit(ir, error);
}
