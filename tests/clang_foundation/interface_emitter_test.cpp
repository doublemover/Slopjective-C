#include "clang_foundation/compile_session.h"
#include "llvm/ADT/ScopeExit.h"
#include "clang_foundation/module_names.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/runtime_interface_types.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTImporter.h"
#include "clang/AST/ASTImporterSharedState.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/ADT/SmallString.h"

using namespace objc3::clang_foundation;

namespace {

void TestPrimitiveAndObjectiveCContractIdentity() {
  auto emit = [](const std::string &source, std::vector<std::string> flags) {
    flags.push_back("--target=x86_64-unknown-linux-gnu");
    auto ast = clang::tooling::buildASTFromCodeWithArgs(source, flags, "contract.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto result = EmitSemanticInterface(ast->getASTContext(), "fixture.contracts", "contracts");
    for (const auto &diagnostic : result.diagnostics)
      std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(), diagnostic.message.c_str());
    assert(result.success);
    return result;
  };
  const std::vector<std::string> c = {"-x", "c", "-std=c17"};
  const std::vector<std::string> objc = {"-x", "objective-c", "-std=c17", "-fobjc-runtime=gnustep-2.2"};
  const std::string byte = "char api_byte(void);\n";
  auto signed_byte = emit(byte, c);
  auto unsigned_flags = c;
  unsigned_flags.push_back("-funsigned-char");
  auto unsigned_byte = emit(byte, unsigned_flags);
  assert(signed_byte.identity.semantic_fingerprint != unsigned_byte.identity.semantic_fingerprint);
  assert(signed_byte.identity.language_profile != unsigned_byte.identity.language_profile);
  auto cpp = std::vector<std::string>{"-x", "c++", "-std=c++20"};
  const std::string wide = "extern \"C\" unsigned second(const wchar_t *);\n";
  auto normal_wide = emit(wide, cpp);
  cpp.push_back("-fshort-wchar");
  auto short_wide = emit(wide, cpp);
  assert(normal_wide.identity.semantic_fingerprint != short_wide.identity.semantic_fingerprint);
  for (const auto &flags : {c, objc}) {
    auto ordinary = emit("extern int counter;\n", flags);
    auto tls = emit("extern _Thread_local int counter;\n", flags);
    auto gnu_tls = emit("extern __thread int counter;\n", flags);
    assert(ordinary.identity.semantic_fingerprint != tls.identity.semantic_fingerprint);
    assert(tls.identity.semantic_fingerprint == gnu_tls.identity.semantic_fingerprint);
    for (const std::string shape : {"int * QUAL find_value(void);\n",
        "typedef int * QUAL Alias; Alias find_value(void);\n",
        "void callback(int * QUAL (*factory)(void));\n"}) {
      auto nullable_source = shape, nonnull_source = shape;
      nullable_source.replace(nullable_source.find("QUAL"), 4, "_Nullable");
      nonnull_source.replace(nonnull_source.find("QUAL"), 4, "_Nonnull");
      auto nullable = emit(nullable_source, flags);
      auto nonnull = emit(nonnull_source, flags);
      assert(nullable.identity.semantic_fingerprint != nonnull.identity.semantic_fingerprint);
    }
  }
  for (const std::string standard : {"c17", "c++20", "c++23"}) {
    const std::vector<std::string> flags = {
        "-x", standard == "c17" ? "objective-c" : "objective-c++",
        "-std=" + standard, "-fobjc-version=3", "-fobjc-runtime=gnustep-2.2"};
    for (const std::string shape : {
        "Optional(int * QUAL) maybe(void);\n",
        "Result(int * QUAL, int) success(void);\n",
        "Result(int, int * QUAL) failure(void);\n",
        "Optional(Result(Optional(int * QUAL), int)) nested(void);\n",
        "void callback(Result(int, Optional(int * QUAL)) (*factory)(void));\n",
        "typedef Optional(int * QUAL) Alias; Alias aliased(void);\n"}) {
      auto nullable_source = shape, nonnull_source = shape;
      nullable_source.replace(nullable_source.find("QUAL"), 4, "_Nullable");
      nonnull_source.replace(nonnull_source.find("QUAL"), 4, "_Nonnull");
      auto nullable = emit(nullable_source, flags);
      auto nonnull = emit(nonnull_source, flags);
      assert(nullable.identity.semantic_fingerprint != nonnull.identity.semantic_fingerprint);
      assert(emit(nullable_source, flags).identity.semantic_fingerprint ==
             nullable.identity.semantic_fingerprint);
    }
  }
  auto avx_flags = c;
  avx_flags.push_back("-mavx2");
  auto avx = emit(byte, avx_flags);
  assert(avx.identity.language_profile != signed_byte.identity.language_profile);
  avx_flags.push_back("-mavx2");
  assert(emit(byte, avx_flags).identity.language_profile == avx.identity.language_profile);
  const std::string object = "__attribute__((objc_root_class)) @interface Object @end\n";
  auto width_one = emit(object + "@interface Flags : Object { @public unsigned flag : 1; unsigned neighbor : 2; } @end\n", objc);
  auto width_two = emit(object + "@interface Flags : Object { @public unsigned flag : 2; unsigned neighbor : 2; } @end\n", objc);
  assert(width_one.identity.semantic_fingerprint != width_two.identity.semantic_fingerprint);
  auto getter_one = emit(object + "@interface Widget : Object\n- (int)first; - (int)second;\n@property(nonatomic,readonly,getter=first) int value;\n@end\n", objc);
  auto getter_two = emit(object + "@interface Widget : Object\n- (int)first; - (int)second;\n@property(nonatomic,readonly,getter=second) int value;\n@end\n", objc);
  assert(getter_one.identity.semantic_fingerprint != getter_two.identity.semantic_fingerprint);
  auto setter_one = emit(object + "@interface Widget : Object\n- (void)first:(int)value; - (void)second:(int)value;\n@property(nonatomic,setter=first:) int value;\n@end\n", objc);
  auto setter_two = emit(object + "@interface Widget : Object\n- (void)first:(int)value; - (void)second:(int)value;\n@property(nonatomic,setter=second:) int value;\n@end\n", objc);
  assert(setter_one.identity.semantic_fingerprint != setter_two.identity.semantic_fingerprint);
  auto required = emit("@protocol P\n@required\n@property(nonatomic) int value;\n@end\n", objc);
  auto optional = emit("@protocol P\n@optional\n@property(nonatomic) int value;\n@end\n", objc);
  assert(required.identity.semantic_fingerprint != optional.identity.semantic_fingerprint);
  const auto protocols = "@protocol P @end\n@protocol Q @end\n";
  auto adopts_p = emit(object + protocols + "@interface Object (Extra) <P> @end\n", objc);
  auto adopts_q = emit(object + protocols + "@interface Object (Extra) <Q> @end\n", objc);
  assert(adopts_p.source_bytes.find("(Extra) <P>") != std::string::npos);
  assert(adopts_p.identity.semantic_fingerprint != adopts_q.identity.semantic_fingerprint);
  auto invariant = emit(object + "@interface Box<T> : Object @end\n", objc);
  auto covariant = emit(object + "@interface Box<__covariant T> : Object @end\n", objc);
  assert(invariant.identity.semantic_fingerprint != covariant.identity.semantic_fingerprint);
  auto animal_bound = emit(object + "@interface Animal : Object @end\n@interface Dog : Animal @end\n@interface Box<T : Animal *> : Object @end\n", objc);
  auto dog_bound = emit(object + "@interface Animal : Object @end\n@interface Dog : Animal @end\n@interface Box<T : Dog *> : Object @end\n", objc);
  assert(animal_bound.identity.semantic_fingerprint != dog_bound.identity.semantic_fingerprint);
  const std::string generic = object + "@interface Animal : Object @end\n@interface Dog : Animal @end\n@interface Box<T> : Object @end\n";
  auto animal_super = emit(generic + "@interface Sub : Box<Animal *> @end\n", objc);
  auto dog_super = emit(generic + "@interface Sub : Box<Dog *> @end\n", objc);
  assert(animal_super.identity.semantic_fingerprint != dog_super.identity.semantic_fingerprint);
  auto rejects_hidden_dependency = [&](const std::string &source,
                                        std::vector<std::string> exports) {
    auto ast = clang::tooling::buildASTFromCodeWithArgs(source, objc, "hidden.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    for (auto *decl : ast->getASTContext().getTranslationUnitDecl()->decls())
      if (auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(decl))
        exports.push_back(ReadInterfaceExportRoot(category));
    auto result = EmitSemanticInterface(ast->getASTContext(), "fixture.hidden", "test",
                                        {}, {}, {}, exports);
    assert(!result.success && std::any_of(result.diagnostics.begin(),
        result.diagnostics.end(), [](const auto &d) { return d.code == "O3IFC156"; }));
  };
  rejects_hidden_dependency(object + "@protocol Hidden @end\n"
      "@interface Object (Extra) <Hidden> @end\n", {"Object"});
  rejects_hidden_dependency(object + "@interface Hidden : Object @end\n"
      "@interface Box<T : Hidden *> : Object @end\n", {"Object", "Box"});
  rejects_hidden_dependency(object + "@interface Hidden : Object @end\n"
      "@interface Box<T> : Object @end\n@interface Sub : Box<Hidden *> @end\n",
      {"Object", "Box", "Sub"});
}

void TestOrdinaryCDeclarationsRoundTrip() {
  constexpr llvm::StringRef source =
      "typedef struct Point { int x; int y; } Point;\n"
      "extern int shared_value;\n"
      "int add_points(Point lhs, Point rhs);\n"
      "typedef enum Kind { first = 2, second = 9 } Kind;\n"
      "typedef struct { int element; } AnonymousAlias;\n"
      "AnonymousAlias use_alias(AnonymousAlias value);\n";
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      source, {"-x", "c", "-std=c11", "--target=x86_64-unknown-linux-gnu"},
      "interface_fixture.c");
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());

  auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.points",
                                       "test-package");
  for (const auto &diagnostic : emitted.diagnostics)
    std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(),
                 diagnostic.message.c_str());
  assert(emitted.success);
  assert(emitted.diagnostics.empty());
  assert(!emitted.identity.semantic_fingerprint.empty());
  assert(emitted.sidecar_json.find("fixture.points") != std::string::npos);
  assert(emitted.source_bytes.find("struct Point") != std::string::npos);

  auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json,
                                                emitted.source_bytes, {});
  assert(static_cast<bool>(decoded));
  assert(decoded->module.canonical_name == "fixture.points");
  assert(decoded->declarations.size() >= 4);
  bool alias_field = false;
  for (const auto &row : decoded->declarations)
    if (row.kind == "Field" && row.qualified_name.ends_with("element")) {
      assert(row.export_root == "AnonymousAlias");
      alias_field = true;
    }
  assert(alias_field);
  assert(decoded->canonical_source_sha256 ==
         ComputeInterfaceSourceSha256(emitted.source_bytes));
}

void TestStandardLibraryImportsAndInstancetype() {
  llvm::SmallString<128> unique;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-sdk-module-contract", unique));
  const std::filesystem::path root(unique.c_str());
  for (const std::string mode : {"valid", "changed-header", "foreign-module",
                                 "non-system", "extra-header", "public-sdk-type"}) {
    const auto directory = root / mode;
    std::filesystem::create_directory(directory);
    const auto header = directory / "core.h";
    std::filesystem::copy_file(
        std::filesystem::path(OBJC3_INTERFACE_TEST_STDLIB_HEADERS) / "core.h", header);
    if (mode == "changed-header") {
      std::ofstream output(header, std::ios::app);
      output << "\n/* modified public SDK header */\n";
    }
    const std::string module = mode == "foreign-module" ? "foreign.core" : "objc3.core";
    {
      std::ofstream output(directory / "module.modulemap");
      output << "module " << (mode == "foreign-module" ? "foreign" : "objc3")
             << (mode == "non-system" ? "" : " [system]")
             << " { explicit module core { header \"core.h\" ";
      if (mode == "extra-header") {
        std::ofstream extra(directory / "extra.h");
        extra << "typedef int unexpected_sdk_type;\n";
        output << "header \"extra.h\" ";
      }
      output << "export * } }\n";
    }
    const std::string source = "@import " + module + ";\n" +
        "__attribute__((objc_root_class)) @interface ExportedRoot\n"
        "+ (instancetype)new;\n@end\n" +
        (mode == "public-sdk-type" ? "objc3_byte_span imported_span(void);\n"
                                    : "int exported_value(void);\n");
    auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
        {"-x", "objective-c", "-std=c17", "-fobjc-version=3",
         "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu",
         "-fmodules", "-fmodules-cache-path=" + (directory / "cache").string(),
         "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR,
         "-I" + directory.string()}, "sdk-module-interface.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    const auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.sdk", "test");
    if (mode == "valid") {
      assert(emitted.success);
      assert(emitted.source_bytes.find("instancetype") != std::string::npos);
      assert(emitted.source_bytes.find("@import") == std::string::npos);
    } else {
      assert(!emitted.success && emitted.source_bytes.empty());
      const char *expected = mode == "public-sdk-type" ? "O3IFC112" : "O3IFC111";
      assert(std::any_of(emitted.diagnostics.begin(), emitted.diagnostics.end(),
          [&](const auto &diagnostic) { return diagnostic.code == expected; }));
    }
  }
  std::filesystem::remove_all(root);
}

void TestNativeOwnershipContractsRoundTrip() {
  for (const char *language : {"objective-c", "objective-c++"}) {
    std::vector<std::string> args = {
        "-x",
        language,
        std::string(language) == "objective-c" ? "-std=c17" : "-std=c++20",
        "-fobjc-version=3",
        "-fobjc-runtime=gnustep-2.2",
        "--target=x86_64-unknown-linux-gnu"};
    auto ast =
        clang::tooling::buildASTFromCodeWithArgs(R"(
      extern char invalid_storage;
      void close_fd(int) __attribute__((nothrow));
      void close_pointer(char *) __attribute__((nothrow));
      typedef int FD __attribute__((objc_requires_cleanup(close_fd),
          objc_resource_family(Descriptors), objc_invalid(-1)));
      typedef FD Alias;
      typedef char *PointerOwner __attribute__((objc_requires_cleanup(close_pointer),
          objc_resource_family(Pointers), objc_invalid(&invalid_storage)));
      FD open_fd(void);
      void observe_fd(FD);
      Alias transfer_fd(Alias);
      PointerOwner open_pointer(void);
    )",
                                                 args, "ownership-interface.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted =
        EmitSemanticInterface(ast->getASTContext(), "fixture.owners", "test");
    assert(emitted.success && emitted.diagnostics.empty());
    const std::vector<llvm::StringRef> semantics = {
        "objc3.canonical-function-effects.v1", kNativeDeclarationAttributeSemantic,
        "objc3.native-ownership-contracts.v1", "objc3.cxx-data-interfaces.v1"};
    auto decoded = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(static_cast<bool>(decoded));
    const InterfaceDeclarationIdentity *fd = nullptr, *alias = nullptr,
                                       *factory = nullptr;
    for (const auto &declaration : decoded->declarations) {
      if (declaration.qualified_name == "FD")
        fd = &declaration;
      if (declaration.qualified_name == "Alias")
        alias = &declaration;
      if (declaration.qualified_name == "open_fd")
        factory = &declaration;
    }
    assert(fd && alias && factory);
    assert(fd->effects.ownership_identity == alias->effects.ownership_identity);
    assert(factory->effects.ownership_identity.find(".return:resource") !=
           std::string::npos);
    assert(emitted.source_bytes.find("objc_requires_cleanup") !=
           std::string::npos);
    auto reparsed = clang::tooling::buildASTFromCodeWithArgs(
        emitted.source_bytes, args, "ownership-import.m");
    assert(reparsed && !reparsed->getDiagnostics().hasErrorOccurred());
    auto repeated = EmitSemanticInterface(reparsed->getASTContext(),
                                          "fixture.owners", "test");
    assert(repeated.success);
    assert(repeated.identity.semantic_fingerprint ==
           emitted.identity.semantic_fingerprint);
    auto unsupported = DecodeSemanticInterfaceSidecar(emitted.sidecar_json,
                                                      emitted.source_bytes, {});
    assert(!unsupported);
    llvm::consumeError(unsupported.takeError());
  }
}

void TestOpaqueKeyPathSignatureRoundTrips() {
  for (const char *language : {"objective-c", "objective-c++"}) {
    std::vector<std::string> args = {
        "-x",
        language,
        std::string(language) == "objective-c" ? "-std=c17" : "-std=c++23",
        "-fobjc-version=3",
        "-fobjc-runtime=gnustep-2.2",
        "--target=x86_64-unknown-linux-gnu"};
    auto ast =
        clang::tooling::buildASTFromCodeWithArgs(R"(
      __attribute__((objc_root_class)) @interface PathRoot @end
      typedef KeyPath<PathRoot *, int> ReadPath;
      typedef WritableKeyPath<PathRoot *, int> WritePath;
      ReadPath read_path(void);
      WritePath write_path(void);
      void receive_path(ReadPath);
    )",
                                                 args, "keypath-signatures.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted =
        EmitSemanticInterface(ast->getASTContext(), "fixture.paths", "test");
    assert(emitted.success);
    assert(emitted.source_bytes.find("__objc3_keypath_") == std::string::npos);
    const std::vector<llvm::StringRef> supported = {
        "objc3.typed-keypaths.v1", "objc3.canonical-function-effects.v1",
        "objc3.native-ownership-contracts.v1", "objc3.cxx-data-interfaces.v1"};
    auto decoded = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, supported);
    assert(decoded);
    auto imported = clang::tooling::buildASTFromCodeWithArgs(
        emitted.source_bytes, args, "relocated.m");
    assert(imported && !imported->getDiagnostics().hasErrorOccurred());
    auto repeated = EmitSemanticInterface(imported->getASTContext(),
                                          "fixture.paths", "test");
    assert(repeated.success);
    assert(repeated.identity.semantic_fingerprint ==
           emitted.identity.semantic_fingerprint);
  }
}

void TestDeducedTaskGroupReplay() {
 for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
  std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fblocks","-fexceptions","-fobjc-exceptions","-fobjc-arc-exceptions","-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
  if(arc)args.push_back("-fobjc-arc");
  clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
  auto parse=[&](const std::string &text){return clang::tooling::buildASTFromCodeWithArgs(text,args,"deduced-group.mm","clang-tool",std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),files);};
  const std::string source=R"(
   #include <objc3c/runtime/objc3_task.hpp>
   namespace PublishedGroup {
    inline objc3::task<int> leaf(int value){co_await objc3::task_yield;co_return value;}
    inline objc3::task<int> callback(objc3::task_group_handle group){auto child=leaf(42).start_child(group);co_return co_await child;}
    inline auto make(){return objc3::with_task_group(callback);}
    inline auto captured(int value){return objc3::with_task_group([value](objc3::task_group_handle group)->objc3::task<int>{auto child=leaf(value).start_child(group);co_return co_await child;});}
   }
  )";
  auto emit=[&](const std::string &text) {
   auto ast=parse(text);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.deduced_group","test",{},{},{},{"PublishedGroup"});
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  assert(first.source_bytes.find("with_task_group_owned")==std::string::npos);
  {
   auto tampered=files;bool changed=false;
   for(auto &[name,bytes]:tampered)if(llvm::StringRef(name).ends_with("/objc3_task.hpp")){bytes+="\n// unmatched task header\n";changed=true;}
   assert(changed);
   auto invalid=clang::tooling::buildASTFromCodeWithArgs(source,args,"mismatched-group.mm","clang-tool",std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),tampered);
   assert(invalid && !invalid->getDiagnostics().hasErrorOccurred());
   assert(!EmitSemanticInterface(invalid->getASTContext(),"fixture.deduced_group","test",{},{},{},{"PublishedGroup"}).success);
  }
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
  assert(llvm::is_contained(decoded->required_semantics,std::string(kSdkRuntimeDeclarationSemantic)));
  auto consumer=parse(first.source_bytes+R"(
   static_assert(std::is_same_v<decltype(PublishedGroup::make()),objc3::task<int>>);
   static_assert(std::is_same_v<decltype(PublishedGroup::captured(42)),objc3::task<int>>);
   objc3::task<int> deduced_group_consumer(){co_return co_await PublishedGroup::captured(42);}
  )");assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
 }
}

void TestNativeEmissionPolicyReplay() {
 for(const char *standard:{"c17","c++20","c++23"})for(bool arc:{false,true}) {
  const bool cpp=std::string(standard)!="c17";
  std::vector<std::string> args={"-x",cpp?"objective-c++":"objective-c",std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2","--target=x86_64-unknown-linux-gnu"};
  if(arc)args.push_back("-fobjc-arc");
  const std::string source=R"(
   extern int policy_data __attribute__((section(".data.objc3.policy"),weak));
   int policy_data=17;
   int policy_read(const int *) __attribute__((section(".text.objc3.policy"),used,retain,noinline,no_sanitize("address","undefined"),disable_sanitizer_instrumentation,no_profile_instrument_function,no_instrument_function,no_stack_protector,nodebug));
   inline int policy_read(const int *value){return *value+policy_data;}
  )";
  auto emit=[&](const std::string &text) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-emission-policy.m");assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.emission_policy","test",{}));
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.emission_policy","test");
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.source_bytes==replay.source_bytes);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
  assert(llvm::is_contained(decoded->required_semantics,std::string(kNativeEmissionPolicySemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kNativeEmissionPolicySemantic));
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(decoded->required_semantics,kNativeEmissionPolicySemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*decoded,supported);assert(!missing);llvm::consumeError(missing.takeError());
  for(const auto &[before,after]:std::vector<std::pair<std::string,std::string>>{{".text.objc3.policy",".text.objc3.other"},{".data.objc3.policy",".data.objc3.other"},{"\"address\",\"undefined\"","\"address\""},{"disable_sanitizer_instrumentation,",""},{"no_profile_instrument_function,",""},{"no_instrument_function,",""},{"no_stack_protector,",""},{",nodebug",""},{",weak",""},{"used,",""},{"retain,",""}}) {
   auto changed=source;auto at=changed.find(before);assert(at!=std::string::npos);changed.replace(at,before.size(),after);
   assert(emit(changed).identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  }
  auto equivalent=source;
  auto reordered=equivalent.find("\"address\",\"undefined\"");assert(reordered!=std::string::npos);
  equivalent.replace(reordered,std::string("\"address\",\"undefined\"").size(),"\"undefined\",\"address\"");
  assert(emit(equivalent).identity.semantic_fingerprint==first.identity.semantic_fingerprint);
  auto repeated=source;auto flags=repeated.find("used,retain");assert(flags!=std::string::npos);
  repeated.replace(flags,std::string("used,retain").size(),"used,used,retain,retain");
  auto merged=emit(repeated);assert(merged.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
  assert(merged.source_bytes==first.source_bytes);
  auto plain=emit("inline int policy_read(const int *value){return *value;}");
  auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
  assert(!llvm::is_contained(invented->required_semantics,std::string(kNativeEmissionPolicySemantic)));
  invented->required_semantics.push_back(kNativeEmissionPolicySemantic);
  auto invalid=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
  if(cpp) {
   auto local=emit(R"(inline int policy_local(){static __attribute__((section(".data.objc3.local"),used,retain)) int value=23;return value;})");
   auto localReplay=emit(local.source_bytes);assert(local.identity.semantic_fingerprint==localReplay.identity.semantic_fingerprint);
   auto changed=emit(R"(inline int policy_local(){static __attribute__((section(".data.objc3.other"),used,retain)) int value=23;return value;})");
   assert(local.identity.semantic_fingerprint!=changed.identity.semantic_fingerprint);
   auto generic=emit(R"(template<class T>__attribute__((no_sanitize("address"),no_profile_instrument_function)) inline T policy_generic(T value){return value;}inline int policy_call(){return policy_generic(29);})");
   auto genericReplay=emit(generic.source_bytes);assert(generic.identity.semantic_fingerprint==genericReplay.identity.semantic_fingerprint);
  }
 }
}

void TestNativeManagedUnionPayloadReplay() {
 for(const char *standard:{"c++20","c++23"}) {
  std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc-arc","-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
  const std::string source=R"(
   @protocol Error @end
   namespace NativeUnionPayload {
    struct Arm {int tag;id __strong owner;id __weak observer;int value;};
    union Value;
    union alignas(64) Value {Arm owned;int scalar;
     Value();Value(const Value &);Value(Value &&);~Value() noexcept;};
    using Maybe=__objc3_optional(Value);using Result=__objc3_result(Value,id __strong);
    inline Maybe make(){return __objc3_some(Value,Value());}
    inline Maybe copy(const Maybe &value){return __objc3_copy(value);}
    inline Maybe implicit_result(){Maybe value=make();return value;}
    inline Maybe checked() __objc3_throws(id<Error>){Maybe value=make();return value;}
    inline Maybe captured(const Maybe &value){auto callback=[value]{return __objc3_copy(value);};return callback();}
   }
   __attribute__((objc_root_class)) @interface PublishedNativeUnionHost
   - (NativeUnionPayload::Maybe)echo:(NativeUnionPayload::Maybe)value;
   @end
   inline NativeUnionPayload::Maybe published_union_message(PublishedNativeUnionHost *_Nonnull object,NativeUnionPayload::Maybe value){return [object echo:value];}
  )";
  auto emit=[&](const std::string &text) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-union-carrier.mm");assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_union_carrier","test",{},{},{},{"Error","NativeUnionPayload","PublishedNativeUnionHost","published_union_message"});
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
  assert(llvm::is_contained(decoded->required_semantics,std::string(kNativeARCRecordPayloadSemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kNativeARCRecordPayloadSemantic));older.push_back("objc3.native-arc-record-payload.v1");
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(decoded->required_semantics,kNativeARCRecordPayloadSemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*decoded,supported);assert(!missing);llvm::consumeError(missing.takeError());
  auto consumer=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes+R"(
   int native_union_consumer(PublishedNativeUnionHost *_Nonnull object){auto value=NativeUnionPayload::implicit_result();auto copy=published_union_message(object,value);return __objc3_has_value(copy);}
  )",args,"native-union-consumer.mm");assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
 }
}

void TestNativeUnnamedEnumReplay() {
 for(const char *standard:{"c17","c++20","c++23"})for(bool arc:{false,true}) {
  const bool cpp=std::string(standard)!="c17";
  std::vector<std::string> args={"-x",cpp?"objective-c++":"objective-c",std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"};
  if(arc)args.push_back("-fobjc-arc");
  const std::string body=R"(
   enum {First=201,Second=203};
   enum {Third=17,Fourth=19};
   void initialize(void) __attribute__((constructor(First)));
  )";
  const std::string extra=cpp?R"(
   using Choice=decltype(First);
   inline constexpr Choice selected=First;
   inline constexpr Choice identity(Choice value){return value;}
   inline decltype(Third) indirect(decltype(Third) value){return value;}
   template<auto Value>struct ValueTag {static constexpr auto value=Value;};
   inline int choose(){return ValueTag<First>::value;}
   using Unlisted=ValueTag<static_cast<Choice>(211)>;
   inline int unlisted(){return Unlisted::value;}
   template<class T>struct TypeTag {T value;};
   using Wrapped=TypeTag<Choice>;
   inline Wrapped wrapped(){return {First};}
   struct Container {enum {Member=23,Other=29};};
  )":"";
  const std::string source=cpp?"namespace NativeEnum {"+body+extra+"}":body;
  auto emit=[&](const std::string &text,const std::vector<std::string> &names=std::vector<std::string>{}) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-enums.m");assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_enums","test",{},{},{},names);
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
  assert(llvm::is_contained(decoded->required_semantics,std::string(kNativeUnnamedEnumSemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kNativeUnnamedEnumSemantic));
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(decoded->required_semantics,kNativeUnnamedEnumSemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*decoded,supported);assert(!missing);llvm::consumeError(missing.takeError());
  const std::string prefix=cpp?"NativeEnum::":"";
  for(const std::string &member:{"First","Second"}) {
   auto subset=emit(source,{prefix+member});auto again=emit(subset.source_bytes,{prefix+member});
   assert(subset.identity.semantic_fingerprint==again.identity.semantic_fingerprint);
   assert(subset.source_bytes.find("Second")!=std::string::npos);
   assert(subset.source_bytes.find("Third")==std::string::npos);
  }
  auto changed=source;changed.replace(changed.find("Second=203"),10,"Second=213");
  assert(emit(changed).identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  auto named=emit(cpp?"namespace NativeEnum {enum Named {First=201,Second=203};}":"enum Named {First=201,Second=203};");
  auto invented=DecodeSemanticInterfaceSidecar(named.sidecar_json,named.source_bytes,supported);assert(invented);
  assert(!llvm::is_contained(invented->required_semantics,std::string(kNativeUnnamedEnumSemantic)));
  invented->required_semantics.push_back(kNativeUnnamedEnumSemantic);
  auto invalid=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
  if(cpp) {
   for(const std::string &boundary:{
       "namespace NativeEnum {Choice unavailable(Choice);}",
       "namespace NativeEnum {static inline Choice hidden(Choice value){return value;} inline Choice unavailable(Choice value){return hidden(value);}}"}) {
    auto ast=clang::tooling::buildASTFromCodeWithArgs(source+boundary,args,"enum-boundary.mm");assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto rejected=EmitSemanticInterface(ast->getASTContext(),"fixture.enum_boundary","test",{},{},{},{"NativeEnum::First","NativeEnum::Choice","NativeEnum::unavailable"});
    assert(!rejected.success);
   }
   const std::string owners="namespace NativeEnum {enum {A=1,B=2};enum {C=1,D=2};using Result=decltype(A);Result selected();}";
   auto other_owner=owners;other_owner.replace(other_owner.find("decltype(A)"),11,"decltype(C)");
   assert(emit(owners).identity.semantic_fingerprint!=emit(other_owner).identity.semantic_fingerprint);
   const auto narrow=emit("namespace NativeEnum {enum : unsigned char {First=1,Second=2};}");
   const auto wide=emit("namespace NativeEnum {enum : unsigned long {First=1,Second=2};}");
   assert(narrow.identity.semantic_fingerprint!=wide.identity.semantic_fingerprint);
   auto consumer=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes+R"(
    static_assert(NativeEnum::identity(NativeEnum::selected)==NativeEnum::First);
    static_assert(NativeEnum::Container::Other==29);
    static_assert(NativeEnum::Unlisted::value==static_cast<NativeEnum::Choice>(211));
    int enum_consumer(){return NativeEnum::choose()+NativeEnum::indirect(NativeEnum::Third);}
   )",args,"enum-consumer.mm");assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
  }
 }
}

void TestNativeStorageLifetimeReplay() {
 for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
  std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"};
  if(arc)args.push_back("-fobjc-arc");
  const std::string source=R"(
   namespace NativeStorage {
    enum PriorityValue {Priority=201};
    struct Owner {int value;constexpr explicit Owner(int value):value(value){}~Owner() noexcept {}};
    void start() __attribute__((constructor(Priority)));
    void finish() __attribute__((destructor(203)));
    inline constinit int ready=42;
    __attribute__((require_constant_initialization)) inline int attribute_ready=43;
    __attribute__((init_priority(202),always_destroy)) inline Owner ordered(44);
    __attribute__((no_destroy)) inline Owner retained(45);
    __attribute__((no_destroy)) inline thread_local Owner thread_retained(46);
    template<class T>__attribute__((no_destroy)) inline T templated{};
    inline Owner &local() {static __attribute__((no_destroy)) Owner value(47);return value;}
    inline Owner &local_destroyed() {static __attribute__((always_destroy)) Owner value(48);return value;}
    inline int local_constant() {static constinit int value=49;return value;}
    int plain();
   }
  )";
  auto emit=[&](const std::string &text,const std::vector<std::string> &names=std::vector<std::string>{"NativeStorage"}) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-storage.mm");assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_storage","test",{},{},{},names);
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto private_ast=clang::tooling::buildASTFromCodeWithArgs(source,args,"private-storage-priority.mm");assert(private_ast && !private_ast->getDiagnostics().hasErrorOccurred());
  auto private_priority=EmitSemanticInterface(private_ast->getASTContext(),"fixture.native_storage","test",{},{},{},{"NativeStorage::start"});
  assert(!private_priority.success);
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
  assert(llvm::is_contained(decoded->required_semantics,std::string(kNativeStorageLifetimeSemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kNativeStorageLifetimeSemantic));
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(decoded->required_semantics,kNativeStorageLifetimeSemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*decoded,supported);assert(!missing);llvm::consumeError(missing.takeError());
  for(const std::string &name:{"local","local_constant","plain"}) {
   auto subset=emit(source,{"NativeStorage::Owner","NativeStorage::"+name});
   auto read=DecodeSemanticInterfaceSidecar(subset.sidecar_json,subset.source_bytes,supported);assert(read);
   assert(llvm::is_contained(read->required_semantics,std::string(kNativeStorageLifetimeSemantic))==(name!="plain"));
   if(name=="plain") {read->required_semantics.push_back(kNativeStorageLifetimeSemantic);auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());}
  }
  for(const auto &change:std::vector<std::pair<std::string,std::string>>{
    {"Priority=201","Priority=211"},{"destructor(203)","destructor(213)"},
    {"init_priority(202)","init_priority(212)"},{"no_destroy","always_destroy"},
    {"constinit int ready","int ready"},{"static __attribute__((no_destroy)) Owner value(47)","static __attribute__((always_destroy)) Owner value(47)"}}) {
   auto changed=source;auto at=changed.find(change.first);assert(at!=std::string::npos);changed.replace(at,change.first.size(),change.second);
   auto next=emit(changed);assert(next.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  }
  for(const char *policy:{"-fc++-static-destructors=thread-local","-fno-c++-static-destructors","-fno-threadsafe-statics"}) {
   auto changed_args=args;changed_args.push_back(policy);
   auto changed_ast=clang::tooling::buildASTFromCodeWithArgs(source,changed_args,"storage-policy.mm");assert(changed_ast && !changed_ast->getDiagnostics().hasErrorOccurred());
   auto changed=EmitSemanticInterface(changed_ast->getASTContext(),"fixture.native_storage","test",{},{},{},{"NativeStorage"});
   for(const auto &d:changed.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(changed.success);assert(changed.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  }
  auto consumer=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes+R"(
   int storage_consumer(){return NativeStorage::local().value+NativeStorage::local_destroyed().value+NativeStorage::local_constant()+NativeStorage::ready+NativeStorage::templated<int>;}
  )",args,"storage-consumer.mm");assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
 }
 for(bool arc:{false,true}) {
  std::vector<std::string> args={"-x","objective-c","-std=c17","-fobjc-version=3","-fobjc-runtime=gnustep-2.2"};
  if(arc)args.push_back("-fobjc-arc");
  auto emit=[&](const std::string &text) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"c-storage-priority.m");assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.c_storage","test",{},{},{},{});assert(result.success);return result;
  };
  auto first=emit("enum NativePriority {NativeStart=201};void initialize(void) __attribute__((constructor(NativeStart)));void finalize(void) __attribute__((destructor(203)));");
  auto replay=emit(first.source_bytes);assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
  assert(llvm::is_contained(decoded->required_semantics,std::string(kNativeStorageLifetimeSemantic)));
 }
}

void TestNativeUnionKeyPathReplay() {
 for(const char *standard:{"c++20","c++23"}) {
  std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
   "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc-arc","-fblocks",
   "-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
  clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args);
  args.insert(args.end(),{"-include","objc3c/runtime/objc3_keypath.h"});
  const std::string source=R"(
   struct PublishedUnionArm {int tag;id owner;id __weak observer;int value;};
   union PublishedManagedUnion {
    PublishedUnionArm owned;struct Plain {int tag,value;} plain;
    PublishedManagedUnion();PublishedManagedUnion(const PublishedManagedUnion &);
    PublishedManagedUnion(PublishedManagedUnion &&) noexcept;~PublishedManagedUnion();
   };
   struct PublishedUnionEnvelope {int prefix;PublishedManagedUnion active;};
   __attribute__((objc_root_class)) @interface PublishedUnionRoot
   @property(nonatomic) PublishedManagedUnion active;
   @property(nonatomic) PublishedUnionEnvelope envelope;
   @property(nonatomic,strong) PublishedUnionRoot *_Nullable next;
   @end
   KeyPath<PublishedUnionRoot *,PublishedManagedUnion> published_union_signature();
   inline PublishedManagedUnion published_union_read(PublishedUnionRoot *_Nonnull root) {
    return KeyPathGet(@keypath(PublishedUnionRoot,active),root);
   }
   inline int published_union_field(PublishedUnionRoot *_Nonnull root,int value) {
    root[@keypath(PublishedUnionRoot,envelope.active.owned.value)]=value;
    return root[@keypath(PublishedUnionRoot,active.owned.value)];
   }
   inline void published_union_owner(PublishedUnionRoot *_Nonnull root,id owner) {
    KeyPathSet(@keypath(PublishedUnionRoot,next.active.owned.owner),root,owner);
   }
   inline int published_union_plain(){return 0;}
  )";
  auto parse=[&](const std::string &text){auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-union-keypath.mm","clang-tool",std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),files);assert(ast && !ast->getDiagnostics().hasErrorOccurred());return ast;};
  const std::vector<std::string> types={"PublishedUnionArm","PublishedManagedUnion","PublishedUnionEnvelope","PublishedUnionRoot"};
  auto emit=[&](const std::string &text,const std::vector<std::string> &names) {
   auto ast=parse(text);auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_union_paths","test",{},{},{},names);
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto names=types;names.insert(names.end(),{"published_union_signature","published_union_read","published_union_field","published_union_owner"});
  auto first=emit(source,names),replay=emit(first.source_bytes,names),relocated=emit("\n\n"+source,names);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
  assert(llvm::is_contained(decoded->required_semantics,std::string(kResourceKeyPathValueSemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kResourceKeyPathValueSemantic));older.push_back("objc3.keypath-resource-values.v1");
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(decoded->required_semantics,kResourceKeyPathValueSemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*decoded,supported);assert(!missing);llvm::consumeError(missing.takeError());
  for(const std::string &name:{"published_union_signature","published_union_field","published_union_plain"}) {
   auto subset_names=types;subset_names.push_back(name);auto subset=emit(source,subset_names);
   auto read=DecodeSemanticInterfaceSidecar(subset.sidecar_json,subset.source_bytes,supported);assert(read);
   assert(llvm::is_contained(read->required_semantics,std::string(kResourceKeyPathValueSemantic))==(name!="published_union_plain"));
   if(name=="published_union_plain") {
    read->required_semantics.push_back(kResourceKeyPathValueSemantic);auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
   }
  }
  auto consumer=parse(first.source_bytes+R"(
   int consume_native_union(PublishedUnionRoot *_Nonnull root,id owner) {
    auto whole=published_union_read(root);published_union_owner(root,owner);
    return published_union_field(root,whole.owned.value);
   }
  )");
 }
}

void TestResourceKeyPathReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
        "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fexceptions","-fobjc-exceptions",
        "--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args);
    args.insert(args.end(),{"-include","objc3c/runtime/objc3_keypath.h"});
    auto parse=[&](const std::string &text,const char *name) {
      return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
          std::make_shared<clang::PCHContainerOperations>(),
          clang::tooling::getClangStripDependencyFileAdjuster(),files);
    };
    const std::string source=R"(
      void published_path_close(int) noexcept;
      typedef int PublishedPathHandle __attribute__((objc_requires_cleanup(published_path_close),objc_resource_family(PublishedPathHandles),objc_invalid(-1)));
      struct PublishedPathOwner {
        PublishedPathHandle handle;int value;
        explicit PublishedPathOwner(int);
        PublishedPathOwner(const PublishedPathOwner &);
        PublishedPathOwner(PublishedPathOwner &&) noexcept;
        ~PublishedPathOwner() noexcept;
      };
      struct PublishedPathPair {PublishedPathOwner first,second;};
      union PublishedPathUnion {
        PublishedPathHandle handle;int plain;
        explicit PublishedPathUnion(int);PublishedPathUnion(const PublishedPathUnion &);
        PublishedPathUnion(PublishedPathUnion &&) noexcept;~PublishedPathUnion() noexcept;
      };
      __attribute__((objc_root_class)) @interface PublishedResourcePath
      @property(nonatomic) PublishedPathOwner owner;
      @property(nonatomic) PublishedPathPair pair;
      @property(nonatomic) PublishedPathUnion active;
      @property(nonatomic,strong) PublishedResourcePath *_Nullable next;
      @end
      inline int published_resource_plain() {return 0;}
      KeyPath<PublishedResourcePath *,PublishedPathUnion> published_union_signature();
      inline PublishedPathUnion published_union_get(PublishedResourcePath *_Nonnull root) {
        return KeyPathGet(@keypath(PublishedResourcePath,active),root);
      }
      KeyPath<PublishedResourcePath *,PublishedPathOwner> published_resource_signature();
      inline PublishedPathOwner published_resource_get(PublishedResourcePath *_Nonnull root) {
        return KeyPathGet(@keypath(PublishedResourcePath,owner),root);
      }
      inline void published_resource_set(PublishedResourcePath *_Nonnull root,PublishedPathOwner value) {
        KeyPathSet(@keypath(PublishedResourcePath,next.owner),root,value);
      }
      inline int published_resource_field(PublishedResourcePath *_Nonnull root,int value) {
        root[@keypath(PublishedResourcePath,pair.first.value)]=value;
        return root[@keypath(PublishedResourcePath,owner.value)];
      }
    )";
    auto emit=[&](const std::string &text,const char *name) {
      auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      auto emitted=EmitSemanticInterface(ast->getASTContext(),"fixture.resource_paths","test",{},{},{},
          {"published_path_close","PublishedPathHandle","PublishedPathOwner","PublishedPathPair","PublishedPathUnion","PublishedResourcePath",
           "published_union_signature","published_union_get","published_resource_signature","published_resource_get","published_resource_set","published_resource_field"});
      for(const auto &d:emitted.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(emitted.success);return emitted;
    };
    auto first=emit(source,"resource-path-provider.mm");
    auto replay=emit(first.source_bytes,"resource-path-replay.mm");
    auto relocated=emit("\n\n"+source,"resource-path-relocated.mm");
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
    assert(llvm::is_contained(decoded->required_semantics,std::string(kResourceKeyPathValueSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kResourceKeyPathValueSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto missing=*decoded;std::erase(missing.required_semantics,kResourceKeyPathValueSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto selected=parse(source,"resource-path-selected.mm");assert(selected && !selected->getDiagnostics().hasErrorOccurred());
    for(const std::string &name:{"published_union_signature","published_union_get","published_resource_signature","published_resource_field","published_resource_plain"}) {
      auto subset=EmitSemanticInterface(selected->getASTContext(),"fixture.resource_paths","test",{},{},{},
          {"published_path_close","PublishedPathHandle","PublishedPathOwner","PublishedPathPair","PublishedPathUnion","PublishedResourcePath",name});
      for(const auto &d:subset.diagnostics)std::fprintf(stderr,"subset %s %s: %s (%s)\n",name.c_str(),d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(subset.success);
      auto read=DecodeSemanticInterfaceSidecar(subset.sidecar_json,subset.source_bytes,supported);assert(read);
      const bool required=name!="published_resource_plain";
      assert(llvm::is_contained(read->required_semantics,std::string(kResourceKeyPathValueSemantic))==required);
      if(!required) {
        read->required_semantics.push_back(kResourceKeyPathValueSemantic);
        auto invented=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invented);llvm::consumeError(invented.takeError());
      }
    }

    auto consumer=parse(first.source_bytes+R"(
      int resource_path_consumer(PublishedResourcePath *_Nonnull object) {
        return published_resource_field(object,41);
      }
    )","resource-path-consumer.mm");
    assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
    auto bad=parse(first.source_bytes+R"(
      PublishedPathHandle raw_resource_consumer(PublishedResourcePath *_Nonnull object) {
        return KeyPathGet(@keypath(PublishedResourcePath,owner.handle),object);
      }
    )","resource-path-raw-rejection.mm");
    assert(bad && bad->getDiagnostics().hasErrorOccurred());
  }
}

void TestDependentKeyPathLiteralBodies() {
  for (const char *standard : {"c++20", "c++23"}) {
    for (bool arc : {false, true}) {
      std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
          "-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "--target=x86_64-unknown-linux-gnu"};
      if (arc) args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;
      AddRuntimeInterfaceFiles(files,args);
      args.push_back("-include");args.push_back("objc3c/runtime/objc3_keypath.h");
      auto parse=[&](llvm::StringRef source,llvm::StringRef file) {
        return clang::tooling::buildASTFromCodeWithArgs(source,args,file,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      const std::string source=R"(
        __attribute__((objc_root_class)) @interface DependentRoot
        @property(nonatomic) int count;
        @property(nonatomic) long alternative;
        @end
        template<class Root> auto make_path() {return @keypath(Root,count);}
        template<class Root> auto read_path(Root root) {
          return KeyPathGet(@keypath(Root,count),root);
        }
        template<class Root, class Value>
        Value typed_read(KeyPath<Root, Value> path, Root root) {
          return KeyPathGet(path,root);
        }
        template<class Root, class Value>
        void typed_write(WritableKeyPath<Root, Value> path, Root root, Value value) {
          KeyPathSet(path,root,value);
        }
        template<class Root, class Value> void typed_signature_only(KeyPath<Root, Value>);
      )";
      const std::vector<std::string> exports={"DependentRoot","make_path","read_path","typed_read","typed_write","typed_signature_only"};
      auto emit=[&](llvm::StringRef text,llvm::StringRef file) {
        auto ast=parse(text,file);
        assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.dependent.paths",
            "test",{},{},{},exports);
        for (const auto &d:result.diagnostics)
          std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);
        return result;
      };
      auto emitted=emit(source,"dependent-path-provider.mm");
      assert(emitted.source_bytes.find("@keypath(")!=std::string::npos);
      assert(emitted.source_bytes.find("__objc3_dependent_keypath_literal")==std::string::npos);
      assert(emitted.sidecar_json.find("objc3.keypath-dependent-literals.v1")!=std::string::npos);
      auto json=llvm::json::parse(emitted.sidecar_json);
      assert(json && json->getAsObject());
      const auto *required=json->getAsObject()->getArray("required_semantics");
      assert(required);
      std::vector<llvm::StringRef> supported;
      for (const auto &item:*required) {assert(item.getAsString());supported.push_back(*item.getAsString());}
      auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,supported);
      if (!decoded) {llvm::errs()<<llvm::toString(decoded.takeError())<<"\n";assert(false);}
      auto repeated=emit(emitted.source_bytes,"dependent-path-consumer.mm");
      assert(repeated.identity.semantic_fingerprint==emitted.identity.semantic_fingerprint);
      auto relocated=emit(source,"relocated/dependent-path-provider.mm");
      assert(relocated.identity.semantic_fingerprint==emitted.identity.semantic_fingerprint);
      assert(emitted.sidecar_json.find("objc3.keypath-dependent-types.v1")!=std::string::npos);
      auto old_types=supported;
      std::erase(old_types,llvm::StringRef("objc3.keypath-dependent-types.v1"));
      auto unsupported_types=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,old_types);
      assert(!unsupported_types);llvm::consumeError(unsupported_types.takeError());
      auto missing_type=*decoded;
      std::erase(missing_type.required_semantics,"objc3.keypath-dependent-types.v1");
      auto rejected_type=EncodeSemanticInterfaceSidecar(missing_type,supported);
      assert(!rejected_type);llvm::consumeError(rejected_type.takeError());
      auto old=supported;
      std::erase(old,llvm::StringRef("objc3.keypath-dependent-literals.v1"));
      auto unsupported=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,old);
      assert(!unsupported);llvm::consumeError(unsupported.takeError());
      auto missing=*decoded;
      std::erase(missing.required_semantics,"objc3.keypath-dependent-literals.v1");
      auto rejected=EncodeSemanticInterfaceSidecar(missing,supported);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto changed=source;
      auto member=changed.find("@keypath(Root,count)");assert(member!=std::string::npos);
      changed.replace(member,std::string("@keypath(Root,count)").size(),"@keypath(Root,alternative)");
      auto different=emit(changed,"dependent-path-changed.mm");
      assert(different.identity.semantic_fingerprint!=emitted.identity.semantic_fingerprint);
      auto consumer=parse(emitted.source_bytes+R"(
        int instantiate_dependent_paths(DependentRoot *_Nonnull root) {
          auto path=make_path<DependentRoot *>();
          KeyPathSet(path,root,37);
          typed_write(path,root,41);
          auto read=KeyPathAsReadOnly(path);
          return typed_read(read,root)+read_path(root)+KeyPathGet(path,root);
        }
      )","dependent-path-instantiation.mm");
      assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
    }
  }
}


void TestNSObjectStorageCapability() {
  for(const char *standard:{"c17","c++20","c++23"}) {
    const bool cxx=std::string(standard)!="c17";
    for(bool arc:{false,true}) {
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"};
      if(arc)args.push_back("-fobjc-arc");
      const std::string source=R"(
        typedef void *NativeObject __attribute__((NSObject));
        typedef NativeObject NativeAlias;
        void native_value(NativeObject);
        void native_consume(NativeObject __attribute__((cf_consumed)));
        NativeObject native_create(void) __attribute__((cf_returns_retained));
        void plain_value(void *);
        #ifdef __cplusplus
        inline int body_only(void *input) {
          typedef void *LocalObject __attribute__((NSObject));
          LocalObject local=(LocalObject)input;return local!=0;
        }
        template<class T> NativeObject native_identity(T input){return (NativeObject)input;}
        #endif
      )";
      auto emit=[&](const std::string &text,const char *file,const std::vector<std::string>&exports) {
        auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,file);
        assert(ast&&!ast->getDiagnostics().hasErrorOccurred());
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.nsobject_storage","test",{},{},{},exports);
        for(const auto&d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto plain=emit(source,"plain-storage.m",{"plain_value"});
      assert(plain.sidecar_json.find(kNSObjectStorageSemantic)==std::string::npos);
      std::vector<std::string> names={"NativeObject","NativeAlias","native_value","native_consume","native_create"};
      if(cxx){names.push_back("body_only");names.push_back("native_identity");}
      for(const auto&name:names) {
        const std::vector<std::string> exports=name=="NativeObject" || name=="body_only"
            ? std::vector<std::string>{name} : std::vector<std::string>{"NativeObject",name};
        auto first=emit(source,"nsobject-provider.m",exports);
        assert(first.sidecar_json.find(kNSObjectStorageSemantic)!=std::string::npos);
        auto replayed=emit(first.source_bytes,"relocated/nsobject-consumer.m",exports);
        assert(first.identity.semantic_fingerprint==replayed.identity.semantic_fingerprint);
        auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
        std::vector<llvm::StringRef> supported;
        for(const auto&item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
        auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
        auto older=supported;std::erase(older,llvm::StringRef(kNSObjectStorageSemantic));
        auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
        assert(!rejected);llvm::consumeError(rejected.takeError());
        auto missing=*decoded;std::erase(missing.required_semantics,kNSObjectStorageSemantic);
        auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
        llvm::consumeError(invalid.takeError());
        auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
        invented->required_semantics.push_back(kNSObjectStorageSemantic);
        auto fabricated=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!fabricated);
        llvm::consumeError(fabricated.takeError());
      }
      auto first=emit(source,"integrated-storage.m",{"NativeObject"});
      auto changed=source;const std::string attribute=" __attribute__((NSObject))";
      const auto at=changed.find(attribute);assert(at!=std::string::npos);changed.erase(at,attribute.size());
      auto erased=emit(changed,"erased-storage.m",{"NativeObject"});
      assert(first.identity.semantic_fingerprint!=erased.identity.semantic_fingerprint);
    }
  }
}

void TestBorrowedMemoryAliasReplay() {
  for (const char *standard : {"c17", "c++20", "c++23"}) {
    const bool cxx = std::string(standard) != "c17";
    for (bool arc : {false, true}) {
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if (arc) args.push_back("-fobjc-arc");
      auto parse=[&](const std::string &source,const char *file) {
        return clang::tooling::buildASTFromCodeWithArgs(source,args,file);
      };
      auto emit=[&](const std::string &source,const char *file) {
        auto ast=parse(source,file);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                     "fixture.borrowed_memory_aliases","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.borrowed_memory_aliases","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      const std::string source=R"(
        typedef borrowed const char *MemoryLoan;
        MemoryLoan published_memory_bytes(void *owner) __attribute__((objc_returns_borrowed(owner_index=0)));
        MemoryLoan published_memory_object_bytes(id owner) __attribute__((objc_returns_borrowed(owner_index=0)));
        void published_memory_use(const char *pointer __attribute__((noescape)));
        struct PublishedMemoryHolder {const char *pointer;};
        struct PublishedMemoryPair {struct PublishedMemoryHolder first,second;};
        struct PublishedMemoryOwner {void *pointer;};
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_stored_owner(void *owner) {
          struct PublishedMemoryOwner box={owner};
          return published_memory_bytes(box.pointer);
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_owner_alias(void *owner,int choose) {
          struct PublishedMemoryOwner first={owner},second={owner};
          const struct PublishedMemoryOwner *alias=choose?&first:&second;
          return published_memory_bytes(alias->pointer);
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_copy(void *owner,int choose) {
          struct PublishedMemoryHolder one={published_memory_bytes(owner)},two={0},target={0};
          __builtin_memcpy(&target,choose?&one:&two,sizeof target);
          return target.pointer;
        }
        inline const char *published_memory_clear(void *owner) {
          struct PublishedMemoryHolder target={published_memory_bytes(owner)};
          struct PublishedMemoryHolder *alias=&target;
          __builtin_memset(alias,0,sizeof target);
          return target.pointer;
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_byte_copy(void *owner) {
          struct PublishedMemoryPair source={{0},{published_memory_bytes(owner)}},target={{0},{0}};
          char *to=(char *)&target;const char *from=(const char *)&source;
          to+=sizeof target.first;from+=sizeof source.first;
          __builtin_memcpy(to,from,sizeof target.second);
          return target.second.pointer;
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_cursor(void *owner) {
          const char *array[2]={0,0};const char **cursor=array;
          ++cursor;*cursor=published_memory_bytes(owner);
          return array[1];
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_walk(void *owner) {
          const char *array[3]={0,0,0};const char **cursor=array,**end=array+3;
          while(cursor!=end)*cursor++=published_memory_bytes(owner);
          return array[2];
        }
        inline const char *published_memory_postfix_clear(void *owner) {
          const char *array[2]={published_memory_bytes(owner),0};const char **cursor=array;
          *cursor++=0;return array[0];
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_byte_typed_store(void *owner) {
          struct PublishedMemoryPair target={{0},{0}};
          char *bytes=(char *)&target;bytes+=sizeof target.first;
          *(const char **)bytes=published_memory_bytes(owner);
          return target.second.pointer;
        }
        #ifdef __cplusplus
        inline void published_memory_field_closure(void *owner) {
          PublishedMemoryOwner box{owner};
          auto work=[box]{void *alias=box.pointer;published_memory_use(published_memory_bytes(alias));};
          work();
        }
        #if __has_feature(objc_arc)
        inline void published_memory_owned_record(id owner) {
          struct OwnedMemoryRecord {id owner;};OwnedMemoryRecord record{owner};
          const char *loan=published_memory_object_bytes(owner);
          auto work=[record,loan]{published_memory_use(published_memory_object_bytes(record.owner));published_memory_use(loan);};
          owner=0;record.owner=0;work();
        }
        #endif
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_joined_reference(void *owner,int choose) {
          const char *one=0,*two=0;
          const char *&reference=choose?one:two;
          reference=published_memory_bytes(owner);
          return choose?one:two;
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_reference_read(void *owner,int choose) {
          const char *one=0,*two=0;
          const char *&reference=choose?one:two;
          one=published_memory_bytes(owner);
          return reference;
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline MemoryLoan published_memory_reference_cell(void *owner) {
          const char *pointer=0;
          struct LocalReference {const char *&pointer;};
          LocalReference reference{pointer};
          reference.pointer=published_memory_bytes(owner);
          return pointer;
        }
        inline int published_memory_local_reference(void *owner,int count) {
          const char *pointer=published_memory_bytes(owner);int observed=0;
          auto work=[&pointer,&observed](auto offset){
            ++observed;published_memory_use(pointer+offset);return observed;
          };
          auto copy=work;auto &alias=copy;const auto &readonly=alias;
          for(int index=0;index<count;++index)alias(index);
          return readonly.template operator()<long>(0L);
        }
        inline void published_memory_generic_immediate(void *owner,int count) {
          const char *pointer=published_memory_bytes(owner);
          [&pointer](auto amount){
            const char *cursor=pointer;
            for(int index=0;index<amount;++index)published_memory_use(cursor++);
          }(count);
          pointer=published_memory_bytes(owner);
          published_memory_use(pointer);
        }
        #if __has_feature(objc_arc)
        inline void published_memory_mutable_record(id owner) {
          struct OwnedMutableRecord {id owner;};OwnedMutableRecord record{owner};
          const char *loan=published_memory_object_bytes(owner);
          auto work=[record,loan,count=0]()mutable{
            ++count;published_memory_use(published_memory_object_bytes(record.owner));published_memory_use(loan);
          };
          owner=0;record.owner=0;work();work();
        }
        #endif
        inline void published_memory_immediate_reference(void *owner) {
          const char *pointer=published_memory_bytes(owner);
          [&pointer]{published_memory_use(pointer);}();
          pointer=published_memory_bytes(owner);
          published_memory_use(pointer);
        }
        #endif
        inline const char *published_memory_byte_clear(void *owner) {
          struct PublishedMemoryPair target={{0},{published_memory_bytes(owner)}};
          char *bytes=(char *)&target;
          bytes+=__builtin_offsetof(struct PublishedMemoryPair,second);
          __builtin_memset(bytes,0,sizeof target.second);
          return target.second.pointer;
        }
        inline void published_memory_field_pin(id owner) {
          struct PublishedPinRecord {id owner;};struct PublishedPinRecord record={owner};
          withLifetime(record.owner) {
            const char *loan=published_memory_object_bytes(record.owner);
            owner=0;record.owner=0;published_memory_use(loan);
          }
        }
      )";
      auto first=emit(source,"memory-alias-provider.m");
      auto replay=emit(first.source_bytes,"memory-alias-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-memory-alias-provider.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto changed=source;auto at=changed.find("__builtin_memcpy");assert(at!=std::string::npos);
      changed.replace(at,std::string("__builtin_memcpy").size(),"__builtin_memmove");
      assert(emit(changed,"changed-memory-alias-provider.m").identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
      auto valid=parse(first.source_bytes+R"(
        __attribute__((objc_returns_borrowed(owner_index=0)))
        MemoryLoan published_memory_forward(void *owner,int choose) {
          published_memory_use(published_memory_copy(owner,choose));
          published_memory_use(published_memory_clear(owner));
          published_memory_use(published_memory_byte_copy(owner));
          published_memory_use(published_memory_byte_clear(owner));
          published_memory_use(published_memory_cursor(owner));
          published_memory_use(published_memory_walk(owner));
          published_memory_use(published_memory_byte_typed_store(owner));
          published_memory_use(published_memory_stored_owner(owner));
          published_memory_use(published_memory_owner_alias(owner,choose));
          published_memory_field_pin((id)0);
          #ifdef __cplusplus
          published_memory_field_closure(owner);
          #if __has_feature(objc_arc)
          published_memory_owned_record((id)0);
          #endif
          published_memory_immediate_reference(owner);
          published_memory_generic_immediate(owner,2);
          (void)published_memory_local_reference(owner,2);
          #if __has_feature(objc_arc)
          published_memory_mutable_record((id)0);
          #endif
          published_memory_use(published_memory_reference_cell(owner));
          published_memory_use(published_memory_joined_reference(owner,choose));
          published_memory_use(published_memory_reference_read(owner,choose));
          #endif
          published_memory_use(published_memory_postfix_clear(owner));
          return published_memory_copy(owner,choose);
        }
      )","memory-alias-consumer.m");
      assert(valid && !valid->getDiagnostics().hasErrorOccurred());
      auto invalid=parse(first.source_bytes+R"(
        const char *published_memory_escape(void *owner) {return published_memory_copy(owner,1);}
      )","memory-alias-escaping-consumer.m");
      assert(invalid && invalid->getDiagnostics().hasErrorOccurred());
      auto partial=parse(first.source_bytes+R"(
        const char *published_partial_escape(void *owner) {
          const char *loan=published_memory_bytes(owner);
          ((unsigned char *)&loan)[0]=0;return loan;
        }
      )","memory-partial-write-consumer.m");
      assert(partial && partial->getDiagnostics().hasErrorOccurred());
      auto cursor=parse(first.source_bytes+R"(
        const char *published_cursor_escape(void *owner) {return published_memory_cursor(owner);}
      )","memory-cursor-escaping-consumer.m");
      assert(cursor && cursor->getDiagnostics().hasErrorOccurred());
      auto bytes=parse(first.source_bytes+R"(
        const char *published_byte_cursor_escape(void *owner) {return published_memory_byte_typed_store(owner);}
      )","memory-byte-cursor-escaping-consumer.m");
      assert(bytes && bytes->getDiagnostics().hasErrorOccurred());
      auto owner_alias=parse(first.source_bytes+R"(
        const char *published_bad_owner_alias(void *owner) {
          return published_memory_owner_alias(owner,1);
        }
      )","memory-owner-alias-escaping-consumer.m");
      assert(owner_alias && owner_alias->getDiagnostics().hasErrorOccurred());
      if(cxx) {
        auto reference=parse(first.source_bytes+R"(
          void published_bad_reference(void *owner,const char *&output) {
            output=published_memory_bytes(owner);
          }
        )","memory-caller-reference-consumer.mm");
        assert(reference && reference->getDiagnostics().hasErrorOccurred());
        auto joined=parse(first.source_bytes+R"(
          const char *published_bad_reference_join(void *owner,int choose) {
            return published_memory_joined_reference(owner,choose);
          }
          void published_memory_escape_pointer(const char *pointer);
          void published_bad_future_reference(void *owner,int choose) {
            const char *one=0,*two=0;
            const char *&reference=choose?one:two;
            two=published_memory_bytes(owner);
            published_memory_escape_pointer(reference);
          }
        )","memory-joined-reference-consumer.mm");
        assert(joined && joined->getDiagnostics().hasErrorOccurred());
        auto fields=parse(first.source_bytes+R"(
          void published_memory_consume(void *owner __attribute__((cf_consumed)));
          void published_bad_captured_owner_field(void *owner) {
            PublishedMemoryOwner box{owner};
            auto work=[box]{void *alias=box.pointer;published_memory_use(published_memory_bytes(alias));};
            published_memory_consume(owner);work();
          }
        )","memory-captured-owner-field-consumer.mm");
        assert(fields && fields->getDiagnostics().hasErrorOccurred());
      }
    }
  }
}

void TestBorrowedStaticStorageReplay() {
  for(const char *standard:{"c17","c++20","c++23"}) {
    const bool cxx=std::string(standard)!="c17";
    for(bool arc:{false,true}) {
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      auto parse=[&](const std::string &source,const char *file) {
        return clang::tooling::buildASTFromCodeWithArgs(source,args,file);
      };
      auto emit=[&](const std::string &source,const char *file) {
        auto ast=parse(source,file);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                     "fixture.borrowed_static_storage","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.borrowed_static_storage","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      std::string source=R"(
        typedef borrowed const char *StaticLoan;
        extern const char static_program_data[8];
        const char static_program_data[8]="program";
        void static_consume(const char *value __attribute__((noescape)));
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline StaticLoan static_program_view(void *owner,int offset) {
          return static_program_data+offset;
        }
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline StaticLoan static_program_choice(void *owner,int choose) {
          return choose?static_program_data:0;
        }
      )";
      source+=R"(
        union StaticPublishedUnion {char bytes[8];long other;};
        extern const union StaticPublishedUnion static_union_data;
        const union StaticPublishedUnion static_union_data={{'u','n','i','o','n',0}};
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline StaticLoan static_union_view(void *owner){return static_union_data.bytes;}
      )";
      if(cxx)source+=R"(
        inline const char *static_inline_source(){return static_union_data.bytes;}
        inline const char *static_inline_offset(int offset){return static_inline_source()+offset;}
        inline const char *static_inline_choice(int choose){return choose?static_program_data:static_inline_source();}
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline StaticLoan static_inline_view(void *owner,int offset){return static_inline_offset(offset);}
        __attribute__((objc_returns_borrowed(owner_index=0)))
        inline StaticLoan static_inline_select(void *owner,int choose){return static_inline_choice(choose);}
      )";
      auto first=emit(source,"static-borrowed-results.m");
      const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      assert(llvm::is_contained(decoded->required_semantics,std::string(kBorrowedResultProofSemantic)));
      auto older=supported;std::erase(older,llvm::StringRef(kBorrowedResultProofSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
      std::erase(decoded->required_semantics,kBorrowedResultProofSemantic);
      auto missing=EncodeSemanticInterfaceSidecar(*decoded,supported);assert(!missing);llvm::consumeError(missing.takeError());

      auto declaration=emit("typedef borrowed const char *StaticLoan;__attribute__((objc_returns_borrowed(owner_index=0))) StaticLoan static_signature(void *owner);","static-borrowed-signature.m");
      auto invented=DecodeSemanticInterfaceSidecar(declaration.sidecar_json,declaration.source_bytes,supported);assert(invented);
      assert(!llvm::is_contained(invented->required_semantics,std::string(kBorrowedResultProofSemantic)));
      invented->required_semantics.push_back(kBorrowedResultProofSemantic);
      auto invalid_capability=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid_capability);llvm::consumeError(invalid_capability.takeError());
      auto replay=emit(first.source_bytes,"replayed-static-borrowed-results.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-static-borrowed-results.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto changed=source;auto offset=changed.find("static_program_data+offset");assert(offset!=std::string::npos);
      changed.replace(offset,std::string("static_program_data+offset").size(),"static_program_data+offset+1");
      assert(emit(changed,"changed-static-borrowed-results.m").identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
      auto valid=parse(first.source_bytes+R"(
        __attribute__((objc_returns_borrowed(owner_index=0)))
        StaticLoan static_forward(void *owner) {
          static_consume(static_union_view(owner));
          static_consume(static_program_view(owner,1));
          return static_program_choice(owner,1);
        }
      )","static-borrowed-consumer.m");
      assert(valid && !valid->getDiagnostics().hasErrorOccurred());
      auto invalid=parse(first.source_bytes+R"(
        const char *static_escape(void *owner) {return static_program_view(owner,1);}
      )","static-borrowed-escape.m");
      assert(invalid && invalid->getDiagnostics().hasErrorOccurred());
    }
  }
}

void TestBorrowedCallableOwnerCapability() {
  for (const char *standard : {"c17", "c++20", "c++23"}) {
    const bool cxx = std::string(standard) != "c17";
    for (bool arc : {false, true}) {
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if (arc) args.push_back("-fobjc-arc");
      const std::string source=R"(
        typedef borrowed const char *CallbackBytes;
        typedef CallbackBytes (*Getter)(void *) __attribute__((objc_returns_borrowed(owner_index=0)));
        typedef CallbackBytes (*GetterFirst)(void *,void *) __attribute__((objc_returns_borrowed(owner_index=0)));
        typedef CallbackBytes (*GetterSecond)(void *,void *) __attribute__((objc_returns_borrowed(owner_index=1)));
        CallbackBytes get_bytes(void *owner) __attribute__((objc_returns_borrowed(owner_index=0)));
        void plain(void *);
        #ifdef __cplusplus
        inline int body_only(void *owner) {
          using Local=CallbackBytes(__attribute__((objc_returns_borrowed(owner_index=0))) *)(void*);
          Local getter=nullptr;(void)getter;(void)owner;return 1;
        }
        template<class G> __attribute__((objc_returns_borrowed(owner_index=1)))
        CallbackBytes callback_forward(G getter,void *owner) {return getter(owner);}
        #endif
      )";
      auto emit=[&](llvm::StringRef text,llvm::StringRef file,const std::vector<std::string>&exports) {
        auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,file);
        assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        auto emitted=EmitSemanticInterface(ast->getASTContext(),"fixture.borrowed.callback","test",{},{},{},exports);
        for(const auto&d:emitted.diagnostics) std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());
        assert(emitted.success);return emitted;
      };
      auto plain=emit(source,"plain-callback.m",{"plain"});
      assert(plain.sidecar_json.find(kBorrowedCallableOwnerSemantic)==std::string::npos);
      std::vector<std::string> names={"Getter","GetterFirst","GetterSecond","get_bytes"};
      if(cxx) {names.push_back("body_only");names.push_back("callback_forward");}
      for(const auto&name:names) {
        const std::vector<std::string> exports={"CallbackBytes",name};
        auto emitted=emit(source,"callback-provider.m",exports);
        assert(emitted.sidecar_json.find(kBorrowedCallableOwnerSemantic)!=std::string::npos);
        auto parsed=llvm::json::parse(emitted.sidecar_json);assert(parsed&&parsed->getAsObject());
        const auto*required=parsed->getAsObject()->getArray("required_semantics");assert(required);
        std::vector<llvm::StringRef> supported;
        for(const auto&item:*required) {assert(item.getAsString());supported.push_back(*item.getAsString());}
        auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,supported);assert(decoded);
        auto replayed=emit(emitted.source_bytes,"relocated/callback-consumer.m",exports);
        assert(replayed.identity.semantic_fingerprint==emitted.identity.semantic_fingerprint);
        auto older=supported;std::erase(older,llvm::StringRef(kBorrowedCallableOwnerSemantic));
        auto rejected=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,older);
        assert(!rejected);llvm::consumeError(rejected.takeError());
        auto missing=*decoded;std::erase(missing.required_semantics,kBorrowedCallableOwnerSemantic);
        auto encoded=EncodeSemanticInterfaceSidecar(missing,supported);assert(!encoded);
        assert(llvm::StringRef(llvm::toString(encoded.takeError())).contains("borrowed callable owner identity"));
        auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
        invented->required_semantics.push_back(kBorrowedCallableOwnerSemantic);
        auto invalid=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid);
        llvm::consumeError(invalid.takeError());
      }
      auto first=emit(source,"first-owner.m",{"CallbackBytes","GetterFirst"});
      const auto position=source.find("(*GetterFirst)(void *,void *) __attribute__((objc_returns_borrowed(owner_index=0)))");
      assert(position!=std::string::npos);
      std::string changed=source;
      auto index=changed.find("owner_index=0",position);assert(index!=std::string::npos);
      changed.replace(index,std::string("owner_index=0").size(),"owner_index=1");
      auto second=emit(changed,"second-owner.m",{"CallbackBytes","GetterFirst"});
      assert(first.identity.semantic_fingerprint!=second.identity.semantic_fingerprint);
    }
  }
}

void TestAsyncConsumedOwnershipCapability() {
  for (const char *standard : {"c17", "c++20", "c++23"}) {
    for (bool arc : {false, true}) {
      std::vector<std::string> args = {"-x", std::string(standard) == "c17" ? "objective-c" : "objective-c++",
          std::string("-std=") + standard, "-fobjc-version=3", "-fobjc-runtime=gnustep-2.2",
          "--target=x86_64-unknown-linux-gnu", "-fblocks"};
      if (arc) args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;
      AddRuntimeInterfaceFiles(files, args);
      const std::string source = R"(
        #include "objc3c/runtime/objc3_runtime_services.h"
        typedef const void *CFRef;
        CFRef body_source(void) __attribute__((nothrow,cf_returns_retained));
        int sync_entry(__attribute__((cf_consumed)) CFRef) __attribute__((nothrow,objc_executor(main)));
        int consume_signature(__attribute__((cf_consumed)) CFRef) __objc3_async_frame;
        static inline int consume_body(void) __objc3_async_frame {
          return __objc3_await sync_entry(body_source());
        }
        static int unpublished_body(void) __objc3_async_frame {
          return __objc3_await sync_entry(body_source());
        }
        __attribute__((objc_root_class)) @interface OwnershipInitializer
        - (instancetype)initFrame __objc3_async_frame;
        @end
        id retained_signature(void) __objc3_async_frame __attribute__((ns_returns_retained));
        static inline id retained_body(void) __objc3_async_frame {return __objc3_await retained_signature();}
        CFRef retained_foreign_signature(void) __objc3_async_frame __attribute__((cf_returns_retained));
        CFRef borrowed_foreign_signature(void) __objc3_async_frame __attribute__((cf_returns_not_retained));
        static inline CFRef retained_foreign_body(void) __objc3_async_frame {
          return __objc3_await retained_foreign_signature();
        }
        int plain(void);
        #ifdef __cplusplus
        #include "objc3c/runtime/objc3_task.hpp"
        extern "C" void CFRelease(const void *) __attribute__((nothrow));
        __attribute__((objc_root_class)) @interface TaskOwnershipInitializer
        - (instancetype)initTask __objc3_async;
        @end
        id retained_task_signature(void) __objc3_async __attribute__((ns_returns_retained));
        CFRef retained_foreign_task_signature(void) __objc3_async __attribute__((cf_returns_retained));
        CFRef borrowed_foreign_task_signature(void) __objc3_async __attribute__((cf_returns_not_retained));
        static inline id retained_task_body(void) __objc3_async {return __objc3_await retained_task_signature();}
        static inline CFRef retained_foreign_task_body(void) __objc3_async {return __objc3_await retained_foreign_task_signature();}
        static inline objc3::task<int> ordinary_body(__attribute__((cf_consumed)) CFRef value) {
          if (value) CFRelease(value);
          co_return 0;
        }
        static inline auto ordinary_block_factory(void) {
          return ^objc3::task<int>(__attribute__((cf_consumed)) CFRef value) {
            if (value) CFRelease(value);
            co_return 0;
          };
        }
        #endif
      )";
      auto emit = [&](llvm::StringRef text, llvm::StringRef file, const std::vector<std::string> &exports) {
        auto ast = clang::tooling::buildASTFromCodeWithArgs(text, args, file, "clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(), files);
        assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        auto result = EmitSemanticInterface(ast->getASTContext(), "fixture.async.ownership", "test", {}, {}, {}, exports);
        for (const auto &diagnostic : result.diagnostics)
          std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(), diagnostic.message.c_str());
        assert(result.success);
        return result;
      };
      const std::vector<std::string> base = {"CFRef", "body_source", "sync_entry", "plain"};
      auto plain = emit(source, "ownership-private.m", base);
      assert(plain.sidecar_json.find(kAsyncConsumedOwnershipSemantic) == std::string::npos);
      std::vector<std::string> consuming_exports = {"consume_signature", "consume_body", "retained_signature", "retained_body", "OwnershipInitializer", "retained_foreign_signature", "borrowed_foreign_signature", "retained_foreign_body"};
      if (std::string(standard) != "c17") {
        consuming_exports.push_back("ordinary_body");
        consuming_exports.push_back("ordinary_block_factory");
        for (const char *name : {"TaskOwnershipInitializer", "retained_task_signature", "retained_foreign_task_signature",
                                 "borrowed_foreign_task_signature", "retained_task_body", "retained_foreign_task_body"})
          consuming_exports.push_back(name);
      }
      for (const std::string &exported : consuming_exports) {
        auto exports = base; exports.push_back(exported);
        if (exported == "ordinary_body" || exported == "ordinary_block_factory") exports.push_back("CFRelease");
        if (exported == "retained_task_body") exports.push_back("retained_task_signature");
        if (exported == "retained_foreign_task_body") exports.push_back("retained_foreign_task_signature");
        if (exported == "retained_body") exports.push_back("retained_signature");
        if (exported == "retained_foreign_body") exports.push_back("retained_foreign_signature");
        auto result = emit(source, "ownership-provider.m", exports);
        assert(result.sidecar_json.find(kAsyncConsumedOwnershipSemantic) != std::string::npos);
        auto parsed = llvm::json::parse(result.sidecar_json);
        assert(parsed && parsed->getAsObject());
        const auto *required = parsed->getAsObject()->getArray("required_semantics"); assert(required);
        std::vector<llvm::StringRef> supported;
        for (const auto &item : *required) {assert(item.getAsString()); supported.push_back(*item.getAsString());}
        auto decoded = DecodeSemanticInterfaceSidecar(result.sidecar_json, result.source_bytes, supported);
        assert(decoded);
        auto replayed = emit(result.source_bytes, "relocated/ownership-consumer.m", exports);
        assert(replayed.identity.semantic_fingerprint == result.identity.semantic_fingerprint);
        auto old = supported; std::erase(old, llvm::StringRef(kAsyncConsumedOwnershipSemantic));
        old.push_back("objc3.async-consuming-ownership.v1");
        old.push_back("objc3.async-consuming-ownership.v2");
        auto unsupported = DecodeSemanticInterfaceSidecar(result.sidecar_json, result.source_bytes, old);
        assert(!unsupported); llvm::consumeError(unsupported.takeError());
        auto missing = *decoded; std::erase(missing.required_semantics, kAsyncConsumedOwnershipSemantic);
        auto rejected = EncodeSemanticInterfaceSidecar(missing, supported);
        assert(!rejected);
        const auto missing_error = llvm::toString(rejected.takeError());
        assert(llvm::StringRef(missing_error).contains("async consuming ownership identity"));
        auto control = DecodeSemanticInterfaceSidecar(plain.sidecar_json, plain.source_bytes, supported);
        assert(control); control->required_semantics.push_back(kAsyncConsumedOwnershipSemantic);
        auto invented = EncodeSemanticInterfaceSidecar(*control, supported);
        assert(!invented);
        const auto invented_error = llvm::toString(invented.takeError());
        assert(llvm::StringRef(invented_error).contains("async consuming ownership identity"));
      }
    }
  }
}

void TestKeyPathSubscriptBodies() {
  for (const char *standard : {"c++20", "c++23"}) {
    for (bool arc : {false, true}) {
      std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
          "-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "--target=x86_64-unknown-linux-gnu"};
      if (arc) args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;
      AddRuntimeInterfaceFiles(files,args);
      args.push_back("-include");args.push_back("objc3c/runtime/objc3_keypath.h");
      auto parse=[&](llvm::StringRef source,llvm::StringRef file) {
        return clang::tooling::buildASTFromCodeWithArgs(source,args,file,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto ast=parse(R"(
        __attribute__((objc_root_class)) @interface IndexedRoot
        @property(nonatomic) int count;
        @property(nonatomic, strong) IndexedRoot * _Nullable next;
        @property(nonatomic, strong) IndexedRoot * _Nullable value;
        @end
        inline int indexed_read(IndexedRoot *_Nonnull root,KeyPath<IndexedRoot*,int> path) {
          return root[path];
        }
        inline int indexed_write(IndexedRoot *_Nonnull root,WritableKeyPath<IndexedRoot*,int> path,int value) {
          int assigned=(root[path]=value);int updated=(root[path]+=2);
          return assigned+updated+root[path]++ + ++root[path];
        }
        inline int indexed_literal(IndexedRoot *_Nonnull root) {
          return root[@keypath(IndexedRoot,count)];
        }
        inline IndexedRoot * _Nullable indexed_nullable(IndexedRoot *_Nonnull root, IndexedRoot * _Nullable value) {
          return (root[@keypath(IndexedRoot,next.value)] = value);
        }
        inline int checked_literal(IndexedRoot *_Nonnull root) {
          return KeyPathGet(@keypath(IndexedRoot,next.count),root);
        }
        inline int explicit_literal(IndexedRoot *_Nonnull root) {
          return KeyPathGet(@keypath(IndexedRoot,count),root);
        }
      )","indexed-provider.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      const std::vector<std::string> exports={"IndexedRoot","indexed_read","indexed_write",
                                             "indexed_literal","explicit_literal","indexed_nullable","checked_literal"};
      const auto emitted=EmitSemanticInterface(ast->getASTContext(),"fixture.indexed","test",{},{},{},exports);
      for (const auto &d:emitted.diagnostics)
        std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(emitted.success);
      assert(emitted.source_bytes.find("root[path]")!=std::string::npos);
      assert(emitted.source_bytes.find("__objc3_keypath_")==std::string::npos);
      assert(emitted.sidecar_json.find("objc3.keypath-subscripts.v1")!=std::string::npos);
      assert(emitted.sidecar_json.find("objc3.keypath-nullable-writes.v1")!=std::string::npos);
      assert(emitted.sidecar_json.find("objc3.keypath-checked-reads.v1")!=std::string::npos);
      const std::vector<llvm::StringRef> supported={"objc3.typed-keypaths.v1","objc3.keypath-checked-reads.v1",
          "objc3.keypath-subscripts.v1","objc3.keypath-nullable-writes.v1","objc3.canonical-function-effects.v1","objc3.cxx-data-interfaces.v1",
          "objc3.cxx-definitions.v1","objc3.native-ownership-contracts.v1"};
      auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,supported);
      if (!decoded) { llvm::errs()<<llvm::toString(decoded.takeError())<<"\n"; assert(false); }
      auto relocated=parse(emitted.source_bytes,"indexed-consumer.mm");
      assert(relocated && !relocated->getDiagnostics().hasErrorOccurred());
      auto repeated=EmitSemanticInterface(relocated->getASTContext(),"fixture.indexed","test",{},{},{},exports);
      assert(repeated.success && repeated.identity.semantic_fingerprint==emitted.identity.semantic_fingerprint);
      const std::vector<llvm::StringRef> old={"objc3.typed-keypaths.v1","objc3.canonical-function-effects.v1","objc3.cxx-data-interfaces.v1",
          "objc3.cxx-definitions.v1","objc3.native-ownership-contracts.v1"};
      auto rejected=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,old);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto unsupported_writes = supported;
      std::erase(unsupported_writes, llvm::StringRef("objc3.keypath-nullable-writes.v1"));
      auto old_writer=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,unsupported_writes);
      assert(!old_writer);llvm::consumeError(old_writer.takeError());
      auto unsupported_reads = supported;
      std::erase(unsupported_reads, llvm::StringRef("objc3.keypath-checked-reads.v1"));
      auto old_reader=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,unsupported_reads);
      assert(!old_reader);llvm::consumeError(old_reader.takeError());
      auto missing_read=*decoded;
      std::erase(missing_read.required_semantics,"objc3.keypath-checked-reads.v1");
      auto missing_read_cap=EncodeSemanticInterfaceSidecar(missing_read,supported);
      assert(!missing_read_cap);llvm::consumeError(missing_read_cap.takeError());
      auto missing_write=*decoded;
      std::erase(missing_write.required_semantics,"objc3.keypath-nullable-writes.v1");
      auto missing_write_cap=EncodeSemanticInterfaceSidecar(missing_write,supported);
      assert(!missing_write_cap);llvm::consumeError(missing_write_cap.takeError());
      auto damaged=*decoded;
      std::erase(damaged.required_semantics,"objc3.typed-keypaths.v1");
      auto missing=EncodeSemanticInterfaceSidecar(damaged,supported);
      assert(!missing);llvm::consumeError(missing.takeError());
      auto hidden=parse(R"(
        __attribute__((objc_root_class)) @interface IndexedRoot @end
        WritableKeyPath<IndexedRoot*,int> hidden_path();
        inline int indexed_read(IndexedRoot *_Nonnull root) {return root[hidden_path()];}
      )","indexed-hidden.mm");
      assert(hidden && !hidden->getDiagnostics().hasErrorOccurred());
      auto excluded=EmitSemanticInterface(hidden->getASTContext(),"fixture.indexed","test",
          {},{},{},{"IndexedRoot","indexed_read"});
      assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),
          [](const auto &d){return d.code=="O3IFC156";}));
    }
  }
}

void TestConsumedAndMethodOwnershipRoundTrips() {
  for (const char *language : {"objective-c", "objective-c++"}) {
    for (bool arc : {false, true}) {
      std::vector<std::string> args = {
          "-x",
          language,
          std::string(language) == "objective-c" ? "-std=c17" : "-std=c++20",
          "-fobjc-version=3",
          "-fobjc-runtime=gnustep-2.2",
          "-fblocks",
          "--target=x86_64-unknown-linux-gnu"};
      if (arc)
        args.push_back("-fobjc-arc");
      auto ast = clang::tooling::buildASTFromCodeWithArgs(
          R"(
        void consume(__attribute__((ns_consumed)) id value);
        void observe(__attribute__((noescape)) void (^body)(void));
        __attribute__((objc_root_class)) @interface OwnershipRoot
        - (void)consume:(__attribute__((ns_consumed)) id)value;
        - (void)observe:(__attribute__((noescape)) void (^)(void))body;
        - (id)retainedValue __attribute__((ns_returns_retained));
        - (id)borrowedValue __attribute__((ns_returns_not_retained));
        - (void)consumeSelf __attribute__((ns_consumes_self));
        @end
      )",
          args, "ownership-contracts.m");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      auto emitted = EmitSemanticInterface(ast->getASTContext(),
                                           "fixture.consume", "test");
      assert(emitted.success);
      for (const char *attribute :
           {"ns_consumed", "noescape", "ns_consumes_self",
            "ns_returns_retained", "ns_returns_not_retained"})
        assert(emitted.source_bytes.find(attribute) != std::string::npos);
      const std::vector<llvm::StringRef> supported = {
          "objc3.canonical-function-effects.v1",
          "objc3.native-return-ownership.v1",
          "objc3.native-ownership-contracts.v1", kNativeDeclarationAttributeSemantic,
          "objc3.cxx-data-interfaces.v1"};
      auto decoded = DecodeSemanticInterfaceSidecar(
          emitted.sidecar_json, emitted.source_bytes, supported);
      assert(decoded);
      bool parameter = false, receiver = false, method_return = false;
      for (const auto &declaration : decoded->declarations) {
        parameter |= declaration.effects.ownership_identity.find(
                         "ns-consumed") != std::string::npos;
        receiver |= declaration.effects.ownership_identity.find(
                        "ns-consumes-self") != std::string::npos;
        method_return |= declaration.kind == "ObjCMethod" &&
                         declaration.effects.ownership_identity.starts_with(
                             "native-return:retained");
      }
      assert(parameter && receiver && method_return);
      auto reparsed = clang::tooling::buildASTFromCodeWithArgs(
          emitted.source_bytes, args, "ownership-import.m");
      assert(reparsed && !reparsed->getDiagnostics().hasErrorOccurred());
      auto repeated = EmitSemanticInterface(reparsed->getASTContext(),
                                            "fixture.consume", "test");
      assert(repeated.success && repeated.identity.semantic_fingerprint ==
                                     emitted.identity.semantic_fingerprint);
    }
  }
  // Hidden arguments are now a modeled native parameter contract in C as well.
  auto sized=clang::tooling::buildASTFromCodeWithArgs(
      "int size(const char *const p __attribute__((pass_object_size(0))));"
      "int nonnegative(int value) __attribute__((enable_if(value>=0,\"nonnegative C entry\")));",
      {"-x","c","-std=c17"},"sized-param.c");
  assert(sized && !sized->getDiagnostics().hasErrorOccurred());
  auto accepted=EmitSemanticInterface(sized->getASTContext(),"fixture.size","test");
  assert(accepted.success && accepted.source_bytes.find("pass_object_size")!=std::string::npos);
  auto replayed=clang::tooling::buildASTFromCodeWithArgs(accepted.source_bytes,{"-x","c","-std=c17"},"replayed-sized-param.c");
  assert(replayed && !replayed->getDiagnostics().hasErrorOccurred());
  auto repeated=EmitSemanticInterface(replayed->getASTContext(),"fixture.size","test");
  assert(repeated.success && repeated.identity.semantic_fingerprint==accepted.identity.semantic_fingerprint);
}

void TestCOverloadableInterfaces() {
  const std::string source = R"(
    int choose(int value) __attribute__((overloadable));
    int choose(double value) __attribute__((overloadable));
    int bounded(int value) __attribute__((overloadable, enable_if(value>=0,"nonnegative")));
    int bounded(int value) __attribute__((overloadable, enable_if(value<0,"negative")));
    int redeclared(int value) __attribute__((overloadable));
    int redeclared(int value) __attribute__((overloadable));
  )";
  for (bool objc : {false, true}) {
    std::vector<std::string> flags = {"-x", objc ? "objective-c" : "c", "-std=c17",
                                    "--target=x86_64-unknown-linux-gnu"};
    if (objc) {
      flags.push_back("-fobjc-version=3");
      flags.push_back("-fobjc-runtime=gnustep-2.2");
    }
    auto emit = [&](const std::string &text, const char *file) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(text, flags, file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),
          ast->getASTContext().getTranslationUnitDecl(), "fixture.overloadable", "test", {}));
      auto result = EmitSemanticInterface(ast->getASTContext(), "fixture.overloadable", "test");
      for (const auto &diagnostic : result.diagnostics)
        std::fprintf(stderr, "C OVERLOAD %s: %s (%s)\n", diagnostic.code.c_str(),
                     diagnostic.message.c_str(), diagnostic.subject.c_str());
      assert(result.success);
      return result;
    };
    const auto first = emit(source, "overloadable.c");
    assert(first.source_bytes.find("overloadable") != std::string::npos);
    assert(first.source_bytes.find("enable_if") != std::string::npos);
    auto replay = emit(first.source_bytes, "replayed-overloadable.c");
    assert(first.identity.semantic_fingerprint == replay.identity.semantic_fingerprint);
    assert(first.source_bytes == replay.source_bytes);
    auto relocated = emit("\n\n" + source, "relocated-overloadable.c");
    assert(first.identity.semantic_fingerprint == relocated.identity.semantic_fingerprint);
    auto renamed = source;
    for (std::size_t position = 0; (position = renamed.find("value", position)) != std::string::npos;) {
      renamed.replace(position, 5, "argument");
      position += 8;
    }
    assert(first.identity.semantic_fingerprint ==
           emit(renamed, "renamed-overloadable.c").identity.semantic_fingerprint);
    auto changed = source;
    const auto predicate = changed.find("value>=0");
    assert(predicate != std::string::npos);
    changed.replace(predicate, 8, "value>0");
    assert(first.identity.semantic_fingerprint !=
           emit(changed, "changed-overloadable.c").identity.semantic_fingerprint);
    auto metadata = llvm::json::parse(first.sidecar_json);
    assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for (const auto &value : *metadata->getAsObject()->getArray("required_semantics"))
      semantics.push_back(*value.getAsString());
    auto decoded = DecodeSemanticInterfaceSidecar(first.sidecar_json, first.source_bytes, semantics);
    assert(decoded);
    std::set<std::string> identities, symbols;
    unsigned functions = 0;
    for (const auto &declaration : decoded->declarations) {
      if (declaration.kind != "Function") continue;
      ++functions;
      assert(HasNativeDeclarationAttributeIdentity(declaration));
      assert(identities.insert(declaration.stable_id).second);
      assert(!declaration.native_symbol_name.empty());
      assert(symbols.insert(declaration.native_symbol_name).second);
    }
    assert(functions == 5);
  }
}

void TestUnmodeledAttributesFailClosed() {
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      "__attribute__((no_profile_instrument_function)) void *old_api(void);\n",
      {"-x", "c", "-std=c11", "--target=x86_64-unknown-linux-gnu"},
      "attributed_fixture.c");
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());

  auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.attrs",
                                       "test-package");
  assert(!emitted.success);
  assert(emitted.source_bytes.empty());
  assert(emitted.sidecar_json.empty());
  bool found_attribute_diagnostic = false;
  for (const auto &diagnostic : emitted.diagnostics)
    found_attribute_diagnostic |= diagnostic.code == "O3IFC116";
  assert(found_attribute_diagnostic);
}

void TestCAsyncPhysicalABIRoundTrips() {
  for (const char *source :
       {"int fetch(int) __objc3_async;",
        "typedef int (*AsyncCallback)(int) __objc3_async;",
        "struct Callbacks { int (*fetch)(int) __objc3_async; };",
        "void accepts(int (*callback)(int) __objc3_async);",
        "@interface Worker - (int)fetch:(int)n __objc3_async; @end",
        "typedef int (^AsyncBlock)(int) __objc3_async;",
        "typedef int (^AsyncBlock)(int) __objc3_async; "
        "@interface Worker @property(copy) AsyncBlock callback; @end"}) {
    auto ast = clang::tooling::buildASTFromCodeWithArgs(
        source,
        {"-x", "objective-c", "-std=c17", "-fobjc-version=3", "-fblocks",
         "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"},
        "async_frame_interface.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted = EmitSemanticInterface(ast->getASTContext(),
                                         "fixture.async_frame", "test-package");
    assert(emitted.success && !emitted.source_bytes.empty());
    assert(emitted.source_bytes.find("__objc3_async_frame") !=
           std::string::npos);
    const std::vector<llvm::StringRef> supported = {
        "objc3.canonical-function-effects.v1", kAsyncEntryABISemantic,
        RuntimeInterfaceSemantic()};
    auto decoded = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, supported);
    assert(decoded);
    bool found = false;
    for (const auto &declaration : decoded->declarations)
      if (!declaration.effects.async_abi_identity.empty()) {
        assert(IsValidAsyncABIIdentity(declaration.effects));
        assert(declaration.effects.async_abi_identity.find("c-frame-v1") !=
               std::string::npos);
        found = true;
      }
    assert(found);
    const std::vector<llvm::StringRef> old_supported = {
        "objc3.canonical-function-effects.v1", "objc3.async-entry-abi.v1",
        "objc3.async-entry-abi.v2", "objc3.async-entry-abi.v3", RuntimeInterfaceSemantic()};
    auto old_reader = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, old_supported);
    assert(!old_reader);
    llvm::consumeError(old_reader.takeError());
    auto stripped = *decoded;
    std::erase(stripped.required_semantics,
               std::string(kAsyncEntryABISemantic));
    auto invalid = EncodeSemanticInterfaceSidecar(stripped, supported);
    assert(!invalid);
    llvm::consumeError(invalid.takeError());
  }
}

void TestPublishedIsolationBodies() {
  std::vector<std::string> args={"-x", "objective-c++", "-std=c++20",
      "-fobjc-version=3", "-fobjc-runtime=gnustep-2.2",
      "--target=x86_64-unknown-linux-gnu"};
  clang::tooling::FileContentMappings files;
  AddRuntimeInterfaceFiles(files,args);
  const std::string source=R"(
#include <objc3c/runtime/objc3_runtime_services.h>
int isolated_entry(int value) noexcept __attribute__((objc_executor(main)));
inline int published_entry(int value) __objc3_async_frame {
  return __objc3_await isolated_entry(value);
}
)";
  auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,
      "published-isolation.mm","clang-tool",std::make_shared<clang::PCHContainerOperations>(),
      clang::tooling::getClangStripDependencyFileAdjuster(),files);
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());
  auto emitted=EmitSemanticInterface(ast->getASTContext(),"fixture.isolation.bodies","test");
  for (const auto &diagnostic:emitted.diagnostics)
    std::fprintf(stderr,"%s: %s\n",diagnostic.code.c_str(),
                 diagnostic.message.c_str());
  assert(emitted.success);
  assert(emitted.source_bytes.find("__objc3_isolation_entry_")==std::string::npos);
  assert(emitted.source_bytes.find("__objc3_await isolated_entry")!=std::string::npos);
  const std::vector<llvm::StringRef> supported={
      "objc3.cxx-data-interfaces.v1", "objc3.canonical-function-effects.v1",
      "objc3.isolation-contracts.v1", "objc3.async-entry-abi.v5",
      kCxxDefinitionSemantic,kCxxAsyncDefinitionSemantic,RuntimeInterfaceSemantic()};
  auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,
                                              emitted.source_bytes,supported);
  assert(decoded);
  std::erase(decoded->required_semantics,kCxxAsyncDefinitionSemantic);
  auto stripped=EncodeSemanticInterfaceSidecar(*decoded,supported);
  assert(!stripped);
  llvm::consumeError(stripped.takeError());
  auto round=clang::tooling::buildASTFromCodeWithArgs(emitted.source_bytes,args,
      "reconstructed-isolation.mm","clang-tool",std::make_shared<clang::PCHContainerOperations>(),
      clang::tooling::getClangStripDependencyFileAdjuster(),files);
  assert(round && !round->getDiagnostics().hasErrorOccurred());
  auto regenerated=EmitSemanticInterface(round->getASTContext(),"fixture.isolation.bodies","test");
  assert(regenerated.success && regenerated.identity.semantic_fingerprint==
         emitted.identity.semantic_fingerprint);
  auto private_dependency=EmitSemanticInterface(ast->getASTContext(),
      "fixture.isolation.bodies","test",{},{},{},{"published_entry"});
  assert(!private_dependency.success && std::any_of(
      private_dependency.diagnostics.begin(),private_dependency.diagnostics.end(),
      [](const auto &diagnostic) { return diagnostic.code=="O3IFC156"; }));
  std::string changed=source;
  changed.replace(changed.find("isolated_entry(value)"),
                  std::string("isolated_entry(value)").size(),
                  "isolated_entry(value + 1)");
  auto changed_ast=clang::tooling::buildASTFromCodeWithArgs(changed,args,
      "changed-isolation.mm","clang-tool",std::make_shared<clang::PCHContainerOperations>(),
      clang::tooling::getClangStripDependencyFileAdjuster(),files);
  assert(changed_ast && !changed_ast->getDiagnostics().hasErrorOccurred());
  auto changed_interface=EmitSemanticInterface(changed_ast->getASTContext(),
      "fixture.isolation.bodies","test");
  assert(changed_interface.success && changed_interface.identity.semantic_fingerprint!=
         emitted.identity.semantic_fingerprint);

  auto task_args=args;
  task_args.push_back("-fcoro-aligned-allocation");
  task_args.push_back(std::string("-resource-dir=")+OBJC3_INTERFACE_TEST_RESOURCE_DIR);
  clang::tooling::FileContentMappings task_files;
  AddRuntimeInterfaceFiles(task_files,task_args,true);
  for (const std::string task_source : {std::string(R"(
#include <objc3c/runtime/objc3_task.hpp>
inline int published_task(int value) __objc3_async { return value; }
)"),std::string(R"(
#include <objc3c/runtime/objc3_task.hpp>
int task_source_leaf(int) noexcept __objc3_async;
inline int frame_from_task(int value) __objc3_async_frame {
  return __objc3_await task_source_leaf(value);
}
)"),std::string(R"(
#include <objc3c/runtime/objc3_task.hpp>
int frame_source_leaf(int) __objc3_async_frame;
inline int task_from_frame(int value) __objc3_async {
  return __objc3_await frame_source_leaf(value);
}
)")})
  for (bool changed_runtime : {false,true}) {
    auto inputs=task_files;
    if (changed_runtime)
      for (auto &file : inputs)
        if (llvm::StringRef(file.first).ends_with("/objc3_task.hpp"))
          file.second+="\n// Changed runtime contract.\n";
    auto task_ast=clang::tooling::buildASTFromCodeWithArgs(task_source,task_args,
        "published-task.mm","clang-tool",std::make_shared<clang::PCHContainerOperations>(),
        clang::tooling::getClangStripDependencyFileAdjuster(),inputs);
    assert(task_ast && !task_ast->getDiagnostics().hasErrorOccurred());
    auto task_interface=EmitSemanticInterface(task_ast->getASTContext(),
        "fixture.task.body","test");
    if (task_interface.success==changed_runtime)
      for (const auto &d:task_interface.diagnostics)
        std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(task_interface.success!=changed_runtime);
    if (changed_runtime)
      assert(std::any_of(task_interface.diagnostics.begin(),task_interface.diagnostics.end(),
          [](const auto &diagnostic) { return diagnostic.code=="O3IFC112"; }));
  }
}

void TestPublishedRuntimeDeclarations() {
  std::vector<std::string> args={"-x","objective-c++","-std=c++20",
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
      "--target=x86_64-unknown-linux-gnu","-fcoro-aligned-allocation",
      "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
  clang::tooling::FileContentMappings files;
  AddRuntimeInterfaceFiles(files,args,true);
  const std::string source=R"(
#include <objc3c/runtime/objc3_task.hpp>
inline int runtime_requests(objc3_executor_t *destination,objc3_task_t *child)
    __objc3_async {
  __objc3_await objc3::task_checkpoint;
  __objc3_await objc3::task_yield;
  __objc3_await objc3::hop_to(destination);
  return __objc3_await objc3::join_task<int>(child);
}
inline int runtime_group(int value) __objc3_async
    __attribute__((objc_task_group));
inline int runtime_group(int value) __objc3_async {
  auto group=objc3::task_group_handle::current_scope();
  group.close();
  auto end=__objc3_await group.next<int>();
  __objc3_await group;
  return end ? 0 : value;
}
int private_implementation() {return objc3_executor_is_current(nullptr);}
inline int actor_request(objc3_actor_t *actor) __objc3_async {
  __objc3_await objc3::hop_to(actor);
  objc3_current_actor_facts facts{};
  facts.struct_size=sizeof(facts);
  facts.version=1;
  return objc3_actor_current_facts(&facts)==OBJC3_SERVICE_OK &&
         facts.actor_identity==objc3_actor_identity(actor) ? 42 : 0;
}
)";
  auto build=[&](const std::string &text,const auto &inputs) {
    auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,
        "runtime-declarations.mm","clang-tool",
        std::make_shared<clang::PCHContainerOperations>(),
        clang::tooling::getClangStripDependencyFileAdjuster(),inputs);
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    return ast;
  };
  auto ast=build(source,files);
  assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
      "fixture.runtime","test",{}));
  auto emitted=EmitSemanticInterface(ast->getASTContext(),"fixture.runtime","test");
  for (const auto &d:emitted.diagnostics)
    std::fprintf(stderr,"%s: %s [%s]\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(emitted.success);
  assert(emitted.source_bytes.find("promise_type")==std::string::npos);
  const std::vector<llvm::StringRef> supported={
      "objc3.cxx-data-interfaces.v1","objc3.canonical-function-effects.v1",
      "objc3.isolation-contracts.v1","objc3.async-entry-abi.v5",
      kCxxDefinitionSemantic,kCxxAsyncDefinitionSemantic,
      kSdkRuntimeDeclarationSemantic,RuntimeInterfaceSemantic(),
      "objc3.module-owned-names.v1"};
  auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,supported);
  assert(decoded);
  assert(std::all_of(decoded->declarations.begin(),decoded->declarations.end(),
      [](const auto &d) {return HasSdkRuntimeDeclarationIdentity(d)==
          (d.qualified_name!="private_implementation");}));
  auto old_reader=supported;
  std::erase(old_reader,kSdkRuntimeDeclarationSemantic);
  auto unsupported=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,old_reader);
  assert(!unsupported);
  llvm::consumeError(unsupported.takeError());
  auto unpinned=*decoded;
  std::erase(unpinned.required_semantics,RuntimeInterfaceSemantic().str());
  auto unpinned_encoding=EncodeSemanticInterfaceSidecar(unpinned,supported);
  assert(!unpinned_encoding);
  llvm::consumeError(unpinned_encoding.takeError());
  std::erase(decoded->required_semantics,kSdkRuntimeDeclarationSemantic);
  auto stripped=EncodeSemanticInterfaceSidecar(*decoded,supported);
  assert(!stripped);
  llvm::consumeError(stripped.takeError());
  auto round=build(emitted.source_bytes,files);
  auto regenerated=EmitSemanticInterface(round->getASTContext(),"fixture.runtime","test");
  assert(regenerated.success && regenerated.identity.semantic_fingerprint==
         emitted.identity.semantic_fingerprint);
  // Printing an asm-labelled definition must restore every non-label attribute.
  auto repeated=EmitSemanticInterface(ast->getASTContext(),"fixture.runtime","test");
  assert(repeated.success && repeated.source_bytes==emitted.source_bytes);
  for (const std::string suffix : {"/objc3_task.hpp","/objc3_runtime_services.h"}) {
    auto changed=files;
    for (auto &file:changed)
      if (llvm::StringRef(file.first).ends_with(suffix)) file.second+="\n// changed SDK\n";
    auto changed_ast=build(source,changed);
    assert(!EmitSemanticInterface(changed_ast->getASTContext(),"fixture.runtime","test").success);
  }
  for (const std::string rejected : {
      "struct objc3_task { int application_owned; };\ninline objc3_task_t *opaque(objc3_task_t *p) {return p;}\n",
      "inline int hidden() {return (int)objc3::task_detail::result_traits<int>::storage_size;}\n"}) {
    auto rejected_ast=build("#include <objc3c/runtime/objc3_task.hpp>\n"+rejected,files);
    auto rejected_interface=EmitSemanticInterface(rejected_ast->getASTContext(),"fixture.runtime","test");
    assert(!rejected_interface.success);
  }
}

void TestCAsyncPhysicalABIImportBoundaries() {
  // Frame ABI is a property of the callable type, not of the destination AST.
  // A C++ consumer simultaneously owns task-returning and frame-returning
  // signatures with the same logical result type.
  // Tooling borrows mapped header bytes. Retain each mapping until after both
  // ASTs and their importer are destroyed, including deferred line-table reads.
  std::vector<std::unique_ptr<clang::tooling::FileContentMappings>> mapped_files;
  auto build = [&](bool cpp) {
    std::vector<std::string> args = {"-x",
                                     cpp ? "objective-c++" : "objective-c",
                                     cpp ? "-std=c++20" : "-std=c17",
                                     "-fobjc-version=3",
                                     "-fobjc-runtime=gnustep-2.2",
                                     "--target=x86_64-unknown-linux-gnu"};
    auto files = std::make_unique<clang::tooling::FileContentMappings>();
    AddRuntimeInterfaceFiles(*files, args);
    const std::string source =
        std::string("#include <objc3c/runtime/objc3_runtime_services.h>\n") +
        (cpp ? "template<class T> struct TaskCarrier { void *context; }; "
               "template<class T> using __objc3_async_task = TaskCarrier<T>; "
               "int task_fetch(int) __objc3_async; "
               "int fetch(int) __objc3_async_frame; "
               "void inspect(void) { (void)task_fetch(1); (void)fetch(2); }"
             : "int fetch(int) __objc3_async; "
               "void inspect(void) { (void)fetch(1); }");
    auto ast = clang::tooling::buildASTFromCodeWithArgs(
        source, args, cpp ? "async_import.mm" : "async_import.m", "clang-tool",
        std::make_shared<clang::PCHContainerOperations>(),
        clang::tooling::getClangStripDependencyFileAdjuster(), *files);
    mapped_files.push_back(std::move(files));
    return ast;
  };
  auto function = [](clang::ASTContext &context, llvm::StringRef name) {
    for (auto *decl : context.getTranslationUnitDecl()->decls())
      if (auto *fd = llvm::dyn_cast<clang::FunctionDecl>(decl))
        if (fd->getName() == name)
          return fd;
    return static_cast<clang::FunctionDecl *>(nullptr);
  };
  for (bool source_cpp : {false, true}) {
    for (bool target_cpp : {false, true}) {
      auto from = build(source_cpp), to = build(target_cpp);
      assert(from && to && !from->getDiagnostics().hasErrorOccurred() &&
             !to->getDiagnostics().hasErrorOccurred());
      auto *from_function = function(from->getASTContext(), "fetch");
      auto *to_function = function(to->getASTContext(), "fetch");
      assert(from_function && to_function);
      auto &context = to->getASTContext();
      clang::ASTImporter importer(context, to->getFileManager(),
                                  from->getASTContext(), from->getFileManager(),
                                  false);
      auto imported = importer.Import(from_function->getType());
      if (!imported) {
        llvm::errs() << "async import " << source_cpp << " -> " << target_cpp
                     << ": " << llvm::toString(imported.takeError()) << '\n';
        assert(false && "async function type import failed");
      }
      if (!context.hasSameType(*imported, to_function->getType()))
        llvm::errs() << "async import " << source_cpp << " -> " << target_cpp
                     << ": imported " << imported->getAsString()
                     << ", expected " << to_function->getType().getAsString()
                     << '\n';
      assert(imported &&
             context.hasSameType(*imported, to_function->getType()));
      const auto *prototype = (*imported)->castAs<clang::FunctionProtoType>();
      assert(prototype->getObjC3FunctionEffects().AsyncFrame);
      if (target_cpp) {
        auto *task = function(context, "task_fetch");
        assert(task && !context.hasSameType(task->getType(), *imported));
        assert(!task->getType()
                    ->castAs<clang::FunctionProtoType>()
                    ->getObjC3FunctionEffects()
                    .AsyncFrame);
      }
      InterfaceDeclarationIdentity identity;
      assert(ReadInterfaceDeclarationIdentity(to_function, context, identity));
      assert(identity.effects.async_abi_identity == "v1:4:type10:c-frame-v1");
    }
  }
}

void TestObjectiveCContainerRoundTrips() {
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      "__attribute__((objc_root_class)) @interface Widget\n- "
      "(int)value;\n@end\n",
      {"-x", "objective-c", "-std=gnu11", "-fobjc-runtime=gnustep-2.2",
       "--target=x86_64-unknown-linux-gnu"},
      "objc_fixture.m");
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());

  auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.widget",
                                       "test-package");
  if (!emitted.success)
    for (const auto &diagnostic : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(),
                   diagnostic.message.c_str());
  assert(emitted.success);
  assert(emitted.source_bytes.find("@interface Widget") != std::string::npos);
  assert(emitted.source_bytes.find("- (int)value") != std::string::npos);
  assert(emitted.source_bytes.find("objc_root_class") != std::string::npos);
}

void TestTypedEffectSemanticInterfacesRoundTrip() {
  for (const char *source :
       {"void typed_api(void) throws(int);\n",
        "typedef void TypedCallback(void) throws(int);\n",
        "typedef void TypedCallback(void) throws(int); "
        "void accepts_callback(TypedCallback *);\n",
        "typedef void TypedCallback(void) throws(int); "
        "struct CallbackStorage { TypedCallback *callback; };\n",
        "void accepts_inline_callback(void (*callback)(void) throws(int));\n",
        "@interface EffectWidget\n- (int)value throws(int);\n@end\n",
        "@protocol EffectProtocol\n+ (int)value:(int)x throws(int);\n@end\n"}) {
    auto ast = clang::tooling::buildASTFromCodeWithArgs(
        source,
        {"-x", "objective-c", "-std=c17", "-fobjc-version=3",
         "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"},
        "effect_fixture.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.effect",
                                         "test-package");
    if (!emitted.success)
      for (const auto &diagnostic : emitted.diagnostics)
        std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(),
                     diagnostic.message.c_str());
    assert(emitted.success && !emitted.source_bytes.empty() &&
           !emitted.sidecar_json.empty());
    const std::vector<llvm::StringRef> semantics = {
        "objc3.canonical-function-effects.v1"};
    auto decoded = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    assert(decoded->required_semantics ==
           std::vector<std::string>{"objc3.canonical-function-effects.v1"});
    bool found_typed_effect = false;
    bool found_canonical_effect = false;
    for (const auto &identity : decoded->declarations) {
      found_canonical_effect |= identity.canonical_type_identity.find(
                                    "throws(int)") != std::string::npos;
      if (identity.effects.is_throwing) {
        assert(identity.effects.throws_kind == "typed");
        assert(identity.effects.error_type_identity == "int");
        assert(identity.canonical_type_identity.find("throws(int)") !=
               std::string::npos);
        found_typed_effect = true;
      }
    }
    assert(found_canonical_effect);
    if (std::string(source).find("accepts_inline_callback") ==
        std::string::npos)
      assert(found_typed_effect);
    auto regenerated_ast = clang::tooling::buildASTFromCodeWithArgs(
        emitted.source_bytes,
        {"-x", "objective-c", "-std=c17", "-fobjc-version=3",
         "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"},
        "relocated-effect-interface.m");
    assert(regenerated_ast &&
           !regenerated_ast->getDiagnostics().hasErrorOccurred());
    auto regenerated = EmitSemanticInterface(regenerated_ast->getASTContext(),
                                             "fixture.effect", "test-package");
    assert(regenerated.success);
    assert(regenerated.source_bytes == emitted.source_bytes);
    assert(regenerated.identity.semantic_fingerprint ==
           emitted.identity.semantic_fingerprint);
  }
}

void TestManagedReturnOwnershipRoundTrips() {
  std::string previous_fingerprint;
  std::map<std::string, SemanticInterfaceSidecar> mrc_views;
  for (bool arc : {false, true})
    for (const char *attribute :
         {"ns_returns_retained", "ns_returns_not_retained",
          "ns_returns_autoreleased"}) {
      const std::string source = "@protocol Error @end\nid managed(void) "
                                 "throws(id<Error>) __attribute__((" +
                                 std::string(attribute) + "));\n";
      std::vector<std::string> arguments = {
          "-x",
          "objective-c",
          "-std=c17",
          "-fobjc-version=3",
          "-fobjc-runtime=gnustep-2.2",
          "--target=x86_64-unknown-linux-gnu"};
      if (arc)
        arguments.push_back("-fobjc-arc");
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source, arguments,
                                                          "managed-return.m");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      auto emitted = EmitSemanticInterface(ast->getASTContext(),
                                           "fixture.managed", "test-package");
      for (const auto &diagnostic : emitted.diagnostics)
        std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(),
                     diagnostic.message.c_str());
      assert(emitted.success &&
             emitted.source_bytes.find(attribute) != std::string::npos);
      assert(previous_fingerprint != emitted.identity.semantic_fingerprint);
      previous_fingerprint = emitted.identity.semantic_fingerprint;
      auto round_trip = clang::tooling::buildASTFromCodeWithArgs(
          emitted.source_bytes, arguments, "relocated.m");
      assert(round_trip && !round_trip->getDiagnostics().hasErrorOccurred());
      auto regenerated = EmitSemanticInterface(
          round_trip->getASTContext(), "fixture.managed", "test-package");
      if (!regenerated.success ||
          regenerated.source_bytes != emitted.source_bytes)
        std::fprintf(stderr, "original:\n%s\nregenerated:\n%s\n",
                     emitted.source_bytes.c_str(),
                     regenerated.source_bytes.c_str());
      assert(regenerated.success &&
             regenerated.source_bytes == emitted.source_bytes);
      assert(regenerated.identity.semantic_fingerprint ==
             emitted.identity.semantic_fingerprint);
      const std::vector<llvm::StringRef> supported = {
          "objc3.canonical-function-effects.v1",
          "objc3.native-return-ownership.v1"};
      auto decoded = DecodeSemanticInterfaceSidecar(
          emitted.sidecar_json, emitted.source_bytes, supported);
      assert(decoded);
      if (!arc)
        mrc_views[attribute] = *decoded;
      else {
        const auto &mrc = mrc_views.at(attribute);
        // Ownership modes remain distinct module identities. The declaration
        // contract must agree for this explicitly modeled managed-return API.
        assert(mrc.module.runtime_abi != decoded->module.runtime_abi);
        assert(mrc.module.semantic_fingerprint !=
               decoded->module.semantic_fingerprint);
        assert(mrc.declarations.size() == decoded->declarations.size());
        for (std::size_t i = 0; i != mrc.declarations.size(); ++i)
          assert(SameInterfaceDeclarationIdentity(mrc.declarations[i],
                                                  decoded->declarations[i]));
      }
      bool found = false;
      for (const auto &declaration : decoded->declarations)
        if (declaration.qualified_name == "managed") {
          assert(declaration.effects.ownership_identity.starts_with(
              "native-return:"));
          found = true;
        }
      assert(found);
      auto stripped = *decoded;
      std::erase(stripped.required_semantics,
                 "objc3.native-return-ownership.v1");
      auto checked = ValidateSemanticInterface(
          stripped, decoded->module, decoded->canonical_source_sha256,
          decoded->declarations,
          {"objc3.canonical-function-effects.v1",
           "objc3.native-return-ownership.v1"});
      bool rejected_erasure = false;
      for (const auto &diagnostic : checked.diagnostics)
        rejected_erasure |= diagnostic.code == "O3IFC014";
      assert(rejected_erasure);
      auto old_consumer = DecodeSemanticInterfaceSidecar(
          emitted.sidecar_json, emitted.source_bytes,
          {"objc3.canonical-function-effects.v1"});
      assert(!old_consumer);
      llvm::consumeError(old_consumer.takeError());
    }
}

void TestDirectMethodIdentityRoundTrip() {
  std::string previous;
  for (bool direct : {false, true}) {
    const std::string source =
        "__attribute__((objc_root_class)) @interface Direct "
        "- (int)value" +
        std::string(direct ? " __attribute__((objc_direct))" : "") + "; @end";
    const std::vector<std::string> args = {
        "-x", "objective-c", "-fobjc-version=3", "-fobjc-runtime=gnustep-2.2",
        "--target=x86_64-unknown-linux-gnu"};
    auto ast =
        clang::tooling::buildASTFromCodeWithArgs(source, args, "direct.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted =
        EmitSemanticInterface(ast->getASTContext(), "direct.methods", "test");
    assert(emitted.success &&
           emitted.identity.semantic_fingerprint != previous);
    previous = emitted.identity.semantic_fingerprint;
    auto round = clang::tooling::buildASTFromCodeWithArgs(emitted.source_bytes,
                                                          args, "round.m");
    assert(round && !round->getDiagnostics().hasErrorOccurred());
    auto regenerated =
        EmitSemanticInterface(round->getASTContext(), "direct.methods", "test");
    assert(regenerated.success &&
           regenerated.identity.semantic_fingerprint == previous);
    const std::vector<llvm::StringRef> supported = {"objc3.direct-dispatch.v1"};
    auto decoded = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, supported);
    assert(decoded);
    if (direct) {
      assert(emitted.source_bytes.find("objc_direct") != std::string::npos);
      assert(decoded->required_semantics ==
             std::vector<std::string>{"objc3.direct-dispatch.v1"});
      auto erased = *decoded;
      erased.required_semantics.clear();
      auto checked = ValidateSemanticInterface(
          erased, decoded->module, decoded->canonical_source_sha256,
          decoded->declarations, {"objc3.direct-dispatch.v1"});
      bool rejected = false;
      for (const auto &diagnostic : checked.diagnostics)
        rejected |= diagnostic.code == "O3IFC015";
      assert(rejected);
      auto unaware = DecodeSemanticInterfaceSidecar(emitted.sidecar_json,
                                                    emitted.source_bytes, {});
      assert(!unaware);
      llvm::consumeError(unaware.takeError());
    }
  }
}

void TestManagedIvarAndPropertyRoundTrip() {
  const std::vector<std::string> args = {"-x",
                                         "objective-c",
                                         "-std=c17",
                                         "-fobjc-version=3",
                                         "-fobjc-arc",
                                         "-fobjc-runtime=gnustep-2.2",
                                         "--target=x86_64-unknown-linux-gnu"};
  std::string previous;
  for (const char *access : {"@public", "@protected", "@private", "@package"}) {
    const std::string source =
        "typedef Optional(id) Strong; "
        "typedef Optional(id __weak) Weak; "
        "struct Record { int marker; Strong value; }; "
        "__attribute__((objc_root_class)) @interface Heap { Class isa; " +
        std::string(access) +
        " Strong values[2]; Weak weak; "
        "id __strong nativeStrong; id __weak nativeWeak; } "
        "@property(nonatomic) Strong value; "
        "@property(nonatomic) struct Record record; @end";
    auto ast = clang::tooling::buildASTFromCodeWithArgs(source, args, "heap.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted =
        EmitSemanticInterface(ast->getASTContext(), "fixture.heap", "test");
    for (const auto &diagnostic : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(),
                   diagnostic.message.c_str());
    assert(emitted.success &&
           emitted.identity.semantic_fingerprint != previous);
    assert(emitted.source_bytes.find(access) != std::string::npos);
    assert(emitted.source_bytes.find("values[2]") != std::string::npos);
    assert(emitted.source_bytes.find("__weak id nativeWeak") !=
           std::string::npos);
    assert(emitted.source_bytes.find("__strong id nativeStrong") !=
           std::string::npos);
    previous = emitted.identity.semantic_fingerprint;
    auto round = clang::tooling::buildASTFromCodeWithArgs(emitted.source_bytes,
                                                          args, "round.m");
    assert(round && !round->getDiagnostics().hasErrorOccurred());
    auto regenerated =
        EmitSemanticInterface(round->getASTContext(), "fixture.heap", "test");
    assert(regenerated.success &&
           regenerated.source_bytes == emitted.source_bytes);
    assert(regenerated.identity.semantic_fingerprint == previous);
  }
}

void TestDeletedAggregateMemberIdentity() {
  const std::vector<std::string> args = {"-x", "c++", "-std=c++20",
                                         "--target=x86_64-unknown-linux-gnu"};
  auto emit = [&](const char *source) {
    auto ast =
        clang::tooling::buildASTFromCodeWithArgs(source, args, "aggregate.cpp");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto result =
        EmitSemanticInterface(ast->getASTContext(), "fixture.deleted", "test");
    assert(result.success);
    return result;
  };
  auto ordinary = emit("struct Plain { int value; int get() const; };");
  auto deleted = emit("struct Plain { int value; int get() const = delete; };");
  assert(ordinary.identity.semantic_fingerprint !=
         deleted.identity.semantic_fingerprint);
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1"};
  auto decoded = DecodeSemanticInterfaceSidecar(
      deleted.sidecar_json, deleted.source_bytes, semantics);
  assert(decoded);
  assert(std::find(decoded->required_semantics.begin(),
                   decoded->required_semantics.end(),
                   "objc3.cxx-class-lifecycles.v1") !=
         decoded->required_semantics.end());
  assert(std::any_of(decoded->declarations.begin(), decoded->declarations.end(),
                     [](const auto &d) {
                       return d.kind == "CXXMethod" &&
                              d.qualified_name == "Plain::get" &&
                              d.canonical_type_identity.find(";deleted=1") !=
                                  std::string::npos;
                     }));
}

void TestCxxClassStructure() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-class-structure.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    const std::vector<std::string> args = {"-x", "c++", standard,
                                           "--target=x86_64-unknown-linux-gnu"};
    auto emit = [&](const std::string &source,
                    const std::vector<std::string> &exports =
                        std::vector<std::string>{}) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source, args,
                                                          "structure.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(), "fixture.structure",
                                   "test", {}, {}, {}, exports);
    };
    const char *source =
        "struct Empty {}; struct First { int first; }; struct Second { long "
        "second; }; "
        "struct Derived : Empty, First, protected Second { int last; "
        "Derived()=default; Derived(const Derived&)=default; "
        "Derived(Derived&&)=default; "
        "Derived& operator=(const Derived&)=default; Derived& "
        "operator=(Derived&&)=default; "
        "~Derived()=default; int sum() const; }; "
        "struct Reference { int &value; Reference()=default; Reference(const "
        "Reference&)=default; };";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s\n", d.code.c_str(), d.message.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    bool bases = false, defaulted = false, deleted_default = false;
    for (const auto &d : decoded->declarations) {
      if (d.kind == "CXXRecord" && d.qualified_name == "Derived") {
        bases =
            d.canonical_type_identity.find(";base=") != std::string::npos &&
            d.canonical_type_identity.find(":offset=8") != std::string::npos &&
            d.canonical_type_identity.find(":access=protected") !=
                std::string::npos;
      }
      defaulted |=
          d.canonical_type_identity.find(";defaulted=1;") != std::string::npos;
      deleted_default |=
          d.qualified_name == "Reference::Reference" &&
          d.canonical_type_identity.find(";deleted=1") != std::string::npos;
    }
    assert(bases && defaulted && deleted_default);
    auto round = clang::tooling::buildASTFromCodeWithArgs(emitted.source_bytes,
                                                          args, "round.cpp");
    assert(round && !round->getDiagnostics().hasErrorOccurred());
    auto regenerated = EmitSemanticInterface(round->getASTContext(),
                                             "fixture.structure", "test");
    assert(regenerated.success &&
           regenerated.source_bytes == emitted.source_bytes &&
           regenerated.identity.semantic_fingerprint ==
               emitted.identity.semantic_fingerprint);
    std::erase(decoded->required_semantics, "objc3.cxx-class-structure.v1");
    decoded->module.semantic_fingerprint =
        ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased);
    llvm::consumeError(erased.takeError());
    auto first = emit("struct A {}; struct B {}; struct D : A, B {};");
    auto reversed = emit("struct A {}; struct B {}; struct D : B, A {};");
    assert(first.success && reversed.success &&
           first.identity.semantic_fingerprint !=
               reversed.identity.semantic_fingerprint);
    auto pub =
        emit("struct B { int n; }; class D : public B { public: D(); };");
    auto priv =
        emit("struct B { int n; }; class D : private B { public: D(); };");
    assert(pub.success && priv.success &&
           pub.identity.semantic_fingerprint !=
               priv.identity.semantic_fingerprint);
    const std::string stable =
        "struct Stable { int n; Stable()=default; Stable(const "
        "Stable&)=default; ~Stable()=default; };";
    auto unused = emit(stable, {"Stable"});
    auto used =
        emit(stable + " void use() { Stable a{}; Stable b(a); }", {"Stable"});
    assert(unused.success && used.success &&
           unused.source_bytes == used.source_bytes &&
           unused.identity.semantic_fingerprint ==
               used.identity.semantic_fingerprint);
    auto derived_exception = emit("struct E { int n; E()=default; };");
    auto explicit_exception =
        emit("struct E { int n; E() noexcept(false)=default; };");
    assert(derived_exception.success && explicit_exception.success &&
           derived_exception.identity.semantic_fingerprint !=
               explicit_exception.identity.semantic_fingerprint);
    auto excluded =
        emit("struct Base { int n; }; struct Derived : Base {};", {"Derived"});
    assert(!excluded.success &&
           std::any_of(excluded.diagnostics.begin(), excluded.diagnostics.end(),
                       [](const auto &d) { return d.code == "O3IFC156"; }));
  }
}

void TestCxxLocalExpressions() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-declaration-expressions.v1", "objc3.cxx-definitions.v1",
      "objc3.cxx-block-expressions.v3", "objc3.cxx-statement-expressions.v1",
      "objc3.module-owned-names.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-xc++",standard,"-fblocks","--target=x86_64-unknown-linux-gnu"},"local-expressions.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.locals","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.locals","test");
    };
    const std::string source=
        "int api(int n=^int {return 42;}()); "
        "struct Value { int n=^int {return 40;}(); int get(int delta=^int {return 2;}()) const {return n+delta;} }; "
        "inline int compute(int n) { __block int state=n; "
        "auto read=^long(int delta) {return n+delta;}; "
        "auto change=^int(int delta) {state+=delta; return state;}; "
        "return ({int answer=(int)read(2); answer==change(2) ? answer : 0;}); }";
    auto emitted=emit(source);
    for (const auto &d:emitted.diagnostics)
      std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(emitted.success);
    auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,semantics);
    assert(decoded);
    auto round=emit(emitted.source_bytes);
    assert(round.success && round.source_bytes==emitted.source_bytes &&
           round.identity.semantic_fingerprint==emitted.identity.semantic_fingerprint);
    auto changed=source;
    changed.replace(changed.find("n+delta"),std::string("n+delta").size(),"n+delta+1");
    auto different=emit(changed);
    assert(different.success && different.identity.semantic_fingerprint!=emitted.identity.semantic_fingerprint);
    for (const auto capability:{"objc3.cxx-block-expressions.v3","objc3.cxx-statement-expressions.v1"}) {
      auto erased=*decoded;
      std::erase(erased.required_semantics,capability);
      erased.module.semantic_fingerprint=ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(erased));
      auto invalid=EncodeSemanticInterfaceSidecar(erased,semantics);
      assert(!invalid); llvm::consumeError(invalid.takeError());
    }
  }
}

void TestCxxLocalTypes() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-definitions.v1",
      "objc3.cxx-member-interfaces.v1", "objc3.cxx-declaration-expressions.v1",
      "objc3.cxx-lambda-expressions.v1", "objc3.cxx-local-types.v1",
      "objc3.cxx-unnamed-local-types.v1",
      "objc3.module-owned-names.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source, const char *filename = "local-types.cpp") {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-xc++", standard, "--target=x86_64-unknown-linux-gnu"}, filename);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),
          ast->getASTContext().getTranslationUnitDecl(), "fixture.localtypes", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.localtypes", "test");
    };
    const std::string source =
        "inline int local_types(int n) { enum class Kind : unsigned {delta=2}; "
        "struct Value { int n; explicit Value(int x):n(x) {} "
        "Value(const Value &v):n(v.n) {} virtual ~Value() {} "
        "virtual int get(int delta) const {return n+delta;} "
        "struct Nested {int n;}; }; "
        "Value first(n); Value copy(first); Value *base=&copy; "
        "Value::Nested nested{base->get((int)Kind::delta)}; "
        "return nested.n; } "
        "inline int local_lambda(int n) { return [n] { "
        "struct Value {int n; int get() const {return n+2;}}; "
        "return Value{n}.get(); }(); }"
        "inline constexpr int unnamed_constant(int n) {enum {delta=2}; "
        "struct {int n; constexpr int get() const {return n+delta;}} value{n}; return value.get();} "
        "inline int unnamed_nested(int n) {struct {struct {int n; int get() const {return n+2;}} part; "
        "int get() const {return part.get();}} value{{n}}; return value.get();} "
        "inline int unnamed_lambda(int n) {return [n] {struct {int n;} value{n};return value.n+2;}();} "
        "inline int unnamed_types(int n) {struct {int n;} one{n}; struct {int n;} two{2}; "
        "using First=decltype(one);First copy=one;return copy.n+two.n;} ";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    assert(std::find(decoded->required_semantics.begin(), decoded->required_semantics.end(),
                     kCxxLocalTypeSemantic) != decoded->required_semantics.end());
    auto round = emit(emitted.source_bytes, "regenerated.cpp");
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto relocated = emit(source, "another-provider.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto changed = source;
    changed.replace(changed.find("delta=2"), std::string("delta=2").size(), "delta=3");
    auto different = emit(changed);
    assert(different.success && different.identity.semantic_fingerprint != emitted.identity.semantic_fingerprint);
    changed = source;
    changed.replace(changed.find("return n+delta"), std::string("return n+delta").size(),
                    "return n+delta+1");
    auto changed_member = emit(changed);
    assert(changed_member.success &&
           changed_member.identity.semantic_fingerprint != emitted.identity.semantic_fingerprint);
    auto defaults = emit(
        "namespace Api { int defaulted(int n=[] { struct Local {int n;}; return Local{42}.n; }()); "
        "struct Holder { int n=[] {struct Local {int n;}; return Local{42}.n;}(); "
        "int get() const {struct Local {int n;}; return Local{n}.n;} }; }");
    assert(defaults.success);
    auto defaults_round = emit(defaults.source_bytes);
    assert(defaults_round.success && defaults_round.source_bytes == defaults.source_bytes &&
           defaults_round.identity.semantic_fingerprint == defaults.identity.semantic_fingerprint);
    auto layout=emit("inline int layout() {struct {int value;char unused;} object{42,0};return object.value;}");
    auto changed_layout=emit("inline int layout() {struct {int value;long unused;} object{42,0};return object.value;}");
    assert(layout.success && changed_layout.success &&
           layout.identity.semantic_fingerprint!=changed_layout.identity.semantic_fingerprint);
    auto unnamed_erased=*decoded;
    std::erase(unnamed_erased.required_semantics,kCxxUnnamedLocalTypeSemantic);
    auto unnamed_invalid=EncodeSemanticInterfaceSidecar(unnamed_erased,semantics);
    assert(!unnamed_invalid);llvm::consumeError(unnamed_invalid.takeError());
    auto erased = *decoded;
    std::erase(erased.required_semantics, kCxxLocalTypeSemantic);
    erased.module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(erased));
    auto invalid = EncodeSemanticInterfaceSidecar(erased, semantics);
    assert(!invalid); llvm::consumeError(invalid.takeError());
    auto factory = emit("inline auto factory() { struct Local {int n;}; return Local{42}; }");
    assert(factory.success && factory.sidecar_json.find(kCxxLocalFactoryResultSemantic)!=std::string::npos);
    for (const auto *boundary : {
             "static int hidden(); inline int call() {struct Local {int get() {return hidden();}}; return Local{}.get();}",
             "struct Public {struct {int n;} value;};"}) {
      auto rejected = emit(boundary);
      assert(!rejected.success);
    }
  }
}

void TestGenericLocalLambdas() {
  const std::vector<llvm::StringRef> semantics={"objc3.cxx-data-interfaces.v1",
      "objc3.cxx-definitions.v1","objc3.cxx-lambda-expressions.v1",
      "objc3.cxx-generic-local-lambdas.v2"};
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &code,const char *file="generic-local.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(code,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(),"fixture.genericlocal","test",{},{},{},exports);
    };
    const std::string source=R"(
      inline int generic_local(int n) {
        auto add=[offset=2](auto value){return value+offset;};
        auto pack=[]<class... T>(T... values){return (0+...+values);};
        auto fixed=[]<int N>(int value){return value+N;};
        return add(n)+add(2L)+pack(n,2)+fixed.template operator()<2>(n);
      }
      inline int generic_conversion(int n) {
        int (*callback)(int)=[](auto x){return x+2;};return callback(n);
      }
      inline int generic_mutable(int n) {
        auto add=[n](auto delta) mutable {n+=delta;return n;};return add(1)+add(1L);
      }
      inline constexpr int generic_immediate() {return [](auto n) consteval {return n+2;}(40);}
      inline constexpr int generic_default() {
        auto value=[]<class T=int>(T n=42){return n;};return value.template operator()<>();
      }
      inline constexpr int generic_constant() {return [](auto n){return n+2;}(40);}
      inline int generic_nested(int n) {
        return [](auto outer){return [](auto inner){return inner+2;}(outer);}(n);
      }
    )";
    auto result=emit(source);
    for (const auto &d:result.diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(result.success);
    auto decoded=DecodeSemanticInterfaceSidecar(result.sidecar_json,result.source_bytes,semantics);
    assert(decoded);
    auto repeated=emit(result.source_bytes,"regenerated.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==result.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"another-provider.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==result.identity.semantic_fingerprint);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxGenericLocalLambdaSemantic);
    auto erased=EncodeSemanticInterfaceSidecar(damaged,semantics);
    assert(!erased);llvm::consumeError(erased.takeError());
    auto hidden=emit("static int pick(int){return 42;} int pick(long){return 0;} "
        "inline int api(){auto f=[](auto x){return pick(x);};return f(1);}","private.cpp",{"pick","api"});
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    auto factory=emit("inline auto api(){return [](auto x){return x;};}");
    assert(factory.success && factory.sidecar_json.find(kCxxClosureFactorySemantic)!=std::string::npos);
    auto local_argument=emit("inline int api(){struct Local{int n;}; "
        "return [](auto x){return x.n;}(Local{42});}");
    assert(!local_argument.success);
    auto constrained=emit("inline int api(){return []<class T>(T value) requires(sizeof(T)>0) {return value;}(42);}");
    assert(constrained.success);
    auto constrained_copy=emit(constrained.source_bytes,"constrained-copy.cpp");
    assert(constrained_copy.success && constrained_copy.identity.semantic_fingerprint==constrained.identity.semantic_fingerprint);
    auto dependent_noexcept=emit("inline int api(){return [](auto value) noexcept(sizeof(value)>0) {return value;}(42);}");
    assert(dependent_noexcept.success);
    auto noexcept_copy=emit(dependent_noexcept.source_bytes,"noexcept-copy.cpp");
    assert(noexcept_copy.success && noexcept_copy.identity.semantic_fingerprint==dependent_noexcept.identity.semantic_fingerprint);
  }
}

void TestGenericLocalContracts() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="generic-contracts.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(),"fixture.genericcontracts","test",{},{},{},exports);
    };
    const std::string source=R"(
      struct ConfigBase { int bias; };
      struct Config : ConfigBase { int values[2]; double scale; };
      struct Choice { int left; int right; };
      struct NullableConfig { const int *pointer; int value; };
      int public_probe(int); long public_probe(long);
      inline int generic_captured_local(int n) {
        struct Local {int pick(int) const {return 42;} int pick(long) const {return 42;}};
        Local receiver;auto run=[receiver](auto value){return receiver.pick(value);};return run(n);
      }
      inline constexpr int generic_overload() {
        return []<class T>(T n) requires requires {public_probe(n);} {return 42;}(1);
      }
      inline constexpr int noexcept_data=[](auto n) noexcept(sizeof(n)>0) {return n;}(42);
      inline int noexcept_default(int n=[](auto value) noexcept(sizeof(value)>0) {return value;}(42)) {return n;}
      struct NoexceptDefaults {int value=[](auto n) noexcept(sizeof(n)>0) {return n;}(42);};
      inline constexpr int constrained() {
        auto checked=[]<class T>(T value) noexcept(noexcept(value+2))
            requires requires(T item) { item+2; } {return value+2;};
        auto head=[]<class T> requires (sizeof(T)>0) (T value) {return value+2;};
        static_assert(noexcept(checked(40)));
        return checked(40)+head(40);
      }
      inline constexpr int structural() {
        auto read=[]<Config C>() {return C.bias+C.values[0]+C.values[1]+int(C.scale);};
        auto pick=[]<Choice C>() {return C.right;};
        auto nullable=[]<NullableConfig C>() {return C.pointer ? 0 : C.value;};
        return read.template operator()<Config{{2},{20,18},2.0}>()+
               pick.template operator()<Choice{0,42}>()+
               nullable.template operator()<NullableConfig{nullptr,42}>();
      }
      inline constexpr int floating() {
        auto read=[]<double D>() {return D==0.0 ? (__builtin_signbit(D) ? 2 : 1) : int(D);};
        return read.template operator()<42.0>()+read.template operator()<0.0>()+
               read.template operator()<-0.0>();
      }
    )";
    auto first=emit(source);
    for (const auto &d:first.diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto roundtrip=emit(first.source_bytes,"reprinted.cpp");
    assert(roundtrip.success && roundtrip.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto concepts=emit("template<class T> concept Number=sizeof(T)>0; "
        "inline int api(){return []<Number T>(T n){return n;}(42);}");
    assert(concepts.success);
    auto hidden_type=emit("struct Secret{int n;}; inline int api(){return "
        "[]<Secret S>(){return 42;}.template operator()<Secret{1}>();}","private.cpp",{"api"});
    assert(!hidden_type.success);
    auto hidden_constraint=emit("static int probe(int); int probe(long); inline int api(){"
        "auto f=[](auto x) requires requires {probe(x);} {return 42;};return f(1);}",
        "private-constraint.cpp",{"probe","api"});
    assert(!hidden_constraint.success && std::any_of(hidden_constraint.diagnostics.begin(),
        hidden_constraint.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
    auto address=emit("inline int value=42; struct Config{const int *p;}; inline int api(){return "
        "[]<Config C>(){return *C.p;}.template operator()<Config{&value}>();}");
    assert(address.success && address.sidecar_json.find(kCxxAddressTemplateArgumentSemantic)!=std::string::npos);
    auto address_replay=emit(address.source_bytes,"address-lambda-replay.cpp");
    assert(address_replay.success && address_replay.identity.semantic_fingerprint==address.identity.semantic_fingerprint);
    auto excessive=emit("struct Large{int values[5000];}; inline int api(){return "
        "[]<Large V>(){return 42;}.template operator()<Large{}>();}");
    assert(!excessive.success);
  }
}

void TestCxxUnionValues() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="unions.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(),"fixture.unions","test");
    };
    const std::string source=R"(
      union Forward; Forward *forward(Forward *);
      union Plain {int integer;double floating;};
      union Choice {int left;int right;};
      typedef union {int integer;long wide;} Aliased;
      struct Nested {union Value {int n;double d;}; Value value;};
      struct Member {int n;constexpr Member(int value):n(value){} constexpr ~Member() {}};
      union Managed {
        Member member;long other;
        constexpr Managed(int n):member(n){}
        constexpr Managed(const Managed &other):member(other.member.n){}
        constexpr ~Managed() {member.~Member();}
        constexpr int number() const {return member.n;}
      };
      inline constexpr int lifecycle() {Managed first(40);Managed copy=first;return copy.number()+2;}
      inline constexpr int choices() {
        auto use=[]<Choice C>() {return 42;};
        return use.template operator()<Choice{.left=42}>()+
               use.template operator()<Choice{.right=42}>();
      }
      inline int local(int n) {union Local {int n;double d;};Local value{.n=n};return value.n+2;}
      inline int unnamed(int n) {union {int n;double d;} value{.n=n};
        auto read=[value]{return value.n+2;};return read();}
      inline Plain plain(int n) {return Plain{.integer=n};}
    )";
    auto first=emit(source);
    for (const auto &d:first.diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    assert(first.sidecar_json.find("objc3.cxx-union-values.v1")!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("double floating"),15,"long double floating");
    auto layout=emit(changed);assert(layout.success && layout.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    std::vector<llvm::StringRef> semantics={"objc3.cxx-data-interfaces.v1","objc3.cxx-union-values.v1"};
    auto simple=emit("union Value{int n;double d;};");
    assert(simple.success);
    auto decoded=DecodeSemanticInterfaceSidecar(simple.sidecar_json,simple.source_bytes,semantics);
    assert(decoded);
    std::erase(decoded->required_semantics,kCxxUnionValueSemantic);
    auto damaged=EncodeSemanticInterfaceSidecar(*decoded,semantics);
    assert(!damaged);llvm::consumeError(damaged.takeError());
    auto factory=emit("inline auto factory(){union Local{int n;};return Local{42};}");
    assert(factory.success && factory.sidecar_json.find(kCxxLocalFactoryResultSemantic)!=std::string::npos);
    auto anonymous_public=emit("struct Public{union{int n;double d;} value;};");
    assert(!anonymous_public.success);
  }
}

void TestPublicConcepts() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="concepts.cpp",
                  const char *module="fixture.concepts",const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(),module,"test",{},{},{},exports);
    };
    const std::string source=R"(
      namespace Predicates {
        template<class T> concept Addable=requires(T value) {value+2;};
        template<class T> concept Wide=Addable<T> && (sizeof(T)>=sizeof(int));
        template<int N> concept Positive=N>0;
        template<class... T> concept All=(Addable<T> && ...);
        template<class T> concept Compound=Addable<T> && requires(T n){requires sizeof(T)>0;n+2;};
      }
      using Predicates::Wide;
      inline constexpr int concept_value() {return []<Wide T>(T n){return n+2;}(40);}
      inline constexpr bool concept_constant=Predicates::Positive<2> && Predicates::All<int,long> && Predicates::Compound<int>;
    )";
    auto first=emit(source);
    for (const auto &d:first.diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find("objc3.cxx-concepts.v1")!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto other=emit(source,"other.cpp","fixture.otherconcepts");
    assert(other.success && other.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("sizeof(T)>=sizeof(int)"),std::string("sizeof(T)>=sizeof(int)").size(),"sizeof(T)>=1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto simple=emit("template<class T> concept Sized=sizeof(T)>0;");
    assert(simple.success);
    const std::vector<llvm::StringRef> semantics={
        "objc3.cxx-data-interfaces.v1",kCxxConceptSemantic};
    auto decoded=DecodeSemanticInterfaceSidecar(simple.sidecar_json,simple.source_bytes,semantics);
    assert(decoded);
    for (const auto *capability:{kCxxConceptSemantic,"objc3.cxx-data-interfaces.v1"}) {
      auto damaged=*decoded;
      std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);
      assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto private_dependency=emit("static int probe(int); int probe(long); "
        "template<class T> concept C=requires(T value){probe(value);};", "private.cpp");
    assert(!private_dependency.success);
    auto excluded=emit("template<class T> concept C=sizeof(T)>0; "
        "inline int api(){return []<C T>(T value){return value;}(42);}","excluded.cpp", "fixture.concepts", {"api"});
    assert(!excluded.success);
    auto forged=emit("template<class T> concept C [[clang::annotate(\"objc3.concept-owner:forged\")]]=true;", "forged.cpp");
    assert(!forged.success);
  }
}

void TestPublicFunctionTemplates() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="templates.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.templates","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.templates","test",{},{},{},exports);
    };
    const std::string source=R"(
      namespace Generic {
        template<class T> concept Number=requires(T n){n+2;};
        template<class T,int Bias=2> constexpr T add(T n){return n+Bias;}
        template<class... T> constexpr int sum(T... n){return (0+...+n);}
        template<class T> requires Number<T> constexpr int rank(T){return 40;}
        template<class T> requires (Number<T> && sizeof(T)>=sizeof(int)) constexpr int rank(T){return 42;}
        template<Number T> constexpr T identity(T n) noexcept(noexcept(n+2)){return n;}
        template<int N> constexpr int count(){return N;}
      }
      inline constexpr int value(){return Generic::add(40)+Generic::sum(20,22)+Generic::rank(1)+
          Generic::identity(42)+Generic::count<42>();}
    )";
    auto first=emit(source);
    for (const auto &d:first.diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find("objc3.cxx-function-templates.v2")!=std::string::npos);
    auto metadata=llvm::json::parse(first.sidecar_json);
    assert(metadata && metadata->getAsObject());
    std::vector<llvm::StringRef> semantics;
    for (const auto &value:*metadata->getAsObject()->getArray("required_semantics"))
      semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);
    assert(decoded);
    for (const auto *capability:{kCxxFunctionTemplateSemantic,"objc3.cxx-data-interfaces.v1"}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);
      assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto repeated=emit(first.source_bytes,"reprinted-templates.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-templates.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("return n+Bias"),13,"return n+Bias+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    assert(!emit("static int hidden;template<class T> int api(T n){return hidden+n;}").success);
    assert(!emit("template<class T> T helper(T n){return n;}inline int api(){return helper(42);}","excluded-template.cpp",{"api"}).success);
    assert(!emit("template<class T> T declared(T n);").success);
  }
}

void TestPublicAliasTemplates() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="aliases.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.aliases","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.aliases","test",{},{},{},exports);
    };
    const std::string source=R"(
      namespace Types {
        template<class T> concept Number=requires(T n){n+2;};
        template<class T=int> using Pointer=T*;
        template<class... T> using Callback=int(*)(T...);
        template<class T> using Element=typename T::element;
        template<Number T> using NumericPointer=T*;
        template<class T> requires(sizeof(T)>0) using SizedPointer=T*;
        struct Box{using element=int;};
      }
      inline Types::Pointer<> pointer=nullptr;
      int sum(int,int);
      inline Types::Callback<int,int> callback=sum;
      inline Types::Element<Types::Box> element=42;
    )";
    auto first=emit(source);
    for (const auto &d:first.diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find("objc3.cxx-alias-templates.v1")!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-aliases.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-aliases.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("using Pointer=T*"),std::string("using Pointer=T*").size(),"using Pointer=const T*");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for (const auto &value:*metadata->getAsObject()->getArray("required_semantics")) semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    for (const auto *capability:{kCxxAliasTemplateSemantic,"objc3.cxx-data-interfaces.v1"}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);
      assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    assert(!emit("template<class T> using Hidden=T*;int api(Hidden<int>);","excluded-alias.cpp",{"api"}).success);
    auto private_type=emit("namespace {struct Hidden{};}template<class T> using Alias=Hidden*;");
    assert(!private_type.success && std::any_of(private_type.diagnostics.begin(),private_type.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    assert(emit("template<template<class>class C> using Alias=C<int>;").success);
  }
}

void TestTemplateDeduction() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="deduction.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.deduction","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.deduction","test");
    };
    const std::string source=R"(
      template<class T> constexpr auto add(T n){return n+2;}
      template<class T> constexpr decltype(auto) borrow(T &n){return (n);}
      template<class T> constexpr auto make_reader(T n){return [n](int d){return n+d;};}
      template<class T> class Holder {
        T value;
      public:
        constexpr Holder(T n):value(n){}
        constexpr auto number()const{return value+2;}
        constexpr auto reader()const{return [n=value](int d){return n+d;};}
        constexpr decltype(auto) reference(){return (value);}
      };
      template<class T=int> inline constexpr auto answer=T{42};
      template<class T> inline auto reader=[n=T{40}](int d){return n+d;};
      inline int storage=40;
      inline constexpr int probe(){return add(40)+make_reader(40)(2)+Holder<long>{40}.number()+answer<>;}
      inline int mutable_probe(){++borrow(storage);return reader<int>(2);}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxTemplateDeductionSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-deduction.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-deduction.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("return n+2"),10,"return n+3");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxTemplateDeductionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto hidden=emit("static int hidden(){return 42;}template<class T>auto bad(T){return hidden();}");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
  }
}

void TestTemplateStructure() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="template-structure.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.structure","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.structure","test");
    };
    const std::string source=R"(
      template<class T=int,int N=sizeof(T)> extern T count;
      template<class U,int M> U count=M;
      template<class T=int,int Bias=2> class Root;
      template<class U,int Delta> class Root {
        U value;
      public:
        explicit Root(U n=U{40}):value(n){}
        virtual ~Root()=default;
        virtual int number()const{return int(value)+Delta;}
      };
      template<class T=int> struct Left:virtual Root<T>{Left():Root<T>(T{40}){}};
      template<class T=int> struct Right:virtual Root<T>{Right():Root<T>(T{40}){}};
      template<class T=int> struct Diamond:Left<T>,Right<T>{
        Diamond():Root<T>(T{40}){}
        int number()const override{return Root<T>::number();}
      };
      inline int probe(){Diamond<> value;Root<int> *root=&value;return root->number()+count<>;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxTemplateStructureSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-structure.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-structure.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("value)+Delta"),std::string("value)+Delta").size(),"value)+Delta+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    for(const auto *capability:{kCxxTemplateStructureSemantic,"objc3.cxx-data-interfaces.v1"}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=*decoded;std::erase(old.required_semantics,kCxxTemplateStructureSemantic);
    for(auto &declaration:old.declarations) {
      const std::string marker=";cxx-template-structure:v1;";
      if(auto at=declaration.canonical_type_identity.find(marker);at!=std::string::npos)
        declaration.canonical_type_identity.erase(at,marker.size());
    }
    auto old_contract=EncodeSemanticInterfaceSidecar(old,semantics);
    assert(!old_contract);llvm::consumeError(old_contract.takeError());
    auto hidden=emit("namespace {template<class T>struct Hidden{};}template<class T>struct Public:Hidden<T>{};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    for(const auto *name:{"Root","count"})
      assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),
          [&](const auto &d){return d.qualified_name==name;})==1);
  }
}

void TestAddressTemplateArguments() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="address-template-arguments.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.address_arguments","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.address_arguments","test",{},{},{},exports);
    };
    const std::string source=R"(

namespace NativeAddressArguments {
  inline int storage=40, other=40;
  int add(int n){return n+2;}
  struct Base {int value;};
  struct Record:Base {int get(int n)const{return value+n;}};
  template<auto Address>struct AddressBox {static constexpr auto address=Address;};
  template<int &Value>struct Reference {static int get(){return Value;}};
  template<auto Address>inline int counter=0;
  template<auto Address>int invoke(int n){return Address(n);}
  template<>inline int invoke<&add>(int n){return add(n);}
  template<auto Address>inline int selected=0;
  template<>inline int selected<&storage> =42;
  template<auto Address>struct Selection {static constexpr int value=0;};
  template<>struct Selection<&storage> {static constexpr int value=42;};
  struct Bundle {int *address;int(*function)(int);int Record::*member;};
  template<Bundle Value>struct Structural {
    static int run(Record &record){return Value.function(*Value.address)+(record.*Value.member);}
  };
  using Bound=Structural<Bundle{&storage,&add,&Base::value}>;
  using Object=AddressBox<&storage>;
  using Function=AddressBox<&add>;
  using Member=AddressBox<&Record::get>;
  using Ref=Reference<storage>;
  struct Array {int values[2];};inline Array array{{40,42}};
  struct Pointer {const int *value;};
  template<Pointer Value>struct Subobject {static int get(){return *Value.value;}};
  using Element=Subobject<Pointer{&array.values[1]}>;
  using End=Subobject<Pointer{&array.values[2]}>;
  int *counter_slot(){++counter<&storage>;return &counter<&storage>;}
}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxAddressTemplateArgumentSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-address-template-arguments.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-address-template-arguments.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxAddressTemplateArgumentIdentity)>=6);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxAddressTemplateArgumentSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxAddressTemplateArgumentSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;auto position=changed.find("using Object=AddressBox<&storage>");assert(position!=std::string::npos);
    changed.replace(position,33,"using Object=AddressBox<&other>");auto altered=emit(changed);
    assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto excluded=emit("inline int hidden=42;template<int* P>struct Box{};using Public=Box<&hidden>;",
        "excluded-address.cpp",{"Box","Public"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC156";}));
    auto private_object=emit("static int hidden=42;template<int* P>struct Box{};using Public=Box<&hidden>;");
    assert(!private_object.success);
  }
}


void TestCInlineDefinitionContract() {
  const std::vector<std::string> args={"-x","objective-c","-std=c17",
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2"};
  auto emit=[&](const std::string &source) {
    auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,"c-inline-provider.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
        "fixture.c_inline","test",{}));
    return EmitSemanticInterface(ast->getASTContext(),"fixture.c_inline","test");
  };
  auto first=emit("inline int c_inline_value(int value){return value+1;}");
  for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(first.success && first.source_bytes.find("return value + 1")!=std::string::npos);
  auto replay=emit(first.source_bytes);assert(replay.success);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.source_bytes==replay.source_bytes);
  assert(first.identity.semantic_fingerprint!=emit("inline int c_inline_value(int value){return value+2;}").identity.semantic_fingerprint);
  auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
  std::vector<llvm::StringRef> semantics;
  for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
  assert(std::any_of(decoded->declarations.begin(),decoded->declarations.end(),HasCInlineDefinitionIdentity));
  assert(std::none_of(decoded->declarations.begin(),decoded->declarations.end(),HasCxxDefinitionIdentity));
  auto missing=*decoded;std::erase(missing.required_semantics,kCInlineDefinitionSemantic);
  auto missing_cap=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!missing_cap);llvm::consumeError(missing_cap.takeError());
  auto old=semantics;std::erase(old,llvm::StringRef(kCInlineDefinitionSemantic));
  auto old_reader=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!old_reader);llvm::consumeError(old_reader.takeError());
  auto invalid_owner=*decoded;
  for(auto &declaration:invalid_owner.declarations)if(HasCInlineDefinitionIdentity(declaration))declaration.kind="Var";
  auto forged=EncodeSemanticInterfaceSidecar(invalid_owner,semantics);assert(!forged);llvm::consumeError(forged.takeError());
  auto plain=emit("int c_inline_declaration(int);");assert(plain.success);
  auto control=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,semantics);assert(control);
  control->required_semantics.push_back(kCInlineDefinitionSemantic);
  auto invented=EncodeSemanticInterfaceSidecar(*control,semantics);assert(!invented);llvm::consumeError(invented.takeError());
  auto immutable=emit("extern inline int c_inline_const(void){static const int value=42;return value;}");
  assert(immutable.success);
  auto immutable_replay=emit(immutable.source_bytes);assert(immutable_replay.success);
  assert(immutable.identity.semantic_fingerprint==immutable_replay.identity.semantic_fingerprint);
  auto mutable_static=emit("extern inline int c_inline_mutable(void){static int value;return ++value;}");
  assert(!mutable_static.success && std::any_of(mutable_static.diagnostics.begin(),mutable_static.diagnostics.end(),
      [](const auto &d){return d.code=="O3IFC115";}));
  auto external=emit("extern inline int c_inline_external(int value){return value;}");
  assert(external.success && external.source_bytes.find("extern inline")==std::string::npos);
  auto external_replay=emit(external.source_bytes);assert(external_replay.success);
  assert(external.identity.semantic_fingerprint==external_replay.identity.semantic_fingerprint);
  auto redeclared=emit("inline int c_inline_redeclared(int value){return value;}extern int c_inline_redeclared(int);");
  assert(redeclared.success);
  auto redeclared_replay=emit(redeclared.source_bytes);assert(redeclared_replay.success);
  assert(redeclared.identity.semantic_fingerprint==redeclared_replay.identity.semantic_fingerprint);
}

void TestDeclarationOnlyFunctionTemplates() {
  const std::string source=R"(
    namespace DeclarationTemplates {
      template<class T> T convert(T value);
      template<class T> T convert(T value);
      template<class T> struct Box {
        T get() const;
        template<class U> U map(U value) const;
        static T value;
        template<class U> static U shared;
      };
    }
  )";
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"},
          "declaration-only-templates.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
          "fixture.declaration_templates","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.declaration_templates","test",{},{},{},
          {"DeclarationTemplates"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.source_bytes==replay.source_bytes);
    assert(first.identity.semantic_fingerprint==emit("\n\n"+source).identity.semantic_fingerprint);
    auto consumer=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes+R"(
      namespace DeclarationTemplates {
        template<class T> T convert(T value){return value;}
        template<class T> T Box<T>::get() const {return T{42};}
        template<class T> template<class U> U Box<T>::map(U value) const {return value;}
        template<class T> T Box<T>::value=T{42};
        template<class T> template<class U> U Box<T>::shared=U{42};
      }
      int declaration_template_consumer(){
        DeclarationTemplates::Box<int> box;
        return DeclarationTemplates::convert(box.get())+box.map(42)+
            DeclarationTemplates::Box<int>::value+DeclarationTemplates::Box<int>::shared<int>;
      }
    )",{"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"},"declaration-only-consumer.mm");
    assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
    auto changed=source;auto at=changed.find("T convert(T value)");assert(at!=std::string::npos);
    while(at!=std::string::npos){changed.replace(at,18,"T convert(const T &value)");at=changed.find("T convert(T value)",at+1);}
    assert(first.identity.semantic_fingerprint!=emit(changed).identity.semantic_fingerprint);
  }
}

void TestNativeAllocatorAndDiagnosticContracts() {
  for(const char *standard:{"c17","c++20","c++23"}) {
    const bool cxx=std::string(standard)!="c17";
    for(bool arc:{false,true}) {
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"};
      if(arc)args.push_back("-fobjc-arc");
      const std::string source=R"(
        typedef unsigned long Size;
        void *allocate(Size size) __attribute__((ownership_returns(example,1)));
        void *allocate(Size size);
        void dispose(void *first,void *second) __attribute__((ownership_takes(example,1,2)));
        void hold(void *pointer) __attribute__((ownership_holds(example,1)));
        void typed(const void *payload,int tag) __attribute__((pointer_with_type_tag(example,1,2)));
        extern const int IntTag __attribute__((type_tag_for_datatype(example,int,layout_compatible)));
        void expected_format(const char *format) __attribute__((format_matches(printf,1,"%d %s")));
        void *copy(const void *source,Size size,void *destination)
            __attribute__((diagnose_as_builtin(__builtin_memcpy,3,1,2)));
        #ifdef __cplusplus
        template<class T> T *allocate_for(Size size) __attribute__((ownership_returns(example,1)));
        template<class T> struct TagInfo {
          static const int tag __attribute__((type_tag_for_datatype(example,T)));
          void check(const char *format) __attribute__((format_matches(printf,2,"%d")));
        };
        #endif
      )";
      auto emit=[&](const std::string &text,const char *file,
                    const std::vector<std::string>&exports=std::vector<std::string>{}) {
        auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,file);
        assert(ast&&!ast->getDiagnostics().hasErrorOccurred());
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_diagnostics","test",{},{},{},exports);
        for(const auto&d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        return result;
      };
      auto first=emit(source,"native-diagnostics.m");assert(first.success);
      assert(first.source_bytes.find("ownership_returns")!=std::string::npos);
      assert(first.source_bytes.find("type_tag_for_datatype")!=std::string::npos);
      assert(first.source_bytes.find("diagnose_as_builtin")!=std::string::npos);
      assert(first.source_bytes.find("format_matches")!=std::string::npos);
      auto repeated=emit(first.source_bytes,"replayed/native-diagnostics.m");assert(repeated.success);
      assert(first.identity.semantic_fingerprint==repeated.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated/native-diagnostics.m");assert(relocated.success);
      assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);

      auto consumer_args=args;consumer_args.push_back("-Werror=format");consumer_args.push_back("-Werror=type-safety");
      auto valid=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes+
          "\nvoid client(void){typed((const int*)0,IntTag);expected_format(\"%d %s\");}",consumer_args,"source-free-valid.m");
      assert(valid&&!valid->getDiagnostics().hasErrorOccurred());
      for(const char *body:{
          "void client(void){typed(\"bad\",IntTag);}",
          "void client(void){expected_format(\"%s %s\");}"}) {
        auto invalid=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes+"\n"+body,consumer_args,"source-free-invalid.m");
        assert(invalid&&invalid->getDiagnostics().hasErrorOccurred());
      }
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto&item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(std::find(supported.begin(),supported.end(),llvm::StringRef("objc3.native-declaration-attributes.v2"))!=supported.end());
      auto older=supported;std::erase(older,llvm::StringRef(kNativeDeclarationAttributeSemantic));
      older.push_back("objc3.native-declaration-attributes.v1");
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      for(const auto&change:std::vector<std::pair<std::string,std::string>>{
          {"ownership_returns(example,1)","ownership_returns(other,1)"},
          {"ownership_takes(example,1,2)","ownership_holds(example,1,2)"},
          {"pointer_with_type_tag(example,1,2)","argument_with_type_tag(example,1,2)"},
          {"type_tag_for_datatype(example,int,layout_compatible)","type_tag_for_datatype(example,long,layout_compatible)"},
          {"type_tag_for_datatype(example,int,layout_compatible)","type_tag_for_datatype(example,int)"},
          {"\"%d %s\"","\"%ld %s\""},
          {"diagnose_as_builtin(__builtin_memcpy,3,1,2)","diagnose_as_builtin(__builtin_memmove,3,1,2)"}}) {
        auto changed=source;const auto position=changed.find(change.first);assert(position!=std::string::npos);
        changed.replace(position,change.first.size(),change.second);
        // Change matching declarations together when the family occurs in a
        // template declaration as well as its ordinary counterpart.
        if(change.first=="ownership_returns(example,1)") {
          for(auto at=changed.find(change.first);at!=std::string::npos;at=changed.find(change.first))
            changed.replace(at,change.first.size(),change.second);
        }
        auto altered=emit(changed,"changed/native-diagnostics.m");assert(altered.success);
        assert(first.identity.semantic_fingerprint!=altered.identity.semantic_fingerprint);
      }
      auto hidden=emit("struct Hidden{int value;};extern const int Tag __attribute__((type_tag_for_datatype(example,struct Hidden)));",
          "hidden-native-type.m",{"Tag"});
      assert(!hidden.success);
    }
  }
}

void TestNativeDeclarationAttributes() {
  std::ifstream input(std::filesystem::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/declaration_attributes.h");
  assert(input);const std::string source((std::istreambuf_iterator<char>(input)),{});
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &text,const char *file="native-attributes.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.attributes","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.attributes","test",{},{},{},{"NativeDeclarationAttributes"});
    };
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"ATTR %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    for(const auto *spelling:{"packed","alignas","no_unique_address","nodiscard","noinline","always_inline","visibility","pure"})
      assert(first.source_bytes.find(spelling)!=std::string::npos);
    for(const auto *spelling:{"pass_object_size","pass_dynamic_object_size","lifetimebound","noescape","callback","sentinel","diagnose_if","returns_twice","disable_tail_calls","not_tail_called","nomerge","noduplicate","optnone","leaf"})
      assert(first.source_bytes.find(spelling)!=std::string::npos);
    assert(first.source_bytes.find("nothrow")!=std::string::npos || first.source_bytes.find("noexcept")!=std::string::npos);
    assert(first.source_bytes.find("enable_if")!=std::string::npos);
    const auto second=first.source_bytes.find("swift_attr(\"@Second\")");
    const auto middle=first.source_bytes.find("swift_attr(\"@First\")",second+1);
    const auto repeated=first.source_bytes.find("swift_attr(\"@Second\")",middle+1);
    assert(second!=std::string::npos && middle!=std::string::npos && repeated!=std::string::npos);
    auto replay=emit(first.source_bytes);
    for(const auto &d:replay.diagnostics)std::fprintf(stderr,"ATTR REPLAY %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    assert(replay.source_bytes==first.source_bytes);
    auto bad_alignment=clang::tooling::buildASTFromCodeWithArgs(
        first.source_bytes+"\nNativeDeclarationAttributes::Slot<int,3> invalid;\n",
        {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"},"bad-dependent-alignment.mm");
    assert(bad_alignment && bad_alignment->getDiagnostics().hasErrorOccurred());
    auto moved=emit("\n\n"+source,"relocated-native-attributes.mm");
    assert(moved.success && moved.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasNativeDeclarationAttributeIdentity)>15);
    auto missing=*decoded;std::erase(missing.required_semantics,kNativeDeclarationAttributeSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto row=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasNativeDeclarationAttributeIdentity);
    row->kind="Namespace";auto bad_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!bad_owner);llvm::consumeError(bad_owner.takeError());
    auto changed=source;auto at=changed.find("alignas(32)");assert(at!=std::string::npos);changed.replace(at,11,"alignas(64)");
    auto aligned=emit(changed);assert(aligned.success && aligned.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;at=changed.find("keep ");assert(at!=std::string::npos);changed.replace(at,4,"save");
    auto message=emit(changed);assert(message.success && message.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;at=changed.find("alloc_size(1)");assert(at!=std::string::npos);changed.replace(at,13,"alloc_size(2)");
    auto allocation=emit(changed);assert(allocation.success && allocation.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;at=changed.find("format(printf,1,2)");assert(at!=std::string::npos);changed.replace(at,18,"format(printf,1,0)");
    auto format=emit(changed);assert(format.success && format.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;at=changed.find("malloc(deallocate,1)");assert(at!=std::string::npos);
    changed.replace(at,std::string("malloc(deallocate,1)").size(),"malloc(deallocate_other,1)");
    auto deallocator=emit(changed);assert(deallocator.success && deallocator.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;at=changed.find("pass_object_size(0)");assert(at!=std::string::npos);
    changed.replace(at,std::string("pass_object_size(0)").size(),"pass_object_size(1)");
    auto size_contract=emit(changed);assert(size_contract.success && size_contract.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;at=changed.find("negative value");assert(at!=std::string::npos);changed.replace(at,14,"invalid value");
    auto predicate_message=emit(changed);assert(predicate_message.success && predicate_message.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;at=changed.find("value<minimum");assert(at!=std::string::npos);changed.replace(at,13,"value<=minimum");
    auto predicate_condition=emit(changed);assert(predicate_condition.success && predicate_condition.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;at=changed.find("nonnegative inline");assert(at!=std::string::npos);
    changed.replace(at,std::string("nonnegative inline").size(),"enabled inline");
    auto enabled_message=emit(changed);assert(enabled_message.success && enabled_message.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto hidden_enabled=emit("namespace Private {inline constexpr int bound=0;}"
        "namespace NativeDeclarationAttributes {int enabled(int value) __attribute__((enable_if(value>=Private::bound,\"hidden bound\")));}");
    assert(!hidden_enabled.success && std::any_of(hidden_enabled.diagnostics.begin(),hidden_enabled.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
    auto hidden_predicate=emit("namespace Private {inline constexpr int minimum=0;}"
        "namespace NativeDeclarationAttributes {int checked(int value) __attribute__((diagnose_if(value<Private::minimum,\"private bound\",\"error\")));}");
    assert(!hidden_predicate.success && std::any_of(hidden_predicate.diagnostics.begin(),hidden_predicate.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
    auto hidden_deallocator=emit("namespace Private {void release(void *);}"
        "namespace NativeDeclarationAttributes {__attribute__((malloc(Private::release,1))) void *allocate(unsigned long);}");
    assert(!hidden_deallocator.success && std::any_of(hidden_deallocator.diagnostics.begin(),hidden_deallocator.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC156";}));
    auto projected=emit(source+"\nnamespace NativeDeclarationAttributes {using PublicProjection=decltype(Family<int>{}.first);}");
    assert(projected.success);
    assert(projected.sidecar_json.find(kCxxUnnamedTemplateProjectionSemantic)!=std::string::npos);
    // Dependent attribute expressions must retain their real API dependencies.
    auto denied=emit("namespace Private {inline constexpr int alignment=32;}\n"
        "namespace NativeDeclarationAttributes {template<class T>struct alignas(sizeof(T)+Private::alignment) Bad {T value;};}\n");
    assert(!denied.success);
    assert(std::any_of(denied.diagnostics.begin(),denied.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
  // C records/functions retain the same native ABI and optimizer contracts.
  const std::string c_source="struct __attribute__((packed,aligned(2))) Packet {char code;int value;};\n"
      "__attribute__((noinline,pure,warn_unused_result)) int read(const struct Packet *value);\n";
  auto emit_c=[&](const std::string &text) {
    auto ast=clang::tooling::buildASTFromCodeWithArgs(text,{"-xc","-std=c17"},"native-attributes.c");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    return EmitSemanticInterface(ast->getASTContext(),"fixture.c_attributes","test");
  };
  auto c=emit_c(c_source);assert(c.success);
  auto replay=emit_c(c.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==c.identity.semantic_fingerprint);
}

void TestUnnamedRecordProjections() {
  std::ifstream input(std::filesystem::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/unnamed_record_projections.h");
  assert(input);const std::string source((std::istreambuf_iterator<char>(input)),{});
  for (const char *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &text,const char *file="unnamed-record-projections.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      ast->getDiagnostics().getClient()->BeginSourceFile(ast->getLangOpts(),&ast->getPreprocessor());
      auto EndDiagnostics=llvm::scope_exit([&]{ast->getDiagnostics().getClient()->EndSourceFile();});
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.record_projections","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.record_projections","test",{},{},{},{"NativeUnnamedRecordProjections"});
    };
    auto first=emit(source);
    for (const auto &d:first.diagnostics)std::fprintf(stderr,"PROJECT %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);
    for (const auto &d:replay.diagnostics)std::fprintf(stderr,"PROJECT REPLAY %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    assert(replay.source_bytes==first.source_bytes);
    auto moved=emit("\n\n"+source,"moved-projections.mm");
    assert(moved.success && moved.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto type=[&](const char *name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(type("NativeUnnamedRecordProjections::First").canonical_type_identity!=type("NativeUnnamedRecordProjections::Second").canonical_type_identity);
    assert(type("NativeUnnamedRecordProjections::Reader").canonical_type_identity!=type("NativeUnnamedRecordProjections::ReverseReader").canonical_type_identity);
    assert(type("NativeUnnamedRecordProjections::Kind").canonical_type_identity!=type("NativeUnnamedRecordProjections::OtherKind").canonical_type_identity);
    assert(type("NativeUnnamedRecordProjections::EnumAnswer").canonical_type_identity!=type("NativeUnnamedRecordProjections::OtherEnumAnswer").canonical_type_identity);
    std::set<std::string> overloads;
    for(const auto &row:decoded->declarations)if(row.qualified_name=="NativeUnnamedRecordProjections::provider_read")overloads.insert(row.native_symbol_name);
    assert(overloads.size()==2 && !overloads.count(""));
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxUnnamedTemplateProjectionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxUnnamedTemplateProjectionIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto bad_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!bad_owner);llvm::consumeError(bad_owner.takeError());
    auto changed=source;const auto at=changed.find("using Nominal=First;");assert(at!=std::string::npos);
    changed.replace(at,std::string("using Nominal=First;").size(),"using Nominal=Second;");
    auto different=emit(changed);assert(different.success && different.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto denied=emit("namespace Private {template<class T>struct Family {struct {T value;} first;};}"
        "namespace NativeUnnamedRecordProjections {using Hidden=decltype(Private::Family<int>{}.first);}");
    assert(!denied.success && std::any_of(denied.diagnostics.begin(),denied.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}

void TestUnnamedTemplateProjections() {
  std::ifstream input(std::filesystem::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/unnamed_template_projections.h");
  assert(input);const std::string source((std::istreambuf_iterator<char>(input)),{});
  for (const char *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &text,const char *file="unnamed-projections.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.projections","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.projections","test",{},{},{},{"NativeUnnamedTemplateProjections"});
    };
    auto first=emit(source);
    for (const auto &d:first.diagnostics)std::fprintf(stderr,"PROJECT %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);
    for (const auto &d:replay.diagnostics)std::fprintf(stderr,"PROJECT REPLAY %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    assert(replay.source_bytes==first.source_bytes);
    auto moved=emit("\n\n"+source,"moved-projections.mm");
    assert(moved.success && moved.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto type=[&](const char *name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(type("NativeUnnamedTemplateProjections::First").canonical_type_identity!=type("NativeUnnamedTemplateProjections::Second").canonical_type_identity);
    assert(type("NativeUnnamedTemplateProjections::Reader").canonical_type_identity!=type("NativeUnnamedTemplateProjections::ReverseReader").canonical_type_identity);
    assert(type("NativeUnnamedTemplateProjections::Kind").canonical_type_identity!=type("NativeUnnamedTemplateProjections::OtherKind").canonical_type_identity);
    assert(type("NativeUnnamedTemplateProjections::EnumAnswer").canonical_type_identity!=type("NativeUnnamedTemplateProjections::OtherEnumAnswer").canonical_type_identity);
    std::set<std::string> overloads;
    for(const auto &row:decoded->declarations)if(row.qualified_name=="NativeUnnamedTemplateProjections::provider_read")overloads.insert(row.native_symbol_name);
    assert(overloads.size()==2 && !overloads.count(""));
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxUnnamedTemplateProjectionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxUnnamedTemplateProjectionIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto bad_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!bad_owner);llvm::consumeError(bad_owner.takeError());
    auto changed=source;const auto at=changed.find("using Nominal=First;");assert(at!=std::string::npos);
    changed.replace(at,std::string("using Nominal=First;").size(),"using Nominal=Second;");
    auto different=emit(changed);assert(different.success && different.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto denied=emit("namespace Private {template<class T>struct Family {struct {T value;} first;};}"
        "namespace NativeUnnamedTemplateProjections {using Hidden=decltype(Private::Family<int>{}.first);}");
    assert(!denied.success && std::any_of(denied.diagnostics.begin(),denied.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}


void TestOpaqueStandardLibrarySpecializations() {
  for(const char *standard:{"c++20","c++23"}) {
    std::vector<std::string> args={"-xc++",std::string("-std=")+standard,"-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    auto emit=[&](const std::string &source,const char *file="opaque-standard.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,file);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      ast->getDiagnostics().getClient()->BeginSourceFile(ast->getLangOpts(),&ast->getPreprocessor());
      auto EndDiagnostics=llvm::scope_exit([&]{ast->getDiagnostics().getClient()->EndSourceFile();});
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.opaque_standard","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.opaque_standard","test",{},{},{},{"NativeOpaqueStandard"},{},&ast->getPreprocessor());
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"OPAQUE STANDARD %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      return result;
    };
    const std::string source=R"(
      #include <functional>
      namespace NativeOpaqueStandard {
        struct Key {int value;};
        template<class T>struct Box {T value;};
      }
      namespace std {
        template<>struct hash<NativeOpaqueStandard::Key>;
        template<class T>struct hash<NativeOpaqueStandard::Box<T>>;
      }
      namespace NativeOpaqueStandard {
        using Hash=std::hash<Key>;
        using BoxHash=std::hash<Box<int>>;
        Hash *provider_hash(Hash *);
      }
    )";
    auto first=emit(source);assert(first.success);
    auto replay=emit(first.source_bytes,"opaque-standard-replay.cpp");
    assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto moved=emit("\n\n"+source,"moved-opaque-standard.cpp");
    assert(moved.success && moved.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto capabilities=SupportedSemanticInterfaceCapabilities();
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,capabilities);assert(decoded);
    unsigned opaque=0;
    for(const auto &row:decoded->declarations)if(HasCxxIncompleteTemplateIdentity(row)) {
      ++opaque;assert(row.kind=="ClassTemplateSpecialization" || row.kind=="ClassTemplatePartialSpecialization");
    }
    assert(opaque==2);
    assert(emit(first.source_bytes+R"(
      static_assert(!__is_complete_type(NativeOpaqueStandard::Hash));
      static_assert(!__is_complete_type(NativeOpaqueStandard::BoxHash));
    )","opaque-standard-consumer.cpp").success);
    const auto completed=first.source_bytes+R"(
      namespace std {
        template<>struct hash<NativeOpaqueStandard::Key> {
          size_t operator()(NativeOpaqueStandard::Key value)const noexcept {return value.value;}
        };
        template<class T>struct hash<NativeOpaqueStandard::Box<T>> {
          size_t operator()(NativeOpaqueStandard::Box<T> value)const noexcept {return value.value;}
        };
      }
      namespace NativeOpaqueStandard {
        inline unsigned completed_hash(){return Hash{}({40})+BoxHash{}({2});}
      }
    )";
    auto complete=emit(completed,"complete-standard.cpp");assert(complete.success);
    auto complete_replay=emit(complete.source_bytes,"complete-standard-replay.cpp");
    assert(complete_replay.success && complete_replay.identity.semantic_fingerprint==complete.identity.semantic_fingerprint);
    auto final=DecodeSemanticInterfaceSidecar(complete.sidecar_json,complete.source_bytes,capabilities);assert(final);
    for(const auto &row:final->declarations)assert(!HasCxxIncompleteTemplateIdentity(row));
  }
}

void TestStandardLibrarySpecializations() {
  llvm::SmallString<256> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-standard-specializations",temporary));
  const auto root=std::filesystem::path(temporary.c_str());
  auto cleanup=llvm::scope_exit([&]{std::filesystem::remove_all(root);});
  const std::string source=R"(
#include <functional>
#include <limits>
#include <ranges>
#include <tuple>
namespace NativeStandard {
struct Key {int value;};
struct Other {int value;};
struct View {int *first;int *last;int *begin()const{return first;}int *end()const{return last;}};
template<class T>struct Box {T value;};
}
namespace std {
template<>struct hash<NativeStandard::Key> {
  constexpr size_t operator()(NativeStandard::Key value) const noexcept {return value.value+100;}
};
template<>struct hash<NativeStandard::Other> {
  constexpr size_t operator()(NativeStandard::Other value) const noexcept {return value.value+200;}
};
template<class T>struct hash<NativeStandard::Box<T>> {
  size_t operator()(const NativeStandard::Box<T> &value) const {return hash<T>{}(value.value);}
};
template<>struct tuple_size<NativeStandard::Key>:integral_constant<size_t,1> {};
template<>struct tuple_element<0,NativeStandard::Key> {using type=int;};
template<>struct common_type<NativeStandard::Key,NativeStandard::Key> {using type=long;};
template<>struct numeric_limits<NativeStandard::Key> {
  static constexpr bool is_specialized=true;
  static constexpr NativeStandard::Key min() noexcept {return {1};}
};
}
namespace std::ranges {
template<>inline constexpr bool enable_borrowed_range<NativeStandard::View> = true;
template<class T>inline constexpr bool enable_view<NativeStandard::Box<T>> = true;
}

namespace NativeStandard {
using Hash=std::hash<Key>;
using Element=std::tuple_element_t<0,Key>;
inline constexpr int total(Key value) {
  return std::hash<Key>{}(value)+std::hash<Other>{}({value.value});
}
}
)";
  for(const char *standard:{"c++20","c++23"})for(int mode=0;mode!=3;++mode) {
    const bool objc=mode!=0,arc=mode==2;
    std::fprintf(stderr,"STANDARD SPECIALIZATIONS %s %d\n",standard,mode);
    std::vector<std::string> flags={"-x",objc?"objective-c++":"c++",std::string("-std=")+standard,
        "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    if(objc)flags.insert(flags.end(),{"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",arc?"-fobjc-arc":"-fno-objc-arc"});
    auto emit=[&](const std::string &text,const char *file="standard-specializations.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,flags,file);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.standard_specializations","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.standard_specializations","test",{},{},{},{"NativeStandard"},{},&ast->getPreprocessor());
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"STANDARD SPECIALIZATION %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      return result;
    };
    auto first=emit(source);assert(first.success);
    assert(first.sidecar_json.find(kCxxStandardLibrarySpecializationSemantic)!=std::string::npos);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.source_bytes==first.source_bytes && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    if(mode==0 && std::string(standard)=="c++20") {
      auto moved=emit("\n\n"+source,"relocated-specializations.cpp");assert(moved.success && moved.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto changed=source;auto at=changed.find("value.value+100");assert(at!=std::string::npos);changed.replace(at,std::string("value.value+100").size(),"value.value+101");
      auto different=emit(changed);assert(different.success && different.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
      auto hidden=emit("#include <functional>\nnamespace Hidden {constexpr int value=42;} namespace NativeStandard {struct Key{int value;};} namespace std {template<>struct hash<NativeStandard::Key>{unsigned long operator()(NativeStandard::Key v)const{return v.value+Hidden::value;}};}");
      assert(!hidden.success);
      auto builtin=emit("#include <functional>\nnamespace std {template<>struct hash<void*>{unsigned long operator()(void*)const{return 0;}};} namespace NativeStandard {std::hash<void*> policy();}");
      assert(!builtin.success);
      auto parsed=llvm::json::parse(first.sidecar_json);assert(parsed);std::vector<llvm::StringRef> semantics;
      for(const auto &value:*parsed->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
      for(int corruption=0;corruption!=3;++corruption) {
        auto bad=*decoded;
        if(corruption==0)std::erase(bad.required_semantics,kCxxStandardLibrarySpecializationSemantic);
        else {
          auto row=std::find_if(bad.declarations.begin(),bad.declarations.end(),HasCxxStandardLibrarySpecializationIdentity);assert(row!=bad.declarations.end());
          if(corruption==1)row->kind="CXXRecord";
          else {auto at=row->canonical_type_identity.find(kCxxStandardLibraryMarker);assert(at!=std::string::npos);row->canonical_type_identity.erase(at);}
        }
        auto rejected=EncodeSemanticInterfaceSidecar(bad,semantics);assert(!rejected);llvm::consumeError(rejected.takeError());
      }
    }
    const auto name=std::string(standard)+"-"+std::to_string(mode);
    const auto interface=root/(name+".objc3i");
    {std::ofstream out(interface);out<<first.source_bytes;}
    {std::ofstream out(interface.string()+".json");out<<first.sidecar_json;}
    InterfaceImportInput input;input.source_path=interface.string();input.locked_identity=first.identity;
    CompileRequest request;request.input_path=(root/(name+(objc?".mm":".cpp"))).string();
    request.source_text=R"(
static_assert(NativeStandard::total({1})==302);
static_assert(std::ranges::borrowed_range<NativeStandard::View>);
static_assert(std::ranges::enable_view<NativeStandard::Box<int>>);
static_assert(std::ranges::enable_view<NativeStandard::Box<long>>);
static_assert(std::tuple_size_v<NativeStandard::Key> == 1);
static_assert(std::is_same_v<NativeStandard::Element,int>);
static_assert(std::is_same_v<std::common_type_t<NativeStandard::Key,NativeStandard::Key>,long>);
static_assert(std::numeric_limits<NativeStandard::Key>::min().value==1);
unsigned long standard_specialization_client() {
  return std::hash<NativeStandard::Box<int>>{}({41});
}
)";
    request.language=objc?BaseLanguage::ObjectiveCxx:BaseLanguage::Cxx;request.objc3=objc;request.arc=arc;
    if(objc)request.objc_runtime="gnustep-2.2";
    request.target_triple="x86_64-unknown-linux-gnu";
    request.arguments={std::string("-std=")+standard,"-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    request.imported_interfaces={input};request.emit=EmitKind::Object;request.output_path=(root/(name+".o")).string();
    auto consumer=Compile(request);
    if(!consumer.success)std::fprintf(stderr,"STANDARD SPECIALIZATION CONSUMER %s\n",consumer.failure.c_str());
    for(const auto &d:consumer.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
    assert(consumer.success);
  }
}

void TestStandardLibraryHeaderContracts() {
  const std::string includes=R"(#include <algorithm>
#include <any>
#include <array>
#include <chrono>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <span>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
)";
  const std::string declarations=R"(
namespace NativeStandard {
  std::string message();
  std::vector<int> numbers();
  std::array<int,2> bounds();
  std::any arbitrary();
  std::function<int(int)> callback();
  std::shared_ptr<int> owner();
  std::optional<int> value();
  std::span<const int> view();
  std::tuple<int,double> entry();
  std::variant<int,double> choice();
  std::chrono::milliseconds duration();
  using Word=std::string;
  using Index=std::vector<int>::size_type;
  template<class T> requires std::is_integral_v<T>
  inline constexpr T bump(T value) {return std::move(value)+1;}
  inline int sum() {auto values=numbers();return std::accumulate(values.begin(),values.end(),0);}
}
)";
  llvm::SmallString<256> directory;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-standard-library-headers",directory));
  const auto test_root=std::filesystem::path(directory.c_str());
  auto cleanup=llvm::scope_exit([&]{std::filesystem::remove_all(test_root);});
  for(const char *standard:{"c++20","c++23"})for(int mode=0;mode!=3;++mode) {
    const auto root=test_root/(std::string(standard)+"-"+std::to_string(mode));
    std::filesystem::create_directory(root);
    std::fprintf(stderr,"STDLIB MODE %s %d\n",standard,mode);
    const bool objc=mode!=0,arc=mode==2;
    std::vector<std::string> args={"-x",objc?"objective-c++":"c++",std::string("-std=")+standard,
        "--target=x86_64-unknown-linux-gnu","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    if(objc){args.push_back("-fobjc-version=3");args.push_back("-fobjc-runtime=gnustep-2.2");}
    if(arc)args.push_back("-fobjc-arc");
    std::string traits_bytes;
    auto emit=[&](const std::string &text,const char *file="standard-library-provider.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      if(traits_bytes.empty()) {
        const auto &source=ast->getSourceManager();
        for(auto it=source.fileinfo_begin();it!=source.fileinfo_end();++it)
          if(it->second->OrigEntry && llvm::sys::path::filename(it->second->OrigEntry->getName())=="type_traits") {
            bool invalid=false;auto bytes=source.getBufferData(source.translateFile(*it->second->OrigEntry),&invalid);
            assert(!invalid);traits_bytes=bytes.str();break;
          }
      }
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.standard_headers","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.standard_headers","test",{},{},{},{"NativeStandard"},{},&ast->getPreprocessor());
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"STDLIB %s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      return result;
    };
    auto first=emit(includes+declarations);assert(first.success);
    assert(first.source_bytes.find("#include <string>")!=std::string::npos);
    assert(first.source_bytes.find("#include <vector>")!=std::string::npos);
    auto replay=emit(first.source_bytes,"standard-library-replay.mm");
    assert(replay.success && replay.source_bytes==first.source_bytes && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    if(mode==0 && std::string(standard)=="c++20") {
    auto relocated=emit("\n\n"+includes+declarations,"relocated-standard-library.mm");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    std::string reverse_includes;
    for(std::size_t end=includes.size();end;) {
      const auto previous=includes.rfind('\n',end-2);
      const auto begin=previous==std::string::npos?0:previous+1;
      reverse_includes+=includes.substr(begin,end-begin);end=begin;
    }
    auto reordered=emit(reverse_includes+declarations);
    assert(reordered.success && reordered.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    }
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto owner=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxStandardLibraryIdentity);
    assert(owner!=decoded->declarations.end());
    std::vector<std::string> headers;assert(ReadCxxStandardLibraryHeaders(*owner,headers));
    assert(std::find(headers.begin(),headers.end(),"string")!=headers.end());
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxStandardLibrarySemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    for(int corruption=0;corruption!=3;++corruption) {
      auto bad=*decoded;auto row=std::find_if(bad.declarations.begin(),bad.declarations.end(),HasCxxStandardLibraryIdentity);
      const auto begin=row->canonical_type_identity.find(std::string(kCxxStandardLibraryMarker)+"headers[");assert(begin!=std::string::npos);
      const auto name=begin+std::string(kCxxStandardLibraryMarker).size()+8;
      if(corruption==0)row->kind="Namespace";
      else if(corruption==1)row->canonical_type_identity.insert(name,"../");
      else {const auto equals=row->canonical_type_identity.find('=',name);row->canonical_type_identity[equals+1]='g';}
      auto rejected=EncodeSemanticInterfaceSidecar(bad,semantics);assert(!rejected);llvm::consumeError(rejected.takeError());
    }
    if(mode==0 && std::string(standard)=="c++20") {
    auto private_type=emit("#include <vector>\nnamespace Hidden {struct Value{int value;};}\nnamespace NativeStandard {std::vector<Hidden::Value> hidden();}");
    assert(!private_type.success);
    auto spoofed=emit("namespace std {template<class T>struct vector{T value;};} namespace NativeStandard {std::vector<int> hidden();}");
    assert(!spoofed.success);
    auto private_alias=emit("#include <vector>\nnamespace Hidden {struct Value{int value;};}\nnamespace NativeStandard {using HiddenIndex=std::vector<Hidden::Value>::size_type;}");
    assert(!private_alias.success);
    auto customization=emit("#include <functional>\nnamespace NativeStandard {struct Key{int value;};}\n"
        "namespace std {template<>struct hash<NativeStandard::Key>{unsigned long operator()(NativeStandard::Key v)const{return v.value;}};}\n"
        "namespace NativeStandard {std::hash<Key> policy();}");
    assert(customization.success);
    assert(customization.sidecar_json.find(kCxxStandardLibrarySpecializationSemantic)!=std::string::npos);
    }
    const auto suffix=std::string(standard)+"-"+std::to_string(mode);
    const auto interface=root/(suffix+".objc3i");
    {std::ofstream file(interface);file<<first.source_bytes;}
    {std::ofstream file(interface.string()+".json");file<<first.sidecar_json;}
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=first.identity;
    CompileRequest request;request.input_path=(root/(suffix+(objc?".mm":".cpp"))).string();
    request.source_text="static_assert(NativeStandard::bump(41)==42);int standard_client(){return NativeStandard::sum()+static_cast<int>(NativeStandard::message().size());}";
    request.language=objc?BaseLanguage::ObjectiveCxx:BaseLanguage::Cxx;request.objc3=objc;request.arc=arc;
    request.target_triple="x86_64-unknown-linux-gnu";if(objc)request.objc_runtime="gnustep-2.2";
    request.arguments={std::string("-std=")+standard,"-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    request.imported_interfaces={imported};request.emit=EmitKind::Object;request.output_path=(root/(suffix+".o")).string();
    auto consumer=Compile(request);
    if(!consumer.success){std::fprintf(stderr,"STDLIB IMPORT %s\n",consumer.failure.c_str());for(const auto &d:consumer.interface_import_diagnostics)std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());for(const auto &d:consumer.diagnostics)std::fprintf(stderr,"%s:%u: %s\n",d.spelling.file.c_str(),d.spelling.line,d.message.c_str());}
    assert(consumer.success && std::filesystem::exists(request.output_path));
    // The registry prepares both header closures, while provider declarations
    // become visible only at their matching source-level import directive.
    auto other_ast=clang::tooling::buildASTFromCodeWithArgs(
        "#include <bitset>\nnamespace NativeOther {std::bitset<8> flags();}",args,"standard-library-other.mm");
    assert(other_ast && !other_ast->getDiagnostics().hasErrorOccurred());
    assert(ApplyModuleOwnedNames(other_ast->getASTContext(),other_ast->getASTContext().getTranslationUnitDecl(),"fixture.standard_other","test",{}));
    auto other=EmitSemanticInterface(other_ast->getASTContext(),"fixture.standard_other","test",{},{},{},{"NativeOther"},{},&other_ast->getPreprocessor());
    assert(other.success);
    const auto other_interface=root/(suffix+"-other.objc3i");
    {std::ofstream file(other_interface);file<<other.source_bytes;}
    {std::ofstream file(other_interface.string()+".json");file<<other.sidecar_json;}
    InterfaceImportInput other_import;other_import.source_path=other_interface.string();other_import.locked_identity=other.identity;
    request.imported_interfaces.push_back(other_import);
    request.output_path=(root/(suffix+"-combined.o")).string();
    auto combined=Compile(request);
    if(!combined.success){std::fprintf(stderr,"STDLIB COMBINED %s\n",combined.failure.c_str());for(const auto &d:combined.interface_import_diagnostics)std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());}
    assert(combined.success && std::filesystem::exists(request.output_path));
    if(objc) {
      auto directed=request;directed.imported_interfaces.clear();directed.source_directed_imports=true;
      directed.interface_search_paths={root.string()};
      InterfaceModuleRequest primary;primary.canonical_name=first.identity.canonical_name;primary.locked_identity=first.identity;primary.initially_visible=false;
      InterfaceModuleRequest secondary;secondary.canonical_name=other.identity.canonical_name;secondary.locked_identity=other.identity;secondary.initially_visible=false;
      directed.module_imports={primary,secondary};
      directed.source_text="@import fixture.standard_headers;\n@import fixture.standard_other;\n"+*request.source_text;
      directed.output_path=(root/(suffix+"-directed.o")).string();
      auto result=Compile(directed);
      if(!result.success){std::fprintf(stderr,"STDLIB DIRECTED %s\n",result.failure.c_str());for(const auto &d:result.interface_import_diagnostics)std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());}
      assert(result.success && std::filesystem::exists(directed.output_path));
    }
    if(mode==0 && std::string(standard)=="c++20") {
      assert(!traits_bytes.empty());
      const auto changed_headers=root/"changed-headers";std::filesystem::create_directory(changed_headers);
      {std::ofstream file(changed_headers/"type_traits");file<<traits_bytes<<"\n// Different native standard-header content.\n";}
      auto changed=request;changed.arguments.insert(changed.arguments.end(),{"-isystem",changed_headers.string()});
      changed.output_path=(root/"rejected-headers.o").string();
      auto rejected=Compile(changed);assert(!rejected.success && !std::filesystem::exists(changed.output_path));
      assert(std::any_of(rejected.interface_import_diagnostics.begin(),rejected.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139" || d.code=="O3IFC183";}));
      auto abi=request;abi.arguments.push_back("-D_GLIBCXX_USE_CXX11_ABI=0");abi.output_path=(root/"rejected-abi.o").string();
      auto incompatible=Compile(abi);assert(!incompatible.success && !std::filesystem::exists(abi.output_path));
      auto configuration=request;configuration.arguments.push_back("-D_GLIBCXX_ASSERTIONS=1");
      configuration.output_path=(root/"rejected-configuration.o").string();
      auto different_configuration=Compile(configuration);
      assert(!different_configuration.success && !std::filesystem::exists(configuration.output_path));
      assert(std::any_of(different_configuration.interface_import_diagnostics.begin(),different_configuration.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139" || d.code=="O3IFC183";}));
      for(const char *flag:{"-fno-exceptions","-fno-rtti"}) {
        auto feature=request;feature.arguments.push_back(flag);
        feature.output_path=(root/(std::string("rejected-")+flag+".o")).string();
        auto result=Compile(feature);
        if(result.success)std::fprintf(stderr,"INCOMPATIBLE STANDARD HEADER FLAG ADMITTED %s\n",flag);
        assert(!result.success && !std::filesystem::exists(feature.output_path));
        assert(std::any_of(result.interface_import_diagnostics.begin(),result.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139" || d.code=="O3IFC183";}));
      }
    }
  }
}

void TestStandardCoroutineTypesAndTraits() {
  const auto header=std::filesystem::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/standard_coroutines.h";
  std::ifstream input(header);assert(input);
  const std::string source((std::istreambuf_iterator<char>(input)),{});
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &text,const char *filename="standard-coroutines.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR},filename);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.standard_coroutines","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.standard_coroutines","test",{},{},{},{"NativeStandardCoroutines"});
    };
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"STD %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    assert(first.source_bytes.find("#include <coroutine>")!=std::string::npos);
    assert(first.source_bytes.find("coroutine_traits<")!=std::string::npos);
    auto replay=emit(first.source_bytes);
    for(const auto &d:replay.diagnostics)std::fprintf(stderr,"STD REPLAY %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    assert(replay.source_bytes==first.source_bytes);
    auto relocated=emit("\n\n"+source,"relocated-standard-coroutines.mm");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::any_of(decoded->declarations.begin(),decoded->declarations.end(),HasCxxStandardCoroutineIdentity));
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),[](const auto &d){return d.canonical_type_identity.find(";cxx-coroutine-traits-specializations:v1;")!=std::string::npos;})==2);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxStandardCoroutineSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxStandardCoroutineIdentity);owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto changed=source;auto at=changed.find("using promise_type=NativeStandardCoroutines::Promise<long>;");assert(at!=std::string::npos);
    changed.insert(at,"static constexpr int marker=42; ");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto private_traits=source;
    const std::string wide_promise="using promise_type=NativeStandardCoroutines::Promise<long>;";
    at=private_traits.find(wide_promise);assert(at!=std::string::npos);
    private_traits.replace(at,wide_promise.size(),"using promise_type=NativePrivate::WidePromise;");
    at=private_traits.find("namespace std {");assert(at!=std::string::npos);
    private_traits.insert(at,"namespace NativePrivate {using WidePromise=NativeStandardCoroutines::Promise<long>;}\n");
    auto denied_traits=emit(private_traits);
    assert(!denied_traits.success && std::any_of(denied_traits.diagnostics.begin(),denied_traits.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
    auto private_type=emit("namespace NativePrivate {struct Promise {};}\n"+source+
        "namespace NativeStandardCoroutines {using PrivateHandle=std::coroutine_handle<NativePrivate::Promise>;}\n");
    assert(!private_type.success && std::any_of(private_type.diagnostics.begin(),private_type.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
    auto spoofed=emit("namespace std {template<class T=void>struct coroutine_handle {void *frame;};} namespace NativeStandardCoroutines {std::coroutine_handle<> handle();}");
    assert(!spoofed.success && std::any_of(spoofed.diagnostics.begin(),spoofed.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}

void TestCustomCoroutineBodies() {
  const auto header=std::filesystem::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/custom_coroutines.h";
  std::ifstream input(header);assert(input);
  const std::string source((std::istreambuf_iterator<char>(input)),{});
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &text,const char *filename="custom-coroutines.mm",
                  const std::vector<std::string> &exports=std::vector<std::string>{"NativeCustomCoroutines"}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR},filename);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.custom_coroutines","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.custom_coroutines","test",{},{},{},exports);
    };
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    assert(first.source_bytes.find("#include <coroutine>")!=std::string::npos);
    assert(first.source_bytes.find("co_yield")!=std::string::npos);
    assert(first.source_bytes.find("co_await")!=std::string::npos);
    assert(first.source_bytes.find("__coro_frame")==std::string::npos);
    auto replay=emit(first.source_bytes);
    for(const auto &d:replay.diagnostics)std::fprintf(stderr,"REPLAY %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    assert(replay.source_bytes==first.source_bytes);
    auto relocated=emit("\n\n"+source,"relocated-custom-coroutines.mm");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::any_of(decoded->declarations.begin(),decoded->declarations.end(),HasCxxCoroutineBodyIdentity));
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxCoroutineBodySemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxCoroutineBodyIdentity);
    owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto changed=source;auto at=changed.find("error=73");assert(at!=std::string::npos);changed.replace(at,8,"error=74");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    // Ordinary lookup can select a global await operator absent from the
    // written body. Export policy applies to that resolved customization too.
    auto private_awaiter=source;
    const std::string declaration="inline NumberAwaiter operator co_await(Token token) {return {token.value};}";
    at=private_awaiter.find(declaration);assert(at!=std::string::npos);
    private_awaiter.replace(at,declaration.size(),
        "} NativeCustomCoroutines::NumberAwaiter operator co_await(NativeCustomCoroutines::Token); namespace NativeCustomCoroutines { using ::operator co_await;");
    auto excluded_awaiter=emit(private_awaiter);assert(!excluded_awaiter.success);
    assert(std::any_of(excluded_awaiter.diagnostics.begin(),excluded_awaiter.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
    auto spoofed_std=source;
    at=spoofed_std.find(declaration);assert(at!=std::string::npos);
    spoofed_std.replace(at,declaration.size(),
        "} namespace std {NativeCustomCoroutines::NumberAwaiter operator co_await(NativeCustomCoroutines::Token);} namespace NativeCustomCoroutines { using std::operator co_await;");
    auto excluded_std=emit(spoofed_std);assert(!excluded_std.success);
    assert(std::any_of(excluded_std.diagnostics.begin(),excluded_std.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
    // Promise lookup must not launder a private customization point merely
    // because its invocation was synthesized by coroutine lowering.
    auto private_source=source;
    at=private_source.find("void unhandled_exception(){error=73;}");assert(at!=std::string::npos);
    private_source.replace(at,std::string("void unhandled_exception(){error=73;}").size(),
        "void unhandled_exception(){error=NativePrivate::failure();}");
    private_source="namespace NativePrivate {int failure();}\n"+private_source;
    auto denied=emit(private_source);assert(!denied.success);
    assert(std::any_of(denied.diagnostics.begin(),denied.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}

void TestForeignImportAttributes() {
  auto emit=[](const std::string &source,const char *file="foreign-attributes.mm") {
    auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
        {"-x","objective-c++","-std=c++20","-fblocks","-fobjc-version=3","-fobjc-runtime=gnustep-2.2"},file);
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.foreign","test",{}));
    return EmitSemanticInterface(ast->getASTContext(),"fixture.foreign","test");
  };
  const std::string source=R"O3SWIFT(
#ifndef NATIVE_FOREIGN_IMPORT_ATTRIBUTES_H
#define NATIVE_FOREIGN_IMPORT_ATTRIBUTES_H
namespace NativeForeignAttributes {
struct __attribute__((swift_bridge("Interop.Value"))) Value {int value;};
typedef int Handle __attribute__((swift_newtype(struct)));
int read(Value value) __attribute__((swift_name("Interop.read(_:)")));
int read(Value value){return value.value;}
inline int increment(int value) __attribute__((swift_name("Interop.increment(_:)"),swift_attr("@Sendable"))){return value+1;}
inline int hidden(int value) __attribute__((swift_private)){return value;}
}
#endif

    void complete(int value,void (^callback)(int))
        __attribute__((swift_async(not_swift_private,2),swift_async_name("Interop.complete(_:)")));
  )O3SWIFT";
  auto first=emit(source);
  for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(first.success && first.source_bytes.find("swift_name")!=std::string::npos);
  auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
  assert(replay.source_bytes==first.source_bytes);
  {
    const std::vector<std::string> args={"-x","objective-c++","-std=c++20","-fblocks","-fobjc-version=3","-fobjc-runtime=gnustep-2.2"};
    auto from=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes,args,"foreign-import-from.mm");
    auto to=clang::tooling::buildASTFromCodeWithArgs("",args,"foreign-import-to.mm");
    assert(from && to);
    struct Functions:clang::RecursiveASTVisitor<Functions> {
      std::vector<clang::FunctionDecl *> declarations;
      bool VisitFunctionDecl(clang::FunctionDecl *function) {
        if(!function->isImplicit())declarations.push_back(function);return true;
      }
    } functions;
    functions.TraverseDecl(from->getASTContext().getTranslationUnitDecl());
    clang::ASTImporter importer(to->getASTContext(),to->getFileManager(),from->getASTContext(),from->getFileManager(),false);
    for(auto *function:functions.declarations) {
      auto imported=importer.Import(function);assert(imported);
      InterfaceDeclarationIdentity original,merged;
      assert(ReadInterfaceDeclarationIdentity(function,from->getASTContext(),original));
      assert(ReadInterfaceDeclarationIdentity(llvm::cast<clang::NamedDecl>(*imported),to->getASTContext(),merged));
      assert(original.canonical_type_identity==merged.canonical_type_identity);
    }
  }
  auto relocated=emit("\n\n"+source,"relocated-foreign.mm");
  assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
  auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
  std::vector<llvm::StringRef> semantics;
  for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
  auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
  assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasForeignImportAttributeIdentity)>=5);
  auto missing=*decoded;std::erase(missing.required_semantics,kForeignImportAttributeSemantic);
  auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
  auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasForeignImportAttributeIdentity);
  assert(owner!=wrong.declarations.end());owner->kind="Namespace";
  auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
  auto changed=source;const auto at=changed.find("Interop.increment");assert(at!=std::string::npos);
  changed.replace(at,std::string("Interop.increment").size(),"Interop.advance");
  auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  // The metadata contract is shared by native C headers and Objective-C
  // declarations, rather than depending on C++ namespace publication.
  for(const char *language:{"c","objective-c"}) {
    std::vector<std::string> args={"-x",language,"-std=c17","-fblocks"};
    const bool objc=std::string_view(language)=="objective-c";
    if(objc)args.insert(args.end(),{"-fobjc-version=3","-fobjc-runtime=gnustep-2.2"});
    auto emit_c=[&](const std::string &code,const char *file) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(code,args,file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.foreign_c","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.foreign_c","test");
    };
    std::string code=R"FOREIGN(
      typedef int ForeignHandle __attribute__((swift_newtype(enum)));
      void complete(int value,void (^callback)(int))
        __attribute__((swift_async(not_swift_private,2),swift_async_error(zero_argument,1)));
    )FOREIGN";
    if(objc)code+=R"FOREIGN(
      __attribute__((objc_root_class,swift_name("ForeignOwner"),swift_objc_members))
      @interface ForeignOwner
      - (int)read:(int)value __attribute__((swift_name("read(_:)"),swift_attr("@available(*, message: \"quoted\")")));
      @end
    )FOREIGN";
    auto original=emit_c(code,"foreign.h");
    for(const auto &d:original.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(original.success);
    auto repeated=emit_c(original.source_bytes,"replayed-foreign.h");
    assert(repeated.success && repeated.identity.semantic_fingerprint==original.identity.semantic_fingerprint);
    assert(repeated.source_bytes==original.source_bytes);
    auto moved=emit_c("\n"+code,"relocated-foreign.h");
    assert(moved.success && moved.identity.semantic_fingerprint==original.identity.semantic_fingerprint);
  }
}

void TestNestedFactoryTypes() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="nested-factory-types.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.nested_factory_types","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.nested_factory_types","test",{},{},{},exports);
    };
    const std::string source=R"(
      namespace NativeClosureFactories {
        struct State {int value;State(int n):value(n){};State(const State &)=default;State(State &&other):value(other.value){other.value=0;}~State(){}};
      }
#ifndef NATIVE_NESTED_FACTORY_TYPES_H
#define NATIVE_NESTED_FACTORY_TYPES_H
namespace NativeNestedFactoryTypes {
inline auto factory(int n){
  struct Root {
    struct Left {struct Value {int value;};};
    struct Right {struct Value {int value;};};
    enum class Code:int {good=42};
    union Choice {int number;double real;};
    Left::Value value;
  };
  return Root{{n}};
}
using Root=decltype(factory(0));using Left=Root::Left::Value;using Right=Root::Right::Value;
using Code=Root::Code;using Choice=Root::Choice;
template<class T> auto family(T n){struct Root {struct Leaf {T value;};Leaf value;};return Root{{n}};}
using Integer=decltype(family(0))::Leaf;using Wide=decltype(family(0L))::Leaf;
inline auto closure(int base){return [base](int n){struct Root{struct Leaf{int value;};Leaf value;};return Root{{base+n}};};}
using Closure=decltype(closure(0)(0))::Leaf;
inline auto owned(int n){struct Root{struct Leaf{NativeClosureFactories::State value;};Leaf value;};return Root{{NativeClosureFactories::State(n)}};}
using Owned=decltype(owned(0))::Leaf;
template<class T> struct Box{T value;};using Wrapped=Box<Left>;
struct Holder{Left left;Right right;};
inline int read(Left value){return value.value;}
inline int read(Right value){return value.value;}
inline Left shared{42};
int provider_read(Left value){return value.value;}
Left *provider_shared(){return &shared;}
Owned provider_owned(int n){return {NativeClosureFactories::State(n)};}
template<class A,class B> struct Pair {A first;B second;};
inline auto aggregate(int n){struct A{int value;};struct B{int value;};return Pair<A,B>{{n},{2}};}
using Aggregate=decltype(aggregate(0));
using AggregateFirst=decltype(aggregate(0).first);using AggregateSecond=decltype(aggregate(0).second);
template<class T> auto wrapped(T n){struct Local{T value;};return Box<Local>{{n}};}
using WrappedInteger=decltype(wrapped(0));using WrappedWide=decltype(wrapped(0L));
inline auto pointer(int n){struct Local{int value;};static Local value{42};value.value=n;return &value;}
inline decltype(auto) reference(){struct Local{int value;};static Local value{42};return (value);}
using Pointer=decltype(pointer(0));using Reference=decltype(reference());
inline auto callback(){struct Local{int value;};return Pair{+[](Local value){return value.value;},Local{42}};}
using Callback=decltype(callback());using CallbackValue=decltype(callback().second);
inline auto member_factory(int base){
  struct Root {
    int base;
    auto make(int n) const {struct Local{int value;};return Local{base+n};}
    auto make(long n) const {struct Local{long value;};return Local{base+n};}
    auto second(int n) const {struct Local{int value;};return Local{base+n};}
    auto nested(int n) const {struct Local{struct Leaf{int value;};Leaf value;};return Local{{base+n}};}
  };
  return Root{base};
}
using Member=decltype(member_factory(0).make(0));using MemberWide=decltype(member_factory(0).make(0L));
using MemberSecond=decltype(member_factory(0).second(0));using MemberLeaf=decltype(member_factory(0).nested(0))::Leaf;
inline int read(Member value){return value.value;}
inline int read(MemberSecond value){return value.value;}
Aggregate provider_aggregate(int n){return aggregate(n);}
Callback provider_callback(){return callback();}
Pointer provider_pointer(int n){return pointer(n);}
Reference provider_reference(){return reference();}
Member provider_member(int n){return member_factory(40).make(n);}

}
#endif
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-closure-result.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto find=[&](llvm::StringRef name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(HasCxxNestedFactoryTypeIdentity(find("NativeNestedFactoryTypes::Left")));
    assert(HasCxxNestedFactoryTypeIdentity(find("NativeNestedFactoryTypes::Choice")));
    assert(find("NativeNestedFactoryTypes::Left").canonical_type_identity!=find("NativeNestedFactoryTypes::Right").canonical_type_identity);
    assert(find("NativeNestedFactoryTypes::Integer").canonical_type_identity!=find("NativeNestedFactoryTypes::Wide").canonical_type_identity);
    for(const auto *name:{"Aggregate","AggregateFirst","AggregateSecond","WrappedInteger","Pointer","Reference","Callback","Member","MemberWide","MemberSecond","MemberLeaf"})
      assert(HasCxxNestedFactoryTypeIdentity(find(std::string("NativeNestedFactoryTypes::")+name)));
    assert(find("NativeNestedFactoryTypes::AggregateFirst").canonical_type_identity!=find("NativeNestedFactoryTypes::AggregateSecond").canonical_type_identity);
    assert(find("NativeNestedFactoryTypes::Member").canonical_type_identity!=find("NativeNestedFactoryTypes::MemberSecond").canonical_type_identity);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxNestedFactoryTypeSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxNestedFactoryTypeIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto excluded=emit("inline auto hidden(){struct Root{struct Leaf{int value;};};return Root{};} using Public=decltype(hidden())::Leaf;", "excluded-closure-result.cpp",{"Public"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}

void TestLambdaFactoryResults() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="lambda-factory-results.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.lambda_factory_results","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.lambda_factory_results","test",{},{},{},exports);
    };
    const std::string source=R"(
      namespace NativeClosureFactories {
        struct State {int value;State(int n):value(n){};State(const State &)=default;State(State &&other):value(other.value){other.value=0;}~State(){}};
      }
#ifndef NATIVE_LAMBDA_FACTORY_RESULTS_H
#define NATIVE_LAMBDA_FACTORY_RESULTS_H
namespace NativeLambdaFactoryResults {
inline auto first(int base){return [base](int n){struct Local{int value;};return Local{base+n};};}
inline auto second(int base){return [base](int n){struct Local{int value;};return Local{base+n};};}
inline auto generic(int base){return [base]<class T>(T n){struct Local{T value;};return Local{base+n};};}
template<class T> auto outer(T base){return [base](T n){struct Local{T value;};return Local{base+n};};}
inline auto nested(int base){return [base](int n){return [value=base+n](int delta){struct Local{int value;};return Local{value+delta};};};}
inline auto owned(int n){return [state=NativeClosureFactories::State(n)](int delta){struct Local{NativeClosureFactories::State value;};return Local{NativeClosureFactories::State(state.value+delta)};};}
using First=decltype(first(0)(0));using Second=decltype(second(0)(0));
using Integer=decltype(generic(0)(0));using Wide=decltype(generic(0)(0L));
using Outer=decltype(outer(0)(0));using OuterWide=decltype(outer(0L)(0L));
using Nested=decltype(nested(0)(0)(0));using Owned=decltype(owned(0)(0));
template<class T> struct Box{T value;};using Wrapped=Box<First>;
struct Holder{First first;Second second;};
inline int read(First value){return value.value;}
inline int read(Second value){return value.value;}
inline First shared=first(40)(2);
inline int local(){return []<class T>(T value){return value.value;}(first(40)(2));}
constexpr auto constant(){return [](int n){struct Local{int value;};return Local{n};};}
template<auto V> struct Value{static constexpr int get(){return V.value;}};
using Structural=Value<constant()(42)>;
int provider_read(First value){return value.value;}
First provider_factory(int n){return first(n)(2);}
First *provider_shared(){return &shared;}
Owned provider_owned(int n){return owned(n)(2);}
}
#endif
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-closure-result.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto find=[&](llvm::StringRef name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(HasCxxLambdaFactoryResultIdentity(find("NativeLambdaFactoryResults::First")));
    assert(HasCxxLambdaFactoryResultIdentity(find("NativeLambdaFactoryResults::local")));
    assert(find("NativeLambdaFactoryResults::First").canonical_type_identity!=find("NativeLambdaFactoryResults::Second").canonical_type_identity);
    assert(find("NativeLambdaFactoryResults::Integer").canonical_type_identity!=find("NativeLambdaFactoryResults::Wide").canonical_type_identity);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxLambdaFactoryResultSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxLambdaFactoryResultIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto excluded=emit("inline auto hidden(){return [](int n){struct Local{int value;};return Local{n};};} using Public=decltype(hidden()(42));", "excluded-closure-result.cpp",{"Public"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}

void TestClosureResultProjections() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="closure-result-projections.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.closure_result_projections","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.closure_result_projections","test",{},{},{},exports);
    };
    const std::string source=R"(
      namespace NativeClosureFactories {
        struct State {int value;State(int n):value(n){};State(const State &)=default;State(State &&other):value(other.value){other.value=0;}~State(){}};
      }
#ifndef NATIVE_CLOSURE_RESULT_PROJECTIONS_H
#define NATIVE_CLOSURE_RESULT_PROJECTIONS_H
namespace NativeClosureResultProjections {
inline auto first(int n){return [n](int delta){return n+delta;};}
inline auto second(int n){return [n](int delta){return n+delta;};}
inline auto generic(int n){return [n]<class T>(T delta){return n+delta;};}
template<class T> auto factory(T n){return [n](T delta){return n+delta;};}
inline auto owned_factory(NativeClosureFactories::State value){return [copy=static_cast<NativeClosureFactories::State &&>(value)](int delta){return copy.value+delta;};}
using First=decltype(first(0));using Second=decltype(second(0));
using Generic=decltype(generic(0));using Integer=decltype(factory(0));using Wide=decltype(factory(0L));
using Owned=decltype(NativeClosureResultProjections::owned_factory(NativeClosureFactories::State(0)));
template<class T> struct Box {T value;int get(){return value(2);}};
using Wrapped=Box<First>;
struct Holder {First first;Second second;int get(){return first(0)+second(0);}};
inline int read(First value){return value(2);}
inline int read(Second value){return value(2);}
inline First shared=first(40);
inline int local(){return []<class T>(T call){return call(2);}(first(40));}
constexpr auto stateless(){return [](int n){return n+2;};}
template<auto Call> int invoke(){return Call(40);}
inline int structural(){return invoke<stateless()>();}
int provider_read(First value){return value(2);}
First provider_factory(int n){return first(n);}
First *provider_shared(){return &shared;}
Owned provider_owned(int n){return NativeClosureResultProjections::owned_factory(NativeClosureFactories::State(n));}
}
#endif

    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-closure-result.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto find=[&](llvm::StringRef name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(HasCxxClosureResultProjectionIdentity(find("NativeClosureResultProjections::First")));
    assert(HasCxxClosureResultProjectionIdentity(find("NativeClosureResultProjections::local")));
    assert(find("NativeClosureResultProjections::First").canonical_type_identity!=find("NativeClosureResultProjections::Second").canonical_type_identity);
    assert(find("NativeClosureResultProjections::Integer").canonical_type_identity!=find("NativeClosureResultProjections::Wide").canonical_type_identity);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxClosureResultProjectionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxClosureResultProjectionIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto excluded=emit("inline auto hidden(){return [](int n){return n;};} using Public=decltype(hidden());", "excluded-closure-result.cpp",{"Public"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}

void TestFactoryValueArguments() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="factory-value-arguments.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.factory_value_arguments","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.factory_value_arguments","test",{},{},{},exports);
    };
    const std::string source=R"(
#ifndef NATIVE_FACTORY_VALUE_ARGUMENTS_H
#define NATIVE_FACTORY_VALUE_ARGUMENTS_H
namespace NativeFactoryValueArguments {
constexpr auto first(int n){struct Local {int value;};return Local{n};}
constexpr auto second(int n){struct Local {int value;};return Local{n};}
template<class T> constexpr auto generic(T n){struct Local {T value;};return Local{n};}
template<auto V> struct Box {static constexpr int get(){return V.value;}};
template<> struct Box<first(42)> {static constexpr int get(){return 42;}};
template<auto V> requires(V.value==40) struct Box<V> {static constexpr int get(){return V.value;}};
template<auto... V> struct Pack {static constexpr int get(){return (0+...+V.value);}};
template<auto V> using Alias=Box<V>;
using First=Box<first(42)>;using Other=Box<second(42)>;
using Partial=Box<first(40)>;using Generic=Box<generic(42L)>;
using Packed=Pack<first(40),second(2)>;using Aliased=Alias<first(42)>;
template<auto V> inline int slot=V.value;
template<> inline int slot<first(42)> = 42;
template<auto V> int read(){return V.value;}
template<> int read<first(42)>(){return 42;}
inline int local(){return []<auto V>(){return V.value;}.template operator()<first(42)>();}
int provider_value(First){return First::get();}
int *provider_slot(){return &slot<first(42)>;}
}
#endif

      constexpr auto array_value(int n){struct Local {int values[2];};return Local{{n,2}};}
      template<auto V> struct ArrayBox {static constexpr int get(){return V.values[0]+V.values[1];}};
      using Array=ArrayBox<array_value(40)>;
      inline int target=42;
      constexpr auto address(){struct Local{int *value;};return Local{&target};}
      template<auto V> struct PointerBox {static int get(){return *V.value;}};
      using Pointer=PointerBox<address()>;
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-factory-value.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto find=[&](llvm::StringRef name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(HasCxxFactoryValueArgumentIdentity(find("NativeFactoryValueArguments::First")));
    assert(HasCxxFactoryValueArgumentIdentity(find("NativeFactoryValueArguments::local")));
    assert(find("NativeFactoryValueArguments::First").canonical_type_identity!=find("NativeFactoryValueArguments::Other").canonical_type_identity);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxFactoryValueArgumentSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxFactoryValueArgumentIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto excluded=emit("constexpr auto hidden(){struct Local{int value;};return Local{42};} template<auto>struct Box{};using Public=Box<hidden()>;", "excluded-factory-value.cpp",{"Box","Public"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
    auto changed=source;auto position=changed.find("Pack<first(40),second(2)>");assert(position!=std::string::npos);
    changed.replace(position,25,"Pack<first(40),second(3)>");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  }
}

void TestGenericFactoryProjections() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="generic-factory-projections.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.generic_factory_projections","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.generic_factory_projections","test",{},{},{},exports);
    };
    const std::string source=R"(
      template<class T> auto make(T n){struct Local {T value;};return Local{n};}
      template<class T> struct Family {auto make(T n) const {struct Local {T value;};return Local{n};}};
      struct Concrete {template<class T> auto make(T n) const {struct Local {T value;};return Local{n};}};
      using Integer=decltype(make(0));using Wide=decltype(make(0L));
      using Member=decltype(Family<int>{}.make(0));using MemberWide=decltype(Family<long>{}.make(0L));
      using Method=decltype(Concrete{}.make(0));
      template<class T> struct Box {T value;};using Wrapped=Box<Integer>;
      struct Holder {Integer first;Member second;Method third;};
      inline int read(Integer value){return value.value;}
      inline int read(Wide value){return value.value;}
      inline Integer shared=make(40);
      inline int local(){return []<class T>(T value){return value.value;}(make(42));}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-generic-factory-projections.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto find=[&](llvm::StringRef name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(HasCxxGenericFactoryProjectionIdentity(find("Integer")) && HasCxxGenericFactoryProjectionIdentity(find("Member")));
    assert(HasCxxGenericFactoryProjectionIdentity(find("local")));
    assert(find("Integer").canonical_type_identity!=find("Wide").canonical_type_identity);
    assert(find("Member").canonical_type_identity!=find("MemberWide").canonical_type_identity);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxGenericFactoryProjectionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxGenericFactoryProjectionIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto excluded=emit("template<class T> auto hidden(T n){struct Local{T value;};return Local{n};} using Public=decltype(hidden(0));", "excluded-generic-factory.cpp",{"Public"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}

void TestFactoryTemplateArguments() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="factory-template-arguments.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.factory_template_arguments","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.factory_template_arguments","test",{},{},{},exports);
    };
    const std::string source=R"(
      inline auto first(int n){struct {int value;} result{n};return result;}
      inline auto second(int n){struct {int value;} result{n};return result;}
      using First=decltype(first(0));using Second=decltype(second(0));
      template<class T> struct Box {T value;};
      template<> struct Box<First> {First value;int get() const{return value.value;}};
      template<class T> struct Box<T *> {T *value;};
      template<class T> struct Direct {First value;};using DirectBox=Direct<int>;
      template<class... T> struct Pack {static constexpr int count=sizeof...(T);};
      template<class T> using Alias=Box<T>;
      using Closed=Box<First>;using Other=Box<Second>;using Nested=Box<Box<First>>;
      using Packed=Pack<First,Second,First *>;using Aliased=Alias<First>;
      using Pointer=First *;using PointerBox=Box<Pointer>;
      using Array=First[2];using ArrayBox=Box<Array>;
      using Member=decltype(&First::value);using MemberBox=Box<Member>;
      template<class T> inline int slot=0;
      template<> inline int slot<First> = 42;
      template<class T> int read(T value){return value.value;}
      template<> int read<First>(First value){return value.value;}
      inline int deduced(First value){return read(value);}
      inline int closed_lambda(int n){auto call=[]<class T>(T value){return value.value;};return call(first(n))+call(second(0));}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-factory-template-arguments.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto scoped=emit("namespace Public {"+source+"}");assert(scoped.success);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto find=[&](llvm::StringRef name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(HasCxxFactoryTemplateArgumentIdentity(find("Closed")) && HasCxxFactoryTemplateArgumentIdentity(find("Nested")));
    assert(HasCxxFactoryTemplateArgumentIdentity(find("closed_lambda")) && HasCxxFactoryTemplateArgumentIdentity(find("deduced")));
    assert(find("Closed").canonical_type_identity!=find("Other").canonical_type_identity);
    auto changed=source;auto field=changed.find("First value;int get()");assert(field!=std::string::npos);
    changed.replace(field,std::string("First value").size(),"Second value");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxFactoryTemplateArgumentSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxFactoryTemplateArgumentIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto excluded=emit("inline auto secret(){struct Local{int value;};return Local{42};} using Value=decltype(secret());template<class T>struct Box{T value;};using Public=Box<Value>;", "excluded-factory-template.cpp",{"Box","Public"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
    auto mixed=emit("inline auto first(){struct Local{int value;};return Local{42};} inline int bad(){struct Private{int value;};auto call=[]<class... T>(T...){return sizeof...(T);};return call(first(),Private{42});}");
    assert(!mixed.success);
  }
}

void TestFactoryTypeProjections() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="factory-type-projections.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","objective-c++",standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
           "--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.factory_type_projections","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.factory_type_projections","test",{},{},{},exports);
    };
    const std::string source=R"(
      inline auto first(int n){struct {int value;} result{n};return result;}
      inline auto second(int n){struct {int value;} result{n};return result;}
      using First=decltype(first(0));using Second=decltype(second(0));
      using Reader=int (*)(First,Second);
      using ReverseReader=int (*)(Second,First);
      struct Holder {First first;Second second;int get() const {return first.value+second.value;}};
      struct Derived:First {int get() const {return value;}};
      inline First shared=first(40);
      inline int read(First a,Second b){return a.value+b.value;}
      inline int read(Second a,First b){return a.value-b.value;}
      inline int pointer(const First *value){return value->value;}
      inline First &reference(){return shared;}
      struct Owner {auto make(int n) const {struct Local {int value;};return Local{n};}};
      using Member=decltype(Owner{}.make(0));
      inline int member(Member value){return value.value;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-factory-type-projections.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto scoped=emit("namespace Public {"+source+"}");assert(scoped.success);
    auto scoped_replay=emit(scoped.source_bytes);assert(scoped_replay.success && scoped_replay.identity.semantic_fingerprint==scoped.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto find=[&](llvm::StringRef name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(decoded->declarations.begin(),decoded->declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=decoded->declarations.end());return *row;
    };
    assert(find("First").canonical_type_identity!=find("Second").canonical_type_identity);
    assert(find("Reader").canonical_type_identity!=find("ReverseReader").canonical_type_identity);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxFactoryTypeProjectionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxFactoryTypeProjectionIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    const std::string generic_owner="template<class T> struct Family {auto make(T n) {struct Local {T value;};return Local{n};}};";
    assert(emit(generic_owner).success);
    auto generic_origin=emit(generic_owner+"using Public=decltype(Family<int>{}.make(42));");
    assert(generic_origin.success && generic_origin.sidecar_json.find(kCxxGenericFactoryProjectionSemantic)!=std::string::npos);
    auto closure_origin=emit("inline auto outer(){return [](int n){struct Local{int value;};return Local{n};};} using Public=decltype(outer()(42));");
    assert(closure_origin.success && closure_origin.sidecar_json.find(kCxxLambdaFactoryResultSemantic)!=std::string::npos);
    auto excluded=emit("inline auto secret(){struct Local{int value;};return Local{42};} using Public=decltype(secret());", "excluded-factory.cpp",{"Public"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(),excluded.diagnostics.end(),[](const auto &d){return d.code=="O3IFC156";}));
  }
}

void TestLocalFactoryResults() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="local-factory-results.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","c++",standard,"--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.local_factory_results","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.local_factory_results","test");
    };
    const std::string source=R"(
      inline auto named(int n) {struct Local {int value;int get() const {return value;}};return Local{n};}
      inline decltype(auto) unnamed(int n) {struct {int value;int get() const {return value;}} result{n};return result;}
      inline auto enumeration() {enum class Local {answer=42};return Local::answer;}
      inline auto anonymous_enum() {enum {answer=42};return answer;}
      inline auto union_value(int n) {union Local {int value;double other;};return Local{n};}
      inline auto nested(int n) {struct Outer {struct Value {int value;};};return Outer::Value{n};}
      inline auto closure() {return [](int n){struct Local {int value;};return Local{n};};}
      template<class T> auto pattern(T n) {struct Local {T value;T get() const {return value;}};return Local{n};}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-local-factory-results.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxLocalFactoryResultIdentity)==8);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxLocalFactoryResultSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxLocalFactoryResultIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto changed=source;changed.replace(changed.find("return value;"),13,"return value+1;");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto secret=emit("static int hidden(); inline auto api(){struct Local {int get() const{return hidden();}};return Local{};}");
    assert(!secret.success && std::any_of(secret.diagnostics.begin(),secret.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    auto indirect=emit("inline auto pointer(){static struct Local {int value;} storage;return &storage;}");
    assert(indirect.success);
    auto indirect_replay=emit(indirect.source_bytes);
    assert(indirect_replay.success && indirect_replay.identity.semantic_fingerprint==indirect.identity.semantic_fingerprint);
  }
}

void TestClosureFactories() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="closure-factories.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x","c++",standard,"--target=x86_64-unknown-linux-gnu"},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.closure_factories","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.closure_factories","test");
    };
    const std::string source=R"(
      struct Owned {
        int value;
        explicit Owned(int n):value(n) {}
        Owned(const Owned &other):value(other.value) {}
        ~Owned() {}
        auto factory() const {return [copy=*this](int n) {return copy.value+n;};}
        auto generic() const {return [copy=*this]<class T>(T n) {return copy.value+n;};}
      };
      inline auto concrete(int base) {return [base](int n) {return base+n;};}
      inline auto generic(int base) {return [base]<class T>(T n) {return base+n;};}
      inline decltype(auto) mutable_factory(int base) {return [base](int n) mutable {base+=n;return base;};}
      inline auto reference_factory(int &base) {return [&base]<class T>(T n) {base+=n;return base;};}
      inline auto nested_factory(int base) {return [base](int n) {return [value=base+n](int delta) {return value+delta;};};}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto replay=emit(first.source_bytes);assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-closure-factories.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxClosureFactoryIdentity)>=7);
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxClosureFactorySemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto owner=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxClosureFactoryIdentity);
    assert(owner!=wrong.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto changed=source;changed.replace(changed.find("base+n"),6,"base+n+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto secret=emit("static int hidden(int n){return n;} inline auto api(){return [](int n){return hidden(n);};}");
    assert(!secret.success && std::any_of(secret.diagnostics.begin(),secret.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
  }
}

void TestAutomaticOwnerUnwindReplay() {
  for(const char *standard:{"c17","c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedOrdinaryBytes;
        __attribute__((objc_root_class)) @interface PublishedOrdinaryOwner {
        @public char payload;
        }
        @end
        void published_ordinary_use(const char *view __attribute__((noescape)));
        inline PublishedOrdinaryBytes published_ordinary_bytes(PublishedOrdinaryOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        typedef void (^PublishedOrdinaryWork)(void);
        inline void published_ordinary_capture(PublishedOrdinaryOwner *_Nonnull owner) {
          PublishedOrdinaryOwner *_Nonnull local=owner;
          const char *view=&local->payload;
          PublishedOrdinaryWork first=^{(void)local;published_ordinary_use(view);};
          PublishedOrdinaryWork second=first;
          second();
        }
        PublishedOrdinaryOwner *_Nonnull published_ordinary_factory(void);
        void published_ordinary_raise(void);
        inline void published_ordinary_body(PublishedOrdinaryOwner *_Nonnull owner,int mode) {
          const char *outer=published_ordinary_bytes(owner);
          @try {
            PublishedOrdinaryOwner *_Nonnull local=published_ordinary_factory();
            const char *view=published_ordinary_bytes(local);
            published_ordinary_use(view);
            if(mode)published_ordinary_raise();
          } @catch(...) {published_ordinary_use(outer);}
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.automatic_owner_unwind","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.automatic_owner_unwind","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"ordinary-provider.m");
      auto replay=emit(first.source_bytes,"ordinary-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-ordinary.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kAutomaticOwnerUnwindSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kAutomaticOwnerUnwindSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kAutomaticOwnerUnwindSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("automatic owner unwind identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kAutomaticOwnerUnwindSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void ordinary_consumer(PublishedOrdinaryOwner *_Nonnull owner) {published_ordinary_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void ordinary_expired(PublishedOrdinaryOwner *_Nonnull owner) {
          const char *view=0;
          @try {
            PublishedOrdinaryOwner *_Nonnull local=published_ordinary_factory();
            view=published_ordinary_bytes(local);published_ordinary_raise();
          } @catch(...) {published_ordinary_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());

    }
}


void TestBlockMutationPackageReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    const bool cxx=std::string(standard)!="c17";
    std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",std::string("-std=")+standard,
        "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system","-fblocks",
        "-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    std::ifstream input(std::filesystem::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/block_values.h");assert(input);
    const std::string source((std::istreambuf_iterator<char>(input)),std::istreambuf_iterator<char>());
    auto emit=[&](const std::string &text,const char *name) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.block_mutation_package","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.block_mutation_package","test");
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source,"computed-block-provider.m");auto replay=emit(first.source_bytes,"computed-block-replay.m");
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"computed-block-relocated.m");assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
    assert(llvm::is_contained(decoded->required_semantics,std::string(kAutomaticOwnerUnwindSemantic)));
    assert(llvm::is_contained(decoded->required_semantics,std::string(kNativeSubobjectSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kAutomaticOwnerUnwindSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto missing=*decoded;std::erase(missing.required_semantics,kAutomaticOwnerUnwindSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto plain=emit("void plain_computed_contract(void);","plain-computed.m");
    auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);invented->required_semantics.push_back(kAutomaticOwnerUnwindSemantic);
    auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);llvm::consumeError(unexpected.takeError());
  }
}

void TestBlockProducerTransferReplay() {
  for(const char *standard:{"c17","c++20","c++23"})for(bool arc:{false,true}) {
    const bool cxx=std::string(standard)!="c17";
    std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",std::string("-std=")+standard,
        "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system","-fblocks",
        "-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      typedef void (^PublishedProducerCallback)(void);
      struct PublishedProducerSlots {PublishedProducerCallback field,array[2];};
      PublishedProducerCallback published_producer_owned(int) __attribute__((ns_returns_retained));
      PublishedProducerCallback published_producer_shared(int);
      int published_producer_index(void);
      void published_producer_checkpoint(void) __attribute__((nothrow));
      void published_producer_consume(PublishedProducerCallback value __attribute__((ns_consumed))) __attribute__((nothrow));
      inline void published_producer_body(struct PublishedProducerSlots *slots) {
        __objc3_defer {published_producer_checkpoint();}
        PublishedProducerCallback first=published_producer_owned(1),second=published_producer_shared(2);
        slots->array[published_producer_index()]=published_producer_owned(3);
        slots->field=published_producer_shared(4);
        published_producer_consume(published_producer_owned(5));published_producer_consume(published_producer_shared(6));
        first();second();
      }
    )";
    auto emit=[&](const std::string &text,const char *name) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.block_producer_transfer","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.block_producer_transfer","test");
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source,"computed-block-provider.m");auto replay=emit(first.source_bytes,"computed-block-replay.m");
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"computed-block-relocated.m");assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
    assert(llvm::is_contained(decoded->required_semantics,std::string(kCxxBlockExpressionSemantic)));
    assert(!llvm::is_contained(decoded->required_semantics,std::string(kNativeSubobjectSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kCxxBlockExpressionSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxBlockExpressionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto plain=emit("void plain_computed_contract(void);","plain-computed.m");
    auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);invented->required_semantics.push_back(kCxxBlockExpressionSemantic);
    auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);llvm::consumeError(unexpected.takeError());
  }
}

void TestBlockMutationReplay() {
  for(const char *standard:{"c17","c++20","c++23"})for(bool arc:{false,true}) {
    const bool cxx=std::string(standard)!="c17";
    std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",std::string("-std=")+standard,
        "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system","-fblocks",
        "-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      typedef void (^PublishedComputedCallback)(void);
      struct PublishedComputedSlots {PublishedComputedCallback field,array[2];};
      int published_computed_step(void);
      void published_computed_checkpoint(void) __attribute__((nothrow));
      void published_computed_consume(PublishedComputedCallback value __attribute__((ns_consumed))) __attribute__((nothrow));
      inline void published_computed_body(struct PublishedComputedSlots *slots,int flag,int index) {
        __objc3_defer {published_computed_checkpoint();}
        PublishedComputedCallback local=(published_computed_step(),flag?slots->field:slots->array[index]);
        published_computed_consume(flag?local:slots->array[index]);local();slots->array[index]();
        slots->array[published_computed_step()]=(published_computed_step(),flag?local:slots->field);
        local=slots->array[index];slots->field=(PublishedComputedCallback)0;
      }
    )";
    auto emit=[&](const std::string &text,const char *name) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.block_mutation","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.block_mutation","test");
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source,"computed-block-provider.m");auto replay=emit(first.source_bytes,"computed-block-replay.m");
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"computed-block-relocated.m");assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
    assert(llvm::is_contained(decoded->required_semantics,std::string(kAutomaticOwnerUnwindSemantic)));
    assert(!llvm::is_contained(decoded->required_semantics,std::string(kNativeSubobjectSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kAutomaticOwnerUnwindSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto missing=*decoded;std::erase(missing.required_semantics,kAutomaticOwnerUnwindSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto plain=emit("void plain_computed_contract(void);","plain-computed.m");
    auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);invented->required_semantics.push_back(kAutomaticOwnerUnwindSemantic);
    auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);llvm::consumeError(unexpected.takeError());
  }
}

void TestDependentExpressionCapabilityScan() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
        "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fblocks"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      namespace DependentExpressionScan {
        template<class T> struct Holder {
          T value;
          explicit Holder(T input):value(input) {}
        };
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"dependent-expression-scan.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
          "fixture.dependent_expression_scan","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.dependent_expression_scan","test",{},{},{},
          {"DependentExpressionScan"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    const auto &catalog=SupportedSemanticInterfaceCapabilities();
    std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    for(bool block:{false,true}) {
      auto first=emit(source+(block?R"(
        namespace DependentExpressionScan {
          typedef void (^Callback)(void);
          inline void use(Callback value) {value();}
        }
      )":""));
      auto replay=emit(first.source_bytes);
      assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source+(block?R"(
        namespace DependentExpressionScan {
          typedef void (^Callback)(void);
          inline void use(Callback value) {value();}
        }
      )":""));
      assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      assert(llvm::is_contained(decoded->required_semantics,std::string(kCxxBlockExpressionSemantic))==block);
    }
  }
}

void TestComputedBlockValuesReplay() {
  for(const char *standard:{"c17","c++20","c++23"})for(bool arc:{false,true}) {
    const bool cxx=std::string(standard)!="c17";
    std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",std::string("-std=")+standard,
        "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system","-fblocks",
        "-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      typedef void (^PublishedComputedCallback)(void);
      struct PublishedComputedSlots {PublishedComputedCallback field,array[2];};
      int published_computed_step(void);
      void published_computed_checkpoint(void) __attribute__((nothrow));
      void published_computed_consume(PublishedComputedCallback value __attribute__((ns_consumed))) __attribute__((nothrow));
      inline void published_computed_body(struct PublishedComputedSlots *slots,int flag,int index) {
        __objc3_defer {published_computed_checkpoint();}
        PublishedComputedCallback local=(published_computed_step(),flag?slots->field:slots->array[index]);
        published_computed_consume(flag?local:slots->array[index]);local();slots->array[index]();
      }
    )";
    auto emit=[&](const std::string &text,const char *name) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.computed_block_values","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.computed_block_values","test");
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source,"computed-block-provider.m");auto replay=emit(first.source_bytes,"computed-block-replay.m");
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"computed-block-relocated.m");assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
    assert(llvm::is_contained(decoded->required_semantics,std::string(kCxxBlockExpressionSemantic)));
    assert(!llvm::is_contained(decoded->required_semantics,std::string(kNativeSubobjectSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kCxxBlockExpressionSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxBlockExpressionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto plain=emit("void plain_computed_contract(void);","plain-computed.m");
    auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);invented->required_semantics.push_back(kCxxBlockExpressionSemantic);
    auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);llvm::consumeError(unexpected.takeError());
  }
}

void TestComputedBlockArrayMemberReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedAllocationBytes;
        __attribute__((objc_root_class)) @interface PublishedAllocationOwner {@public char payload;}
        @end
        void published_allocation_use(const char *view __attribute__((noescape)));
        PublishedAllocationOwner *_Nonnull published_subobject_factory(void);
        void published_subobject_raise(void);
        struct PublishedSubobjectBase {explicit PublishedSubobjectBase(int);~PublishedSubobjectBase() noexcept(false);};
        struct PublishedSubobjectMember {explicit PublishedSubobjectMember(int);~PublishedSubobjectMember() noexcept(false);};
        inline PublishedAllocationBytes published_allocation_bytes(PublishedAllocationOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        struct PublishedSubobjectValue:virtual PublishedSubobjectBase {
          PublishedSubobjectMember first,second;
          PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner,int)
              :PublishedSubobjectBase(0),first(1),second(2) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          explicit PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner) try
              :PublishedSubobjectValue(owner,0) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          } catch(...) {published_allocation_use(published_allocation_bytes(owner));}
          ~PublishedSubobjectValue() noexcept(false) try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          } catch(...) {
            PublishedAllocationOwner *_Nonnull handler=published_subobject_factory();
            const char *view=published_allocation_bytes(handler);
            __objc3_defer {published_allocation_use(view);}return;
          }
        };
        struct PublishedVirtualMost:PublishedSubobjectValue {
          explicit PublishedVirtualMost(PublishedAllocationOwner *_Nonnull owner)
              :PublishedSubobjectBase(3),PublishedSubobjectValue(owner) {}
          ~PublishedVirtualMost() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
          }
        };
        struct PublishedAnonymousValue {
          union {PublishedSubobjectMember active;int fallback;};
          explicit PublishedAnonymousValue(PublishedAllocationOwner *_Nonnull owner):active(1) {
            const char *view=published_allocation_bytes(owner);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          ~PublishedAnonymousValue() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
            active.~PublishedSubobjectMember();
          }
        };
        typedef void (^PublishedMemberCallback)(void);
        struct PublishedBlockMembers {
          PublishedMemberCallback first[2][2],second[2][2];
          PublishedBlockMembers(PublishedAllocationOwner *_Nonnull owner,PublishedMemberCallback callback,int flag,int scalar)
              :first{{(published_subobject_raise(),callback),flag?callback:(PublishedMemberCallback)0},
                     {^{(void)scalar;},^{}}},second{{callback},{}} {
            const char *view=published_allocation_bytes(owner);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          ~PublishedBlockMembers() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
        };

        void published_member_consume(PublishedMemberCallback value __attribute__((ns_consumed))) __attribute__((nothrow));
        inline void published_computed_local(PublishedAllocationOwner *_Nonnull owner,PublishedBlockMembers &storage,int flag,int index) {
          const char *view=published_allocation_bytes(owner);__objc3_defer {published_allocation_use(view);}
          PublishedMemberCallback local=(published_subobject_raise(),flag?storage.first[0][index]:storage.second[0][0]);
          published_member_consume(flag?local:storage.first[0][index]);local();storage.first[0][index]();
        }
        inline void published_allocation_body(PublishedAllocationOwner *_Nonnull owner,int mode) {
          withLifetime(owner) {
            const char *view=published_allocation_bytes(owner);
            try {__objc3_defer {published_allocation_use(view);}PublishedVirtualMost value(owner);PublishedAnonymousValue active(owner);(void)value;(void)active;}
            catch(...) {published_allocation_use(view);}
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.computed_block_array_member","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.computed_block_array_member","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeSubobjectSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kNativeSubobjectSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeSubobjectSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native subobject cleanup identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeSubobjectSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedAllocationOwner *_Nonnull owner) {published_allocation_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void subobject_expired(PublishedAllocationOwner *_Nonnull owner) {
          const char *view=0;
          try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();view=published_allocation_bytes(local);
            PublishedSubobjectValue value(owner);(void)value;
          } catch(...) {published_allocation_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());

    }
}

void TestNativeBlockArrayMemberReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedAllocationBytes;
        __attribute__((objc_root_class)) @interface PublishedAllocationOwner {@public char payload;}
        @end
        void published_allocation_use(const char *view __attribute__((noescape)));
        PublishedAllocationOwner *_Nonnull published_subobject_factory(void);
        void published_subobject_raise(void);
        struct PublishedSubobjectBase {explicit PublishedSubobjectBase(int);~PublishedSubobjectBase() noexcept(false);};
        struct PublishedSubobjectMember {explicit PublishedSubobjectMember(int);~PublishedSubobjectMember() noexcept(false);};
        inline PublishedAllocationBytes published_allocation_bytes(PublishedAllocationOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        struct PublishedSubobjectValue:virtual PublishedSubobjectBase {
          PublishedSubobjectMember first,second;
          PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner,int)
              :PublishedSubobjectBase(0),first(1),second(2) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          explicit PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner) try
              :PublishedSubobjectValue(owner,0) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          } catch(...) {published_allocation_use(published_allocation_bytes(owner));}
          ~PublishedSubobjectValue() noexcept(false) try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          } catch(...) {
            PublishedAllocationOwner *_Nonnull handler=published_subobject_factory();
            const char *view=published_allocation_bytes(handler);
            __objc3_defer {published_allocation_use(view);}return;
          }
        };
        struct PublishedVirtualMost:PublishedSubobjectValue {
          explicit PublishedVirtualMost(PublishedAllocationOwner *_Nonnull owner)
              :PublishedSubobjectBase(3),PublishedSubobjectValue(owner) {}
          ~PublishedVirtualMost() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
          }
        };
        struct PublishedAnonymousValue {
          union {PublishedSubobjectMember active;int fallback;};
          explicit PublishedAnonymousValue(PublishedAllocationOwner *_Nonnull owner):active(1) {
            const char *view=published_allocation_bytes(owner);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          ~PublishedAnonymousValue() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
            active.~PublishedSubobjectMember();
          }
        };
        typedef void (^PublishedMemberCallback)(void);
        struct PublishedBlockMembers {
          PublishedMemberCallback first[3],second[2];
          PublishedBlockMembers(PublishedAllocationOwner *_Nonnull owner,PublishedMemberCallback callback)
              :first{callback,callback,callback},second{} {
            const char *view=published_allocation_bytes(owner);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          ~PublishedBlockMembers() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
        };
        inline void published_allocation_body(PublishedAllocationOwner *_Nonnull owner,int mode) {
          withLifetime(owner) {
            const char *view=published_allocation_bytes(owner);
            try {__objc3_defer {published_allocation_use(view);}PublishedVirtualMost value(owner);PublishedAnonymousValue active(owner);(void)value;(void)active;}
            catch(...) {published_allocation_use(view);}
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_block_array_member","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_block_array_member","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeSubobjectSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kNativeSubobjectSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeSubobjectSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native subobject cleanup identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeSubobjectSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedAllocationOwner *_Nonnull owner) {published_allocation_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void subobject_expired(PublishedAllocationOwner *_Nonnull owner) {
          const char *view=0;
          try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();view=published_allocation_bytes(local);
            PublishedSubobjectValue value(owner);(void)value;
          } catch(...) {published_allocation_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());

    }
}

void TestNativeVirtualSubobjectReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedAllocationBytes;
        __attribute__((objc_root_class)) @interface PublishedAllocationOwner {@public char payload;}
        @end
        void published_allocation_use(const char *view __attribute__((noescape)));
        PublishedAllocationOwner *_Nonnull published_subobject_factory(void);
        void published_subobject_raise(void);
        struct PublishedSubobjectBase {explicit PublishedSubobjectBase(int);~PublishedSubobjectBase() noexcept(false);};
        struct PublishedSubobjectMember {explicit PublishedSubobjectMember(int);~PublishedSubobjectMember() noexcept(false);};
        inline PublishedAllocationBytes published_allocation_bytes(PublishedAllocationOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        struct PublishedSubobjectValue:virtual PublishedSubobjectBase {
          PublishedSubobjectMember first,second;
          PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner,int)
              :PublishedSubobjectBase(0),first(1),second(2) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          explicit PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner) try
              :PublishedSubobjectValue(owner,0) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          } catch(...) {published_allocation_use(published_allocation_bytes(owner));}
          ~PublishedSubobjectValue() noexcept(false) try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          } catch(...) {
            PublishedAllocationOwner *_Nonnull handler=published_subobject_factory();
            const char *view=published_allocation_bytes(handler);
            __objc3_defer {published_allocation_use(view);}return;
          }
        };
        struct PublishedVirtualMost:PublishedSubobjectValue {
          explicit PublishedVirtualMost(PublishedAllocationOwner *_Nonnull owner)
              :PublishedSubobjectBase(3),PublishedSubobjectValue(owner) {}
          ~PublishedVirtualMost() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
          }
        };
        struct PublishedAnonymousValue {
          union {PublishedSubobjectMember active;int fallback;};
          explicit PublishedAnonymousValue(PublishedAllocationOwner *_Nonnull owner):active(1) {
            const char *view=published_allocation_bytes(owner);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          ~PublishedAnonymousValue() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
            active.~PublishedSubobjectMember();
          }
        };
        typedef void (^PublishedMemberCallback)(void);
        struct PublishedBlockMembers {
          PublishedMemberCallback first,second;
          PublishedBlockMembers(PublishedAllocationOwner *_Nonnull owner,PublishedMemberCallback callback)
              :first(callback),second(callback) {
            const char *view=published_allocation_bytes(owner);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          ~PublishedBlockMembers() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);__objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
        };
        inline void published_allocation_body(PublishedAllocationOwner *_Nonnull owner,int mode) {
          withLifetime(owner) {
            const char *view=published_allocation_bytes(owner);
            try {__objc3_defer {published_allocation_use(view);}PublishedVirtualMost value(owner);PublishedAnonymousValue active(owner);(void)value;(void)active;}
            catch(...) {published_allocation_use(view);}
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_virtual_subobject","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_virtual_subobject","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeSubobjectSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kNativeSubobjectSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeSubobjectSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native subobject cleanup identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeSubobjectSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedAllocationOwner *_Nonnull owner) {published_allocation_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void subobject_expired(PublishedAllocationOwner *_Nonnull owner) {
          const char *view=0;
          try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();view=published_allocation_bytes(local);
            PublishedSubobjectValue value(owner);(void)value;
          } catch(...) {published_allocation_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());

    }
}

void TestNativeFunctionTryReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedAllocationBytes;
        __attribute__((objc_root_class)) @interface PublishedAllocationOwner {@public char payload;}
        @end
        void published_allocation_use(const char *view __attribute__((noescape)));
        PublishedAllocationOwner *_Nonnull published_subobject_factory(void);
        void published_subobject_raise(void);
        struct PublishedSubobjectBase {explicit PublishedSubobjectBase(int);~PublishedSubobjectBase() noexcept(false);};
        struct PublishedSubobjectMember {explicit PublishedSubobjectMember(int);~PublishedSubobjectMember() noexcept(false);};
        inline PublishedAllocationBytes published_allocation_bytes(PublishedAllocationOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        struct PublishedSubobjectValue:PublishedSubobjectBase {
          PublishedSubobjectMember first,second;
          PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner,int)
              :PublishedSubobjectBase(0),first(1),second(2) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          explicit PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner) try
              :PublishedSubobjectValue(owner,0) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          } catch(...) {published_allocation_use(published_allocation_bytes(owner));}
          ~PublishedSubobjectValue() noexcept(false) try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          } catch(...) {
            PublishedAllocationOwner *_Nonnull handler=published_subobject_factory();
            const char *view=published_allocation_bytes(handler);
            __objc3_defer {published_allocation_use(view);}return;
          }
        };
        inline void published_allocation_body(PublishedAllocationOwner *_Nonnull owner,int mode) {
          withLifetime(owner) {
            const char *view=published_allocation_bytes(owner);
            try {__objc3_defer {published_allocation_use(view);}PublishedSubobjectValue value(owner);(void)value;}
            catch(...) {published_allocation_use(view);}
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_function_try","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_function_try","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeSubobjectSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kNativeSubobjectSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeSubobjectSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native subobject cleanup identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeSubobjectSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedAllocationOwner *_Nonnull owner) {published_allocation_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void subobject_expired(PublishedAllocationOwner *_Nonnull owner) {
          const char *view=0;
          try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();view=published_allocation_bytes(local);
            PublishedSubobjectValue value(owner);(void)value;
          } catch(...) {published_allocation_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());

    }
}

void TestNativeSubobjectReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedAllocationBytes;
        __attribute__((objc_root_class)) @interface PublishedAllocationOwner {@public char payload;}
        @end
        void published_allocation_use(const char *view __attribute__((noescape)));
        PublishedAllocationOwner *_Nonnull published_subobject_factory(void);
        void published_subobject_raise(void);
        struct PublishedSubobjectBase {explicit PublishedSubobjectBase(int);~PublishedSubobjectBase() noexcept(false);};
        struct PublishedSubobjectMember {explicit PublishedSubobjectMember(int);~PublishedSubobjectMember() noexcept(false);};
        inline PublishedAllocationBytes published_allocation_bytes(PublishedAllocationOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        struct PublishedSubobjectValue:PublishedSubobjectBase {
          PublishedSubobjectMember first,second;
          explicit PublishedSubobjectValue(PublishedAllocationOwner *_Nonnull owner)
              :PublishedSubobjectBase(0),first(1),second(2) {
            const char *view=published_allocation_bytes(owner);
            __objc3_defer {published_allocation_use(view);}
            published_subobject_raise();
          }
          ~PublishedSubobjectValue() noexcept(false) {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();
            const char *view=published_allocation_bytes(local);
            try {__objc3_defer {published_allocation_use(view);}return;}
            catch(...) {published_allocation_use(view);}
          }
        };
        inline void published_allocation_body(PublishedAllocationOwner *_Nonnull owner,int mode) {
          withLifetime(owner) {
            const char *view=published_allocation_bytes(owner);
            try {__objc3_defer {published_allocation_use(view);}PublishedSubobjectValue value(owner);(void)value;}
            catch(...) {published_allocation_use(view);}
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_subobjects","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_subobjects","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeSubobjectSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kNativeSubobjectSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeSubobjectSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native subobject cleanup identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeSubobjectSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedAllocationOwner *_Nonnull owner) {published_allocation_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void subobject_expired(PublishedAllocationOwner *_Nonnull owner) {
          const char *view=0;
          try {
            PublishedAllocationOwner *_Nonnull local=published_subobject_factory();view=published_allocation_bytes(local);
            PublishedSubobjectValue value(owner);(void)value;
          } catch(...) {published_allocation_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());

    }
}

void TestNativeArrayLifetimeReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedAllocationBytes;
        __attribute__((objc_root_class)) @interface PublishedAllocationOwner {
        @public char payload;
        }
        @end
        struct PublishedAllocationValue {
          PublishedAllocationValue();
          ~PublishedAllocationValue() noexcept(false);
          static void operator delete(void *) noexcept;
        };
        struct PublishedAllocationArray {
          PublishedAllocationArray();
          ~PublishedAllocationArray() noexcept(false);
          static void operator delete[](void *) noexcept;
        };
        void published_allocation_use(const char *view __attribute__((noescape)));
        inline PublishedAllocationBytes published_allocation_bytes(PublishedAllocationOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        inline void published_allocation_body(PublishedAllocationOwner *_Nonnull owner,int mode) {
          withLifetime(owner) {
            const char *view=published_allocation_bytes(owner);
            try {
              __objc3_defer {published_allocation_use(view);}
              PublishedAllocationValue local[3];(void)local;
              if(mode)delete[] new PublishedAllocationArray[3];
              else delete[] new PublishedAllocationArray[3];
            } catch(...) {published_allocation_use(view);}
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_array_lifetimes","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_array_lifetimes","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeArrayLifetimeSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kNativeArrayLifetimeSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeArrayLifetimeSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native array lifetime identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeArrayLifetimeSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedAllocationOwner *_Nonnull owner) {published_allocation_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void allocation_expired(PublishedAllocationOwner *_Nonnull owner) {
          const char *view=published_allocation_bytes(owner);
          try {{__objc3_defer {owner=(id)0;}PublishedAllocationValue local[3];(void)local;}}
          catch(...) {published_allocation_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
      files.emplace_back("private-native-allocator.h","void *operator new(__SIZE_TYPE__);\nvoid *operator new[](__SIZE_TYPE__);\n");
      auto hidden=parse("#include \"private-native-allocator.h\"\n"+source,"private-allocation-provider.mm");
      assert(hidden && !hidden->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(hidden->getASTContext(),hidden->getASTContext().getTranslationUnitDecl(),
                                  "fixture.native_array_lifetimes","test",{}));
      auto hidden_result=EmitSemanticInterface(hidden->getASTContext(),"fixture.native_array_lifetimes","test");
      assert(!hidden_result.success);
      assert(std::any_of(hidden_result.diagnostics.begin(),hidden_result.diagnostics.end(),
                        [](const auto &d){return d.code=="O3IFC112";}));
    }
}

void TestNativeAllocationReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedAllocationBytes;
        __attribute__((objc_root_class)) @interface PublishedAllocationOwner {
        @public char payload;
        }
        @end
        struct PublishedAllocationValue {
          explicit PublishedAllocationValue(int);
          ~PublishedAllocationValue() noexcept;
          static void operator delete(void *) noexcept;
        };
        struct PublishedAllocationArray {
          PublishedAllocationArray() noexcept;
          ~PublishedAllocationArray() noexcept(false);
          static void operator delete[](void *) noexcept;
        };
        void published_allocation_use(const char *view __attribute__((noescape)));
        inline PublishedAllocationBytes published_allocation_bytes(PublishedAllocationOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        inline void published_allocation_body(PublishedAllocationOwner *_Nonnull owner,int mode) {
          withLifetime(owner) {
            const char *view=published_allocation_bytes(owner);
            try {
              __objc3_defer {published_allocation_use(view);}
              if(mode)delete new PublishedAllocationValue(7);
              else delete[] new PublishedAllocationArray[3];
            } catch(...) {published_allocation_use(view);}
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_allocation","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_allocation","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeAllocationSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kNativeAllocationSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeAllocationSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native allocation cleanup identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeAllocationSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedAllocationOwner *_Nonnull owner) {published_allocation_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void allocation_expired(PublishedAllocationOwner *_Nonnull owner) {
          const char *view=published_allocation_bytes(owner);
          try {{__objc3_defer {owner=(id)0;}delete new PublishedAllocationValue(7);}}
          catch(...) {published_allocation_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
      files.emplace_back("private-native-allocator.h","void *operator new(__SIZE_TYPE__);\nvoid *operator new[](__SIZE_TYPE__);\n");
      auto hidden=parse("#include \"private-native-allocator.h\"\n"+source,"private-allocation-provider.mm");
      assert(hidden && !hidden->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(hidden->getASTContext(),hidden->getASTContext().getTranslationUnitDecl(),
                                  "fixture.native_allocation","test",{}));
      auto hidden_result=EmitSemanticInterface(hidden->getASTContext(),"fixture.native_allocation","test");
      assert(!hidden_result.success);
      assert(std::any_of(hidden_result.diagnostics.begin(),hidden_result.diagnostics.end(),
                        [](const auto &d){return d.code=="O3IFC112";}));
    }
}

void TestNativeDisposalReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedDisposalBytes;
        __attribute__((objc_root_class)) @interface PublishedDisposalOwner {
        @public char payload;
        }
        @end
        struct PublishedDisposalValue {
          ~PublishedDisposalValue() noexcept(false);
          static void operator delete(void *) noexcept;
        };
        void published_disposal_use(const char *view __attribute__((noescape)));
        inline PublishedDisposalBytes published_disposal_bytes(PublishedDisposalOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        inline void published_disposal_body(PublishedDisposalOwner *_Nonnull owner,PublishedDisposalValue *value) {
          withLifetime(owner) {
            const char *view=published_disposal_bytes(owner);
            try {__objc3_defer {published_disposal_use(view);}delete value;}
            catch(...) {published_disposal_use(view);}
            published_disposal_use(view);
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_disposal","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_disposal","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeDisposalSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kNativeDisposalSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeDisposalSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native disposal identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeDisposalSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedDisposalOwner *_Nonnull owner) {published_disposal_body(owner,nullptr);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void disposal_expired(PublishedDisposalOwner *_Nonnull owner,PublishedDisposalValue *value) {
          const char *view=published_disposal_bytes(owner);
          try {{__objc3_defer {owner=(id)0;}delete value;}}
          catch(...) {published_disposal_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
    }
}

void TestInitializedOwnerReplay() {
  for(const char *standard:{"c17","c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedPartialBytes;
        __attribute__((objc_root_class)) @interface PublishedPartialOwner {
        @public char payload;
        }
        @end
        void published_partial_use(const char *view __attribute__((noescape)));
        int published_partial_change(PublishedPartialOwner *_Nullable __strong *slot,
            const char *view __attribute__((noescape)));
        inline PublishedPartialBytes published_partial_bytes(PublishedPartialOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        inline void published_partial_body(PublishedPartialOwner *_Nonnull owner,int mode) {
          const char *view=published_partial_bytes(owner);
          #if !__has_feature(objc_arc)
          withLifetime(owner)
          #endif
          {
            struct Pair {PublishedPartialOwner *_Nullable first;int tag;};
            struct Pair pair={owner,published_partial_change(&owner,view)};
            __objc3_defer {published_partial_use(view);}
            published_partial_use(view);(void)pair;(void)mode;
          }
        }
        #if defined(__cplusplus) && __has_feature(objc_arc)
        inline void published_partial_capture(PublishedPartialOwner *_Nonnull owner) {
          const char *view=published_partial_bytes(owner);
          auto work=[first=owner,tag=published_partial_change(&owner,view),view] {
            published_partial_use(view);(void)first;(void)tag;
          };
          work();
        }
        #endif
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.initialized_owners","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.initialized_owners","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(arc==llvm::is_contained(supported,llvm::StringRef(kInitializedOwnerSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      if(arc) {
        auto older=supported;std::erase(older,llvm::StringRef(kInitializedOwnerSemantic));
        auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
        assert(!rejected);llvm::consumeError(rejected.takeError());
        auto missing=*decoded;std::erase(missing.required_semantics,kInitializedOwnerSemantic);
        auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
        assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("initialized owner prefix identity"));
      } else supported.push_back(kInitializedOwnerSemantic);
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kInitializedOwnerSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedPartialOwner *_Nonnull owner) {published_partial_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=R"(
        void partial_expired(PublishedPartialOwner *_Nonnull owner) {
          const char *view=published_partial_bytes(owner);
          @try {
            struct Pair {PublishedPartialOwner *_Nullable first;int tag;};
            struct Pair pair={owner,published_partial_change(&owner,view)};(void)pair;
          } @catch(id exception) {published_partial_use(view);}
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
    }
}

void TestNativeFinallyReplay() {
  for(const char *standard:{"c17","c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        typedef borrowed const char *PublishedFinallyBytes;
        __attribute__((objc_root_class)) @interface PublishedFinallyOwner {
        @public char payload;
        }
        #if !__has_feature(objc_arc)
        - (void)release;
        #endif
        @end
        void published_finally_use(const char *view __attribute__((noescape)));
        inline PublishedFinallyBytes published_finally_bytes(PublishedFinallyOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        inline void published_finally_rethrow(PublishedFinallyOwner *_Nonnull owner) {
          @try {@throw owner;} @catch(id exception) {@throw;}
        }
        inline int published_finally_body(PublishedFinallyOwner *_Nonnull owner,int mode) {
          withLifetime(owner) {
            const char *view=published_finally_bytes(owner);
            @try {
              __objc3_defer {published_finally_use(view);}
              if(mode==1)return 1;
              if(mode==2)@throw owner;
            } @catch(id exception) {published_finally_use(view);}
              @finally {
                __objc3_defer {published_finally_use(view);}
                published_finally_use(view);
              }
            return 2;
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_finally","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_finally","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"finally-provider.m");
      auto replay=emit(first.source_bytes,"finally-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-finally.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kNativeFinallySemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      assert(std::any_of(decoded->declarations.begin(),decoded->declarations.end(),[](const auto &d) {
        return d.qualified_name.find("published_finally_rethrow")!=std::string::npos && HasNativeFinallyIdentity(d);
      }));
      auto older=supported;std::erase(older,llvm::StringRef(kNativeFinallySemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeFinallySemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("native finally identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kNativeFinallySemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void finally_consumer(PublishedFinallyOwner *_Nonnull owner) {published_finally_body(owner,0);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=arc?R"(
        void finally_expired(PublishedFinallyOwner *_Nonnull owner) {
          const char *view=published_finally_bytes(owner);
          @try {published_finally_use(view);} @finally {owner=(id)0;}
          published_finally_use(view);
        }
      )":R"(
        void finally_expired(PublishedFinallyOwner *_Nonnull owner) {
          const char *view=published_finally_bytes(owner);
          @try {published_finally_use(view);} @finally {[owner release];}
          published_finally_use(view);
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
    }
}

void TestManagedRuntimeScopeReplay() {
  for(const char *standard:{"c17","c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      args.push_back("-fblocks");args.push_back("-fexceptions");args.push_back("-fobjc-exceptions");
      args.push_back("-fobjc-arc-exceptions");args.push_back("-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR);
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      const std::string source=R"(
        #ifdef __cplusplus
        #include <objc3c/runtime/objc3_task.hpp>
        #else
        #include <objc3c/runtime/objc3_runtime_services.h>
        #endif
        @protocol Sendable @end
        typedef borrowed const char *PublishedRuntimeBytes;
        __attribute__((objc_root_class)) @interface PublishedRuntimeOwner <Sendable> {
        @public char payload;
        }
        #if !__has_feature(objc_arc)
        - (id)retain;
        - (id)autorelease;
        - (void)release;
        #endif
        @end
        void published_runtime_use(const char *view __attribute__((noescape)));
        inline PublishedRuntimeBytes published_runtime_bytes(PublishedRuntimeOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return &owner->payload;}
        inline void published_runtime_body(PublishedRuntimeOwner *_Nonnull owner) {
          const char *view=published_runtime_bytes(owner);
          @autoreleasepool {@synchronized(owner) {
            __objc3_defer {published_runtime_use(view);}
            #if __has_feature(objc_arc)
            owner=(id)0;
            #endif
            published_runtime_use(view);
          }}
        }
        int published_runtime_wait(void) __objc3_async_frame;
        inline int published_runtime_async(PublishedRuntimeOwner *_Nonnull owner) __objc3_async_frame {
          withLifetime(owner) {
            const char *view=published_runtime_bytes(owner);
            @autoreleasepool {
              #if !__has_feature(objc_arc)
              [owner autorelease];
              #endif
              int value=__objc3_await published_runtime_wait();published_runtime_use(view);return value;
            }
          }
        }
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.managed_runtime_scopes","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.managed_runtime_scopes","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"runtime-provider.m");
      auto replay=emit(first.source_bytes,"runtime-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-runtime.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kManagedRuntimeScopeSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kManagedRuntimeScopeSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kManagedRuntimeScopeSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("managed runtime scope identity"));
      auto plain=emit("void plain_scope_contract(void);","plain-scope-contract.m");
      auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
      invented->required_semantics.push_back(kManagedRuntimeScopeSemantic);
      auto unexpected=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unexpected);
      llvm::consumeError(unexpected.takeError());
      auto good=parse(first.source_bytes+R"(
        void runtime_consumer(PublishedRuntimeOwner *_Nonnull owner) {published_runtime_body(owner);}
      )","runtime-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      const std::string expired=arc?R"(
        void runtime_expired(PublishedRuntimeOwner *_Nonnull owner) {
          const char *view=published_runtime_bytes(owner);
          @synchronized(owner){owner=(id)0;published_runtime_use(view);}
          published_runtime_use(view);
        }
      )":R"(
        void runtime_expired(PublishedRuntimeOwner *_Nonnull owner) {
          const char *view=published_runtime_bytes(owner);
          @autoreleasepool {[owner autorelease];}published_runtime_use(view);
        }
      )";
      auto bad=parse(first.source_bytes+expired,"expired-runtime-scope.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
    }
}

void TestBorrowedReceiverResultReplay() {
  for(const char *standard:{"c17","c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      const std::string source=R"(
        typedef borrowed const char *PublishedReceiverBytes;
        __attribute__((objc_root_class)) @interface PublishedReceiverOwner
        @property(nonatomic,readonly) const char *view __attribute__((objc_returns_inner_pointer));
        - (const char *)bytes __attribute__((objc_returns_inner_pointer));
        #ifdef __cplusplus
        - (char &)reference __attribute__((objc_returns_inner_pointer));
        #endif
        @end
        void published_receiver_use(const char *bytes __attribute__((noescape)));
        inline PublishedReceiverBytes published_receiver_bytes(PublishedReceiverOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return [owner bytes];}
        inline PublishedReceiverBytes published_receiver_view(PublishedReceiverOwner *_Nonnull owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return owner.view;}
        #ifdef __cplusplus
        inline char published_receiver_copy(PublishedReceiverOwner *_Nonnull owner) {
          char &reference=[owner reference];char copy=reference;owner=(id)0;return copy;
        }
        #endif
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.borrowed_receiver_results","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.borrowed_receiver_results","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"receiver-provider.m");
      auto replay=emit(first.source_bytes,"receiver-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-receiver.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kBorrowedReceiverResultSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kBorrowedReceiverResultSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kBorrowedReceiverResultSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("borrowed receiver result identity"));
      auto good=parse(first.source_bytes+R"(
        void receiver_consumer(PublishedReceiverOwner *_Nonnull owner) {
          const char *bytes=published_receiver_bytes(owner);published_receiver_use(bytes);
          withLifetime(owner){owner=(id)0;published_receiver_use(bytes);}
        }
      )","receiver-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      auto bad=parse(first.source_bytes+R"(
        void receiver_expired(PublishedReceiverOwner *_Nonnull owner) {
          const char *bytes=owner.view;owner=(id)0;published_receiver_use(bytes);
        }
      )","expired-receiver.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
      if(cxx) {
        auto invalid_reference=parse(first.source_bytes+R"(
          char &escape_receiver_reference(PublishedReceiverOwner *_Nonnull owner) {return [owner reference];}
          void write_expired_receiver_reference(PublishedReceiverOwner *_Nonnull owner) {
            char &reference=[owner reference];owner=(id)0;reference='x';
          }
        )","expired-receiver-reference.mm");
        assert(invalid_reference && invalid_reference->getDiagnostics().hasErrorOccurred());
      }
    }
}

void TestBorrowedInteriorStorageReplay() {
  for(const char *standard:{"c17","c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      const std::string source=R"(
        struct PublishedInteriorPayload {char bytes[4];int scalar;};
        __attribute__((objc_root_class)) @interface PublishedInteriorOwner {
        @public char byte;struct PublishedInteriorPayload payload;
        } @end
        void published_interior_use(const char *bytes __attribute__((noescape)));
        inline borrowed const char *published_interior_bytes(PublishedInteriorOwner *owner)
            __attribute__((objc_returns_borrowed(owner_index=0))) {return owner->payload.bytes;}
        inline int published_interior_copy(PublishedInteriorOwner *owner) {
          int copy=owner->payload.scalar;owner=(id)0;return copy;
        }
        #ifdef __cplusplus
        inline int published_interior_reference(PublishedInteriorOwner *owner) {
          char &reference=owner->byte;char copy=reference;
          published_interior_use(__builtin_addressof(reference));owner=(id)0;return copy;
        }
        #endif
      )";
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.borrowed_interior_storage","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.borrowed_interior_storage","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      auto first=emit(source,"interior-provider.m");
      auto replay=emit(first.source_bytes,"interior-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-interior.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> supported;
      for(const auto &item:*metadata->getAsObject()->getArray("required_semantics"))supported.push_back(*item.getAsString());
      assert(llvm::is_contained(supported,llvm::StringRef(kBorrowedInteriorStorageSemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
      auto older=supported;std::erase(older,llvm::StringRef(kBorrowedInteriorStorageSemantic));
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kBorrowedInteriorStorageSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);
      assert(llvm::StringRef(llvm::toString(invalid.takeError())).contains("borrowed interior storage identity"));
      auto good=parse(first.source_bytes+R"(
        void interior_consumer(PublishedInteriorOwner *owner) {
          const char *bytes=published_interior_bytes(owner);published_interior_use(bytes);
          withLifetime(owner){owner=(id)0;published_interior_use(bytes);}
        }
      )","interior-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      auto bad=parse(first.source_bytes+R"(
        void interior_expired(PublishedInteriorOwner *owner) {
          const char *bytes=owner->payload.bytes;owner=(id)0;published_interior_use(bytes);
        }
      )","expired-interior.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
      if(cxx) {
        auto invalid_reference=parse(first.source_bytes+R"(
          char &escape_interior_reference(PublishedInteriorOwner *owner) {return owner->byte;}
          void write_expired_interior_reference(PublishedInteriorOwner *owner) {
            char &reference=owner->byte;owner=(id)0;reference='x';
          }
        )","expired-interior-reference.mm");
        assert(invalid_reference && invalid_reference->getDiagnostics().hasErrorOccurred());
      }
    }
}

void TestBorrowedCatchBindingReplay() {
  for(const char *standard:{"c17","c++20","c++23"})
    for(bool arc:{false,true}) {
      const bool cxx=std::string(standard)!="c17";
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fobjc-runtime=gnustep-2.2",
          "-fobjc3-profile=strict-system","-fexceptions","-fobjc-exceptions",
          "--target=x86_64-unknown-linux-gnu"};
      if(arc)args.push_back("-fobjc-arc");
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name);
      };
      const std::string source=R"(
        typedef borrowed const char *PublishedHandlerBytes;
        PublishedHandlerBytes published_handler_bytes(id owner)
            __attribute__((objc_returns_borrowed(owner_index=0)));
        void published_handler_use(const char *bytes __attribute__((noescape)));
        void published_handler_raise(void);
        inline void published_handler_body(int mode) {
          @try {
            @try {published_handler_raise();}
            @catch(id owner) {
              PublishedHandlerBytes bytes=published_handler_bytes(owner);
              __objc3_defer {published_handler_use(bytes);}
              published_handler_use(bytes);
              if(mode)@throw;
              return;
            }
          } @catch(id outer) {published_handler_use(published_handler_bytes(outer));}
        }
      )";
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.borrowed_catch_bindings","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.borrowed_catch_bindings","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        return result;
      };
      auto first=emit(source,"catch-bindings-provider.m");assert(first.success);
      auto replay=emit(first.source_bytes,"catch-bindings-replay.m");
      assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-catch-bindings.m");
      assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto good=parse(first.source_bytes+"void consume_handler(void){published_handler_body(0);}","catch-bindings-consumer.m");
      assert(good && !good->getDiagnostics().hasErrorOccurred());
      auto bad=parse(first.source_bytes+R"(
        void escape_handler(void) {
          const char *bytes=0;
          @try {published_handler_raise();}
          @catch(id owner){bytes=published_handler_bytes(owner);}
          published_handler_use(bytes);
        }
      )","expired-catch-binding.m");
      assert(bad && bad->getDiagnostics().hasErrorOccurred());
    }
}

void TestExplicitResourcePropertyReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
          "-fobjc-version=3","-fblocks","-fobjc-runtime=gnustep-2.2",
          "-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu",
          "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
      if(arc)args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      const std::string source=R"(
        #include <objc3c/runtime/objc3_task.hpp>
        void published_resource_close(int) noexcept;
        typedef int PublishedResourceHandle __attribute__((objc_requires_cleanup(published_resource_close),objc_resource_family(PublishedResourceHandles),objc_invalid(-1)));
        struct __attribute__((objc_unsafe_sendable)) PublishedFrameValue {
          PublishedResourceHandle handle;int value;
          PublishedFrameValue(int);
          PublishedFrameValue(const PublishedFrameValue &);
          PublishedFrameValue(PublishedFrameValue &&);
          ~PublishedFrameValue() noexcept;
        };
        struct __attribute__((objc_unsafe_sendable)) PublishedFramePair {PublishedFrameValue values[2];};
        struct __attribute__((objc_unsafe_sendable)) PublishedResourceMove {
          PublishedResourceHandle handle;explicit PublishedResourceMove(int);
          PublishedResourceMove(const PublishedResourceMove &)=delete;
          PublishedResourceMove(PublishedResourceMove &&);~PublishedResourceMove() noexcept;
        };
        union __attribute__((objc_unsafe_sendable)) PublishedResourceUnion {
          PublishedResourceHandle handle;long scalar;
          explicit PublishedResourceUnion(int);PublishedResourceUnion(const PublishedResourceUnion &);
          PublishedResourceUnion(PublishedResourceUnion &&);~PublishedResourceUnion() noexcept;int value() const;
        };
        @protocol Sendable @end
        __attribute__((objc_root_class)) @interface PublishedThrowingProperty <Sendable>
        @property(nonatomic) PublishedFrameValue value;
        - (PublishedFrameValue)value __attribute__((objc_executor(main)));
        - (void)setValue:(PublishedFrameValue)value __attribute__((objc_executor(main)));
        @property(nonatomic) PublishedFramePair pair;
        - (PublishedFramePair)pair __attribute__((objc_executor(main)));
        - (void)setPair:(PublishedFramePair)value __attribute__((objc_executor(main)));
        @property(nonatomic) PublishedResourceMove move;
        - (PublishedResourceMove)move __attribute__((objc_executor(main)));
        - (void)setMove:(PublishedResourceMove)value __attribute__((objc_executor(main)));
        @property(nonatomic) PublishedResourceUnion variant;
        - (PublishedResourceUnion)variant __attribute__((objc_executor(main)));
        - (void)setVariant:(PublishedResourceUnion)value __attribute__((objc_executor(main)));
        @end
        inline int published_throwing_property(PublishedThrowingProperty *object,
                                               PublishedFrameValue *input,PublishedFramePair *pair) __objc3_async_frame {
          __objc3_await (object.value=*input);
          __objc3_await (object.pair=*pair);
          __objc3_await (object.move=PublishedResourceMove(43));
          __objc3_await (object.variant=PublishedResourceUnion(44));
          PublishedFramePair paired=__objc3_await object.pair;
          PublishedResourceUnion variant=__objc3_await object.variant;
          PublishedFrameValue result=__objc3_await object.value;
          if(result.value<0)throw result.value;
          return result.value+paired.values[0].value+variant.value();
        }
        template<class T> int published_throwing_template(T object,PublishedFrameValue *input,PublishedFramePair *pair)
            __objc3_async_frame {return __objc3_await published_throwing_property(object,input,pair);}
      )";
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.explicit_resource_properties","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.explicit_resource_properties","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        return result;
      };
      auto first=emit(source,"frame-exceptions-provider.mm");assert(first.success);
      auto replay=emit(first.source_bytes,"frame-exceptions-replay.mm");
      assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-frame-exceptions.mm");
      assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> semantics;
      for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
      assert(llvm::is_contained(semantics,llvm::StringRef("objc3.async-entry-abi.v5")));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
      auto old=semantics;std::erase(old,llvm::StringRef(kAsyncEntryABISemantic));
      old.push_back("objc3.async-entry-abi.v4");
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto consumer=parse(first.source_bytes+R"(
        int consume_throwing(PublishedThrowingProperty *object,PublishedFrameValue *input,PublishedFramePair *pair) __objc3_async {
          try {return __objc3_await published_throwing_template(object,input,pair);}
          catch(int value){return value;}
        }
      )","frame-exceptions-consumer.mm");
      assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
    }
}

void TestNativeFrameExceptionReplay() {
  for(const char *standard:{"c++20","c++23"})
    for(bool arc:{false,true}) {
      std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
          "-fobjc-version=3","-fblocks","-fobjc-runtime=gnustep-2.2",
          "-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu",
          "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
      if(arc)args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      const std::string source=R"(
        #include <objc3c/runtime/objc3_task.hpp>
        struct __attribute__((objc_unsafe_sendable)) PublishedFrameValue {
          int value;
          PublishedFrameValue(int);
          PublishedFrameValue(const PublishedFrameValue &);
          PublishedFrameValue(PublishedFrameValue &&);
          ~PublishedFrameValue() noexcept;
        };
        @protocol Sendable @end
        __attribute__((objc_root_class)) @interface PublishedThrowingProperty <Sendable>
        @property(nonatomic) PublishedFrameValue value;
        - (PublishedFrameValue)value __attribute__((objc_executor(main)));
        - (void)setValue:(PublishedFrameValue)value __attribute__((objc_executor(main)));
        @end
        inline int published_throwing_property(PublishedThrowingProperty *object,
                                               PublishedFrameValue *input) __objc3_async_frame {
          __objc3_await (object.value=*input);
          PublishedFrameValue result=__objc3_await object.value;
          if(result.value<0)throw result.value;
          return result.value;
        }
        template<class T> int published_throwing_template(T object,PublishedFrameValue *input)
            __objc3_async_frame {return __objc3_await published_throwing_property(object,input);}
      )";
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.native_frame_exceptions","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_frame_exceptions","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        return result;
      };
      auto first=emit(source,"frame-exceptions-provider.mm");assert(first.success);
      auto replay=emit(first.source_bytes,"frame-exceptions-replay.mm");
      assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-frame-exceptions.mm");
      assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> semantics;
      for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
      assert(llvm::is_contained(semantics,llvm::StringRef("objc3.async-entry-abi.v5")));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
      auto old=semantics;std::erase(old,llvm::StringRef(kAsyncEntryABISemantic));
      old.push_back("objc3.async-entry-abi.v4");
      auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
      assert(!rejected);llvm::consumeError(rejected.takeError());
      auto consumer=parse(first.source_bytes+R"(
        int consume_throwing(PublishedThrowingProperty *object,PublishedFrameValue *input) __objc3_async {
          try {return __objc3_await published_throwing_template(object,input);}
          catch(int value){return value;}
        }
      )","frame-exceptions-consumer.mm");
      assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
    }
}

void CheckEffectfulIsolatedPropertyReplay(bool async) {
  for(const char *standard:{"c17","c++20","c++23"})for(bool arc:{false,true}) {
    const bool cxx=std::string(standard)!="c17";
    std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",std::string("-std=")+standard,
        "-fobjc-version=3","-fblocks","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-concurrency",
        "--target=x86_64-unknown-linux-gnu","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    if(arc)args.push_back("-fobjc-arc");
    clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
    auto parse=[&](const std::string &text,const char *name) {
      return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
          std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),files);
    };
    std::string source=R"(
      #include <objc3c/runtime/objc3_runtime_services.h>
      @protocol Sendable @end
      @protocol Error <Sendable> @end
      __attribute__((objc_root_class)) @interface PublishedCheckedProperty <Sendable>
      @property(nonatomic) int value;
      - (int)value __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
      - (void)setValue:(int)value __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
      @property(nonatomic) int getterOnly;
      - (int)getterOnly __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
      - (void)setGetterOnly:(int)value __attribute__((nothrow,objc_executor(main)));
      @end
      inline int published_checked_property(PublishedCheckedProperty *object,int value) __objc3_async_frame __objc3_throws(id<Error>) {
        __objc3_try(__objc3_await (object.value=value));
        __objc3_try(__objc3_await (object.getterOnly+=2));
        __objc3_try(__objc3_await object.value++);
        return __objc3_try(__objc3_await --object.value);
      }
    )";
    if(async) {
      const std::string old="__objc3_throws(id<Error>) __attribute__((nothrow";
      const std::string updated="__objc3_async_frame "+old;
      for(std::size_t at=0;(at=source.find(old,at))!=std::string::npos;at+=updated.size())source.replace(at,old.size(),updated);
    }
    if(cxx) {
      source=std::string("#include <objc3c/runtime/objc3_task.hpp>\n")+source;
      source+=R"(
        template<class T> int published_generic_checked_property(T object,int value) __objc3_async_frame __objc3_throws(id<Error>) {
          return __objc3_try(__objc3_await (object.value+=value));
        }
      )";
      if(!async)source+=R"(
        struct __attribute__((objc_unsafe_sendable)) PublishedCheckedOwner {
          int value;explicit PublishedCheckedOwner(int) noexcept;
          PublishedCheckedOwner(const PublishedCheckedOwner &) noexcept;PublishedCheckedOwner(PublishedCheckedOwner &&) noexcept;
          ~PublishedCheckedOwner() noexcept;
        };
        @interface PublishedCheckedProperty (NativeOwner)
        @property(nonatomic) PublishedCheckedOwner owner;
        - (PublishedCheckedOwner)owner __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
        - (void)setOwner:(PublishedCheckedOwner)value __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
        @end
        inline int published_checked_owner(PublishedCheckedProperty *object) __objc3_async_frame __objc3_throws(id<Error>) {
          PublishedCheckedOwner value=__objc3_try(__objc3_await object.owner);
          __objc3_try(__objc3_await (object.owner=value));return value.value;
        }
      )";
      if(async)source+=R"(
        struct __attribute__((objc_unsafe_sendable)) PublishedAsyncPropertyOwner {
          int value;explicit PublishedAsyncPropertyOwner(int) noexcept;
          PublishedAsyncPropertyOwner(const PublishedAsyncPropertyOwner &) noexcept;
          PublishedAsyncPropertyOwner(PublishedAsyncPropertyOwner &&) noexcept;
          ~PublishedAsyncPropertyOwner() noexcept;
        };
        @interface PublishedCheckedProperty (AsyncOwner)
        @property(nonatomic) PublishedAsyncPropertyOwner owner;
        - (PublishedAsyncPropertyOwner)owner __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
        - (void)setOwner:(PublishedAsyncPropertyOwner)value __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
        @property(nonatomic) int taskValue;
        - (int)taskValue __objc3_async __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
        - (void)setTaskValue:(int)value __objc3_async __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
        @end
        inline int published_async_owner(PublishedCheckedProperty *object) __objc3_async_frame __objc3_throws(id<Error>) {
          PublishedAsyncPropertyOwner value=__objc3_try(__objc3_await object.owner);
          __objc3_try(__objc3_await (object.owner=value));
          return __objc3_try(__objc3_await (object.taskValue+=value.value));
        }
      )";
    }
    auto emit=[&](const std::string &text,const char *name) {
      auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
          "fixture.checked_properties","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.checked_properties","test");
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source,"checked-property-provider.m"),replay=emit(first.source_bytes,"checked-property-replay.m");
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"checked-property-relocated.m");
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
    assert(llvm::is_contained(decoded->required_semantics,std::string(kAsyncEntryABISemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kAsyncEntryABISemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto missing=*decoded;std::erase(missing.required_semantics,kAsyncEntryABISemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    assert(first.source_bytes.find("__objc3_isolation_entry_")==std::string::npos);
    auto consumer=parse(first.source_bytes+"objc3_async_frame checked_consumer(PublishedCheckedProperty *object){return published_checked_property(object,42);}"+
        (cxx?"objc3_async_frame generic_checked_consumer(PublishedCheckedProperty *object){return published_generic_checked_property(object,42);}":""),"checked-property-consumer.m");
    assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
    auto bad=parse(first.source_bytes+"int unchecked_consumer(PublishedCheckedProperty *object) __objc3_async_frame {return __objc3_await (object.value+=1);}","unchecked-property-consumer.m");
    assert(bad && bad->getDiagnostics().hasErrorOccurred());
  }
}

void TestMixedCarrierCallReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string prelude=R"(
      @protocol Error @end
      namespace PublishedMixedCall {
        struct Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef id __strong Strong;typedef __objc3_result(Value,Strong) Maybe;
        typedef Maybe (^Callback)(Maybe);
      }
      __attribute__((objc_root_class)) @interface PublishedMixedCallHost
      - (PublishedMixedCall::Maybe)echo:(PublishedMixedCall::Maybe)value;
      - (PublishedMixedCall::Maybe)checked:(PublishedMixedCall::Maybe)value __objc3_throws(id<Error>);
      @end
    )";
    const auto source=prelude+R"(
      namespace PublishedMixedCall {
        inline Maybe message(PublishedMixedCallHost *_Nonnull object){return [object echo:__objc3_ok(Value,Strong,Value(7))];}
        inline Maybe implicit_message(PublishedMixedCallHost *_Nonnull object,const Maybe &value){return [object echo:value];}
        inline Maybe implicit_block(Callback _Nonnull callback,const Maybe &value){return callback(value);}
        inline int checked(PublishedMixedCallHost *_Nonnull object){auto value=__objc3_try_optional([object checked:__objc3_ok(Value,Strong,Value(7))]);return __objc3_has_value(value);}
        inline Maybe invoke(Callback _Nonnull callback){return callback(__objc3_ok(Value,Strong,Value(7)));}
        inline int literal(){auto callback=^Maybe(Maybe value){return __objc3_copy(value);};auto value=callback(__objc3_err(Value,Strong,(id)0));return __objc3_is_ok(value);}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"mixed-carrier-call.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.mixed_carrier_call","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.mixed_carrier_call","test",{},{},{},{"PublishedMixedCall","PublishedMixedCallHost","Error"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kMixedCarrierCallSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kMixedCarrierCallSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kMixedCarrierCallSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto declarations=emit(R"(
      @protocol Error @end
      namespace PublishedMixedCall {struct Plain {int value;};}
      __attribute__((objc_root_class)) @interface PublishedMixedCallHost @end
    )");auto invented=DecodeSemanticInterfaceSidecar(declarations.sidecar_json,declarations.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kMixedCarrierCallSemantic)));
    invented->required_semantics.push_back(kMixedCarrierCallSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestNativeByrefStorageReplay() {
 for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
  std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
   "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
   "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
  if(arc)args.push_back("-fobjc-arc");
  const std::string source=R"(
   namespace PublishedByref {
    struct alignas(128) Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
    typedef __objc3_optional(Value) Maybe;
    inline int capture(int seed){__block Value value(seed);auto callback=^int{return value.value;};return callback();}
    inline int scalar(int seed){alignas(64) __block int value=seed;auto callback=^int{return ++value;};return callback();}
    inline int carrier(int seed){__block Maybe value=__objc3_some(Value,Value(seed));auto callback=^int{return __objc3_has_value(value);};return callback();}
    template<class T> inline int generic(int seed){__block __objc3_optional(T) value=__objc3_some(T,T(seed));auto callback=^int{return __objc3_has_value(value);};return callback();}
   }
  )";
  auto emit=[&](const std::string &text) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-byref-storage.mm");
   assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.byref_storage","test",{}));
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.byref_storage","test",{},{},{},{"PublishedByref"});
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  auto changed=source;auto alignment=changed.find("alignas(64)");assert(alignment!=std::string::npos);
  changed.replace(alignment,std::string("alignas(64)").size(),"alignas(256)");
  assert(first.identity.semantic_fingerprint!=emit(changed).identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
  assert(llvm::is_contained(read->required_semantics,std::string(kNativeByrefStorageSemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kNativeByrefStorageSemantic));
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(read->required_semantics,kNativeByrefStorageSemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*read,supported);assert(!missing);llvm::consumeError(missing.takeError());
  auto plain=emit("namespace PublishedByref {inline int plain(){return 42;}}");
  auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
  invented->required_semantics.push_back(kNativeByrefStorageSemantic);
  auto invalid=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
 }
 for(bool arc:{false,true}) {
  std::vector<std::string> args={"-x","objective-c","-std=c17",
   "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
   "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
  if(arc)args.push_back("-fobjc-arc");
  const std::string source=R"(
   struct __attribute__((aligned(64))) PublishedCAlignedValue {int value;};
   inline int published_c_aligned_capture(int seed) {
    __block struct PublishedCAlignedValue value={seed};__block int scalar __attribute__((aligned(32)))=seed;
    int (^block)(void)=^int{return ++value.value+scalar;};return block();
   }
  )";
  auto emit=[&](const std::string &text) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-c-byref-storage.m");
   assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.c_byref_storage","test",{}));
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.c_byref_storage","test");
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  auto changed=source;auto alignment=changed.find("aligned(32)");assert(alignment!=std::string::npos);
  changed.replace(alignment,std::string("aligned(32)").size(),"aligned(64)");
  assert(first.identity.semantic_fingerprint!=emit(changed).identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
  assert(llvm::is_contained(read->required_semantics,std::string(kNativeByrefStorageSemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kNativeByrefStorageSemantic));
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(read->required_semantics,kNativeByrefStorageSemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*read,supported);assert(!missing);llvm::consumeError(missing.takeError());
  auto plain=emit("inline int published_c_plain(void){return 42;}");
  auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
  invented->required_semantics.push_back(kNativeByrefStorageSemantic);
  auto invalid=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
 }
}

void TestNativeBlockAlignmentReplay() {
 for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
  std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
   "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
   "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
  if(arc)args.push_back("-fobjc-arc");
  const std::string source=R"(
   namespace PublishedAlignedBlock {
    struct alignas(128) Value {int value;explicit Value(int);Value(const Value &);~Value() noexcept;};
    inline int capture(int seed){Value value(seed);auto callback=^int{return value.value;};return callback();}
    inline int scalar(int seed){alignas(64) int value=seed;auto callback=^int{return value;};return callback();}
    template<class T> inline int aligned_local(T value){alignas(T) T copy(value);auto callback=^int{(void)copy;return 1;};return callback();}
    template<class T> inline int generic(T value){auto callback=^int{(void)value;return 1;};return callback();}
   }
  )";
  auto emit=[&](const std::string &text) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-aligned-block.mm");
   assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.aligned_block","test",{}));
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.aligned_block","test",{},{},{},{"PublishedAlignedBlock"});
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  auto changed=source;auto alignment=changed.find("alignas(64)");assert(alignment!=std::string::npos);
  changed.replace(alignment,std::string("alignas(64)").size(),"alignas(256)");
  assert(first.identity.semantic_fingerprint!=emit(changed).identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
  assert(llvm::is_contained(read->required_semantics,std::string(kNativeBlockAlignmentSemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kNativeBlockAlignmentSemantic));
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(read->required_semantics,kNativeBlockAlignmentSemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*read,supported);assert(!missing);llvm::consumeError(missing.takeError());
  auto plain=emit("namespace PublishedAlignedBlock {inline int plain(){return 42;}}");
  auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
  invented->required_semantics.push_back(kNativeBlockAlignmentSemantic);
  auto invalid=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
 }
 for(bool arc:{false,true}) {
  std::vector<std::string> args={"-x","objective-c","-std=c17",
   "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
   "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
  if(arc)args.push_back("-fobjc-arc");
  const std::string source=R"(
   struct __attribute__((aligned(64))) PublishedCAlignedValue {int value;};
   inline int published_c_aligned_capture(int seed) {
    struct PublishedCAlignedValue value={seed};int scalar __attribute__((aligned(32)))=seed;
    int (^block)(void)=^int{return value.value+scalar;};return block();
   }
  )";
  auto emit=[&](const std::string &text) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-c-aligned-block.m");
   assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.c_aligned_block","test",{}));
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.c_aligned_block","test");
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  auto changed=source;auto alignment=changed.find("aligned(32)");assert(alignment!=std::string::npos);
  changed.replace(alignment,std::string("aligned(32)").size(),"aligned(64)");
  assert(first.identity.semantic_fingerprint!=emit(changed).identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
  assert(llvm::is_contained(read->required_semantics,std::string(kNativeBlockAlignmentSemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kNativeBlockAlignmentSemantic));
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(read->required_semantics,kNativeBlockAlignmentSemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*read,supported);assert(!missing);llvm::consumeError(missing.takeError());
  auto plain=emit("inline int published_c_plain(void){return 42;}");
  auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,supported);assert(invented);
  invented->required_semantics.push_back(kNativeBlockAlignmentSemantic);
  auto invalid=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
 }
}

void TestCppCarrierValueReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string prelude=R"(
      @protocol Error @end
      namespace PublishedCppValue {
        struct Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);};
        typedef __objc3_optional(Value) Maybe;
        struct alignas(128) Wide{int value;};typedef __objc3_optional(Wide) WideMaybe;struct Plain{int value;};struct Empty{};union Active{int value;double other;};
        typedef __objc3_optional(Plain) PlainMaybe;typedef __objc3_optional(Empty) EmptyMaybe;typedef __objc3_optional(Active) UnionMaybe;
        typedef Maybe (^Callback)(Maybe);
      }
      __attribute__((objc_root_class)) @interface PublishedCppValueHost
      - (PublishedCppValue::WideMaybe)wide:(PublishedCppValue::WideMaybe)value;
      - (PublishedCppValue::PlainMaybe)plain:(PublishedCppValue::PlainMaybe)value;
      - (PublishedCppValue::EmptyMaybe)empty:(PublishedCppValue::EmptyMaybe)value;
      - (PublishedCppValue::UnionMaybe)active:(PublishedCppValue::UnionMaybe)value;
      - (PublishedCppValue::Maybe)echo:(PublishedCppValue::Maybe)value;
      - (PublishedCppValue::Maybe)checked:(PublishedCppValue::Maybe)value __objc3_throws(id<Error>);
      @end
    )";
    const auto source=prelude+R"(
      namespace PublishedCppValue {
        inline PlainMaybe plain_message(PublishedCppValueHost *_Nonnull object){return [object plain:__objc3_some(Plain,Plain{42})];}
        inline EmptyMaybe empty_message(PublishedCppValueHost *_Nonnull object){return [object empty:__objc3_some(Empty,Empty{})];}
        inline UnionMaybe union_message(PublishedCppValueHost *_Nonnull object){return [object active:__objc3_some(Active,Active{73})];}
        inline Maybe message(PublishedCppValueHost *_Nonnull object){return [object echo:__objc3_some(Value,Value(7))];}
        inline Maybe implicit_message(PublishedCppValueHost *_Nonnull object,const Maybe &value){return [object echo:value];}
        inline Maybe implicit_block(Callback _Nonnull callback,const Maybe &value){return callback(value);}
        inline int checked(PublishedCppValueHost *_Nonnull object){auto value=__objc3_try_optional([object checked:__objc3_some(Value,Value(7))]);return __objc3_has_value(value);}
        inline Maybe invoke(Callback _Nonnull callback){return callback(__objc3_some(Value,Value(7)));}
        inline int literal(){auto captured=__objc3_some(Value,Value(7));auto callback=^Maybe(Maybe value){return __objc3_has_value(value)?__objc3_copy(captured):__objc3_none(Value);};auto value=callback(__objc3_none(Value));return __objc3_has_value(value);}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"cpp-carrier-values.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.cpp_carrier_values","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.cpp_carrier_values","test",{},{},{},{"PublishedCppValue","PublishedCppValueHost","Error"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kCppCarrierCallSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kCppCarrierCallSemantic));older.push_back("objc3.cxx-carrier-call.v1");
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kCppCarrierCallSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto declarations=emit(R"(
      @protocol Error @end
      namespace PublishedCppValue {struct Plain {int value;};}
      __attribute__((objc_root_class)) @interface PublishedCppValueHost @end
    )");auto invented=DecodeSemanticInterfaceSidecar(declarations.sidecar_json,declarations.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kCppCarrierCallSemantic)));
    invented->required_semantics.push_back(kCppCarrierCallSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestCppCarrierRuntimeReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string prelude=R"(
      @protocol Error @end
      namespace PublishedCppRuntime {
        struct Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef __objc3_optional(Value) Maybe;
        typedef Maybe (^Callback)(Maybe);
      }
      __attribute__((objc_root_class)) @interface PublishedCppRuntimeHost
      - (PublishedCppRuntime::Maybe)echo:(PublishedCppRuntime::Maybe)value;
      - (PublishedCppRuntime::Maybe)checked:(PublishedCppRuntime::Maybe)value __objc3_throws(id<Error>);
      @end
    )";
    const auto source=prelude+R"(
      namespace PublishedCppRuntime {
        inline Maybe message(PublishedCppRuntimeHost *_Nonnull object){return [object echo:__objc3_some(Value,Value(7))];}
        inline Maybe implicit_message(PublishedCppRuntimeHost *_Nonnull object,const Maybe &value){return [object echo:value];}
        inline Maybe implicit_block(Callback _Nonnull callback,const Maybe &value){return callback(value);}
        inline int checked(PublishedCppRuntimeHost *_Nonnull object){auto value=__objc3_try_optional([object checked:__objc3_some(Value,Value(7))]);return __objc3_has_value(value);}
        inline Maybe invoke(Callback _Nonnull callback){return callback(__objc3_some(Value,Value(7)));}
        inline int literal(){auto captured=__objc3_some(Value,Value(7));auto callback=^Maybe(Maybe value){return __objc3_has_value(value)?__objc3_copy(captured):__objc3_none(Value);};auto value=callback(__objc3_none(Value));return __objc3_has_value(value);}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"cpp-carrier-runtime.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.cpp_carrier_runtime","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.cpp_carrier_runtime","test",{},{},{},{"PublishedCppRuntime","PublishedCppRuntimeHost","Error"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kCppCarrierRuntimeSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kCppCarrierRuntimeSemantic));older.push_back("objc3.cxx-carrier-runtime.v1");
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kCppCarrierRuntimeSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto declarations=emit(R"(
      @protocol Error @end
      namespace PublishedCppRuntime {struct Plain {int value;};}
      __attribute__((objc_root_class)) @interface PublishedCppRuntimeHost @end
    )");auto invented=DecodeSemanticInterfaceSidecar(declarations.sidecar_json,declarations.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kCppCarrierRuntimeSemantic)));
    invented->required_semantics.push_back(kCppCarrierRuntimeSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestCheckedCppRecordResultReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string prelude=R"(
      @protocol Error @end
      namespace PublishedCheckedRegister {
        struct Small {int value;};struct Pair {long value,other;};
        struct Mixed {double value;int other;};struct Large {long value,other[4];};
        struct [[clang::trivial_abi]] Owner {long value;explicit Owner(int);Owner(const Owner &);Owner(Owner &&);~Owner() noexcept;};
      }
    )";
    const auto source=prelude+R"(
      namespace PublishedCheckedRegister {
        Small small_source(int) __objc3_throws(id<Error>);
        Pair pair_source(int) __objc3_throws(id<Error>);
        Mixed mixed_source(int) __objc3_throws(id<Error>);
        Large large_source(int) __objc3_throws(id<Error>);
        Owner owner_source(int) __objc3_throws(id<Error>);
        inline Small small(int mode) __objc3_throws(id<Error>){return __objc3_try(small_source(mode));}
        inline Pair pair(int mode) __objc3_throws(id<Error>){return __objc3_try(pair_source(mode));}
        inline Mixed mixed(int mode) __objc3_throws(id<Error>){return __objc3_try(mixed_source(mode));}
        inline Large large(int mode) __objc3_throws(id<Error>){return __objc3_try(large_source(mode));}
        inline Owner owner(int mode) __objc3_throws(id<Error>){return __objc3_try(owner_source(mode));}
        template<class T,class E> inline T block_forward(T (*source)(int) __objc3_throws(E),int mode) __objc3_throws(E) {
          auto factory=^T(int input) __objc3_throws(E){return __objc3_try(source(input));};
          return __objc3_try(factory(mode));
        }
        inline Owner generic_block(int mode) __objc3_throws(id<Error>){return __objc3_try(block_forward<Owner,id<Error>>(owner_source,mode));}
        inline int optional(int mode) {
          auto small=__objc3_try_optional(small_source(mode));auto owner=__objc3_try_optional(owner_source(mode));
          __objc3_try_optional(small_source(mode));__objc3_try_optional(owner_source(mode));
          return __objc3_has_value(small)+__objc3_has_value(owner);
        }
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"checked-cpp-record-results.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.checked_register_results","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.checked_register_results","test",{},{},{},{"PublishedCheckedRegister","Error"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    auto ordinary_source=source;auto attribute=ordinary_source.find("[[clang::trivial_abi]] ");assert(attribute!=std::string::npos);
    ordinary_source.erase(attribute,std::string("[[clang::trivial_abi]] ").size());
    auto ordinary=emit(ordinary_source);
    assert(first.identity.semantic_fingerprint!=ordinary.identity.semantic_fingerprint);

    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kCheckedCppRecordResultSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kCheckedCppRecordResultSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kCheckedCppRecordResultSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto declarations=emit(prelude);auto invented=DecodeSemanticInterfaceSidecar(declarations.sidecar_json,declarations.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kCheckedCppRecordResultSemantic)));
    invented->required_semantics.push_back(kCheckedCppRecordResultSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestCppCarrierDefaultStorageReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string prelude=R"(
      namespace PublishedCheckedMessage {
        struct Value {int value;Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef __objc3_optional(Value) Maybe;
      }
    )";
    const auto source=prelude+R"(
      namespace PublishedCheckedMessage {
        struct Defaults {Maybe value{};Maybe array[2][2]{};};
        inline int array_only(){Maybe values[2][2]{};return __objc3_has_value(values[1][1]);}
        inline int defaults() {Maybe value{};Defaults fields;return __objc3_has_value(value)+__objc3_has_value(fields.array[1][1]);}
        inline void dynamic(int count) {auto *values=new Maybe[count]{__objc3_some(Value,Value(7))};delete[] values;}
        inline void matrix(int count) {auto *values=new Maybe[count][2]{{__objc3_some(Value,Value(7))}};delete[] values;}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"carrier-default-storage.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.carrier_defaults","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.carrier_defaults","test",{},{},{},{"PublishedCheckedMessage"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kCppCarrierStorageSemantic)));
    bool array_only=false;
    for(const auto &d:read->declarations)if(d.qualified_name=="PublishedCheckedMessage::array_only"){assert(HasCppCarrierStorageIdentity(d));array_only=true;}
    assert(array_only);
    auto older=supported;std::erase(older,llvm::StringRef(kCppCarrierStorageSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kCppCarrierStorageSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto declarations=emit(prelude);auto invented=DecodeSemanticInterfaceSidecar(declarations.sidecar_json,declarations.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kCppCarrierStorageSemantic)));
    invented->required_semantics.push_back(kCppCarrierStorageSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestCheckedCppMessageReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string prelude=R"(
      @protocol Error @end
      namespace PublishedCheckedMessage {struct Value {int value;Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};}
      __attribute__((objc_root_class)) @interface PublishedCheckedMessageHost
      @property(nonatomic,readonly) PublishedCheckedMessage::Value value;
      - (PublishedCheckedMessage::Value)value __objc3_throws(id<Error>);
      - (PublishedCheckedMessage::Value)make:(int)mode __objc3_throws(id<Error>);
      @end
    )";
    const auto source=prelude+R"(
      namespace PublishedCheckedMessage {
        inline Value message(PublishedCheckedMessageHost *_Nonnull object,int mode) __objc3_throws(id<Error>) {return __objc3_try([object make:mode]);}
        inline int property(PublishedCheckedMessageHost *_Nonnull object) __objc3_throws(id<Error>) {
          auto value=__objc3_try(object.value);return value.value;
        }
        inline int optional(PublishedCheckedMessageHost *_Nonnull object) {
          auto value=__objc3_try_optional(object.value);__objc3_try_optional([object make:0]);return __objc3_has_value(value);
        }
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"checked-cpp-messages.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.checked_messages","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.checked_messages","test",{},{},{},{"PublishedCheckedMessage","PublishedCheckedMessageHost","Error"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kCheckedCppMessageSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kCheckedCppMessageSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kCheckedCppMessageSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto declarations=emit(prelude);auto invented=DecodeSemanticInterfaceSidecar(declarations.sidecar_json,declarations.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kCheckedCppMessageSemantic)));
    invented->required_semantics.push_back(kCheckedCppMessageSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestAtomicManagedPropertyReplay() {
 for(const char *standard:{"c17","c++20","c++23"}) {
  const bool cpp=std::string(standard)!="c17";
  std::vector<std::string> args={"-x",cpp?"objective-c++":"objective-c",std::string("-std=")+standard,
   "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc-arc","-fblocks",
   "-fobjc3-profile=strict-system","--target=x86_64-unknown-linux-gnu"};
  const std::string source=R"(
   typedef __objc3_optional(id) AtomicMaybe;
   struct AtomicRecord {int generation;AtomicMaybe value;};
   __attribute__((objc_root_class)) @interface PublishedAtomicHost
   @property(atomic) AtomicMaybe value;
   @property(atomic) struct AtomicRecord record;
   @end
   inline int published_atomic_read(PublishedAtomicHost *_Nonnull host){struct AtomicRecord value=host.record;return value.generation+__objc3_has_value(value.value);}
  )";
  auto emit=[&](const std::string &text) {
   auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"atomic-managed-property.m");
   assert(ast && !ast->getDiagnostics().hasErrorOccurred());
   assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.atomic_managed","test",{}));
   auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.atomic_managed","test",{},{},{},{"AtomicMaybe","AtomicRecord","PublishedAtomicHost","published_atomic_read"});
   for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
   assert(result.success);return result;
  };
  auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
  assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
  assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
  const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
  auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
  assert(llvm::is_contained(read->required_semantics,std::string(kAtomicManagedPropertySemantic)));
  auto older=supported;std::erase(older,llvm::StringRef(kAtomicManagedPropertySemantic));
  auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
  std::erase(read->required_semantics,kAtomicManagedPropertySemantic);
  auto missing=EncodeSemanticInterfaceSidecar(*read,supported);assert(!missing);llvm::consumeError(missing.takeError());
  auto rawOnly=emit("typedef __objc3_optional(id) AtomicMaybe;struct AtomicRecord {id object;};__attribute__((objc_root_class)) @interface PublishedAtomicHost @property(atomic) struct AtomicRecord record;@end inline int published_atomic_read(PublishedAtomicHost *_Nonnull host){return host.record.object!=(id)0;}");
  auto rawSidecar=DecodeSemanticInterfaceSidecar(rawOnly.sidecar_json,rawOnly.source_bytes,supported);assert(rawSidecar);
  assert(llvm::is_contained(rawSidecar->required_semantics,std::string(kAtomicManagedPropertySemantic)));
  std::string ordinary=source;size_t at=0;
  while((at=ordinary.find("@property(atomic)",at))!=std::string::npos){ordinary.replace(at,17,"@property(nonatomic)");at+=20;}
  auto changed=emit(ordinary);assert(first.identity.semantic_fingerprint!=changed.identity.semantic_fingerprint);
  auto invented=DecodeSemanticInterfaceSidecar(changed.sidecar_json,changed.source_bytes,supported);assert(invented);
  assert(!llvm::is_contained(invented->required_semantics,std::string(kAtomicManagedPropertySemantic)));
  invented->required_semantics.push_back(kAtomicManagedPropertySemantic);
  auto invalid=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
 }
}

void TestImplicitCheckedRecordReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      @protocol Error @end
      namespace PublishedImplicitChecked {
        struct Leaf {int value;~Leaf() noexcept;};struct Owner {Leaf value;};
        Owner produce() __objc3_throws(id<Error>);
        inline int use() __objc3_throws(id<Error>){auto value=__objc3_try(produce());return value.value.value;}
        struct ARC {id __strong object;id __weak observer;};
        ARC arc() __objc3_throws(id<Error>);
        inline int arc_use() __objc3_throws(id<Error>){auto value=__objc3_try(arc());return value.object==value.observer;}
        template<class T> T generic() __objc3_throws(id<Error>);
        inline Owner instantiate() __objc3_throws(id<Error>){return __objc3_try(generic<Owner>());}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text.find("@protocol Error")==std::string::npos?"@protocol Error @end\n"+text:text,args,"implicit-checked-record.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.implicit_checked_record","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.implicit_checked_record","test",{},{},{},{"PublishedImplicitChecked","Error"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kCheckedCppRecordResultSemantic)));
    assert(std::string(kCheckedCppRecordResultSemantic)=="objc3.checked-cxx-record-results.v2");
    auto older=supported;std::erase(older,llvm::StringRef(kCheckedCppRecordResultSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kCheckedCppRecordResultSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto declarationOnly=emit("@protocol Error @end namespace PublishedImplicitChecked {struct Leaf {~Leaf() noexcept;};struct Owner {Leaf value;};Owner factory() __objc3_throws(id<Error>);}");
    auto declarationSidecar=DecodeSemanticInterfaceSidecar(declarationOnly.sidecar_json,declarationOnly.source_bytes,supported);assert(declarationSidecar);
    assert(llvm::is_contained(declarationSidecar->required_semantics,std::string(kCheckedCppRecordResultSemantic)));
    auto simple=emit("namespace PublishedImplicitChecked {struct Plain {~Plain() noexcept;};__objc3_optional(Plain) factory();}");
    auto invented=DecodeSemanticInterfaceSidecar(simple.sidecar_json,simple.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kCheckedCppRecordResultSemantic)));
    invented->required_semantics.push_back(kCheckedCppRecordResultSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestNativeARCRecordPayloadReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      namespace PublishedARCRecord {
        typedef id __strong Strong;typedef id __weak Weak;typedef int (^Callback)();
        struct Owner {Strong object;Weak observer;Callback callback;};
        typedef __objc3_optional(Owner) Maybe;typedef __objc3_result(Owner,Strong) Result;
        struct UnsafeOwner {id __unsafe_unretained object;};
        typedef __objc3_optional(UnsafeOwner) UnsafeMaybe;
        inline UnsafeMaybe unsafe_copy(const UnsafeMaybe &value){return __objc3_copy(value);}

        inline Maybe make(Strong object,Callback callback){return __objc3_some(Owner,Owner{object,object,callback});}
        inline int use(Strong object,Callback callback) {
          auto value=make(object,callback);auto copied=__objc3_copy(value);auto moved=__objc3_move(copied);
          auto probe=[moved]{return __objc3_has_value(moved);};return probe();
        }
        template<class T>inline __objc3_optional(T) copy(const __objc3_optional(T) &value){return __objc3_copy(value);}
        inline Maybe instantiate(const Maybe &value){return copy(value);}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"arc-record.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.arc_record_payload","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.arc_record_payload","test",{},{},{},{"PublishedARCRecord"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kNativeARCRecordPayloadSemantic)));
    assert(std::string(kNativeARCRecordPayloadSemantic)=="objc3.native-arc-record-payload.v2");
    auto older=supported;std::erase(older,llvm::StringRef(kNativeARCRecordPayloadSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kNativeARCRecordPayloadSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto unsafeOnly=emit("namespace PublishedARCRecord {struct UnsafeOwner {id __unsafe_unretained object;};typedef __objc3_optional(UnsafeOwner) UnsafeMaybe;inline UnsafeMaybe copy(const UnsafeMaybe &value){return __objc3_copy(value);}}");
    auto unsafeSidecar=DecodeSemanticInterfaceSidecar(unsafeOnly.sidecar_json,unsafeOnly.source_bytes,supported);assert(unsafeSidecar);
    assert(llvm::is_contained(unsafeSidecar->required_semantics,std::string(kNativeARCRecordPayloadSemantic)));
    auto simple=emit("namespace PublishedARCRecord {struct Plain {~Plain() noexcept;};__objc3_optional(Plain) factory();}");
    auto invented=DecodeSemanticInterfaceSidecar(simple.sidecar_json,simple.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kNativeARCRecordPayloadSemantic)));
    invented->required_semantics.push_back(kNativeARCRecordPayloadSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestNativeCarrierLambdaReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      namespace PublishedCarrierLambda {
        struct Leaf {int value;explicit Leaf(int);Leaf(const Leaf &);Leaf(Leaf &&);~Leaf() noexcept;};
        typedef id __strong Strong;typedef __objc3_result(Leaf,Strong) Mixed;
        inline int use(const Mixed &source) {
          auto value=__objc3_copy(source);auto callback=[value]{return __objc3_is_ok(value);};
          auto copy=callback;auto moved=static_cast<decltype(copy) &&>(copy);
          auto generic=[value]<class T>(T n){return __objc3_is_ok(value)+n;};return moved()+generic(2);
        }
        template<class T>inline int capture(const T &value){auto callback=[value]{return __objc3_is_ok(value);};return callback();}
        inline int instantiate(const Mixed &value){return capture(value);}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"carrier-lambda.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.carrier_lambda","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.carrier_lambda","test",{},{},{},{"PublishedCarrierLambda"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kNativeCarrierLambdaSemantic)));
    assert(std::string(kNativeCarrierLambdaSemantic)=="objc3.native-carrier-lambda.v1");
    auto older=supported;std::erase(older,llvm::StringRef(kNativeCarrierLambdaSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kNativeCarrierLambdaSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto simple=emit("namespace PublishedCarrierLambda {struct Plain {~Plain() noexcept;};__objc3_optional(Plain) factory();}");
    auto invented=DecodeSemanticInterfaceSidecar(simple.sidecar_json,simple.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kNativeCarrierLambdaSemantic)));
    invented->required_semantics.push_back(kNativeCarrierLambdaSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestMixedCarrierStorageReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      namespace PublishedMixedStorage {
        struct Leaf {int value;explicit Leaf(int);Leaf(const Leaf &);Leaf(Leaf &&);~Leaf() noexcept;};
        typedef id __strong Strong;typedef __objc3_result(Leaf,Strong) Mixed;
        typedef __objc3_optional(Mixed) Maybe;
        struct Record {
          Mixed first,values[2][2];
          explicit Record(int v):first(__objc3_ok(Leaf,Strong,Leaf(v))),
            values{{__objc3_err(Leaf,Strong,(id)0),__objc3_ok(Leaf,Strong,Leaf(v+1))},
                   {__objc3_ok(Leaf,Strong,Leaf(v+2)),__objc3_err(Leaf,Strong,(id)0)}} {}
          Record(const Record &)=default;Record(Record &&)=default;~Record() noexcept=default;
        };
        inline int use(int seed){Record value(seed),copied(value),moved(static_cast<Record &&>(copied));Maybe empty[2][2]={};return __objc3_is_ok(moved.values[1][1])+__objc3_has_value(empty[0][0]);}
        typedef __objc3_optional(Record) Outer;
        inline Outer outer(int seed){return __objc3_some(Record,Record(seed));}
        inline int nested(int seed){auto value=outer(seed);auto copy=__objc3_copy(value);auto moved=__objc3_move(copy);return __objc3_has_value(moved);}
        template<class T>struct Generic {__objc3_result(T,Strong) values[2];};
        inline int generic(const Generic<Leaf> &source){Generic<Leaf> copy(source);return __objc3_is_ok(copy.values[1]);}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"mixed-storage.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.mixed_storage","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.mixed_storage","test",{},{},{},{"PublishedMixedStorage"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kCppCarrierCompositionSemantic)));
    assert(std::string(kCppCarrierCompositionSemantic)=="objc3.cxx-carrier-composition.v2");
    auto older=supported;std::erase(older,llvm::StringRef(kCppCarrierCompositionSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kCppCarrierCompositionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto simple=emit("namespace PublishedMixedStorage {struct Plain {~Plain() noexcept;};__objc3_optional(Plain) factory();}");
    auto invented=DecodeSemanticInterfaceSidecar(simple.sidecar_json,simple.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kCppCarrierCompositionSemantic)));
    invented->required_semantics.push_back(kCppCarrierCompositionSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestCppCarrierCompositionReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      namespace PublishedCarrierStorage {
        struct Leaf {int value;explicit Leaf(int);Leaf(const Leaf &);Leaf(Leaf &&);~Leaf() noexcept;};
        typedef __objc3_optional(Leaf) Maybe;
        struct Scalar {
          Maybe value;
          explicit Scalar(int v):value(__objc3_some(Leaf,Leaf(v))) {}
          Scalar(const Scalar &)=default;Scalar(Scalar &&)=default;~Scalar() noexcept=default;
        };
        struct Array {
          Maybe values[2][2];
          explicit Array(int v):values{{__objc3_some(Leaf,Leaf(v)),__objc3_none(Leaf)},
              {__objc3_none(Leaf),__objc3_some(Leaf,Leaf(v+1))}} {}
          Array(const Array &)=default;Array(Array &&)=default;~Array() noexcept=default;
        };
        typedef __objc3_optional(Scalar) ScalarValue;
        typedef __objc3_optional(Array) ArrayValue;
        inline ArrayValue make(bool some){if(some)return __objc3_some(Array,Array(7));return __objc3_none(Array);}
        inline int use(bool some){auto value=make(some);auto copy=__objc3_copy(value);auto moved=__objc3_move(copy);return __objc3_has_value(moved);}
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"composed-carriers.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.carrier_storage","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.carrier_storage","test",{},{},{},{"PublishedCarrierStorage"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto read=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(read);
    assert(llvm::is_contained(read->required_semantics,std::string(kCppCarrierCompositionSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kCppCarrierCompositionSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    std::erase(read->required_semantics,kCppCarrierCompositionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto simple=emit("namespace PublishedCarrierStorage {struct Plain {~Plain() noexcept;};__objc3_optional(Plain) factory();}");
    auto invented=DecodeSemanticInterfaceSidecar(simple.sidecar_json,simple.source_bytes,supported);assert(invented);
    assert(!llvm::is_contained(invented->required_semantics,std::string(kCppCarrierCompositionSemantic)));
    invented->required_semantics.push_back(kCppCarrierCompositionSemantic);
    auto unsupported=EncodeSemanticInterfaceSidecar(*invented,supported);assert(!unsupported);llvm::consumeError(unsupported.takeError());
  }
}

void TestNativeCppCarrierCallReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
      "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system",
      "-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string prelude=R"(
      @protocol Error @end
      namespace PublishedCarrierCall {
        struct Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef __objc3_optional(Value) Maybe;
        typedef __objc3_result(Value,Value) Result;
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-carrier-call.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.carrier_call","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.carrier_call","test",{},{},{},{"PublishedCarrierCall","Error"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    const std::string source=prelude+R"(
      namespace PublishedCarrierCall {
        Maybe produce(bool);int consume_value(Maybe);
        template<class T>struct Box {T value;~Box() noexcept {}};
        typedef __objc3_optional(Box<int>) MaybeBox;
        MaybeBox box();inline void discard_box(){box();}
        inline Maybe relay(bool some){return produce(some);}
        Maybe checked(int) __objc3_throws(id<Error>);
        inline Maybe checked_relay(int mode) __objc3_throws(id<Error>) {return __objc3_try(checked(mode));}
        inline int checked_optional(int mode) {
          auto value=__objc3_try_optional(checked(mode));
          __objc3_try_optional(checked(mode));return __objc3_has_value(value);
        }
        inline Maybe copy_value(const Maybe &value){return __objc3_copy(value);}
        inline Maybe move_value(Maybe &value){return __objc3_move(value);}
        inline Result result(bool ok){if(ok)return __objc3_ok(Value,Value,Value(7));return __objc3_err(Value,Value,Value(8));}
        inline int use(bool some){auto value=relay(some);auto duplicate=copy_value(value);return consume_value(__objc3_move(duplicate));}
      }
    )";
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto check=[&](const auto &artifact,bool required) {
      auto read=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,supported);assert(read);
      assert(llvm::is_contained(read->required_semantics,std::string(kCppCarrierCallSemantic))==required);
      if(required) {
        auto older=supported;std::erase(older,llvm::StringRef(kCppCarrierCallSemantic));
        auto rejected=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
        std::erase(read->required_semantics,kCppCarrierCallSemantic);
      }else read->required_semantics.push_back(kCppCarrierCallSemantic);
      auto invalid=EncodeSemanticInterfaceSidecar(*read,supported);assert(!invalid);llvm::consumeError(invalid.takeError());
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);check(first,true);
    check(emit(prelude+"namespace PublishedCarrierCall {Maybe signature(bool);}"),true);
    check(emit(prelude+"namespace PublishedCarrierCall {const Maybe &view();}"),false);
    check(emit(prelude+"namespace PublishedCarrierCall {inline void destroy_view(Maybe *value){value->~Maybe();}}"),true);
    check(emit(prelude+"namespace PublishedCarrierCall {inline void destroy_array(Maybe *value){delete[] value;}}"),true);
    check(emit(prelude+R"(
      namespace PublishedCarrierCall {
        inline int body_only(bool some) {
          Maybe (*factory)(bool)=nullptr;auto value=factory(some);return __objc3_has_value(value);
        }
      }
    )"),true);
  }
}

void TestNativeCheckedCppResultReplay() {
  for(const char *standard:{"c++20","c++23"})for(bool arc:{false,true}) {
    std::vector<std::string> args={"-x","objective-c++",std::string("-std=")+standard,
        "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system","-fblocks","-fexceptions","-fobjc-exceptions",
        "--target=x86_64-unknown-linux-gnu"};
    if(arc)args.push_back("-fobjc-arc");
    const std::string source=R"(
      @protocol Error @end
      namespace NativeCheckedResults {
        struct Value {
          int value;explicit Value(int) noexcept;
          Value(const Value &) noexcept;Value(Value &&) noexcept;~Value() noexcept;
        };
        struct MoveOnly {
          int value;explicit MoveOnly(int) noexcept;
          MoveOnly(const MoveOnly &)=delete;MoveOnly(MoveOnly &&) noexcept;~MoveOnly() noexcept;
        };
        Value produce(int) __objc3_throws(id<Error>);
        MoveOnly produce_move(int) __objc3_throws(id<Error>);
        template<class T> T generic_produce(int) __objc3_throws(id<Error>);
        template<class T> T generic_forward(int mode) __objc3_throws(id<Error>) {return __objc3_try(generic_produce<T>(mode));}
        int consume(Value,int) __objc3_throws(id<Error>);
        inline Value forward(int mode) __objc3_throws(id<Error>) {return __objc3_try(produce(mode));}
        inline MoveOnly forward_move(int mode) __objc3_throws(id<Error>) {return __objc3_try(produce_move(mode));}
        inline int use(int mode) __objc3_throws(id<Error>) {
          __objc3_try(produce(mode));
          return __objc3_try(consume(__objc3_try(forward(mode)),mode));
        }
        int produce_integer(int) __objc3_throws(id<Error>);
        struct Aggregate {Value first,second;int tail;};
        struct References {const Value &first;int tail;};
        inline int conditional_use(bool selected,int mode) __objc3_throws(id<Error>) {
          Value value=selected?__objc3_try(produce(mode)):__objc3_try(produce(mode+1));
          return value.value;
        }
        inline int aggregate_use(int mode) __objc3_throws(id<Error>) {
          Aggregate value={__objc3_try(produce(0)),__objc3_try(produce(mode)),__objc3_try(produce_integer(mode))};
          return value.first.value+value.second.value+value.tail;
        }
        inline int reference_use(int mode) __objc3_throws(id<Error>) {
          References value={__objc3_try(produce(0)),__objc3_try(produce_integer(mode))};
          return value.first.value+value.tail;
        }
        typedef __objc3_optional(Value) OptionalValue;
        struct OptionalFields {OptionalValue first,second;};
        inline int optional_use(int mode,bool selected) {
          OptionalValue value=__objc3_try_optional(produce(mode));
          OptionalValue conditional=selected?__objc3_try_optional(produce(0)):__objc3_try_optional(produce(mode));
          const OptionalValue &reference=__objc3_try_optional(produce(mode));
          OptionalValue array[2]={__objc3_try_optional(produce(0)),__objc3_try_optional(produce(mode))};
          OptionalFields fields={__objc3_try_optional(produce(0)),__objc3_try_optional(produce(mode))};
          __objc3_try_optional(produce(mode));
          return __objc3_has_value(value)+__objc3_has_value(conditional)+__objc3_has_value(reference)+__objc3_has_value(array[0])+__objc3_has_value(fields.second);
        }
        inline int optional_move(int mode) {
          auto value=__objc3_try_optional(produce_move(mode));
          auto moved=__objc3_move(value);return __objc3_has_value(moved);
        }
        inline const OptionalValue &static_optional_reference(bool thread,int mode) {
          if(thread) {
            static thread_local const OptionalValue &value=__objc3_try_optional(produce(mode));
            return value;
          }
          static const OptionalValue &value=__objc3_try_optional(produce(mode));return value;
        }
        inline const Value &static_native_reference(bool thread,int mode) {
          if(thread) {
            static thread_local const Value &value=__objc3_try_force(produce(mode));return value;
          }
          static const Value &value=__objc3_try_force(produce(mode));return value;
        }
        typedef Value (^Factory)(int) __objc3_throws(id<Error>);
        inline int block_use(int mode) __objc3_throws(id<Error>) {
          Factory factory=^Value(int value) __objc3_throws(id<Error>) {return __objc3_try(produce(value));};
          Value result=__objc3_try(factory(mode));return result.value;
        }
      }
    )";
    auto emit=[&](const std::string &text) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(text,args,"native-checked-results.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
          "fixture.native_checked_results","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_checked_results","test",{},{},{},{"NativeCheckedResults","Error"});
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(result.success);return result;
    };
    auto first=emit(source),replay=emit(first.source_bytes),relocated=emit("\n\n"+source);
    assert(first.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
    assert(first.identity.semantic_fingerprint==relocated.identity.semantic_fingerprint);
    const auto &catalog=SupportedSemanticInterfaceCapabilities();std::vector<llvm::StringRef> supported(catalog.begin(),catalog.end());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,supported);assert(decoded);
    assert(llvm::is_contained(decoded->required_semantics,std::string(kCxxBlockExpressionSemantic)));
    auto older=supported;std::erase(older,llvm::StringRef(kCxxBlockExpressionSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto consumer=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes+R"(
      int consume_checked_generic(int mode) __objc3_throws(id<Error>) {
        NativeCheckedResults::Value result=__objc3_try(NativeCheckedResults::generic_forward<NativeCheckedResults::Value>(mode));
        return result.value;
      }
    )",args,"native-checked-generic-consumer.mm");
    assert(consumer && !consumer->getDiagnostics().hasErrorOccurred());
  }
}

void TestCheckedIsolatedPropertyReplay() {CheckEffectfulIsolatedPropertyReplay(false);}
void TestAsyncIsolatedPropertyReplay() {CheckEffectfulIsolatedPropertyReplay(true);}

void TestIsolatedPropertyReplay() {
  for (const char *standard : {"c17","c++20","c++23"}) {
    const bool cxx=std::string(standard)!="c17";
    for (bool arc : {false,true}) {
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fblocks",
          "-fobjc-runtime=gnustep-2.2","--target=x86_64-unknown-linux-gnu",
          "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
      if (arc) args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      std::string source="#include <objc3c/runtime/objc3_runtime_services.h>\n";
      source+=R"(
        @protocol Sendable @end
        __attribute__((objc_root_class)) @interface PublishedPropertyRoot <Sendable> @end
        actor class PublishedPropertyActor : PublishedPropertyRoot
        @property(nonatomic,getter=fetchValue,setter=storeValue:) int value;
        - (int)fetchValue __attribute__((nothrow));
        - (void)storeValue:(int)value __attribute__((nothrow));
        @end
        @interface PublishedPropertyHost : PublishedPropertyRoot
        @property(class,nonatomic) int value;
        + (int)value __attribute__((nothrow,objc_executor(main)));
        + (void)setValue:(int)value __attribute__((nothrow,objc_executor(main)));
        @end
        inline int published_class_property(int value) __objc3_async_frame {
          return __objc3_await (PublishedPropertyHost.value=value);
        }
        inline int published_property_get(PublishedPropertyActor *root) __objc3_async_frame {
          return __objc3_await root.value;
        }
        inline int published_property_set(PublishedPropertyActor *root,int value) __objc3_async_frame {
          return __objc3_await (root.value=value);
        }
        inline int published_property_update(PublishedPropertyActor *root,int value) __objc3_async_frame {
          return __objc3_await (root.value+=value);
        }
        inline int published_property_post(PublishedPropertyActor *root) __objc3_async_frame {
          return __objc3_await root.value++;
        }
      )";
      if(cxx)source+=R"(
        template<class T> int published_property_template(T root,int value) __objc3_async_frame {
          return __objc3_await (root.value+=value);
        }
      )";
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.isolated_properties","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.isolated_properties","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        return result;
      };
      auto first=emit(source,"property-provider.m");assert(first.success);
      auto replay=emit(first.source_bytes,"property-replay.m");
      assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-property-provider.m");
      assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> semantics;
      for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
      assert(llvm::is_contained(semantics,llvm::StringRef(kAsyncEntryABISemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
      auto missing=*decoded;std::erase(missing.required_semantics,kAsyncEntryABISemantic);
      auto rejected=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!rejected);llvm::consumeError(rejected.takeError());
      std::string consumer=(cxx?"#include <objc3c/runtime/objc3_task.hpp>\n":"")+first.source_bytes+R"(
        int consume_property(PublishedPropertyActor *root) __objc3_async_frame {
          int result=__objc3_await published_property_update(root,42);
          return result+(__objc3_await root.value);
        }
      )";
      if(cxx)consumer+=R"(
        int consume_property_task(PublishedPropertyActor *root) __objc3_async {
          return __objc3_await (root.value=17);
        }
        int consume_generic_property(PublishedPropertyActor *root) __objc3_async_frame {
          return __objc3_await published_property_template(root,1);
        }
      )";
      auto valid=parse(consumer,"property-consumer.m");assert(valid && !valid->getDiagnostics().hasErrorOccurred());
      auto invalid=parse(first.source_bytes+"int escape_property(PublishedPropertyActor *root){return root.value;}",
                         "unawaited-property-consumer.m");
      assert(invalid && invalid->getDiagnostics().hasErrorOccurred());
      auto changed=source;const auto position=changed.find("storeValue:");assert(position!=std::string::npos);
      while(changed.find("storeValue")!=std::string::npos)changed.replace(changed.find("storeValue"),10,"replaceValue");
      auto altered=emit(changed,"changed-property-provider.m");
      assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    }
  }
}

void TestOwnedNilResultReplay() {
  for (const char *standard : {"c17", "c++20", "c++23"}) {
    const bool cxx=std::string(standard)!="c17";
    for(bool arc:{false,true}) {
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fobjc-version=3","-fblocks",
          "-fobjc-runtime=gnustep-2.2","-fexceptions","-fobjc-exceptions",
          "--target=x86_64-unknown-linux-gnu",
          "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
      if(arc)args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      std::string source=cxx?"#include <objc3c/runtime/objc3_task.hpp>\n"
                             :"#include <objc3c/runtime/objc3_runtime_services.h>\n";
      source+=R"(
        void published_nil_close(int);
        typedef int PublishedNilHandle __attribute__((objc_requires_cleanup(published_nil_close),
            objc_resource_family(PublishedNilHandles),objc_invalid(-1)));
        struct PublishedNilRecord {PublishedNilHandle value;};
      )";
      if(cxx)source+=R"(
        struct PublishedNilValue {
          explicit PublishedNilValue(int);
          PublishedNilValue(const PublishedNilValue &);
          ~PublishedNilValue()noexcept;
        };
      )";
      source+=R"(
        __attribute__((objc_root_class)) @interface PublishedNilRoot
        - (PublishedNilHandle)resource __objc3_async_frame;
        - (struct PublishedNilRecord)record __objc3_async_frame;
      )";
      if(cxx)source+=R"(
        - (PublishedNilValue)valueFrame __objc3_async_frame;
        - (PublishedNilValue)valueTask __objc3_async;
      )";
      source+=R"(
        @end
        inline objc3_async_frame published_nil_resource(PublishedNilRoot *root) {return [root resource];}
        inline objc3_async_frame published_nil_record(PublishedNilRoot *root) {return [root record];}
      )";
      if(cxx)source+=R"(
        inline objc3_async_frame published_nil_value_frame(PublishedNilRoot *root) {return [root valueFrame];}
        inline objc3::task<PublishedNilValue> published_nil_value_task(PublishedNilRoot *root) {return [root valueTask];}
      )";
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                    "fixture.owned_nil_results","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.owned_nil_results","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        return result;
      };
      auto first=emit(source,"owned-nil-provider.m");assert(first.success);
      auto replay=emit(first.source_bytes,"replayed-owned-nil-provider.m");
      assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-owned-nil-provider.m");
      assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> semantics;
      for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
      assert(llvm::is_contained(semantics,llvm::StringRef(kAsyncEntryABISemantic)));
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
      auto old=semantics;std::erase(old,llvm::StringRef(kAsyncEntryABISemantic));old.push_back("objc3.async-entry-abi.v3");
      auto old_reader=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
      assert(!old_reader);llvm::consumeError(old_reader.takeError());
      auto missing=*decoded;std::erase(missing.required_semantics,kAsyncEntryABISemantic);
      auto stripped=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!stripped);llvm::consumeError(stripped.takeError());
      std::string consumer=first.source_bytes+R"(
        objc3_async_frame consume_owned_nil(PublishedNilRoot *root) {return published_nil_resource(root);}
      )";
      if(cxx)consumer+=R"(
        objc3::task<PublishedNilValue> consume_owned_nil_task(PublishedNilRoot *root) {return published_nil_value_task(root);}
      )";
      auto valid=parse(consumer,"owned-nil-consumer.m");
      assert(valid && !valid->getDiagnostics().hasErrorOccurred());

      // A synchronous isolated declaration can synthesize an async entry in a
      // consumer. Its contract needs the same cutover without an explicit async
      // declaration elsewhere in the interface.
      const std::string implicit=R"(
        void implicit_nil_close(int);
        typedef int ImplicitNilHandle __attribute__((objc_requires_cleanup(implicit_nil_close),
            objc_resource_family(ImplicitNilHandles),objc_invalid(-1)));
        __attribute__((objc_root_class)) @interface ImplicitNilRoot
        - (ImplicitNilHandle)resource __attribute__((nothrow,objc_executor(main)));
        @end
      )";
      auto isolated=emit(implicit,"implicit-nil-provider.m");assert(isolated.success);
      auto isolated_replay=emit(isolated.source_bytes,"implicit-nil-replay.m");
      assert(isolated_replay.success && isolated_replay.identity.semantic_fingerprint==isolated.identity.semantic_fingerprint);
      auto isolated_metadata=llvm::json::parse(isolated.sidecar_json);assert(isolated_metadata);
      std::vector<llvm::StringRef> isolated_semantics;
      for(const auto &value:*isolated_metadata->getAsObject()->getArray("required_semantics"))
        isolated_semantics.push_back(*value.getAsString());
      auto isolated_sidecar=DecodeSemanticInterfaceSidecar(isolated.sidecar_json,isolated.source_bytes,isolated_semantics);
      assert(isolated_sidecar && llvm::is_contained(isolated_sidecar->required_semantics,kAsyncEntryABISemantic));
      auto isolated_old_semantics=isolated_semantics;
      std::erase(isolated_old_semantics,llvm::StringRef(kAsyncEntryABISemantic));
      isolated_old_semantics.push_back("objc3.async-entry-abi.v3");
      auto isolated_old=DecodeSemanticInterfaceSidecar(isolated.sidecar_json,isolated.source_bytes,isolated_old_semantics);
      assert(!isolated_old);llvm::consumeError(isolated_old.takeError());
      auto isolated_missing=*isolated_sidecar;
      std::erase(isolated_missing.required_semantics,kAsyncEntryABISemantic);
      auto isolated_stripped=EncodeSemanticInterfaceSidecar(isolated_missing,isolated_semantics);
      assert(!isolated_stripped);llvm::consumeError(isolated_stripped.takeError());
    }
  }
}

void TestStrictAsyncProfileReplay() {
  for (const char *standard : {"c++20", "c++23"}) {
    auto plain=clang::tooling::buildASTFromCodeWithArgs("int allocation_policy;",
        {"-x","c++",std::string("-std=")+standard});
    assert(plain && !plain->getDiagnostics().hasErrorOccurred());
    assert(!plain->getASTContext().getLangOpts().CoroAlignedAllocation);
  }
  for (const char *profile : {"strict", "strict-concurrency", "strict-system"}) {
    for (const char *standard : {"c17", "c++20", "c++23"}) {
      const bool cxx = std::string(standard) != "c17";
      for (bool arc : {false, true}) {
        std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
            std::string("-std=")+standard,"-fobjc-version=3","-fblocks",
            "-fobjc-runtime=gnustep-2.2",std::string("-fobjc3-profile=")+profile,
            "-fexceptions","-fobjc-exceptions","-fobjc-arc-exceptions",
            "--target=x86_64-unknown-linux-gnu",
            "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
        if (arc) args.push_back("-fobjc-arc");
        clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,cxx);
        auto parse=[&](const std::string &text,const char *name) {
          return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
              std::make_shared<clang::PCHContainerOperations>(),
              clang::tooling::getClangStripDependencyFileAdjuster(),files);
        };
        std::string source = cxx ? "#include <objc3c/runtime/objc3_task.hpp>\n"
                                 : "#include <objc3c/runtime/objc3_runtime_services.h>\n";
        source += R"(
          inline int strict_frame_leaf(int value) __objc3_async_frame {return value+1;}
          inline int strict_frame_parent(int value) __objc3_async_frame {
            return __objc3_await strict_frame_leaf(value);
          }
        )";
        std::vector<std::string> exports={"strict_frame_leaf","strict_frame_parent"};
        if (cxx) {
          source += R"(
            inline int strict_task_leaf(int value) __objc3_async {return value+1;}
            template<class T> int strict_task_generic(T value) __objc3_async {
              return __objc3_await strict_task_leaf(int(value));
            }
          )";
          exports.push_back("strict_task_leaf");exports.push_back("strict_task_generic");
        }
        auto emit=[&](const std::string &text,const char *name) {
          auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
          assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                      "fixture.strict_async_profiles","test",{}));
          auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.strict_async_profiles","test",{},{},{},exports);
          for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
          return result;
        };
        if (cxx) {
          auto aligned=parse("int strict_alignment_policy;","aligned-policy.mm");
          assert(aligned && !aligned->getDiagnostics().hasErrorOccurred());
          const auto aligned_profile=ReadInterfaceLanguageProfile(aligned->getASTContext());
          args.push_back("-fno-coro-aligned-allocation");
          auto unaligned=parse("int strict_alignment_policy;","unaligned-policy.mm");
          args.pop_back();
          assert(unaligned && !unaligned->getDiagnostics().hasErrorOccurred());
          const auto unaligned_profile=ReadInterfaceLanguageProfile(unaligned->getASTContext());
          assert(aligned_profile!=unaligned_profile);
          assert(aligned_profile.find("coroutine-aligned-allocation=true")!=std::string::npos);
          assert(unaligned_profile.find("coroutine-aligned-allocation=false")!=std::string::npos);
        }
        auto first=emit(source,"strict-async-provider.m");assert(first.success);
        assert(first.identity.language_profile.find(profile)!=std::string::npos);
        auto replay=emit(first.source_bytes,"replayed-strict-async-provider.m");
        assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
        auto relocated=emit("\n\n"+source,"relocated-strict-async-provider.m");
        assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
        std::string consumer=first.source_bytes+R"(
          objc3_async_frame strict_frame_consumer(void) {return strict_frame_parent(41);}
        )";
        if(cxx)consumer+=R"(
          objc3::task<int> strict_task_consumer(void) {return strict_task_generic(41L);}
        )";
        auto valid=parse(consumer,"strict-async-consumer.m");
        assert(valid && !valid->getDiagnostics().hasErrorOccurred());
      }
    }
  }
}

void TestNativeDependentAwaitReplay() {
  std::ifstream input(std::filesystem::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/explicit_cxx_storage.h");
  assert(input);const std::string source((std::istreambuf_iterator<char>(input)),{});
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    for(bool arc:{false,true}) {
      std::vector<std::string> args={"-x","objective-c++",standard,"-fobjc-version=3",
          "-fobjc-runtime=gnustep-2.2","-fobjc3-strict-concurrency",
          "--target=x86_64-unknown-linux-gnu","-fcoro-aligned-allocation",
          "-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
      if(arc)args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
      auto parse=[&](const std::string &text,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(text,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &text,const char *name) {
        auto ast=parse(text,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                     "fixture.native_dependent_await","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.native_dependent_await","test",{},{},{},
                                     {"NativeExplicitStorage"});
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        return result;
      };
      auto first=emit(source,"native-dependent-await.mm");assert(first.success);
      auto replay=emit(first.source_bytes,"replayed-native-dependent-await.mm");
      assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-native-dependent-await.mm");
      assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
      std::vector<llvm::StringRef> semantics;
      for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
      auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
      assert(std::any_of(decoded->declarations.begin(),decoded->declarations.end(),HasNativeDependentAwaitIdentity));
      assert(first.source_bytes.find("MemberEntries")!=std::string::npos);
      assert(std::any_of(decoded->declarations.begin(),decoded->declarations.end(),[](const auto &declaration) {
        return declaration.qualified_name=="NativeExplicitStorage::member_generic";
      }));
      auto missing=*decoded;std::erase(missing.required_semantics,kNativeDependentAwaitSemantic);
      auto absent=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!absent);llvm::consumeError(absent.takeError());
      auto old=semantics;std::erase(old,llvm::StringRef(kNativeDependentAwaitSemantic));
      auto old_reader=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
      assert(!old_reader);llvm::consumeError(old_reader.takeError());
      auto ordinary=emit(R"(
        #include <objc3c/runtime/objc3_task.hpp>
        namespace NativeExplicitStorage {
          template<class Work> objc3::task<int> ordinary(Work work) {
            co_await work;co_return 42;
          }
        }
      )","ordinary-dependent-await.mm");
      assert(ordinary.success && ordinary.sidecar_json.find(kNativeDependentAwaitSemantic)==std::string::npos);
      assert(ordinary.source_bytes.find("co_await work")!=std::string::npos);
      const std::string task_only_source=R"(
        #include <objc3c/runtime/objc3_task.hpp>
        namespace NativeExplicitStorage {
          template<class Work> int task_only(Work work) __objc3_async {
            return __objc3_await work;
          }
        }
      )";
      auto task_only=emit(task_only_source,"native-task-only-await.mm");
      assert(task_only.success && task_only.sidecar_json.find(kNativeDependentAwaitSemantic)!=std::string::npos);
      auto task_replay=emit(task_only.source_bytes,"native-task-only-await-replay.mm");
      assert(task_replay.success && task_replay.identity.semantic_fingerprint==task_only.identity.semantic_fingerprint);
      assert(task_only.source_bytes.find("__objc3_await work")!=std::string::npos);
      auto ordinary_task_source=task_only_source;
      auto native_keyword=ordinary_task_source.find("__objc3_await");assert(native_keyword!=std::string::npos);
      ordinary_task_source.replace(native_keyword,std::string("__objc3_await").size(),"co_await");
      auto ordinary_task=emit(ordinary_task_source,"ordinary-task-only-await.mm");
      assert(ordinary_task.success && ordinary_task.sidecar_json.find(kNativeDependentAwaitSemantic)==std::string::npos);
      assert(ordinary_task.identity.semantic_fingerprint!=task_only.identity.semantic_fingerprint);
      auto plain=emit("namespace NativeExplicitStorage{inline int plain(){return 1;}}","plain-native-await.mm");
      assert(plain.success);
      auto control=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,semantics);assert(control);
      control->required_semantics.push_back(kNativeDependentAwaitSemantic);
      auto invented=EncodeSemanticInterfaceSidecar(*control,semantics);assert(!invented);llvm::consumeError(invented.takeError());
      auto valid=parse(first.source_bytes+R"(
        objc3_async_frame use_native_dependent_await(NativeExplicitStorage::Counts *counts) {
          return NativeExplicitStorage::generic(NativeExplicitStorage::Value(counts,42));
        }
        objc3_async_frame use_native_member_entries(NativeExplicitStorage::Counts *counts) {
          return NativeExplicitStorage::member_generic(NativeExplicitStorage::Value(counts,42));
        }
        objc3::task<int> use_dependent_task_await(NativeExplicitStorage::Counts *counts) {
          return NativeExplicitStorage::task_generic(NativeExplicitStorage::Value(counts,42));
        }
        objc3::task<int> use_manual_factory(NativeExplicitStorage::Counts *counts) {
          return NativeExplicitStorage::raw_factory_group(NativeExplicitStorage::Value(counts,42));
        }
        objc3::task<int> use_task_function_factory() {
          return NativeExplicitStorage::function_factory_group();
        }
      )","native-dependent-await-consumer.mm");
      assert(valid && !valid->getDiagnostics().hasErrorOccurred());
    }
  }
}

void TestConcurrentBlockValueReplay() {
  for (const char *standard : {"c17", "c++20", "c++23"}) {
    const bool cxx=std::string(standard)!="c17";
    for (bool arc : {false,true}) {
      std::vector<std::string> args={"-x",cxx?"objective-c++":"objective-c",
          std::string("-std=")+standard,"-fblocks","-fobjc-version=3",
          "-fobjc-runtime=gnustep-2.2","-fobjc3-strict-concurrency",
          "--target=x86_64-unknown-linux-gnu"};
      if (arc) args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args);
      auto parse=[&](const std::string &source,const char *file) {
        return clang::tooling::buildASTFromCodeWithArgs(source,args,file,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &source,const char *file) {
        auto ast=parse(source,file);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                     "fixture.concurrent_block_values","test",{}));
        auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.concurrent_block_values","test");
        for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
        assert(result.success);return result;
      };
      const std::string source=R"(
        #include <objc3c/runtime/objc3_runtime_services.h>
        @protocol Sendable @end
        __attribute__((objc_root_class)) @interface PublishedBlockRoot @end
        @interface PublishedBlockSafe:PublishedBlockRoot<Sendable> @end
        typedef void (^PublishedAsyncWork)(void) __objc3_async_frame;
        int published_start(PublishedAsyncWork) __attribute__((objc_task_spawn));
        inline int published_block_choice(PublishedBlockSafe *value,int choose) {
          PublishedAsyncWork const first=^void(void) __objc3_async_frame {(void)value;};
          PublishedAsyncWork const second=^void(void) __objc3_async_frame {};
          PublishedAsyncWork const choice=choose?first:second;
          return published_start(choose?choice:choice);
        }
        inline int published_nested_block(PublishedBlockSafe *value,int choose) {
          void (^const first)(void)=^{(void)value;};
          void (^const second)(void)=^{};
          void (^const callback)(void)=choose?first:second;
          return published_start(^void(void) __objc3_async_frame {callback();});
        }
        inline int published_strong_block(PublishedBlockSafe *value) {
          __unsafe_unretained PublishedBlockSafe *source=value;
          return published_start(^[strong source] void(void) __objc3_async_frame {(void)source;});
        }
      )";
      auto first=emit(source,"block-value-provider.m");
      auto replay=emit(first.source_bytes,"block-value-replay.m");
      assert(replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-block-value-provider.m");
      assert(relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto changed=source;auto at=changed.find("choose?first:second");assert(at!=std::string::npos);
      changed.replace(at,std::string("choose?first:second").size(),"choose?second:first");
      assert(emit(changed,"changed-block-value-provider.m").identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
      auto valid=parse(first.source_bytes+R"(
        int published_block_consumer(PublishedBlockSafe *value,int choose) {
          return published_block_choice(value,choose)+published_nested_block(value,choose)+published_strong_block(value);
        }
      )","block-value-consumer.m");
      assert(valid && !valid->getDiagnostics().hasErrorOccurred());
      auto invalid=parse(first.source_bytes+R"(
        int published_block_bad(PublishedBlockRoot *value,int choose) {
          PublishedAsyncWork const safe=^void(void) __objc3_async_frame {};
          PublishedAsyncWork const bad=^void(void) __objc3_async_frame {(void)value;};
          return published_start(choose?safe:bad);
        }
      )","block-value-invalid-consumer.m");
      assert(invalid && invalid->getDiagnostics().hasErrorOccurred());
    }
  }
}

void TestConcurrentCallableCaptureReplay() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    for (bool arc : {false, true}) {
      std::vector<std::string> args={"-x","objective-c++",standard,"-fblocks",
          "-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc-weak",
          "-fobjc3-strict-concurrency","--target=x86_64-unknown-linux-gnu",
          "-fcoro-aligned-allocation","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
      if (arc) args.push_back("-fobjc-arc");
      clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
      auto parse=[&](const std::string &source,const char *name) {
        return clang::tooling::buildASTFromCodeWithArgs(source,args,name,"clang-tool",
            std::make_shared<clang::PCHContainerOperations>(),
            clang::tooling::getClangStripDependencyFileAdjuster(),files);
      };
      auto emit=[&](const std::string &source,const char *name) {
        auto ast=parse(source,name);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
        assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                     "fixture.concurrent_captures","test",{}));
        return EmitSemanticInterface(ast->getASTContext(),"fixture.concurrent_captures","test",{},{},{},
                                     {"ConcurrentCaptures","ConcurrentCaptureRoot","ConcurrentCaptureSafe","Sendable"});
      };
      const std::string source=R"(
        #include <objc3c/runtime/objc3_task.hpp>
        @protocol Sendable @end
        __attribute__((objc_root_class)) @interface ConcurrentCaptureRoot @end
        @interface ConcurrentCaptureSafe:ConcurrentCaptureRoot<Sendable> @end
        namespace ConcurrentCaptures {
          template<class Work> int start(Work) __attribute__((objc_task_spawn)) {return 1;}
          template<class Work> inline int launch(Work work) {return start(work);}
          inline int object(ConcurrentCaptureSafe *value) {
            return launch([value]()->int __objc3_async_frame {return value!=0;});
          }
          typedef void (^Work)(void) __objc3_async_frame;
          int start_block(Work) __attribute__((objc_task_spawn));
          inline int aliases(ConcurrentCaptureSafe *value) {
            Work const first=^void(void) __objc3_async_frame {(void)value;};
            Work const second=first;
            Work const third=second;
            return start_block(third);
          }
        }
      )";
      auto first=emit(source,"concurrent-captures.mm");assert(first.success);
      auto replay=emit(first.source_bytes,"replayed-concurrent-captures.mm");
      assert(replay.success && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto relocated=emit("\n\n"+source,"relocated-concurrent-captures.mm");
      assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
      auto valid=parse(first.source_bytes+R"(
        int valid_capture(int n) {
          return ConcurrentCaptures::launch([n]()->int __objc3_async_frame {return n;});
        }
      )","valid-concurrent-consumer.mm");
      assert(valid && !valid->getDiagnostics().hasErrorOccurred());
      auto invalid=parse(first.source_bytes+R"(
        int invalid_capture(__unsafe_unretained ConcurrentCaptureSafe *value) {
          return ConcurrentCaptures::launch([value]()->int __objc3_async_frame {return value!=0;});
        }
      )","invalid-concurrent-consumer.mm");
      assert(invalid && invalid->getDiagnostics().hasErrorOccurred());
    }
  }
}

void TestEffectfulLambdas() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    std::vector<std::string> args={"-x","objective-c++",standard,"-fobjc-version=3",
        "-fobjc-runtime=gnustep-2.2","--target=x86_64-unknown-linux-gnu",
        "-fcoro-aligned-allocation","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
    auto emit=[&](const std::string &source,const char *file="effectful-lambdas.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,file,"clang-tool",
          std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),files);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.effectful_lambdas","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.effectful_lambdas","test",{},{},{},{"Members","MemberError","Error"});
    };
    const std::string source=R"(
      #include <objc3c/runtime/objc3_task.hpp>
      @protocol Error @end
      @protocol MemberError<Error> @end

namespace Members {
  inline int pause(int n) __objc3_async {return n;}
  inline int task(int n) __objc3_async {
    auto next=[bias=2]<class T>(T value)->T __objc3_async {
      __objc3_await objc3::task_yield;return value+bias;
    };
    return __objc3_await next(n);
  }
  inline int frame(int n) __objc3_async_frame {
    auto next=[bias=2]<class T>(T value)->T __objc3_async_frame {
      (void)__objc3_await pause(0);return value+bias;
    };
    return __objc3_await next(n);
  }
  inline int checked(int n,id<MemberError> error) __objc3_throws(id<MemberError>) {
    auto next=[]<class E>(int value,E error)->int __objc3_throws(E) {
      if(error)__objc3_throw(error);return value;
    };
    return __objc3_try(next(n,error));
  }
  inline int checked_task(int n,id<MemberError> error) __objc3_async __objc3_throws(id<MemberError>) {
    auto next=[]<class E>(int value,E error)->int __objc3_async __objc3_throws(E) {
      __objc3_await objc3::task_yield;if(error)__objc3_throw(error);return value;
    };
    return __objc3_try(__objc3_await next(n,error));
  }
  inline auto factory(int bias) {
    return [bias](int n)->int __objc3_async {__objc3_await objc3::task_yield;return bias+n;};
  }
  struct Owned {
    int value;Owned(int n):value(n){}Owned(const Owned &other):value(other.value){}~Owned(){}
  };
  inline auto owned_factory(Owned value) {
    return [value](int n)->int __objc3_async {__objc3_await objc3::task_yield;return value.value+n;};
  }
}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxEffectfulLambdaSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-effectful-lambdas.mm");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-effectful-lambdas.mm");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxEffectfulLambdaIdentity)==6);
    auto wrong_owner=*decoded;
    auto owner=std::find_if(wrong_owner.declarations.begin(),wrong_owner.declarations.end(),HasCxxEffectfulLambdaIdentity);
    assert(owner!=wrong_owner.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong_owner,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxEffectfulLambdaSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto changed=source;auto position=changed.find("value+bias");assert(position!=std::string::npos);
    changed.replace(position,10,"value+bias+1");auto altered=emit(changed);
    assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxEffectfulLambdaSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
  }
}



void TestTemplateCallableProjections() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    std::vector<std::string> args={"-x","objective-c++",standard,"-fobjc-version=3",
        "-fobjc-runtime=gnustep-2.2","--target=x86_64-unknown-linux-gnu",
        "-fcoro-aligned-allocation","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
    auto emit=[&](const std::string &source,const char *file="template-callable-projections.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,file,"clang-tool",
          std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),files);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.callable_projections","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.callable_projections","test",{},{},{},{"Members","MemberError","Error"});
    };
    const std::string source=R"(
      #include <objc3c/runtime/objc3_task.hpp>
      @protocol Error @end
      @protocol MemberError<Error> @end

namespace Members {
  inline int pause(int n) __objc3_async {return n;}
  template<class T>T task(T n) __objc3_async {__objc3_await objc3::task_yield;return n;}
  template<>inline int task<int>(int n) __objc3_async {__objc3_await objc3::task_yield;return n+2;}
  template<class T>T frame(T n) __objc3_async_frame {(void)__objc3_await pause(0);return n;}
  template<>inline int frame<int>(int n) __objc3_async_frame {(void)__objc3_await pause(0);return n+2;}
  template<class T>T provider(T n) __objc3_async {return n;}
  template<>int provider<int>(int n) __objc3_async {__objc3_await objc3::task_yield;return n+2;}
  template<class T>using Callback=T(*)(T) __objc3_async;
  template<class T>using FrameCallback=T(*)(T) __objc3_async_frame;
  template<class T>inline Callback<T> callback=&task<T>;
  template<>inline Callback<int> callback<int> = &task<int>;
  template<class T>inline FrameCallback<T> frame_callback=&frame<T>;
  template<class T>struct Family {
    template<class U>using Callback=U(*)(U) __objc3_async;
    template<class U>inline static Callback<U> callback=&task<U>;
    template<class U>U call(U n)const __objc3_async {return n;}
  };
  template<>template<>inline int Family<int>::call<int>(int n)const __objc3_async {
    __objc3_await objc3::task_yield;return n+2;
  }
  struct Concrete {
    template<class U>U call(U n)const __objc3_async {return n;}
    template<class U>U framed(U n)const __objc3_async_frame {return n;}
  };
  template<>inline int Concrete::call<int>(int n)const __objc3_async {
    __objc3_await objc3::task_yield;return n+2;
  }
  template<>inline int Concrete::framed<int>(int n)const __objc3_async_frame {
    (void)__objc3_await pause(0);return n+2;
  }
  template<class T>using Member=T(Concrete::*)(T) const __objc3_async;
  template<class T>inline Member<T> member=&Concrete::call<T>;
  template<class T>inline Callback<T> partial=&task<T>;
  template<class T>inline Callback<T*> partial<T*> = &task<T*>;
  template<class T>inline Callback<T> stored=&task<T>;
  template<>Callback<int> stored<int> = &task<int>;
  template<class E>int dependent(int n,E error) __objc3_throws(E) {
    if(error)__objc3_throw(error);return n;
  }
  template<class E>int dependent_task(int n,E error) __objc3_async __objc3_throws(E) {
    __objc3_await objc3::task_yield;if(error)__objc3_throw(error);return n;
  }
  template<class E>using Checked=int(*)(int,E) __objc3_throws(E);
  template<class E>inline Checked<E> checked=&dependent<E>;
}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxTemplateCallableProjectionSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-template-callable-projections.mm");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-template-callable-projections.mm");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxTemplateCallableProjectionIdentity)>=10);
    for (const auto &declaration:decoded->declarations) {
      const auto &name=declaration.qualified_name;
      if (name=="Members::Callback" || name=="Members::Member" || name=="Members::member")
        assert(declaration.effects.is_async && declaration.effects.async_abi_identity.find("cxx-task-v1")!=std::string::npos);
      if (name=="Members::FrameCallback")
        assert(declaration.effects.is_async && declaration.effects.async_abi_identity.find("c-frame-v1")!=std::string::npos);
      if (name=="Members::dependent" || name=="Members::dependent_task" || name=="Members::Checked")
        assert(declaration.effects.is_throwing && declaration.effects.throws_kind=="typed");
    }
    auto wrong_owner=*decoded;
    auto owner=std::find_if(wrong_owner.declarations.begin(),wrong_owner.declarations.end(),HasCxxTemplateCallableProjectionIdentity);
    assert(owner!=wrong_owner.declarations.end());owner->kind="Namespace";
    auto invalid_owner=EncodeSemanticInterfaceSidecar(wrong_owner,semantics);assert(!invalid_owner);llvm::consumeError(invalid_owner.takeError());
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxTemplateCallableProjectionSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto changed=source;auto position=changed.find("return n+2;");assert(position!=std::string::npos);
    changed.replace(position,11,"return n+3;");auto altered=emit(changed);
    assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto private_change=source;position=private_change.find("return n+2;",private_change.find("template<>int provider<int>"));
    assert(position!=std::string::npos);private_change.replace(position,11,"return n+3;");auto private_altered=emit(private_change);
    assert(private_altered.success && private_altered.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxTemplateCallableProjectionSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
  }
}


void TestEffectfulTemplateMembers() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    std::vector<std::string> args={"-x","objective-c++",standard,"-fobjc-version=3",
        "-fobjc-runtime=gnustep-2.2","--target=x86_64-unknown-linux-gnu",
        "-fcoro-aligned-allocation","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
    auto emit=[&](const std::string &source,const char *file="effectful-template-members.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,file,"clang-tool",
          std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),files);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.effectful_members","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.effectful_members","test",{},{},{},{"Members","MemberError","Error"});
    };
    const std::string source=R"(
      #include <objc3c/runtime/objc3_task.hpp>
      @protocol Error @end
      @protocol MemberError<Error> @end
      namespace Members {
        inline int pause(int n) __objc3_async {__objc3_await objc3::task_yield;return n;}
        template<class T>struct Family {
          T value;
          T task()const __objc3_async {__objc3_await objc3::task_yield;return value;}
          T frame()const __objc3_async_frame {(void)__objc3_await pause(0);return value;}
          template<class U>U echo(U n)const __objc3_async {__objc3_await objc3::task_yield;return n;}
          template<class U>U framed(U n)const __objc3_async_frame {(void)__objc3_await pause(0);return n;}
          T checked(id<MemberError> error)const __objc3_async __objc3_throws(id<MemberError>) {
            __objc3_await objc3::task_yield;if(error)__objc3_throw(error);return value;
          }
        };
        struct Concrete {
          template<class U>U task(U n)const __objc3_async {__objc3_await objc3::task_yield;return n;}
          template<class U>U frame(U n)const __objc3_async_frame {(void)__objc3_await pause(0);return n;}
          template<class U>U checked(U n,id<MemberError> error)const __objc3_throws(id<MemberError>) {
            if(error)__objc3_throw(error);return n;
          }
        };
      }
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxEffectfulTemplateMemberSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-effectful-template-members.mm");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-effectful-template-members.mm");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxEffectfulTemplateMemberIdentity)==2);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxEffectfulTemplateMemberSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxEffectfulTemplateMemberSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
  }
}

void TestEffectfulFunctionTemplates() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    std::vector<std::string> args={"-x","objective-c++",standard,"-fobjc-version=3",
        "-fobjc-runtime=gnustep-2.2","--target=x86_64-unknown-linux-gnu",
        "-fcoro-aligned-allocation","-resource-dir=" OBJC3_INTERFACE_TEST_RESOURCE_DIR};
    clang::tooling::FileContentMappings files;AddRuntimeInterfaceFiles(files,args,true);
    auto emit=[&](const std::string &source,const char *file="effectful-function-templates.mm") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,file,"clang-tool",
          std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),files);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.effectful_templates","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.effectful_templates","test",{},{},{},{"Effects","TemplateEffectError","Error"});
    };
    const std::string source=R"(
      #include <objc3c/runtime/objc3_task.hpp>
      @protocol Error @end
      @protocol TemplateEffectError<Error> @end
      namespace Effects {
        inline int pause(int n) __objc3_async {__objc3_await objc3::task_yield;return n;}
        template<class T>T task(T value) __objc3_async {
          __objc3_await objc3::task_yield;return value;
        }
        template<class T>T frame(T value) __objc3_async_frame {
          (void)__objc3_await pause(0);return value;
        }
        template<class T>T checked(T value,id<TemplateEffectError> error) __objc3_throws(id<TemplateEffectError>) {
          if(error)__objc3_throw(error);return value;
        }
        template<class T>T checked_task(T value,id<TemplateEffectError> error) __objc3_async __objc3_throws(id<TemplateEffectError>) {
          __objc3_await objc3::task_yield;if(error)__objc3_throw(error);return value;
        }
      }
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxEffectfulFunctionTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-effectful-function-templates.mm");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-effectful-function-templates.mm");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxEffectfulFunctionTemplateIdentity)==4);
    for(const auto &declaration:decoded->declarations) {
      if(declaration.qualified_name=="Effects::frame")
        assert(declaration.effects.is_async && declaration.effects.async_abi_identity.find("c-frame-v1")!=std::string::npos);
      if(declaration.qualified_name=="Effects::task" || declaration.qualified_name=="Effects::checked_task")
        assert(declaration.effects.is_async && declaration.effects.async_abi_identity.find("cxx-task-v1")!=std::string::npos);
      if(declaration.qualified_name=="Effects::checked" || declaration.qualified_name=="Effects::checked_task")
        assert(declaration.effects.is_throwing && declaration.effects.throws_kind=="typed");
    }
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxEffectfulFunctionTemplateSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxEffectfulFunctionTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
  }
}

void TestClosedParentPatterns() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="closed-parent-patterns.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.closed_patterns","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.closed_patterns","test");
    };
    const std::string source=R"(
      namespace N {
        template<class T>struct Outer {
          template<class U>struct Inner {int get()const{return 1;}};
          template<class U>inline static int value=1;
          template<class U>inline static thread_local int local=1;
          template<class U>int call(U n)const{return n;}
        };
      }
      template<>template<class U>struct N::Outer<int>::Inner {U value;int get()const{return value+2;}};
      template<>template<class U>inline int N::Outer<int>::value=42;
      template<>template<class U>inline thread_local int N::Outer<int>::local=42;
      template<>template<class U>int N::Outer<int>::call(U n)const{return n+2;}
      template<>template<class U>struct N::Outer<long>::Inner<U*> {U *value;int get()const{return *value+2;}};
      template<>template<class U>inline int N::Outer<long>::value<U*> =42;
      template<>template<class U>inline thread_local int N::Outer<long>::local<U*> =42;
      inline int probe(){int n=40;return N::Outer<int>::Inner<long>{40}.get()+N::Outer<int>::value<long>+N::Outer<int>{}.call(40L)+N::Outer<long>::Inner<int*>{&n}.get()+N::Outer<long>::value<int*>+N::Outer<long>::value<int>;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxClosedParentPatternSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-closed-parent-patterns.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-closed-parent-patterns.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxClosedParentPatternIdentity)==1);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxClosedParentPatternSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxClosedParentPatternSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("n+2"),3,"n+3");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  }
}

void TestClosedParentMemberSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="closed-parent-members.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.closed_parents","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.closed_parents","test");
    };
    const std::string source=R"(
      namespace N {
        template<class T>struct Outer {
          template<class U>struct Inner {int get()const{return 1;}};
          template<class U>inline static int value=1;
          template<class U>inline static thread_local int local=1;
          template<class U>int call(U n)const{return n;}
        };
      }
      template<>struct N::Outer<double> {
        template<class U>struct Inner {int get()const{return 1;}};
        template<class U>inline static int value=1;
        template<class U>int call(U n)const{return n;}
      };
      template<>struct N::Outer<double>::Inner<long> {int get()const{return 42;}};
      template<>inline int N::Outer<double>::value<long> =42;
      template<>inline int N::Outer<double>::call<long>(long n)const{return n+2;}
      template<>template<>struct N::Outer<int>::Inner<long> {int get()const{return 42;}};
      template<>template<>inline int N::Outer<int>::value<long> =42;
      template<>template<>inline thread_local int N::Outer<int>::local<long> =42;
      template<>template<>inline int N::Outer<int>::call<long>(long n)const{return n+2;}
      inline int probe(){return N::Outer<double>::Inner<long>{}.get()+N::Outer<double>::value<long>+N::Outer<double>{}.call(40L)+N::Outer<int>::Inner<long>{}.get()+N::Outer<int>::value<long>+N::Outer<int>{}.call(40L)+N::Outer<long>{}.call(42L);}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxClosedParentMemberSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-closed-parent-members.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-closed-parent-members.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxClosedParentMemberIdentity)==1);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxClosedParentMemberSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxClosedParentMemberSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("n+2"),3,"n+3");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  }
}

void TestDependentMemberPartials() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="dependent-member-partials.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.dependent_partials","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.dependent_partials","test");
    };
    const std::string source=R"(
      namespace N {
        template<class T>concept Number=sizeof(T)>0;
        template<class T>struct Outer {
          template<class U>struct Inner {U value;int get()const{return 1;}};
          template<class U>inline static int number=1;
          template<class U>inline static thread_local int local=1;
          template<int V,bool Positive=true>struct Count {int get()const{return 1;}};
        };
      }
      template<class T>template<N::Number U>struct N::Outer<T>::Inner<U*> {
        U *value;int get()const{return 42;}
      };
      template<class T>template<N::Number U>inline int N::Outer<T>::number<U*> =42;
      template<class T>template<N::Number U>inline thread_local int N::Outer<T>::local<U*> =42;
      template<class T>template<int V>requires(V>0)struct N::Outer<T>::Count<V,true> {
        int get()const{return V;}
      };
      inline int probe(){N::Outer<long>::Inner<int*> x{nullptr};return x.get()+N::Outer<int>::number<long*>+N::Outer<long>::local<int*>+N::Outer<int>::Count<42>{}.get()+N::Outer<int>::number<long>;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxDependentMemberPartialSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-dependent-member-partials.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-dependent-member-partials.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxDependentMemberPartialIdentity)==1);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxDependentMemberPartialSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxDependentMemberPartialSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto hidden=emit("static int hidden(){return 42;}template<class T>struct Outer{template<class U>inline static int value=1;};template<class T>template<class U>inline int Outer<T>::value<U*> =hidden();");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    auto changed=source;changed.replace(changed.find("return 42;"),10,"return 43;");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  }
}

void TestConcreteSpecialMemberSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="concrete-special-member-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.special_members","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.special_members","test");
    };
    const std::string source=R"(
      namespace N {
        struct Concrete {
          int value;
          inline static int live=0;
          Concrete():value(40){++live;}
          virtual ~Concrete(){--live;}
          template<class T>explicit Concrete(T n):value(n){++live;}
          template<class T>T operator()(T n)const{return value+n;}
          template<class T>explicit operator T()const{return value;}
          template<class T>Concrete &operator=(T n){value=n;return *this;}
        };
      }
      template<>inline N::Concrete::Concrete(int n):value(n+1){++live;}
      template<>inline int N::Concrete::operator()<int>(int n)const{return value+n+1;}
      template<>inline N::Concrete::operator int()const{return value+2;}
      template<>inline N::Concrete &N::Concrete::operator=<int>(int n){value=n+1;return *this;}
      inline int probe(){N::Concrete value{41};N::Concrete ordinary;ordinary=41;return value(1)+static_cast<int>(ordinary)+N::Concrete{40L}(2L)+static_cast<long>(value);}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxConcreteSpecialMemberSpecializationSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-concrete-special-members.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-concrete-special-members.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxConcreteSpecialMemberSpecializationIdentity)==1);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxConcreteSpecialMemberSpecializationSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxConcreteSpecialMemberSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("value(n+1)"),10,"value(n+2)");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
  }
}

void TestConcreteMemberVariableSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="concrete-member-variable-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.concrete_variables","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.concrete_variables","test");
    };
    const std::string source=R"(
      static int private_value(){return 42;}
      namespace N {
        struct Value{int n;explicit Value(int n):n(n){}~Value(){}};
        template<class T>concept Number=sizeof(T)>0;
        struct Concrete {
          template<class T>inline static int value=0;
          template<class T>inline static int inside=0;
          template<class T>inline static int inside<T*> =42;
          template<class T>inline static thread_local int local=0;
          template<class T>inline static Value object{40};
          template<int V,bool Positive=true>inline static int count=0;
        };
      }
      template<>int N::Concrete::value<int> =private_value();
      template<N::Number T>inline int N::Concrete::value<T*> =40;
      template<>inline thread_local int N::Concrete::local<int> =42;
      template<N::Number T>inline thread_local int N::Concrete::local<T*> =40;
      template<>N::Value N::Concrete::object<int>{42};
      template<int V>requires(V>0)inline int N::Concrete::count<V,true> =V;
      inline int probe(){return N::Concrete::inside<int*>+N::Concrete::value<int>+N::Concrete::value<int*>+N::Concrete::local<int>+N::Concrete::local<int*>+N::Concrete::object<int>.n+N::Concrete::count<42>;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxConcreteMemberVariableSpecializationSemantic)!=std::string::npos);
    assert(first.source_bytes.find("private_value")==std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-concrete-member-variables.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-concrete-member-variables.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxConcreteMemberVariableSpecializationIdentity)==1);
    auto from=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes,{"-xc++",standard},"concrete-member-variables-replay.cpp");
    auto to=clang::tooling::buildASTFromCodeWithArgs("",{"-xc++",standard},"concrete-member-variables-consumer.cpp");
    assert(from && to && !from->getDiagnostics().hasErrorOccurred());
    auto shared=std::make_shared<clang::ASTImporterSharedState>(*to->getASTContext().getTranslationUnitDecl());
    clang::ASTImporter importer(to->getASTContext(),to->getFileManager(),
                               from->getASTContext(),from->getFileManager(),false,shared);
    for(auto *root:from->getASTContext().getTranslationUnitDecl()->decls())
      if(auto *scope=llvm::dyn_cast<clang::NamespaceDecl>(root))
        for(auto *member:scope->decls()) {
          auto *owner=llvm::dyn_cast<clang::NamedDecl>(member);
          if(!owner || ReadConcreteMemberVariableSpecializations(owner).empty())continue;
          auto imported=importer.Import(owner);assert(imported);
          for(auto *specialization:ReadConcreteMemberVariableSpecializations(owner)) {
            auto variable=importer.Import(specialization);assert(variable);
          }
          InterfaceDeclarationIdentity expected,actual;
          assert(ReadInterfaceDeclarationIdentity(owner,from->getASTContext(),expected));
          assert(ReadInterfaceDeclarationIdentity(llvm::cast<clang::NamedDecl>(*imported),to->getASTContext(),actual));
          assert(SameInterfaceDeclarationIdentity(expected,actual));
        }
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxConcreteMemberVariableSpecializationSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxConcreteMemberVariableSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("value<T*> =40"),13,"value<T*> =41");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto private_changed=source;private_changed.replace(private_changed.find("return 42"),9,"return 43");
    auto private_altered=emit(private_changed);assert(private_altered.success && private_altered.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto hidden=emit("static int hidden(){return 42;}struct Concrete{template<class T>inline static int value=0;};template<>inline int Concrete::value<int> =hidden();");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
  }
}

void TestConcreteMemberFunctionSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="concrete-member-function-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.concrete_functions","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.concrete_functions","test");
    };
    const std::string source=R"(
      static int private_twice(int n){return n+n+2;}
      namespace N {
        struct Concrete {
          int base=40;
          template<class T=int>T add(T n=1)const{return base+n;}
          template<class T>static T twice(T n){return n+n;}
          template<auto V>static int constant(){return int(V);}
        };
      }
      template<>inline int N::Concrete::add<int>(int n)const{return base+n+1;}
      template<>int N::Concrete::twice<int>(int n){return private_twice(n);}
      template<>inline int N::Concrete::constant<0.0>(){return 1;}
      template<>inline int N::Concrete::constant<-0.0>(){return 2;}
      inline int probe(){return N::Concrete{}.add()+N::Concrete::twice(20)+N::Concrete{}.add(2L)+N::Concrete::constant<0.0>()+N::Concrete::constant<-0.0>();}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxConcreteMemberFunctionSpecializationSemantic)!=std::string::npos);
    assert(first.source_bytes.find("private_twice")==std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-concrete-member-functions.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-concrete-member-functions.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxConcreteMemberFunctionSpecializationIdentity)==1);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxConcreteMemberFunctionSpecializationSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxConcreteMemberFunctionSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("return base+n+1"),15,"return base+n+2");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto private_changed=source;private_changed.replace(private_changed.find("return n+n+2"),12,"return n+n+3");
    auto private_altered=emit(private_changed);assert(private_altered.success && private_altered.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto hidden=emit("static int hidden(){return 42;}struct Concrete{template<class T>T get(T n){return n;}};template<>inline int Concrete::get<int>(int n){return hidden();}");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
  }
}

void TestConcreteMemberClassSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="concrete-member-class-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.concrete_specs","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.concrete_specs","test");
    };
    const std::string source=R"(
      namespace N {
        template<class T>concept Number=sizeof(T)>0;
        struct Concrete {
          template<class T>struct Box{T value;};
          template<int V,bool Positive=true>struct Count{static constexpr int value=0;};
        };
      }
      template<>struct N::Concrete::Box<int>{
        inline static int live=0;
        int value;explicit Box(int n):value(n){++live;}
        Box(const Box& other):value(other.value){++live;}
        virtual ~Box(){--live;}
        virtual int get()const{return value;}
        template<class U=int>int add(U n=2)const{return get()+n;}
      };
      template<N::Number T>struct N::Concrete::Box<T*>{T* value;T get()const{return *value;}};
      template<int V>requires(V>0)struct N::Concrete::Count<V,true>{static constexpr int value=V;};
      inline int probe(){int n=42;return N::Concrete::Box<int>{40}.add()+N::Concrete::Box<int*>{&n}.get()+N::Concrete::Count<42>::value;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxConcreteMemberClassSpecializationSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-concrete-member-class-specializations.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-concrete-member-class-specializations.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxConcreteMemberClassSpecializationIdentity)==1);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxConcreteMemberClassSpecializationSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxConcreteMemberClassSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("return get()+n"),14,"return get()+n+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto hidden=emit("static int hidden(){return 42;}struct Concrete{template<class T>struct Box{};};template<>struct Concrete::Box<int>{int get(){return hidden();}};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("struct Concrete{template<class T>struct Box{};};template<>struct Concrete::Box<int>;").success);
  }
}

void TestDetachedNestedTemplates() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="detached-nested-templates.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.detached_nested","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.detached_nested","test");
    };
    const std::string source=R"(
      namespace N {
        template<class T>struct Wrap{T value;};
        template<class T>struct Outer{struct Plain;template<class U=int>struct Inner;};
        template<class T>concept Number=sizeof(T)>0;
        template<class T>struct Partial{};
        template<Number T>struct Partial<T*>{struct Plain;};
        struct Concrete{template<class T=int>struct Inner;};
      }
      template<class T>struct N::Outer<T>::Plain{
        inline static thread_local int local=40;
        template<template<class>class C>using Alias=C<T>;
        T value;explicit Plain(T n):value(n){}Plain(const Plain&)=default;
        virtual ~Plain(){}virtual T get()const{return value;}
        template<class U=int>T add(U n=2)const{return get()+n;}
      };
      template<class T>template<class U>struct N::Outer<T>::Inner{T left;U right;struct Deep;int sum()const{return left+right;}};
      template<class T>template<class U>struct N::Outer<T>::Inner<U>::Deep{T left;U right;int sum()const{return left+right;}};
      template<class T>struct N::Concrete::Inner{T value;T get()const{return value;}};
      template<N::Number T>struct N::Partial<T*>::Plain{T *value;T get()const{return *value;}};
      inline int probe(){int n=42;return N::Outer<int>::Plain::Alias<N::Wrap>{42}.value+N::Outer<int>::Plain{40}.add()+N::Outer<int>::Inner<>::Deep{40,2}.sum()+N::Concrete::Inner<>{42}.get()+N::Partial<int*>::Plain{&n}.get();}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxDetachedNestedTemplateSemantic)!=std::string::npos);
    assert(first.sidecar_json.find(kCxxTemplateTemplateSemantic)!=std::string::npos &&
           first.sidecar_json.find("objc3.cxx-thread-local.v1")!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-detached-nested-templates.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-detached-nested-templates.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxDetachedNestedTemplateIdentity)==3);
    auto from=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes,{"-xc++",standard},"detached-nested-replay.cpp");
    auto to=clang::tooling::buildASTFromCodeWithArgs("",{"-xc++",standard},"detached-nested-consumer.cpp");
    assert(from && to && !from->getDiagnostics().hasErrorOccurred());
    auto shared=std::make_shared<clang::ASTImporterSharedState>(*to->getASTContext().getTranslationUnitDecl());
    clang::ASTImporter importer(to->getASTContext(),to->getFileManager(),
                               from->getASTContext(),from->getFileManager(),false,shared);
    for(auto *root:from->getASTContext().getTranslationUnitDecl()->decls())
      if(auto *scope=llvm::dyn_cast<clang::NamespaceDecl>(root))
        for(auto *member:scope->decls()) {
          auto *owner=llvm::dyn_cast<clang::NamedDecl>(member);
          if(!owner || ReadDetachedNestedTemplateDefinitions(owner).empty())continue;
          auto imported=importer.Import(owner);assert(imported);
          for(auto *definition:ReadDetachedNestedTemplateDefinitions(owner)) {
            auto nested=importer.Import(definition);assert(nested);
          }
          InterfaceDeclarationIdentity expected,actual;
          assert(ReadInterfaceDeclarationIdentity(owner,from->getASTContext(),expected));
          assert(ReadInterfaceDeclarationIdentity(llvm::cast<clang::NamedDecl>(*imported),to->getASTContext(),actual));
          assert(SameInterfaceDeclarationIdentity(expected,actual));
        }
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxDetachedNestedTemplateSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxDetachedNestedTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("return left+right"),17,"return left+right+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto hidden=emit("static int hidden(){return 42;}template<class T>struct Outer{struct Inner;};template<class T>struct Outer<T>::Inner{int get(){return hidden();}};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("template<class T>struct Outer{struct Inner;};").success);
  }
}

void TestDetachedStaticDataTemplates() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="detached-static-data.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.detached_data","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.detached_data","test");
    };
    const std::string source=R"(
      namespace N {
        template<class T>struct Box {
          static int value;static int zero;static thread_local int local;
          template<class U=int>static int member;
        };
        template<int N>struct Count{static int value;int get()const;};
        template<class T>concept Number=sizeof(T)>0;
        template<class T>struct Partial{};
        template<Number T>struct Partial<T*>{static int value;};
        struct Concrete{template<class T=int>static int value;};
      }
      template<int N>int N::Count<N>::value=N;
      template<int N>int N::Count<N>::get()const{return N;}
      template<class T>int N::Box<T>::value=40;
      template<class T>int N::Box<T>::zero;
      template<class T>thread_local int N::Box<T>::local=41;
      template<class T>template<class U>int N::Box<T>::member=sizeof(T)+sizeof(U);
      template<N::Number T>int N::Partial<T*>::value=42;
      template<class T>int N::Concrete::value=42;
      inline int probe(){return N::Count<42>::value+N::Count<42>{}.get()+N::Box<int>::value+N::Box<int>::zero+N::Box<int>::local+N::Box<int>::member<>+N::Partial<int*>::value+N::Concrete::value<>;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxDetachedStaticDataTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-detached-static-data.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-detached-static-data.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxDetachedStaticDataTemplateIdentity)==4);
    auto from=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes,{"-xc++",standard},"detached-data-replay.cpp");
    auto to=clang::tooling::buildASTFromCodeWithArgs("",{"-xc++",standard},"detached-data-consumer.cpp");
    assert(from && to && !from->getDiagnostics().hasErrorOccurred());
    clang::ASTImporter importer(to->getASTContext(),to->getFileManager(),
                               from->getASTContext(),from->getFileManager(),false);
    for(auto *root:from->getASTContext().getTranslationUnitDecl()->decls()) {
      if(!llvm::isa<clang::NamespaceDecl,clang::VarTemplateDecl,clang::VarDecl,clang::CXXMethodDecl>(root))continue;
      auto imported=importer.Import(root);
      if(!imported)llvm::errs()<<llvm::toString(imported.takeError())<<"\n";
      assert(imported);
    }
    for(auto *root:from->getASTContext().getTranslationUnitDecl()->decls())
      if(auto *scope=llvm::dyn_cast<clang::NamespaceDecl>(root))
        for(auto *member:scope->decls()) {
          auto *owner=llvm::dyn_cast<clang::NamedDecl>(member);
          if(!owner || ReadDetachedStaticDataTemplateDefinitions(owner).empty())continue;
          auto imported=importer.Import(owner);assert(imported);
          InterfaceDeclarationIdentity expected,actual;
          assert(ReadInterfaceDeclarationIdentity(owner,from->getASTContext(),expected));
          assert(ReadInterfaceDeclarationIdentity(llvm::cast<clang::NamedDecl>(*imported),to->getASTContext(),actual));
          assert(SameInterfaceDeclarationIdentity(expected,actual));
        }
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxDetachedStaticDataTemplateSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxDetachedStaticDataTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("value=40"),8,"value=41");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto hidden=emit("static int hidden(){return 42;}template<class T>struct Box{static int value;};template<class T>int Box<T>::value=hidden();");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("template<class T>struct Box{static int value;};").success);
  }
}

void TestDetachedCallableTemplates() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="detached-callable-templates.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.detached_callables","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.detached_callables","test");
    };
    const std::string source=R"(
      namespace N {
        template<class T>struct Box {
          T value;
          explicit Box(T);
          Box(const Box &);
          virtual ~Box();
          virtual T get()const;
          template<class U=int>T add(U n=2)const;
          template<class U>auto closure(U n)const;
        };
        template<class T>concept Number=requires(T n){n+1;};
        template<class T>struct Partial{T value;};
        template<Number T>struct Partial<T*>{
          T *value;explicit Partial(T*);Partial(const Partial&);
          virtual ~Partial();virtual T get()const;
        };
        struct Concrete{template<Number U=int>U twice(U n=21)const;};
      }
      template<class T>N::Box<T>::Box(T n):value(n){}
      template<class T>N::Box<T>::Box(const Box &other):value(other.value){}
      template<class T>N::Box<T>::~Box(){}
      template<class T>T N::Box<T>::get()const{return value;}
      template<class T>template<class U>T N::Box<T>::add(U n)const{return get()+n;}
      template<class T>template<class U>auto N::Box<T>::closure(U n)const{return [saved=value,n]{return saved+n;};}
      template<N::Number T>N::Partial<T*>::Partial(T* n):value(n){}
      template<N::Number T>N::Partial<T*>::Partial(const Partial &other):value(other.value){}
      template<N::Number T>N::Partial<T*>::~Partial(){}
      template<N::Number T>T N::Partial<T*>::get()const{return *value;}
      template<N::Number U>U N::Concrete::twice(U n)const{return n+n;}
      inline int probe(){N::Box<int> value{40};return value.add()+value.closure(2)()+N::Concrete{}.twice();}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxDetachedCallableTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-detached-callables.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-detached-callables.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxDetachedCallableTemplateIdentity)==3);
    auto from=clang::tooling::buildASTFromCodeWithArgs(first.source_bytes,{"-xc++",standard},"detached-replay.cpp");
    auto to=clang::tooling::buildASTFromCodeWithArgs("",{"-xc++",standard},"detached-consumer.cpp");
    assert(from && to && !from->getDiagnostics().hasErrorOccurred());
    clang::ASTImporter importer(to->getASTContext(),to->getFileManager(),
                               from->getASTContext(),from->getFileManager(),false);
    for(auto *root:from->getASTContext().getTranslationUnitDecl()->decls()) {
      if(!llvm::isa<clang::NamespaceDecl,clang::FunctionTemplateDecl,clang::CXXMethodDecl>(root))continue;
      auto imported=importer.Import(root);assert(imported);
    }
    for(auto *root:from->getASTContext().getTranslationUnitDecl()->decls())
      if(auto *scope=llvm::dyn_cast<clang::NamespaceDecl>(root))
        for(auto *member:scope->decls()) {
          auto *owner=llvm::dyn_cast<clang::NamedDecl>(member);
          if(!owner || ReadDetachedCallableTemplateDefinitions(owner).empty())continue;
          auto imported=importer.Import(owner);assert(imported);
          InterfaceDeclarationIdentity expected,actual;
          assert(ReadInterfaceDeclarationIdentity(owner,from->getASTContext(),expected));
          assert(ReadInterfaceDeclarationIdentity(llvm::cast<clang::NamedDecl>(*imported),to->getASTContext(),actual));
          assert(SameInterfaceDeclarationIdentity(expected,actual));
        }
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxDetachedCallableTemplateSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxDetachedCallableTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("return get()+n"),14,"return get()+n+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto initializer=source;initializer.replace(initializer.find("value(n)"),8,"value(n+1)");
    auto reinitialized=emit(initializer);
    assert(reinitialized.success && reinitialized.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto hidden=emit("static int hidden(){return 42;}template<class T>struct Box{int get();};template<class T>int Box<T>::get(){return hidden();}");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("template<class T>struct Box{int get();};").success);
  }
}

void TestExternalVariableSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="external-variable-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.external_variables","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.external_variables","test");
    };
    const std::string source=R"(
      struct Value{int n;explicit Value(int n):n(n){}~Value(){}};
      template<class T>Value object{40};
      template<>Value object<int>{42};
      template<class T>int external=0;
      static int secret_init(){return 40;}
      template<>int external<int> =secret_init();
      template<class T>thread_local int local=0;
      template<>thread_local int local<int> =40;
      template<class T>inline int zero=1;
      template<>inline int zero<int>;
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxVariableSpecializationStorageSemantic)!=std::string::npos);
    assert(first.source_bytes.find("secret_init")==std::string::npos);
    assert(first.source_bytes.find("template <> extern")!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-external-variables.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-external-variables.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxVariableSpecializationStorageIdentity)==4);
    for(const auto *capability:{kCxxVariableSpecializationStorageSemantic,kCxxVariableSpecializationSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxVariableSpecializationStorageSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto changed=source;changed.replace(changed.find("return 40"),9,"return 41");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
  }
}

void TestPartialVariableSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="partial-variable-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.partial_variables","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.partial_variables","test");
    };
    const std::string source=R"(
      template<class T>concept Sized=sizeof(T)>0;
      namespace Values{template<class T>inline int shared=0;}
      template<Sized T>inline int Values::shared<T*> =40;
      template<class T>inline thread_local int local=0;
      template<Sized T>inline thread_local int local<T*> =41;
      template<int N,bool Positive=true>inline constexpr int count=0;
      template<int N>requires(N>0)inline constexpr int count<N,true> =N;
      template<class...Ts>inline constexpr unsigned width=0;
      template<class T,class...Ts>inline constexpr unsigned width<T,Ts...> =1+sizeof...(Ts);
      template<class T>inline auto deduced=0;
      template<class T>inline auto deduced<T*> =T{40};
      template<class T>struct Cell{T value;};
      template<class T>inline constexpr int size=0;
      template<template<class>class C,class T>inline constexpr int size<C<T>> =sizeof(C<T>);
      inline int probe(){return ++Values::shared<int*> + ++local<int*> + count<4> + width<int,long> + deduced<long*>;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxVariablePartialSpecializationSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-partial-variables.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-partial-variables.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxVariablePartialSpecializationIdentity)==6);
    assert(first.sidecar_json.find(kCxxTemplateTemplateSemantic)!=std::string::npos);
    auto changed=source;changed.replace(changed.find("=40"),3,"=42");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    for(const auto *capability:{kCxxVariablePartialSpecializationSemantic,kCxxVariableTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxVariablePartialSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto hidden=emit("static int hidden(){return 42;}template<class T>inline int value=0;template<class T>inline int value<T*> =hidden();");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("template<class T>inline int value=0;template<class T>extern int value<T*>;").success);
  }
}

void TestPartialClassSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="partial-class-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.partial_classes","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.partial_classes","test");
    };
    const std::string source=R"(
      template<class T>concept Sized=sizeof(T)>0;
      namespace Values{template<class T>struct Box{T value;};}
      template<Sized T>struct Values::Box<T*>{
        inline static int live=0;
        T* value;
        explicit Box(T* n):value(n){++live;}
        Box(const Box &other):value(other.value){++live;}
        virtual ~Box(){--live;}
        virtual int get()const{return *value;}
        template<class U>int add(U n)const{return get()+n;}
      };
      template<int N,bool Positive=true>struct Count{static constexpr int value=0;};
      template<int N>requires(N>0)struct Count<N,true>{static constexpr int value=N;};
      template<class...Ts>struct Pack{static constexpr int value=0;};
      template<class T,class...Ts>struct Pack<T,Ts...>{static constexpr int value=1+sizeof...(Ts);};
      template<class T>struct Cell{T value;};
      template<class T>struct Rebind{};
      template<template<class>class C,class T>struct Rebind<C<T>>{C<T> value;};
      inline int probe(){int n=40;return Values::Box<int*>{&n}.add(2)+Count<4>::value+Pack<int,long>::value;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxClassPartialSpecializationSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-partial-classes.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-partial-classes.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxClassPartialSpecializationIdentity)==4);
    assert(first.sidecar_json.find(kCxxTemplateTemplateSemantic)!=std::string::npos);
    auto changed=source;changed.replace(changed.find("return *value"),13,"return *value+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    for(const auto *capability:{kCxxClassPartialSpecializationSemantic,kCxxClassSpecializationSemantic,kCxxClassTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxClassPartialSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto hidden=emit("static int hidden(){return 42;}template<class T>struct Box{};template<class T>struct Box<T*>{int get(){return hidden();}};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("template<class T>struct Box{};template<class T>struct Box<T*>;").success);
  }
}

void TestClassSpecializations() {
  for(const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="class-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.class_specializations","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.class_specializations","test");
    };
    const std::string source=R"(
      namespace Values {template<class T>struct Box{T value;};template<>struct Box<int>;}
      struct Later{int value;};
      template<>struct Values::Box<int>{
        Later value;
        explicit Box(int n):value{n}{}
        Box(const Box &other):value(other.value){}
        virtual ~Box(){}
        virtual int get()const{return value.value;}
        template<class U>int add(U n)const{return [](auto value){return value;}(get()+n);}
        template<template<class>class C>int size()const{return sizeof(C<int>);}
      };
      template<auto Value>struct Number{static constexpr int value=0;};
      template<>struct Number<0.0>{static constexpr int value=1;};
      template<>struct Number<-0.0>{static constexpr int value=2;};
      inline int probe(){return Values::Box<int>{40}.add(2)+Number<0.0>::value+Number<-0.0>::value;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxClassSpecializationSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-class-specializations.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-class-specializations.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxClassSpecializationIdentity)==2);
    assert(first.sidecar_json.find(kCxxTemplateTemplateSemantic)!=std::string::npos);
    assert(first.sidecar_json.find("objc3.cxx-generic-local-lambdas.v2")!=std::string::npos);
    assert(std::none_of(decoded->declarations.begin(),decoded->declarations.end(),[](const auto &d){return d.kind=="ClassTemplateSpecialization";}));
    auto changed=source;changed.replace(changed.find("return value.value"),18,"return value.value+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    for(const auto *capability:{kCxxClassSpecializationSemantic,kCxxClassTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxClassSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto hidden=emit("static int hidden(){return 42;}template<class T>struct Box{};template<>struct Box<int>{int get(){return hidden();}};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("template<class T>struct Box{};template<>struct Box<int>;").success);
    assert(!emit("template<class T>struct Box{};template<class T>struct Box<T*>{Box();};").success);
  }
}

void TestVariableSpecializations() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="variable-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.variable_specializations","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.variable_specializations","test");
    };
    const std::string source=R"(
      template<class T> inline int shared=1;
      template<> inline int shared<int> =42;
      template<class T> inline thread_local int local=2;
      template<> inline thread_local int local<int> =43;
      template<auto Value> inline constexpr int number=0;
      template<> inline constexpr int number<0.0> =1;
      template<> inline constexpr int number<-0.0> =2;
      inline int probe(){return ++shared<int> + ++local<int> + number<0.0>;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxVariableSpecializationSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-variable-specializations.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-variable-specializations.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    std::set<std::string> ids,symbols;
    for(const auto &d:decoded->declarations) if(HasCxxVariableSpecializationIdentity(d)) {
      assert(d.kind=="VarTemplateSpecialization" && ids.insert(d.stable_id).second && symbols.insert(d.native_symbol_name).second);
      assert(HasValidModuleOwnedName(decoded->module,d));
    }
    assert(ids.size()==4);
    auto changed=source;changed.replace(changed.find("=42"),3,"=44");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    for(const auto *capability:{kCxxVariableSpecializationSemantic,kCxxVariableTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxVariableSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto hidden=emit("static int hidden(){return 42;}template<class T>inline int value=0;template<>inline int value<int> =hidden();");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("template<class T>inline int value=0;template<>int value<int> __attribute__((used)) =42;").success);
    assert(!emit("template<class T>inline int value=0;template<class T>extern int value<T*>;").success);
  }
}

void TestFunctionSpecializations() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="function-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.function_specializations","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.function_specializations","test");
    };
    const std::string source=R"(
      template<class T> T add(T n){return n+1;}
      template<> inline int add<int>(int n){return n+2;}
      template<> long add<long>(long n){return n+3;}
      template<auto Value> constexpr int number(){return 0;}
      template<> constexpr int number<0.0>(){return 1;}
      template<> constexpr int number<-0.0>(){return 2;}
      template<class T> int next(){static int n=40;return ++n;}
      template<> inline int next<int>(){static int n=100;return ++n;}
      inline int probe(){return add(40)+number<0.0>()+number<-0.0>()+next<int>();}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxFunctionSpecializationSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-specializations.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-specializations.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    std::set<std::string> ids,symbols;
    for(const auto &d:decoded->declarations) if(HasCxxFunctionSpecializationIdentity(d)) {
      assert(d.kind=="Function" && ids.insert(d.stable_id).second && symbols.insert(d.native_symbol_name).second);
      assert(HasValidModuleOwnedName(decoded->module,d));
    }
    assert(ids.size()==5);
    auto changed=source;changed.replace(changed.find("return n+2"),10,"return n+4");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    for(const auto *capability:{kCxxFunctionSpecializationSemantic,kCxxFunctionTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxFunctionSpecializationSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto hidden=emit("static int hidden(){return 42;}template<class T>int f(T){return 0;}template<>inline int f<int>(int){return hidden();}");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    assert(!emit("template<class T>int f(T){return 0;}template<>int f<int>(int)=delete;").success);
  }
}

void TestLargeExportPolicy() {
  std::string source;
  std::vector<std::string> exports;
  for (unsigned i=0;i<kMaxInterfaceExportNames;++i) {
    exports.push_back("export_"+std::to_string(i));
    source+="int "+exports.back()+"();\n";
  }
  auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++","-std=c++20"},"large-library.cpp");
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());
  auto accepted=EmitSemanticInterface(ast->getASTContext(),"fixture.large_library","test",{},{},{},exports);
  assert(accepted.success);
  exports.push_back("one_too_many");
  auto rejected=EmitSemanticInterface(ast->getASTContext(),"fixture.large_library","test",{},{},{},exports);
  assert(!rejected.success && std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),
      [](const auto &d){return d.code=="O3IFC155";}));
}

void TestTemplateTemplateParameters() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="template-template-parameters.cpp",
                  const std::string &module="fixture.template_templates",const std::string &package="test") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),module,package,{}));
      return EmitSemanticInterface(ast->getASTContext(),module,package);
    };
    const std::string source=R"(
      template<class T> struct Box{T value;};
      template<class T> using Alias=Box<T>;
      template<template<class>class C=Alias,class T=int> constexpr T read(C<T> value){return value.value;}
      template<template<class>class C=Box> struct Wrapper{C<int> value;};
      template<template<template<class>class>class W=Wrapper,template<class>class C=Alias>
      using Higher=W<C>;
      template<template<class>class C=Alias,class T=int> using Rebind=C<T>;
      template<template<class>class C=Alias> inline constexpr unsigned width=sizeof(C<int>);
      template<template<class>class... C> constexpr unsigned arity(){return sizeof...(C);}
      template<template<class>class C=Alias> requires(sizeof(C<int>)>0)
      constexpr int constrained(C<int> value){return value.value;}
      template<template<class>class C> int next(){static int n=40;return ++n;}
      template<class T> struct Family {
        template<class U> using Member=Box<U>;
        template<template<class>class C=Alias> using Type=C<T>;
        template<template<class>class C=Alias> static constexpr int read(C<T> n){return n.value;}
        template<template<class>class C=Alias> inline static C<T> saved{40};
      };
      struct Concrete {
        template<template<class>class C=Alias> using Type=C<int>;
        template<template<class>class C=Alias> static constexpr int read(C<int> n){return n.value;}
        template<template<class>class C=Alias> inline static C<int> saved{40};
        template<template<class>class C=Alias> struct Nested{C<int> value;};
      };
      template<template<__SIZE_TYPE__,class...>class C=__type_pack_element>
      using First=C<0,int,long>;
      inline constexpr int probe(){
        Rebind<Alias,long> n{42};Higher<> higher{{42}};
        return read<Alias>(Box<int>{int(n.value)})+higher.value.value;
      }
      inline int state(){return next<Alias>()+next<Family<int>::Member>()+Family<int>::saved<Alias>.value;}
      template<template<class>class C=Alias> int forwarded(int n);
      template<template<class>class C> int forwarded(int n){return C<int>{n}.value;}
      inline int lambda_probe(){auto call=[]<template<class>class C>(C<int> n){return n.value;};return call.template operator()<Alias>({42});}
      static_assert(arity<Box,Alias>()==2 && width<> ==sizeof(int));
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxTemplateTemplateSemantic)!=std::string::npos);
    assert(first.sidecar_json.find(kCxxNominalAliasTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-template-templates.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-template-templates.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto row=[](const auto &s,const char *name)->const InterfaceDeclarationIdentity & {
      return *std::find_if(s.declarations.begin(),s.declarations.end(),
          [&](const auto &d){return d.qualified_name==name;});
    };
    assert(HasCxxNominalAliasTemplateIdentity(row(*decoded,"Alias")) && !row(*decoded,"Alias").native_symbol_name.empty());
    auto moved=emit(source,"other-package.cpp","fixture.template_templates","other");assert(moved.success);
    auto other=DecodeSemanticInterfaceSidecar(moved.sidecar_json,moved.source_bytes,semantics);assert(other);
    assert(row(*decoded,"Alias").native_symbol_name!=row(*other,"Alias").native_symbol_name);
    auto changed=source;changed.replace(changed.find("return value.value"),18,"return value.value+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    for(const auto *capability:{kCxxTemplateTemplateSemantic,kCxxNominalAliasTemplateSemantic,kCxxAliasTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxTemplateTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto renamed=source;
    const auto at=renamed.find("template<template<class>class C=Alias> requires(sizeof(C<int>)>0)");
    assert(at!=std::string::npos);
    const std::string before="template<template<class>class C=Alias> requires(sizeof(C<int>)>0)\n      constexpr int constrained(C<int> value)";
    const std::string after="template<template<class>class Container=Alias> requires(sizeof(Container<int>)>0)\n      constexpr int constrained(Container<int> value)";
    renamed.replace(at,before.size(),after);
    auto alpha=emit(renamed,"renamed-template-templates.cpp");assert(alpha.success);
    auto alpha_sidecar=DecodeSemanticInterfaceSidecar(alpha.sidecar_json,alpha.source_bytes,semantics);assert(alpha_sidecar);
    assert(row(*decoded,"constrained").native_symbol_name==row(*alpha_sidecar,"constrained").native_symbol_name);
    auto hidden=emit("namespace{template<class T>struct Hidden{T value;};}template<template<class>class C=Hidden>struct Public{C<int> value;};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
  }
}

void TestConcreteNestedTemplates() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="concrete-nested-templates.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.concrete_nested_templates","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.concrete_nested_templates","test");
    };
    const std::string source=R"(
      struct Outer {
        template<class U=int,int Bias=2> class Box {
          template<class V=U> using Storage=V;
          Storage<U> value;
        public:
          using element=U;
          template<class V=U> using Pointer=V*;
          inline static int live=0;
          explicit Box(U n=U{40}):value(n){++live;}
          Box(const Box &other):value(other.value){++live;}
          virtual ~Box(){--live;}
          virtual U number()const{return value+Bias;}
          template<class V> auto add(V n)const{return number()+n;}
        };
        template<double D> struct Floating {
          template<class U> struct Inner {static constexpr int answer(){return 42;}};
        };
      };
      Outer::Box<int> *make(){return new Outer::Box<int>;}
      inline int probe(){Outer::Box<long,3> value(39);return value.add(0);}
      inline constexpr double argument=__builtin_bit_cast(double,0x0000000000000000ULL);
      inline constexpr int exact_probe(){return Outer::Floating<argument>::Inner<int>::answer();}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxConcreteNestedTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-concrete-nested.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-concrete-nested.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::none_of(decoded->declarations.begin(),decoded->declarations.end(),
        [](const auto &d){return d.qualified_name.starts_with("Outer::");}));
    auto changed=source;changed.replace(changed.find("return value+Bias"),17,"return value+Bias+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;changed.replace(changed.find("0x0000000000000000ULL"),21,"0x8000000000000000ULL");
    auto zero=emit(changed);assert(zero.success);
    auto negative=DecodeSemanticInterfaceSidecar(zero.sidecar_json,zero.source_bytes,semantics);assert(negative);
    auto row=[](const auto &s)->const InterfaceDeclarationIdentity & {
      return *std::find_if(s.declarations.begin(),s.declarations.end(),
          [](const auto &d){return d.qualified_name=="exact_probe";});
    };
    assert(row(*decoded).canonical_type_identity!=row(*negative).canonical_type_identity);
    for(const auto *capability:{kCxxConcreteNestedTemplateSemantic,kCxxConcreteMemberTemplateSemantic,
        kCxxClassTemplateSemantic,kCxxNestedTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxConcreteNestedTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    assert(emit("struct Outer{template<class T>struct Box{T value;};};").success);
    auto specialized=emit("struct Outer{template<class T>struct Box{T value;};};template<class T>struct Outer::Box<T*>{T *value;};");
    assert(specialized.success && specialized.sidecar_json.find(kCxxConcreteMemberClassSpecializationSemantic)!=std::string::npos);
    auto detached=emit("struct Outer{template<class T>struct Box;};template<class T>struct Outer::Box{T value;};");
    assert(detached.success && detached.sidecar_json.find(kCxxDetachedNestedTemplateSemantic)!=std::string::npos);
    auto hidden=emit("static int hidden(){return 42;}struct Outer{template<class T>struct Box{int f(){return hidden();}};};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    for(const auto *source:{
        "struct Outer{template<class T>struct Box;};",
        "struct Outer{template<class T>union Box{T value;};};"})
      assert(!emit(source).success);
  }
}

void TestConcreteMemberTemplates() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="concrete-member-templates.cpp",
                  const std::vector<std::string> &exports={}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.concrete_member_templates","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.concrete_member_templates","test",{},{},{},exports);
    };
    const std::string source=R"(
      class Concrete {
        template<class U> using Storage=U;
        Storage<int> value;
      public:
        Concrete():value(40){}
        template<class U> explicit(sizeof(U)>1) Concrete(U n):value(int(n)){}
        virtual ~Concrete()=default;
        virtual int number()const{return value;}
        template<class U=int> auto add(U n=U{2})const noexcept(noexcept(value+n)){return value+n;}
        template<class U> requires requires(U n){n+1;}
        U operator()(U n)const{return U(value)+n;}
        template<class U> static auto reader(U n){return [n]{return n;};}
        template<class U=int> static int next(){static int n=40;return ++n;}
        template<class U=int> using Pointer=U*;
        template<int N> using Array=int[N];
        template<class U=int> inline static U shared=U{40};
        template<class U=int> inline static thread_local U local=U{40};
        template<int N> inline static constexpr int answer=N+2;
      };
      inline int probe(){Concrete value(40L);return value.add()+Concrete::reader(0)()+Concrete::shared<>+Concrete::local<>;}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxConcreteMemberTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-concrete-members.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-concrete-members.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("return value+n"),14,"return value+n+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    for(const auto &d:decoded->declarations) {
      assert(d.kind!="FunctionTemplate" && d.kind!="AliasTemplate" && d.kind!="VarTemplate");
      if (d.qualified_name=="Concrete") assert(HasCxxConcreteMemberTemplateIdentity(d));
    }
    assert(std::any_of(decoded->declarations.begin(),decoded->declarations.end(),
        [](const auto &d){return d.qualified_name=="Concrete::number" && d.kind=="CXXMethod";}));
    for(const auto *capability:{kCxxConcreteMemberTemplateSemantic,kCxxMemberTemplateSemantic,
        kCxxMemberAliasTemplateSemantic,kCxxVariableTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxConcreteMemberTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    for(const auto *source:{
        "static int hidden(){return 42;}struct Concrete{template<class U>int f(U){return hidden();}};",
        "static int hidden(){return 42;}struct Concrete{template<class U>int f(U n=hidden()){return n;}};",
        "static int hidden(){return 42;}struct Concrete{template<class U>inline static int value=hidden();};"}) {
      auto hidden=emit(source);
      assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
          [](const auto &d){return d.code=="O3IFC112";}));
    }
    auto variable_specialized=emit("struct Concrete{template<class U>inline static U value=U{40};};template<>inline int Concrete::value<int> =42;");
    assert(variable_specialized.success && variable_specialized.sidecar_json.find(kCxxConcreteMemberVariableSpecializationSemantic)!=std::string::npos);
    auto specialized=emit("struct Concrete{template<class U>int f(U n){return n;}};template<>int Concrete::f<int>(int n){return n+1;}");
    assert(specialized.success && specialized.sidecar_json.find(kCxxConcreteMemberFunctionSpecializationSemantic)!=std::string::npos);
    for(const auto *source:{
        "struct Concrete{template<class U>int f(U);};",
        "struct Concrete{template<class U>static U value;};",
        "struct Concrete{template<template<class>class U>int f();};",
        "struct Concrete{template<class U>int g(U);};"})
      assert(!emit(source).success);
  }
}

void TestVariadicTemplates() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="variadic-templates.cpp",
                  const std::vector<std::string> &exports={}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.variadic_templates","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.variadic_templates","test",{},{},{},exports);
    };
    const std::string source=R"(
      template<class T=int> T sum(unsigned count, ...) {
        __builtin_va_list args, copy;
        __builtin_va_start(args,count);__builtin_va_copy(copy,args);
        int result=0;
        for(unsigned i=0;i<count;++i) result+=__builtin_va_arg(copy,int);
        __builtin_va_end(copy);__builtin_va_end(args);return T(result);
      }
      template<class T> struct Variadic {
        T sum(unsigned count, ...) const {
          __builtin_va_list args;__builtin_va_start(args,count);
          int result=0;for(unsigned i=0;i<count;++i)result+=__builtin_va_arg(args,int);
          __builtin_va_end(args);return T(result);
        }
        template<class U=T> U add(unsigned count, ...) const {
          __builtin_va_list args;__builtin_va_start(args,count);
          double result=0;for(unsigned i=0;i<count;++i)result+=__builtin_va_arg(args,double);
          __builtin_va_end(args);return U(result);
        }
      };
      inline int probe(){return sum<>(2,40,2)+Variadic<long>{}.add<int>(2,40.0,2.0);}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxVariadicTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-variadic.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-variadic.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("return T(result)"),16,"return T(result+1)");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),HasCxxVariadicTemplateIdentity)==2);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxVariadicTemplateSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxVariadicTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    auto hidden=emit("static int hidden(){return 42;}template<class T>T sum(unsigned n,...){return T(hidden());}");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    for(const auto *fake_source:{
        "namespace User{struct __va_list_tag{int value;};}template<class T>int fake(unsigned,...){return sizeof(User::__va_list_tag);}",
        "namespace User{using __builtin_va_list=int;}template<class T>int fake(unsigned,...){User::__builtin_va_list value=42;return value;}"}) {
      auto fake=emit(fake_source,"user-va-list.cpp",{"fake"});
      assert(!fake.success && std::any_of(fake.diagnostics.begin(),fake.diagnostics.end(),
          [](const auto &d){return d.code=="O3IFC156";}));
    }
  }
}

void TestNestedTemplateTypes() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="nested-templates.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.nested_templates","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.nested_templates","test");
    };
    const std::string source=R"(

    template<class T> class NestedFamily {
    public:
      using element=T;
      struct Detail {using element=T;T value;explicit Detail(T n):value(n){} T number()const{return value;}};
      template<class U=T,int Bias=2> class Box {
        U value;
      public:
        using element=U;
        template<class V=U> using Pointer=V*;
        inline static int live=0;
        explicit Box(U n=U{40}):value(n){++live;}
        Box(const Box &other):value(other.value){++live;}
        virtual ~Box(){--live;}
        virtual U number()const{return value+Bias;}
        template<class V> auto add(V n)const{return number()+n;}
      };
      template<class U=int> inline static U shared=U{40};
      template<class U=int> inline static thread_local U local=U{40};
      template<int N> inline static constexpr int answer=N+2;
    };
    NestedFamily<int>::Box<int> *make_nested(){return new NestedFamily<int>::Box<int>;}
    int nested_state(){return ++NestedFamily<int>::shared<> + ++NestedFamily<int>::local<>;}
    template<double D> struct FloatingFamily {
      template<class T> struct Inner {static constexpr int answer(){return 42;}};
    };
    inline constexpr double nested_argument=__builtin_bit_cast(double,0x0000000000000000ULL);
    inline constexpr int nested_argument_probe(){return FloatingFamily<nested_argument>::Inner<int>::answer();}

    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxNestedTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-nested-templates.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-nested-templates.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::none_of(decoded->declarations.begin(),decoded->declarations.end(),
        [](const auto &d){return d.qualified_name.starts_with("NestedFamily::") || d.qualified_name.starts_with("FloatingFamily::");}));
    auto changed=source;changed.replace(changed.find("value+Bias"),10,"value+Bias+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    changed=source;changed.replace(changed.find("0x0000000000000000ULL"),21,"0x8000000000000000ULL");
    auto signed_zero=emit(changed);assert(signed_zero.success);
    auto negative=DecodeSemanticInterfaceSidecar(signed_zero.sidecar_json,signed_zero.source_bytes,semantics);assert(negative);
    auto row=[](const auto &sidecar)->const InterfaceDeclarationIdentity & {
      return *std::find_if(sidecar.declarations.begin(),sidecar.declarations.end(),
          [](const auto &d){return d.qualified_name=="nested_argument_probe";});
    };
    assert(row(*decoded).canonical_type_identity!=row(*negative).canonical_type_identity);
    for(const auto *capability:{kCxxNestedTemplateSemantic,kCxxVariableTemplateSemantic,kCxxClassTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxNestedTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    for(const auto *source:{
      "static int hidden(){return 42;}template<class T>struct Outer{struct Inner{int f(){return hidden();}};};",
      "static int hidden(){return 42;}template<class T>struct Outer{template<class U>inline static int value=hidden();};",
      "static constexpr bool hidden(){return true;}template<class T>struct Outer{int f()requires(hidden()){return 42;}};"}) {
      auto hidden=emit(source);
      assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
          [](const auto &d){return d.code=="O3IFC112";}));
    }
    for(const auto *gated:{
      "template<class T>struct Outer{struct Inner;};",
      "template<class T>struct Outer{union Inner{T value;};};",
      "template<class T>struct Outer{template<class U>static U value;};"})
      assert(!emit(gated).success);
    auto partial=emit("template<class T>struct Outer{template<class U>struct Inner{};template<class U>struct Inner<U*>{};};");
    assert(partial.success && partial.sidecar_json.find(kCxxDependentMemberPartialSemantic)!=std::string::npos);
  }
}

void TestClassMemberAliasTemplates() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="member-aliases.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.member_aliases","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.member_aliases","test",{},{},{},exports);
    };
    const std::string source=R"(

    template<class T> struct AliasValue {using element=T;T value;};
    template<class T> struct MemberAliases {
      template<class U=T> using Pointer=U*;
      template<class... U> using Callback=T(*)(U...);
      template<int N> using Array=T[N];
      template<class U=T> using Rebind=AliasValue<U>;
      template<class U> requires requires{typename U::element;}
      using Element=typename U::element;
      template<class U> static constexpr Element<Rebind<U>> number(U n){return n;}
    };
    inline constexpr int alias_member_probe(){
      MemberAliases<int>::Array<2> values={40,2};
      MemberAliases<int>::Rebind<long> value{values[0]};
      return MemberAliases<int>::number(value.value)+values[1];
    }

    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxMemberAliasTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-member-aliases.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-member-aliases.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("using Pointer=U*"),16,"using Pointer=U const*");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::none_of(decoded->declarations.begin(),decoded->declarations.end(),
        [](const auto &d){return d.qualified_name.starts_with("MemberAliases::");}));
    for(const auto *capability:{kCxxMemberAliasTemplateSemantic,kCxxClassTemplateSemantic,kCxxAliasTemplateSemantic}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxMemberAliasTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    for(const auto *source:{
        "struct Hidden{};template<class T>struct Types{template<class U>using Leak=Hidden;};",
        "struct Hidden{};template<class T>struct Types{template<class U=Hidden>using Leak=U;};"}) {
      auto hidden=emit(source,"hidden-member-alias.cpp",{"Types"});
      assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
          [](const auto &d){return d.code=="O3IFC156";}));
    }
    assert(emit("template<class T>struct Types{template<template<class>class U>using Rebind=U<T>;};").success);
  }
}

void TestClassMemberTemplates() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="members.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.members","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.members","test");
    };
    const std::string source=R"(

    template<class T> class MemberBox {
      T value;
    public:
      constexpr MemberBox(T n):value(n){}
      template<class U> requires requires(U n){static_cast<T>(n);}
      constexpr explicit(sizeof(U)>0) MemberBox(U n):value(static_cast<T>(n)){}
      template<class U=T,int Delta=2>
      constexpr auto add(U n=U{2})const noexcept(noexcept(value+n)){return value+n+Delta;}
      template<class U=T> constexpr decltype(auto) reference(){return (value);}
      template<class U> constexpr auto reader(U offset)const {
        return [n=value,offset](int extra){return n+offset+extra;};
      }
      template<class... U> constexpr auto sum(U... n)const{return (value+...+n);}
      template<class U> constexpr auto operator()(U n)const{return value+n;}
      template<class U> constexpr explicit operator U()const{return static_cast<U>(value);}
      template<class U=int> static int next(){static int calls=0;return ++calls;}
    };
    constexpr int member_probe(){MemberBox<int> box(40L);return box.add(0);}
    int provider(){return MemberBox<int>::next<>();}

    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxMemberTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-members.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-members.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("value+n+Delta"),13,"value+n+Delta+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),
        [](const auto &d){return d.qualified_name=="MemberBox";})==1);
    assert(std::none_of(decoded->declarations.begin(),decoded->declarations.end(),
        [](const auto &d){return d.qualified_name.starts_with("MemberBox::");}));
    for(const auto *capability:{kCxxMemberTemplateSemantic,kCxxClassTemplateSemantic,"objc3.cxx-data-interfaces.v1"}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    auto old=semantics;std::erase(old,llvm::StringRef(kCxxMemberTemplateSemantic));
    auto rejected=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,old);
    assert(!rejected);llvm::consumeError(rejected.takeError());
    for(const auto *source:{
        "static int hidden(){return 42;}template<class T>struct Box{template<class U>int f(U){return hidden();}};",
        "static int hidden(){return 42;}template<class T>struct Box{template<class U>int f(U n=hidden()){return n;}};"}) {
      auto hidden=emit(source);
      assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
          [](const auto &d){return d.code=="O3IFC112";}));
    }
    for(const auto *gated:{
        "template<class T>struct Box{template<class U>U f(U);};",
        "template<class T>struct Box{template<template<class>class U>int f();};",
        "template<class T>struct Box{template<class U>int f(U,...)=delete;};"})
      assert(!emit(gated).success);
    const std::string specialized="template<class T>struct Box{template<class U>int f(U n){return n;}};template<>template<>int Box<int>::f<int>(int n){return n+1;}";
    auto member=emit(specialized);
    assert(member.success && member.sidecar_json.find(kCxxClosedParentMemberSemantic)!=std::string::npos);
    auto implementation=specialized;
    implementation.replace(implementation.find("n+1"),3,"n+2");
    auto private_change=emit(implementation);
    assert(private_change.success && private_change.identity.semantic_fingerprint==member.identity.semantic_fingerprint);
  }
}


void TestIncompleteTemplateDeclarations() {
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    auto parse=[&](const std::string &source,const char *file) {
      return clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
    };
    auto emit=[&](const std::string &source,const char *file="incomplete-templates.cpp") {
      auto ast=parse(source,file);assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      ast->getDiagnostics().getClient()->BeginSourceFile(ast->getLangOpts(),&ast->getPreprocessor());
      auto EndDiagnostics=llvm::scope_exit([&]{ast->getDiagnostics().getClient()->EndSourceFile();});
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.incomplete","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.incomplete","test");
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"INCOMPLETE %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      return result;
    };
    const std::string source=R"(
      namespace Incomplete {
        template<class T=int,int Bias=2> requires(sizeof(T)>0) struct Box;
        template<class T=int> extern T value;
        template<class T=int> extern thread_local T local;
        struct Enclosing {template<class T>struct Opaque;};
        template<class T>struct Wrapper {template<class U>struct Opaque;};
        using Default=Box<>;
        Box<> *pointer();
        inline int read(){return value<int>+local<int>;}
      }
    )";
    auto first=emit(source);assert(first.success);
    auto replay=emit(first.source_bytes,"replayed-incomplete.cpp");
    assert(replay.success && replay.source_bytes==first.source_bytes && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-incomplete.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &v:*metadata->getAsObject()->getArray("required_semantics"))semantics.push_back(*v.getAsString());
    assert(llvm::is_contained(semantics,llvm::StringRef(kCxxIncompleteTemplateSemantic)));
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    unsigned incomplete=0;
    for(const auto &row:decoded->declarations)if(HasCxxIncompleteTemplateIdentity(row)) {
      ++incomplete;assert(row.kind=="ClassTemplate" || row.kind=="VarTemplate" || row.kind=="CXXRecord");
    }
    assert(incomplete==5);
    auto older=semantics;std::erase(older,llvm::StringRef(kCxxIncompleteTemplateSemantic));
    auto unavailable=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,older);assert(!unavailable);llvm::consumeError(unavailable.takeError());
    auto missing=*decoded;std::erase(missing.required_semantics,kCxxIncompleteTemplateSemantic);
    auto invalid=EncodeSemanticInterfaceSidecar(missing,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto wrong=*decoded;auto row=std::find_if(wrong.declarations.begin(),wrong.declarations.end(),HasCxxIncompleteTemplateIdentity);assert(row!=wrong.declarations.end());row->kind="Namespace";
    auto bad_owner=EncodeSemanticInterfaceSidecar(wrong,semantics);assert(!bad_owner);llvm::consumeError(bad_owner.takeError());
    auto changed=source;auto at=changed.find("class T=int,int Bias");assert(at!=std::string::npos);changed.replace(at,11,"class T=long");
    auto different=emit(changed);assert(different.success && different.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto altered=DecodeSemanticInterfaceSidecar(different.sidecar_json,different.source_bytes,semantics);assert(altered);
    auto find=[](const auto &sidecar,const char *name)->const InterfaceDeclarationIdentity & {
      auto row=std::find_if(sidecar.declarations.begin(),sidecar.declarations.end(),[&](const auto &d){return d.qualified_name==name;});
      assert(row!=sidecar.declarations.end());return *row;
    };
    assert(find(*decoded,"Incomplete::Box").native_symbol_name==find(*altered,"Incomplete::Box").native_symbol_name);
    assert(find(*decoded,"Incomplete::Box").stable_id==find(*altered,"Incomplete::Box").stable_id);
    auto plain=emit("void incomplete_plain(void);");assert(plain.success);
    auto invented=DecodeSemanticInterfaceSidecar(plain.sidecar_json,plain.source_bytes,semantics);assert(invented);
    invented->required_semantics.push_back(kCxxIncompleteTemplateSemantic);
    auto unnecessary=EncodeSemanticInterfaceSidecar(*invented,semantics);assert(!unnecessary);llvm::consumeError(unnecessary.takeError());
    assert(!emit("namespace {struct Hidden{};}template<class T=Hidden>struct HiddenDefault;").success);
    assert(!emit("namespace {struct Hidden{};}template<class T=Hidden>extern T hidden_value;").success);
    assert(!emit("static constexpr int hidden(){return 42;}template<int N=hidden()>struct HiddenValue;").success);
    assert(emit("template<class T>struct Partial{};template<class T>struct Partial<T*>;").success);
    const std::string completed=first.source_bytes+R"(
      namespace Incomplete {
        template<class T,int Bias> requires(sizeof(T)>0) struct Box {T payload;constexpr T get()const{return payload+Bias;}};
        template<class T> T value=40;
        template<class T> thread_local T local=2;
      }
      inline int incomplete_consumer(){Incomplete::Default value{40};return value.get()+Incomplete::read();}
    )";
    auto complete=emit(completed,"completed-templates.cpp");assert(complete.success);
    auto complete_metadata=llvm::json::parse(complete.sidecar_json);assert(complete_metadata);
    std::vector<llvm::StringRef> complete_semantics;
    for(const auto &v:*complete_metadata->getAsObject()->getArray("required_semantics"))complete_semantics.push_back(*v.getAsString());
    auto complete_sidecar=DecodeSemanticInterfaceSidecar(complete.sidecar_json,complete.source_bytes,complete_semantics);assert(complete_sidecar);
    unsigned remaining=0;
    for(const auto &row:complete_sidecar->declarations)if(HasCxxIncompleteTemplateIdentity(row)) {
      ++remaining;assert(row.qualified_name=="Incomplete::Enclosing" || row.qualified_name=="Incomplete::Wrapper");
    }
    assert(remaining==2);
    auto complete_replay=emit(complete.source_bytes,"completed-replay.cpp");
    assert(complete_replay.success && complete_replay.identity.semantic_fingerprint==complete.identity.semantic_fingerprint);
  }
}


void TestIncompleteTemplateSpecializations() {
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="incomplete-specializations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      ast->getDiagnostics().getClient()->BeginSourceFile(ast->getLangOpts(),&ast->getPreprocessor());
      auto EndDiagnostics=llvm::scope_exit([&]{ast->getDiagnostics().getClient()->EndSourceFile();});
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),"fixture.incomplete_specializations","test",{}));
      auto result=EmitSemanticInterface(ast->getASTContext(),"fixture.incomplete_specializations","test");
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"INCOMPLETE SPECIALIZATION %s: %s %s\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      return result;
    };
    const std::string source=R"(
      namespace Specializations {
        template<class T>struct Family {static constexpr int tag=1;};
        template<class T>struct Family<T*>;
        template<>struct Family<char>;
        template<class T>extern int value;
        template<class T>extern int value<T*>;
        template<class T>extern thread_local int local;
        template<class T>extern thread_local int local<T*>;
        template<class T>struct Outer {
          template<class U>struct Inner {static constexpr int tag=1;};
          template<class U>struct Inner<U*>;
          template<class U>static int data;
          template<class U>static int data<U*>;
        };
        struct Ordinary {
          template<class T>struct Family {static constexpr int tag=1;};
          template<class T>struct Family<T*>;
          template<class T>static int data;
          template<class T>static int data<T*>;
        };
        template<>struct Ordinary::Family<char>;
        Family<int*> *pointer();
        Family<char> *character();
      }
    )";
    auto first=emit(source);assert(first.success);
    auto replay=emit(first.source_bytes,"specialization-replay.cpp");
    assert(replay.success && replay.source_bytes==first.source_bytes && replay.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"specialization-relocated.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto capabilities=SupportedSemanticInterfaceCapabilities();
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,capabilities);assert(decoded);
    bool primary=false,partial=false,explicit_class=false,variables=false;
    for(const auto &row:decoded->declarations)if(HasCxxIncompleteTemplateIdentity(row)) {
      primary|=row.kind=="ClassTemplate";
      // Namespace specializations belong to the primary's semantic row.
      partial|=row.canonical_type_identity.find(";cxx-class-partial-specializations:v1;")!=std::string::npos;
      explicit_class|=row.canonical_type_identity.find(";cxx-class-specializations:v1;")!=std::string::npos;
      variables|=row.kind=="VarTemplate";
    }
    assert(primary && partial && explicit_class && variables);
    assert(emit(first.source_bytes+R"(
      static_assert(!__is_complete_type(Specializations::Family<int*>));
      static_assert(!__is_complete_type(Specializations::Family<char>));
      static_assert(!__is_complete_type(Specializations::Outer<int>::Inner<long*>));
      static_assert(!__is_complete_type(Specializations::Ordinary::Family<int*>));
      static_assert(!__is_complete_type(Specializations::Ordinary::Family<char>));
    )","opaque-specialization-consumer.cpp").success);
    auto completed=first.source_bytes+R"(
      namespace Specializations {
        template<class T>struct Family<T*> {static constexpr int tag=2;};
        template<>struct Family<char> {static constexpr int tag=3;};
        template<class T>int value=10;
        template<class T>int value<T*> = 40;
        template<class T>thread_local int local=1;
        template<class T>thread_local int local<T*> = 2;
        template<class T>template<class U>struct Outer<T>::Inner<U*> {static constexpr int tag=4;};
        template<class T>template<class U>int Outer<T>::data=10;
        template<class T>template<class U>int Outer<T>::data<U*> = 42;
        template<class T>struct Ordinary::Family<T*> {static constexpr int tag=5;};
        template<>struct Ordinary::Family<char> {static constexpr int tag=6;};
        template<class T>int Ordinary::data=10;
        template<class T>int Ordinary::data<T*> = 42;
      }
      static_assert(Specializations::Family<int>::tag==1);
      static_assert(Specializations::Family<int*>::tag==2);
      static_assert(Specializations::Family<char>::tag==3);
      static_assert(Specializations::Outer<int>::Inner<long*>::tag==4);
      static_assert(Specializations::Ordinary::Family<int*>::tag==5);
      static_assert(Specializations::Ordinary::Family<char>::tag==6);
      inline int specialization_consumer(){return Specializations::value<int*>+Specializations::local<int*>+Specializations::Outer<int>::data<long*>;}
    )";
    auto complete=emit(completed,"completed-specializations.cpp");assert(complete.success);
    auto complete_replay=emit(complete.source_bytes,"completed-specialization-replay.cpp");
    assert(complete_replay.success && complete_replay.identity.semantic_fingerprint==complete.identity.semantic_fingerprint);
    auto completion=DecodeSemanticInterfaceSidecar(complete.sidecar_json,complete.source_bytes,capabilities);assert(completion);
    for(const auto &row:completion->declarations)assert(!HasCxxIncompleteTemplateIdentity(row));
    assert(!emit("namespace{struct Hidden{};}template<class T>struct Public{};template<>struct Public<Hidden>;").success);
  }
}

void TestPublicClassTemplates() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="classes.cpp",
                  const std::vector<std::string> &exports=std::vector<std::string>{}) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.classes","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.classes","test",{},{},{},exports);
    };
    const std::string source=R"(
      namespace Types {
        template<class T> concept Number=requires(T n){n+2;};
        template<Number T=int,int Bias=2> class Box {
          T value;
        public:
          using element=T;
          enum class Kind{answer=42};
          inline static int visits=0;
          constexpr Box(T n=T{40}):value(n){}
          constexpr Box(const Box&)=default;
          constexpr T number()const noexcept{return value+Bias;}
          static int next(){return ++visits;}
        };
        template<class... T> struct Arity {static constexpr unsigned count=sizeof...(T);};
        template<class T> requires(sizeof(T)>0) struct Width {T value{};};
      }
      template<double D> struct Floating {static constexpr int answer(){return 42;}};
      inline constexpr double class_argument=__builtin_bit_cast(double,0x0000000000000000ULL);
      inline constexpr int class_argument_probe(){return Floating<class_argument>::answer();}
      inline constexpr int named_class_argument=2;
      inline constexpr int class_construct_probe(){return Types::Box<int,named_class_argument>{40}.number();}
      inline constexpr int probe(){return Types::Box<int>(40).number();}
    )";
    auto first=emit(source);
    for(const auto &d:first.diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxClassTemplateSemantic)!=std::string::npos);
    auto repeated=emit(first.source_bytes,"reprinted-classes.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-classes.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("value+Bias"),10,"value+Bias+1");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for(const auto &value:*metadata->getAsObject()->getArray("required_semantics")) semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    changed=source;changed.replace(changed.find("0x0000000000000000ULL"),std::string("0x0000000000000000ULL").size(),"0x8000000000000000ULL");
    auto negative_zero=emit(changed);assert(negative_zero.success);
    auto negative=DecodeSemanticInterfaceSidecar(negative_zero.sidecar_json,negative_zero.source_bytes,semantics);assert(negative);
    auto row=[](const auto &sidecar)->const InterfaceDeclarationIdentity & {
      return *std::find_if(sidecar.declarations.begin(),sidecar.declarations.end(),
          [](const auto &d){return d.qualified_name=="class_argument_probe";});
    };
    assert(row(*decoded).canonical_type_identity!=row(*negative).canonical_type_identity);

    for(const auto *capability:{kCxxClassTemplateSemantic,"objc3.cxx-data-interfaces.v1"}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),
        [](const auto &d){return d.qualified_name=="Types::Box";})==1);
    assert(!emit("template<class T>struct Hidden{T n;};inline int api(){return Hidden<int>{42}.n;}",
                 "excluded-class.cpp",{"api"}).success);
    auto hidden=emit("static int hidden(){return 42;}template<class T>struct Box{int api(){return hidden();}};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    assert(emit("template<class T>struct Box;").success);
    // Declaration-only members are supported contracts. Preserve their rows
    // and replay identity instead of retaining the old definition-only gate.
    for(const auto *admitted:{"template<class T>struct Box{T api();};",
                             "template<class T>struct Box{};template<class T>struct Box<T*>{Box();};",
                             "template<class T>struct Box{};template<>struct Box<int>{Box();};",
                             "template<class T>struct Box{template<class U> U api(U n);};"}) {
      auto declared=emit(admitted);assert(declared.success);
      auto metadata=llvm::json::parse(declared.sidecar_json);assert(metadata);
      assert(std::any_of(metadata->getAsObject()->getArray("declarations")->begin(),
                        metadata->getAsObject()->getArray("declarations")->end(),
                        [](const auto &row){return row.getAsObject()->getString("qualified_name")=="Box";}));
      auto replay=emit(declared.source_bytes,"replayed-member-declarations.cpp");
      assert(replay.success && replay.identity.semantic_fingerprint==declared.identity.semantic_fingerprint);
    }
  }
}

void TestPublicVariableTemplates() {
  for (const auto *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source, const char *file = "variables.cpp",
                    const std::vector<std::string> &exports = std::vector<std::string>{}) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source, {"-xc++", standard}, file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(), ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.variables", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.variables", "test", {}, {}, {}, exports);
    };
    const std::string source = R"(
      namespace Values {
        template<class T=int, int N=40> inline constexpr T answer=T{N+2};
        template<class T=int> inline T shared=40;
        template<class T=int> inline thread_local T local=40;
        template<class... T> inline constexpr unsigned count=sizeof...(T);
        template<class T> requires(sizeof(T)>0) inline constexpr unsigned width=sizeof(T);
        template<class T> T zero;
      }
      template<double D> inline constexpr int floating=42;
      inline constexpr double argument=__builtin_bit_cast(double,0x0000000000000000ULL);
      inline constexpr int argument_probe(){return floating<argument>;}
      inline int probe(){return Values::shared<>+Values::local<long>;}
    )";
    auto first = emit(source);
    for (const auto &d : first.diagnostics)
      std::fprintf(stderr, "%s: %s (%s)\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(first.success && first.sidecar_json.find(kCxxVariableTemplateSemantic)!=std::string::npos);
    auto repeated = emit(first.source_bytes, "reprinted-variables.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated = emit("\n\n"+source, "relocated-variables.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto changed=source;changed.replace(changed.find("N=40"),4,"N=41");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for (const auto &value:*metadata->getAsObject()->getArray("required_semantics")) semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto row=[](const auto &sidecar,const char *name)->const InterfaceDeclarationIdentity & {
      return *std::find_if(sidecar.declarations.begin(),sidecar.declarations.end(),
          [&](const auto &d){return d.qualified_name==name;});
    };
    auto altered_sidecar=DecodeSemanticInterfaceSidecar(altered.sidecar_json,altered.source_bytes,semantics);assert(altered_sidecar);
    assert(row(*decoded,"Values::answer").native_symbol_name==row(*altered_sidecar,"Values::answer").native_symbol_name);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),
        [](const auto &d){return d.qualified_name=="Values::shared";})==1);
    changed=source;changed.replace(changed.find("0x0000000000000000ULL"),std::string("0x0000000000000000ULL").size(),"0x8000000000000000ULL");
    auto signed_zero=emit(changed);assert(signed_zero.success);
    auto zero_sidecar=DecodeSemanticInterfaceSidecar(signed_zero.sidecar_json,signed_zero.source_bytes,semantics);assert(zero_sidecar);
    assert(row(*decoded,"argument_probe").canonical_type_identity!=row(*zero_sidecar,"argument_probe").canonical_type_identity);

    for (const auto *capability:{kCxxVariableTemplateSemantic,"objc3.cxx-data-interfaces.v1"}) {
      auto damaged=*decoded;std::erase(damaged.required_semantics,capability);
      auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    }
    assert(!emit("template<class T> inline T hidden=40;inline int api(){return hidden<int>;}",
                 "private-variable.cpp",{"api"}).success);
    auto hidden=emit("namespace {struct Hidden{};} template<class T> inline Hidden value{};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    assert(emit("template<class T> extern T value;").success);
    assert(emit("template<class T> inline int value=40;template<class T> extern int value<T*>;").success);
    for (const auto *gated:{"template<class T> inline int value=40;template<> int value<long> __attribute__((used))=42;"}) {
      std::string fixed=gated;
      for (std::size_t pos=0;(pos=fixed.find(">=",pos))!=std::string::npos;pos+=3) fixed.replace(pos,2,"> =");
      assert(!emit(fixed).success);
    }
  }
}

void TestFunctionTemplateDeclarations() {
  for (const auto *standard:{"-std=c++20","-std=c++23"}) {
    auto emit=[&](const std::string &source,const char *file="template-declarations.cpp") {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,{"-xc++",standard},file);
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
                                  "fixture.declarations","test",{}));
      return EmitSemanticInterface(ast->getASTContext(),"fixture.declarations","test");
    };
    const std::string source=R"(
      template<class T=int,int Bias=2> constexpr T forwarded(T n=T{40});
      template<class U,int Delta> constexpr U forwarded(U value){return value+Delta;}
      template<class T,int N> constexpr T late_default(T n);
      template<class U,int M=2> constexpr U late_default(U value){return value+M;}
      template<class T=int,int N=2> constexpr T tail_decl(T n=T{40}){return n+N;}
      template<class U,int M> constexpr U tail_decl(U value);
      template<int N=40> requires(N>0) constexpr int constrained_default(int n=N);
      template<int M> requires(M>0) constexpr int constrained_default(int value){return value+2;}
      template<class T=int> constexpr T generic_default(T n=T{40}){return n+2;}
      template<double D> constexpr int floating_value(){return 42;}
      inline constexpr double argument_value=__builtin_bit_cast(double,0x0000000000000000ULL);
      inline constexpr int argument_probe(){return floating_value<argument_value>();}
      inline constexpr int defaults_probe(){return forwarded()+late_default(40)+tail_decl()+
          constrained_default()+generic_default();}
    )";
    auto first=emit(source);
    for (const auto &d:first.diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
    assert(first.success);
    auto repeated=emit(first.source_bytes,"reprinted-declarations.cpp");
    assert(repeated.success && repeated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto relocated=emit("\n\n"+source,"relocated-declarations.cpp");
    assert(relocated.success && relocated.identity.semantic_fingerprint==first.identity.semantic_fingerprint);
    auto metadata=llvm::json::parse(first.sidecar_json);assert(metadata);
    std::vector<llvm::StringRef> semantics;
    for (const auto &value:*metadata->getAsObject()->getArray("required_semantics")) semantics.push_back(*value.getAsString());
    auto decoded=DecodeSemanticInterfaceSidecar(first.sidecar_json,first.source_bytes,semantics);assert(decoded);
    auto changed=source;changed.replace(changed.find("Bias=2"),std::string("Bias=2").size(),"Bias=3");
    auto altered=emit(changed);assert(altered.success && altered.identity.semantic_fingerprint!=first.identity.semantic_fingerprint);
    auto altered_sidecar=DecodeSemanticInterfaceSidecar(altered.sidecar_json,altered.source_bytes,semantics);assert(altered_sidecar);
    auto row=[](const auto &sidecar,const char *name)->const InterfaceDeclarationIdentity & {
      return *std::find_if(sidecar.declarations.begin(),sidecar.declarations.end(),[&](const auto &d){return d.qualified_name==name;});
    };
    assert(row(*decoded,"forwarded").native_symbol_name==row(*altered_sidecar,"forwarded").native_symbol_name);
    assert(std::count_if(decoded->declarations.begin(),decoded->declarations.end(),[](const auto &d){return d.qualified_name=="forwarded";})==1);
    changed=source;changed.replace(changed.find("0x0000000000000000ULL"),std::string("0x0000000000000000ULL").size(),"0x8000000000000000ULL");
    auto signed_zero=emit(changed);assert(signed_zero.success);
    auto zero_sidecar=DecodeSemanticInterfaceSidecar(signed_zero.sidecar_json,signed_zero.source_bytes,semantics);assert(zero_sidecar);
    assert(row(*decoded,"argument_probe").canonical_type_identity!=row(*zero_sidecar,"argument_probe").canonical_type_identity);
    auto damaged=*decoded;std::erase(damaged.required_semantics,kCxxFunctionTemplateSemantic);
    damaged.required_semantics.push_back("objc3.cxx-function-templates.v1");
    auto invalid=EncodeSemanticInterfaceSidecar(damaged,semantics);assert(!invalid);llvm::consumeError(invalid.takeError());
    auto hidden=emit("static int hidden(){return 40;}template<class T=int>T api(T n=hidden()){return n+2;}");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
  }
}

void TestCarrierExpressionRoundtrip() {
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-xobjective-c++", standard, "-fobjc-version=3", "-fblocks",
           "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"},
          "carrier-expressions.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),
          ast->getASTContext().getTranslationUnitDecl(), "fixture.carrier", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.carrier", "test");
    };
    const std::string source =
        "inline int fallback(Optional(int) value) { return value ?? 42; } "
        "inline int precedence(int a,int b,int c){return (a+b)*c;}";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s\n", d.code.c_str(), d.message.c_str());
    assert(emitted.success);
    auto regenerated = emit(emitted.source_bytes);
    assert(regenerated.success && regenerated.source_bytes == emitted.source_bytes &&
           regenerated.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto parens = emit(
        "inline int fallback(Optional(int) value) { return (((value ?? 42))); } "
        "inline int precedence(int a,int b,int c){return (((a+b)))*c;}");
    assert(parens.success &&
           parens.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto different = emit(
        "inline int fallback(Optional(int) value) { return value ?? 43; } "
        "inline int precedence(int a,int b,int c){return a+b*c;}");
    assert(different.success &&
           different.identity.semantic_fingerprint != emitted.identity.semantic_fingerprint);
  }
}

void TestNativeExportRoots() {
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto ast = clang::tooling::buildASTFromCodeWithArgs(
        "namespace Api { struct Box { int n; inline static int count=0; "
        "friend int probe(Box value){return value.n;} }; int value(); "
        "enum class Kind {one}; } namespace Short=Api; using Api::value; "
        "using namespace Api; using enum Api::Kind;",
        {"-xc++",standard,"--target=x86_64-unknown-linux-gnu"},"export-roots.cpp");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted=EmitSemanticInterface(ast->getASTContext(),"fixture.roots","test");
    assert(emitted.success);
    const llvm::StringRef semantics[]={"objc3.cxx-data-interfaces.v1",
        "objc3.cxx-member-interfaces.v1",kCxxDeclarationExpressionSemantic,
        kCxxClassLifecycleSemantic,kCxxNamespaceSemantic,kCxxDefinitionSemantic,
        kCxxUsingSemantic,kCxxFriendSemantic};
    auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,semantics);
    assert(decoded);
    bool field=false, data=false, friend_function=false, alias=false, directive=false;
    for (const auto &row:decoded->declarations) {
      if (row.qualified_name=="Api::Box::n") {assert(row.export_root=="Api::Box");field=true;}
      if (row.qualified_name=="Api::Box::count") {assert(row.export_root=="Api::Box");data=true;}
      if (row.qualified_name=="Api::probe") {assert(row.export_root=="Api::Box");friend_function=true;}
      if (row.qualified_name=="Short") {assert(row.export_root=="Short");alias=true;}
      if (row.kind=="UsingDirective") {assert(row.export_root=="@using:Api");directive=true;}
      assert(!row.export_root.empty());
    }
    assert(field && data && friend_function && alias && directive);
  }
}

void TestCxxFriends() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-definitions.v1",
      "objc3.cxx-friends.v1", "objc3.module-owned-names.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"}, "friends.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(), ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.friends", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.friends", "test");
    };
    const std::string source =
        "struct Reader; class Value { int n; public: constexpr Value(int n):n(n){} "
        "friend struct Reader; friend constexpr int reveal(const Value &v) {return v.n;} "
        "friend bool operator==(const Value &a,const Value &b) {return a.n==b.n;} "
        "friend int external(Value); friend inline int delayed(const Value &); "
        "friend int operator+(Value,int)=delete; }; "
        "struct Reader { static int read(const Value &v) {return v.n;} }; "
        "int external(Value v) {return v.n+2;} inline int delayed(const Value &v) {return v.n+2;}";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto changed=source;
    changed.replace(changed.find("return v.n;}"),std::string("return v.n;}").size(),"return v.n+1;}");
    auto different=emit(changed);
    assert(different.success && different.identity.semantic_fingerprint!=emitted.identity.semantic_fingerprint);
    assert(!emit("struct V { template<class T> friend int generic(T); };").success);
    assert(!emit("struct __attribute__((annotate(\"objc3.spi:Testing\"))) A {friend int shared();}; "
                 "struct B {friend int shared();};").success);
    std::erase(decoded->required_semantics,"objc3.cxx-friends.v1");
    decoded->module.semantic_fingerprint=ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased=EncodeSemanticInterfaceSidecar(*decoded,semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxThreadLocalData() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-definitions.v1", "objc3.cxx-thread-local.v1", "objc3.module-owned-names.v1",
      "objc3.native-ownership-contracts.v1", "objc3.canonical-function-effects.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"}, "thread-data.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(), ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.threads", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.threads", "test");
    };
    const std::string source =
        "int initial(); thread_local int external=40; inline thread_local int shared=initial(); "
        "struct State { static thread_local int value; inline static thread_local int shared=initial(); }; "
        "thread_local int State::value=40; "
        "inline int local() { static thread_local int n=initial(); return ++n; }";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    assert(emitted.source_bytes.find("extern thread_local int external") != std::string::npos);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    if (!decoded) {
      std::fprintf(stderr, "TLS decode: %s\n", llvm::toString(decoded.takeError()).c_str());
      std::abort();
    }
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto changed = source;
    changed.replace(changed.find("thread_local int external"), 13, "");
    auto ordinary = emit(changed);
    assert(ordinary.success && ordinary.identity.semantic_fingerprint != emitted.identity.semantic_fingerprint);
    auto local_ordinary = source;
    local_ordinary.replace(local_ordinary.find("static thread_local int n"),
                           std::string("static thread_local int n").size(), "static int n");
    auto local_changed = emit(local_ordinary);
    assert(local_changed.success && local_changed.identity.semantic_fingerprint != emitted.identity.semantic_fingerprint);
    std::erase(decoded->required_semantics, "objc3.cxx-thread-local.v1");
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxLambdaExpressions() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-declaration-expressions.v1", "objc3.cxx-definitions.v1",
      "objc3.cxx-lambda-expressions.v1", "objc3.module-owned-names.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"}, "closures.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(), ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.closures", "test", {}));
      class ClosureNames final : public clang::RecursiveASTVisitor<ClosureNames> {
      public:
        bool VisitLambdaExpr(clang::LambdaExpr *expression) {
          assert(ReadNativeSymbolName(expression->getCallOperator()).empty());
          return true;
        }
      } names;
      names.TraverseDecl(ast->getASTContext().getTranslationUnitDecl());
      return EmitSemanticInterface(ast->getASTContext(), "fixture.closures", "test");
    };
    const std::string source =
        "int api(int n=[base=40] { return base+2; }()); "
        "struct Value { int n=[] { return 40; }(); int get(int delta=[] {return 2;}()) const {return n+delta;} }; "
        "inline int compute(int n) { auto first=[base=n](int delta) {return base+delta;}; "
        "auto second=[copy=first] {return copy(2);}; int state=n; "
        "auto reference=[&state](int delta) {state+=delta; return state;}; "
        "auto invoke=[&] {return reference(2);}; return invoke()==second() ? second() : 0; }";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto changed = source;
    changed.replace(changed.find("copy(2)"), 7, "copy(3)");
    auto different = emit(changed);
    assert(different.success && different.identity.semantic_fingerprint != emitted.identity.semantic_fingerprint);
    auto generic = emit("inline int generic(int n) { return [](auto value) {return value;}(n); }");
    assert(generic.success && generic.sidecar_json.find("objc3.cxx-generic-local-lambdas.v2")!=std::string::npos);
    std::erase(decoded->required_semantics, "objc3.cxx-lambda-expressions.v1");
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxNamespaceAttributes() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-namespaces.v1",
      "objc3.cxx-namespace-attributes.v1", "objc3.module-owned-names.v1",
      "objc3.api-availability.v1", "objc3.api-partitions.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"}, "namespace_attributes.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(), ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.attributes", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.attributes", "test");
    };
    const std::string source =
        "namespace __attribute__((visibility(\"hidden\"))) Hidden { struct Value { int n; }; int get(Value); } "
        "namespace Hidden { int later(); } "
        "namespace __attribute__((type_visibility(\"hidden\"))) Typed { struct Value { int n; }; } "
        "namespace __attribute__((availability(macos,introduced=10.1,deprecated=10.2,message=\"old\"))) Available { int api(); } "
        "namespace [[deprecated(\"old ;cxx-class-dispatch:v1;\")]] Old { int get(); } "
        "namespace __attribute__((annotate(\"objc3.spi:Testing\"))) Private { int secret(); struct Secret { int n; }; } "
        "namespace Version { inline namespace __attribute__((abi_tag(\"v2\"))) v2 { struct Value { int n; }; int get(Value); } }";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    assert(std::find(decoded->required_semantics.begin(), decoded->required_semantics.end(),
                     "objc3.cxx-class-dispatch.v1") == decoded->required_semantics.end());
    assert(std::any_of(decoded->declarations.begin(), decoded->declarations.end(), [](const auto &d) {
      return d.qualified_name == "Hidden::get" && d.canonical_type_identity.find(
          ";visibility=" + std::to_string(clang::HiddenVisibility) + ";") != std::string::npos;
    }));
    for (const auto &d : decoded->declarations)
      if (d.qualified_name == "Private" || d.qualified_name.starts_with("Private::"))
        assert(d.spi_group == "Testing");
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto changed = source;
    changed.replace(changed.find("hidden"), 6, "default");
    auto visible = emit(changed);
    assert(visible.success && visible.identity.semantic_fingerprint != emitted.identity.semantic_fingerprint);
    auto late = emit("namespace Late { int first(); } namespace __attribute__((visibility(\"hidden\"))) Late { int later(); }");
    assert(!late.success);
    auto unknown = emit("namespace __attribute__((annotate(\"other\"))) Unknown { int api(); }");
    assert(!unknown.success);
    auto conflict = emit("namespace __attribute__((annotate(\"objc3.spi:Testing\"))) Private { "
        "__attribute__((annotate(\"objc3.spi:Other\"))) int api(); }");
    assert(!conflict.success);
    std::erase(decoded->required_semantics, "objc3.cxx-namespace-attributes.v1");
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxImmediateDefaulting() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-class-structure.v1",
      "objc3.cxx-declaration-expressions.v1", "objc3.cxx-definitions.v1",
      "objc3.cxx-defaulted-comparisons.v1", "objc3.cxx-out-of-line-defaulting.v1",
      "objc3.cxx-immediate-defaulting.v1", "objc3.module-owned-names.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"}, "immediate.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(), ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.immediate", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.immediate", "test");
    };
    const std::string source = "struct Immediate { int n=42; consteval Immediate()=default; "
        "consteval bool operator==(const Immediate&) const=default; "
        "constexpr int get() const { return n; } }; "
        "struct Out { int n=42; consteval Out(); }; consteval Out::Out()=default; "
        "inline constexpr Immediate answer{}; inline constexpr Out other{};";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto constexpr_source = source;
    for (std::size_t pos=0; (pos=constexpr_source.find("consteval",pos))!=std::string::npos; pos+=9)
      constexpr_source.replace(pos,9,"constexpr");
    auto ordinary = emit(constexpr_source);
    assert(ordinary.success && ordinary.identity.semantic_fingerprint != emitted.identity.semantic_fingerprint);
    std::erase(decoded->required_semantics, "objc3.cxx-immediate-defaulting.v1");
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxOutOfLineDefaulting() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-class-structure.v1",
      "objc3.cxx-class-dispatch.v1", "objc3.cxx-namespaces.v1",
      "objc3.cxx-declaration-expressions.v1", "objc3.cxx-out-of-line-defaulting.v1",
      "objc3.module-owned-names.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"}, "defaulting.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(), ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.defaulting", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.defaulting", "test");
    };
    const std::string source = "namespace Model { struct Value { int n=40; Value(); "
        "Value(const Value&); Value(Value&&); Value& operator=(const Value&); "
        "Value& operator=(Value&&); ~Value(); }; "
        "Value::Value()=default; Value::Value(const Value&)=default; Value::Value(Value&&)=default; "
        "Value& Value::operator=(const Value&)=default; Value& Value::operator=(Value&&)=default; "
        "Value::~Value()=default; class Abstract { public: virtual ~Abstract()=0; }; "
        "Abstract::~Abstract()=default; }";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    std::erase(decoded->required_semantics, "objc3.cxx-out-of-line-defaulting.v1");
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxDefaultedComparisons() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-class-structure.v1",
      "objc3.cxx-defaulted-comparisons.v1", "objc3.module-owned-names.v1", "objc3.api-partitions.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source,
          {"-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"}, "comparisons.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(), ast->getASTContext().getTranslationUnitDecl(),
                                   "fixture.comparisons", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.comparisons", "test", {}, {}, {}, {"Member", "Friend"});
    };
    const std::string source = "struct Member { int n; int values[2]; "
        "bool operator==(const Member&) const=default; bool operator!=(const Member&) const=default; }; "
        "struct Friend { Member value; friend bool operator==(const Friend&,const Friend&)=default; };";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto used = emit(source + "bool private_use() { return Member{42,{1,2}}==Member{42,{1,2}} && "
        "Friend{{42,{1,2}}}!=Friend{{43,{1,2}}}; }");
    assert(used.success && used.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto spi_source = source;
    spi_source.replace(spi_source.find("struct Friend"), std::string("struct Friend").size(),
        "struct __attribute__((annotate(\"objc3.spi:secret\"))) Friend");
    auto spi = emit(spi_source);
    assert(spi.success);
    auto spi_metadata = DecodeSemanticInterfaceSidecar(spi.sidecar_json, spi.source_bytes, semantics);
    assert(spi_metadata);
    assert(std::any_of(spi_metadata->declarations.begin(), spi_metadata->declarations.end(),
        [](const auto &d) { return d.kind == "Function" && d.spi_group == "secret" &&
            d.canonical_type_identity.find("friend-owner=") != std::string::npos; }));
    std::erase(decoded->required_semantics, "objc3.cxx-defaulted-comparisons.v1");
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxDefinitions() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-class-structure.v1",
      "objc3.cxx-namespaces.v1", "objc3.cxx-definitions.v1",
      "objc3.cxx-declaration-expressions.v1", "objc3.module-owned-names.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    const std::vector<std::string> args = {"-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"};
    auto emit = [&](const std::string &source,
                    const std::vector<std::string> &exports = std::vector<std::string>{}) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source, args, "definitions.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      assert(ApplyModuleOwnedNames(ast->getASTContext(),
          ast->getASTContext().getTranslationUnitDecl(), "fixture.definitions", "test", {}));
      return EmitSemanticInterface(ast->getASTContext(), "fixture.definitions", "test", {}, {}, {}, exports);
    };
    const std::string source = "namespace Inline { inline int total=40; inline constexpr int extra=2; "
        "extern const int fixed=42; "
        "constexpr int compute(int n) { int result=0; for(int i=0;i<n;++i) result+=2; return result; } "
        "consteval int immediate(int n) { return n+2; } "
        "struct Value { int n; inline static int updates=0; static constexpr int offset=2; "
        "constexpr Value(int value):n(value) {} "
        "constexpr int get() const { return n+offset; } int update() { ++updates; return ++total; } }; "
        "inline int step(int delta=0) { static int calls=0; ++calls; total+=delta; return calls; } }";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto first = emit("inline int api(int n) { return n+2; }");
    auto second = emit("inline int api(int n) { return n+3; }");
    assert(first.success && second.success && first.identity.semantic_fingerprint != second.identity.semantic_fingerprint);
    first = emit("inline constexpr int answer=42;");
    second = emit("inline constexpr int answer=43;");
    assert(first.success && second.success && first.identity.semantic_fingerprint != second.identity.semantic_fingerprint);
    auto excluded = emit("int hidden(); inline int api() { return hidden(); }", {"api"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(), excluded.diagnostics.end(),
        [](const auto &d) { return d.code == "O3IFC156"; }));
    std::erase(decoded->required_semantics, "objc3.cxx-definitions.v1");
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxDeclarationExpressions() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-class-structure.v1",
      "objc3.cxx-nested-types.v1", "objc3.cxx-namespaces.v1",
      "objc3.cxx-declaration-expressions.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    const std::vector<std::string> args = {
        "-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"};
    auto emit = [&](const std::string &source,
                    const std::vector<std::string> &exports = std::vector<std::string>{}) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source, args, "defaults.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(), "fixture.defaults", "test", {}, {}, {}, exports);
    };
    const std::string source =
        "namespace Defaults { int seed(); int accept(int n=seed()); int accept(int n); "
        "struct Value { int first=seed(); int second{first+2}; int slots[2]={20,22}; "
        "Value()=default; Value(int n, int delta=seed()); int get(int n=seed()) const; }; "
        "struct Aggregate { int value=42; }; int aggregate(Aggregate a={}); }";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s %s\n", d.code.c_str(), d.message.c_str(), d.subject.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto different_default = emit("int api(int n=42);");
    auto other_default = emit("int api(int n=43);");
    assert(different_default.success && other_default.success &&
           different_default.identity.semantic_fingerprint != other_default.identity.semantic_fingerprint);
    auto first = emit("struct Data { int n=42; };");
    auto second = emit("struct Data { int n=43; };");
    assert(first.success && second.success &&
           first.identity.semantic_fingerprint != second.identity.semantic_fingerprint);
    auto excluded = emit("int hidden(); int api(int n=hidden());", {"api"});
    assert(!excluded.success && std::any_of(excluded.diagnostics.begin(), excluded.diagnostics.end(),
        [](const auto &d) { return d.code == "O3IFC156"; }));
    auto local = emit("static int hidden(); int api(int n=hidden());");
    assert(!local.success && std::any_of(local.diagnostics.begin(), local.diagnostics.end(),
        [](const auto &d) { return d.code == "O3IFC112"; }));
    auto spi = emit("__attribute__((annotate(\"objc3.spi:secret\"))) int hidden(); int api(int n=hidden());");
    assert(!spi.success && std::any_of(spi.diagnostics.begin(), spi.diagnostics.end(),
        [](const auto &d) { return d.code == "O3IFC158"; }));
    std::erase(decoded->required_semantics, "objc3.cxx-declaration-expressions.v1");
    decoded->module.semantic_fingerprint = ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
  }
}

void TestCxxUsingDeclarations() {
  const std::vector<llvm::StringRef> semantics={
      "objc3.cxx-data-interfaces.v1","objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1","objc3.cxx-class-structure.v1",
      "objc3.cxx-namespaces.v1","objc3.cxx-using.v1"};
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    const std::vector<std::string> args={"-x","c++",standard,"--target=x86_64-unknown-linux-gnu"};
    auto emit=[&](const std::string &source) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,"using.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(),"fixture.using","test");
    };
    const std::string source="namespace A { struct Item { int n; }; int call(Item); int call(int); } "
        "namespace C { long call(long); } namespace B { using A::Item; using A::call; using C::call; } "
        "using B::call; using namespace A; enum class Choice { Answer=42 }; using enum Choice; "
        "struct Base { Base(int); protected: int get() const; }; "
        "struct Derived:Base { using Base::Base; using Base::get; };";
    auto emitted=emit(source);
    for(const auto &d:emitted.diagnostics) std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());
    assert(emitted.success);
    auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,semantics);
    assert(decoded);
    assert(std::any_of(decoded->declarations.begin(),decoded->declarations.end(),[](const auto &d) { return d.kind=="UsingDirective"; }));
    auto round=emit(emitted.source_bytes);
    assert(round.success && round.source_bytes==emitted.source_bytes &&
           round.identity.semantic_fingerprint==emitted.identity.semantic_fingerprint);
    auto a=emit("namespace A { int f(); } namespace B { int f(); } using A::f;");
    auto b=emit("namespace A { int f(); } namespace B { int f(); } using B::f;");
    assert(a.success && b.success && a.identity.semantic_fingerprint!=b.identity.semantic_fingerprint);
    std::erase(decoded->required_semantics,"objc3.cxx-using.v1");
    decoded->module.semantic_fingerprint=ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased=EncodeSemanticInterfaceSidecar(*decoded,semantics);
    assert(!erased);llvm::consumeError(erased.takeError());
  }
}

void TestCxxNestedTypes() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-nested-types.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    const std::vector<std::string> args={"-x","c++",standard,"--target=x86_64-unknown-linux-gnu"};
    auto emit=[&](const std::string &source) {
      auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,"nested.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(),"fixture.nested","test");
    };
    auto emitted=emit("class Family { struct Hidden { int token; }; "
        "protected: using Counter=long; public: enum class Kind:unsigned { One=1, Two=2 }; "
        "struct Value { int n; Kind kind; int get() const; }; "
        "using Result=Value; using PublicHidden=Hidden; Value stored; }; "
        "int Family::Value::get() const { return n; }");
    for (const auto &d:emitted.diagnostics)
      std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());
    assert(emitted.success);
    auto decoded=DecodeSemanticInterfaceSidecar(emitted.sidecar_json,emitted.source_bytes,semantics);
    assert(decoded);
    bool hidden=false,alias=false,method=false,enumerator=false;
    for(const auto &d:decoded->declarations) {
      hidden |= d.qualified_name=="Family::Hidden" && d.canonical_type_identity.find(";access=private")!=std::string::npos;
      alias |= d.qualified_name=="Family::Result" && d.kind=="TypeAlias";
      method |= d.qualified_name=="Family::Value::get";
      enumerator |= d.qualified_name=="Family::Kind::Two";
    }
    assert(hidden && alias && method && enumerator);
    auto round=emit(emitted.source_bytes);
    assert(round.success && round.source_bytes==emitted.source_bytes &&
           round.identity.semantic_fingerprint==emitted.identity.semantic_fingerprint);
    auto pub=emit("struct Family { struct Value { int n; }; }; ");
    auto priv=emit("class Family { struct Value { int n; }; }; ");
    assert(pub.success && priv.success && pub.identity.semantic_fingerprint!=priv.identity.semantic_fingerprint);
    std::erase(decoded->required_semantics,"objc3.cxx-nested-types.v1");
    decoded->module.semantic_fingerprint=ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased=EncodeSemanticInterfaceSidecar(*decoded,semantics);
    assert(!erased);llvm::consumeError(erased.takeError());
  }
}

void TestCxxNamedNamespaces() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-namespaces.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    const std::vector<std::string> args = {
        "-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"};
    auto emit = [&](const std::string &source,
                    const std::vector<std::string> &exports = std::vector<std::string>{}) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source, args, "namespaces.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(), "fixture.namespaces",
                                   "test", {}, {}, {}, exports);
    };
    const std::string source =
        "namespace A { using Value=int; struct Node { Value n; }; "
        "int api(Node); int overload(int); long overload(long); "
        "namespace { int helper() { return 1; } } } "
        "namespace B { using Value=long; } "
        "namespace A { int api(Node v) { return v.n + helper(); } } "
        "namespace Outer { inline namespace v1 { struct Token { int n; }; "
        "int run(Token); } }";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s\n", d.code.c_str(), d.message.c_str());
    assert(emitted.success && emitted.source_bytes.find("helper") == std::string::npos);
    auto decoded = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    std::string a, b;
    for (const auto &d : decoded->declarations) {
      if (d.qualified_name == "A::Value") a=d.stable_id;
      if (d.qualified_name == "B::Value") b=d.stable_id;
    }
    assert(!a.empty() && !b.empty() && a != b);
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    auto selected = emit(source, {"A::Node", "A::Value", "A::api"});
    assert(selected.success && selected.source_bytes.find("namespace B") == std::string::npos &&
           selected.source_bytes.find("overload") == std::string::npos);
    auto whole = emit(source, {"A"});
    assert(whole.success && whole.source_bytes.find("overload") != std::string::npos &&
           whole.source_bytes.find("namespace B") == std::string::npos);
    auto excluded = emit(source, {"A::api"});
    assert(!excluded.success);
    auto aliases = emit(source + " namespace Alias = A; namespace Again = Alias;");
    assert(aliases.success);
    auto alias_round = emit(aliases.source_bytes);
    assert(alias_round.success && alias_round.source_bytes == aliases.source_bytes &&
           alias_round.identity.semantic_fingerprint == aliases.identity.semantic_fingerprint);
    assert(!emit(source + " namespace Alias = A;", {"Alias"}).success);
    assert(emit(source, {"Outer::v1::Token", "Outer::v1::run"}).success);
    std::erase(decoded->required_semantics, "objc3.cxx-namespaces.v1");
    decoded->module.semantic_fingerprint =
        ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
    auto external = emit("namespace Q { int api(); } int Q::api() { return 42; }");
    assert(external.success && external.source_bytes.find("Q::api") == std::string::npos);
    assert(emit("namespace Q { extern \"C\" int api(); } "
                "namespace R { extern \"C\" int api(); }").success);
    assert(emit("namespace Q { extern \"C\" int api(); }").success);
    auto ordinary = emit("namespace X { int api(); }");
    auto inlined = emit("inline namespace X { int api(); }");
    assert(ordinary.success && inlined.success &&
           ordinary.identity.semantic_fingerprint != inlined.identity.semantic_fingerprint);
  }
}

void TestCxxClassDispatch() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1", "objc3.cxx-class-structure.v1",
      "objc3.cxx-class-dispatch.v1"};
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    const std::vector<std::string> args = {
        "-x", "c++", standard, "--target=x86_64-unknown-linux-gnu"};
    auto emit = [&](const std::string &source) {
      auto ast = clang::tooling::buildASTFromCodeWithArgs(source, args,
                                                        "dispatch.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      return EmitSemanticInterface(ast->getASTContext(), "fixture.dispatch",
                                   "test");
    };
    const std::string source =
        "struct Root { int n; Root(); virtual ~Root(); "
        "virtual int get() const = 0; virtual Root *self(); }; "
        "struct Left : virtual Root { Left(); int get() const override; }; "
        "struct Right : virtual Root { Right(); virtual int right() const; }; "
        "struct Diamond final : Left, Right { Diamond(); ~Diamond() override; "
        "int get() const override final; Diamond *self() override; };";
    auto emitted = emit(source);
    for (const auto &d : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s\n", d.code.c_str(), d.message.c_str());
    assert(emitted.success);
    auto decoded = DecodeSemanticInterfaceSidecar(
        emitted.sidecar_json, emitted.source_bytes, semantics);
    assert(decoded);
    assert(std::any_of(decoded->declarations.begin(), decoded->declarations.end(),
                       [](const auto &d) {
      return d.qualified_name == "Diamond" &&
             d.canonical_type_identity.find(";vbase=") != std::string::npos &&
             d.canonical_type_identity.find(";final=1") != std::string::npos;
    }));
    assert(std::any_of(decoded->declarations.begin(), decoded->declarations.end(),
                       [](const auto &d) {
      return d.qualified_name == "Diamond::get" &&
             d.canonical_type_identity.find(";override=") != std::string::npos &&
             d.canonical_type_identity.find(";final=1") != std::string::npos;
    }));
    auto round = emit(emitted.source_bytes);
    assert(round.success && round.source_bytes == emitted.source_bytes &&
           round.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    std::erase(decoded->required_semantics, "objc3.cxx-class-dispatch.v1");
    decoded->module.semantic_fingerprint =
        ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
    auto erased = EncodeSemanticInterfaceSidecar(*decoded, semantics);
    assert(!erased); llvm::consumeError(erased.takeError());
    auto no_rtti_args = args;
    no_rtti_args.push_back("-fno-rtti");
    auto no_rtti = clang::tooling::buildASTFromCodeWithArgs(
        "struct V { virtual int get(); };", no_rtti_args, "no-rtti.cpp");
    assert(no_rtti && !no_rtti->getDiagnostics().hasErrorOccurred());
    assert(!EmitSemanticInterface(no_rtti->getASTContext(),
                                  "fixture.no-rtti", "test").success);
    auto ordered = emit("struct V { virtual int a(); virtual int b(); };");
    auto reversed = emit("struct V { virtual int b(); virtual int a(); };");
    assert(ordered.success && reversed.success &&
           ordered.identity.semantic_fingerprint != reversed.identity.semantic_fingerprint);
    auto pure = emit("struct V { virtual int a()=0; };");
    auto concrete = emit("struct V { virtual int a(); };");
    assert(pure.success && concrete.success &&
           pure.identity.semantic_fingerprint != concrete.identity.semantic_fingerprint);
    auto ordinary = emit("struct R { int n; }; struct V : R {};");
    auto shared = emit("struct R { int n; }; struct V : virtual R {};");
    assert(ordinary.success && shared.success &&
           ordinary.identity.semantic_fingerprint != shared.identity.semantic_fingerprint);
    auto final = emit("struct V final { virtual int a(); };");
    assert(final.success && final.identity.semantic_fingerprint != concrete.identity.semantic_fingerprint);
  }
}

void TestCxxClassLifecycles() {
  const std::vector<llvm::StringRef> semantics = {
      "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
      "objc3.cxx-class-lifecycles.v1"};
  const std::vector<std::string> args = {"-x", "c++", "-std=c++20",
                                         "--target=x86_64-unknown-linux-gnu"};
  const char *source =
      "class Value { int value; mutable int reads; "
      "int helper() const; public: explicit Value(int); Value(const Value&); "
      "Value(Value&&); Value& operator=(const Value&); Value& "
      "operator=(Value&&); "
      "~Value() noexcept; explicit operator bool() const; int get() const; }; "
      "class MoveOnly { int value; public: MoveOnly(int); "
      "MoveOnly(const MoveOnly&)=delete; MoveOnly(MoveOnly&&); ~MoveOnly(); };";
  auto ast =
      clang::tooling::buildASTFromCodeWithArgs(source, args, "classes.cpp");
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());
  auto emitted =
      EmitSemanticInterface(ast->getASTContext(), "fixture.classes", "test");
  for (const auto &d : emitted.diagnostics)
    std::fprintf(stderr, "%s: %s\n", d.code.c_str(), d.message.c_str());
  assert(emitted.success);
  auto decoded = DecodeSemanticInterfaceSidecar(
      emitted.sidecar_json, emitted.source_bytes, semantics);
  assert(decoded);
  bool ctor = false, dtor = false, conversion = false, deleted = false;
  for (const auto &d : decoded->declarations) {
    ctor |= d.kind == "CXXConstructor";
    dtor |= d.kind == "CXXDestructor";
    conversion |= d.kind == "CXXConversion";
    deleted |=
        d.canonical_type_identity.find(";deleted=1") != std::string::npos;
  }
  assert(ctor && dtor && conversion && deleted);
  auto round = clang::tooling::buildASTFromCodeWithArgs(
      emitted.source_bytes, args, "round-classes.cpp");
  assert(round && !round->getDiagnostics().hasErrorOccurred());
  auto regenerated =
      EmitSemanticInterface(round->getASTContext(), "fixture.classes", "test");
  assert(regenerated.success && regenerated.identity.semantic_fingerprint ==
                                    emitted.identity.semantic_fingerprint);
  std::erase(decoded->required_semantics, "objc3.cxx-class-lifecycles.v1");
  decoded->module.semantic_fingerprint =
      ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
  auto invalid = EncodeSemanticInterfaceSidecar(*decoded, semantics);
  assert(!invalid);
  assert(llvm::toString(invalid.takeError()).find("class lifecycle") !=
         std::string::npos);
}

void TestBoundedCxxSemanticInterfaces() {
  {
    const std::vector<llvm::StringRef> semantics = {
        "objc3.cxx-data-interfaces.v1", "objc3.cxx-member-interfaces.v1",
        "objc3.canonical-function-effects.v1", "objc3.async-entry-abi.v5"};
    std::string previous;
    for (const char *signature :
         {"int get() const __objc3_async __objc3_throws(id<First>);",
          "int get() const __objc3_async __objc3_throws(id<Second>);",
          "static int get() __objc3_async __objc3_throws(id<Second>);"}) {
      const std::string source =
          std::string(
              "@protocol Error @end @protocol First<Error> @end "
              "@protocol Second<Error> @end struct Worker { int value; ") +
          signature + " };";
      const std::vector<std::string> args = {
          "-x",
          "objective-c++",
          "-std=c++20",
          "-fobjc-version=3",
          "-fobjc-runtime=gnustep-2.2",
          "--target=x86_64-unknown-linux-gnu"};
      auto ast =
          clang::tooling::buildASTFromCodeWithArgs(source, args, "members.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      auto emitted = EmitSemanticInterface(ast->getASTContext(),
                                           "fixture.members", "test");
      for (const auto &diagnostic : emitted.diagnostics)
        std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(),
                     diagnostic.message.c_str());
      assert(emitted.success &&
             emitted.identity.semantic_fingerprint != previous);
      auto decoded = DecodeSemanticInterfaceSidecar(
          emitted.sidecar_json, emitted.source_bytes, semantics);
      assert(decoded);
      bool found = false;
      for (const auto &declaration : decoded->declarations)
        if (declaration.kind == "CXXMethod") {
          found = true;
          assert(declaration.effects.is_async &&
                 declaration.effects.is_throwing);
          assert(declaration.canonical_type_identity.find(";cxx-method=") !=
                 std::string::npos);
        }
      assert(found);
      previous = emitted.identity.semantic_fingerprint;
      auto round = clang::tooling::buildASTFromCodeWithArgs(
          emitted.source_bytes, args, "round-members.mm");
      assert(round && !round->getDiagnostics().hasErrorOccurred());
      auto regenerated = EmitSemanticInterface(round->getASTContext(),
                                               "fixture.members", "test");
      assert(regenerated.success &&
             regenerated.identity.semantic_fingerprint == previous);
      std::erase(decoded->required_semantics, "objc3.cxx-member-interfaces.v1");
      decoded->module.semantic_fingerprint =
          ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
      auto invalid = EncodeSemanticInterfaceSidecar(*decoded, semantics);
      assert(!invalid);
      assert(llvm::toString(invalid.takeError())
                 .find("C++ member declarations and required capability") !=
             std::string::npos);
    }
  }
  std::string prior;
  const std::vector<llvm::StringRef> semantics = {
      "objc3.canonical-function-effects.v1", "objc3.native-carrier-types.v1",
      "objc3.native-return-ownership.v1", "objc3.direct-dispatch.v1",
      "objc3.cxx-data-interfaces.v1"};
  for (const char *language : {"c++", "objective-c++"}) {
    const bool objc = std::string(language) == "objective-c++";
    for (const char *standard : {"c++20", "c++23"}) {
      std::string source =
          "namespace { struct Private { Private(); }; int helper() { return 0; "
          "} } "
          "using Scalar = int; struct Packet { Scalar value; }; "
          "extern \"C\" Packet c_echo(Packet); Packet echo(Packet); "
          "int overloaded(int); long overloaded(long); extern \"C\" int "
          "shared;";
      if (objc)
        source +=
            "typedef Optional(id) Strong; "
            "struct Owned { Strong value; id __weak weak; Strong values[2]; }; "
            "extern \"C\" Owned owned_echo(Owned);";
      std::vector<std::string> args = {"-x", language,
                                       std::string("-std=") + standard,
                                       "--target=x86_64-unknown-linux-gnu"};
      if (objc)
        args.insert(args.end(), {"-fobjc-version=3", "-fobjc-arc",
                                 "-fobjc-runtime=gnustep-2.2"});
      auto ast =
          clang::tooling::buildASTFromCodeWithArgs(source, args, "data.cpp");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      auto emitted =
          EmitSemanticInterface(ast->getASTContext(), "fixture.cxx", "test");
      for (const auto &diagnostic : emitted.diagnostics)
        std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(),
                     diagnostic.message.c_str());
      assert(emitted.success && emitted.identity.semantic_fingerprint != prior);
      assert(emitted.source_bytes.find("extern \"C\"") != std::string::npos);
      assert(emitted.identity.language_profile.find(
                 std::string("standard=") + standard) != std::string::npos);
      prior = emitted.identity.semantic_fingerprint;
      auto round = clang::tooling::buildASTFromCodeWithArgs(
          emitted.source_bytes, args, "relocated.cpp");
      assert(round && !round->getDiagnostics().hasErrorOccurred());
      auto regenerated =
          EmitSemanticInterface(round->getASTContext(), "fixture.cxx", "test");
      assert(regenerated.success &&
             regenerated.source_bytes == emitted.source_bytes);
      assert(regenerated.identity.semantic_fingerprint == prior);
      auto decoded = DecodeSemanticInterfaceSidecar(
          emitted.sidecar_json, emitted.source_bytes, semantics);
      assert(decoded);
      auto erased = *decoded;
      std::erase(erased.required_semantics, "objc3.cxx-data-interfaces.v1");
      auto invalid = ValidateSemanticInterface(
          erased, decoded->module, decoded->canonical_source_sha256,
          decoded->declarations,
          {"objc3.canonical-function-effects.v1",
           "objc3.native-carrier-types.v1", "objc3.native-return-ownership.v1",
           "objc3.direct-dispatch.v1", "objc3.cxx-data-interfaces.v1"});
      bool rejected = false;
      for (const auto &diagnostic : invalid.diagnostics)
        rejected |= diagnostic.code == "O3IFC016";
      assert(rejected);
    }
  }
  std::string enum_identity;
  for (const char *source : {"enum Mode : unsigned { One=1 };",
                             "enum class Mode : unsigned { One=1 };"}) {
    auto ast = clang::tooling::buildASTFromCodeWithArgs(
        source, {"-x", "c++", "-std=c++20"}, "enum.cpp");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted =
        EmitSemanticInterface(ast->getASTContext(), "fixture.enum", "test");
    assert(emitted.success &&
           emitted.identity.semantic_fingerprint != enum_identity);
    enum_identity = emitted.identity.semantic_fingerprint;
  }
  for (const char *source :
       {"namespace nested { inline int api() { return [](auto value) { return value; }(42); } }",
        "struct Base { int value; }; struct Derived : virtual Base { int "
        "second=1; int get() const { return [](auto value) { return value; }(42); } };",

        "struct Method { int value; inline int get() const { return [](auto value) { return value; }(42); } "
        "};",
        "struct Default { int value=[](auto value) { return value; }(42); };",
        "struct Conditional { explicit(true) Conditional(int); };",
        "struct Comparison { int n; template<class T> friend bool operator==(const Comparison &a, const T &b) "
        "{ return a.n==b.n; } };",
        "struct Nested { struct Inner { int value=[](auto value) { return value; }(42); }; Inner child; };",
        "int api(int argument=[](auto value) { return value; }(42));", "inline int api() { return [](auto value) { return value; }(42); }",
        "extern constexpr int exported=[](auto value) { return value; }(42);", "extern const int exported=[](auto value) { return value; }(42);",
        "constexpr int evaluated() { return [](auto value) { return value; }(42); }",
        "struct Public {struct {int n;} value;};"}) {
    auto ast = clang::tooling::buildASTFromCodeWithArgs(
        source,
        {"-x", "c++", "-std=c++20", "--target=x86_64-unknown-linux-gnu"},
        "unsupported.cpp");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted = EmitSemanticInterface(ast->getASTContext(),
                                         "fixture.unsupported", "test");
    if (llvm::StringRef(source).contains("[](auto value)")) {
      assert(emitted.success);
      assert(emitted.sidecar_json.find("objc3.cxx-generic-local-lambdas.v2")!=std::string::npos);
    } else {
      assert(!emitted.success && emitted.source_bytes.empty() && emitted.sidecar_json.empty());
    }
  }
}

} // namespace

int main(int argc,char **argv) {
  if(argc==2) {
    const std::string group=argv[1];
    if(group=="standard-library-headers") {TestStandardLibraryHeaderContracts();return 0;}
    if(group=="standard-library-specializations") {TestStandardLibrarySpecializations();return 0;}
    return 2;
  }
  if(argc!=1)return 2;
  TestPrimitiveAndObjectiveCContractIdentity();
  TestOrdinaryCDeclarationsRoundTrip();
  TestStandardLibraryImportsAndInstancetype();
  TestNativeOwnershipContractsRoundTrip();
  TestOpaqueKeyPathSignatureRoundTrips();
  TestKeyPathSubscriptBodies();
  TestNativeManagedUnionPayloadReplay();
  TestNativeUnnamedEnumReplay();
  TestNativeStorageLifetimeReplay();
  TestNativeUnionKeyPathReplay();
  TestResourceKeyPathReplay();
  TestDependentKeyPathLiteralBodies();
  TestNSObjectStorageCapability();
  TestBorrowedMemoryAliasReplay();
  TestBorrowedStaticStorageReplay();
  TestBorrowedCallableOwnerCapability();
  TestAutomaticOwnerUnwindReplay();
  TestBlockMutationPackageReplay();
  TestBlockProducerTransferReplay();
  TestBlockMutationReplay();
  TestDependentExpressionCapabilityScan();
  TestComputedBlockValuesReplay();
  TestComputedBlockArrayMemberReplay();
  TestNativeBlockArrayMemberReplay();
  TestNativeVirtualSubobjectReplay();
  TestNativeFunctionTryReplay();
  TestNativeSubobjectReplay();
  TestNativeArrayLifetimeReplay();
  TestNativeAllocationReplay();
  TestNativeDisposalReplay();
  TestInitializedOwnerReplay();
  TestNativeFinallyReplay();
  TestManagedRuntimeScopeReplay();
  TestBorrowedReceiverResultReplay();
  TestBorrowedInteriorStorageReplay();
  TestAsyncConsumedOwnershipCapability();
  TestConsumedAndMethodOwnershipRoundTrips();
  TestUnmodeledAttributesFailClosed();
  TestCAsyncPhysicalABIRoundTrips();
  TestCAsyncPhysicalABIImportBoundaries();
  TestPublishedIsolationBodies();
  TestPublishedRuntimeDeclarations();
  TestObjectiveCContainerRoundTrips();
  TestTypedEffectSemanticInterfacesRoundTrip();
  TestManagedReturnOwnershipRoundTrips();
  TestDirectMethodIdentityRoundTrip();
  TestManagedIvarAndPropertyRoundTrip();
  TestBoundedCxxSemanticInterfaces();
  TestCxxClassLifecycles();
  TestCxxClassStructure();
  TestCxxClassDispatch();
  TestCxxNamedNamespaces();
  TestCxxNestedTypes();
  TestCxxUsingDeclarations();
  TestCxxDeclarationExpressions();
  TestCxxDefinitions();
  TestCxxDefaultedComparisons();
  TestCxxOutOfLineDefaulting();
  TestCxxImmediateDefaulting();
  TestCxxNamespaceAttributes();
  TestCxxLambdaExpressions();
  TestCxxThreadLocalData();
  TestCxxFriends();
  TestNativeExportRoots();
  TestCxxLocalExpressions();
  TestCxxLocalTypes();
  TestGenericLocalLambdas();
  TestGenericLocalContracts();
  TestCxxUnionValues();
  TestPublicConcepts();
  TestPublicFunctionTemplates();
  TestPublicAliasTemplates();
  TestFunctionTemplateDeclarations();
  TestPublicVariableTemplates();
  TestIncompleteTemplateSpecializations();
  TestIncompleteTemplateDeclarations();
  TestPublicClassTemplates();
  TestClassMemberTemplates();
  TestClassMemberAliasTemplates();
  TestFunctionSpecializations();
  TestVariableSpecializations();
  TestClassSpecializations();
  TestPartialClassSpecializations();
  TestPartialVariableSpecializations();
  TestExternalVariableSpecializations();
  TestCInlineDefinitionContract();
  TestDeclarationOnlyFunctionTemplates();
  TestNativeAllocatorAndDiagnosticContracts();
  TestNativeDeclarationAttributes();
  TestCOverloadableInterfaces();
  TestUnnamedRecordProjections();
  TestUnnamedTemplateProjections();
  TestStandardCoroutineTypesAndTraits();
  TestCustomCoroutineBodies();
  TestForeignImportAttributes();
  TestNestedFactoryTypes();
  TestLambdaFactoryResults();
  TestClosureResultProjections();
  TestFactoryValueArguments();
  TestGenericFactoryProjections();
  TestFactoryTemplateArguments();
  TestFactoryTypeProjections();
  TestLocalFactoryResults();
  TestClosureFactories();
  TestCheckedIsolatedPropertyReplay();
  TestAsyncIsolatedPropertyReplay();
  TestMixedCarrierCallReplay();
  TestNativeByrefStorageReplay();
  TestNativeBlockAlignmentReplay();
  TestCppCarrierValueReplay();
  TestCppCarrierRuntimeReplay();
  TestCheckedCppRecordResultReplay();
  TestCppCarrierDefaultStorageReplay();
  TestCheckedCppMessageReplay();
  TestAtomicManagedPropertyReplay();
  TestImplicitCheckedRecordReplay();
  TestNativeARCRecordPayloadReplay();
  TestNativeCarrierLambdaReplay();
  TestMixedCarrierStorageReplay();
  TestCppCarrierCompositionReplay();
  TestNativeCppCarrierCallReplay();
  TestNativeCheckedCppResultReplay();
  TestIsolatedPropertyReplay();
  TestOwnedNilResultReplay();
  TestStrictAsyncProfileReplay();
  TestNativeDependentAwaitReplay();
  TestConcurrentBlockValueReplay();
  TestConcurrentCallableCaptureReplay();
  TestEffectfulLambdas();
  TestAddressTemplateArguments();
  TestTemplateCallableProjections();
  TestEffectfulTemplateMembers();
  TestEffectfulFunctionTemplates();
  TestClosedParentPatterns();
  TestClosedParentMemberSpecializations();
  TestDependentMemberPartials();
  TestConcreteSpecialMemberSpecializations();
  TestConcreteMemberVariableSpecializations();
  TestConcreteMemberFunctionSpecializations();
  TestConcreteMemberClassSpecializations();
  TestDetachedNestedTemplates();
  TestDetachedStaticDataTemplates();
  TestDetachedCallableTemplates();
  TestLargeExportPolicy();
  TestTemplateTemplateParameters();
  TestConcreteNestedTemplates();
  TestConcreteMemberTemplates();
  TestVariadicTemplates();
  TestNestedTemplateTypes();
  TestTemplateStructure();
  TestTemplateDeduction();
  TestCarrierExpressionRoundtrip();
  TestDeletedAggregateMemberIdentity();
  return 0;
}
