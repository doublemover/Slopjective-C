  Objc3DraftSyntaxSurfaceSummary BuildObjc3DraftSyntaxSurfaceSummary(
      const Objc3ParsedProgram &program) const {
    Objc3DraftSyntaxSurfaceSummary summary;
    const Objc3Program &ast = Objc3ParsedProgramAst(program);
    for (const auto &global : ast.globals) {
      CountDraftSyntaxExpr(global.value.get(), summary);
    }
    for (const auto &protocol_decl : ast.protocols) {
      CountDraftSyntaxProperties(protocol_decl.properties, summary);
      for (const auto &method : protocol_decl.methods) {
        CountDraftSyntaxCallable(method, summary);
      }
    }
    for (const auto &interface_decl : ast.interfaces) {
      if (interface_decl.is_actor) {
        ++summary.actor_interface_sites;
      }
      CountDraftSyntaxProperties(interface_decl.properties, summary);
      for (const auto &method : interface_decl.methods) {
        CountDraftSyntaxCallable(method, summary);
      }
    }
    for (const auto &implementation_decl : ast.implementations) {
      CountDraftSyntaxProperties(implementation_decl.properties, summary);
      for (const auto &method : implementation_decl.methods) {
        CountDraftSyntaxCallable(method, summary);
      }
    }
    for (const auto &function : ast.functions) {
      CountDraftSyntaxCallable(function, summary);
    }
    summary.draft_syntax_surface_sites =
        CountObjc3DraftSyntaxSurfaceSites(summary);
    summary.replay_key = BuildObjc3DraftSyntaxSurfaceReplayKey(summary);
    summary.normalized = IsObjc3DraftSyntaxSurfaceSummaryNormalized(summary);
    return summary;
  }
