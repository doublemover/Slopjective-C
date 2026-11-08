#pragma once

#include <string>
#include <vector>

namespace objc3::clang_foundation {

inline constexpr unsigned kMaxInterfaceExportNames = 4096;
inline constexpr unsigned kInterfaceSchemaMajor = 2;
inline constexpr unsigned kInterfaceSchemaMinor = 2;
inline constexpr const char *kInterfaceSchemaId =
    "objc3c.semantic-interface.v2";

// Identity fields that can change the meaning or link contract of an import.
// File paths and timestamps intentionally do not participate.
struct InterfaceModuleIdentity {
  std::string canonical_name;
  std::string semantic_fingerprint;
  std::string target_triple;
  std::string runtime_abi;
  std::string extension_abi;
  std::string language_revision;
  std::string language_profile;
  std::string package_identity;
};

struct InterfaceEffectIdentity {
  bool is_async = false;
  bool is_throwing = false;
  std::string throws_kind;
  std::string error_type_identity;
  std::string actor_isolation_identity;
  std::string executor_affinity_identity;
  std::string ownership_identity;
  // Position-tagged physical entry ABI, including nested callable types.
  // Logical async effects alone cannot distinguish a C frame from task<T>.
  std::string async_abi_identity;
};

inline constexpr const char *kAsyncEntryABISemantic =
    "objc3.async-entry-abi.v5";

[[nodiscard]] bool
IsValidAsyncABIIdentity(const InterfaceEffectIdentity &effects);

struct InterfaceDeclarationIdentity {
  std::string stable_id; // Clang USR, normalized for file-scope C typedefs.
  std::string kind;
  std::string qualified_name;
  std::string canonical_type_identity;
  InterfaceEffectIdentity effects;
  // Empty for public API. A named SPI group is a required semantic contract.
  std::string spi_group;
  std::string native_symbol_name;
  // Native declaration owning this row's export policy. Class members and
  // friend functions bind to their class; namespace descendants retain scope.
  std::string export_root;
};

inline bool IsCxxMemberDeclarationKind(const std::string &kind) {
  return kind == "CXXMethod" || kind == "CXXConstructor" ||
         kind == "CXXDestructor" || kind == "CXXConversion";
}
inline constexpr const char *kCxxClassLifecycleSemantic =
    "objc3.cxx-class-lifecycles.v1";
inline bool
HasCxxClassLifecycleIdentity(const InterfaceDeclarationIdentity &d) {
  return d.kind == "CXXConstructor" || d.kind == "CXXDestructor" ||
         d.kind == "CXXConversion" ||
         d.canonical_type_identity.find(";cxx-class-lifecycle:v1;") !=
             std::string::npos;
}

inline constexpr const char *kCxxClassStructureSemantic =
    "objc3.cxx-class-structure.v1";
inline bool
HasCxxClassStructureIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-class-structure:v1;") !=
         std::string::npos;
}

inline constexpr const char *kCxxClassDispatchSemantic =
    "objc3.cxx-class-dispatch.v1";
inline bool
HasCxxClassDispatchIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-class-dispatch:v1;") !=
         std::string::npos;
}

inline constexpr const char *kCxxNamespaceSemantic =
    "objc3.cxx-namespaces.v1";
inline bool HasCxxNamespaceIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-namespaces:v1;") !=
         std::string::npos;
}

inline constexpr const char *kCxxNestedTypeSemantic =
    "objc3.cxx-nested-types.v1";
inline bool HasCxxNestedTypeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-nested-types:v1;") !=
         std::string::npos;
}

inline constexpr const char *kCxxUsingSemantic="objc3.cxx-using.v1";
inline bool HasCxxUsingIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-using:v1;")!=std::string::npos;
}
inline constexpr const char *kCxxDeclarationExpressionSemantic =
    "objc3.cxx-declaration-expressions.v1";
inline bool HasCxxDeclarationExpressionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-declaration-expressions:v1;") !=
         std::string::npos;
}
inline constexpr const char *kCInlineDefinitionSemantic = "objc3.c-inline-definitions.v1";
inline bool HasCInlineDefinitionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";c-inline-definitions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxDefinitionSemantic = "objc3.cxx-definitions.v1";
inline bool HasCxxDefinitionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-definitions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxDefaultedComparisonSemantic =
    "objc3.cxx-defaulted-comparisons.v1";
inline bool HasCxxDefaultedComparisonIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-defaulted-comparisons:v1;") != std::string::npos;
}
inline constexpr const char *kCxxOutOfLineDefaultingSemantic =
    "objc3.cxx-out-of-line-defaulting.v1";
inline bool HasCxxOutOfLineDefaultingIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-out-of-line-defaulting:v1;") != std::string::npos;
}
inline constexpr const char *kCxxImmediateDefaultingSemantic =
    "objc3.cxx-immediate-defaulting.v1";
inline bool HasCxxImmediateDefaultingIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-immediate-defaulting:v1;") != std::string::npos;
}
inline constexpr const char *kCxxNamespaceAttributeSemantic =
    "objc3.cxx-namespace-attributes.v1";
inline bool HasCxxNamespaceAttributeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-namespace-attributes:v1;") != std::string::npos;
}
inline constexpr const char *kCxxLambdaExpressionSemantic = "objc3.cxx-lambda-expressions.v1";
inline bool HasCxxLambdaExpressionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-lambda-expressions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxThreadLocalSemantic = "objc3.cxx-thread-local.v1";
inline bool HasCxxThreadLocalIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-thread-local:v1;") != std::string::npos;
}
inline constexpr const char *kCxxFriendSemantic = "objc3.cxx-friends.v1";
inline bool HasCxxFriendIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-friends:v1;") != std::string::npos;
}
inline constexpr const char *kCxxBlockExpressionSemantic = "objc3.cxx-block-expressions.v3";
inline bool HasCxxBlockExpressionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-block-expressions:v3;") != std::string::npos;
}
inline constexpr const char *kCxxStatementExpressionSemantic = "objc3.cxx-statement-expressions.v1";
inline bool HasCxxStatementExpressionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-statement-expressions:v1;") != std::string::npos;
}
inline constexpr const char *kSdkRuntimeDeclarationSemantic = "objc3.sdk-runtime-declarations.v1";
inline bool HasSdkRuntimeDeclarationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";sdk-runtime-declarations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxAsyncDefinitionSemantic = "objc3.cxx-async-definitions.v1";
inline bool HasCxxAsyncDefinitionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-async-definitions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxGenericLocalLambdaSemantic = "objc3.cxx-generic-local-lambdas.v2";
inline bool HasCxxGenericLocalLambdaIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-generic-local-lambdas:v2;") != std::string::npos;
}

inline constexpr const char *kCxxUnnamedLocalTypeSemantic = "objc3.cxx-unnamed-local-types.v1";
inline bool HasCxxUnnamedLocalTypeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-unnamed-local-types:v1;") != std::string::npos;
}

inline constexpr const char *kCxxUnionValueSemantic = "objc3.cxx-union-values.v1";
inline bool HasCxxUnionValueIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-union-values:v1;") != std::string::npos;
}
inline constexpr const char *kNativeDependentAwaitSemantic = "objc3.native-dependent-await.v1";
inline bool HasNativeDependentAwaitIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";native-dependent-await:v1;") != std::string::npos;
}
inline constexpr const char *kNSObjectStorageSemantic = "objc3.nsobject-storage.v1";
inline bool HasNSObjectStorageIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";nsobject-storage:v1;") != std::string::npos ||
         d.effects.ownership_identity.find(";nsobject-storage:v1;") != std::string::npos;
}
inline constexpr const char *kAutomaticOwnerUnwindSemantic = "objc3.automatic-owner-unwind.v2";
inline bool HasAutomaticOwnerUnwindIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";automatic-owner-unwind:v2;") != std::string::npos;
}
inline constexpr const char *kNativeArrayLifetimeSemantic = "objc3.native-array-lifetimes.v1";
inline bool HasNativeArrayLifetimeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";native-array-lifetimes:v1;") != std::string::npos;
}
inline constexpr const char *kCxxIncompleteTemplateSemantic = "objc3.cxx-incomplete-template-declarations.v2";
inline bool HasCxxIncompleteTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-incomplete-template-declarations:v2;") != std::string::npos;
}
inline constexpr const char *kNativeSubobjectSemantic = "objc3.native-subobject-cleanup.v5";
inline bool HasNativeSubobjectIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";native-subobject-cleanup:v5;") != std::string::npos;
}
inline constexpr const char *kNativeAllocationSemantic = "objc3.native-allocation-cleanup.v2";
inline bool HasNativeAllocationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";native-allocation-cleanup:v2;") != std::string::npos;
}
inline constexpr const char *kNativeDisposalSemantic = "objc3.native-disposal.v2";
inline bool HasNativeDisposalIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";native-disposal:v2;") != std::string::npos;
}
inline constexpr const char *kInitializedOwnerSemantic = "objc3.initialized-owner-prefixes.v1";
inline bool HasInitializedOwnerIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";initialized-owner-prefixes:v1;") != std::string::npos;
}
inline constexpr const char *kNativeFinallySemantic = "objc3.native-finally.v1";
inline bool HasNativeFinallyIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";native-finally:v1;") != std::string::npos;
}
inline constexpr const char *kManagedRuntimeScopeSemantic = "objc3.managed-runtime-scopes.v1";
inline bool HasManagedRuntimeScopeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";managed-runtime-scopes:v1;") != std::string::npos;
}
inline constexpr const char *kBorrowedReceiverResultSemantic = "objc3.borrowed-receiver-results.v1";
inline bool HasBorrowedReceiverResultIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";borrowed-receiver-results:v1;") != std::string::npos ||
         d.effects.ownership_identity.find(";borrowed-receiver-results:v1;") != std::string::npos;
}
inline constexpr const char *kBorrowedInteriorStorageSemantic = "objc3.borrowed-interior-storage.v1";
inline bool HasBorrowedInteriorStorageIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";borrowed-interior-storage:v1;") != std::string::npos;
}
inline constexpr const char *kBorrowedCallableOwnerSemantic = "objc3.borrowed-callable-owners.v1";
inline bool HasBorrowedCallableOwnerIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";borrowed-callable-owners:v1;") != std::string::npos ||
         d.effects.ownership_identity.find(";borrowed-callable-owners:v1;") != std::string::npos;
}
inline constexpr const char *kAsyncConsumedOwnershipSemantic = "objc3.async-consuming-ownership.v3";
inline bool HasAsyncConsumedOwnershipIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";async-consuming-ownership:v3;") != std::string::npos ||
         d.effects.ownership_identity.find(";async-consuming-ownership:v3;") != std::string::npos;
}
inline constexpr const char *kDependentKeyPathTypeSemantic = "objc3.keypath-dependent-types.v1";
inline bool HasDependentKeyPathTypeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";keypath-dependent-types:v1;") != std::string::npos ||
         d.effects.ownership_identity.find(";keypath-dependent-types:v1;") != std::string::npos;
}
inline constexpr const char *kDependentKeyPathLiteralSemantic = "objc3.keypath-dependent-literals.v1";
inline bool HasDependentKeyPathLiteralIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";keypath-dependent-literals:v1;") != std::string::npos;
}
inline constexpr const char *kCheckedKeyPathReadSemantic = "objc3.keypath-checked-reads.v1";
inline bool HasCheckedKeyPathReadIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";keypath-checked-reads:v1;") != std::string::npos;
}
inline constexpr const char *kNativeBlockAlignmentSemantic = "objc3.native-block-alignment.v1";
inline bool HasNativeBlockAlignmentIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";native-block-alignment:v1;") != std::string::npos;
}
inline constexpr const char *kMixedCarrierCallSemantic = "objc3.mixed-carrier-call.v1";
inline bool HasMixedCarrierCallIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";mixed-carrier-call:v1;") != std::string::npos;
}
inline constexpr const char *kCppCarrierRuntimeSemantic = "objc3.cxx-carrier-runtime.v2";
inline bool HasCppCarrierRuntimeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-carrier-runtime:v2;") != std::string::npos;
}
inline constexpr const char *kCheckedCppRecordResultSemantic = "objc3.checked-cxx-record-results.v1";
inline bool HasCheckedCppRecordResultIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";checked-cxx-record-results:v1;") != std::string::npos;
}
inline constexpr const char *kCppCarrierStorageSemantic = "objc3.cxx-carrier-storage.v1";
inline bool HasCppCarrierStorageIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-carrier-storage:v1;") != std::string::npos;
}
inline constexpr const char *kCheckedCppMessageSemantic = "objc3.checked-cxx-messages.v1";
inline bool HasCheckedCppMessageIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";checked-cxx-messages:v1;") != std::string::npos;
}
inline constexpr const char *kCppCarrierCompositionSemantic = "objc3.cxx-carrier-composition.v1";
inline bool HasCppCarrierCompositionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-carrier-composition:v1;") != std::string::npos ||
         d.effects.ownership_identity.find(";cxx-carrier-composition:v1;") != std::string::npos;
}
inline constexpr const char *kCppCarrierCallSemantic = "objc3.cxx-carrier-call.v2";
inline bool HasCppCarrierCallIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-carrier-call:v2;") != std::string::npos ||
         d.effects.ownership_identity.find(";cxx-carrier-call:v2;") != std::string::npos;
}
inline constexpr const char *kResourceKeyPathValueSemantic = "objc3.keypath-resource-values.v1";
inline bool HasResourceKeyPathValueIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";keypath-resource-values:v1;") != std::string::npos ||
         d.effects.ownership_identity.find(";keypath-resource-values:v1;") != std::string::npos;
}
inline constexpr const char *kNullableKeyPathWriteSemantic = "objc3.keypath-nullable-writes.v1";
inline bool HasNullableKeyPathWriteIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";keypath-nullable-writes:v1;") != std::string::npos;
}
inline constexpr const char *kNativeDeclarationAttributeSemantic = "objc3.native-declaration-attributes.v2";
inline bool HasNativeDeclarationAttributeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";native-declaration-attributes:v2;") != std::string::npos;
}
inline bool HasCxxCoroutineTraitsSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-coroutine-traits-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxUnnamedTemplateProjectionSemantic = "objc3.cxx-unnamed-record-projections.v2";
inline bool HasCxxUnnamedTemplateProjectionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-unnamed-record-projections:v2;") != std::string::npos;
}
inline constexpr const char *kCxxStandardLibrarySpecializationSemantic = "objc3.cxx-standard-library-specializations.v1";
inline bool HasCxxStandardLibrarySpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-standard-library-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxStandardLibrarySemantic = "objc3.cxx-standard-library-headers.v1";
inline constexpr const char *kCxxStandardLibraryMarker = ";cxx-standard-library-headers:v1;";
inline bool IsStandardLibraryHeaderName(const std::string &name) {
  static const std::string names = "|algorithm|any|array|atomic|barrier|bit|bitset|cassert|cctype|cerrno|cfenv|cfloat|charconv|chrono|cinttypes|climits|clocale|cmath|codecvt|compare|complex|concepts|condition_variable|coroutine|csetjmp|csignal|cstdarg|cstddef|cstdint|cstdio|cstdlib|cstring|ctime|cuchar|cwchar|cwctype|deque|exception|execution|expected|filesystem|flat_map|flat_set|format|forward_list|fstream|functional|future|generator|initializer_list|iomanip|ios|iosfwd|iostream|istream|iterator|latch|limits|list|map|mdspan|memory|memory_resource|mutex|new|numbers|numeric|optional|ostream|print|queue|random|ranges|ratio|regex|scope|scoped_allocator|semaphore|set|shared_mutex|source_location|span|spanstream|sstream|stack|stacktrace|stdexcept|stdfloat|stop_token|streambuf|string|string_view|strstream|syncstream|system_error|thread|tuple|type_traits|typeindex|typeinfo|unordered_map|unordered_set|utility|valarray|variant|vector|version|";
  return !name.empty() && name.find('|') == std::string::npos && names.find("|" + name + "|") != std::string::npos;
}
inline bool HasCxxStandardLibraryIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(kCxxStandardLibraryMarker) != std::string::npos;
}
// Header names are data-only, bounded and closed. Never turn arbitrary metadata
// text into a preprocessor include or accept a capability without its contract.
inline bool ReadCxxStandardLibraryHeaders(const InterfaceDeclarationIdentity &d,
                                          std::vector<std::string> &headers) {
  const auto &text=d.canonical_type_identity;
  const auto begin=text.find(kCxxStandardLibraryMarker);
  if(begin==std::string::npos) return true;
  const std::string prefix=std::string(kCxxStandardLibraryMarker)+"headers[";
  if(text.compare(begin,prefix.size(),prefix)!=0 ||
      text.find(kCxxStandardLibraryMarker,begin+prefix.size())!=std::string::npos) return false;
  auto start=begin+prefix.size();const auto end=text.find("];",start);
  if(end==std::string::npos || end==start) return false;
  std::string previous;unsigned count=0;
  while(start<end) {
    auto stop=text.find(',',start);if(stop==std::string::npos || stop>end)stop=end;
    const auto equals=text.find('=',start);
    if(equals==std::string::npos || equals>=stop || stop-equals-1!=64) return false;
    auto name=text.substr(start,equals-start);
    if(!IsStandardLibraryHeaderName(name) || (!previous.empty() && name<=previous) || ++count>128)return false;
    for(auto i=equals+1;i<stop;++i)if(!((text[i]>='0'&&text[i]<='9')||(text[i]>='a'&&text[i]<='f')))return false;
    headers.push_back(name);previous=std::move(name);
    if(stop==end)return true;
    start=stop+1;if(start==end)return false;
  }
  return false;
}
inline constexpr const char *kCxxStandardCoroutineSemantic = "objc3.cxx-standard-coroutines.v1";
inline bool HasCxxStandardCoroutineIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-standard-coroutines:v1;") != std::string::npos;
}
inline constexpr const char *kCxxCoroutineBodySemantic = "objc3.cxx-coroutine-bodies.v1";
inline bool HasCxxCoroutineBodyIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-coroutine-bodies:v1;") != std::string::npos;
}
inline constexpr const char *kForeignImportAttributeSemantic = "objc3.foreign-import-attributes.v1";
inline bool HasForeignImportAttributeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";foreign-import-attributes:v1;") != std::string::npos;
}
inline constexpr const char *kCxxNestedFactoryTypeSemantic = "objc3.cxx-nested-factory-types.v1";
inline bool HasCxxNestedFactoryTypeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-nested-factory-types:v1;") != std::string::npos;
}
inline constexpr const char *kCxxLambdaFactoryResultSemantic = "objc3.cxx-lambda-factory-results.v1";
inline bool HasCxxLambdaFactoryResultIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-lambda-factory-results:v1;") != std::string::npos;
}
inline constexpr const char *kCxxClosureResultProjectionSemantic = "objc3.cxx-closure-result-projections.v1";
inline bool HasCxxClosureResultProjectionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-closure-result-projections:v1;") != std::string::npos;
}
inline constexpr const char *kCxxFactoryValueArgumentSemantic = "objc3.cxx-factory-value-arguments.v1";
inline bool HasCxxFactoryValueArgumentIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-factory-value-arguments:v1;") != std::string::npos;
}
inline constexpr const char *kCxxGenericFactoryProjectionSemantic = "objc3.cxx-generic-factory-projections.v1";
inline bool HasCxxGenericFactoryProjectionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-generic-factory-projections:v1;") != std::string::npos;
}
inline constexpr const char *kCxxFactoryTemplateArgumentSemantic = "objc3.cxx-factory-template-arguments.v1";
inline bool HasCxxFactoryTemplateArgumentIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-factory-template-arguments:v1;") != std::string::npos;
}
inline constexpr const char *kCxxFactoryTypeProjectionSemantic = "objc3.cxx-factory-type-projections.v1";
inline bool HasCxxFactoryTypeProjectionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-factory-type-projections:v1;") != std::string::npos;
}
inline constexpr const char *kCxxLocalFactoryResultSemantic = "objc3.cxx-local-factory-results.v1";
inline bool HasCxxLocalFactoryResultIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-local-factory-results:v1;") != std::string::npos;
}
inline constexpr const char *kCxxClosureFactorySemantic = "objc3.cxx-closure-factories.v1";
inline bool HasCxxClosureFactoryIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-closure-factories:v1;") != std::string::npos;
}
inline constexpr const char *kCxxEffectfulLambdaSemantic = "objc3.cxx-effectful-lambdas.v1";
inline bool HasCxxEffectfulLambdaIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-effectful-lambdas:v1;") != std::string::npos;
}
inline constexpr const char *kCxxAddressTemplateArgumentSemantic = "objc3.cxx-address-template-arguments.v1";
inline bool HasCxxAddressTemplateArgumentIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-address-template-arguments:v1;") != std::string::npos;
}
inline constexpr const char *kCxxTemplateCallableProjectionSemantic = "objc3.cxx-template-callable-projections.v1";
inline bool HasCxxTemplateCallableProjectionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-template-callable-projections:v1;") != std::string::npos;
}
inline constexpr const char *kCxxEffectfulTemplateMemberSemantic = "objc3.cxx-effectful-template-members.v1";
inline bool HasCxxEffectfulTemplateMemberIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-effectful-template-members:v1;") != std::string::npos;
}
inline constexpr const char *kCxxEffectfulFunctionTemplateSemantic = "objc3.cxx-effectful-function-templates.v1";
inline bool HasCxxEffectfulFunctionTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.kind=="FunctionTemplate" &&
      d.canonical_type_identity.find(";cxx-effectful-function-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxClosedParentPatternSemantic = "objc3.cxx-closed-parent-patterns.v1";
inline bool HasCxxClosedParentPatternIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-closed-parent-patterns:v1;") != std::string::npos;
}
inline constexpr const char *kCxxClosedParentMemberSemantic = "objc3.cxx-closed-parent-members.v1";
inline bool HasCxxClosedParentMemberIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-closed-parent-members:v1;") != std::string::npos;
}
inline constexpr const char *kCxxDependentMemberPartialSemantic = "objc3.cxx-dependent-member-partials.v1";
inline bool HasCxxDependentMemberPartialIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-dependent-member-partials:v1;") != std::string::npos;
}
inline constexpr const char *kCxxConcreteSpecialMemberSpecializationSemantic = "objc3.cxx-concrete-special-member-specializations.v1";
inline bool HasCxxConcreteSpecialMemberSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-concrete-special-member-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxConcreteMemberVariableSpecializationSemantic = "objc3.cxx-concrete-member-variable-specializations.v1";
inline bool HasCxxConcreteMemberVariableSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-concrete-member-variable-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxConcreteMemberFunctionSpecializationSemantic = "objc3.cxx-concrete-member-function-specializations.v1";
inline bool HasCxxConcreteMemberFunctionSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-concrete-member-function-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxConcreteMemberClassSpecializationSemantic = "objc3.cxx-concrete-member-class-specializations.v1";
inline bool HasCxxConcreteMemberClassSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-concrete-member-class-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxDetachedNestedTemplateSemantic = "objc3.cxx-detached-nested-templates.v1";
inline bool HasCxxDetachedNestedTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-detached-nested-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxDetachedStaticDataTemplateSemantic = "objc3.cxx-detached-static-data-templates.v1";
inline bool HasCxxDetachedStaticDataTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-detached-static-data-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxDetachedCallableTemplateSemantic = "objc3.cxx-detached-callable-templates.v1";
inline bool HasCxxDetachedCallableTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-detached-callable-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxVariableSpecializationStorageSemantic = "objc3.cxx-variable-specialization-storage.v1";
inline bool HasCxxVariableSpecializationStorageIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-variable-specialization-storage:v1;") != std::string::npos;
}
inline constexpr const char *kCxxVariablePartialSpecializationSemantic = "objc3.cxx-variable-partial-specializations.v1";
inline bool HasCxxVariablePartialSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-variable-partial-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxClassPartialSpecializationSemantic = "objc3.cxx-class-partial-specializations.v1";
inline bool HasCxxClassPartialSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-class-partial-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxClassSpecializationSemantic = "objc3.cxx-class-specializations.v1";
inline bool HasCxxClassSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-class-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxVariableSpecializationSemantic = "objc3.cxx-variable-specializations.v1";
inline bool HasCxxVariableSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-variable-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxFunctionSpecializationSemantic = "objc3.cxx-function-specializations.v1";
inline bool HasCxxFunctionSpecializationIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-function-specializations:v1;") != std::string::npos;
}
inline constexpr const char *kCxxTemplateTemplateSemantic = "objc3.cxx-template-template-parameters.v1";
inline bool HasCxxTemplateTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-template-template-parameters:v1;") != std::string::npos;
}
inline constexpr const char *kCxxNominalAliasTemplateSemantic = "objc3.cxx-nominal-alias-templates.v1";
inline bool HasCxxNominalAliasTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-nominal-alias-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxConcreteNestedTemplateSemantic = "objc3.cxx-concrete-nested-templates.v1";
inline bool HasCxxConcreteNestedTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-concrete-nested-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxConcreteMemberTemplateSemantic = "objc3.cxx-concrete-member-templates.v1";
inline bool HasCxxConcreteMemberTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-concrete-member-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxVariadicTemplateSemantic = "objc3.cxx-variadic-templates.v1";
inline bool HasCxxVariadicTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-variadic-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxNestedTemplateSemantic = "objc3.cxx-nested-templates.v1";
inline bool HasCxxNestedTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-nested-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxMemberAliasTemplateSemantic = "objc3.cxx-member-alias-templates.v1";
inline bool HasCxxMemberAliasTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-member-alias-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxMemberTemplateSemantic = "objc3.cxx-member-templates.v1";
inline bool HasCxxMemberTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-member-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxTemplateDeductionSemantic = "objc3.cxx-template-deduction.v1";
inline bool HasCxxTemplateDeductionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-template-deduction:v1;") != std::string::npos;
}
inline constexpr const char *kCxxTemplateStructureSemantic = "objc3.cxx-template-structure.v1";
inline bool HasCxxTemplateStructureIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-template-structure:v1;") != std::string::npos;
}
inline constexpr const char *kCxxClassTemplateSemantic = "objc3.cxx-class-templates.v1";
inline bool HasCxxClassTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-class-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxVariableTemplateSemantic = "objc3.cxx-variable-templates.v1";
inline bool HasCxxVariableTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-variable-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxAliasTemplateSemantic = "objc3.cxx-alias-templates.v1";
inline bool HasCxxAliasTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-alias-templates:v1;") != std::string::npos;
}
inline constexpr const char *kCxxFunctionTemplateSemantic = "objc3.cxx-function-templates.v2";
inline bool HasCxxFunctionTemplateIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-function-templates:v2;") != std::string::npos;
}
inline constexpr const char *kCxxConceptSemantic = "objc3.cxx-concepts.v1";
inline bool HasCxxConceptIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-concepts:v1;") != std::string::npos;
}

inline constexpr const char *kCxxLocalTypeSemantic = "objc3.cxx-local-types.v1";
inline bool HasCxxLocalTypeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-local-types:v1;") != std::string::npos;
}
inline bool IsCxxOverloadDeclaration(const InterfaceDeclarationIdentity &d) {
  return d.kind=="Function" || d.kind=="FunctionTemplate" || IsCxxMemberDeclarationKind(d.kind) ||
         (d.kind=="Using" && d.canonical_type_identity.find(";functions=1;")!=std::string::npos);
}

// Compare the complete declaration contract, including every modeled effect.
[[nodiscard]] bool
SameInterfaceDeclarationIdentity(const InterfaceDeclarationIdentity &left,
                                 const InterfaceDeclarationIdentity &right);

struct InterfaceDependency {
  InterfaceModuleIdentity module;
  bool reexported = false;
  bool public_import = false;
};

// Canonical source and exported identities needed to parse a dependent
// interface through Clang's normal frontend. The path is a stable virtual
// include name; it is never a module identity or a filesystem authority.
struct InterfaceSourceArtifact {
  InterfaceModuleIdentity module;
  std::string virtual_include_path;
  std::string source_bytes;
  std::vector<InterfaceDeclarationIdentity> declarations;
  std::vector<InterfaceDependency> dependencies;
};

// `required_semantics` names versioned semantics whose absence changes meaning.
// Optional producer hints belong elsewhere and are ignored by importers.
struct SemanticInterfaceSidecar {
  unsigned schema_major = kInterfaceSchemaMajor;
  unsigned schema_minor = kInterfaceSchemaMinor;
  InterfaceModuleIdentity module;
  std::string canonical_source_sha256;
  // Canonical producer sidecar for a verified opposite-ownership C ABI view.
  // Its source binding names these exact canonical declaration bytes.
  std::string ownership_projection;
  std::vector<std::string> required_semantics;
  std::vector<InterfaceDeclarationIdentity> declarations;
  std::vector<InterfaceDependency> dependencies;
};

struct InterfaceContractDiagnostic {
  std::string code;
  std::string message;
  std::string subject;
};

struct InterfaceEmissionArtifact {
  bool success = false;
  std::string source_bytes;
  std::string sidecar_json;
  InterfaceModuleIdentity identity;
  std::vector<InterfaceContractDiagnostic> diagnostics;
};

struct InterfaceContractValidation {
  std::vector<InterfaceContractDiagnostic> diagnostics;
  [[nodiscard]] bool ok() const { return diagnostics.empty(); }
};

[[nodiscard]] bool IsValidSPIGroup(const std::string &group);
[[nodiscard]] bool IsGrantedSPI(const InterfaceDeclarationIdentity &declaration,
                                const std::vector<std::string> &groups);

// Stable fingerprint input sorts semantically unordered collections and omits
// source formatting and its own fingerprint. The sidecar codec owns JSON.
[[nodiscard]] std::string
BuildSemanticFingerprintInput(const SemanticInterfaceSidecar &sidecar);

[[nodiscard]] InterfaceContractValidation ValidateSemanticInterface(
    const SemanticInterfaceSidecar &sidecar,
    const InterfaceModuleIdentity &expected,
    const std::string &canonical_source_sha256,
    const std::vector<InterfaceDeclarationIdentity> &parsed_clang_declarations,
    const std::vector<std::string> &supported_required_semantics);

} // namespace objc3::clang_foundation
