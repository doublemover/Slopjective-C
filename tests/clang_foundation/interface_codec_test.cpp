#include "clang_foundation/interface_codec.h"

#include <cassert>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/FormatVariadic.h"

using namespace objc3::clang_foundation;

namespace {
InterfaceModuleIdentity Module(std::string name, std::string fingerprint) {
  return {
      std::move(name),    std::move(fingerprint), "x86_64-unknown-linux-gnu",
      "objc3-runtime-v1", "objc3-abi-v1",         "c18-r1",
      "canonical-c18",    "package-main"};
}

template <class T> bool Fails(llvm::Expected<T> result) {
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
  const auto production=SupportedSemanticInterfaceCapabilities();
  std::set<std::string> capabilities;
  for(auto semantic:production)assert(capabilities.insert(semantic.str()).second);
  assert(capabilities.count(kCxxIncompleteTemplateSemantic));
  assert(capabilities.count(kNativeSubobjectSemantic));
  assert(capabilities.count(kCxxUnnamedTemplateProjectionSemantic));
  assert(capabilities.count("objc3.c-inline-definitions.v1"));
  assert(!capabilities.count("objc3.borrowed-result-proofs.v8"));
  assert(!capabilities.count("objc3.borrowed-result-proofs.v9"));
  assert(!capabilities.count("objc3.borrowed-result-proofs.v10"));
  assert(!capabilities.count("objc3.borrowed-result-proofs.v11"));
  assert(!capabilities.count("objc3.borrowed-result-proofs.v12"));
  assert(capabilities.count(kManualFamilyCallProofSemantic));
  assert(capabilities.count(kCxxFriendSemantic));
  assert(capabilities.count(kCxxConstantEvaluationControlSemantic));
  assert(capabilities.count(kNativeControlFlowSemantic));
  assert(capabilities.count(kCOverloadableSemantic));
  assert(capabilities.count(kActorReceiverBindingSemantic));
  assert(capabilities.count(kNativeFunctionPolicySemantic));
  assert(!capabilities.count("objc3.cxx-friends.v1"));
  assert(!capabilities.count("objc3.cxx-friends.v2"));
  assert(capabilities.count("objc3.cxx-class-templates.v4"));
  assert(!capabilities.count("objc3.cxx-class-templates.v1"));
  assert(!capabilities.count("objc3.cxx-class-templates.v2"));
  assert(!capabilities.count("objc3.cxx-class-templates.v3"));
  assert(capabilities.count("objc3.cxx-variable-templates.v2"));
  assert(!capabilities.count("objc3.cxx-variable-templates.v1"));
  assert(capabilities.count(kCxxExplicitSpecifierSemantic));
  assert(capabilities.count(kCxxDeductionGuideSemantic));
  assert(capabilities.count(kCxxConceptSemantic));
  assert(capabilities.count(kCxxAliasTemplateSemantic));
  assert(!capabilities.count("objc3.cxx-concepts.v1"));
  assert(!capabilities.count("objc3.cxx-alias-templates.v1"));
  assert(capabilities.count(kCxxOutOfLineDefinitionSemantic));
  assert(!capabilities.count("objc3.native-declaration-attributes.v3"));
  assert(capabilities.count("objc3.native-declaration-attributes.v6"));
  assert(!capabilities.count("objc3.native-declaration-attributes.v4"));
  assert(!capabilities.count("objc3.native-declaration-attributes.v5"));
  assert(!capabilities.count("objc3.cxx-inline-closure-objects.v1"));
  assert(!capabilities.count("objc3.cxx-inline-closure-objects.v2"));
  assert(!capabilities.count("objc3.native-subobject-cleanup.v2"));
  assert(!capabilities.count("objc3.native-subobject-cleanup.v3"));
  assert(!capabilities.count("objc3.native-subobject-cleanup.v4"));
  assert(!capabilities.count("objc3.cxx-block-expressions.v1"));
  assert(!capabilities.count("objc3.cxx-block-expressions.v2"));
  assert(!capabilities.count("objc3.automatic-owner-unwind.v1"));
  assert(!capabilities.count("objc3.cxx-unnamed-template-projections.v1"));
  assert(!capabilities.count("objc3.cxx-incomplete-template-declarations.v1"));
  assert(!capabilities.count("objc3.native-emission-policies.v2"));
  assert(capabilities.count(kNativeObjectExpressionSemantic));
  assert(!capabilities.count("objc3.native-object-expressions.v1"));
  {
    InterfaceDeclarationIdentity guide,pattern;
    guide.stable_id="guide:int";guide.kind="CXXDeductionGuide";
    guide.canonical_type_identity="auto (int) -> Box<long>;cxx-deduction-guides:v1;owner=Box;explicit=implicit;";
    pattern.stable_id="guide:template";pattern.kind="FunctionTemplate";
    pattern.canonical_type_identity="template Box<T>;cxx-deduction-guides:v1;owner=Box;explicit=conditional;";
    assert(CanShareInterfaceFunctionLookup(guide,pattern,true));
    assert(CanShareInterfaceFunctionLookup(pattern,guide,true));
    assert(!CanShareInterfaceFunctionLookup(guide,pattern,false));
    assert(!CanShareInterfaceFunctionLookup(guide,guide,true));
    auto invalid=guide;invalid.canonical_type_identity="auto (int) -> Box<long>";
    assert(!CanShareInterfaceFunctionLookup(invalid,pattern,true));
    invalid=guide;invalid.native_symbol_name="invalid_guide_entry";
    assert(!CanShareInterfaceFunctionLookup(invalid,pattern,true));
    invalid=guide;invalid.kind="Namespace";
    assert(!CanShareInterfaceFunctionLookup(invalid,pattern,true));
  }
  const llvm::StringRef supported[] = {"effects.async.v1", "ownership.v1",
                                       "objc3.async-entry-abi.v5"};
  const llvm::StringRef source = "int answer(void);\n";
  SemanticInterfaceSidecar original;
  original.module = Module("A", "sha256:semantic-A");
  original.canonical_source_sha256 = ComputeInterfaceSourceSha256(source);
  original.required_semantics = {"ownership.v1", "effects.async.v1",
                                 "objc3.async-entry-abi.v5"};
  InterfaceDeclarationIdentity function;
  function.stable_id="c:@F@answer";function.kind="function";
  function.qualified_name="A::answer";function.canonical_type_identity="int ()";
  function.export_root = "A::answer";
  function.effects.is_async = true;
  function.effects.async_abi_identity = "v1:4:type11:cxx-task-v1";
  function.effects.throws_kind = "none";
  function.effects.ownership_identity = "borrowed-param-0";
  InterfaceDeclarationIdentity global;
  global.stable_id="c:@answerCount";global.kind="global";
  global.qualified_name="A::answerCount";global.canonical_type_identity="unsigned long";
  global.export_root = "A::answerCount";
  original.declarations = {global, function};
  original.dependencies = {{Module("Base", "sha256:base"), true, true}};

  auto encoded = EncodeSemanticInterfaceSidecar(original, supported);
  assert(encoded);
  assert(encoded->find("c:@F@answer") < encoded->find("c:@answerCount"));
  assert(encoded->find("effects.async.v1") < encoded->find("ownership.v1"));
  auto decoded = DecodeSemanticInterfaceSidecar(*encoded, source, supported);
  assert(decoded);
  assert(decoded->schema_major == original.schema_major);
  // Contract-looking text inside an ordinary type argument remains data.
  // Only a framed outer binding can claim an external symbol contract.
  {
    auto data=*decoded;
    data.declarations.front().canonical_type_identity="int (Name<\";native-external-binding:v1;symbol=3:ffi;\">)";
    auto encoded_data=EncodeSemanticInterfaceSidecar(data,supported);assert(encoded_data);
  }
  // Body-only lifetime/proof capabilities cannot be fabricated on namespace
  // declarations, even when their marker and required-semantics list agree.
  for(const auto &[marker,capability]:std::vector<std::pair<std::string,std::string>>{
      {";native-storage-lifetime:v6;",kNativeStorageLifetimeSemantic},
      {";borrowed-result-proofs:v13;",kBorrowedResultProofSemantic},
      {";manual-family-call-proofs:v1;",kManualFamilyCallProofSemantic},
      {";cxx-friends:v3;",kCxxFriendSemantic},
      {";cxx-constant-evaluation-control:v1;",kCxxConstantEvaluationControlSemantic},
      {";native-control-flow-contracts:v1;",kNativeControlFlowSemantic},
      {";c-overloadable-functions:v1;",kCOverloadableSemantic},
      {";actor-receiver-bindings:v1;",kActorReceiverBindingSemantic},
      {";native-function-policies:v1;",kNativeFunctionPolicySemantic},
      {";native-function-effects:v1;",kNativeFunctionEffectSemantic},
      {";native-lifetime-annotations:v1;",kNativeLifetimeAnnotationSemantic},
      {";native-object-expressions:v2;",kNativeObjectExpressionSemantic},
      {";native-external-binding:v1;symbol=3:ffi;",kNativeExternalBindingSemantic},
      {";native-emission-policies:v3;",kNativeEmissionPolicySemantic},
      {";native-thread-safety:v1;",kNativeThreadSafetySemantic},
      {";cxx-virtual-call-expressions:v1;",kCxxVirtualCallSemantic},
      {";native-cxx-async-import:v1;",kNativeCxxAsyncImportSemantic},
      {";cxx-special-member-selection:v1;",kCxxSpecialMemberSelectionSemantic},
      {";cxx-inline-closure-objects:v3;",kCxxInlineClosureObjectSemantic},
      {";cxx-base-adjustment-expressions:v1;",kCxxBaseAdjustmentSemantic},
      {";native-cpu-variant:v1;",kNativeEmissionPolicySemantic}}) {
    auto invalid=*decoded;
    InterfaceDeclarationIdentity space;
    space.stable_id="c:@N@Bad";space.kind="Namespace";
    space.qualified_name="A::Bad";space.canonical_type_identity="namespace";
    space.export_root="A::Bad";
    invalid.declarations.push_back(space);
    auto valid_namespace=EncodeSemanticInterfaceSidecar(invalid,supported);
    assert(valid_namespace);
    invalid.declarations.back().canonical_type_identity+=marker;
    invalid.required_semantics.push_back(capability);
    std::vector<llvm::StringRef> reader(std::begin(supported),std::end(supported));
    reader.push_back(capability);
    assert(Fails(EncodeSemanticInterfaceSidecar(invalid,reader)));
    auto json=llvm::json::parse(*valid_namespace);assert(json);
    json->getAsObject()->getArray("required_semantics")->push_back(capability);
    for(auto &row:*json->getAsObject()->getArray("declarations"))
      if(row.getAsObject()->getString("stable_id")=="c:@N@Bad")
        (*row.getAsObject())["canonical_type_identity"]="namespace"+marker;
    assert(Fails(DecodeSemanticInterfaceSidecar(
        llvm::formatv("{0}",*json).str(),source,reader)));
  }
  // A valid C overload does not authorize capability markers on another kind.
  {
    auto valid=*decoded;valid.declarations.front().kind="Function";
    valid.declarations.front().canonical_type_identity+=";c-overloadable-functions:v1;";
    valid.required_semantics.push_back(kCOverloadableSemantic);
    std::vector<llvm::StringRef> reader(std::begin(supported),std::end(supported));reader.push_back(kCOverloadableSemantic);
    auto good=EncodeSemanticInterfaceSidecar(valid,reader);assert(good);
    for(const char *kind:{"Var","Record","Typedef"}) {
      auto invalid=valid;auto owner=original.declarations.front();
      owner.stable_id="c:@bad-owner";owner.qualified_name=owner.export_root="A::bad_owner";owner.kind=kind;
      owner.canonical_type_identity+=";c-overloadable-functions:v1;";
      invalid.declarations.push_back(owner);assert(Fails(EncodeSemanticInterfaceSidecar(invalid,reader)));
    }
  }
  // Function policy markers remain scoped when a legitimate owner coexists.
  {
    auto valid=*decoded;valid.declarations.front().kind="Function";
    valid.declarations.front().canonical_type_identity+=";native-function-policies:v1;";
    valid.required_semantics.push_back(kNativeFunctionPolicySemantic);
    std::vector<llvm::StringRef> reader(std::begin(supported),std::end(supported));reader.push_back(kNativeFunctionPolicySemantic);
    assert(EncodeSemanticInterfaceSidecar(valid,reader));
    for(const char *kind:{"Namespace","Typedef","Enum","EnumConstant","ObjCProperty","ObjCIvar"}) {
      auto invalid=valid;auto owner=original.declarations.front();
      owner.stable_id="c:@bad-policy-owner";owner.qualified_name=owner.export_root="A::bad_policy_owner";owner.kind=kind;
      owner.canonical_type_identity+=";native-function-policies:v1;";
      invalid.declarations.push_back(owner);assert(Fails(EncodeSemanticInterfaceSidecar(invalid,reader)));
    }
  }
  // One legitimate actor owner must not authorize unrelated marked rows.
  {
    auto valid=*decoded;valid.declarations.front().kind="ObjCInterface";
    valid.declarations.front().canonical_type_identity+=";actor-receiver-bindings:v1;";
    valid.required_semantics.push_back(kActorReceiverBindingSemantic);
    std::vector<llvm::StringRef> reader(std::begin(supported),std::end(supported));reader.push_back(kActorReceiverBindingSemantic);
    auto good=EncodeSemanticInterfaceSidecar(valid,reader);assert(good);
    for(const char *kind:{"Function","Var","Record","Typedef","ObjCProtocol","ObjCCategory"}) {
      auto invalid=valid;auto owner=original.declarations.front();
      owner.stable_id="c:@bad-actor-owner";owner.qualified_name=owner.export_root="A::bad_actor_owner";owner.kind=kind;
      owner.canonical_type_identity+=";actor-receiver-bindings:v1;";
      invalid.declarations.push_back(owner);assert(Fails(EncodeSemanticInterfaceSidecar(invalid,reader)));
    }
  }
  // A legitimate marked function must not hide a forged namespace marker,
  // with or without the corresponding required-semantics declaration.
  for(const auto &[marker,capability]:std::vector<std::pair<std::string,std::string>>{
      {";cxx-virtual-call-expressions:v1;",kCxxVirtualCallSemantic},
      {";native-cxx-async-import:v1;",kNativeCxxAsyncImportSemantic},
      {";cxx-special-member-selection:v1;",kCxxSpecialMemberSelectionSemantic},
      {";cxx-inline-closure-objects:v3;",kCxxInlineClosureObjectSemantic},
      {";cxx-base-adjustment-expressions:v1;",kCxxBaseAdjustmentSemantic}}) {
    auto legitimate=*decoded;legitimate.declarations.front().canonical_type_identity+=marker;
    legitimate.required_semantics.push_back(capability);
    std::vector<llvm::StringRef> reader(std::begin(supported),std::end(supported));reader.push_back(capability);
    auto encoded_legitimate=EncodeSemanticInterfaceSidecar(legitimate,reader);assert(encoded_legitimate);
    InterfaceDeclarationIdentity space;
    space.stable_id="c:@N@Forged";space.kind="Namespace";
    space.qualified_name="A::Forged";space.canonical_type_identity="namespace"+marker;
    space.export_root="A::Forged";
    legitimate.declarations.push_back(space);
    assert(Fails(EncodeSemanticInterfaceSidecar(legitimate,reader)));
    legitimate=*decoded;legitimate.declarations.push_back(space);
    assert(Fails(EncodeSemanticInterfaceSidecar(legitimate,reader)));
    auto clean=*decoded;clean.declarations.front().canonical_type_identity+=marker;
    clean.required_semantics.push_back(capability);space.canonical_type_identity="namespace";
    clean.declarations.push_back(space);
    auto encoded_clean=EncodeSemanticInterfaceSidecar(clean,reader);assert(encoded_clean);
    assert(DecodeSemanticInterfaceSidecar(*encoded_clean,source,reader));
    auto document=llvm::json::parse(*encoded_clean);assert(document);
    for(auto &row:*document->getAsObject()->getArray("declarations"))
      if(row.getAsObject()->getString("stable_id")==space.stable_id)
        (*row.getAsObject())["canonical_type_identity"]="namespace"+marker;
    assert(Fails(DecodeSemanticInterfaceSidecar(llvm::formatv("{0}",*document).str(),source,reader)));
  }
  assert(decoded->schema_minor == 2);
  assert(decoded->declarations[0].export_root == "A::answer");
  auto changed_root = *decoded;
  changed_root.declarations[0].export_root = "Other";
  assert(BuildSemanticFingerprintInput(changed_root) !=
         BuildSemanticFingerprintInput(*decoded));
  auto missing_root_json = llvm::json::parse(*encoded);
  assert(missing_root_json);
  missing_root_json->getAsObject()->getArray("declarations")->front()
      .getAsObject()->erase("export_root");
  auto missing_root = DecodeSemanticInterfaceSidecar(
      llvm::formatv("{0}", *missing_root_json).str(), source, supported);
  assert(!missing_root);
  llvm::consumeError(missing_root.takeError());
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
              "effects.async.v1", "objc3.async-entry-abi.v5", "ownership.v1"}));
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
      ReplaceOnce(*encoded, "\"schema_minor\":2", "\"schema_minor\":1");
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
  assert(Fails(EncodeSemanticInterfaceSidecar(duplicate, supported)));
  auto invalid_utf8 = original;
  invalid_utf8.module.canonical_name = std::string("\xff", 1);
  assert(Fails(EncodeSemanticInterfaceSidecar(invalid_utf8, supported)));
  auto unsupported =
      EncodeSemanticInterfaceSidecar(original, no_supported_semantics);
  assert(!unsupported);
  llvm::consumeError(unsupported.takeError());
}
