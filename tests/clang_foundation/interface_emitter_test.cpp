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
#include <string>
#include <vector>

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTImporter.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/ADT/SmallString.h"

using namespace objc3::clang_foundation;

namespace {

void TestOrdinaryCDeclarationsRoundTrip() {
  constexpr llvm::StringRef source =
      "typedef struct Point { int x; int y; } Point;\n"
      "extern int shared_value;\n"
      "int add_points(Point lhs, Point rhs);\n"
      "typedef enum Kind { first = 2, second = 9 } Kind;\n";
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      source, {"-x", "c", "-std=c11", "--target=x86_64-unknown-linux-gnu"},
      "interface_fixture.c");
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());

  auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.points",
                                       "test-package");
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
       {"namespace nested { inline int api() { return [] { return 42; }(); } }",
        "template<class T> struct Generic { T value; };",
        "struct Base { int value; }; struct Derived : virtual Base { int "
        "second=1; int get() const { return [] { return 42; }(); } };",

        "struct Method { int value; inline int get() const { return [] { return 42; }(); } "
        "};",
        "struct Default { int value=[] { return 42; }(); };",
        "struct Conditional { explicit(true) Conditional(int); };",
        "struct Defaulted { consteval Defaulted(); }; consteval Defaulted::Defaulted() = default;",
        "struct Comparison { int n; friend bool operator==(const Comparison &a, const Comparison &b) "
        "{ return a.n==b.n; } };",
        "struct Immediate { consteval Immediate() = default; };",
        "struct Nested { struct Inner { int value=[] { return 42; }(); }; Inner child; };",
        "int api(int argument=[] { return 42; }());", "inline int api() { return [] { return 42; }(); }",
        "extern constexpr int exported=[] { return 42; }();", "extern const int exported=[] { return 42; }();",
        "constexpr int evaluated() { return [] { return 42; }(); }",
        "union Overlap { int one; long two; };"}) {
    auto ast = clang::tooling::buildASTFromCodeWithArgs(
        source,
        {"-x", "c++", "-std=c++20", "--target=x86_64-unknown-linux-gnu"},
        "unsupported.cpp");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted = EmitSemanticInterface(ast->getASTContext(),
                                         "fixture.unsupported", "test");
    assert(!emitted.success && emitted.source_bytes.empty() &&
           emitted.sidecar_json.empty());
  }
}

} // namespace

int main() {
  TestOrdinaryCDeclarationsRoundTrip();
  TestStandardLibraryImportsAndInstancetype();
  TestNativeOwnershipContractsRoundTrip();
  TestOpaqueKeyPathSignatureRoundTrips();
  TestConsumedAndMethodOwnershipRoundTrips();
  TestUnmodeledAttributesFailClosed();
  TestCAsyncPhysicalABIRoundTrips();
  TestCAsyncPhysicalABIImportBoundaries();
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
  TestDeletedAggregateMemberIdentity();
  return 0;
}
