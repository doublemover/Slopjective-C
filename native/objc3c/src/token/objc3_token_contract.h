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

// source-surface/frontend-closure anchor: bootstrap
// registration descriptor and image-root identifiers are file-scope prelude
// pragmas rather than new parser tokens so later frontend/lowering/runtime
// work can extend one deterministic source contract and emitted
// registration-descriptor artifact boundary without widening the base token
// stream.
inline constexpr const char *kObjc3BootstrapRegistrationDescriptorPragmaName =
    "objc_registration_descriptor";
inline constexpr const char *kObjc3BootstrapImageRootPragmaName =
    "objc_image_root";

// source-closure truth anchor: task/executor/cancellation source
// admission remains identifier-profile driven on top of the existing async
// token surface. This tranche does not reserve new `task`/`cancel` keywords;
// it freezes one deterministic source contract around async entry points,
// canonical `objc_executor(...)` attributes, and parser-owned task-runtime /
// cancellation symbol profiling.
inline constexpr const char *kObjc3TaskExecutorCancellationSourceClosureContractId =
    "objc3c.concurrency.task.executor.cancellation.source.closure.v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimTaskExecutorCancellationProfiles =
    "source-only:task-executor-cancellation-profiles";
inline constexpr const char *kObjc3ActorIsolationSendableSourceClosureContractId =
    "objc3c.concurrency.actor.isolation.sendable.source.closure.v1";
inline constexpr const char *kObjc3ActorMemberIsolationSourceClosureContractId =
    "objc3c.concurrency.actor.member.isolation.source.closure.v1";
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
inline constexpr const char *kObjc3ConcurrencyTaskGroupCancellationSourceClosureContractId =
    "objc3c.concurrency.task.group.cancellation.source.closure.v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimTaskCreationSites =
    "source-only:task-creation-sites";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimSupportedTaskGroupSurface =
    "source-only:supported-task-group-surface";
inline constexpr const char *kObjc3OwnershipSystemExtensionSourceClosureContractId =
    "objc3c.ownership.resource.borrowed.capture.source.closure.v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimResourceHandleAnnotations =
    "source-only:resource-handle-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimBorrowedPointerAnnotations =
    "source-only:borrowed-pointer-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimBorrowedReturnRelations =
    "source-only:borrowed-return-relations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimExplicitBlockCaptureLists =
    "source-only:explicit-block-capture-lists";
inline constexpr const char *kObjc3OwnershipCleanupResourceCaptureSurfaceCompletionContractId =
    "objc3c.ownership.cleanup.resource.capture.source.closure.v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimCleanupHookAnnotations =
    "source-only:cleanup-hook-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimCleanupResourceSugar =
    "source-only:cleanup-resource-sugar";
inline constexpr const char *kObjc3OwnershipRetainableCFamilySourceCompletionContractId =
    "objc3c.ownership.retainable.c.family.source.completion.v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimRetainableCFamilyCallableAnnotations =
    "source-only:retainable-c-family-callable-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimRetainableCFamilyCompatibilityAliases =
    "source-only:retainable-c-family-compatibility-aliases";
inline constexpr const char *kObjc3DispatchDispatchIntentSourceClosureContractId =
    "objc3c.dispatch.dispatch.intent.source.closure.v1";
inline constexpr const char *kObjc3DispatchDispatchIntentSourceCompletionContractId =
    "objc3c.dispatch.dispatch.intent.source.completion.v1";
inline constexpr const char *kObjc3MetaprogrammingMetaprogrammingSourceClosureContractId =
    "objc3c.metaprogramming.metaprogramming.source.closure.v1";
inline constexpr const char *kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionContractId =
    "objc3c.metaprogramming.macro.package.provenance.source.completion.v1";
inline constexpr const char *kObjc3MetaprogrammingPropertyBehaviorSourceCompletionContractId =
    "objc3c.metaprogramming.property.behavior.source.completion.v1";
inline constexpr const char *kObjc3InteropForeignImportSourceClosureContractId =
    "objc3c.interop.foreign.declaration.import.source.closure.v1";
inline constexpr const char *kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId =
    "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1";
inline constexpr const char
    *kObjc3InteropForeignSurfaceInterfacePreservationContractId =
        "objc3c.interop.foreign.surface.interface.preservation.v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimDeriveMarkers =
    "source-only:derive-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimMacroMarkers =
    "source-only:macro-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimPropertyBehaviorMarkers =
    "source-only:property-behavior-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimPropertyBehaviorSynthesisVisibility =
    "source-only:property-behavior-synthesis-visibility";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimMacroPackageMarkers =
    "source-only:macro-package-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimMacroProvenanceMarkers =
    "source-only:macro-provenance-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimMacroExpansionVisibleState =
    "source-only:macro-expansion-visible-state";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimForeignDeclarationMarkers =
    "source-only:foreign-declaration-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimImportedModuleAnnotations =
    "source-only:imported-module-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimInteropAnnotationMarkers =
    "source-only:interop-annotation-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimSwiftFacingAnnotationMarkers =
    "source-only:swift-facing-annotation-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimCppFacingAnnotationMarkers =
    "source-only:cpp-facing-annotation-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimInteropMetadataAnnotationMarkers =
    "source-only:interop-metadata-annotation-markers";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimInteropNamedMetadataPayloads =
    "source-only:interop-named-metadata-payloads";
inline constexpr const char *kObjc3ToolingDiagnosticsMigratorSourceInventoryContractId =
    "objc3c.tooling.diagnostics.fixit.migrator.source.inventory.v1";
inline constexpr const char *kObjc3ToolingMigrationCanonicalizationSourceCompletionContractId =
    "objc3c.tooling.migration.canonicalization.source.completion.v1";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimAdvancedDiagnosticInventory =
    "source-only:advanced-diagnostic-inventory";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimAdvancedFixItInventory =
    "source-only:advanced-fixit-inventory";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimAdvancedMigratorInventory =
    "source-only:advanced-migrator-inventory";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimAdvancedCanonicalizationInventory =
    "source-only:advanced-canonicalization-inventory";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimAdvancedMigrationAssistFlow =
    "source-only:advanced-migration-assist-flow";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimDirectMethodAnnotations =
    "source-only:direct-method-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimDirectMembersClassAnnotations =
    "source-only:direct-members-class-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimFinalAnnotations =
    "source-only:final-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimSealedAnnotations =
    "source-only:sealed-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimDynamicMethodAnnotations =
    "source-only:dynamic-method-annotations";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimPrefixedDispatchIntentAttributes =
    "source-only:prefixed-dispatch-intent-attributes";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimDirectMembersDefaultingSurfaces =
    "source-only:direct-members-defaulting-surfaces";
inline constexpr const char *kObjc3SourceOnlyFeatureClaimDynamicOptOutDefaultingSurfaces =
    "source-only:dynamic-opt-out-defaulting-surfaces";

// source/mode truth anchor: advertised Objective-C 3 feature claims
// must be emitted from one canonical frontend inventory that separates runnable,
// source-only, and fail-closed unsupported surfaces.
inline constexpr const char *kObjc3RunnableFeatureClaimInventoryContractId =
    "objc3c.runnable.feature.claim.inventory.v1";
inline constexpr const char *kObjc3RunnableFeatureClaimModeName =
    "objc3-v1-native-subset";
inline constexpr const char *kObjc3RunnableFeatureClaimTruthModel =
    "truthful-runnable-subset-plus-source-only-plus-fail-closed-unsupported";
inline constexpr const char *kObjc3FeatureClaimStrictnessTruthSurfaceContractId =
    "objc3c.feature.claim.strictness.truth.surface.v1";
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
// Part 3 source-closure anchor: the frontend now admits protocol
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
inline constexpr const char *kObjc3ErrorHandlingFailClosedConstructTryExpressions =
    "error_handling-fail-closed:try-expressions";
inline constexpr const char *kObjc3ErrorHandlingFailClosedConstructThrowStatements =
    "error_handling-fail-closed:throw-statements";
inline constexpr const char *kObjc3ErrorHandlingFailClosedConstructDoCatchStatements =
    "error_handling-fail-closed:do-catch-statements";

// control-flow source+semantic surface anchor: the frontend now
// owns the Part 5 keyword classification boundary directly. Guard bindings,
// defer statements, and statement-form match stay explicit parser-owned
// surfaces while match-expression / guarded-pattern / type-test-pattern
// families remain fail-closed instead of drifting as ordinary identifiers.
inline constexpr const char *kObjc3ControlFlowSourceSurfaceGuardBindings =
    "control_flow-source:guard-bindings";
inline constexpr const char *kObjc3ControlFlowSourceSurfaceGuardConditionLists =
    "control_flow-source:guard-condition-lists";
inline constexpr const char *kObjc3ControlFlowSourceSurfaceSwitchCasePatterns =
    "control_flow-source:switch-case-patterns";
inline constexpr const char *kObjc3ControlFlowSourceSurfaceDeferStatements =
    "control_flow-source:defer-statements";
inline constexpr const char *kObjc3ControlFlowSourceSurfaceMatchStatement =
    "control_flow-source:match-statement";
inline constexpr const char *kObjc3ControlFlowSourceSurfaceMatchWildcardPatterns =
    "control_flow-source:match-wildcard-patterns";
inline constexpr const char *kObjc3ControlFlowSourceSurfaceMatchLiteralPatterns =
    "control_flow-source:match-literal-patterns";
inline constexpr const char *kObjc3ControlFlowSourceSurfaceMatchBindingPatterns =
    "control_flow-source:match-binding-patterns";
inline constexpr const char *kObjc3ControlFlowSourceSurfaceMatchResultCasePatterns =
    "control_flow-source:match-result-case-patterns";
inline constexpr const char *kObjc3ControlFlowFailClosedConstructMatchExpression =
    "control_flow-fail-closed:match-expression";
inline constexpr const char *kObjc3ControlFlowFailClosedConstructGuardedPatterns =
    "control_flow-fail-closed:guarded-match-patterns";
inline constexpr const char *kObjc3ControlFlowFailClosedConstructMatchTypeTestPatterns =
    "control_flow-fail-closed:match-type-test-patterns";

// runnable-core compatibility guard anchor: later advanced surfaces
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
