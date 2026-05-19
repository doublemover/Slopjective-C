Objc3ControlFlowControlFlowSemanticModelSummary
BuildControlFlowControlFlowSemanticModelSummary(const Objc3Program &ast) {
  Objc3ControlFlowControlFlowSemanticModelSummary summary;
  for (const auto &function : ast.functions) {
    CollectControlFlowControlFlowSemanticStmtSitesFromList(function.body, 0, 0, 0, 0,
                                                     false, summary);
  }
  for (const auto &implementation : ast.implementations) {
    for (const auto &method : implementation.methods) {
      CollectControlFlowControlFlowSemanticStmtSitesFromList(method.body, 0, 0, 0, 0,
                                                       false, summary);
    }
  }

  FinalizeControlFlowControlFlowSemanticModelReadiness(summary);
  PublishControlFlowControlFlowSemanticModelReplayKey(summary);
  return summary;
}
