static std::vector<std::string> SortedPureContractNames(
    const std::unordered_map<std::string,
                             PureContractEffectInfo::SourceLoc> &names) {
  std::vector<std::string> ordered;
  ordered.reserve(names.size());
  for (const auto &entry : names) {
    ordered.push_back(entry.first);
  }
  std::sort(ordered.begin(), ordered.end());
  return ordered;
}

static std::vector<std::string> BuildOrderedPureContractFunctionNames(
    const std::unordered_map<std::string,
                             PureContractEffectInfo> &function_effects) {
  std::vector<std::string> ordered_functions;
  ordered_functions.reserve(function_effects.size());
  for (const auto &entry : function_effects) {
    ordered_functions.push_back(entry.first);
  }
  std::sort(ordered_functions.begin(), ordered_functions.end());
  return ordered_functions;
}

static std::unordered_map<std::string, bool> BuildPureContractAnnotationMap(
    const std::unordered_map<std::string, FunctionInfo> &surface_functions) {
  std::unordered_map<std::string, bool> pure_annotations;
  for (const auto &entry : surface_functions) {
    pure_annotations[entry.first] = entry.second.is_pure_annotation;
  }
  return pure_annotations;
}

static PureContractCause DetermineDirectPureContractImpurityCause(
    const PureContractEffectInfo &info) {
  PureContractCause cause;
  if (info.global_write_site.present) {
    cause.token = "global-write";
    cause.line = info.global_write_site.line;
    cause.column = info.global_write_site.column;
    cause.present = true;
    cause.detail_token = cause.token;
    cause.detail_line = cause.line;
    cause.detail_column = cause.column;
    cause.detail_present = true;
    return cause;
  }
  if (info.message_send_site.present) {
    cause.token = "message-send";
    cause.line = info.message_send_site.line;
    cause.column = info.message_send_site.column;
    cause.present = true;
    cause.detail_token = cause.token;
    cause.detail_line = cause.line;
    cause.detail_column = cause.column;
    cause.detail_present = true;
    return cause;
  }
  return cause;
}

static bool IsBetterPureContractCause(const PureContractCause &candidate,
                                      const PureContractCause &current) {
  if (!candidate.present) {
    return false;
  }
  if (!current.present) {
    return true;
  }
  if (candidate.token != current.token) {
    return candidate.token < current.token;
  }
  if (candidate.line != current.line) {
    return candidate.line < current.line;
  }
  return candidate.column < current.column;
}

static PureContractCause BuildPureContractCalleeCause(
    const std::string &callee,
    const PureContractEffectInfo &effect,
    const std::unordered_set<std::string> &defined_functions,
    const std::unordered_map<std::string, bool> &pure_annotations,
    const std::unordered_set<std::string> &impure_functions,
    const std::unordered_map<std::string, PureContractCause> &impure_causes) {
  const bool callee_defined =
      defined_functions.find(callee) != defined_functions.end();
  const bool callee_pure =
      pure_annotations.find(callee) != pure_annotations.end() &&
      pure_annotations.find(callee)->second;
  PureContractCause candidate_cause;
  const auto call_site_it = effect.called_functions.find(callee);
  if (call_site_it != effect.called_functions.end() &&
      call_site_it->second.present) {
    candidate_cause.line = call_site_it->second.line;
    candidate_cause.column = call_site_it->second.column;
    candidate_cause.present = true;
  }
  if (!callee_defined && !callee_pure) {
    candidate_cause.token = "unannotated-extern-call:" + callee;
    if (candidate_cause.present) {
      candidate_cause.detail_token = candidate_cause.token;
      candidate_cause.detail_line = candidate_cause.line;
      candidate_cause.detail_column = candidate_cause.column;
      candidate_cause.detail_present = true;
    }
  } else if (impure_functions.find(callee) != impure_functions.end()) {
    candidate_cause.token = "impure-callee:" + callee;
    const auto callee_cause_it = impure_causes.find(callee);
    if (callee_cause_it != impure_causes.end()) {
      const PureContractCause &callee_cause = callee_cause_it->second;
      if (callee_cause.detail_present) {
        candidate_cause.detail_token = callee_cause.detail_token;
        candidate_cause.detail_line = callee_cause.detail_line;
        candidate_cause.detail_column = callee_cause.detail_column;
        candidate_cause.detail_present = true;
      } else if (callee_cause.present) {
        candidate_cause.detail_token = callee_cause.token;
        candidate_cause.detail_line = callee_cause.line;
        candidate_cause.detail_column = callee_cause.column;
        candidate_cause.detail_present = true;
      }
    }
  }
  return candidate_cause;
}

static void NormalizePureContractCauseDetail(PureContractCause &cause) {
  if (!cause.detail_present) {
    cause.detail_token = cause.token;
    cause.detail_line = cause.line;
    cause.detail_column = cause.column;
    cause.detail_present = true;
  }
}

static PureContractCause SelectTransitivePureContractCause(
    const PureContractEffectInfo &effect,
    const std::unordered_set<std::string> &defined_functions,
    const std::unordered_map<std::string, bool> &pure_annotations,
    const std::unordered_set<std::string> &impure_functions,
    const std::unordered_map<std::string, PureContractCause> &impure_causes) {
  const std::vector<std::string> callees =
      SortedPureContractNames(effect.called_functions);
  PureContractCause selected_cause;
  for (const std::string &callee : callees) {
    const PureContractCause candidate_cause =
        BuildPureContractCalleeCause(callee, effect, defined_functions,
                                     pure_annotations, impure_functions,
                                     impure_causes);
    if (IsBetterPureContractCause(candidate_cause, selected_cause)) {
      selected_cause = candidate_cause;
    }
  }
  return selected_cause;
}

static void SeedDirectPureContractImpurities(
    const std::vector<std::string> &ordered_functions,
    const std::unordered_map<std::string,
                             PureContractEffectInfo> &function_effects,
    std::unordered_set<std::string> &impure_functions,
    std::unordered_map<std::string, PureContractCause> &impure_causes) {
  for (const std::string &name : ordered_functions) {
    const auto effect_it = function_effects.find(name);
    if (effect_it == function_effects.end()) {
      continue;
    }
    const PureContractCause direct_cause =
        DetermineDirectPureContractImpurityCause(effect_it->second);
    if (direct_cause.present) {
      impure_functions.insert(name);
      impure_causes[name] = direct_cause;
    }
  }
}

static void PropagatePureContractImpurityCauses(
    const std::vector<std::string> &ordered_functions,
    const std::unordered_map<std::string,
                             PureContractEffectInfo> &function_effects,
    const std::unordered_set<std::string> &defined_functions,
    const std::unordered_map<std::string, bool> &pure_annotations,
    std::unordered_set<std::string> &impure_functions,
    std::unordered_map<std::string, PureContractCause> &impure_causes) {
  bool changed = true;
  while (changed) {
    changed = false;
    for (const std::string &name : ordered_functions) {
      if (impure_functions.find(name) != impure_functions.end()) {
        continue;
      }
      const auto effect_it = function_effects.find(name);
      if (effect_it == function_effects.end()) {
        continue;
      }
      PureContractCause selected_cause = SelectTransitivePureContractCause(
          effect_it->second, defined_functions, pure_annotations,
          impure_functions, impure_causes);
      if (!selected_cause.present) {
        continue;
      }
      NormalizePureContractCauseDetail(selected_cause);
      impure_functions.insert(name);
      impure_causes[name] = selected_cause;
      changed = true;
    }
  }
}
