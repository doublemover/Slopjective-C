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
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/ADT/SmallString.h"

using namespace objc3::clang_foundation;

namespace {

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
        "objc3.canonical-function-effects.v1",
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
        inline int explicit_literal(IndexedRoot *_Nonnull root) {
          return KeyPathGet(@keypath(IndexedRoot,count),root);
        }
      )","indexed-provider.mm");
      assert(ast && !ast->getDiagnostics().hasErrorOccurred());
      const std::vector<std::string> exports={"IndexedRoot","indexed_read","indexed_write",
                                             "indexed_literal","explicit_literal","indexed_nullable"};
      const auto emitted=EmitSemanticInterface(ast->getASTContext(),"fixture.indexed","test",{},{},{},exports);
      for (const auto &d:emitted.diagnostics)
        std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
      assert(emitted.success);
      assert(emitted.source_bytes.find("root[path]")!=std::string::npos);
      assert(emitted.source_bytes.find("__objc3_keypath_")==std::string::npos);
      assert(emitted.sidecar_json.find("objc3.keypath-subscripts.v1")!=std::string::npos);
      assert(emitted.sidecar_json.find("objc3.keypath-nullable-writes.v1")!=std::string::npos);
      const std::vector<llvm::StringRef> supported={"objc3.typed-keypaths.v1",
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
          "objc3.native-ownership-contracts.v1",
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
  // A dropped parameter attribute can change hidden arguments even when the
  // ordinary function pointer ABI appears to have the same source type.
  auto unmodeled = clang::tooling::buildASTFromCodeWithArgs(
      "int size(const char *const p __attribute__((pass_object_size(0))));",
      {"-x", "c", "-std=c17"}, "unmodeled-param.c");
  assert(unmodeled && !unmodeled->getDiagnostics().hasErrorOccurred());
  auto rejected =
      EmitSemanticInterface(unmodeled->getASTContext(), "fixture.size", "test");
  assert(!rejected.success);
  bool diagnosed = false;
  for (const auto &diagnostic : rejected.diagnostics)
    diagnosed |= diagnostic.code == "O3IFC116";
  assert(diagnosed);
}

void TestUnmodeledAttributesFailClosed() {
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      "__attribute__((noinline)) int old_api(void);\n",
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
      "objc3.isolation-contracts.v1", "objc3.async-entry-abi.v1",
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
      "objc3.isolation-contracts.v1","objc3.async-entry-abi.v1",
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
  auto build = [](bool cpp) {
    std::vector<std::string> args = {"-x",
                                     cpp ? "objective-c++" : "objective-c",
                                     cpp ? "-std=c++20" : "-std=c17",
                                     "-fobjc-version=3",
                                     "-fobjc-runtime=gnustep-2.2",
                                     "--target=x86_64-unknown-linux-gnu"};
    clang::tooling::FileContentMappings files;
    AddRuntimeInterfaceFiles(files, args);
    const std::string source =
        std::string("#include <objc3c/runtime/objc3_runtime_services.h>\n") +
        (cpp ? "template<class T> struct TaskCarrier { void *context; }; "
               "template<class T> using __objc3_async_task = TaskCarrier<T>; "
               "int task_fetch(int) __objc3_async; "
               "int fetch(int) __objc3_async_frame; "
               "void inspect(void) { (void)task_fetch(1); (void)fetch(2); }"
             : "int fetch(int) __objc3_async; "
               "void inspect(void) { (void)fetch(1); }");
    return clang::tooling::buildASTFromCodeWithArgs(
        source, args, cpp ? "async_import.mm" : "async_import.m", "clang-tool",
        std::make_shared<clang::PCHContainerOperations>(),
        clang::tooling::getClangStripDependencyFileAdjuster(), files);
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
      "objc3.cxx-block-expressions.v1", "objc3.cxx-statement-expressions.v1",
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
    for (const auto capability:{"objc3.cxx-block-expressions.v1","objc3.cxx-statement-expressions.v1"}) {
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
    auto escapes = emit("inline auto escape() { struct Local {int n;}; return Local{42}; }");
    assert(!escapes.success);
    for (const auto *boundary : {
             "static int hidden(); inline int call() {struct Local {int get() {return hidden();}}; return Local{}.get();}",
             "inline auto escape() {struct {int n;} value{42};return value;}",
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
    auto escapes=emit("inline auto api(){return [](auto x){return x;};}");
    assert(!escapes.success);
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
    assert(!address.success);
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
    auto escaping=emit("inline auto escape(){union Local{int n;};return Local{42};}");
    assert(!escaping.success);
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
    auto hidden=emit("static int hidden(){return 42;}struct Outer{template<class T>struct Box{int f(){return hidden();}};};");
    assert(!hidden.success && std::any_of(hidden.diagnostics.begin(),hidden.diagnostics.end(),
        [](const auto &d){return d.code=="O3IFC112";}));
    for(const auto *source:{
        "struct Outer{template<class T>struct Box;};",
        "struct Outer{template<class T>union Box{T value;};};",
        "struct Outer{template<class T>struct Box{T value;};};template<class T>struct Outer::Box<T*>{T *value;};",
        "struct Outer{template<class T>struct Box;};template<class T>struct Outer::Box{T value;};"})
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
    for(const auto *source:{
        "struct Concrete{template<class U>int f(U);};",
        "struct Concrete{template<class U>static U value;};",
        "struct Concrete{template<template<class>class U>int f();};",
        "struct Concrete{template<class U>int f(U n){return n;}};template<>int Concrete::f<int>(int n){return n+1;}",
        "struct Concrete{template<class U>inline static U value=U{40};};template<>inline int Concrete::value<int> =42;"})
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
      "template<class T>struct Outer{template<class U>static U value;};",
      "template<class T>struct Outer{template<class U>struct Inner{};template<class U>struct Inner<U*>{};};"})
      assert(!emit(gated).success);
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
        "template<class T>struct Box{template<class U>int f(U,...)=delete;};",
        "template<class T>struct Box{template<class U>int f(U n){return n;}};template<>template<>int Box<int>::f<int>(int n){return n+1;}"})
      assert(!emit(gated).success);
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
    for(const auto *gated:{"template<class T>struct Box;",
                          "template<class T>struct Box{T api();};",
                          "template<class T>struct Box{};template<class T>struct Box<T*>{Box();};",
                          "template<class T>struct Box{};template<>struct Box<int>{Box();};",
                          "template<class T>struct Box{template<class U> U api(U n);};"})
      assert(!emit(gated).success);
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
    for (const auto *gated:{"template<class T> extern T value;",
                           "template<class T> inline int value=40;template<> int value<long> __attribute__((used))=42;",
                           "template<class T> inline int value=40;template<class T> extern int value<T*>;"}) {
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
      "objc3.cxx-definitions.v1", "objc3.cxx-thread-local.v1", "objc3.module-owned-names.v1"};
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
    assert(decoded);
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
        "objc3.canonical-function-effects.v1", "objc3.async-entry-abi.v1"};
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

int main() {
  TestOrdinaryCDeclarationsRoundTrip();
  TestStandardLibraryImportsAndInstancetype();
  TestNativeOwnershipContractsRoundTrip();
  TestOpaqueKeyPathSignatureRoundTrips();
  TestKeyPathSubscriptBodies();
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
  TestPublicClassTemplates();
  TestClassMemberTemplates();
  TestClassMemberAliasTemplates();
  TestFunctionSpecializations();
  TestVariableSpecializations();
  TestClassSpecializations();
  TestPartialClassSpecializations();
  TestPartialVariableSpecializations();
  TestExternalVariableSpecializations();
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
