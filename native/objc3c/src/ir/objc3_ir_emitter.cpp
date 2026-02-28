#include "ir/objc3_ir_emitter.h"

#include <algorithm>
#include <array>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

bool ResolveGlobalInitializerValues(const std::vector<GlobalDecl> &globals, std::vector<int> &values);

class Objc3IREmitter {
 public:
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
    function_signatures_ = BuildLoweredFunctionSignatures(program_);
    CollectSelectorLiterals();
    CollectMutableGlobalSymbols();
    CollectFunctionEffects();
  }

  bool Emit(std::string &ir, std::string &error) {
    runtime_dispatch_call_emitted_ = false;

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

    EmitSelectorConstants(body);

    EmitPrototypeDeclarations(body);

    for (const FunctionDecl *fn : function_definitions_) {
      EmitFunction(*fn, body);
      body << "\n";
    }

    EmitEntryPoint(body);

    std::ostringstream out;
    out << "; objc3c native frontend IR\n";
    out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\n";
    out << "; runtime_dispatch_decl = " << Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_) << "\n";
    out << "; simd_vector_lowering = " << Objc3SimdVectorTypeLoweringReplayKey() << "\n";
    if (!frontend_metadata_.lowering_property_synthesis_ivar_binding_replay_key.empty()) {
      out << "; property_synthesis_ivar_binding_lowering = "
          << frontend_metadata_.lowering_property_synthesis_ivar_binding_replay_key << "\n";
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
    if (!frontend_metadata_.lowering_super_dispatch_method_family_replay_key.empty()) {
      out << "; super_dispatch_method_family_lowering = "
          << frontend_metadata_.lowering_super_dispatch_method_family_replay_key << "\n";
    }
    out << "; simd_vector_function_signatures = " << vector_signature_function_count_ << "\n";
    out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)
        << ", compatibility_mode=" << frontend_metadata_.compatibility_mode
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
    if (runtime_dispatch_call_emitted_) {
      out << Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_) << "\n\n";
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
  };

  struct ControlLabels {
    std::string continue_label;
    std::string break_label;
    bool continue_allowed = false;
  };

  struct FunctionContext {
    std::vector<std::string> entry_lines;
    std::vector<std::string> code_lines;
    std::vector<std::unordered_map<std::string, std::string>> scopes;
    std::vector<ControlLabels> control_stack;
    std::unordered_set<std::string> nil_bound_ptrs;
    std::unordered_set<std::string> nonzero_bound_ptrs;
    std::unordered_map<std::string, int> const_value_ptrs;
    ValueType return_type = ValueType::I32;
    int temp_counter = 0;
    int label_counter = 0;
    bool terminated = false;
    bool global_proofs_invalidated = false;
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
      if (c == '\\' || c == '"') {
        out << '\\' << static_cast<char>(c);
        continue;
      }
      if (c >= 32 && c <= 126) {
        out << static_cast<char>(c);
        continue;
      }
      std::ostringstream byte;
      byte << std::hex << std::uppercase << static_cast<int>(c);
      std::string value = byte.str();
      if (value.size() < 2) {
        value = "0" + value;
      }
      out << "\\" << value;
    }
    return out.str();
  }

  void EmitFrontendMetadata(std::ostringstream &out) const {
    out << "!objc3.frontend = !{!0}\n";
    out << "!objc3.objc_interface_implementation = !{!1}\n";
    out << "!objc3.objc_protocol_category = !{!2}\n";
    out << "!objc3.objc_class_protocol_category_linking = !{!7}\n";
    out << "!objc3.objc_selector_normalization = !{!3}\n";
    out << "!objc3.objc_property_attribute = !{!4}\n";
    out << "!objc3.objc_object_pointer_nullability_generics = !{!5}\n";
    out << "!objc3.objc_symbol_graph_scope_resolution = !{!6}\n";
    out << "!objc3.objc_id_class_sel_object_pointer_typecheck = !{!8}\n";
    out << "!objc3.objc_message_send_selector_lowering = !{!9}\n";
    out << "!objc3.objc_dispatch_abi_marshalling = !{!10}\n";
    out << "!objc3.objc_nil_receiver_semantics_foldability = !{!11}\n";
    out << "!objc3.objc_super_dispatch_method_family = !{!12}\n";
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
  }

  void RegisterSelectorLiteral(const std::string &selector) {
    if (selector.empty() || selector_globals_.find(selector) != selector_globals_.end()) {
      return;
    }
    selector_globals_.emplace(selector, "");
  }

  void AssignSelectorGlobalNames() {
    std::size_t index = 0;
    for (auto &entry : selector_globals_) {
      entry.second = "@.objc3.sel." + std::to_string(index++);
    }
  }

  void CollectSelectorExpr(const Expr *expr) {
    if (expr == nullptr) {
      return;
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

  void CollectSelectorLiterals() {
    for (const auto &global : program_.globals) {
      CollectSelectorExpr(global.value.get());
    }
    for (const auto &fn : program_.functions) {
      for (const auto &stmt : fn.body) {
        CollectSelectorStmt(stmt.get());
      }
    }
    AssignSelectorGlobalNames();
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

  void EmitSelectorConstants(std::ostringstream &out) const {
    for (const auto &entry : selector_globals_) {
      const std::string &selector = entry.first;
      const std::string &global_name = entry.second;
      const std::size_t storage_len = selector.size() + 1;
      out << global_name << " = private unnamed_addr constant [" << storage_len << " x i8] c\""
          << EscapeCStringLiteral(selector) << "\\00\", align 1\n";
    }
    if (!selector_globals_.empty()) {
      out << "\n";
    }
  }

  std::string NewTemp(FunctionContext &ctx) const { return "%t" + std::to_string(ctx.temp_counter++); }

  std::string NewLabel(FunctionContext &ctx, const std::string &prefix) const {
    return prefix + std::to_string(ctx.label_counter++);
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

  void EmitTypedReturn(const std::string &i32_value, FunctionContext &ctx) const {
    if (ctx.return_type == ValueType::Void) {
      ctx.code_lines.push_back("  ret void");
      return;
    }
    if (ctx.return_type == ValueType::Bool) {
      const std::string bool_i1 = CoerceI32ToBoolI1(i32_value, ctx);
      ctx.code_lines.push_back("  ret i1 " + bool_i1);
      return;
    }
    ctx.code_lines.push_back("  ret i32 " + i32_value);
  }

  void EmitTypedParamStore(const FuncParam &param, std::size_t index, const std::string &ptr, FunctionContext &ctx) const {
    if (param.type == ValueType::Bool) {
      const std::string widened = "%arg" + std::to_string(index) + ".zext." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + widened + " = zext i1 %arg" + std::to_string(index) + " to i32");
      ctx.entry_lines.push_back("  store i32 " + widened + ", ptr " + ptr + ", align 4");
      return;
    }
    ctx.entry_lines.push_back("  store i32 %arg" + std::to_string(index) + ", ptr " + ptr + ", align 4");
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
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
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
      return false;
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
    return true;
  }

  LoweredMessageSend LowerMessageSendExpr(const Expr *expr, FunctionContext &ctx) const {
    LoweredMessageSend lowered;
    lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, "0");
    if (expr == nullptr) {
      return lowered;
    }

    lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);
    lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);
    lowered.receiver = EmitExpr(expr->receiver.get(), ctx);
    lowered.selector = expr->selector;
    for (std::size_t i = 0; i < expr->args.size() && i < lowered.args.size(); ++i) {
      lowered.args[i] = EmitExpr(expr->args[i].get(), ctx);
    }
    return lowered;
  }

  std::string EmitRuntimeDispatch(const LoweredMessageSend &lowered, FunctionContext &ctx) const {
    if (lowered.receiver_is_compile_time_zero) {
      return "0";
    }

    auto selector_it = selector_globals_.find(lowered.selector);
    if (selector_it == selector_globals_.end()) {
      return "0";
    }

    const std::size_t selector_len = lowered.selector.size() + 1;
    const std::string selector_ptr = NewTemp(ctx);
    ctx.code_lines.push_back("  " + selector_ptr + " = getelementptr inbounds [" + std::to_string(selector_len) +
                             " x i8], ptr " + selector_it->second + ", i32 0, i32 0");

    const auto emit_dispatch_call = [&](const std::string &dispatch_value) {
      std::ostringstream call;
      call << "  " << dispatch_value << " = call i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32 "
           << lowered.receiver << ", ptr " << selector_ptr;
      for (const std::string &arg : lowered.args) {
        call << ", i32 " << arg;
      }
      call << ")";
      runtime_dispatch_call_emitted_ = true;
      ctx.code_lines.push_back(call.str());
    };

    if (lowered.receiver_is_compile_time_nonzero) {
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
    const LoweredMessageSend lowered = LowerMessageSendExpr(expr, ctx);
    return EmitRuntimeDispatch(lowered, ctx);
  }

  std::string EmitExpr(const Expr *expr, FunctionContext &ctx) const {
    if (expr == nullptr) {
      return "0";
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
        return std::to_string(expr->number);
      case Expr::Kind::BoolLiteral:
        return expr->bool_value ? "1" : "0";
      case Expr::Kind::NilLiteral:
        return "0";
      case Expr::Kind::Identifier: {
        const std::string ptr = LookupVarPtr(ctx, expr->ident);
        if (!ptr.empty()) {
          const std::string tmp = NewTemp(ctx);
          ctx.code_lines.push_back("  " + tmp + " = load i32, ptr " + ptr + ", align 4");
          return tmp;
        }
        if (globals_.find(expr->ident) != globals_.end()) {
          const std::string tmp = NewTemp(ctx);
          ctx.code_lines.push_back("  " + tmp + " = load i32, ptr @" + expr->ident + ", align 4");
          return tmp;
        }
        return "0";
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
          return "0";
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
    return "0";
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
          EmitTypedReturn("0", ctx);
        } else {
          const std::string value = EmitExpr(ret->value.get(), ctx);
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
        EmitAssignmentStore(ptr, assign->op, assign->value.get(), ctx);
        return;
      }
      case Stmt::Kind::Break: {
        if (ctx.control_stack.empty()) {
          ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
        } else {
          ctx.code_lines.push_back("  br label %" + ctx.control_stack.back().break_label);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Continue: {
        std::string continue_label;
        for (auto it = ctx.control_stack.rbegin(); it != ctx.control_stack.rend(); ++it) {
          if (it->continue_allowed) {
            continue_label = it->continue_label;
            break;
          }
        }
        if (continue_label.empty()) {
          ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
        } else {
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
        ctx.scopes.push_back({});
        for (const auto &nested_stmt : block_stmt->body) {
          EmitStatement(nested_stmt.get(), ctx);
        }
        ctx.scopes.pop_back();
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
        ctx.scopes.push_back({});
        ctx.control_stack.push_back({cond_label, end_label, true});
        ctx.terminated = false;
        for (const auto &s : while_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        ctx.scopes.pop_back();
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
        ctx.scopes.push_back({});
        ctx.control_stack.push_back({cond_label, end_label, true});
        ctx.terminated = false;
        for (const auto &s : do_while_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        ctx.scopes.pop_back();
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

        ctx.scopes.push_back({});
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
        ctx.scopes.push_back({});
        ctx.control_stack.push_back({step_label, end_label, true});
        ctx.terminated = false;
        for (const auto &s : for_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        ctx.scopes.pop_back();
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + step_label);
        }

        ctx.code_lines.push_back(step_label + ":");
        EmitForClause(for_stmt->step, ctx);
        ctx.code_lines.push_back("  br label %" + cond_label);

        ctx.code_lines.push_back(end_label + ":");
        ctx.scopes.pop_back();
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
          ctx.scopes.push_back({});
          ctx.control_stack.push_back({"", end_label, false});
          ctx.terminated = false;
          for (const auto &case_body_stmt : case_stmt.body) {
            EmitStatement(case_body_stmt.get(), ctx);
          }
          const bool arm_terminated = ctx.terminated;
          ctx.control_stack.pop_back();
          ctx.scopes.pop_back();

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

        const std::string cond = EmitExpr(if_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        const std::string then_label = NewLabel(ctx, "if_then_");
        const std::string else_label = NewLabel(ctx, "if_else_");
        const std::string merge_label = NewLabel(ctx, "if_end_");

        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + then_label + ", label %" + else_label);

        ctx.code_lines.push_back(then_label + ":");
        ctx.scopes.push_back({});
        ctx.terminated = false;
        for (const auto &s : if_stmt->then_body) {
          EmitStatement(s.get(), ctx);
        }
        const bool then_terminated = ctx.terminated;
        ctx.scopes.pop_back();
        if (!then_terminated) {
          ctx.code_lines.push_back("  br label %" + merge_label);
        }

        ctx.code_lines.push_back(else_label + ":");
        ctx.scopes.push_back({});
        ctx.terminated = false;
        for (const auto &s : if_stmt->else_body) {
          EmitStatement(s.get(), ctx);
        }
        const bool else_terminated = ctx.terminated;
        ctx.scopes.pop_back();
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
    for (const auto &entry : function_signatures_) {
      if (defined_functions_.find(entry.first) != defined_functions_.end()) {
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
    ctx.scopes.push_back({});

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
      if (fn.return_type == ValueType::Void) {
        ctx.code_lines.push_back("  ret void");
      } else {
        ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(fn.return_type)) + " 0");
      }
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }

    out << "}\n";
  }

  void EmitEntryPoint(std::ostringstream &out) const {
    out << "define i32 @objc3c_entry() {\n";
    out << "entry:\n";

    auto main_it = function_arity_.find("main");
    if (main_it != function_arity_.end() && main_it->second == 0) {
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
  std::unordered_map<std::string, FunctionEffectInfo> function_effects_;
  std::unordered_set<std::string> impure_functions_;
  std::unordered_map<std::string, std::size_t> function_arity_;
  std::map<std::string, LoweredFunctionSignature> function_signatures_;
  std::map<std::string, std::string> selector_globals_;
  std::size_t vector_signature_function_count_ = 0;
  mutable bool runtime_dispatch_call_emitted_ = false;
};

bool EmitObjc3IRText(const Objc3ParsedProgram &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error) {
  Objc3IREmitter emitter(Objc3ParsedProgramAst(program), lowering_contract, frontend_metadata);
  return emitter.Emit(ir, error);
}
