Objc3IdClassSelObjectPointerTypecheckContract
BuildIdClassSelObjectPointerTypecheckContract(const Objc3Program &program) {
  Objc3IdClassSelObjectPointerTypecheckContract contract;
  for (const auto &fn : program.functions) {
    AccumulateIdClassSelObjectPointerTypecheckSite(
        fn.return_id_spelling, fn.return_class_spelling,
        fn.return_sel_spelling, fn.return_object_pointer_type_spelling,
        fn.return_object_pointer_type_name, contract);
    for (const auto &param : fn.params) {
      AccumulateIdClassSelObjectPointerTypecheckSite(
          param.id_spelling, param.class_spelling, param.sel_spelling,
          param.object_pointer_type_spelling, param.object_pointer_type_name,
          contract);
    }
  }
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
      program.protocols, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
      program.interfaces, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
      program.implementations, contract);
  return contract;
}

Objc3PropertySynthesisIvarBindingContract
BuildPropertySynthesisIvarBindingContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  const Objc3PropertySynthesisIvarBindingSummary &summary =
      sema_parity_surface.property_synthesis_ivar_binding_summary;
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = summary.property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings =
      summary.property_synthesis_explicit_ivar_bindings;
  contract.property_synthesis_default_ivar_bindings =
      summary.property_synthesis_default_ivar_bindings;
  contract.interface_owned_property_synthesis_sites =
      summary.interface_owned_property_synthesis_sites;
  contract.implementation_property_redeclaration_sites =
      summary.implementation_property_redeclaration_sites;
  contract.ivar_binding_sites = summary.ivar_binding_sites;
  contract.ivar_binding_resolved = summary.ivar_binding_resolved;
  contract.ivar_binding_missing = summary.ivar_binding_missing;
  contract.ivar_binding_conflicts = summary.ivar_binding_conflicts;
  contract.deterministic =
      summary.deterministic &&
      sema_parity_surface.deterministic_property_synthesis_ivar_binding_handoff;
  return contract;
}

Objc3DispatchSurfaceClassificationContract
BuildDispatchSurfaceClassificationContract(const Objc3Program &program) {
  Objc3DispatchSurfaceClassificationContract contract;
  auto accumulate_expr = [&](const Expr *expr) {
    AccumulateDispatchSurfaceClassificationExpr(expr, contract);
  };
  auto accumulate_stmt = [&](const Stmt *stmt) {
    AccumulateDispatchSurfaceClassificationStmt(stmt, contract);
  };
  WalkProgramExecutableRoots(program, accumulate_expr, accumulate_stmt);
  return contract;
}
