  void PublishProgramProfiles(Objc3ParsedProgram &program) {
    FinalizeObjcProgramPropertyIvarLayoutClosure(program);
    ast_builder_.SetDraftSyntaxSurfaceSummary(
        program, BuildObjc3DraftSyntaxSurfaceSummary(program));
  }
