#pragma once

struct Objc3FrontendPipelineResult;
struct Objc3FrontendOptions;
struct Objc3LexerCanonicalLiteralRejectionCounts;
struct Objc3SemanticValidationOptions;
struct Objc3SemaPassManagerInput;
struct Objc3SemaPassManagerResult;

void CaptureObjc3FrontendCanonicalLiteralRejections(
    Objc3FrontendPipelineResult &result,
    const Objc3LexerCanonicalLiteralRejectionCounts &lexer_counts);

void PopulateObjc3FrontendSemaInputHandoff(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options,
    const Objc3SemanticValidationOptions &semantic_options,
    Objc3SemaPassManagerInput &sema_input);

void AdoptObjc3FrontendSemaResult(Objc3FrontendPipelineResult &result,
                                  Objc3SemaPassManagerResult &&sema_result);
