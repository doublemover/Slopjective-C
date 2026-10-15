static constexpr const char *kObjc3GenericCallableReificationPolicyErasedDefault =
    "erased_default";
static constexpr const char *kObjc3GenericCallableReificationPolicyExplicitReified =
    "explicit_reified";
static constexpr const char *kObjc3GenericCallableManglingPolicyId =
    "objc3c.generic-callable.semantic-mangling.v1";
static constexpr const char *kObjc3GenericCallableReifiedManglingPolicyId =
    "objc3c.generic-callable.reified-semantic-mangling.v1";

static bool IsObjc3GenericCallableReificationPolicyClaimable(
    const std::string &policy) {
  return policy == kObjc3GenericCallableReificationPolicyErasedDefault ||
         policy == kObjc3GenericCallableReificationPolicyExplicitReified;
}

static bool IsObjc3GenericCallableManglingPolicyClaimable(
    const std::string &policy_id) {
  return policy_id == kObjc3GenericCallableManglingPolicyId ||
         policy_id == kObjc3GenericCallableReifiedManglingPolicyId;
}

static std::string Objc3GenericCallableReificationPolicyForMarker(
    bool reification_marker_declared) {
  return reification_marker_declared
             ? kObjc3GenericCallableReificationPolicyExplicitReified
             : kObjc3GenericCallableReificationPolicyErasedDefault;
}

static std::string Objc3GenericCallableManglingPolicyForReificationPolicy(
    const std::string &reification_policy) {
  return reification_policy ==
                 kObjc3GenericCallableReificationPolicyExplicitReified
             ? kObjc3GenericCallableReifiedManglingPolicyId
             : kObjc3GenericCallableManglingPolicyId;
}

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
CollectGenericParameterNamesSourceOrder(
    const std::vector<Objc3GenericParamDecl> &generic_params) {
  std::vector<std::string> names;
  names.reserve(generic_params.size());
  for (const Objc3GenericParamDecl &generic_param : generic_params) {
    names.push_back(generic_param.name);
  }
  return names;
}

static std::vector<std::string>
CollectGenericParameterVarianceSourceOrder(
    const std::vector<Objc3GenericParamDecl> &generic_params) {
  std::vector<std::string> variances;
  variances.reserve(generic_params.size());
  for (const Objc3GenericParamDecl &generic_param : generic_params) {
    variances.push_back(generic_param.variance_spelling);
  }
  return variances;
}

static std::vector<std::vector<std::string>>
CollectGenericParameterConstraintsLexicographic(
    const std::vector<Objc3GenericParamDecl> &generic_params) {
  std::vector<std::vector<std::string>> constraints_by_parameter;
  constraints_by_parameter.reserve(generic_params.size());
  for (const Objc3GenericParamDecl &generic_param : generic_params) {
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

static std::vector<std::string>
CollectFunctionGenericParameterNamesSourceOrder(const FunctionDecl &fn) {
  return CollectGenericParameterNamesSourceOrder(fn.generic_params);
}

static std::vector<std::string>
CollectFunctionGenericParameterVarianceSourceOrder(const FunctionDecl &fn) {
  return CollectGenericParameterVarianceSourceOrder(fn.generic_params);
}

static std::vector<std::vector<std::string>>
CollectFunctionGenericParameterConstraintsLexicographic(const FunctionDecl &fn) {
  return CollectGenericParameterConstraintsLexicographic(fn.generic_params);
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
      Objc3GenericCallableReificationPolicyForMarker(
          fn.generic_callable_reification_marker_declared),
      Objc3GenericCallableManglingPolicyForReificationPolicy(
          Objc3GenericCallableReificationPolicyForMarker(
              fn.generic_callable_reification_marker_declared)));
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
         existing.generic_callable_contract_deterministic &&
         IsObjc3GenericCallableReificationPolicyClaimable(
             existing.generic_callable_reification_policy) &&
         IsObjc3GenericCallableManglingPolicyClaimable(
             existing.generic_callable_mangling_policy_id) &&
         existing.generic_callable_signature_replay_key ==
             BuildFunctionGenericCallableSignatureReplayKeyFromDecl(fn);
}

static bool HasGenericCallableConstraintCycle(
    const std::vector<Objc3GenericParamDecl> &generic_params) {
  std::unordered_set<std::string> parameter_names;
  for (const auto &param : generic_params) {
    parameter_names.insert(param.name);
  }
  std::unordered_map<std::string, std::string> edges;
  for (const auto &param : generic_params) {
    if (param.has_constraint &&
        parameter_names.find(param.constraint_type_name) != parameter_names.end()) {
      edges[param.name] = param.constraint_type_name;
    }
  }
  for (const auto &entry : edges) {
    std::unordered_set<std::string> active;
    std::string cursor = entry.first;
    while (true) {
      if (!active.insert(cursor).second) {
        return true;
      }
      const auto next = edges.find(cursor);
      if (next == edges.end()) {
        break;
      }
      cursor = next->second;
    }
  }
  return false;
}

static void ValidateGenericCallableParameterList(
    const std::vector<Objc3GenericParamDecl> &generic_params,
    const std::string &callable_description,
    bool reification_marker_declared,
    unsigned reification_marker_line,
    unsigned reification_marker_column,
    std::vector<std::string> &diagnostics) {
  if (reification_marker_declared && generic_params.empty()) {
    diagnostics.push_back(MakeDiag(
        reification_marker_line, reification_marker_column, "O3S206",
        "type mismatch: @reify_generics requires a generic callable for " +
            callable_description));
    return;
  }
  std::unordered_set<std::string> seen;
  for (const auto &generic_param : generic_params) {
    if (!seen.insert(generic_param.name).second) {
      diagnostics.push_back(MakeDiag(
          generic_param.line, generic_param.column, "O3S200",
          "duplicate generic parameter '" + generic_param.name + "' in " +
              callable_description));
    }
  }
  if (HasGenericCallableConstraintCycle(generic_params)) {
    diagnostics.push_back(MakeDiag(
        generic_params.empty() ? reification_marker_line
                               : generic_params.front().line,
        generic_params.empty() ? reification_marker_column
                               : generic_params.front().column,
        "O3S206",
        "type mismatch: generic callable constraint cycle in " +
            callable_description));
  }
  for (const auto &generic_param : generic_params) {
    if (!generic_param.has_constraint) {
      continue;
    }
    if (seen.find(generic_param.constraint_type_name) != seen.end()) {
      continue;
    }
    if (generic_param.constraint_type_name != "id" ||
        !generic_param.has_constraint_generic_suffix) {
      diagnostics.push_back(MakeDiag(
          generic_param.constraint_line, generic_param.constraint_column,
          "O3S206",
          "type mismatch: generic parameter '" + generic_param.name +
              "' in " + callable_description +
              " requires an id<Protocol> constraint"));
      continue;
    }
    const ProtocolCompositionParseResult parsed =
        ParseProtocolCompositionSuffixText(
            generic_param.constraint_generic_suffix_text);
    if (!parsed.IsValid()) {
      diagnostics.push_back(MakeDiag(
          generic_param.constraint_line, generic_param.constraint_column,
          "O3S206",
          "type mismatch: generic parameter '" + generic_param.name +
              "' in " + callable_description +
              " has an invalid protocol constraint"));
      continue;
    }
  }
}
