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
  for (const auto &item : result.diagnostics) std::cerr << item.message << '\n';
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
#include <stdbool.h>
typedef int Integer;
typedef const Integer *Input;
@protocol Error @end
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
static int generated_checked(int mode) async throws(id<Error>) {
  if (mode) __objc3_throw(generated_failure());
  return 42;
}
#define generated_value(...) invalid_if_reexpanded
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
  provider.async_c_exports = {"generated_value", "generated_void", "generated_checked"};
  provider.async_c_header_output_path = (root / "generated.h").string();
  provider.emit_semantic_interface = true;
  provider.module_name = "test.GeneratedCompletion";
  provider.package_identity = "generated-completion-v1";
  provider.interface_exports = {"generated_value_async_c", "generated_void_async_c",
      "generated_checked_async_c", "generated_value_completion_t",
      "generated_void_completion_t", "generated_checked_completion_t", "Error"};
  auto generated = Compile(provider);
  Compiled(generated);
  Require(generated.interface_artifact.has_value(), "generated C interface missing");
  const auto &interface = *generated.interface_artifact;
  Require(interface.source_bytes.find("generated_checked_async_c") != std::string::npos &&
          interface.sidecar_json.find(RuntimeCompletionSemantic()) != std::string::npos,
          "generated C callable ABI was omitted from semantic metadata");

  std::string separate_body;
  if (declaration_only) {
    auto rejected = provider;
    rejected.async_c_exports = {"generated_failure"};
    auto result = Compile(rejected);
    Require(!result.success && result.failure.find("native async frame") != std::string::npos,
            "an ordinary declaration was admitted as an async export");
    const auto header_before = llvm::MemoryBuffer::getFile(provider.async_c_header_output_path);
    Require(static_cast<bool>(header_before), "generated header missing");
    const std::string generated_header = (*header_before)->getBuffer().str();
    Write(provider.async_c_header_output_path, "previous header\n");
    auto failed = provider;
    failed.output_path = includes.string();
    Require(!Compile(failed).success, "invalid object destination was accepted");
    const auto header_after = llvm::MemoryBuffer::getFile(provider.async_c_header_output_path);
    Require(header_after && (*header_after)->getBuffer() == "previous header\n",
            "failed object publication changed the generated header");
    Write(provider.async_c_header_output_path, generated_header);
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
  const auto client = root / "client.c";
  Write(client, R"(
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdatomic.h>
#include <time.h>
#include "generated.h"
extern objc3_async_c_error generated_failure(void);
static atomic_int complete[4];
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
int main(void) {
  const int input = 41;
  generated_value_async_c(true, &input, &complete[0], value_done);
  generated_void_async_c(0, void_done);
  generated_checked_async_c(0, &complete[2], checked_done);
  generated_checked_async_c(1, &complete[3], checked_done);
  for (int elapsed = 0; elapsed < 2000; ++elapsed) {
    if (atomic_load(&complete[0]) + atomic_load(&complete[1]) +
        atomic_load(&complete[2]) + atomic_load(&complete[3]) == 4) return 0;
    nanosleep(&(struct timespec){0, 1000000}, 0);
  }
  return 1;
}
)");
  CompileRequest consumer;
  consumer.input_path = client.string();
  consumer.output_path = (root / "client.o").string();
  consumer.language = BaseLanguage::C;
  consumer.emit = EmitKind::Object;
  consumer.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  consumer.arguments = {"-std=c17", "-I" + includes.string()};
  Compiled(Compile(consumer));
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

}
int main() {
  try {
    for (const std::string standard : {"c17", "c23", "c++20", "c++23"})
      for (const bool arc : {false, true}) {
        Check(standard, arc);
        CheckCCompletion(standard, arc);
        CheckGeneratedCCompletion(standard, arc);
        if (standard.starts_with("c++"))
          CheckThrowingDestructor(standard, arc);
      }
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
