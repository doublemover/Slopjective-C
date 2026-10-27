#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/runtime_interface_types.h"
#include "clang/AST/ASTImporter.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/ODRHash.h"
#include "clang/AST/ObjC3Isolation.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Serialization/PCHContainerOperations.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;
namespace {
constexpr llvm::StringLiteral Root = R"(
__attribute__((objc_root_class)) @interface Root @end
@protocol Sendable @end
@interface SafeMessage : Root <Sendable> @end
@interface MutableMessage : Root @end
actor class Counter : Root { @public int storage; }
@property(nonatomic) int value;
@property(nonatomic, readonly) int token __attribute__((objc_nonisolated));
- (int)read __attribute__((nothrow));
- (int)stable __attribute__((objc_nonisolated));
@end
)";

CompileRequest Request(std::string Source, bool Cxx = false, bool Arc = true,
                       const std::string &Standard = "") {
  CompileRequest R;
  R.input_path = Cxx ? "isolation.mm" : "isolation.m";
  R.source_text = std::move(Source);
  R.language = Cxx ? BaseLanguage::ObjectiveCxx : BaseLanguage::ObjectiveC;
  R.objc3 = true;
  R.arc = Arc;
  R.strict_concurrency = true;
  R.target_triple = "x86_64-unknown-linux-gnu";
  R.objc_runtime = "gnustep-2.2";
  R.arguments = {"-std=" +
                     (Standard.empty() ? (Cxx ? "c++20" : "c17") : Standard),
                 "-fblocks", "-fobjc-weak", "-Wno-objc-property-implementation",
                 std::string("-I") + OBJC3_ISOLATION_RUNTIME_HEADERS};
  if (Cxx)
    R.arguments.push_back("-fcoro-aligned-allocation");
  return R;
}
bool Has(const CompileResult &R, llvm::StringRef Message) {
  for (const auto &D : R.diagnostics)
    if (llvm::StringRef(D.message).contains(Message))
      return true;
  return false;
}
void Success(const CompileResult &R) {
  if (!R.success) {
    std::cerr << R.failure << '\n';
    for (const auto &D : R.diagnostics)
      std::cerr << D.message << '\n';
  }
  assert(R.success);
}
void Reject(CompileRequest R, llvm::StringRef Message) {
  auto Result = Compile(R);
  assert(!Result.success);
  if (!Has(Result, Message)) {
    std::cerr << "Missing expected diagnostic: " << Message.str() << '\n';
    for (const auto &D : Result.diagnostics)
      std::cerr << D.message << '\n';
  }
  assert(Has(Result, Message));
}
std::vector<std::string> ASTArgs(bool Cxx, bool Arc) {
  return {"-x",
          Cxx ? "objective-c++" : "objective-c",
          Cxx ? "-std=c++20" : "-std=c17",
          "-fobjc-version=3",
          "-fobjc-runtime=gnustep-2.2",
          "-fblocks",
          "-fobjc3-strict-concurrency",
          Arc ? "-fobjc-arc" : "-fno-objc-arc",
          "--target=x86_64-unknown-linux-gnu"};
}
std::unique_ptr<clang::ASTUnit> Parse(llvm::StringRef Source, bool Cxx = false,
                                      bool Arc = true) {
  auto Arguments = ASTArgs(Cxx, Arc);
  clang::tooling::FileContentMappings Files;
  AddRuntimeInterfaceFiles(Files, Arguments);
  auto Unit = clang::tooling::buildASTFromCodeWithArgs(
      Source, Arguments, Cxx ? "isolation.mm" : "isolation.m", "clang-tool",
      std::make_shared<clang::PCHContainerOperations>(),
      clang::tooling::getClangStripDependencyFileAdjuster(), Files);
  assert(Unit && !Unit->getDiagnostics().hasErrorOccurred());
  return Unit;
}
template <class T> T *Named(clang::ASTUnit &Unit, llvm::StringRef Name) {
  for (auto *D : Unit.getASTContext().getTranslationUnitDecl()->decls())
    if (auto *N = llvm::dyn_cast<T>(D))
      if (N->getName() == Name)
        return N;
  assert(false && "required test declaration missing");
  return nullptr;
}

void CheckSource(bool Cxx, bool Arc, const std::string &Standard) {
  auto Make = [&](std::string S) {
    return Request(std::move(S), Cxx, Arc, Standard);
  };
  Success(Compile(Make(Root.str() + R"(
@implementation Counter
- (int)read { return storage + self.value + [self stable]; }
- (int)stable { return 7; }
- (int)token { return 9; }
@end
int nonisolated_read(Counter *value) { return [value stable] + value.token; }
__attribute__((objc_executor(main))) int main_value(void) { return 1; }
__attribute__((objc_executor(main))) int same_executor(void) { return main_value(); }
__attribute__((objc_executor(named("store.serial")))) int named_value(void) { return 2; }
__attribute__((objc_executor(named("store.serial")))) int same_named(void) { return named_value(); }
)")));
  Reject(Make(Root.str() + "int cross(Counter *c) { return [c read]; }"),
         "cross-actor entry requires an explicit await");
  Reject(Make("@class Counter;\n" + Root.str() +
              "int forward_actor(Counter *c) { return [c read]; }"),
         "cross-actor entry requires an explicit await");
  Reject(Make(Root.str() + "int cross(Counter *c) { return c.value; }"),
         "cross-actor entry requires an explicit await");
  Reject(Make(Root.str() + R"(
@interface MutableMessage (Ambiguous)
- (int)read __attribute__((nothrow));
@end
int ambiguous(id value) { return [value read]; }
)"),
         "ambiguous Objective-C 3 isolation or task-boundary metadata");
  Reject(Make(Root.str() + "int cross(Counter *c) { return c->storage; }"),
         "actor-isolated state");
  Reject(Make(Root.str() + R"(
@implementation Counter
- (int)read { return storage; }
- (int)stable { return storage; }
- (int)token { return 0; }
@end
)"),
         "actor-isolated state");
  Reject(Make(Root.str() + R"(
@interface Counter (Other)
- (int)other:(Counter *)other;
@end
@implementation Counter (Other)
- (int)other:(Counter *)other { return [other read]; }
@end
)"),
         "cross-actor entry requires an explicit await");
  Success(Compile(Make(Root.str() + R"(
@interface Counter (IndependentAffinity)
- (int)mainToken __attribute__((objc_nonisolated, objc_executor(main)));
@end
@implementation Counter (IndependentAffinity)
- (int)mainToken { return 1; }
@end
__attribute__((objc_executor(main))) int affine_nonisolated(Counter *c) {
  return [c mainToken];
}
)")));
  Reject(Make(Root.str() + R"(
@interface Counter (IndependentAffinity)
- (int)mainToken __attribute__((objc_nonisolated, objc_executor(main)));
@end
int nonaffine_nonisolated(Counter *c) { return [c mainToken]; }
)"),
         "cross-executor entry requires an explicit await");
  Reject(Make(Root.str() + R"(
__attribute__((objc_executor(main))) int main_only(void);
__attribute__((objc_executor(global))) int wrong_executor(void) { return main_only(); }
)"),
         "cross-executor entry requires an explicit await");
  Reject(Make(Root.str() + R"(
__attribute__((objc_executor(named("one")))) int named_only(void);
__attribute__((objc_executor(named("two")))) int wrong_named(void) { return named_only(); }
)"),
         "cross-executor entry requires an explicit await");
  Reject(Make(Root.str() + R"(
@interface Counter (Signatures)
- (MutableMessage *)bad:(MutableMessage *)value __attribute__((objc_nonisolated));
@end
)"),
         "non-Sendable type");
  Success(Compile(Make(Root.str() + R"(
@interface Counter (Signatures)
- (SafeMessage *)safe:(Counter *)value __attribute__((objc_nonisolated));
@end
)")));
  Reject(Make(Root.str() + R"(
@interface Counter (Future)
- (void)future __attribute__((objc_actor_nonreentrant));
@end
)"),
         "explicit actor nonreentrancy is deferred in v1");
  Reject(Make(Root.str() + R"(
@interface Counter (Future)
- (void)future nonreentrant;
@end
)"),
         "explicit actor nonreentrancy is deferred in v1");
  Success(Compile(
      Make("int nonreentrant(int nonreentrant) { return nonreentrant; }")));
  Success(Compile(Make(Root.str() + R"(
@interface Counter ()
@property(nonatomic, readwrite) int token;
@end
)")));
  Reject(Make(Root.str() + R"(
@interface MutableMessage (Wrong)
- (int)wrong __attribute__((objc_nonisolated));
@end
)"),
         "requires an actor instance method or property");
  Reject(Make(R"(
__attribute__((objc_executor(main))) int redeclared(void);
__attribute__((objc_executor(global))) int redeclared(void);
)"),
         "does not match the previous declaration");
  Reject(Make(R"(
__attribute__((objc_executor(named("")))) int empty_identity(void);
)"),
         "nonempty ordinary string");
  Reject(Make(R"(
__attribute__((objc_executor(main))) int main_entry(void);
int (*erased)(void) = main_entry;
)"),
         "erases isolation or task-boundary metadata");
  Success(Compile(Make(R"(
__attribute__((objc_executor(main))) int inherited(void);
int inherited(void) { return 1; }
)")));
  auto Unsafe = Compile(Make(Root.str() + R"(
__attribute__((objc_unsafe_sendable)) @interface UnsafeMessage : Root @end
@interface Counter (Unchecked)
- (UnsafeMessage *)unchecked __attribute__((objc_nonisolated));
@end
)"));
  Success(Unsafe);
  assert(Has(Unsafe, "relies on objc_unsafe_sendable"));
  Reject(Make(Root.str() + R"(
void close_handle(int);
typedef int Handle __attribute__((objc_requires_cleanup(close_handle)));
typedef struct ResourcePacket { Handle handle; } ResourcePacket;
@interface Counter (Resources)
- (ResourcePacket)resource __attribute__((objc_nonisolated));
@end
)"),
         "non-Sendable type");
  Reject(Make(Root.str() + R"(
typedef struct BorrowPacket { borrowed int *pointer; } BorrowPacket;
typedef struct __attribute__((objc_unsafe_sendable)) OuterPacket { BorrowPacket inner; } OuterPacket;
@interface Counter (Borrowed)
- (OuterPacket)borrowedPacket __attribute__((objc_nonisolated));
@end
)"),
         "non-Sendable type");
}

void CheckConcurrentCaptures(bool Arc, const std::string &Standard) {
  const std::string Preamble = Root.str() + R"(
#include "objc3_task.hpp"
typedef void (^Work)(void) __objc3_async;
void *spawn(Work) __attribute__((objc_task_spawn));
void *detach(Work) __attribute__((objc_task_detached));
void *group_spawn(Work) __attribute__((objc_task_group, objc_task_spawn));
void *group_spawn(Work) __attribute__((objc_task_group));
#define SPAWN(e) spawn(e)
struct Packet { int value; Counter *actor; };
)";
  auto Make = [&](std::string S) {
    return Request(Preamble + S, true, Arc, Standard);
  };
  Success(Compile(Make(R"(
void valid(struct Packet packet, SafeMessage *message) {
  SPAWN(^void() __objc3_async { (void)packet.value; (void)message; });
}
void nested_immutable(int value) {
  void (^const helper)(void) = ^{ (void)value; };
  spawn(^void() __objc3_async { helper(); });
}
void strong_snapshot(Counter *source) {
  __unsafe_unretained Counter *actor = source;
  spawn(^[actor] void() __objc3_async { (void)actor; });
}
void shared_source_snapshot(void) {
  __block int value = 1;
  spawn(^[value] void() __objc3_async { (void)value; });
}
void additional_sync_block(int value) {
  extern void *with_callback(Work, void (^)(void)) __attribute__((objc_task_spawn));
  with_callback(^void() __objc3_async {}, ^{ (void)value; });
}
)")));
  Reject(Make(R"(
void unsafe(MutableMessage *object) {
  SPAWN(^void() __objc3_async { (void)object; });
}
)"),
         "concurrent capture");
  Reject(Make(R"(
void shared(void) {
  __block int state = 0;
  detach(^void() __objc3_async { state++; });
}
)"),
         "shared __block capture");
  Reject(Make(R"(
void weak(MutableMessage *object) {
  __weak MutableMessage *weakObject = object;
  spawn(^void() __objc3_async { (void)weakObject; });
}
)"),
         "concurrent capture");
  Reject(Make(R"(
void unowned(Counter *actor) {
  spawn(^[unowned actor] void() __objc3_async { (void)actor; });
}
)"),
         "concurrent capture");
  Reject(Make(R"(
struct CustomDestruction { int value; ~CustomDestruction() {} };
void custom_destructor(CustomDestruction value) {
  spawn(^void() __objc3_async { (void)value; });
}
)"),
         "concurrent capture");
  Reject(Make(R"(
void unknown(Work block) { spawn(block); }
)"),
         "cannot prove Sendable captures");
  Reject(Make(R"(
void *bad_api(int) __attribute__((objc_task_spawn));
)"),
         "requires an async block work-item parameter");
  Reject(Make(R"(
void bad_result(Work) __attribute__((objc_task_spawn));
)"),
         "must return a task handle");
  auto Unused = Compile(Make(R"(
void unused_child(void) { spawn(^void() __objc3_async {}); }
)"));
  Success(Unused);
  assert(Has(Unused, "child task handle is unused"));
  auto Acknowledged = Compile(Make(R"(
void intentional_discard(void) {
  (void)spawn(^void() __objc3_async {});
  detach(^void() __objc3_async {});
}
)"));
  Success(Acknowledged);
  assert(!Has(Acknowledged, "child task handle is unused"));
}

void CheckAwaitBoundaries(bool Cxx, bool Arc, const std::string &Standard) {
  const std::string Header = Cxx ? "#include \"objc3_task.hpp\"\n"
                                 : "#include \"objc3_runtime_services.h\"\n";
  auto Make = [&](std::string S) {
    return Request(Header + Root.str() + S, Cxx, Arc, Standard);
  };
  auto Plain = Compile(Make(R"(
int synchronous(void) { return 7; }
int unnecessary(void) __objc3_async { return __objc3_await synchronous(); }
)"));
  Success(Plain);
  assert(Has(Plain, "native await operand does not suspend"));
  Success(Compile(Make(R"(
int actor_cross(Counter *c) __objc3_async { return __objc3_await [c read]; }
)")));
  const std::string NoThrow = Cxx ? " noexcept" : "";
  for (const std::string Executor :
       {"main", "global", "named(\"db.serial\")"}) {
    auto Cross =
        Compile(Make("__attribute__((objc_executor(" + Executor +
                     "))) int entry(int)" + NoThrow +
                     ";\nint executor_cross(int value) __objc3_async { return "
                     "__objc3_await entry(value); }"));
    Success(Cross);
    assert(!Has(Cross, "native await operand does not suspend"));
  }
  Success(Compile(Make("struct PairResult { int value; };\n"
              "__attribute__((objc_executor(main))) struct PairResult "
              "record_result(void)" +
              NoThrow +
              ";\nstruct PairResult bounded(void) __objc3_async { return "
              "__objc3_await record_result(); }")));
  Success(Compile(Make("struct Packet { int value; double weights[3]; };\n"
              "__attribute__((objc_executor(main))) struct Packet "
              "record_entry(struct Packet)" + NoThrow +
              ";\nstruct Packet cross(struct Packet value) __objc3_async { return "
              "__objc3_await record_entry(value); }")));
  if (Arc)
    Success(Compile(Make("struct OwnedPacket { SafeMessage *object; int value; };\n"
                "__attribute__((objc_executor(main))) struct OwnedPacket "
                "owned_entry(struct OwnedPacket)" + NoThrow +
                ";\nstruct OwnedPacket cross(struct OwnedPacket value) __objc3_async { return "
                "__objc3_await owned_entry(value); }")));
  else
    Reject(Make("struct OwnedPacket { SafeMessage *object; };\n"
                "__attribute__((objc_executor(main))) struct OwnedPacket "
                "owned_entry(struct OwnedPacket)" + NoThrow +
                ";\nstruct OwnedPacket cross(struct OwnedPacket value) __objc3_async { return "
                "__objc3_await owned_entry(value); }"),
           "awaited isolated entry requires");
  Reject(Make("struct UnsafePacket { MutableMessage *object; };\n"
              "__attribute__((objc_executor(main))) struct UnsafePacket "
              "unsafe_entry(struct UnsafePacket)" + NoThrow +
              ";\nstruct UnsafePacket cross(struct UnsafePacket value) __objc3_async { return "
              "__objc3_await unsafe_entry(value); }"),
         Arc ? "non-Sendable type" : "awaited isolated entry requires");
  if (Cxx)
    Success(Compile(Make(R"(
struct __attribute__((objc_unsafe_sendable)) MovePacket {
  int value;
  MovePacket(const MovePacket &)=delete;
  MovePacket(MovePacket &&)=default;
  ~MovePacket() noexcept {}
};
int move_entry(MovePacket) noexcept __attribute__((objc_executor(main)));
int move_cross(MovePacket value) __objc3_async {
  return __objc3_await move_entry(static_cast<MovePacket &&>(value));
}
)")));
  Reject(Make(R"(
void close_handle(int);
typedef int Handle __attribute__((objc_requires_cleanup(close_handle)));
struct __attribute__((objc_unsafe_sendable)) ResourcePacket { Handle value; };
)" + std::string("struct ResourcePacket resource_entry(struct ResourcePacket)") +
              NoThrow + " __attribute__((objc_executor(main)));\n"
              "struct ResourcePacket cross(struct ResourcePacket value) __objc3_async {"
              "return __objc3_await resource_entry(value);}"),
         "awaited isolated entry requires");
  Reject(Make("union PacketUnion { int value; double other; };\n"
              "union PacketUnion union_entry(union PacketUnion)" + NoThrow +
              " __attribute__((objc_executor(main)));\n"
              "union PacketUnion cross(union PacketUnion value) __objc3_async {"
              "return __objc3_await union_entry(value);}"),
         "awaited isolated entry requires");
  Success(Compile(Make(R"(
@interface Root (ClassEntries)
+ (int)classValue:(int)value __attribute__((nothrow, objc_executor(main)));
@end
int class_cross(int value) __objc3_async {
  return __objc3_await [Counter classValue:value];
}
)")));
  Reject(Make(R"(
@interface Root (ClassEntries)
+ (int)classValue:(int)value __attribute__((nothrow, objc_executor(main)));
@end
int class_cross(int value) { return [Counter classValue:value]; }
)"), "cross-executor entry requires an explicit await");
  Success(Compile(Make(R"(
@interface SuperBase : SafeMessage
- (int)baseValue:(int)value __attribute__((nothrow, objc_executor(main)));
+ (int)classValue:(int)value __attribute__((nothrow, objc_executor(main)));
@end
@interface SuperChild : SuperBase
- (int)frameEntry:(int)value __objc3_async_frame;
- (int)taskEntry:(int)value __objc3_async;
+ (int)classEntry:(int)value __objc3_async_frame;
@end
@implementation SuperChild
- (int)frameEntry:(int)value __objc3_async_frame {
  return __objc3_await [super baseValue:value];
}
- (int)taskEntry:(int)value __objc3_async {
  return __objc3_await [super baseValue:value];
}
+ (int)classEntry:(int)value __objc3_async_frame {
  return __objc3_await [super classValue:value];
}
@end
)")));
  const std::string Consuming =
      "int consume(__attribute__((ns_consumed)) SafeMessage *value)" + NoThrow +
      " __attribute__((objc_executor(main)));\n"
      "@interface Counter (Consumption)\n"
      "- (int)take:(__attribute__((ns_consumed)) SafeMessage *)value "
      "__attribute__((nothrow));\n"
      "- (int)finish __attribute__((nothrow, ns_consumes_self));\n"
      "- (int)finishAfter:(int)value __attribute__((nothrow, ns_consumes_self));\n@end\n"
      "int consume_cross(Counter *counter, SafeMessage *value) __objc3_async {"
      "int first=__objc3_await consume(value);"
      "int second=__objc3_await [counter take:value];"
      "return first+second+__objc3_await [counter finish];}";
  if (Arc) {
    Success(Compile(Make(Consuming)));
    Success(Compile(Make(Consuming +
        "int clear_counter(Counter *__strong *value) {*value=0;return 0;}\n"
        "int consume_snapshot(Counter *counter) __objc3_async {"
        "return __objc3_await [counter finishAfter:clear_counter(&counter)];}")));
  } else
    Reject(Make(Consuming), "awaited isolated entry requires");
  Reject(Make("typedef const struct __CFToken *CFTokenRef;\n"
              "int cf_consume(__attribute__((cf_consumed)) CFTokenRef value)" + NoThrow +
              " __attribute__((objc_executor(main)));\n"
              "int cf_cross(CFTokenRef value) __objc3_async {"
              "return __objc3_await cf_consume(value);}"),
         "awaited isolated entry requires");
  Success(
      Compile(Make("__attribute__((objc_executor(main), ns_returns_retained)) "
                   "SafeMessage *owned_result(void)" +
                   NoThrow + R"(;
SafeMessage *owned_cross(void) __objc3_async {
  return __objc3_await owned_result();
}
)")));
  Success(Compile(Make(R"(
__attribute__((objc_executor(global), nothrow))
int frame_entry(int value) __objc3_async_frame;
int frame_cross(int value) __objc3_async {
  return __objc3_await frame_entry(value);
}
)")));
  Reject(Make("__attribute__((objc_executor(main))) int nested(void)" +
              NoThrow +
              ";\n__attribute__((objc_executor(global))) int outer(int)" +
              NoThrow +
              ";\nint bad_nested(void) __objc3_async { return "
              "__objc3_await outer(nested()); }"),
         "executor crossing requires executor-hop lowering");
  Reject(
      Make("__attribute__((objc_executor(main))) int unsafe(MutableMessage *)" +
           NoThrow +
           ";\nint bad_transfer(MutableMessage *value) __objc3_async "
           "{ return __objc3_await unsafe(value); }"),
      "isolated argument");
  if (Cxx)
    Reject(Make(R"(
__attribute__((objc_executor(main))) int native_cpp_throw(void);
int bounded_cpp(void) __objc3_async { return __objc3_await native_cpp_throw(); }
)"),
           "awaited isolated entry requires");
  if (Cxx)
    Reject(Make(R"(
int throwing_sync(void) throws(int);
int unchecked_sync(void) __objc3_async { return __objc3_await throwing_sync(); }
)"),
           "throwing call requires an explicit");
}

void CheckExecutorEntryIR(bool Cxx, bool Arc, const std::string &Standard) {
  namespace fs = std::filesystem;
  auto Directory =
      fs::temp_directory_path() /
      ("objc3-isolation-ir-" +
       std::to_string(
           std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directory(Directory);
  const std::string Header = Cxx ? "#include \"objc3_task.hpp\"\n"
                                 : "#include \"objc3_runtime_services.h\"\n";
  const std::string NoThrow = Cxx ? " noexcept" : "";
  const std::string CLinkage = Cxx ? "extern \"C\" " : "";
  auto R = Request(
      Header + Root.str() + CLinkage + "int next_value(void)" + NoThrow +
          ";\n" + CLinkage + "int consume_main(int, SafeMessage *)" + NoThrow +
          " __attribute__((objc_executor(main)));\n" + CLinkage +
          "int consume_global(int)" + NoThrow +
          " __attribute__((objc_executor(global)));\n" + CLinkage +
          "int consume_named(int)" + NoThrow +
          " __attribute__((objc_executor(named(\"ir.destination\"))));\n" +
          CLinkage + "Counter *actor_source(void)" + NoThrow + ";\n" + R"(
@interface Counter (AffineEntry)
- (int)affinityRead __attribute__((objc_executor(named("ir.destination")), nothrow));
@end
int executor_ir(SafeMessage *message) __objc3_async {
  int value = __objc3_await consume_main(next_value(), message);
  value = __objc3_await consume_global(value);
  value += __objc3_await [actor_source() affinityRead];
  return __objc3_await consume_named(value);
}
)",
      Cxx, Arc, Standard);
  R.emit = EmitKind::LLVM;
  R.arguments.insert(R.arguments.end(), {"-Xclang", "-disable-llvm-passes"});
  R.output_path = (Directory / "entry.ll").string();
  Success(Compile(R));
  std::ifstream Stream(R.output_path);
  std::string IR{std::istreambuf_iterator<char>(Stream), {}};
  for (auto Symbol :
       {"@objc3_executor_get_main", "@objc3_executor_get_global",
        "@objc3_executor_get_named", "@objc3_executor_is_current",
        "@objc3_executor_release", "@objc3_task_hop",
        "@objc3_actor_get_for_object", "@objc3_actor_release",
        "@objc3_actor_executor_identity", "@objc3_executor_identity",
        "@objc3_task_await", "@llvm.coro.suspend"})
    assert(IR.find(Symbol) != std::string::npos);
  assert(IR.find("ir.destination") != std::string::npos);
  assert(IR.find("named:ir.destination") == std::string::npos);
  // One original source argument call plus its external declaration. Frame
  // construction and resumption must not re-evaluate the source expression.
  unsigned Occurrences = 0;
  for (size_t Position = 0;
       (Position = IR.find("@next_value(", Position)) != std::string::npos;
       ++Position)
    ++Occurrences;
  assert(Occurrences == 2);
  Occurrences = 0;
  for (size_t Position = 0;
       (Position = IR.find("@actor_source(", Position)) != std::string::npos;
       ++Position)
    ++Occurrences;
  assert(Occurrences == 2);
  fs::remove_all(Directory);
}

void CheckPrintAndPersistence(bool Cxx, bool Arc) {
  const std::string Source = Root.str() + R"(
__attribute__((objc_executor(named("db.\"quoted\"")))) int named_entry(void);
typedef void (^Work)(void) __objc3_async;
void *spawn(Work) __attribute__((objc_task_spawn));
struct ModulePacket { int value; double weights[3]; };
struct ModulePacket packet_entry(struct ModulePacket)
    __attribute__((objc_executor(main), nothrow));
int module_consume(__attribute__((ns_consumed)) SafeMessage *)
    __attribute__((objc_executor(main), nothrow));
@interface Counter (Packets)
- (struct ModulePacket)transformPacket:(struct ModulePacket)packet
    __attribute__((nothrow));
- (int)consumeSelf __attribute__((nothrow, ns_consumes_self));
+ (int)classConsume:(__attribute__((ns_consumed)) SafeMessage *)message
    __attribute__((nothrow, objc_executor(main)));
+ (struct ModulePacket)classPacket:(struct ModulePacket)packet
    __attribute__((nothrow, objc_executor(main)));
@end
)";
  auto From = Parse(Source, Cxx, Arc);
  auto *Counter = Named<clang::ObjCInterfaceDecl>(*From, "Counter");
  assert(clang::isObjC3Actor(Counter));
  auto *NamedEntry = Named<clang::FunctionDecl>(*From, "named_entry");
  auto Emitted = EmitSemanticInterface(From->getASTContext(),
                                       "Isolation.Contracts", "isolation-v1");
  assert(Emitted.success);
  const std::vector<llvm::StringRef> Semantics = {
      "objc3.canonical-function-effects.v1",
      "objc3.async-entry-abi.v1",
      RuntimeInterfaceSemantic(),
      "objc3.native-carrier-types.v1",
      "objc3.native-return-ownership.v1",
      "objc3.native-ownership-contracts.v1",
      "objc3.isolation-contracts.v1",
      "objc3.typed-keypaths.v1",
      "objc3.direct-dispatch.v1",
      "objc3.cxx-data-interfaces.v1",
      "objc3.cxx-member-interfaces.v1",
      "objc3.ownership-projection.v1",
      "objc3.api-availability.v1",
      "objc3.api-partitions.v1",
      "objc3.module-owned-names.v1"};
  auto Decoded = DecodeSemanticInterfaceSidecar(
      Emitted.sidecar_json, Emitted.source_bytes, Semantics);
  if (!Decoded) {
    llvm::errs() << "isolation sidecar decode failed: "
                 << llvm::toString(Decoded.takeError()) << '\n';
    assert(false && "isolation sidecar decode failed");
  }
  assert(Decoded);
  bool ActorIdentity = false, ExecutorIdentity = false;
  for (const auto &Declaration : Decoded->declarations) {
    ActorIdentity |= Declaration.qualified_name == "Counter" &&
                     !Declaration.effects.actor_isolation_identity.empty();
    ExecutorIdentity |=
        Declaration.qualified_name == "named_entry" &&
        Declaration.effects.executor_affinity_identity == "named:db.\"quoted\"";
  }
  assert(ActorIdentity && ExecutorIdentity);
  // Exercise the real source-free import pipeline, including an otherwise
  // digest-consistent source whose actor metadata has been removed.
  namespace fs = std::filesystem;
  auto Directory =
      fs::temp_directory_path() /
      ("objc3-isolation-import-" +
       std::to_string(
           std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directories(Directory);
  auto InterfacePath = Directory / "Isolation.objc3i";
  std::ofstream(InterfacePath) << Emitted.source_bytes;
  std::ofstream(InterfacePath.string() + ".json") << Emitted.sidecar_json;
  InterfaceImportInput Input;
  Input.source_path = InterfacePath.string();
  Input.locked_identity = Emitted.identity;
  auto Consumer =
      Request("int client(Counter *c) { return [c stable]; }", Cxx, Arc);
  Consumer.imported_interfaces = {Input};
  Success(Compile(Consumer));
  if (Arc) {
    Consumer.source_text =
        std::string(Cxx ? "#include \"objc3_task.hpp\"\n"
                        : "#include \"objc3_runtime_services.h\"\n") + R"(
int client_consuming(Counter *counter, SafeMessage *message) __objc3_async {
  int first=__objc3_await module_consume(message);
  int second=__objc3_await [Counter classConsume:message];
  return first+second+__objc3_await [counter consumeSelf];
}
)";
    Success(Compile(Consumer));
  }
  Consumer.source_text =
      std::string(Cxx ? "#include \"objc3_task.hpp\"\n"
                      : "#include \"objc3_runtime_services.h\"\n") + R"(
struct ModulePacket client_class_packet(struct ModulePacket packet)
    __objc3_async {
  return __objc3_await [Counter classPacket:packet];
}
struct ModulePacket client_packet(Counter *counter, struct ModulePacket packet)
    __objc3_async {
  struct ModulePacket value=__objc3_await packet_entry(packet);
  return __objc3_await [counter transformPacket:value];
}
)";
  Success(Compile(Consumer));
  Consumer.source_text = "int client(Counter *c) { return [c read]; }";
  auto CrossModule = Compile(Consumer);
  assert(!CrossModule.success &&
         Has(CrossModule, "cross-actor entry requires an explicit await"));
  std::string StrippedSource = Emitted.source_bytes;
  auto ActorSpelling = StrippedSource.find("actor class Counter");
  assert(ActorSpelling != std::string::npos);
  StrippedSource.replace(ActorSpelling, std::string("actor class").size(),
                         "@interface");
  const std::string Nonisolated = "__attribute__((objc_nonisolated))";
  for (auto Position = StrippedSource.find(Nonisolated);
       Position != std::string::npos;
       Position = StrippedSource.find(Nonisolated))
    StrippedSource.erase(Position, Nonisolated.size());
  // The metadata-loss fixture stays valid ordinary Objective-C; rejection
  // must come from the reconstructed interface contract.
  auto StrippedAST = Parse(StrippedSource, Cxx, Arc);

  Decoded->canonical_source_sha256 =
      ComputeInterfaceSourceSha256(StrippedSource);
  auto StrippedSidecar = EncodeSemanticInterfaceSidecar(*Decoded, Semantics);
  assert(StrippedSidecar);
  std::ofstream(InterfacePath) << StrippedSource;
  std::ofstream(InterfacePath.string() + ".json") << *StrippedSidecar;
  Consumer.source_text = "int baseline(void) { return 1; }";
  auto MissingMetadata = Compile(Consumer);
  assert(!MissingMetadata.success &&
         !MissingMetadata.interface_import_diagnostics.empty());
  fs::remove_all(Directory);

  auto EmittedAST = Parse(Emitted.source_bytes, Cxx, Arc);
  assert(clang::isObjC3Actor(
      Named<clang::ObjCInterfaceDecl>(*EmittedAST, "Counter")));
  assert(clang::getObjC3Executor(NamedEntry)->getExecutor() ==
         "named:db.\"quoted\"");
  clang::PrintingPolicy Policy(From->getLangOpts());
  Policy.PolishForDeclaration = true;
  std::string Printed;
  llvm::raw_string_ostream Out(Printed);
  for (const auto *D :
       From->getASTContext().getTranslationUnitDecl()->decls()) {
    if (D->isImplicit() ||
        !From->getSourceManager().isWrittenInMainFile(D->getLocation()))
      continue;
    D->print(Out, Policy);
    Out << ";\n";
  }
  assert(Printed.find("actor class Counter") != std::string::npos);
  assert(Printed.find("objc_executor(named(") != std::string::npos);
  assert(Printed.find("objc_task_spawn") != std::string::npos);
  auto Regenerated = Parse(Printed, Cxx, Arc);
  assert(clang::isObjC3Actor(
      Named<clang::ObjCInterfaceDecl>(*Regenerated, "Counter")));
  assert(clang::haveSameObjC3Isolation(
      NamedEntry, Named<clang::FunctionDecl>(*Regenerated, "named_entry")));

  llvm::SmallString<256> Path;
  int FD;
  assert(
      !llvm::sys::fs::createTemporaryFile("objc3-isolation", "ast", FD, Path));
  llvm::ToolOutputFile File(Path, FD);
  assert(!From->Save(Path));
  auto VFS = From->getFileManager().getVirtualFileSystemPtr();
  auto Options = std::make_shared<clang::DiagnosticOptions>();
  auto Diagnostics = clang::CompilerInstance::createDiagnostics(*VFS, *Options);
  auto Containers = std::make_shared<clang::PCHContainerOperations>();
  clang::HeaderSearchOptions HeaderOptions;
  auto Reloaded = clang::ASTUnit::LoadFromASTFile(
      Path, Containers->getRawReader(), clang::ASTUnit::LoadEverything, VFS,
      Options, Diagnostics, From->getFileSystemOpts(), HeaderOptions);
  assert(Reloaded);
  assert(clang::isObjC3Actor(
      Named<clang::ObjCInterfaceDecl>(*Reloaded, "Counter")));
  assert(clang::haveSameObjC3Isolation(
      NamedEntry, Named<clang::FunctionDecl>(*Reloaded, "named_entry")));
  assert(clang::getObjC3TaskBoundaryKind(
             Named<clang::FunctionDecl>(*Reloaded, "spawn")) == 1);
}

void CheckImportConflicts() {
  for (auto Other : {"global", "named(\"background\")"}) {
    auto From = Parse("__attribute__((objc_executor(main))) int entry(void);");
    auto To = Parse(std::string("__attribute__((objc_executor(") + Other +
                    "))) int entry(void);");
    clang::ASTImporter Importer(To->getASTContext(), To->getFileManager(),
                                From->getASTContext(), From->getFileManager(),
                                false);
    auto Imported = Importer.Import(Named<clang::FunctionDecl>(*From, "entry"));
    assert(!Imported);
    llvm::consumeError(Imported.takeError());
  }
  auto From = Parse(Root);
  auto To = Parse("__attribute__((objc_root_class)) @interface Root @end\n"
                  "@interface Counter : Root @end");
  clang::ASTImporter Importer(To->getASTContext(), To->getFileManager(),
                              From->getASTContext(), From->getFileManager(),
                              false);
  auto Imported =
      Importer.Import(Named<clang::ObjCInterfaceDecl>(*From, "Counter"));
  assert(!Imported);
  llvm::consumeError(Imported.takeError());
  auto A = Parse(
      "__attribute__((objc_executor(main))) int hashed(void) { return 1; }");
  auto B = Parse(
      "__attribute__((objc_executor(global))) int hashed(void) { return 1; }");
  clang::ODRHash AH, BH;
  AH.AddFunctionDecl(Named<clang::FunctionDecl>(*A, "hashed"));
  BH.AddFunctionDecl(Named<clang::FunctionDecl>(*B, "hashed"));
  assert(AH.CalculateHash() != BH.CalculateHash());
}
} // namespace

int main() {
  for (bool Arc : {false, true}) {
    CheckSource(false, Arc, "c17");
    CheckPrintAndPersistence(false, Arc);
    CheckAwaitBoundaries(false, Arc, "c17");
    CheckExecutorEntryIR(false, Arc, "c17");
    for (const std::string Standard : {"c++20", "c++23"}) {
      CheckSource(true, Arc, Standard);
      CheckConcurrentCaptures(Arc, Standard);
      CheckAwaitBoundaries(true, Arc, Standard);
      CheckExecutorEntryIR(true, Arc, Standard);
    }
    CheckPrintAndPersistence(true, Arc);
  }
  CheckImportConflicts();
}
