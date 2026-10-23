#include "clang_foundation/interface_codec.h"

#include <cassert>
#include <string>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"

using namespace objc3::clang_foundation;

namespace {
InterfaceModuleIdentity Module(std::string name, std::string fingerprint) {
  return {
      std::move(name),    std::move(fingerprint), "x86_64-unknown-linux-gnu",
      "objc3-runtime-v1", "objc3-abi-v1",         "c18-r1",
      "canonical-c18",    "package-main"};
}

bool Fails(llvm::Expected<SemanticInterfaceSidecar> result) {
  if (result)
    return false;
  llvm::consumeError(result.takeError());
  return true;
}

std::string ReplaceOnce(std::string text, const std::string &from,
                        const std::string &to) {
  const auto position = text.find(from);
  assert(position != std::string::npos);
  text.replace(position, from.size(), to);
  return text;
}
} // namespace

int main() {
  const llvm::StringRef supported[] = {"effects.async.v1", "ownership.v1",
                                       "objc3.async-entry-abi.v1"};
  const llvm::StringRef source = "int answer(void);\n";
  SemanticInterfaceSidecar original;
  original.module = Module("A", "sha256:semantic-A");
  original.canonical_source_sha256 = ComputeInterfaceSourceSha256(source);
  original.required_semantics = {"ownership.v1", "effects.async.v1",
                                 "objc3.async-entry-abi.v1"};
  InterfaceDeclarationIdentity function{
      "c:@F@answer", "function", "A::answer", "int ()", {}};
  function.effects.is_async = true;
  function.effects.async_abi_identity = "v1:4:type11:cxx-task-v1";
  function.effects.throws_kind = "none";
  function.effects.ownership_identity = "borrowed-param-0";
  InterfaceDeclarationIdentity global{
      "c:@answerCount", "global", "A::answerCount", "unsigned long", {}};
  original.declarations = {global, function};
  original.dependencies = {{Module("Base", "sha256:base"), true, true}};

  auto encoded = EncodeSemanticInterfaceSidecar(original, supported);
  assert(encoded);
  assert(encoded->find("c:@F@answer") < encoded->find("c:@answerCount"));
  assert(encoded->find("effects.async.v1") < encoded->find("ownership.v1"));
  auto decoded = DecodeSemanticInterfaceSidecar(*encoded, source, supported);
  assert(decoded);
  assert(decoded->schema_major == original.schema_major);
  assert(decoded->module.canonical_name == original.module.canonical_name);
  assert(decoded->module.semantic_fingerprint ==
         original.module.semantic_fingerprint);
  assert(decoded->module.target_triple == original.module.target_triple);
  assert(decoded->module.runtime_abi == original.module.runtime_abi);
  assert(decoded->module.extension_abi == original.module.extension_abi);
  assert(decoded->module.language_revision ==
         original.module.language_revision);
  assert(decoded->module.language_profile == original.module.language_profile);
  assert(decoded->module.package_identity == original.module.package_identity);
  assert(decoded->canonical_source_sha256 == original.canonical_source_sha256);
  assert((decoded->required_semantics ==
          std::vector<std::string>{
              "effects.async.v1", "objc3.async-entry-abi.v1", "ownership.v1"}));
  assert(decoded->declarations.size() == 2);
  assert(decoded->declarations[0].stable_id == "c:@F@answer");
  assert(decoded->declarations[0].effects.is_async);
  assert(decoded->declarations[0].effects.async_abi_identity ==
         function.effects.async_abi_identity);
  assert(decoded->declarations[0].effects.ownership_identity ==
         "borrowed-param-0");
  assert(decoded->declarations[1].stable_id == "c:@answerCount");
  assert(decoded->dependencies.size() == 1);
  assert(decoded->dependencies[0].reexported &&
         decoded->dependencies[0].public_import);
  auto reencoded = EncodeSemanticInterfaceSidecar(*decoded, supported);
  assert(reencoded && *reencoded == *encoded);

  auto permuted = original;
  std::reverse(permuted.declarations.begin(), permuted.declarations.end());
  std::reverse(permuted.required_semantics.begin(),
               permuted.required_semantics.end());
  assert(llvm::cantFail(EncodeSemanticInterfaceSidecar(permuted, supported)) ==
         *encoded);

  const std::vector<llvm::StringRef> no_supported_semantics;
  assert(llvm::cantFail(EncodeSemanticInterfaceSidecar(original, supported))
             .size() > 0);
  auto unknown_required =
      ReplaceOnce(*encoded, "ownership.v1", "future.required.v9");
  assert(Fails(
      DecodeSemanticInterfaceSidecar(unknown_required, source, supported)));
  assert(Fails(DecodeSemanticInterfaceSidecar(*encoded, "int answer(void);\r\n",
                                              supported)));
  assert(Fails(DecodeSemanticInterfaceSidecar(
      *encoded, llvm::StringRef("\xff", 1), supported)));
  assert(Fails(DecodeSemanticInterfaceSidecar(*encoded + " ", source,
                                              no_supported_semantics)));
  auto wrong_type = ReplaceOnce(*encoded, "\"async\":true", "\"async\":1");
  assert(Fails(DecodeSemanticInterfaceSidecar(wrong_type, source, supported)));
  auto unknown_field = ReplaceOnce(
      *encoded, "\"schema_id\":", "\"future_field\":0,\"schema_id\":");
  assert(
      Fails(DecodeSemanticInterfaceSidecar(unknown_field, source, supported)));
  auto duplicate_key = ReplaceOnce(
      *encoded, "\"schema_id\":\"objc3c.semantic-interface.v2\"",
      "\"schema_id\":\"objc3c.semantic-interface.v2\",\"schema_id\":\"bad\"");
  assert(
      Fails(DecodeSemanticInterfaceSidecar(duplicate_key, source, supported)));
  auto future_schema =
      ReplaceOnce(*encoded, "\"schema_major\":2", "\"schema_major\":3");
  assert(
      Fails(DecodeSemanticInterfaceSidecar(future_schema, source, supported)));
  auto retired_schema =
      ReplaceOnce(*encoded, "\"schema_minor\":1", "\"schema_minor\":0");
  assert(
      Fails(DecodeSemanticInterfaceSidecar(retired_schema, source, supported)));
  auto missing_abi = ReplaceOnce(
      *encoded, "\"async_abi_identity\":\"v1:4:type11:cxx-task-v1\"",
      "\"async_abi_identity\":\"\"");
  assert(Fails(DecodeSemanticInterfaceSidecar(missing_abi, source, supported)));
  auto unknown_abi = ReplaceOnce(*encoded, "cxx-task-v1", "cxx-task-v9");
  assert(Fails(DecodeSemanticInterfaceSidecar(unknown_abi, source, supported)));
  auto duplicate_id = ReplaceOnce(*encoded, "\"stable_id\":\"c:@answerCount\"",
                                  "\"stable_id\":\"c:@F@answer\"");
  assert(
      Fails(DecodeSemanticInterfaceSidecar(duplicate_id, source, supported)));
  auto bad_digest = ReplaceOnce(*encoded, original.canonical_source_sha256,
                                std::string(64, '0'));
  assert(Fails(DecodeSemanticInterfaceSidecar(bad_digest, source, supported)));
  assert(Fails(DecodeSemanticInterfaceSidecar(
      std::string(33u * 1024u * 1024u, ' '), source, supported)));

  SemanticInterfaceSidecar duplicate = original;
  duplicate.declarations.push_back(function);
  assert(!EncodeSemanticInterfaceSidecar(duplicate, supported));
  auto invalid_utf8 = original;
  invalid_utf8.module.canonical_name = std::string("\xff", 1);
  assert(!EncodeSemanticInterfaceSidecar(invalid_utf8, supported));
  auto unsupported =
      EncodeSemanticInterfaceSidecar(original, no_supported_semantics);
  assert(!unsupported);
}
