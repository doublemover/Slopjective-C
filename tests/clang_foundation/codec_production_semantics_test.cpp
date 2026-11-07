#include "CodecTarget.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/module_names.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Error.h"
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;

int main() {
  for (bool cxx : {false, true}) {
    const std::string source = cxx ? R"(
      namespace Catalog {
        template<class T=int>struct Opaque;
        template<class T=int>extern T value;
        template<class T=int>extern thread_local T local;
        Opaque<> *pointer();
      }
    )" : "inline int catalog_answer(void){return 42;}\n";
    auto ast=clang::tooling::buildASTFromCodeWithArgs(source,
        cxx?std::vector<std::string>{"-xc++","-std=c++23"}:
            std::vector<std::string>{"-xc","-std=c17"},"catalog-source");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
        "fixture.catalog","test",{}));
    auto artifact=EmitSemanticInterface(ast->getASTContext(),"fixture.catalog","test");
    assert(artifact.success);
    const auto capabilities=SupportedSemanticInterfaceCapabilities();
    auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,capabilities);
    assert(decoded);
    const std::string expected=cxx?kCxxIncompleteTemplateSemantic:"objc3.c-inline-definitions.v1";
    assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),expected)!=decoded->required_semantics.end());
    auto bytes=objc3::fuzz::Pack(artifact.source_bytes,artifact.sidecar_json);assert(!bytes.empty());
    std::string first;
    for(unsigned replay=0;replay!=20;++replay) {
      std::string observed;
      assert(objc3::fuzz::Codec(bytes.data(),bytes.size(),&observed)==objc3::fuzz::Outcome::Accepted);
      if(!replay)first=observed;else assert(observed==first);
    }
    auto changed=objc3::fuzz::Pack(artifact.source_bytes+" ",artifact.sidecar_json);
    assert(objc3::fuzz::Codec(changed.data(),changed.size())==objc3::fuzz::Outcome::SemanticRejected);
    auto restricted=std::vector<llvm::StringRef>(capabilities.begin(),capabilities.end());
    restricted.push_back("objc3.unknown.catalog.v1");
    decoded->required_semantics.push_back("objc3.unknown.catalog.v1");
    auto unknown=EncodeSemanticInterfaceSidecar(*decoded,restricted);assert(unknown);
    auto unsupported=objc3::fuzz::Pack(artifact.source_bytes,*unknown);
    assert(objc3::fuzz::Codec(unsupported.data(),unsupported.size())==objc3::fuzz::Outcome::SemanticRejected);
  }
}
