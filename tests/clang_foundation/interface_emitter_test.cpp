#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"

#include <cassert>
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
      "@interface Widget\n- (int)value;\n@end\n",
      {"-x", "objective-c", "-std=gnu11",
       "-fobjc-runtime=gnustep-2.0", "--target=x86_64-unknown-linux-gnu"},
      "objc_fixture.m");
  assert(ast && !ast->getDiagnostics().hasErrorOccurred());

  auto emitted = EmitSemanticInterface(ast->getASTContext(), "fixture.widget",
                                       "test-package");
  assert(emitted.success);
  assert(emitted.source_bytes.find("@interface Widget") != std::string::npos);
  assert(emitted.source_bytes.find("- (int)value") != std::string::npos);
}

}  // namespace

int main() {
  TestOrdinaryCDeclarationsRoundTrip();
  TestUnmodeledAttributesFailClosed();
  TestObjectiveCContainerRoundTrips();
  return 0;
}
