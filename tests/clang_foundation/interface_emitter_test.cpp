#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "clang/AST/ASTContext.h"
#include "clang/Tooling/Tooling.h"

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

  auto decoded = DecodeSemanticInterfaceSidecar(
      emitted.sidecar_json, emitted.source_bytes, {});
  assert(static_cast<bool>(decoded));
  assert(decoded->module.canonical_name == "fixture.points");
  assert(decoded->declarations.size() >= 4);
  assert(decoded->canonical_source_sha256 ==
         ComputeInterfaceSourceSha256(emitted.source_bytes));
}

void TestUnmodeledAttributesFailClosed() {
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      "__attribute__((deprecated)) int old_api(void);\n",
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

void TestObjectiveCContainerRoundTrips() {
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      "__attribute__((objc_root_class)) @interface Widget\n- (int)value;\n@end\n",
      {"-x", "objective-c", "-std=gnu11",
       "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"},
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
  for (const char *source : {
      "void typed_api(void) __objc3_throws(int);\n",
      "typedef void TypedCallback(void) __objc3_throws(int);\n",
      "typedef void TypedCallback(void) __objc3_throws(int); "
      "void accepts_callback(TypedCallback *);\n",
      "typedef void TypedCallback(void) __objc3_throws(int); "
      "struct CallbackStorage { TypedCallback *callback; };\n",
      "void accepts_inline_callback(void (*callback)(void) __objc3_throws(int));\n",
      "@interface EffectWidget\n- (int)value __objc3_throws(int);\n@end\n",
      "@protocol EffectProtocol\n+ (int)value:(int)x __objc3_throws(int);\n@end\n"}) {
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
      found_canonical_effect |= identity.canonical_type_identity.find("throws(int)") !=
          std::string::npos;
      if (identity.effects.is_throwing) {
        assert(identity.effects.throws_kind == "typed");
        assert(identity.effects.error_type_identity == "int");
        assert(identity.canonical_type_identity.find("throws(int)") !=
            std::string::npos);
        found_typed_effect = true;
      }
    }
    assert(found_canonical_effect);
    if (std::string(source).find("accepts_inline_callback") == std::string::npos)
      assert(found_typed_effect);
    auto regenerated_ast = clang::tooling::buildASTFromCodeWithArgs(
        emitted.source_bytes,
        {"-x", "objective-c", "-std=c17", "-fobjc-version=3",
         "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"},
        "relocated-effect-interface.m");
    assert(regenerated_ast && !regenerated_ast->getDiagnostics().hasErrorOccurred());
    auto regenerated = EmitSemanticInterface(regenerated_ast->getASTContext(),
        "fixture.effect", "test-package");
    assert(regenerated.success);
    assert(regenerated.source_bytes == emitted.source_bytes);
    assert(regenerated.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
  }
}

void TestManagedReturnOwnershipRoundTrips() {
  std::string previous_fingerprint;
  std::map<std::string, SemanticInterfaceSidecar> mrc_views;
  for (bool arc : {false, true})
  for (const char *attribute : {"ns_returns_retained", "ns_returns_not_retained", "ns_returns_autoreleased"}) {
    const std::string source = "@protocol Error @end\nid managed(void) __objc3_throws(id<Error>) __attribute__((" +
        std::string(attribute) + "));\n";
    std::vector<std::string> arguments = {"-x", "objective-c", "-std=c17",
        "-fobjc-version=3", "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"};
    if (arc) arguments.push_back("-fobjc-arc");
    auto ast = clang::tooling::buildASTFromCodeWithArgs(source, arguments, "managed-return.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.managed", "test-package");
    for (const auto &diagnostic : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(), diagnostic.message.c_str());
    assert(emitted.success && emitted.source_bytes.find(attribute) != std::string::npos);
    assert(previous_fingerprint != emitted.identity.semantic_fingerprint);
    previous_fingerprint = emitted.identity.semantic_fingerprint;
    auto round_trip = clang::tooling::buildASTFromCodeWithArgs(emitted.source_bytes, arguments, "relocated.m");
    assert(round_trip && !round_trip->getDiagnostics().hasErrorOccurred());
    auto regenerated = EmitSemanticInterface(round_trip->getASTContext(), "fixture.managed", "test-package");
    if (!regenerated.success || regenerated.source_bytes != emitted.source_bytes)
      std::fprintf(stderr, "original:\n%s\nregenerated:\n%s\n", emitted.source_bytes.c_str(), regenerated.source_bytes.c_str());
    assert(regenerated.success && regenerated.source_bytes == emitted.source_bytes);
    assert(regenerated.identity.semantic_fingerprint == emitted.identity.semantic_fingerprint);
    const std::vector<llvm::StringRef> supported = {"objc3.canonical-function-effects.v1", "objc3.native-return-ownership.v1"};
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, supported);
    assert(decoded);
    if (!arc) mrc_views[attribute] = *decoded;
    else {
      const auto &mrc = mrc_views.at(attribute);
      // Ownership modes remain distinct module identities. The declaration
      // contract must agree for this explicitly modeled managed-return API.
      assert(mrc.module.runtime_abi != decoded->module.runtime_abi);
      assert(mrc.module.semantic_fingerprint != decoded->module.semantic_fingerprint);
      assert(mrc.declarations.size() == decoded->declarations.size());
      for (std::size_t i = 0; i != mrc.declarations.size(); ++i)
        assert(SameInterfaceDeclarationIdentity(mrc.declarations[i], decoded->declarations[i]));
    }
    bool found = false;
    for (const auto &declaration : decoded->declarations)
      if (declaration.qualified_name == "managed") {
        assert(declaration.effects.ownership_identity.starts_with("native-return:"));
        found = true;
      }
    assert(found);
    auto stripped = *decoded;
    std::erase(stripped.required_semantics, "objc3.native-return-ownership.v1");
    auto checked = ValidateSemanticInterface(stripped, decoded->module,
        decoded->canonical_source_sha256, decoded->declarations,
        {"objc3.canonical-function-effects.v1", "objc3.native-return-ownership.v1"});
    bool rejected_erasure = false;
    for (const auto &diagnostic : checked.diagnostics)
      rejected_erasure |= diagnostic.code == "O3IFC014";
    assert(rejected_erasure);
    auto old_consumer = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes,
        {"objc3.canonical-function-effects.v1"});
    assert(!old_consumer);
    llvm::consumeError(old_consumer.takeError());
  }
}

void TestDirectMethodIdentityRoundTrip() {
  std::string previous;
  for (bool direct : {false, true}) {
    const std::string source = "__attribute__((objc_root_class)) @interface Direct "
        "- (int)value" + std::string(direct ? " __attribute__((objc_direct))" : "") + "; @end";
    const std::vector<std::string> args = {"-x", "objective-c", "-fobjc-version=3",
        "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"};
    auto ast = clang::tooling::buildASTFromCodeWithArgs(source, args, "direct.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted = EmitSemanticInterface(ast->getASTContext(), "direct.methods", "test");
    assert(emitted.success && emitted.identity.semantic_fingerprint != previous);
    previous = emitted.identity.semantic_fingerprint;
    auto round = clang::tooling::buildASTFromCodeWithArgs(emitted.source_bytes, args, "round.m");
    assert(round && !round->getDiagnostics().hasErrorOccurred());
    auto regenerated = EmitSemanticInterface(round->getASTContext(), "direct.methods", "test");
    assert(regenerated.success && regenerated.identity.semantic_fingerprint == previous);
    const std::vector<llvm::StringRef> supported = {"objc3.direct-dispatch.v1"};
    auto decoded = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, supported);
    assert(decoded);
    if (direct) {
      assert(emitted.source_bytes.find("objc_direct") != std::string::npos);
      assert(decoded->required_semantics == std::vector<std::string>{"objc3.direct-dispatch.v1"});
      auto erased = *decoded;
      erased.required_semantics.clear();
      auto checked = ValidateSemanticInterface(erased, decoded->module,
          decoded->canonical_source_sha256, decoded->declarations, {"objc3.direct-dispatch.v1"});
      bool rejected = false;
      for (const auto &diagnostic : checked.diagnostics) rejected |= diagnostic.code == "O3IFC015";
      assert(rejected);
      auto unaware = DecodeSemanticInterfaceSidecar(emitted.sidecar_json, emitted.source_bytes, {});
      assert(!unaware);
      llvm::consumeError(unaware.takeError());
    }
  }
}

void TestManagedIvarAndPropertyRoundTrip() {
  const std::vector<std::string> args = {
      "-x", "objective-c", "-std=c17", "-fobjc-version=3", "-fobjc-arc",
      "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"};
  std::string previous;
  for (const char *access : {"@public", "@protected", "@private", "@package"}) {
    const std::string source =
        "typedef __objc3_optional(id) Strong; "
        "typedef __objc3_optional(id __weak) Weak; "
        "struct Record { int marker; Strong value; }; "
        "__attribute__((objc_root_class)) @interface Heap { Class isa; " +
        std::string(access) + " Strong values[2]; Weak weak; "
        "id __strong nativeStrong; id __weak nativeWeak; } "
        "@property(nonatomic) Strong value; "
        "@property(nonatomic) struct Record record; @end";
    auto ast = clang::tooling::buildASTFromCodeWithArgs(source, args, "heap.m");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.heap", "test");
    for (const auto &diagnostic : emitted.diagnostics)
      std::fprintf(stderr, "%s: %s\n", diagnostic.code.c_str(), diagnostic.message.c_str());
    assert(emitted.success && emitted.identity.semantic_fingerprint != previous);
    assert(emitted.source_bytes.find(access) != std::string::npos);
    assert(emitted.source_bytes.find("values[2]") != std::string::npos);
    assert(emitted.source_bytes.find("__weak id nativeWeak") != std::string::npos);
    assert(emitted.source_bytes.find("__strong id nativeStrong") != std::string::npos);
    previous = emitted.identity.semantic_fingerprint;
    auto round = clang::tooling::buildASTFromCodeWithArgs(emitted.source_bytes, args, "round.m");
    assert(round && !round->getDiagnostics().hasErrorOccurred());
    auto regenerated = EmitSemanticInterface(round->getASTContext(), "fixture.heap", "test");
    assert(regenerated.success && regenerated.source_bytes == emitted.source_bytes);
    assert(regenerated.identity.semantic_fingerprint == previous);
  }
}

void TestBoundedCxxSemanticInterfaces() {
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
            "typedef __objc3_optional(id) Strong; "
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
       {"namespace nested { int api(); }",
        "template<class T> struct Generic { T value; };",
        "struct Base { int value; }; struct Derived : Base { int second; };",
        "struct Special { Special(); int value; };",
        "struct Method { int value; int get() const; };",
        "class Hidden { int value; };", "struct Default { int value=42; };",
        "struct Nested { struct Inner { int value; }; Inner child; };",
        "int api(int argument=42);", "inline int api() { return 42; }",
        "extern constexpr int exported=42;", "extern const int exported=42;",
        "constexpr int evaluated() { return 42; }",
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

}  // namespace

int main() {
  TestOrdinaryCDeclarationsRoundTrip();
  TestUnmodeledAttributesFailClosed();
  TestObjectiveCContainerRoundTrips();
  TestTypedEffectSemanticInterfacesRoundTrip();
  TestManagedReturnOwnershipRoundTrips();
  TestDirectMethodIdentityRoundTrip();
  TestManagedIvarAndPropertyRoundTrip();
  TestBoundedCxxSemanticInterfaces();
  return 0;
}
