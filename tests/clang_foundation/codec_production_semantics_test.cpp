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
  for (unsigned family=0;family!=6;++family) {
    const bool cxx=family&1,blocks=family>=2,producers=family>=4;
    const std::string source = producers ? R"(
      typedef void (^CatalogCallback)(void);
      struct CatalogSlots {CatalogCallback slots[2];};
      CatalogCallback catalog_producer(int) __attribute__((ns_returns_retained));
      CatalogCallback catalog_shared(int);
      int catalog_step(void);void catalog_checkpoint(void) __attribute__((nothrow));
      void catalog_consume(CatalogCallback __attribute__((ns_consumed))) __attribute__((nothrow));
      inline void catalog_production(struct CatalogSlots *store) {
        __objc3_defer {catalog_checkpoint();}
        CatalogCallback local=catalog_producer(1),shared=catalog_shared(2);
        store->slots[catalog_step()]=catalog_producer(3);
        catalog_consume(catalog_producer(4));local();shared();
      }
    )" : blocks ? R"(
      typedef void (^CatalogCallback)(void);
      struct CatalogSlots {CatalogCallback slots[2];};
      int catalog_step(void);void catalog_checkpoint(void) __attribute__((nothrow));
      inline void catalog_mutation(struct CatalogSlots *store,CatalogCallback value) {
        __objc3_defer {catalog_checkpoint();}
        store->slots[catalog_step()]=(catalog_step(),value);
      }
    )" : cxx ? R"(
      namespace Catalog {
        template<class T=int>struct Opaque;
        template<class T>struct Opaque<T*>;
        template<>struct Opaque<char>;
        template<class T=int>extern T value;
        template<class T>extern T *value<T*>;
        template<class T=int>extern thread_local T local;
        template<class T>extern thread_local T *local<T*>;
        Opaque<> *pointer();
      }
    )" : "inline int catalog_answer(void){return 42;}\n";
    std::vector<std::string> args={cxx?(blocks?"-xobjective-c++":"-xc++"):(blocks?"-xobjective-c":"-xc"),cxx?"-std=c++23":"-std=c17"};
    if(blocks)args.insert(args.end(),{"-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system","-fobjc-arc","-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"});
    auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,"catalog-source");
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
        "fixture.catalog","test",{}));
    auto artifact=EmitSemanticInterface(ast->getASTContext(),"fixture.catalog","test");
    assert(artifact.success);
    const auto capabilities=SupportedSemanticInterfaceCapabilities();
    auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,capabilities);
    assert(decoded);
    const std::string expected=producers?kCxxBlockExpressionSemantic:blocks?kAutomaticOwnerUnwindSemantic:cxx?kCxxIncompleteTemplateSemantic:"objc3.c-inline-definitions.v1";
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
