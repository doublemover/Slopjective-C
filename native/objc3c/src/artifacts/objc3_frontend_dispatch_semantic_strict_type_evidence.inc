void AccumulateIdClassSelObjectPointerTypecheckSite(
    bool id_spelling,
    bool class_spelling,
    bool sel_spelling,
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t active_spelling_count =
      (id_spelling ? 1u : 0u) + (class_spelling ? 1u : 0u) +
      (sel_spelling ? 1u : 0u) + (object_pointer_type_spelling ? 1u : 0u);
  if (active_spelling_count > 1u) {
    contract.deterministic = false;
  }

  if (id_spelling) {
    ++contract.id_typecheck_sites;
  }
  if (class_spelling) {
    ++contract.class_typecheck_sites;
  }
  if (sel_spelling) {
    ++contract.sel_typecheck_sites;
  }
  if (object_pointer_type_spelling) {
    ++contract.object_pointer_typecheck_sites;
    if (object_pointer_type_name.empty()) {
      contract.deterministic = false;
    }
  }

  if (active_spelling_count > 0u) {
    ++contract.total_typecheck_sites;
  }
}

void AccumulateIdClassSelObjectPointerTypecheckMethod(
    const Objc3MethodDecl &method,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  AccumulateIdClassSelObjectPointerTypecheckSite(
      method.return_id_spelling, method.return_class_spelling,
      method.return_sel_spelling, method.return_object_pointer_type_spelling,
      method.return_object_pointer_type_name, contract);
  for (const auto &param : method.params) {
    AccumulateIdClassSelObjectPointerTypecheckSite(
        param.id_spelling, param.class_spelling, param.sel_spelling,
        param.object_pointer_type_spelling, param.object_pointer_type_name,
        contract);
  }
}

template <typename Container>
void AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
    const Container &declarations,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateIdClassSelObjectPointerTypecheckSite(
          property.id_spelling, property.class_spelling, property.sel_spelling,
          property.object_pointer_type_spelling,
          property.object_pointer_type_name, contract);
    }
    for (const auto &method : declaration.methods) {
      AccumulateIdClassSelObjectPointerTypecheckMethod(method, contract);
    }
  }
}
