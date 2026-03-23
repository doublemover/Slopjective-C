#pragma once

#include <string>
#include <utility>
#include <vector>

// Lexer output contract consumed across parser/lowering/IR boundaries.
enum class Objc3LexTokenKind {
  Eof,
  Identifier,
  Number,
  String,
  KwModule,
  KwLet,
  KwVar,
  KwFn,
  KwAsync,
  KwPure,
  KwExtern,
  KwReturn,
  KwIf,
  KwElse,
  KwGuard,
  KwDefer,
  KwDo,
  KwAwait,
  KwTry,
  KwThrow,
  KwCatch,
  KwFor,
  KwSwitch,
  KwMatch,
  KwCase,
  KwDefault,
  KwWhile,
  KwBreak,
  KwContinue,
  KwI32,
  KwBool,
  KwBOOL,
  KwNSInteger,
  KwNSUInteger,
  KwVoid,
  KwId,
  KwClass,
  KwSEL,
  KwProtocol,
  KwInstancetype,
  KwTrue,
  KwFalse,
  KwNil,
  KwAtInterface,
  KwAtImplementation,
  KwAtProtocol,
  KwAtRequired,
  KwAtOptional,
  KwAtProperty,
  KwAtKeypath,
  KwAtCleanup,
  KwAtResource,
  KwAtEnd,
  KwAtAutoreleasePool,
  LParen,
  RParen,
  LBracket,
  RBracket,
  LBrace,
  RBrace,
  Comma,
  Colon,
  Dot,
  Semicolon,
  Equal,
  PlusEqual,
  MinusEqual,
  StarEqual,
  SlashEqual,
  PercentEqual,
  AmpersandEqual,
  PipeEqual,
  CaretEqual,
  LessLessEqual,
  GreaterGreaterEqual,
  PlusPlus,
  MinusMinus,
  EqualEqual,
  Bang,
  BangEqual,
  Less,
  LessLess,
  LessEqual,
  Greater,
  GreaterGreater,
  GreaterEqual,
  Ampersand,
  Pipe,
  Caret,
  AndAnd,
  OrOr,
  Question,
  QuestionDot,
  QuestionQuestion,
  Tilde,
  Plus,
  Minus,
  Star,
  Slash,
  Percent
};

struct Objc3LexToken {
  Objc3LexTokenKind kind = Objc3LexTokenKind::Eof;
  std::string text;
  unsigned line = 1;
  unsigned column = 1;
};

using Objc3LexTokenStream = std::vector<Objc3LexToken>;

// M263-A001/A002 source-surface/frontend-closure anchor: bootstrap
// registration descriptor and image-root identifiers are file-scope prelude
// pragmas rather than new parser tokens so later frontend/lowering/runtime
// work can extend one deterministic source contract and emitted
// registration-descriptor artifact boundary without widening the base token
// stream.
inline constexpr const char *kObjc3BootstrapRegistrationDescriptorPragmaName =
    "objc_registration_descriptor";
inline constexpr const char *kObjc3BootstrapImageRootPragmaName =
    "objc_image_root";

// M269-A001 source-closure truth anchor: task/executor/cancellation source
// admission remains identifier-profile driven on top of the existing async
// token surface. This tranche does not reserve new `task`/`cancel` keywords;
// it freezes one deterministic source contract around async entry points,
// canonical `objc_executor(...)` attributes, and parser-owned task-runtime /
// cancellation symbol profiling.
inline constexpr const char *kObjc3TaskExecutorCancellationSourceClosureContractId =
    "objc3c-part7-task-executor-cancellation-source-closure/m269-a001-v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimTaskExecutorCancellationProfiles =
    "source-only:task-executor-cancellation-profiles";
inline constexpr const char *kObjc3ActorIsolationSendableSourceClosureContractId =
    "objc3c-part7-actor-isolation-sendable-source-closure/m270-a001-v1";
inline constexpr const char *kObjc3ActorMemberIsolationSourceClosureContractId =
    "objc3c-part7-actor-member-isolation-source-closure/m270-a002-v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimActorDeclarationMarkers =
    "source-only:actor-declaration-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimIsolationAnnotationMarkers =
    "source-only:isolation-annotation-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimActorMemberSurfaces =
    "source-only:actor-member-surfaces";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimActorMetadataSurfaces =
    "source-only:actor-metadata-surfaces";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimSendableMarkers =
    "source-only:sendable-markers";
inline constexpr const char *kObjc3Part7TaskGroupCancellationSourceClosureContractId =
    "objc3c-part7-task-group-cancellation-source-closure/m269-a002-v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimTaskCreationSites =
    "source-only:task-creation-sites";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimSupportedTaskGroupSurface =
    "source-only:supported-task-group-surface";
inline constexpr const char *kObjc3Part8SystemExtensionSourceClosureContractId =
    "objc3c-part8-resource-borrowed-capture-source-closure/m271-a001-v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimResourceHandleAnnotations =
    "source-only:resource-handle-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimBorrowedPointerAnnotations =
    "source-only:borrowed-pointer-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimBorrowedReturnRelations =
    "source-only:borrowed-return-relations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimExplicitBlockCaptureLists =
    "source-only:explicit-block-capture-lists";
inline constexpr const char *kObjc3Part8CleanupResourceCaptureSurfaceCompletionContractId =
    "objc3c-part8-cleanup-resource-capture-source-closure/m271-a002-v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimCleanupHookAnnotations =
    "source-only:cleanup-hook-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimCleanupResourceSugar =
    "source-only:cleanup-resource-sugar";

// M264-A001 source/mode truth anchor: advertised Objective-C 3 feature claims
// must be emitted from one canonical frontend inventory that separates runnable,
// source-only, and fail-closed unsupported surfaces.
inline constexpr const char *kObjc3RunnableFeatureClaimInventoryContractId =
    "objc3c-runnable-feature-claim-inventory/m264-a001-v1";
inline constexpr const char *kObjc3RunnableFeatureClaimModeName =
    "objc3-v1-native-subset";
inline constexpr const char *kObjc3RunnableFeatureClaimTruthModel =
    "truthful-runnable-subset-plus-source-only-plus-fail-closed-unsupported";
inline constexpr const char *kObjc3FeatureClaimStrictnessTruthSurfaceContractId =
    "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1";
inline constexpr const char *kObjc3FeatureClaimStrictnessTruthDriverSurfaceModel =
    "language-version-and-compatibility-live-strictness-and-feature-macro-fail-closed";

inline constexpr const char *kObjc3RunnableFeatureClaimModule =
    "runnable:module-declaration";
inline constexpr const char *kObjc3RunnableFeatureClaimGlobalLet =
    "runnable:global-let";
inline constexpr const char *kObjc3RunnableFeatureClaimFunctionBodies =
    "runnable:function-bodies";
inline constexpr const char *kObjc3RunnableFeatureClaimExternPrototypes =
    "runnable:extern-prototypes";
inline constexpr const char *kObjc3RunnableFeatureClaimScalarCore =
    "runnable:scalar-core";
inline constexpr const char *kObjc3RunnableFeatureClaimControlFlow =
    "runnable:control-flow";
inline constexpr const char *kObjc3RunnableFeatureClaimMessageSend =
    "runnable:message-send-basic";

inline constexpr const char *kObjc3SourceOnlyFeatureClaimProtocols =
    "source-only:protocol-declarations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimInterfaces =
    "source-only:interface-declarations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimImplementations =
    "source-only:implementation-declarations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimCategories =
    "source-only:category-declarations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimProperties =
    "source-only:property-declarations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimObjectPointerSurface =
    "source-only:object-pointer-nullability-generics";
// M265 Part 3 source-closure anchor: the frontend now admits protocol
// optional partitions, object-pointer nullability/generic suffix carriers,
// optional bindings/sends/member-access/coalescing, and typed key-path
// literals as explicit source surfaces. Typed key-path execution still remains
// deferred.
inline constexpr const char *kObjc3SourceOnlyFeatureClaimProtocolOptionalPartitions =
    "source-only:protocol-optional-partitions";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimObjectPointerNullabilitySuffixes =
    "source-only:object-pointer-nullability-suffixes";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimPragmaticGenericSuffixes =
    "source-only:pragmatic-generic-suffixes";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimOptionalBindings =
    "source-only:optional-bindings";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimOptionalSends =
    "source-only:optional-sends";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimNilCoalescing =
    "source-only:nil-coalescing";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimTypedKeyPathLiterals =
    "source-only:typed-keypath-literals";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimThrowsDeclarations =
    "source-only:throws-declarations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimAsyncDeclarations =
    "source-only:async-declarations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimAwaitExpressions =
    "source-only:await-expressions";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimExecutorAffinityAttributes =
    "source-only:executor-affinity-attributes";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimResultCarrierProfiles =
    "source-only:result-carrier-profiles";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles =
    "source-only:nserror-bridging-profiles";
inline constexpr const char *kObjc3Part6FailClosedConstructTryExpressions =
    "part6-fail-closed:try-expressions";
inline constexpr const char *kObjc3Part6FailClosedConstructThrowStatements =
    "part6-fail-closed:throw-statements";
inline constexpr const char *kObjc3Part6FailClosedConstructDoCatchStatements =
    "part6-fail-closed:do-catch-statements";

// M266-A001/B003 control-flow source+semantic surface anchor: the frontend now
// owns the Part 5 keyword classification boundary directly. Guard bindings,
// defer statements, and statement-form match stay explicit parser-owned
// surfaces while match-expression / guarded-pattern / type-test-pattern
// families remain fail-closed instead of drifting as ordinary identifiers.
inline constexpr const char *kObjc3Part5SourceSurfaceGuardBindings =
    "part5-source:guard-bindings";
inline constexpr const char *kObjc3Part5SourceSurfaceGuardConditionLists =
    "part5-source:guard-condition-lists";
inline constexpr const char *kObjc3Part5SourceSurfaceSwitchCasePatterns =
    "part5-source:switch-case-patterns";
inline constexpr const char *kObjc3Part5SourceSurfaceDeferStatements =
    "part5-source:defer-statements";
inline constexpr const char *kObjc3Part5SourceSurfaceMatchStatement =
    "part5-source:match-statement";
inline constexpr const char *kObjc3Part5SourceSurfaceMatchWildcardPatterns =
    "part5-source:match-wildcard-patterns";
inline constexpr const char *kObjc3Part5SourceSurfaceMatchLiteralPatterns =
    "part5-source:match-literal-patterns";
inline constexpr const char *kObjc3Part5SourceSurfaceMatchBindingPatterns =
    "part5-source:match-binding-patterns";
inline constexpr const char *kObjc3Part5SourceSurfaceMatchResultCasePatterns =
    "part5-source:match-result-case-patterns";
inline constexpr const char *kObjc3Part5FailClosedConstructMatchExpression =
    "part5-fail-closed:match-expression";
inline constexpr const char *kObjc3Part5FailClosedConstructGuardedPatterns =
    "part5-fail-closed:guarded-match-patterns";
inline constexpr const char *kObjc3Part5FailClosedConstructMatchTypeTestPatterns =
    "part5-fail-closed:match-type-test-patterns";

// M259-B001 runnable-core compatibility guard anchor: later advanced surfaces
// remain explicitly non-runnable claim families until dedicated runtime-backed
// support lands.
inline constexpr const char *kObjc3UnsupportedFeatureClaimStrictness =
    "unsupported:strictness-selection";
inline constexpr const char *kObjc3UnsupportedFeatureClaimStrictConcurrency =
    "unsupported:strict-concurrency-selection";
inline constexpr const char *kObjc3UnsupportedFeatureClaimThrows =
    "unsupported:throws";
inline constexpr const char *kObjc3UnsupportedFeatureClaimAsyncAwait =
    "unsupported:async-await";
inline constexpr const char *kObjc3UnsupportedFeatureClaimActors =
    "unsupported:actors";
inline constexpr const char *kObjc3UnsupportedFeatureClaimBlocks =
    "unsupported:blocks";
inline constexpr const char *kObjc3UnsupportedFeatureClaimArc =
    "unsupported:arc";
inline constexpr const char *kObjc3UnsupportedFeatureClaimOptionalMemberAccess =
    "unsupported:optional-member-access";

inline constexpr const char *kObjc3SupportedSelectionSurfaceLanguageVersion =
    "selection:language-version";
inline constexpr const char *kObjc3SupportedSelectionSurfaceCompatibilityMode =
    "selection:compatibility-mode";
inline constexpr const char *kObjc3SupportedSelectionSurfaceMigrationAssist =
    "selection:migration-assist";
inline constexpr const char *kObjc3UnsupportedSelectionSurfaceStrictness =
    "selection:strictness";
inline constexpr const char *kObjc3UnsupportedSelectionSurfaceStrictConcurrency =
    "selection:strict-concurrency";

inline constexpr const char *kObjc3SuppressedMacroClaimStrictnessLevel =
    "macro-claim:__OBJC3_STRICTNESS_LEVEL__";
inline constexpr const char *kObjc3SuppressedMacroClaimConcurrencyMode =
    "macro-claim:__OBJC3_CONCURRENCY_MODE__";
inline constexpr const char *kObjc3SuppressedMacroClaimConcurrencyStrict =
    "macro-claim:__OBJC3_CONCURRENCY_STRICT__";

enum class Objc3SemaTokenKind {
  PointerDeclarator,
  NullabilitySuffix,
  OwnershipQualifier,
};

struct Objc3SemaTokenMetadata {
  Objc3SemaTokenKind kind = Objc3SemaTokenKind::PointerDeclarator;
  std::string text;
  unsigned line = 1;
  unsigned column = 1;
};

inline Objc3SemaTokenMetadata MakeObjc3SemaTokenMetadata(Objc3SemaTokenKind kind, std::string text, unsigned line,
                                                         unsigned column) {
  Objc3SemaTokenMetadata metadata;
  metadata.kind = kind;
  metadata.text = std::move(text);
  metadata.line = line;
  metadata.column = column;
  return metadata;
}
