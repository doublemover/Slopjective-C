struct OwnershipBorrowedBindingState {
  bool borrowed = false;
};

using OwnershipBorrowedScope =
    std::unordered_map<std::string, OwnershipBorrowedBindingState>;

struct OwnershipBorrowedCallableContract {
  std::vector<bool> param_borrowed;
  bool return_borrowed = false;
};

struct OwnershipBorrowedReturnContract {
  bool valid = false;
};

static OwnershipBorrowedBindingState *LookupOwnershipBorrowedBinding(
    std::vector<OwnershipBorrowedScope> &scopes, const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

static const OwnershipBorrowedBindingState *LookupOwnershipBorrowedBinding(
    const std::vector<OwnershipBorrowedScope> &scopes, const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

static void MergeOwnershipBorrowedState(
    std::vector<OwnershipBorrowedScope> &target_scopes,
    const std::vector<OwnershipBorrowedScope> &branch_scopes) {
  const std::size_t scope_count =
      std::min(target_scopes.size(), branch_scopes.size());
  for (std::size_t scope_index = 0; scope_index < scope_count; ++scope_index) {
    for (auto &[name, target_state] : target_scopes[scope_index]) {
      auto branch_it = branch_scopes[scope_index].find(name);
      if (branch_it == branch_scopes[scope_index].end()) {
        continue;
      }
      target_state.borrowed = target_state.borrowed || branch_it->second.borrowed;
    }
  }
}

static OwnershipBorrowedCallableContract BuildOwnershipBorrowedCallableContract(
    const FunctionDecl &fn) {
  OwnershipBorrowedCallableContract contract;
  contract.param_borrowed.reserve(fn.params.size());
  for (const auto &param : fn.params) {
    contract.param_borrowed.push_back(param.borrowed_pointer_qualified);
  }
  contract.return_borrowed =
      fn.return_borrowed_pointer_qualified || fn.objc_returns_borrowed_declared;
  return contract;
}

static std::unordered_map<std::string, OwnershipBorrowedCallableContract>
BuildOwnershipBorrowedCallableContracts(const Objc3Program &program) {
  std::unordered_map<std::string, OwnershipBorrowedCallableContract> contracts;
  for (const auto &fn : program.functions) {
    contracts.emplace(fn.name, BuildOwnershipBorrowedCallableContract(fn));
  }
  return contracts;
}

static OwnershipBorrowedReturnContract BuildOwnershipBorrowedReturnContract(
    const FunctionDecl &fn) {
  OwnershipBorrowedReturnContract contract;
  contract.valid = fn.return_borrowed_pointer_qualified &&
                   fn.objc_returns_borrowed_declared &&
                   fn.objc_returns_borrowed_owner_index < fn.params.size();
  return contract;
}

static OwnershipBorrowedReturnContract BuildOwnershipBorrowedReturnContract(
    const Objc3MethodDecl &method) {
  OwnershipBorrowedReturnContract contract;
  contract.valid = method.return_borrowed_pointer_qualified &&
                   method.objc_returns_borrowed_declared &&
                   method.objc_returns_borrowed_owner_index < method.params.size();
  return contract;
}

static bool IsRetainableFamilyCanonicalOperationAttribute(
    const std::string &attribute_name) {
  return attribute_name == "objc_family_retain" ||
         attribute_name == "objc_family_release" ||
         attribute_name == "objc_family_autorelease";
}

static bool IsRetainableFamilyReturnAliasAttribute(
    const std::string &attribute_name) {
  return attribute_name == "os_returns_retained" ||
         attribute_name == "os_returns_not_retained" ||
         attribute_name == "cf_returns_retained" ||
         attribute_name == "cf_returns_not_retained" ||
         attribute_name == "ns_returns_retained" ||
         attribute_name == "ns_returns_not_retained";
}

static bool IsRetainableFamilyConsumedAliasAttribute(
    const std::string &attribute_name) {
  return attribute_name == "os_consumed" ||
         attribute_name == "cf_consumed" ||
         attribute_name == "ns_consumed";
}

template <typename TCallableDecl>
static bool HasObjCReferenceParameter(const TCallableDecl &decl) {
  return std::any_of(
      decl.params.begin(), decl.params.end(), [](const auto &param) {
        return IsObjCReferenceSemanticType(MakeSemanticTypeFromParam(param));
      });
}

template <typename TCallableDecl>
static bool ReturnsObjCReference(const TCallableDecl &decl) {
  if constexpr (std::is_same_v<TCallableDecl, FunctionDecl>) {
    return IsObjCReferenceSemanticType(MakeSemanticTypeFromFunctionReturn(decl));
  } else {
    return IsObjCReferenceSemanticType(MakeSemanticTypeFromMethodReturn(decl));
  }
}

template <typename TCallableDecl>
static bool ReturnsVoid(const TCallableDecl &decl) {
  if constexpr (std::is_same_v<TCallableDecl, FunctionDecl>) {
    return IsVoidSemanticType(MakeSemanticTypeFromFunctionReturn(decl));
  } else {
    return IsVoidSemanticType(MakeSemanticTypeFromMethodReturn(decl));
  }
}

template <typename TCallableDecl>
static void DiagnoseOwnershipRetainableFamilyCallableLegality(
    const TCallableDecl &decl, const std::string &callable_label,
    std::vector<std::string> &diagnostics) {
  std::size_t canonical_operation_count = 0;
  bool has_returns_retained_alias = false;
  bool has_returns_not_retained_alias = false;
  bool has_consumed_alias = false;
  bool has_retain = false;
  bool has_release = false;
  bool has_autorelease = false;

  for (const auto &attribute_name :
       decl.retainable_c_family_callable_attributes) {
    if (IsRetainableFamilyCanonicalOperationAttribute(attribute_name)) {
      ++canonical_operation_count;
    }
    if (attribute_name == "objc_family_retain") {
      has_retain = true;
    } else if (attribute_name == "objc_family_release") {
      has_release = true;
    } else if (attribute_name == "objc_family_autorelease") {
      has_autorelease = true;
    } else if (attribute_name == "os_returns_retained" ||
               attribute_name == "cf_returns_retained" ||
               attribute_name == "ns_returns_retained") {
      has_returns_retained_alias = true;
    } else if (attribute_name == "os_returns_not_retained" ||
               attribute_name == "cf_returns_not_retained" ||
               attribute_name == "ns_returns_not_retained") {
      has_returns_not_retained_alias = true;
    } else if (IsRetainableFamilyConsumedAliasAttribute(attribute_name)) {
      has_consumed_alias = true;
    }
  }

  if (canonical_operation_count > 1u ||
      (has_returns_retained_alias && has_returns_not_retained_alias)) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S304",
        "retainable-family legality failed: conflicting retainable-family annotations on " +
            callable_label));
  }

  const bool returns_objc_reference = ReturnsObjCReference(decl);
  const bool has_objc_reference_parameter = HasObjCReferenceParameter(decl);
  if ((has_retain || has_autorelease) &&
      (!returns_objc_reference || !has_objc_reference_parameter)) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S305",
        "retainable-family legality failed: " + callable_label +
            " must return an Objective-C reference and accept an Objective-C reference parameter for family retain/autorelease annotations"));
  } else if (has_release &&
             (!ReturnsVoid(decl) || !has_objc_reference_parameter)) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S305",
        "retainable-family legality failed: " + callable_label +
            " must return void and accept an Objective-C reference parameter for family release annotations"));
  }

  if ((has_returns_retained_alias || has_returns_not_retained_alias) &&
      !returns_objc_reference) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S306",
        "retainable-family legality failed: returns-retained compatibility aliases on " +
            callable_label +
            " require an Objective-C reference return type"));
  }
  if (has_consumed_alias && !has_objc_reference_parameter) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S306",
        "retainable-family legality failed: consumed compatibility aliases on " +
            callable_label +
            " require at least one Objective-C reference parameter"));
  }
}

#include "sema/objc3_semantic_passes_borrowed_ownership_diagnostics_pointer_escape.inc"
