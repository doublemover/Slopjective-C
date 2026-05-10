  snapshot.draft_syntax_surface_count =
      ast.draft_syntax_surface_summary.draft_syntax_surface_sites;
  snapshot.draft_syntax_surface_fingerprint =
      BuildObjc3DraftSyntaxSurfaceFingerprint(ast.draft_syntax_surface_summary);
  snapshot.draft_syntax_surface_handoff_key =
      ast.draft_syntax_surface_summary.replay_key;
  snapshot.draft_syntax_surface_handoff_deterministic =
      ast.draft_syntax_surface_summary.normalized &&
      snapshot.draft_syntax_surface_fingerprint != 0u &&
      !snapshot.draft_syntax_surface_handoff_key.empty();
  snapshot.ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(program);
  snapshot.ast_top_level_layout_fingerprint = BuildObjc3ParsedProgramTopLevelLayoutFingerprint(program);
  snapshot.top_level_declaration_count = snapshot.global_decl_count + snapshot.protocol_decl_count +
                                         snapshot.interface_decl_count + snapshot.implementation_decl_count +
                                         snapshot.function_decl_count;
