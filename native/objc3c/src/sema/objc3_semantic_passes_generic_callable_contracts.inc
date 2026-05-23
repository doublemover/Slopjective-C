static constexpr const char *kObjc3GenericCallableReificationPolicyErasedDefault =
    "erased_default";
static constexpr const char *kObjc3GenericCallableManglingPolicyId =
    "objc3c.generic-callable.semantic-mangling.v1";

static std::string BuildObjc3GenericCallableConstraintReplayKey(
    const std::vector<std::vector<std::string>> &constraints) {
  std::ostringstream out;
  for (std::size_t i = 0; i < constraints.size(); ++i) {
    if (i > 0u) {
      out << "|";
    }
    out << "[";
    for (std::size_t j = 0; j < constraints[i].size(); ++j) {
      if (j > 0u) {
        out << ",";
      }
      out << constraints[i][j];
    }
    out << "]";
  }
  return out.str();
}

static std::vector<std::string>
CollectFunctionGenericParameterNamesSourceOrder(const FunctionDecl &fn) {
  std::vector<std::string> names;
  names.reserve(fn.generic_params.size());
  for (const Objc3GenericParamDecl &generic_param : fn.generic_params) {
    names.push_back(generic_param.name);
  }
  return names;
}

static std::vector<std::string>
CollectFunctionGenericParameterVarianceSourceOrder(const FunctionDecl &fn) {
  std::vector<std::string> variances;
  variances.reserve(fn.generic_params.size());
  for (const Objc3GenericParamDecl &generic_param : fn.generic_params) {
    variances.push_back(generic_param.variance_spelling);
  }
  return variances;
}

static std::vector<std::vector<std::string>>
CollectFunctionGenericParameterConstraintsLexicographic(
    const FunctionDecl &fn) {
  std::vector<std::vector<std::string>> constraints_by_parameter;
  constraints_by_parameter.reserve(fn.generic_params.size());
  for (const Objc3GenericParamDecl &generic_param : fn.generic_params) {
    std::vector<std::string> constraints;
    if (generic_param.has_constraint &&
        generic_param.constraint_type_name == "id" &&
        generic_param.has_constraint_generic_suffix) {
      const ProtocolCompositionParseResult parsed =
          ParseProtocolCompositionSuffixText(
              generic_param.constraint_generic_suffix_text);
      if (parsed.IsValid()) {
        constraints = parsed.names_lexicographic;
      }
    }
    constraints_by_parameter.push_back(std::move(constraints));
  }
  return constraints_by_parameter;
}

static std::string BuildObjc3GenericCallableSignatureReplayKey(
    const std::string &callable_kind,
    const std::string &callable_name,
    const std::vector<std::string> &generic_parameter_names_source_order,
    const std::vector<std::string> &generic_parameter_variance_source_order,
    const std::vector<std::vector<std::string>>
        &generic_parameter_constraints_lexicographic,
    const std::string &reification_policy,
    const std::string &mangling_policy_id) {
  std::ostringstream out;
  out << "kind=" << callable_kind << ";name=" << callable_name
      << ";generic_arity=" << generic_parameter_names_source_order.size()
      << ";names=";
  for (std::size_t i = 0; i < generic_parameter_names_source_order.size();
       ++i) {
    if (i > 0u) {
      out << ",";
    }
    out << generic_parameter_names_source_order[i];
  }
  out << ";variance=";
  for (std::size_t i = 0; i < generic_parameter_variance_source_order.size();
       ++i) {
    if (i > 0u) {
      out << ",";
    }
    out << generic_parameter_variance_source_order[i];
  }
  out << ";constraints="
      << BuildObjc3GenericCallableConstraintReplayKey(
             generic_parameter_constraints_lexicographic)
      << ";reification_policy=" << reification_policy
      << ";mangling_policy_id=" << mangling_policy_id;
  return out.str();
}

static std::string BuildFunctionGenericCallableSignatureReplayKeyFromDecl(
    const FunctionDecl &fn) {
  return BuildObjc3GenericCallableSignatureReplayKey(
      "function", fn.name, CollectFunctionGenericParameterNamesSourceOrder(fn),
      CollectFunctionGenericParameterVarianceSourceOrder(fn),
      CollectFunctionGenericParameterConstraintsLexicographic(fn),
      kObjc3GenericCallableReificationPolicyErasedDefault,
      kObjc3GenericCallableManglingPolicyId);
}

static bool IsFunctionGenericCallableSignatureCompatibleWithDecl(
    const FunctionInfo &existing,
    const FunctionDecl &fn) {
  return existing.generic_parameter_names_source_order ==
             CollectFunctionGenericParameterNamesSourceOrder(fn) &&
         existing.generic_parameter_variance_source_order ==
             CollectFunctionGenericParameterVarianceSourceOrder(fn) &&
         existing.generic_parameter_constraints_lexicographic ==
             CollectFunctionGenericParameterConstraintsLexicographic(fn) &&
         existing.generic_callable_reification_policy ==
             kObjc3GenericCallableReificationPolicyErasedDefault &&
         existing.generic_callable_mangling_policy_id ==
             kObjc3GenericCallableManglingPolicyId &&
         existing.generic_callable_signature_replay_key ==
             BuildFunctionGenericCallableSignatureReplayKeyFromDecl(fn);
}
