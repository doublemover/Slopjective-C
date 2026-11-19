#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/runtime_interface_types.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/StringSaver.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <stdexcept>
#include <vector>
using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
void Require(bool condition, const std::string &message) {
  if (!condition) throw std::runtime_error(message);
}
void Compiled(const CompileResult &result) {
  if (result.success) return;
  std::cerr << result.failure << '\n';
  for (const auto &item : result.diagnostics)
    std::cerr << item.spelling.file << ':' << item.spelling.line << ':'
              << item.spelling.column << ": " << item.message << '\n';
  for (const auto &item : result.interface_import_diagnostics) std::cerr << item.message << '\n';
  if (result.interface_artifact)
    for (const auto &item : result.interface_artifact->diagnostics)
      std::cerr << item.code << ": " << item.message << " [" << item.subject << "]\n";
  throw std::runtime_error("native compilation failed");
}
void Write(const fs::path &path, const std::string &text) {
  std::ofstream stream(path); stream << text; Require(stream.good(), "write failed: " + path.string());
}
void Execute(const std::vector<std::string> &command) {
  std::vector<llvm::StringRef> arguments(command.begin(), command.end());
  Require(llvm::sys::ExecuteAndWait(command.front(), arguments) == 0, "command failed: " + command.front());
}
void LinkAndRun(const fs::path &root, const std::string &provider,
                const std::string &consumer, const std::string &extra_object = {}) {
  const auto program = (root / "program").string();
  std::vector<std::string> link{OBJC3_PUBLIC_ASYNC_LINKER, "--no-default-config",
                                "--target=" OBJC3_TEST_NATIVE_LINUX_TARGET};
  llvm::BumpPtrAllocator allocator;
  llvm::StringSaver saver(allocator);
  llvm::SmallVector<const char *, 16> flags;
  llvm::cl::TokenizeGNUCommandLine(OBJC3_PUBLIC_ASYNC_LINK_FLAGS, saver, flags);
  link.insert(link.end(), flags.begin(), flags.end());
  link.insert(link.end(), {"-fuse-ld=lld", provider, consumer});
  if (!extra_object.empty()) link.push_back(extra_object);
  for (const auto &argument : {
       std::string(OBJC3_PUBLIC_ASYNC_RUNTIME), std::string(OBJC3_PUBLIC_ASYNC_OBJC),
       "-Wl,-rpath," + fs::path(OBJC3_PUBLIC_ASYNC_OBJC).parent_path().string(),
       std::string("-pthread"), std::string("-ldl"), std::string("-o"), program})
    link.push_back(argument);
  Execute(link);
  Execute({program});
}
void Check(const std::string &standard, bool arc) {
  llvm::SmallString<128> temporary;
  Require(!llvm::sys::fs::createUniqueDirectory("objc3-public-async", temporary), "temporary directory failed");
  const fs::path root(temporary.c_str());
  struct Cleanup { fs::path path; ~Cleanup() { fs::remove_all(path); } } cleanup{root};
  const bool cxx = standard.starts_with("c++");
  const auto source = root / (cxx ? "provider.mm" : "provider.m");
  const std::string implementation = R"(
#include "objc3_runtime_services.h"
static int calls;
int keyword_count(void){return calls;}
int keyword_leaf(int value) async {++calls;return value+1;}
int keyword_parent(int value) async {return await keyword_leaf(value)+1;}
int keyword_indirect(int value) async {
  int (*const work)(int) async=keyword_leaf;
  return await work(value);
}
void keyword_void(void) async {++calls;}
)";
  Write(source, implementation);
  CompileRequest provider;
  provider.input_path = source.string(); provider.language = cxx ? BaseLanguage::ObjectiveCxx : BaseLanguage::ObjectiveC;
  provider.objc3 = true; provider.arc = arc; provider.objc_runtime = "gnustep-2.2";
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-O2", "-fblocks", std::string("-I") + OBJC3_PUBLIC_ASYNC_HEADERS};
  provider.emit = EmitKind::Object; provider.output_path = (root / "provider.o").string();
  provider.emit_semantic_interface = true; provider.module_name = "test.PublicAsync";
  provider.package_identity = "public-async-v1";
  provider.interface_exports = {"keyword_count", "keyword_leaf", "keyword_parent", "keyword_indirect", "keyword_void"};
  auto built = Compile(provider); Compiled(built);
  Require(built.interface_artifact && built.interface_artifact->success, "interface missing");
  const auto artifact = *built.interface_artifact;
  Require(artifact.source_bytes.find("__objc3_async_frame") != std::string::npos, "physical frame identity omitted");
  // Public syntax normalizes to the existing persistent frame ABI, preserving
  // the separate C++ task ABI rather than conflating two physical entries.
  auto intrinsic = implementation;
  const auto replace_all = [&](const std::string &from, const std::string &to) {
    for (auto position = intrinsic.find(from); position != std::string::npos;
         position = intrinsic.find(from, position + to.size()))
      intrinsic.replace(position, from.size(), to);
  };
  replace_all(" async", " __objc3_async_frame");
  replace_all("await ", "__objc3_await ");
  auto normalized = provider; normalized.source_text = intrinsic; normalized.emit = EmitKind::Syntax; normalized.output_path.clear();
  auto replay = Compile(normalized); Compiled(replay);
  Require(replay.interface_artifact && replay.interface_artifact->success, "normalized interface missing");
  Require(replay.interface_artifact->source_bytes == artifact.source_bytes &&
          replay.interface_artifact->identity.semantic_fingerprint == artifact.identity.semantic_fingerprint,
          "public spelling changed the physical or semantic interface");
  const auto interface = root / "PublicAsync.objc3i";
  Write(interface, artifact.source_bytes); Write(interface.string() + ".json", artifact.sidecar_json);
  Require(fs::remove(source), "provider source removal failed");
  CompileRequest consumer = provider;
  consumer.input_path = (root / (cxx ? "consumer.mm" : "consumer.m")).string();
  consumer.output_path = (root / "consumer.o").string();
  consumer.emit_semantic_interface = false; consumer.module_name.clear();
  consumer.package_identity.clear(); consumer.interface_exports.clear();
  InterfaceImportInput input; input.source_path = interface.string();
  input.sidecar_path = interface.string() + ".json"; input.locked_identity = artifact.identity;
  consumer.imported_interfaces = {input};
  consumer.source_text = R"(
#include "objc3_runtime_services.h"
static int execute(objc3_async_frame *frame,objc3_executor_t *executor,int expected,int empty,int cancelled){
  objc3_task_t *task=0;
  if(objc3_async_frame_start(frame,executor,0,&task)!=OBJC3_SERVICE_OK || frame->context)return 1;
  if(cancelled)objc3_task_request_cancel(task);
  unsigned executed=0;
  if(objc3_executor_pump(executor,1000,&executed)!=OBJC3_SERVICE_OK)return 3;
  objc3_completion completion={0};
  if(objc3_task_get_completion(task,&completion)!=OBJC3_SERVICE_OK)return 4;
  if(cancelled){if(completion.kind!=OBJC3_COMPLETION_CANCELLED)return 5;}
  else if(completion.kind!=OBJC3_COMPLETION_SUCCESS || completion.has_payload==empty)return 6;
  else if(!empty && (!completion.payload || *(const int *)completion.payload!=expected))return 7;
  objc3_task_release(task);return 0;
}
int main(void){
  objc3_executor_t *executor=0;
  if(objc3_executor_create(1,&executor)!=OBJC3_SERVICE_OK || keyword_count()!=0)return 10;
  objc3_async_frame parent=keyword_parent(40);
  if(keyword_count()!=0 || execute(&parent,executor,42,0,0))return 11;
  objc3_async_frame indirect=keyword_indirect(41);
  if(execute(&indirect,executor,42,0,0))return 12;
  objc3_async_frame empty=keyword_void();
  if(execute(&empty,executor,0,1,0) || keyword_count()!=3)return 13;
  objc3_async_frame cancelled=keyword_parent(40);
  if(execute(&cancelled,executor,0,0,1) || keyword_count()!=3)return 14;
  objc3_executor_release(executor);return 0;
}
)";
  Compiled(Compile(consumer));
  LinkAndRun(root, provider.output_path, consumer.output_path);
  std::cout << "Public async source-free execution " << standard << (arc ? " ARC\n" : " MRC\n");
}

void CheckGeneratedCCompletion(const std::string &standard, bool arc) {
  llvm::SmallString<128> temporary;
  Require(!llvm::sys::fs::createUniqueDirectory("objc3-generated-c-completion", temporary),
          "temporary directory failed");
  const fs::path root(temporary.c_str());
  struct Cleanup { fs::path path; ~Cleanup() { fs::remove_all(path); } } cleanup{root};
  const bool cxx = standard.starts_with("c++");
  const bool declaration_only = standard == "c17" && !arc;
  const auto source = root / (cxx ? "provider.mm" : "provider.m");
  const auto includes = root / "include";
  fs::create_directories(includes / "objc3c");
  fs::create_directory_symlink(OBJC3_PUBLIC_ASYNC_HEADERS, includes / "objc3c/runtime");
  Write(source, R"(
#include <assert.h>
#include <stdbool.h>
typedef int Integer;
typedef const Integer *Input;
@protocol Error @end
@protocol Sendable @end
@protocol GeneratedActorError <Error, Sendable> @end
__attribute__((objc_root_class)) @interface GeneratedError <Error>
+ (Class)class;
@end
@implementation GeneratedError
+ (Class)class { return self; }
@end
#ifdef __cplusplus
extern "C"
#endif
id<Error> generated_failure(void) { return (id<Error>)[GeneratedError class]; }
#ifdef DECLARATION_ONLY
Integer generated_value(bool enabled, Input value) async;
#else
static Integer generated_value(bool enabled, Input value) async {
  return enabled ? *value + 1 : 0;
}
#endif
static void generated_void(void) async {}
static void generated_cancelled(void) async throws(id<Error>) {
  try objc3_task_check_cancellation();
  __builtin_trap();
}
static int generated_checked(int mode) async throws(id<Error>) {
  if (mode) __objc3_throw(generated_failure());
  return 42;
}
static id<Error> _Nonnull generated_object(id<Error> _Nonnull input) async { return input; }
typedef int (^GeneratedBlock)(int);
typedef int (*GeneratedCallback)(int, void * _Nullable);
enum GeneratedMode : signed char { GeneratedModeInvalid = -4, GeneratedModeAnswer = 42 };
enum GeneratedWide : unsigned long long { GeneratedWideMax = 18446744073709551615ULL };
enum GeneratedSignedWide : long long { GeneratedSignedWideMin = -9223372036854775807LL - 1LL };
struct GeneratedEnumBox { enum GeneratedSignedWide minimum; enum GeneratedMode mode; long long values[2]; };
typedef struct GeneratedEnumBox (*GeneratedEnumTransform)(struct GeneratedEnumBox);
typedef struct GeneratedEnumBox (^GeneratedEnumBlock)(struct GeneratedEnumBox);
typedef enum GeneratedWide (*GeneratedEnumCallback)(enum GeneratedMode);
static enum GeneratedWide generated_enum(enum GeneratedMode mode, GeneratedEnumCallback callback) async {
  return callback(mode);
}
static GeneratedBlock generated_block(int input) async {
  return ^(int offset) { return input + offset; };
}
static GeneratedCallback _Nonnull generated_callback(GeneratedCallback _Nonnull value) async {
  return value;
}
struct GeneratedPort { char flag; int lane; };
struct GeneratedAggregate { struct GeneratedPort port; long values[2]; };
static struct GeneratedAggregate generated_aggregate(struct GeneratedAggregate value) async {
  ++value.port.lane;
  ++value.values[1];
  return value;
}
static int generated_main(int value) async __attribute__((nothrow, objc_executor(main))) {
  objc3_executor_t *executor = 0;
  if (objc3_executor_get_main(&executor) != OBJC3_SERVICE_OK) __builtin_trap();
  int current = objc3_executor_is_current(executor);
  objc3_executor_release(executor);
  return current ? value + 1 : -1;
}
static int generated_named(int value) async throws(id<Error>)
    __attribute__((nothrow, objc_executor(named("generated.completion")))) {
  objc3_executor_t *executor = 0;
  if (objc3_executor_get_named("generated.completion", &executor) != OBJC3_SERVICE_OK)
    __builtin_trap();
  int current = objc3_executor_is_current(executor);
  objc3_executor_release(executor);
  return current ? value + 1 : -1;
}
static void generated_missing(void) async
    __attribute__((nothrow, objc_executor(named("generated.missing")))) {
  __builtin_trap();
}

#ifdef __cplusplus
extern "C" {
#endif
extern id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern id object_dispose(id);
extern void *_Block_copy(const void *);
extern void _Block_release(const void *);
#if !__has_feature(objc_arc)
extern void objc_release(id);
extern id objc_autorelease(id);
#endif
#ifdef __cplusplus
}
#endif
static _Atomic(unsigned) generated_actor_destructions;
__attribute__((objc_root_class, objc_runtime_name("GeneratedCArgumentRoot")))
@interface GeneratedActorRoot { Class isa; }
+ (instancetype)new;
- (int)contribution;
@end
@implementation GeneratedActorRoot
+ (instancetype)new { return class_createInstance(self, 0); }
- (int)contribution { return 11; }
- (void)dealloc {
  __c11_atomic_fetch_add(&generated_actor_destructions, 1, __ATOMIC_RELAXED);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
static int generated_arguments(GeneratedActorRoot *input, GeneratedBlock apply) async {
  return [input contribution] + apply(20);
}
static __attribute__((nothrow)) int generated_consumed_sync(bool fail,
    GeneratedActorRoot *input __attribute__((ns_consumed)),
    GeneratedBlock apply __attribute__((ns_consumed)))
    __objc3_throws(id<Error>) {
  int result = [input contribution] + apply(20);
#if !__has_feature(objc_arc)
  objc_release(input);
  _Block_release((const void *)apply);
#endif
  if (fail) __objc3_throw(generated_failure());
  return result;
}
static int generated_consumed(bool fail,
    GeneratedActorRoot *input __attribute__((ns_consumed)),
    GeneratedBlock apply __attribute__((ns_consumed))) async throws(id<Error>) {
  int result = [input contribution] + apply(20);
#if !__has_feature(objc_arc)
  objc_release(input);
  _Block_release((const void *)apply);
#endif
  if (fail) __objc3_throw(generated_failure());
  return result;
}
static _Atomic(unsigned) generated_sync_errors_destroyed;
__attribute__((objc_root_class)) @interface GeneratedOwnedError <Error> { Class isa; }
+ (instancetype)new;
@end
@implementation GeneratedOwnedError
+ (instancetype)new {return class_createInstance(self, 0);}
- (void)dealloc {
  __c11_atomic_fetch_add(&generated_sync_errors_destroyed, 1, __ATOMIC_RELAXED);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
static int generated_sync_checked(int input) __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (input == -2) {
    id<Error> error = [GeneratedOwnedError new];
#if !__has_feature(objc_arc)
    objc_autorelease(error);
#endif
    __objc3_throw(error);
  }
  if (input < 0) __objc3_throw(generated_failure());
  return input + 1;
}
static void generated_sync_void(bool fail) __objc3_throws __attribute__((nothrow)) {
  if (fail) __objc3_throw(generated_failure());
}
static const int * _Nonnull generated_sync_pointer(const int * _Nullable value)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!value) __objc3_throw(generated_failure());
  return value;
}
static int generated_sync_nested(const int * _Nonnull * _Nullable value)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!value) __objc3_throw(generated_failure());
  return **value;
}
static id<Error> _Nonnull generated_sync_object(id<Error> _Nullable value)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!value) __objc3_throw(generated_failure());
  return value;
}
static GeneratedBlock _Nonnull generated_sync_block(GeneratedBlock _Nullable value)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!value) __objc3_throw(generated_failure());
  return value;
}
static int generated_sync_invoke(GeneratedCallback _Nullable callback, void * _Nullable context)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!callback) __objc3_throw(generated_failure());
  return callback(40, context);
}
static GeneratedCallback _Nonnull generated_sync_callback(GeneratedCallback _Nullable callback)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!callback) __objc3_throw(generated_failure());
  return callback;
}
static enum GeneratedMode generated_sync_enum(enum GeneratedMode mode)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (mode == GeneratedModeInvalid) __objc3_throw(generated_failure());
  return mode;
}
static struct GeneratedEnumBox generated_sync_enum_box(struct GeneratedEnumBox value,
                                                        GeneratedEnumTransform transform)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  value.mode = __objc3_try(generated_sync_enum(value.mode));
  return transform(value);
}
static struct GeneratedEnumBox generated_sync_enum_block(struct GeneratedEnumBox value,
                                                          GeneratedEnumBlock transform)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!transform) __objc3_throw(generated_failure());
  return transform(value);
}
static struct GeneratedPort generated_sync_port(struct GeneratedPort value)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!value.flag) __objc3_throw(generated_failure());
  ++value.lane;
  return value;
}
static struct GeneratedAggregate generated_sync_aggregate(struct GeneratedAggregate value)
    __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (!value.port.flag) __objc3_throw(generated_failure());
  ++value.port.lane;
  ++value.values[1];
  return value;
}
static int generated_abandoned_arguments(GeneratedActorRoot *input, GeneratedBlock apply) async
    __attribute__((nothrow, objc_executor(named("generated.missing")))) {
  (void)input;
  (void)apply;
  __builtin_trap();
}
actor class GeneratedActor : GeneratedActorRoot { int value; }
- (int)add:(int)left by:(int)right __attribute__((nothrow));
- (int)current __attribute__((nothrow));
- (int)checked:(bool)fail async throws(id<GeneratedActorError>) __attribute__((nothrow));
- (int)consume:(GeneratedActorRoot *)__attribute__((ns_consumed))input
    __attribute__((nothrow, ns_consumes_self));
- (void)ordinary __attribute__((objc_nonisolated));
@end
static void *generated_actor_pointer(GeneratedActor *object) {
#if __has_feature(objc_arc)
  return (__bridge void *)object;
#else
  return (void *)object;
#endif
}
static void generated_actor_check(GeneratedActor *object) {
  objc3_current_actor_facts facts = {sizeof(facts), OBJC3_ACTOR_SERVICE_VERSION};
  objc3_actor_t *bound = 0;
  assert(objc3_actor_current_facts(&facts) == OBJC3_SERVICE_OK);
  assert(objc3_actor_get_or_create_for_object(generated_actor_pointer(object), 0, &bound) == OBJC3_SERVICE_OK);
  assert(facts.actor_identity == objc3_actor_identity(bound));
  objc3_actor_release(bound);
}
@implementation GeneratedActor
- (int)add:(int)left by:(int)right {
  generated_actor_check(self);
  return value += left + right;
}
- (int)current { generated_actor_check(self); return value; }
- (int)checked:(bool)fail async throws(id<GeneratedActorError>) {
  generated_actor_check(self);
  if (fail) __objc3_throw((id<GeneratedActorError>)generated_failure());
  return 42;
}
- (int)consume:(GeneratedActorRoot *)input {
  generated_actor_check(self);
  int result = [input contribution] + 31;
#if !__has_feature(objc_arc)
  objc_release(input);
  objc_release(self);
#endif
  return result;
}
- (void)ordinary {}
@end
@interface GeneratedDerivedActor : GeneratedActor
@end
@implementation GeneratedDerivedActor
@end
#ifdef __cplusplus
extern "C" {
#endif
void *generated_actor_create(void) {
#if __has_feature(objc_arc)
  return (__bridge_retained void *)[GeneratedActor new];
#else
  return (void *)[GeneratedActor new];
#endif
}
void *generated_derived_actor_create(void) {
#if __has_feature(objc_arc)
  return (__bridge_retained void *)[GeneratedDerivedActor new];
#else
  return (void *)[GeneratedDerivedActor new];
#endif
}
void *generated_argument_create(void) {
#if __has_feature(objc_arc)
  return (__bridge_retained void *)[GeneratedActorRoot new];
#else
  return (void *)[GeneratedActorRoot new];
#endif
}
void *generated_argument_block_create(void) {
  GeneratedActorRoot *captured = [GeneratedActorRoot new];
  GeneratedBlock action = ^(int value) { return [captured contribution] + value; };
#if __has_feature(objc_arc)
  return _Block_copy((__bridge const void *)action);
#else
  void *copy = _Block_copy((const void *)action);
  objc_release(captured);
  return copy;
#endif
}
void generated_actor_release(void *actor) {
#if __has_feature(objc_arc)
  id owner = (__bridge_transfer id)actor;
  (void)owner;
#else
  objc_release((id)actor);
#endif
}
unsigned generated_actors_destroyed(void) {
  return __c11_atomic_load(&generated_actor_destructions, __ATOMIC_RELAXED);
}
unsigned generated_checked_errors_destroyed(void) {
  return __c11_atomic_load(&generated_sync_errors_destroyed, __ATOMIC_RELAXED);
}
#ifdef __cplusplus
}
namespace ExportRegion::Nested {
static int pulse(int input) async { return input + 1; }
static int check(int input) __objc3_throws(id<Error>) __attribute__((nothrow)) {
  if (input < 0) __objc3_throw(generated_failure());
  return input + 2;
}
}
namespace ExportAlias = ExportRegion::Nested;
#define ExportRegion InvalidExportRegion
#define ExportAlias InvalidExportAlias
#define pulse(...) invalid_if_reexpanded
#define check(...) invalid_if_reexpanded
#endif
#define generated_value(...) invalid_if_reexpanded
#define add invalid_if_reexpanded
#define GeneratedActor InvalidActor
)");
  CompileRequest provider;
  provider.input_path = source.string();
  provider.language = cxx ? BaseLanguage::ObjectiveCxx : BaseLanguage::ObjectiveC;
  provider.objc3 = true;
  provider.arc = arc;
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-fblocks", "-fobjc-exceptions",
                        "-I" + includes.string()};
  if (declaration_only) provider.arguments.push_back("-DDECLARATION_ONLY");
  provider.emit = EmitKind::Object;
  provider.output_path = (root / "provider.o").string();
  provider.async_c_exports = {"generated_value", "generated_void", "generated_checked",
      "generated_cancelled", "generated_consumed", "GeneratedActor::consume:",
      "generated_object", "generated_block", "generated_callback", "generated_enum", "generated_aggregate", "generated_main",
      "generated_named", "generated_missing", "generated_arguments",
      "generated_abandoned_arguments", "GeneratedActor::add:by:",
      "GeneratedActor::current", "GeneratedDerivedActor::current", "GeneratedDerivedActor::checked:"};
  provider.throws_c_exports = {"generated_sync_checked", "generated_sync_void", "generated_sync_pointer", "generated_sync_nested",
      "generated_sync_object", "generated_sync_block", "generated_sync_invoke", "generated_sync_callback",
      "generated_sync_port", "generated_sync_aggregate", "generated_consumed_sync", "generated_sync_enum",
      "generated_sync_enum_box", "generated_sync_enum_block"};
  if (cxx) {
    provider.async_c_exports.push_back("ExportRegion::Nested::pulse");
    provider.throws_c_exports.push_back("ExportAlias::check");
  }
  provider.c_export_header_output_path = (root / "generated.h").string();
  provider.emit_semantic_interface = true;
  provider.module_name = "test.GeneratedCompletion";
  provider.package_identity = "generated-completion-v1";
  provider.interface_exports = {"generated_value_async_c", "generated_void_async_c",
      "generated_checked_async_c", "generated_value_completion_t",
      "generated_void_completion_t", "generated_checked_completion_t",
      "generated_object_async_c", "generated_block_async_c", "Error", "GeneratedActorRoot"};
  auto generated = Compile(provider);
  Compiled(generated);
  Require(generated.interface_artifact.has_value(), "generated C interface missing");
  const auto &interface = *generated.interface_artifact;
  Require(interface.source_bytes.find("generated_checked_async_c") != std::string::npos &&
          interface.source_bytes.find("generated_checked_async_c_task") != std::string::npos &&
          interface.source_bytes.find("GeneratedActor_add_by_async_c") != std::string::npos &&
          interface.source_bytes.find("GeneratedDerivedActor_checked_async_c") != std::string::npos &&
          interface.source_bytes.find("generated_sync_checked_throws_c") != std::string::npos &&
          interface.source_bytes.find("generated_sync_enum_throws_c") != std::string::npos &&
          interface.sidecar_json.find(RuntimeCompletionSemantic()) != std::string::npos,
          "generated C callable ABI was omitted from semantic metadata");

  std::string separate_body;
  if (standard == "c++20" && !arc) {
    auto rejected = provider;
    rejected.async_c_exports.clear();
    rejected.throws_c_exports = {"generated_unmarked_checked"};
    std::ifstream original(source);
    Require(original.good(), "generated provider fixture is missing");
    const std::string source_text((std::istreambuf_iterator<char>(original)), {});
    rejected.source_text = source_text + R"(
static int generated_unmarked_checked(void) __objc3_throws {return 1;}
)";
    const auto unsafe = Compile(rejected);
    Require(!unsafe.success && unsafe.failure.find("nonthrowing native exception contract") != std::string::npos,
            "a potentially unwinding Objective-C++ function was admitted as a synchronous C export");
    rejected.throws_c_exports = {"ExportRegion::Nested::check", "ExportRegion_Nested::check"};
    rejected.source_text = source_text + R"(
#undef check
namespace ExportRegion_Nested {
static int check(int input) __objc3_throws __attribute__((nothrow)) {return input;}
}
)";
    const auto conflict = Compile(rejected);
    Require(!conflict.success && conflict.failure.find("already declared or selected") != std::string::npos,
            "two namespace selections published the same C symbol");
  }
  if (declaration_only) {
    auto rejected = provider;
    rejected.async_c_exports = {"generated_failure"};
    auto result = Compile(rejected);
    Require(!result.success && result.failure.find("native async frame") != std::string::npos,
            "an ordinary declaration was admitted as an async export");
    for (const std::string selection : {"GeneratedActor::ordinary", "GeneratedDerivedActor::ordinary",
                                       "GeneratedActor::add:", "GeneratedActor::add::", "GeneratedError::class"}) {
      rejected.async_c_exports = {selection};
      const auto invalid_actor = Compile(rejected);
      Require(!invalid_actor.success && invalid_actor.failure.find("isolated instance selector") != std::string::npos,
              "an invalid actor selector was admitted as a C completion export");
    }
    rejected.async_c_exports = {"GeneratedActor::current", "GeneratedActor::current"};
    Require(!Compile(rejected).success, "duplicate actor export was admitted");
    rejected.async_c_exports.clear();
    rejected.throws_c_exports = {"generated_value"};
    const auto wrong_effect = Compile(rejected);
    Require(!wrong_effect.success && wrong_effect.failure.find("synchronous throwing entry") != std::string::npos,
            "an async entry was admitted as a synchronous C export");
    rejected.throws_c_exports = {"generated_variadic_pointer"};
    std::ifstream fixture(source);
    rejected.source_text = std::string(std::istreambuf_iterator<char>(fixture), {}) + R"(
typedef int (*Variadic)(int, ...);
static Variadic generated_variadic_pointer(Variadic callback) __objc3_throws {
  return callback;
}
)";
    const auto nonportable = Compile(rejected);
    Require(!nonportable.success && nonportable.failure.find("self-contained C value declaration") != std::string::npos,
            "a variadic callback was admitted through the generated C header");
    rejected.throws_c_exports = {"generated_unfixed_enum"};
    *rejected.source_text += R"(
enum GeneratedUnfixed { GeneratedUnfixedZero };
static enum GeneratedUnfixed generated_unfixed_enum(void) __objc3_throws {
  return GeneratedUnfixedZero;
}
)";
    const auto unfixed = Compile(rejected);
    Require(!unfixed.success && unfixed.failure.find("self-contained C value declaration") != std::string::npos,
            "an enum without an explicit representation was admitted through the generated C header");
    const auto header_before = llvm::MemoryBuffer::getFile(provider.c_export_header_output_path);
    Require(static_cast<bool>(header_before), "generated header missing");
    const std::string generated_header = (*header_before)->getBuffer().str();
    Require(generated_header.find("const int * _Nullable generated_sync_pointer_throws_c") != std::string::npos &&
            generated_header.find("const int * _Nonnull * _Nullable") != std::string::npos &&
            generated_header.find("void * _Nullable generated_sync_object_throws_c_result_t") != std::string::npos &&
            generated_header.find("void * _Nonnull generated_object_async_c_arg0_t") != std::string::npos &&
            generated_header.find("void * _Nullable generated_object_async_c_result_t") != std::string::npos,
            "generated C declarations lost nested nullability or promised a nonnull failure result");
    Write(provider.c_export_header_output_path, "previous header\n");
    auto failed = provider;
    failed.output_path = includes.string();
    Require(!Compile(failed).success, "invalid object destination was accepted");
    const auto header_after = llvm::MemoryBuffer::getFile(provider.c_export_header_output_path);
    Require(header_after && (*header_after)->getBuffer() == "previous header\n",
            "failed object publication changed the generated header");
    Write(provider.c_export_header_output_path, generated_header);
    CompileRequest body;
    body.input_path = (root / "body.m").string();
    body.source_text = R"(
#include <stdbool.h>
#include <objc3c/runtime/objc3_runtime_services.h>
int generated_value(bool enabled, const int *value) async {
  return enabled ? *value + 1 : 0;
}
)";
    body.objc3 = true;
    body.emit = EmitKind::Object;
    body.output_path = (root / "body.o").string();
    body.target_triple = provider.target_triple;
    body.arguments = provider.arguments;
    Compiled(Compile(body));
    separate_body = body.output_path;
  }

  Require(fs::remove(source), "generated provider source removal failed");
  const auto interface_path = root / "GeneratedCompletion.objc3i";
  Write(interface_path, interface.source_bytes);
  Write(interface_path.string() + ".json", interface.sidecar_json);
  CompileRequest imported;
  imported.input_path = (root / (cxx ? "imported.mm" : "imported.m")).string();
  imported.language = provider.language;
  imported.target_triple = provider.target_triple;
  imported.objc3 = true;
  imported.arc = arc;
  imported.emit = EmitKind::Syntax;
  imported.arguments = provider.arguments;
  InterfaceImportInput imported_api;
  imported_api.source_path = interface_path.string();
  imported_api.sidecar_path = interface_path.string() + ".json";
  imported_api.locked_identity = interface.identity;
  imported.imported_interfaces = {imported_api};
  imported.source_text = R"(
static void observe(void *context, objc_async_completion_status_t status,
                    int value, objc3_async_c_error error) {
  (void)context; (void)status; (void)value; (void)error;
}
void call_imported_actor(void *receiver) {
  objc3_task_release(GeneratedActor_add_by_async_c_task(receiver, 40, 2, 0, observe));
  GeneratedDerivedActor_checked_async_c(receiver, 0, 0, observe);
}
void call_imported_arguments(generated_arguments_async_c_arg0_t value,
                             generated_arguments_async_c_arg1_t action) {
  generated_arguments_async_c(value, action, 0, observe);
}
void call_imported_consumed(generated_consumed_async_c_arg1_t value,
                            generated_consumed_async_c_arg2_t action) {
  objc3_task_release(generated_consumed_async_c_task(0, value, action, 0, observe));
}
int call_imported_checked(objc3_async_c_error *failure) {
  return generated_sync_checked_throws_c(41, failure);
}
struct GeneratedAggregate call_imported_aggregate(struct GeneratedAggregate value,
                                                    objc3_async_c_error *failure) {
  return generated_sync_aggregate_throws_c(value, failure);
}
enum GeneratedMode call_imported_enum(enum GeneratedMode mode, objc3_async_c_error *failure) {
  return generated_sync_enum_throws_c(mode, failure);
}
generated_sync_block_throws_c_result_t call_imported_block(generated_sync_block_throws_c_arg0_t block,
                                                           objc3_async_c_error *failure) {
  return generated_sync_block_throws_c(block, failure);
}
generated_sync_callback_throws_c_result_t call_imported_callback(generated_sync_callback_throws_c_arg0_t callback,
                                                                 objc3_async_c_error *failure) {
  return generated_sync_callback_throws_c(callback, failure);
}
)";
  if (cxx) *imported.source_text += R"(
int call_imported_namespace(objc3_async_c_error *failure) {
  objc3_task_release(ExportRegion_Nested_pulse_async_c_task(41, 0, observe));
  return ExportAlias_check_throws_c(40, failure);
}
)";
  Compiled(Compile(imported));
  const auto client = root / "client.c";
  Write(client, R"(
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <time.h>
#include "generated.h"
extern objc3_async_c_error generated_failure(void);
extern void *generated_actor_create(void);
extern void *generated_derived_actor_create(void);
extern void *generated_argument_create(void);
extern void *generated_argument_block_create(void);
extern void generated_actor_release(void *);
extern unsigned generated_actors_destroyed(void);
extern unsigned generated_checked_errors_destroyed(void);
extern void *objc_autoreleasePoolPush(void);
extern void objc_autoreleasePoolPop(void *);
extern void *objc_retain(void *);
extern void objc_release(void *);
extern void *_Block_copy(const void *);
extern void _Block_release(const void *);
#ifdef __BLOCKS__
typedef int (^ClientBlock)(int);
static ClientBlock retained_block;
#else
typedef void *ClientBlock;
#endif
#ifdef EXPORTED_CPP_NAMESPACE
static atomic_int complete[20];
static void namespace_done(void *context, objc_async_completion_status_t status,
                           int value, objc3_async_c_error error) {
  assert(context == &complete[19] && status == OBJC_ASYNC_COMPLETION_SUCCESS && value == 42 && !error);
  assert(atomic_fetch_add(&complete[19], 1) == 0);
}
#else
static atomic_int complete[19];
#endif
static atomic_uint paused_workers;
static atomic_bool resume_workers;
static atomic_int cancelled_task;
static atomic_int consumed_calls[4];
static atomic_int enum_calls;
static enum GeneratedWide wide_value(enum GeneratedMode mode) {
  assert(mode == GeneratedModeAnswer);
  return GeneratedWideMax;
}
static struct GeneratedEnumBox transform_enum_box(struct GeneratedEnumBox value) {
  assert(value.minimum == GeneratedSignedWideMin && value.mode == GeneratedModeAnswer);
  value.values[0] += value.values[1];
  return value;
}
static void enum_done(void *context, objc_async_completion_status_t status,
                      enum GeneratedWide value, objc3_async_c_error error) {
  assert(context == &enum_calls && status == OBJC_ASYNC_COMPLETION_SUCCESS);
  assert(value == GeneratedWideMax && !error);
  assert(atomic_fetch_add(&enum_calls, 1) == 0);
}
static void release_submitted(objc3_task_t *task) {
  assert(task);
  objc3_task_release(task);
}
static void hold_worker(void *unused) {
  (void)unused;
  atomic_fetch_add(&paused_workers, 1);
  while (!atomic_load(&resume_workers)) nanosleep(&(struct timespec){0, 1000000}, 0);
}
static void value_done(void *context, objc_async_completion_status_t status,
                       int value, objc3_async_c_error error) {
  assert(context == &complete[0] && status == OBJC_ASYNC_COMPLETION_SUCCESS);
  assert(value == 42 && !error);
  assert(atomic_fetch_add(&complete[0], 1) == 0);
}
static void void_done(void *context, objc_async_completion_status_t status,
                      objc3_async_c_error error) {
  assert(!context && status == OBJC_ASYNC_COMPLETION_SUCCESS && !error);
  assert(atomic_fetch_add(&complete[1], 1) == 0);
}
static void checked_done(void *context, objc_async_completion_status_t status,
                          int value, objc3_async_c_error error) {
  if (context == &complete[2]) {
    assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && value == 42 && !error);
  } else {
    assert(context == &complete[3] && status == OBJC_ASYNC_COMPLETION_ERROR);
    assert(error && error == generated_failure());
  }
  assert(atomic_fetch_add((atomic_int *)context, 1) == 0);
}
static void object_done(void *context, objc_async_completion_status_t status,
                        objc3_async_c_error value, objc3_async_c_error error) {
  assert(context == &complete[4] && status == OBJC_ASYNC_COMPLETION_SUCCESS);
  assert(value == generated_failure() && !error);
  assert(atomic_fetch_add(&complete[4], 1) == 0);
}
static void block_done(void *context, objc_async_completion_status_t status,
                       ClientBlock value, objc3_async_c_error error) {
  assert(context == &complete[5] && status == OBJC_ASYNC_COMPLETION_SUCCESS);
  assert(value && !error);
#ifdef __BLOCKS__
  assert(value(2) == 42);
  retained_block = (ClientBlock)_Block_copy((const void *)value);
  assert(retained_block);
#endif
  assert(atomic_fetch_add(&complete[5], 1) == 0);
}
static void aggregate_done(void *context, objc_async_completion_status_t status,
                           struct GeneratedAggregate value, objc3_async_c_error error) {
  assert(context == &complete[6] && status == OBJC_ASYNC_COMPLETION_SUCCESS && !error);
  assert(value.port.flag == 'C' && value.port.lane == 42);
  assert(value.values[0] == 73 && value.values[1] == 91);
  assert(atomic_fetch_add(&complete[6], 1) == 0);
}
static void executor_done(void *context, objc_async_completion_status_t status,
                          int value, objc3_async_c_error error) {
  if (context == &complete[9]) {
    assert(status == OBJC_ASYNC_COMPLETION_ERROR && error);
  } else {
    assert(context == &complete[7] || context == &complete[8]);
    assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && value == 42 && !error);
  }
  assert(atomic_fetch_add((atomic_int *)context, 1) == 0);
}
static void missing_done(void *context, objc_async_completion_status_t status,
                         objc3_async_c_error error) {
  assert(context == &complete[10] && status == OBJC_ASYNC_COMPLETION_CANCELLED);
  assert(objc3_async_c_error_is_cancellation(error));
  assert(atomic_fetch_add(&complete[10], 1) == 0);
}
static void cancelled_task_done(void *context, objc_async_completion_status_t status,
                                objc3_async_c_error error) {
  assert(context == &cancelled_task && status == OBJC_ASYNC_COMPLETION_CANCELLED);
  assert(objc3_async_c_error_is_cancellation(error));
  assert(atomic_fetch_add(&cancelled_task, 1) == 0);
}
static void consumed_done(void *context, objc_async_completion_status_t status,
                          int value, objc3_async_c_error error) {
  if (context == &consumed_calls[1]) {
    assert(status == OBJC_ASYNC_COMPLETION_ERROR && error == generated_failure());
  } else if (context == &consumed_calls[2]) {
    assert(status == OBJC_ASYNC_COMPLETION_CANCELLED && objc3_async_c_error_is_cancellation(error));
  } else {
    assert(context == &consumed_calls[0] || context == &consumed_calls[3]);
    assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && value == 42 && !error);
  }
  assert(atomic_fetch_add((atomic_int *)context, 1) == 0);
}
static void actor_done(void *context, objc_async_completion_status_t status,
                       int value, objc3_async_c_error error) {
  if (context == &complete[14]) {
    assert(status == OBJC_ASYNC_COMPLETION_ERROR && error == generated_failure());
  } else if (context == &complete[15]) {
    assert(status == OBJC_ASYNC_COMPLETION_CANCELLED && objc3_async_c_error_is_cancellation(error));
  } else {
    assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && !error);
    assert(value == (context == &complete[12] ? 0 : 42));
  }
  assert(atomic_fetch_add((atomic_int *)context, 1) == 0);
}
static void arguments_done(void *context, objc_async_completion_status_t status,
                           int value, objc3_async_c_error error) {
  if (context == &complete[16]) {
    assert(status == OBJC_ASYNC_COMPLETION_SUCCESS && value == 42 && !error);
  } else {
    assert(context == &complete[17] && status == OBJC_ASYNC_COMPLETION_CANCELLED);
    assert(objc3_async_c_error_is_cancellation(error));
  }
  assert(atomic_fetch_add((atomic_int *)context, 1) == 0);
}
static int add_value(int value, void *context) {
  return value + *(const int *)context;
}
static void callback_done(void *context, objc_async_completion_status_t status,
                          generated_callback_async_c_result_t value, objc3_async_c_error error) {
  assert(context == &complete[18] && status == OBJC_ASYNC_COMPLETION_SUCCESS && !error);
  const int increment = 2;
  assert(value == add_value && value(40, (void *)&increment) == 42);
  assert(atomic_fetch_add(&complete[18], 1) == 0);
}
int main(void) {
  generated_named_async_c(41, &complete[9], executor_done);
  for (int elapsed = 0; elapsed < 2000 && !atomic_load(&complete[9]); ++elapsed)
    nanosleep(&(struct timespec){0, 1000000}, 0);
  assert(atomic_load(&complete[9]) == 1);
  objc3_executor_t *main_executor = 0, *named_executor = 0;
  assert(objc3_executor_get_main(&main_executor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_create(0, &named_executor) == OBJC3_SERVICE_OK);
  assert(objc3_executor_register_named("generated.completion", named_executor) == OBJC3_SERVICE_OK);
  void *actor = generated_derived_actor_create(), *other = generated_derived_actor_create();
  void *closed = generated_actor_create();
  assert(actor && other && closed && actor != other);
  objc3_actor_t *closed_mailbox = 0;
  assert(objc3_actor_create(named_executor, &closed_mailbox) == OBJC3_SERVICE_OK);
  assert(objc3_actor_bind_object(closed, closed_mailbox) == OBJC3_SERVICE_OK);
  assert(objc3_actor_close(closed_mailbox) == OBJC3_SERVICE_OK);

  const int input = 41;
  objc3_async_c_error synchronous_failure = generated_failure();
  assert(generated_sync_checked_throws_c(41, &synchronous_failure) == 42 && !synchronous_failure);
  assert(generated_sync_checked_throws_c(-1, &synchronous_failure) == 0 && synchronous_failure == generated_failure());
  assert(generated_sync_checked_throws_c(-1, 0) == 0);
  generated_sync_void_throws_c(false, &synchronous_failure);
  assert(!synchronous_failure);
  generated_sync_void_throws_c(true, &synchronous_failure);
  assert(synchronous_failure == generated_failure());
  assert(generated_sync_pointer_throws_c(&input, &synchronous_failure) == &input && !synchronous_failure);
  assert(!generated_sync_pointer_throws_c(0, &synchronous_failure) && synchronous_failure == generated_failure());
  const int * _Nonnull nested = &input;
  assert(generated_sync_nested_throws_c(&nested, &synchronous_failure) == 41 && !synchronous_failure);
  assert(generated_sync_nested_throws_c(0, &synchronous_failure) == 0 && synchronous_failure == generated_failure());
  assert(generated_sync_object_throws_c(generated_failure(), &synchronous_failure) == generated_failure());
  assert(!synchronous_failure);
  assert(!generated_sync_object_throws_c(0, &synchronous_failure) && synchronous_failure == generated_failure());
  assert(!generated_sync_block_throws_c(0, &synchronous_failure) && synchronous_failure == generated_failure());
  int increment = 2;
  assert(generated_sync_invoke_throws_c(add_value, &increment, &synchronous_failure) == 42 && !synchronous_failure);
  assert(generated_sync_callback_throws_c(add_value, &synchronous_failure) == add_value && !synchronous_failure);
  assert(!generated_sync_callback_throws_c(0, &synchronous_failure) && synchronous_failure == generated_failure());
#ifdef EXPORTED_CPP_NAMESPACE
  assert(ExportAlias_check_throws_c(40, &synchronous_failure) == 42 && !synchronous_failure);
  assert(ExportAlias_check_throws_c(-1, &synchronous_failure) == 0 && synchronous_failure == generated_failure());
  ExportRegion_Nested_pulse_async_c(41, &complete[19], namespace_done);
#endif
  struct GeneratedPort port = generated_sync_port_throws_c((struct GeneratedPort){'C', 41}, &synchronous_failure);
  assert(!synchronous_failure && port.flag == 'C' && port.lane == 42);
  (void)generated_sync_port_throws_c((struct GeneratedPort){0, 41}, &synchronous_failure);
  assert(synchronous_failure == generated_failure());
  struct GeneratedAggregate initial = {{'C', 41}, {73, 90}};
  struct GeneratedAggregate aggregate = generated_sync_aggregate_throws_c(initial, &synchronous_failure);
  assert(!synchronous_failure && aggregate.port.flag == 'C' && aggregate.port.lane == 42);
  assert(aggregate.values[0] == 73 && aggregate.values[1] == 91);
  (void)generated_sync_aggregate_throws_c((struct GeneratedAggregate){0}, &synchronous_failure);
  assert(synchronous_failure == generated_failure());
  static_assert(sizeof(enum GeneratedMode) == 1 && GeneratedModeInvalid < 0, "signed C enum");
  static_assert(sizeof(enum GeneratedWide) == 8 && GeneratedWideMax == 18446744073709551615ULL, "wide C enum");
  assert(generated_sync_enum_throws_c(GeneratedModeAnswer, &synchronous_failure) == GeneratedModeAnswer && !synchronous_failure);
  (void)generated_sync_enum_throws_c(GeneratedModeInvalid, &synchronous_failure);
  assert(synchronous_failure == generated_failure());
  struct GeneratedEnumBox box = {GeneratedSignedWideMin, GeneratedModeAnswer, {40, 2}};
  box = generated_sync_enum_box_throws_c(box, transform_enum_box, &synchronous_failure);
  assert(!synchronous_failure && box.mode == GeneratedModeAnswer && box.minimum == (-9223372036854775807LL - 1LL));
  assert(box.values[0] == 42 && box.values[1] == 2);
#ifdef __BLOCKS__
  box = generated_sync_enum_block_throws_c(box, ^struct GeneratedEnumBox(struct GeneratedEnumBox value) {
    value.values[1] += value.mode;
    return value;
  }, &synchronous_failure);
  assert(!synchronous_failure && box.values[0] == 42 && box.values[1] == 44);
#endif
  void *pool = objc_autoreleasePoolPush();
  assert(generated_sync_checked_throws_c(-2, &synchronous_failure) == 0);
  assert(synchronous_failure && synchronous_failure != generated_failure());
  assert(generated_checked_errors_destroyed() == 0);
  assert(objc_retain(synchronous_failure) == synchronous_failure);
  objc_autoreleasePoolPop(pool);
  assert(generated_checked_errors_destroyed() == 0);
  objc_release(synchronous_failure);
  assert(generated_checked_errors_destroyed() == 1);
  void *sync_input = generated_argument_create();
  void *sync_apply = generated_argument_block_create();
  assert(generated_consumed_sync_throws_c(false, sync_input, (ClientBlock)sync_apply,
      &synchronous_failure) == 42 && !synchronous_failure);
  assert(generated_consumed_sync_throws_c(true, sync_input, (ClientBlock)sync_apply,
      &synchronous_failure) == 0 && synchronous_failure == generated_failure());
  assert(generated_actors_destroyed() == 0);
  generated_actor_release(sync_input);
  _Block_release(sync_apply);
  assert(generated_actors_destroyed() == 2);
  generated_value_async_c(true, &input, &complete[0], value_done);
  generated_void_async_c(0, void_done);
  generated_checked_async_c(0, &complete[2], checked_done);
  release_submitted(generated_checked_async_c_task(1, &complete[3], checked_done));
  release_submitted(generated_object_async_c_task(generated_failure(), &complete[4], object_done));
  release_submitted(generated_block_async_c_task(40, &complete[5], block_done));
  generated_callback_async_c(add_value, &complete[18], callback_done);
  release_submitted(generated_aggregate_async_c_task(initial, &complete[6], aggregate_done));
  release_submitted(generated_enum_async_c_task(GeneratedModeAnswer, wide_value, &enum_calls, enum_done));
  generated_main_async_c(41, &complete[7], executor_done);
  generated_named_async_c(41, &complete[8], executor_done);
  generated_missing_async_c(&complete[10], missing_done);
  // Workers cannot enter a bridge until after the caller releases its
  // references. The canonical executor has four shared workers.
  objc3_executor_t *global = 0;
  assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
  for (unsigned worker = 0; worker != 4; ++worker)
    assert(objc3_executor_enqueue(global, hold_worker, 0) == OBJC3_SERVICE_OK);
  for (unsigned elapsed = 0; elapsed < 2000 && atomic_load(&paused_workers) != 4; ++elapsed)
    nanosleep(&(struct timespec){0, 1000000}, 0);
  assert(atomic_load(&paused_workers) == 4);
  objc3_executor_release(global);
  objc3_task_t *cancel = generated_cancelled_async_c_task(&cancelled_task, cancelled_task_done);
  assert(cancel && !objc3_task_is_cancel_requested(cancel));
  objc3_task_request_cancel(cancel);
  objc3_task_request_cancel(cancel);
  assert(objc3_task_is_cancel_requested(cancel) && !atomic_load(&cancelled_task));
  objc3_task_release(cancel);
  for (int index = 0; index < 3; ++index) {
    void *input = generated_argument_create();
    void *apply = generated_argument_block_create();
    objc3_task_t *task = generated_consumed_async_c_task(index == 1, input, (ClientBlock)apply,
                                                        &consumed_calls[index], consumed_done);
    assert(task);
    if (index == 2) objc3_task_request_cancel(task);
    objc3_task_release(task);
    generated_actor_release(input);
    _Block_release(apply);
  }
  void *consuming_actor = generated_actor_create();
  void *consumed_input = generated_argument_create();
  release_submitted(GeneratedActor_consume_async_c_task(consuming_actor, consumed_input,
                                                        &consumed_calls[3], consumed_done));
  generated_actor_release(consuming_actor);
  generated_actor_release(consumed_input);
  release_submitted(GeneratedActor_add_by_async_c_task(actor, 40, 2, &complete[11], actor_done));
  GeneratedDerivedActor_current_async_c(other, &complete[12], actor_done);
  GeneratedDerivedActor_checked_async_c(actor, false, &complete[13], actor_done);
  GeneratedDerivedActor_checked_async_c(actor, true, &complete[14], actor_done);
  GeneratedActor_current_async_c(closed, &complete[15], actor_done);
  generated_actor_release(actor);
  void *object = generated_argument_create();
  void *action = generated_argument_block_create();
  void *abandoned_object = generated_argument_create();
  void *abandoned_action = generated_argument_block_create();
  assert(object && action && abandoned_object && abandoned_action);
  pool = objc_autoreleasePoolPush();
  ClientBlock synchronous_block = generated_sync_block_throws_c((ClientBlock)action, &synchronous_failure);
  assert(!synchronous_failure && (const void *)synchronous_block == action);
#ifdef __BLOCKS__
  assert(synchronous_block(31) == 42);
#endif
  objc_autoreleasePoolPop(pool);
  generated_arguments_async_c(object, (ClientBlock)action, &complete[16], arguments_done);
  generated_abandoned_arguments_async_c(abandoned_object, (ClientBlock)abandoned_action,
                                       &complete[17], arguments_done);
  generated_actor_release(object);
  generated_actor_release(abandoned_object);
  _Block_release(action);
  _Block_release(abandoned_action);
  assert(generated_actors_destroyed() == 2);
  atomic_store(&resume_workers, true);
  for (int elapsed = 0; elapsed < 2000; ++elapsed) {
    unsigned executed = 0;
    assert(objc3_executor_pump(main_executor, 8, &executed) == OBJC3_SERVICE_OK);
    unsigned count = 0;
    for (unsigned index = 0; index < sizeof(complete) / sizeof(complete[0]); ++index)
      count += atomic_load(&complete[index]);
    unsigned consumed_count = 0;
    for (unsigned index = 0; index < 4; ++index) consumed_count += atomic_load(&consumed_calls[index]);
    if (count == sizeof(complete) / sizeof(complete[0]) && atomic_load(&cancelled_task) == 1 &&
        consumed_count == 4 && atomic_load(&enum_calls) == 1) {
#ifdef __BLOCKS__
      assert(retained_block(2) == 42);
      _Block_release((const void *)retained_block);
#endif
      assert(objc3_executor_unregister_named("generated.completion") == OBJC3_SERVICE_OK);
      assert(objc3_actor_join(closed_mailbox) == OBJC3_SERVICE_OK);
      assert(objc3_actor_unbind_object(closed) == OBJC3_SERVICE_OK);
      objc3_actor_release(closed_mailbox);
      generated_actor_release(closed);
      generated_actor_release(other);
      for (unsigned wait = 0; wait < 2000 && generated_actors_destroyed() != 17; ++wait)
        nanosleep(&(struct timespec){0, 1000000}, 0);
      assert(generated_actors_destroyed() == 17);
      objc3_executor_release(named_executor);
      objc3_executor_release(main_executor);
      return 0;
    }
    nanosleep(&(struct timespec){0, 1000000}, 0);
  }
  for (unsigned index = 0; index < sizeof(complete) / sizeof(complete[0]); ++index)
    if (!atomic_load(&complete[index])) fprintf(stderr, "Missing generated completion %u\n", index);
  if (!atomic_load(&cancelled_task)) fprintf(stderr, "Missing requested native cancellation\n");
  if (!atomic_load(&enum_calls)) fprintf(stderr, "Missing enum completion\n");
  for (unsigned index = 0; index < 4; ++index)
    if (!atomic_load(&consumed_calls[index])) fprintf(stderr, "Missing consumed completion %u\n", index);
  return 1;
}
)");
  CompileRequest consumer;
  consumer.input_path = client.string();
  consumer.output_path = (root / "client.o").string();
  consumer.language = BaseLanguage::C;
  consumer.emit = EmitKind::Object;
  consumer.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  consumer.arguments = {standard.ends_with("23") ? "-std=c23" : "-std=c17", "-I" + includes.string()};
  if (arc) consumer.arguments.push_back("-fblocks");
  if (cxx) consumer.arguments.push_back("-DEXPORTED_CPP_NAMESPACE");
  Compiled(Compile(consumer));
  if (declaration_only) {
    auto packed = consumer;
    packed.emit = EmitKind::Syntax;
    packed.output_path.clear();
    packed.arguments.push_back("-fpack-struct=1");
    const auto mismatch = Compile(packed);
    Require(!mismatch.success && std::any_of(mismatch.diagnostics.begin(), mismatch.diagnostics.end(),
                [](const auto &diagnostic) {
                  return diagnostic.message.find("C export aggregate layout") != std::string::npos;
                }), "a C consumer with an incompatible aggregate layout was admitted");
  }
  LinkAndRun(root, provider.output_path, consumer.output_path, separate_body);
  std::cout << "generated C completion " << standard << (arc ? " ARC\n" : " MRC\n");
}

void CheckCCompletion(const std::string &standard, bool arc) {
  llvm::SmallString<128> temporary;
  Require(!llvm::sys::fs::createUniqueDirectory("objc3-c-completion", temporary),
          "temporary directory failed");
  const fs::path root(temporary.c_str());
  struct Cleanup { fs::path path; ~Cleanup() { fs::remove_all(path); } } cleanup{root};
  const bool cxx = standard.starts_with("c++");
  const fs::path fixtures(OBJC3_PUBLIC_ASYNC_C_FIXTURES);
  const auto provider_source = root / (cxx ? "provider.mm" : "provider.m");
  fs::copy_file(fixtures / "provider.m", provider_source);
  fs::copy_file(fixtures / "api.h", root / "api.h");
  const auto client_source = root / "client.c";
  fs::copy_file(fixtures / "client.c", client_source);
  CompileRequest provider;
  provider.input_path = provider_source.string();
  provider.language = cxx ? BaseLanguage::ObjectiveCxx : BaseLanguage::ObjectiveC;
  provider.objc3 = true;
  provider.arc = arc;
  provider.objc_runtime = "gnustep-2.2";
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-O2", "-fblocks", "-fobjc-exceptions",
                        std::string("-I") + OBJC3_PUBLIC_ASYNC_HEADERS};
  if (cxx) {
    provider.arguments.push_back("-DOBJC3_COMPLETION_PRIVATE_TASK=1");
    provider.arguments.push_back("-fcoro-aligned-allocation");
  }
  provider.emit = EmitKind::Object;
  provider.output_path = (root / "provider.o").string();
  provider.emit_semantic_interface = true;
  provider.module_name = "test.CCompletion";
  provider.module_source_root = root.string();
  provider.package_identity = "c-completion-v1";
  provider.interface_exports = {"completion_value_async_c", "completion_void_async_c",
      "completion_object_async_c", "completion_block_async_c", "completion_error_async_c",
      "completion_cancel_async_c", "completion_worker_async_c", "CompletionRecord",
      "completion_published_checkpoint", "completion_published_group_join", "Error",
      "CompletionObject", "CompletionBlock", "completion_value_completion_t",
      "completion_void_completion_t", "completion_object_completion_t",
      "completion_block_completion_t", "completion_error_completion_t",
      "completion_cancel_completion_t", "completion_worker_completion_t"};
  const auto publication = Compile(provider);
  Compiled(publication);
  Require(publication.interface_artifact && publication.interface_artifact->success,
          "C completion interface missing");
  const auto &artifact = *publication.interface_artifact;
  const auto supported = SupportedSemanticInterfaceCapabilities();
  auto decoded = DecodeSemanticInterfaceSidecar(artifact.sidecar_json, artifact.source_bytes,
                                                  supported);
  if (!decoded) throw std::runtime_error(llvm::toString(decoded.takeError()));
  const auto completion_semantic = RuntimeCompletionSemantic().str();
  Require(std::find(decoded->required_semantics.begin(), decoded->required_semantics.end(),
                    completion_semantic) != decoded->required_semantics.end(),
          "C completion header was not bound to the interface");
  for (const auto &symbol : {"completion_value_async_c", "completion_void_async_c",
                            "completion_block_async_c", "completion_cancel_async_c"})
    Require(std::any_of(decoded->declarations.begin(), decoded->declarations.end(),
                       [&](const auto &entry) { return entry.qualified_name == symbol; }),
            std::string("missing C export declaration: ") + symbol);
  const auto interface_path = root / "CCompletion.objc3i";
  Write(interface_path, artifact.source_bytes);
  Write(interface_path.string() + ".json", artifact.sidecar_json);
  if (standard == "c17" && !arc) {
    // Rehash a valid sidecar after removing only the completion capability.
    // The source still imports those types and must no longer be admissible.
    std::erase(decoded->required_semantics, completion_semantic);
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(
        BuildSemanticFingerprintInput(*decoded));
    auto incomplete = EncodeSemanticInterfaceSidecar(*decoded, supported);
    if (!incomplete) throw std::runtime_error(llvm::toString(incomplete.takeError()));
    auto rejected = DecodeSemanticInterfaceSidecar(*incomplete, artifact.source_bytes, supported);
    Require(!rejected, "a C completion interface lost its required header capability");
    Require(llvm::toString(rejected.takeError()).find("pinned header capability") != std::string::npos,
            "header capability rejection did not identify the missing contract");
    const auto altered_headers = root / "altered-runtime";
    fs::create_directory(altered_headers);
    const auto altered_header = altered_headers / "objc3_async_c.h";
    fs::copy_file(fs::path(OBJC3_PUBLIC_ASYNC_HEADERS) / "objc3_async_c.h", altered_header);
    {
      std::ofstream changed(altered_header, std::ios::app);
      changed << "\n/* Different SDK header bytes. */\n";
      Require(changed.good(), "completion header mutation failed");
    }
    CompileRequest altered = provider;
    altered.arguments.insert(altered.arguments.begin(), "-I" + altered_headers.string());
    altered.input_path = (root / "altered-provider.m").string();
    altered.output_path = (root / "altered-provider.o").string();
    altered.interface_exports = {"published_altered_checkpoint"};
    altered.source_text = R"(
#include "objc3_async_c.h"
inline void published_altered_checkpoint(void) {
  try! objc3_task_check_cancellation();
}
)";
    const auto incompatible = Compile(altered);
    Require(!incompatible.success && incompatible.interface_artifact &&
                std::any_of(incompatible.interface_artifact->diagnostics.begin(),
                            incompatible.interface_artifact->diagnostics.end(),
                            [](const auto &diagnostic) {
                              return diagnostic.code == "O3IFC112" &&
                                     diagnostic.subject == "objc3_task_check_cancellation";
                            }),
            "published checkpoint did not reject the altered completion header");
  }
  Require(fs::remove(provider_source), "provider source removal failed");
  // Check physical callable declarations directly from the paired interface.
  // The ordinary C client below independently uses the public C header.
  CompileRequest imported;
  imported.input_path = (root / (cxx ? "interface-client.mm" : "interface-client.m")).string();
  imported.language = provider.language;
  imported.objc3 = true;
  imported.arc = arc;
  imported.objc_runtime = provider.objc_runtime;
  imported.target_triple = provider.target_triple;
  imported.arguments = provider.arguments;
  imported.emit = EmitKind::Object;
  imported.output_path = (root / "interface-client.o").string();
  imported.source_text = R"(
static void check_value(void *context, objc_async_completion_status_t status,
                        CompletionRecord value, objc3_async_c_error error) {
  (void)context; (void)status; (void)value; (void)error;
}
static void check_void(void *context, objc_async_completion_status_t status,
                       objc3_async_c_error error) {
  (void)context; (void)status; (void)error;
}
void interface_client(void) {
  completion_value_async_c(32, 41, 0, check_value);
  completion_void_async_c(0, check_void);
}
#ifdef __cplusplus
extern "C"
#endif
void interface_checkpoint(void) {
  if (completion_published_checkpoint(1)) __builtin_trap();
}
#ifdef __cplusplus
extern "C"
#endif
objc3_async_frame completion_group_join_frame(objc3_task_group_t *group) {
  return completion_published_group_join(group);
}
)";
  InterfaceImportInput input;
  input.source_path = interface_path.string();
  input.sidecar_path = interface_path.string() + ".json";
  input.locked_identity = artifact.identity;
  imported.imported_interfaces = {input};
  Compiled(Compile(imported));
  CompileRequest client;
  client.input_path = client_source.string();
  client.language = BaseLanguage::C;
  client.target_triple = provider.target_triple;
  client.arguments = {standard == "c23" || standard == "c++23" ? "-std=c23" : "-std=c17",
                      "-O2", "-fblocks", "-UNDEBUG",
                      std::string("-I") + OBJC3_PUBLIC_ASYNC_HEADERS};
  if (cxx) client.arguments.push_back("-DOBJC3_COMPLETION_PRIVATE_TASK=1");
  client.emit = EmitKind::Object;
  client.output_path = (root / "client.o").string();
  Compiled(Compile(client));
  LinkAndRun(root, provider.output_path, client.output_path, imported.output_path);
  std::cout << "C completion source-free execution " << standard << (arc ? " ARC\n" : " MRC\n");
}

void CheckThrowingDestructor(const std::string &standard, bool arc) {
  llvm::SmallString<128> temporary;
  Require(!llvm::sys::fs::createUniqueDirectory("objc3-checked-sret", temporary),
          "temporary directory failed");
  const fs::path root(temporary.c_str());
  struct Cleanup { fs::path path; ~Cleanup() { fs::remove_all(path); } } cleanup{root};
  const auto provider_path = root / "checked-provider.mm";
  Write(provider_path, R"(
@protocol Error
@end
__attribute__((objc_root_class))
@interface CheckedOwnerError <Error>
+ (id)self;
+ (id)retain;
+ (oneway void)release;
+ (id)autorelease;
@end
@implementation CheckedOwnerError
+ (id)self { return self; }
+ (id)retain { return self; }
+ (oneway void)release {}
+ (id)autorelease { return self; }
@end

struct DestructorException { int code; };
struct CheckedOwner {
  int value;
  bool explode;
  CheckedOwner(int value, bool explode);
  CheckedOwner(const CheckedOwner &) = delete;
  ~CheckedOwner() noexcept(false);
};
struct CheckedArgument {
  bool explode;
  ~CheckedArgument() noexcept(false);
};
static int alive;
static int destroyed;
CheckedOwner::CheckedOwner(int value, bool explode) : value(value), explode(explode) {
  ++alive;
}
CheckedOwner::~CheckedOwner() noexcept(false) {
  --alive;
  ++destroyed;
  if (explode) throw DestructorException{value};
}
CheckedArgument::~CheckedArgument() noexcept(false) {
  if (explode) throw DestructorException{99};
}
int checked_alive() { return alive; }
int checked_destroyed() { return destroyed; }
id<Error> checked_owner_error() { return (id<Error>)[CheckedOwnerError self]; }
CheckedOwner make_checked_owner(bool fail, bool explode, const CheckedArgument &)
    __objc3_throws(id<Error>) {
  if (fail) __objc3_throw(checked_owner_error());
  return CheckedOwner(42, explode);
}
CheckedOwner relay_checked_owner(bool fail, bool explode, bool argument_throws)
    __objc3_throws(id<Error>) {
  return try make_checked_owner(fail, explode, CheckedArgument{argument_throws});
}
)");
  CompileRequest provider;
  provider.input_path = provider_path.string();
  provider.language = BaseLanguage::ObjectiveCxx;
  provider.objc3 = true;
  provider.arc = arc;
  provider.objc_runtime = "gnustep-2.2";
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-O2", "-fblocks", "-fexceptions",
                        "-fobjc-exceptions", "-UNDEBUG"};
  provider.emit = EmitKind::Object;
  provider.output_path = (root / "checked-provider.o").string();
  provider.emit_semantic_interface = true;
  provider.module_name = "test.CheckedSRet";
  provider.package_identity = "checked-sret-v1";
  provider.interface_exports = {"Error", "DestructorException", "CheckedOwner",
      "CheckedArgument", "checked_alive", "checked_destroyed", "checked_owner_error",
      "make_checked_owner", "relay_checked_owner"};
  const auto published = Compile(provider);
  Compiled(published);
  Require(published.interface_artifact && published.interface_artifact->success,
          "checked result interface missing");
  const auto &artifact = *published.interface_artifact;
  const auto interface = root / "checked.objc3i";
  Write(interface, artifact.source_bytes);
  Write(interface.string() + ".json", artifact.sidecar_json);
  Require(fs::remove(provider_path), "checked result provider source removal failed");

  CompileRequest consumer = provider;
  consumer.input_path = (root / "checked-client.mm").string();
  consumer.output_path = (root / "checked-client.o").string();
  consumer.emit_semantic_interface = false;
  consumer.module_name.clear();
  consumer.package_identity.clear();
  consumer.interface_exports.clear();
  InterfaceImportInput input;
  input.source_path = interface.string();
  input.sidecar_path = interface.string() + ".json";
  input.locked_identity = artifact.identity;
  consumer.imported_interfaces = {input};
  consumer.source_text = R"(
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

using MaybeCheckedOwner = __objc3_optional(CheckedOwner);
static const MaybeCheckedOwner &stored_owner(bool argument_throws) {
  static const MaybeCheckedOwner &value =
      try? make_checked_owner(false, false, CheckedArgument{argument_throws});
  return value;
}
static const MaybeCheckedOwner &thread_owner(bool argument_throws) {
  static thread_local const MaybeCheckedOwner &value =
      try? make_checked_owner(false, false, CheckedArgument{argument_throws});
  return value;
}
static void *check_thread_owner(void *) {
  try {
    (void)thread_owner(true);
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 99);
  }
  assert(!checked_alive() && checked_destroyed() == 12);
  assert(__objc3_has_value(thread_owner(false)));
  assert(checked_alive() == 1 && checked_destroyed() == 12);
  return nullptr;
}
static void check_static_teardown() {
  assert(!checked_alive() && checked_destroyed() == 15);
}
int main() {
  __objc3_do {
    CheckedOwner value = try relay_checked_owner(true, true, false);
    (void)value;
    __builtin_trap();
  } __objc3_catch(id<Error> error) {
    assert(error == checked_owner_error());
    assert(!checked_alive() && !checked_destroyed());
  }
  try {
    CheckedOwner value = try! relay_checked_owner(false, true, false);
    assert(value.value == 42 && checked_alive() == 1 && !checked_destroyed());
  } catch (const DestructorException &error) {
    assert(error.code == 42);
  }
  assert(!checked_alive() && checked_destroyed() == 1);
  try {
    try! make_checked_owner(false, true, CheckedArgument{false});
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 42);
  }
  assert(!checked_alive() && checked_destroyed() == 2);
  try {
    CheckedOwner value = try! make_checked_owner(false, false, CheckedArgument{true});
    (void)value;
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 99);
  }
  assert(!checked_alive() && checked_destroyed() == 3);
  try {
    CheckedOwner value = try! relay_checked_owner(false, false, true);
    (void)value;
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 99);
  }
  assert(!checked_alive() && checked_destroyed() == 4);
  {
    MaybeCheckedOwner missing = try? relay_checked_owner(true, true, false);
    assert(!__objc3_has_value(missing) && !checked_alive() && checked_destroyed() == 4);
  }
  try {
    MaybeCheckedOwner present = try? relay_checked_owner(false, true, false);
    assert(__objc3_has_value(present) && checked_alive() == 1 && checked_destroyed() == 4);
  } catch (const DestructorException &error) {
    assert(error.code == 42);
  }
  assert(!checked_alive() && checked_destroyed() == 5);
  try {
    try? make_checked_owner(false, true, CheckedArgument{false});
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 42);
  }
  assert(!checked_alive() && checked_destroyed() == 6);
  int alive_during_comma = 0;
  try {
    (try? relay_checked_owner(false, true, false), alive_during_comma = checked_alive());
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 42 && alive_during_comma == 1);
  }
  assert(!checked_alive() && checked_destroyed() == 7);
  try {
    MaybeCheckedOwner value = try? make_checked_owner(false, false, CheckedArgument{true});
    (void)value;
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 99);
  }
  assert(!checked_alive() && checked_destroyed() == 8);

  {
    MaybeCheckedOwner selected = checked_alive()
        ? try? relay_checked_owner(true, true, false)
        : try? relay_checked_owner(false, false, false);
    assert(__objc3_has_value(selected) && checked_alive() == 1);
  }
  assert(!checked_alive() && checked_destroyed() == 9);
  try {
    MaybeCheckedOwner values[] = {
        try? relay_checked_owner(false, false, false),
        try? make_checked_owner(false, false, CheckedArgument{true})
    };
    (void)values;
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 99);
  }
  assert(!checked_alive() && checked_destroyed() == 11);

  pthread_t worker;
  assert(pthread_create(&worker, nullptr, check_thread_owner, nullptr) == 0);
  assert(pthread_join(worker, nullptr) == 0);
  assert(!checked_alive() && checked_destroyed() == 13);
  try {
    (void)stored_owner(true);
    __builtin_trap();
  } catch (const DestructorException &error) {
    assert(error.code == 99);
  }
  assert(!checked_alive() && checked_destroyed() == 14);
  assert(atexit(check_static_teardown) == 0);
  assert(__objc3_has_value(stored_owner(false)));
  assert(checked_alive() == 1 && checked_destroyed() == 14);
}
)";
  Compiled(Compile(consumer));
  LinkAndRun(root, provider.output_path, consumer.output_path);
  std::cout << "Checked throwing destructor source-free execution "
            << standard << (arc ? " ARC\n" : " MRC\n");
}

void CheckOptionalTransport(const std::string &standard, bool arc) {
  llvm::SmallString<128> temporary;
  Require(!llvm::sys::fs::createUniqueDirectory("objc3-optional-transport", temporary),
          "temporary directory failed");
  const fs::path root(temporary.c_str());
  struct Cleanup { fs::path path; ~Cleanup() { fs::remove_all(path); } } cleanup{root};
  const auto provider_path = root / "transport-provider.mm";
  Write(provider_path, R"(
@protocol Error @end
__attribute__((objc_root_class))
@interface TransportCheckedError <Error>
+ (Class)class;
@end
@implementation TransportCheckedError
+ (Class)class { return self; }
@end

struct TransportError { int code; };
struct TransportOwner {
  int value;
  int role;
  TransportOwner(int value, int role = 0);
  TransportOwner(const TransportOwner &);
  TransportOwner(TransportOwner &&);
  ~TransportOwner() noexcept(false);
};
using MaybeTransportOwner = __objc3_optional(TransportOwner);
struct [[clang::trivial_abi]] RegisterOwner {
  int value;
  explicit RegisterOwner(int value);
  RegisterOwner(const RegisterOwner &other);
  ~RegisterOwner() noexcept(false);
};
struct [[clang::trivial_abi]] RegisterPair {
  double first, second;
  RegisterPair(double first, double second);
  RegisterPair(const RegisterPair &other);
  ~RegisterPair() noexcept(false);
};
using MaybeRegister = __objc3_optional(RegisterOwner);

int transport_live();
int transport_destroyed();
int transport_copied();
int transport_moved();
int transport_consumed();
MaybeTransportOwner transport_make(int mode);
MaybeTransportOwner transport_relay(int mode);
id<Error> transport_checked_error();
MaybeTransportOwner transport_checked(int mode) __objc3_throws(id<Error>);
MaybeTransportOwner transport_checked_relay(int mode) __objc3_throws(id<Error>);
MaybeTransportOwner transport_checked_argument(MaybeTransportOwner value, int mode)
    __objc3_throws(id<Error>);
void transport_consume(MaybeTransportOwner value, int value_mode);
void transport_pair(MaybeTransportOwner first, MaybeTransportOwner second);
RegisterOwner make_register(int mode) __objc3_throws(id<Error>);
RegisterOwner relay_register(int mode) __objc3_throws(id<Error>);
RegisterPair make_register_pair(bool fail) __objc3_throws(id<Error>);
MaybeRegister wrap_register(int mode);
bool consume_register(MaybeRegister value);
__attribute__((objc_root_class))
@interface RegisterFactory
+ (RegisterOwner)value:(int)mode __objc3_throws(id<Error>);
@end
__attribute__((objc_root_class))
@interface TransportRecipient { Class isa; }
+ (instancetype)new;
- (MaybeTransportOwner)echo:(MaybeTransportOwner)value;
+ (MaybeTransportOwner)checked:(MaybeTransportOwner)value mode:(int)mode
    __objc3_throws(id<Error>);
@end
extern "C" id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern "C" id object_dispose(id);

static int live, destroyed, copied, moved, consumed;
TransportOwner::TransportOwner(int value, int role) : value(value), role(role) {
  if (role == 4 || (role == 5 && live)) throw TransportError{34};
  ++live;
}
TransportOwner::TransportOwner(const TransportOwner &other)
    : value(other.value), role(other.role) {
  if (role == 3) throw TransportError{33};
  ++live;
  ++copied;
}
TransportOwner::TransportOwner(TransportOwner &&other)
    : value(other.value), role(other.role) {
  other.role = 0;
  ++live;
  ++moved;
}
TransportOwner::~TransportOwner() noexcept(false) {
  --live;
  ++destroyed;
  if (role == 1) throw TransportError{31};
}
struct ReturnCleanup {
  int mode;
  int owner_value(int value) const { return value; }
  ~ReturnCleanup() noexcept(false) {
    if (mode == 2) throw TransportError{32};
  }
};
int transport_live() { return live; }
int transport_destroyed() { return destroyed; }
int transport_copied() { return copied; }
int transport_moved() { return moved; }
int transport_consumed() { return consumed; }

MaybeTransportOwner transport_make(int mode) {
  if (mode < 0) return __objc3_none(TransportOwner);
  return __objc3_some(TransportOwner,
      TransportOwner(ReturnCleanup{mode}.owner_value(42), mode));
}
MaybeTransportOwner transport_relay(int mode) {
  return transport_make(mode);
}
id<Error> transport_checked_error() { return (id<Error>)[TransportCheckedError class]; }
MaybeTransportOwner transport_checked(int mode) __objc3_throws(id<Error>) {
  if (mode == -2) __objc3_throw(transport_checked_error());
  return transport_make(ReturnCleanup{mode}.owner_value(mode == 2 ? 0 : mode));
}
MaybeTransportOwner transport_checked_relay(int mode) __objc3_throws(id<Error>) {
  return try transport_checked(mode);
}
MaybeTransportOwner transport_checked_argument(MaybeTransportOwner value, int mode)
    __objc3_throws(id<Error>) {
  ++consumed;
  if (mode == -2) __objc3_throw(transport_checked_error());
  if (mode == 1) return transport_make(0);
  return static_cast<MaybeTransportOwner &&>(value);
}
void transport_consume(MaybeTransportOwner value, int mode) {
  ++consumed;
  if (__objc3_has_value(value) != (mode >= 0)) __builtin_trap();
  if (mode == 2) throw TransportError{38};
}
void transport_pair(MaybeTransportOwner first, MaybeTransportOwner second) {
  if (__objc3_has_value(first) || __objc3_has_value(second)) __builtin_trap();
}

RegisterOwner::RegisterOwner(int value) : value(value) { ++live; }
RegisterOwner::RegisterOwner(const RegisterOwner &other) : value(other.value) {
  ++live;
  ++copied;
}
RegisterOwner::~RegisterOwner() noexcept(false) {
  --live;
  ++destroyed;
  if (value == 31) throw TransportError{31};
}
RegisterPair::RegisterPair(double first, double second) : first(first), second(second) { ++live; }
RegisterPair::RegisterPair(const RegisterPair &other) : first(other.first), second(other.second) {
  ++live;
  ++copied;
}
RegisterPair::~RegisterPair() noexcept(false) {
  --live;
  ++destroyed;
}
struct LateReturn {
  int mode;
  int value() const { return mode == 1 ? 31 : 42; }
  ~LateReturn() noexcept(false) {
    if (mode == 2) throw TransportError{32};
  }
};
RegisterOwner make_register(int mode) __objc3_throws(id<Error>) {
  if (mode < 0) __objc3_throw(transport_checked_error());
  return RegisterOwner(LateReturn{mode}.value());
}
RegisterOwner relay_register(int mode) __objc3_throws(id<Error>) {
  return try make_register(mode);
}
RegisterPair make_register_pair(bool fail) __objc3_throws(id<Error>) {
  if (fail) __objc3_throw(transport_checked_error());
  return RegisterPair(17.0, 25.0);
}
MaybeRegister wrap_register(int mode) {
  if (mode < 0) return __objc3_none(RegisterOwner);
  return __objc3_some(RegisterOwner, try! make_register(mode));
}
bool consume_register(MaybeRegister value) {
  return __objc3_has_value(value);
}
@implementation RegisterFactory
+ (RegisterOwner)value:(int)mode __objc3_throws(id<Error>) {
  return try relay_register(mode);
}
@end
@implementation TransportRecipient
+ (instancetype)new { return class_createInstance(self, 0); }
- (void)dealloc { object_dispose(self); }
- (MaybeTransportOwner)echo:(MaybeTransportOwner)value {
  ++consumed;
  return static_cast<MaybeTransportOwner &&>(value);
}
+ (MaybeTransportOwner)checked:(MaybeTransportOwner)value mode:(int)mode
    __objc3_throws(id<Error>) {
  ++consumed;
  if (mode == -2) __objc3_throw(transport_checked_error());
  if (mode == 1) return transport_make(0);
  return static_cast<MaybeTransportOwner &&>(value);
}
@end
)");
  CompileRequest provider;
  provider.input_path = provider_path.string();
  provider.language = BaseLanguage::ObjectiveCxx;
  provider.objc3 = true;
  provider.arc = arc;
  provider.objc_runtime = "gnustep-2.2";
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-O2", "-fblocks", "-fexceptions",
                        "-fobjc-exceptions", "-UNDEBUG"};
  provider.emit = EmitKind::Object;
  provider.output_path = (root / "transport-provider.o").string();
  provider.emit_semantic_interface = true;
  provider.module_name = "test.OptionalTransport";
  provider.package_identity = "optional-transport-v1";
  provider.interface_exports = {"Error", "TransportError", "TransportOwner",
      "MaybeTransportOwner", "transport_live", "transport_destroyed",
      "transport_copied", "transport_moved", "transport_consumed",
      "transport_make", "transport_relay", "transport_consume", "transport_pair",
      "transport_checked_error", "transport_checked", "transport_checked_relay",
      "transport_checked_argument",
      "RegisterOwner", "RegisterPair", "RegisterFactory", "TransportRecipient", "make_register",
      "relay_register", "make_register_pair", "MaybeRegister", "wrap_register",
      "consume_register"};
  const auto published = Compile(provider);
  Compiled(published);
  Require(published.interface_artifact && published.interface_artifact->success,
          "Optional transport interface missing");
  const auto &artifact = *published.interface_artifact;
  const auto interface = root / "transport.objc3i";
  Write(interface, artifact.source_bytes);
  Write(interface.string() + ".json", artifact.sidecar_json);
  Require(fs::remove(provider_path), "Optional transport provider source removal failed");

  CompileRequest consumer = provider;
  consumer.input_path = (root / "transport-client.mm").string();
  consumer.output_path = (root / "transport-client.o").string();
  consumer.emit_semantic_interface = false;
  consumer.module_name.clear();
  consumer.package_identity.clear();
  consumer.interface_exports.clear();
  InterfaceImportInput input;
  input.source_path = interface.string();
  input.sidecar_path = interface.string() + ".json";
  input.locked_identity = artifact.identity;
  consumer.imported_interfaces = {input};
  consumer.source_text = R"(
static void require(bool condition) { if (!condition) __builtin_trap(); }
static void count(int live, int destroyed, int copied, int moved, int consumed) {
  require(transport_live() == live && transport_destroyed() == destroyed &&
          transport_copied() == copied && transport_moved() == moved &&
          transport_consumed() == consumed);
}
int main() {
  {
    auto none = transport_relay(-1);
    transport_consume(none, -1);
    count(0, 0, 0, 0, 1);
  }
  {
    auto value = transport_relay(0);
    count(1, 0, 0, 0, 1);
    transport_consume(value, 0);
    count(1, 1, 1, 0, 2);
    transport_consume(static_cast<MaybeTransportOwner &&>(value), 0);
    count(1, 2, 1, 1, 3);
  }
  count(0, 3, 1, 1, 3);
  try {
    (void)transport_relay(2);
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 32); }
  count(0, 4, 1, 1, 3);
  try {
    transport_consume(transport_relay(1), 0);
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 31); }
  count(0, 5, 1, 1, 4);
  try {
    transport_consume(transport_relay(0), 2);
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 38); }
  count(0, 6, 1, 1, 5);
  try {
    // Whichever argument is evaluated first acquires the pending owner.
    transport_pair(transport_relay(5), transport_relay(5));
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 34); }
  count(0, 7, 1, 1, 5);
  {
    auto source = transport_make(3);
    try {
      transport_consume(source, 0);
      __builtin_trap();
    } catch (const TransportError &error) { require(error.code == 33); }
    count(1, 7, 1, 1, 5);
  }
  count(0, 8, 1, 1, 5);

  __objc3_do {
    auto ignored = try transport_checked_relay(-2);
    (void)ignored;
    __builtin_trap();
  } __objc3_catch(id<Error> error) {
    require(error == transport_checked_error());
    count(0, 8, 1, 1, 5);
  }
  {
    auto missing = try? transport_checked_relay(-2);
    auto present_none = try? transport_checked_relay(-1);
    require(!__objc3_has_value(missing) && __objc3_has_value(present_none));
    count(0, 8, 1, 1, 5);
  }
  {
    auto nested = try? transport_checked_relay(0);
    require(__objc3_has_value(nested));
    count(1, 8, 1, 1, 5);
  }
  count(0, 9, 1, 1, 5);
  {
    auto present = try! transport_checked_relay(0);
    require(__objc3_has_value(present));
    count(1, 9, 1, 1, 5);
  }
  count(0, 10, 1, 1, 5);
  try {
    try? transport_checked_relay(1);
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 31); }
  count(0, 11, 1, 1, 5);
  try {
    auto nested = try? transport_checked_relay(2);
    (void)nested;
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 32); }
  count(0, 12, 1, 1, 5);

  __objc3_do {
    auto missing = try relay_register(-1);
    (void)missing;
    __builtin_trap();
  } __objc3_catch(id<Error> error) {
    require(error == transport_checked_error());
    count(0, 12, 1, 1, 5);
  }
  {
    auto missing = try? relay_register(-1);
    auto absent_pair = try? make_register_pair(true);
    require(!__objc3_has_value(missing) && !__objc3_has_value(absent_pair));
    count(0, 12, 1, 1, 5);
  }
  {
    auto ready = try! relay_register(0);
    auto pair = try! make_register_pair(false);
    require(ready.value == 42 && pair.first == 17.0 && pair.second == 25.0);
    count(2, 12, 1, 1, 5);
  }
  count(0, 14, 1, 1, 5);
  {
    auto ready = try? relay_register(0);
    require(__objc3_has_value(ready));
    count(1, 14, 1, 1, 5);
  }
  count(0, 15, 1, 1, 5);
  try {
    try? relay_register(1);
    __builtin_trap();
  } catch (const TransportError &error) {
    require(error.code == 31);
  }
  count(0, 16, 1, 1, 5);
  try {
    auto value = try! relay_register(2);
    (void)value;
    __builtin_trap();
  } catch (const TransportError &error) {
    require(error.code == 32);
  }
  count(0, 17, 1, 1, 5);
  {
    auto via_message = try! [RegisterFactory value:0];
    auto message_failure = try? [RegisterFactory value:-1];
    require(via_message.value == 42 && !__objc3_has_value(message_failure));
    count(1, 17, 1, 1, 5);
  }
  count(0, 18, 1, 1, 5);
  {
    using CheckedRegister = RegisterOwner (^)(int) __objc3_throws(id<Error>);
    CheckedRegister block = ^RegisterOwner(int mode) __objc3_throws(id<Error>) {
      return try relay_register(mode);
    };
    auto via_block = try! block(0);
    auto block_failure = try? block(-1);
    auto *function = &relay_register;
    auto indirect = try! function(0);
    require(via_block.value == 42 && indirect.value == 42 && !__objc3_has_value(block_failure));
    count(2, 18, 1, 1, 5);
  }
  count(0, 20, 1, 1, 5);
  {
    auto missing = wrap_register(-1);
    require(!consume_register(missing));
    count(0, 20, 1, 1, 5);
    auto present = wrap_register(0);
    count(1, 20, 1, 1, 5);
    require(consume_register(present));
    count(1, 21, 2, 1, 5);
  }
  count(0, 22, 2, 1, 5);
  try {
    consume_register(wrap_register(1));
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 31); }
  count(0, 23, 2, 1, 5);
  try {
    (void)wrap_register(2);
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 32); }
  count(0, 24, 2, 1, 5);
  {
    auto none = transport_make(-1);
    auto returned = try! transport_checked_argument(none, 0);
    require(!__objc3_has_value(returned));
    count(0, 24, 2, 1, 6);
  }
  {
    auto source = transport_make(0);
    auto *call = &transport_checked_argument;
    auto returned = try? call(source, 0);
    require(__objc3_has_value(returned));
    count(2, 25, 3, 2, 7);
  }
  count(0, 27, 3, 2, 7);
  {
    auto source = transport_make(0);
    __objc3_do {
      try transport_checked_argument(static_cast<MaybeTransportOwner &&>(source), -2);
      __builtin_trap();
    } __objc3_catch(id<Error> error) {
      require(error == transport_checked_error());
      count(1, 28, 3, 3, 8);
    }
  }
  count(0, 29, 3, 3, 8);
  {
    auto source = transport_make(3);
    try {
      try? transport_checked_argument(source, 0);
      __builtin_trap();
    } catch (const TransportError &error) { require(error.code == 33); }
    count(1, 29, 3, 3, 8);
  }
  count(0, 30, 3, 3, 8);
  try {
    auto result = try? transport_checked_argument(transport_make(1), 1);
    (void)result;
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 31); }
  count(0, 32, 3, 3, 9);
  try {
    auto result = try! transport_checked_argument(transport_make(1), 1);
    (void)result;
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 31); }
  count(0, 34, 3, 3, 10);
  try {
    __objc3_do {
      try transport_checked_argument(transport_make(1), -2);
      __builtin_trap();
    } __objc3_catch(id<Error> error) {
      (void)error;
      __builtin_trap();
    }
  } catch (const TransportError &error) { require(error.code == 31); }
  count(0, 35, 3, 3, 11);

  {
    TransportRecipient *receiver = [TransportRecipient new];
    auto source = transport_make(0);
    auto echoed = [receiver echo:source];
    require(__objc3_has_value(echoed));
    count(2, 36, 4, 4, 12);
#if !__has_feature(objc_arc)
    extern void release_recipient(TransportRecipient *) asm("objc_release");
    release_recipient(receiver);
#endif
  }
  count(0, 38, 4, 4, 12);
  {
    auto source = transport_make(0);
    auto missing = try? [TransportRecipient checked:source mode:-2];
    require(!__objc3_has_value(missing));
    count(1, 39, 5, 4, 13);
  }
  count(0, 40, 5, 4, 13);
  try {
    auto result = try! [TransportRecipient checked:transport_make(1) mode:1];
    (void)result;
    __builtin_trap();
  } catch (const TransportError &error) { require(error.code == 31); }
  count(0, 42, 5, 4, 14);
  {
    // A typed nil result cannot fabricate a native C++ owner.
    TransportRecipient *nil_receiver = (TransportRecipient *)0;
    bool failed = false;
    try { (void)[nil_receiver echo:transport_make(0)]; }
    catch (...) { failed = true; }
    require(failed);
    count(0, 43, 5, 4, 14);
  }
  {
    using CheckedOptional = MaybeTransportOwner (^)(MaybeTransportOwner, int)
        __objc3_throws(id<Error>);
    CheckedOptional block =
        ^MaybeTransportOwner(MaybeTransportOwner value, int mode) __objc3_throws(id<Error>) {
          if (mode == -2) __objc3_throw(transport_checked_error());
          if (mode == 1) return transport_make(0);
          return static_cast<MaybeTransportOwner &&>(value);
        };
    auto empty = try! block(transport_make(-1), 0);
    auto checked_failure = try? block(transport_make(0), -2);
    require(!__objc3_has_value(empty) && !__objc3_has_value(checked_failure));
    count(0, 44, 5, 4, 14);
    try {
      auto result = try? block(transport_make(1), 1);
      (void)result;
      __builtin_trap();
    } catch (const TransportError &error) { require(error.code == 31); }
    count(0, 46, 5, 4, 14);
  }
}
)";
  Compiled(Compile(consumer));
  LinkAndRun(root, provider.output_path, consumer.output_path);
  std::cout << "Synchronous Optional transport source-free execution "
            << standard << (arc ? " ARC\n" : " MRC\n");
}


}
int main() {
  try {
    for (const std::string standard : {"c17", "c23", "c++20", "c++23"})
      for (const bool arc : {false, true}) {
        Check(standard, arc);
        CheckCCompletion(standard, arc);
        CheckGeneratedCCompletion(standard, arc);
        if (standard.starts_with("c++")) {
          CheckThrowingDestructor(standard, arc);
          CheckOptionalTransport(standard, arc);
        }
      }
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
