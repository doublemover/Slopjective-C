  Objc3SemaPassManagerResult result;
  const Objc3ParserSemaHandoffScaffold handoff = BuildObjc3ParserSemaHandoffScaffold(input);
  if (input.program == nullptr) {
    return result;
  }
  const Objc3SemaParserHandoffPublication handoff_publication =
      PublishObjc3ParserSemaHandoff(handoff, result);
  if (!handoff_publication.ready) {
    return result;
  }
