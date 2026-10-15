  return deterministic;
}

#include "sema/objc3_semantic_type_metadata_handoff_determinism_explanation.inc"

bool IsObjc3ReservedFullWidthI64ValueOptionalLanguageAbi(
    const Objc3ValueOptionalTypeDescriptor &descriptor) {
  return descriptor.present && descriptor.payload_full_width_i64 &&
         descriptor.full_width_i64_runtime_helper_supported &&
         !descriptor.full_width_i64_language_call_abi_supported;
}

void DiagnoseObjc3ReservedFullWidthI64ValueOptionalLanguageAbi(
    const Objc3ValueOptionalTypeDescriptor &descriptor,
    const std::string &site,
    std::vector<std::string> &diagnostics) {
  if (!IsObjc3ReservedFullWidthI64ValueOptionalLanguageAbi(descriptor)) {
    return;
  }
  diagnostics.push_back(MakeDiag(
      descriptor.line, descriptor.column, "O3P159",
      site +
          " uses an Optional<i64> form outside the wide-carrier direct "
          "function/method call/return ABI; unsupported object/nullability "
          "bridge, nested/generic, storage, unwrap, nil, conversion, and "
          "dynamic paths remain fail-closed "
          "{recovery: strategy=value-optional-i64-language-abi-boundary; "
          "boundary=callable-signature; deterministic=true; "
          "recovery-counts-as-success=false}"));
}

void DiagnoseObjc3FunctionValueOptionalLanguageAbiBoundaries(
    const FunctionDecl &fn,
    std::vector<std::string> &diagnostics) {
  DiagnoseObjc3ReservedFullWidthI64ValueOptionalLanguageAbi(
      fn.return_value_optional,
      "function '" + fn.name + "' return type",
      diagnostics);
  for (const FuncParam &param : fn.params) {
    DiagnoseObjc3ReservedFullWidthI64ValueOptionalLanguageAbi(
        param.value_optional,
        "function '" + fn.name + "' parameter '" + param.name + "'",
        diagnostics);
  }
}

void DiagnoseObjc3MethodValueOptionalLanguageAbiBoundaries(
    const Objc3MethodDecl &method,
    const std::string &owner_context,
    std::vector<std::string> &diagnostics) {
  const std::string selector = MethodSelectorName(method);
  DiagnoseObjc3ReservedFullWidthI64ValueOptionalLanguageAbi(
      method.return_value_optional,
      "method '" + selector + "' in " + owner_context + " return type",
      diagnostics);
  for (const FuncParam &param : method.params) {
    DiagnoseObjc3ReservedFullWidthI64ValueOptionalLanguageAbi(
        param.value_optional,
        "method '" + selector + "' in " + owner_context + " parameter '" +
            param.name + "'",
        diagnostics);
  }
}

void ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,
                            const Objc3SemanticValidationOptions &options,
                            Objc3SemaLanguageProfile language_profile,
                            std::vector<std::string> &diagnostics) {
  const Objc3SemanticBodyValidationContext context =
      BuildObjc3SemanticBodyValidationContext(program, surface, options, diagnostics);
  const Objc3Program &ast = *context.ast;
  const bool strict_concurrency =
      Objc3SemaLanguageProfileUsesStrictConcurrency(language_profile);
  std::vector<std::string> protocol_definition_diagnostics;
  const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
      protocol_definitions = BuildProtocolSemanticDefinitions(
          ast, options.arc_mode_enabled, protocol_definition_diagnostics);
  std::unordered_set<std::string> generic_interface_names;
  for (const auto &entry : surface.interfaces) {
    if (!entry.second.generic_parameter_names_source_order.empty()) {
      generic_interface_names.insert(entry.first);
    }
  }
  const auto borrowed_callable_contracts =
      BuildOwnershipBorrowedCallableContracts(ast);
  std::unordered_map<std::string, const Objc3InterfaceDecl *> actor_interfaces;
  for (const auto &interface_decl : ast.interfaces) {
    if (interface_decl.is_actor) {
      actor_interfaces.emplace(interface_decl.name, &interface_decl);
    }
  }
  std::unordered_map<std::string, ValueType> body_globals = surface.globals;
  for (const auto &interface_decl : ast.interfaces) {
    body_globals.try_emplace(interface_decl.name, ValueType::ObjCClass);
  }
  for (const auto &implementation_decl : ast.implementations) {
    body_globals.try_emplace(implementation_decl.name, ValueType::ObjCClass);
  }
  StaticScalarBindings global_static_bindings;
  std::unordered_set<std::string> assigned_identifier_names;
  for (const auto &fn : ast.functions) {
    CollectAssignedIdentifiers(fn.body, assigned_identifier_names);
  }
  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      if (!method.has_body) {
        continue;
      }
      CollectAssignedIdentifiers(method.body, assigned_identifier_names);
    }
  }
  std::vector<int> global_initializer_values;
  if (ResolveObjc3GlobalInitializerValues(ast.globals,
                                          global_initializer_values)) {
    const std::size_t count = std::min(ast.globals.size(), global_initializer_values.size());
    for (std::size_t i = 0; i < count; ++i) {
      const std::string &name = ast.globals[i].name;
      if (assigned_identifier_names.find(name) != assigned_identifier_names.end()) {
        continue;
      }
      global_static_bindings[name] = global_initializer_values[i];
    }
  }

  for (const auto &fn : ast.functions) {
    DiagnoseObjc3FunctionValueOptionalLanguageAbiBoundaries(fn, diagnostics);
    ValidateReturnTypeSuffixes(fn, diagnostics, &generic_interface_names);
    ValidateParameterTypeSuffixes(fn, diagnostics, &generic_interface_names);
    DiagnoseOwnershipRetainableFamilyCallableLegality(
        fn, "function '" + fn.name + "'", diagnostics);
    if (fn.executor_affinity_declared && !fn.async_declared) {
      diagnostics.push_back(MakeDiag(
          fn.line, fn.column, "O3S224",
          "async semantics failed: objc_executor requires an async function or method"));
    }
    if (fn.async_declared && fn.is_prototype) {
      diagnostics.push_back(MakeDiag(
          fn.line, fn.column, "O3S225",
          "async semantics failed: async function prototypes remain unsupported until continuation lowering lands"));
    }
    if (fn.async_declared && fn.throws_declared) {
      diagnostics.push_back(MakeDiag(
          fn.line, fn.column, "O3S226",
          "async semantics failed: async throws functions remain unsupported until async error propagation lands"));
    }

    std::vector<SemanticScope> scopes;
    scopes.push_back({});
    for (const auto &param : fn.params) {
      if (scopes.back().find(param.name) != scopes.back().end()) {
        diagnostics.push_back(MakeDiag(param.line, param.column, "O3S201", "duplicate parameter '" + param.name + "'"));
      } else {
        SemanticTypeInfo param_type = MakeSemanticTypeFromParam(param);
        param_type.is_mutable_binding = true;
        scopes.back().emplace(param.name, param_type);
      }
    }

    if (!fn.is_prototype) {
      const Objc3ConcurrencyTaskCallableLegalityProfile task_legality_profile =
          BuildConcurrencyTaskCallableLegalityProfile(fn.body);
      if (strict_concurrency) {
        DiagnoseConcurrencyTaskCallableLegality(fn.body, fn.async_declared,
                                                diagnostics);
        DiagnoseConcurrencyExecutorAffinityCompletion(
            task_legality_profile, fn.async_declared,
            fn.executor_affinity_declared, fn.executor_affinity_kind, fn.line,
            fn.column, diagnostics);
      }
      DiagnoseOwnershipResourceMoveSemantics(fn.body, scopes, diagnostics);
      std::unordered_set<std::string> borrowed_parameter_names;
      for (const auto &param : fn.params) {
        if (param.borrowed_pointer_qualified) {
          borrowed_parameter_names.insert(param.name);
        }
      }
      DiagnoseOwnershipBorrowedPointerEscapeSemantics(
          fn.body, scopes, borrowed_parameter_names, borrowed_callable_contracts,
          BuildOwnershipBorrowedReturnContract(fn), diagnostics);
      const SemanticTypeInfo expected_return_type = MakeSemanticTypeFromFunctionReturn(fn);
      const StaticScalarBindings static_scalar_bindings = CollectFunctionStaticScalarBindings(fn, &global_static_bindings);
      Objc3MessageSendResolutionContext message_send_context;
      message_send_context.surface = &surface;
      message_send_context.protocol_definitions = &protocol_definitions;
      message_send_context.inside_async_context = fn.async_declared;
      ValidateStatements(fn.body, scopes, body_globals, surface.functions, expected_return_type, fn.name, diagnostics,
                         0, 0, 0, 0, false, options.max_message_send_args,
                         message_send_context);
      // Legacy extraction anchor retained for contract tests:
      // ValidateStatements(fn.body, scopes, surface.globals, surface.functions, fn.return_type, fn.name, diagnostics,
      if (!(expected_return_type.type == ValueType::Void && !expected_return_type.is_vector) &&
          !BlockAlwaysReturns(fn.body, &static_scalar_bindings)) {
        diagnostics.push_back(
            BuildObjc3SemaMissingReturnDiagnostic(
                fn.line, fn.column, "function '" + fn.name + "'"));
      }
    }
  }

  for (const auto &protocol_decl : ast.protocols) {
    for (const auto &method : protocol_decl.methods) {
      DiagnoseObjc3MethodValueOptionalLanguageAbiBoundaries(
          method, "protocol '" + protocol_decl.name + "'", diagnostics);
      DiagnoseOwnershipRetainableFamilyCallableLegality(
          method,
          "method '" + MethodSelectorName(method) + "' in protocol '" +
              protocol_decl.name + "'",
          diagnostics);
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method : interface_decl.methods) {
      DiagnoseObjc3MethodValueOptionalLanguageAbiBoundaries(
          method, "interface '" + interface_decl.name + "'", diagnostics);
      DiagnoseOwnershipRetainableFamilyCallableLegality(
          method,
          "method '" + MethodSelectorName(method) + "' in interface '" +
              interface_decl.name + "'",
          diagnostics);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      DiagnoseObjc3MethodValueOptionalLanguageAbiBoundaries(
          method, "implementation '" + implementation_decl.name + "'",
          diagnostics);
      DiagnoseOwnershipRetainableFamilyCallableLegality(
          method,
          "method '" + MethodSelectorName(method) + "' in implementation '" +
              implementation_decl.name + "'",
          diagnostics);
      if (strict_concurrency) {
        DiagnoseConcurrencyActorMethodIsolationRules(
            method, IsActorInterfaceOwner(actor_interfaces, implementation_decl.name),
            method.line, method.column, diagnostics);
        DiagnoseConcurrencyActorRaceHazardRules(
            method, IsActorInterfaceOwner(actor_interfaces, implementation_decl.name),
            method.line, method.column, diagnostics);
      }
      if (method.executor_affinity_declared && !method.async_declared) {
        diagnostics.push_back(MakeDiag(
            method.line, method.column, "O3S224",
            "async semantics failed: objc_executor requires an async function or method"));
      }
      if (method.async_declared && method.throws_declared) {
        diagnostics.push_back(MakeDiag(
            method.line, method.column, "O3S226",
            "async semantics failed: async throws functions remain unsupported until async error propagation lands"));
      }
      if (!method.has_body) {
        continue;
      }

      std::vector<SemanticScope> scopes;
      scopes.push_back({});
      scopes.back().emplace("self",
                            method.is_class_method
                                ? MakeScalarSemanticType(ValueType::ObjCClass)
                                : MakeScalarSemanticType(ValueType::ObjCId));
      scopes.back().emplace("super",
                            method.is_class_method
                                ? MakeScalarSemanticType(ValueType::ObjCClass)
                                : MakeScalarSemanticType(ValueType::ObjCId));
      for (const auto &param : method.params) {
        if (scopes.back().find(param.name) != scopes.back().end()) {
          diagnostics.push_back(MakeDiag(param.line, param.column, "O3S201", "duplicate parameter '" + param.name + "'"));
        } else {
          SemanticTypeInfo param_type = MakeSemanticTypeFromParam(param);
          param_type.is_mutable_binding = true;
          scopes.back().emplace(param.name, param_type);
        }
      }

      const Objc3ConcurrencyTaskCallableLegalityProfile task_legality_profile =
          BuildConcurrencyTaskCallableLegalityProfile(method.body);
      if (strict_concurrency) {
        DiagnoseConcurrencyTaskCallableLegality(method.body, method.async_declared,
                                          diagnostics);
        DiagnoseConcurrencyExecutorAffinityCompletion(
            task_legality_profile, method.async_declared,
            method.executor_affinity_declared, method.executor_affinity_kind,
            method.line, method.column, diagnostics);
      }
      DiagnoseOwnershipResourceMoveSemantics(method.body, scopes, diagnostics);
      std::unordered_set<std::string> borrowed_parameter_names;
      for (const auto &param : method.params) {
        if (param.borrowed_pointer_qualified) {
          borrowed_parameter_names.insert(param.name);
        }
      }
      DiagnoseOwnershipBorrowedPointerEscapeSemantics(
          method.body, scopes, borrowed_parameter_names,
          borrowed_callable_contracts,
          BuildOwnershipBorrowedReturnContract(method), diagnostics);
      const SemanticTypeInfo expected_return_type = MakeSemanticTypeFromMethodReturn(method);
      const std::string method_context =
          "method '" + MethodSelectorName(method) + "' in implementation '" + implementation_decl.name + "'";
      Objc3MessageSendResolutionContext message_send_context;
      message_send_context.surface = &surface;
      message_send_context.protocol_definitions = &protocol_definitions;
      message_send_context.current_implementation_name = implementation_decl.name;
      message_send_context.inside_method = true;
      message_send_context.is_class_method = method.is_class_method;
      message_send_context.inside_async_context = method.async_declared;
      const auto interface_it = surface.interfaces.find(implementation_decl.name);
      if (interface_it != surface.interfaces.end()) {
        message_send_context.current_super_name = interface_it->second.super_name;
      }
      ValidateStatements(method.body, scopes, body_globals, surface.functions, expected_return_type, method_context,
                         diagnostics, 0, 0, 0, 0, false,
                         options.max_message_send_args,
                         message_send_context);
      if (!(expected_return_type.type == ValueType::Void && !expected_return_type.is_vector) &&
          !BlockAlwaysReturns(method.body, &global_static_bindings)) {
        diagnostics.push_back(BuildObjc3SemaMissingReturnDiagnostic(
            method.line, method.column, method_context));
      }
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method : interface_decl.methods) {
      if (strict_concurrency) {
        DiagnoseConcurrencyActorMethodIsolationRules(method, interface_decl.is_actor,
                                               method.line, method.column,
                                               diagnostics);
      }
    }
  }
}

void RefreshSemanticIntegrationSurfaceAfterBodyValidation(
    const Objc3ParsedProgram &program,
    Objc3SemanticIntegrationSurface &surface) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  surface.block_copy_dispose_sites_lexicographic =
      BuildBlockCopyDisposeSiteMetadataLexicographic(ast);
  surface.block_copy_dispose_semantics_summary =
      BuildBlockCopyDisposeSemanticsSummaryFromIntegrationSurface(surface);
}
