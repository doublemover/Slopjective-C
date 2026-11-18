#include "clang_foundation/meta_compilation.h"
#include "clang_foundation/compile_session.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/ObjC3Borrowed.h"
#include "clang/AST/ObjC3Isolation.h"
#include "clang/AST/ObjC3ResourceTypes.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Sema/ObjC3Metaprogramming.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaConsumer.h"
#include "clang/Sema/SemaObjC.h"
#include <algorithm>
namespace objc3::clang_foundation {
namespace {
bool MatchesExecutor(const clang::Decl *D,std::string_view contract) {
  const bool actor=clang::isObjC3ActorIsolated(D);
  const auto *executor=clang::getObjC3Executor(D);
  if(contract=="none")return !actor && !executor;
  if(contract=="actor")return actor && !executor;
  const bool requires_actor=contract.starts_with("actor@");
  if(requires_actor)contract.remove_prefix(6);
  if(actor!=requires_actor || !executor)return false;
  return executor->getExecutor()==llvm::StringRef(contract.data(),contract.size());
}
class MetaConsumer final : public clang::SemaConsumer {
  const CompileRequest &request_;
  MetaCompilation &state_;
  clang::Sema *sema_=nullptr;
  struct Request {clang::Decl *target;clang::ObjCInterfaceDecl *owner;
    MetaEntryKind kind;std::string name;clang::SourceRange range;};
  static std::string requestIdentity(const Request &R) {
    auto result=std::to_string(unsigned(R.kind))+":"+R.name;
    if(R.kind==MetaEntryKind::PropertyBehavior)result+=":"+llvm::cast<clang::NamedDecl>(R.target)->getNameAsString();
    return result;
  }
  std::vector<Request> requests_;
  std::set<std::string> injected_;
  std::string key(clang::ObjCInterfaceDecl *D) const {
    auto &SM=sema_->getSourceManager();
    return SM.getFilename(SM.getExpansionLoc(D->getLocation())).str()+"::"+D->getNameAsString();
  }
  bool fail(clang::SourceLocation loc,const std::string &message) {
    state_.failed=true;if(state_.error.empty())state_.error=message;
    sema_->Diag(loc,sema_->getDiagnostics().getCustomDiagID(
        clang::DiagnosticsEngine::Error,"Objective-C 3 metaprogramming: %0"))<<message;
    return false;
  }
  bool attribute(clang::Decl *D,clang::ObjC3MetaAttribute kind,
                 llvm::StringRef name,clang::SourceRange range) {
    if(kind==clang::ObjC3MetaAttribute::Entrypoint) {
      for(const auto *A:D->specific_attrs<clang::AnnotateAttr>())
        if(A->getAnnotation()=="objc3.meta.entry.v1")return true;
      D->addAttr(clang::AnnotateAttr::CreateImplicit(sema_->Context,
          "objc3.meta.entry.v1",nullptr,0,range));return true;
    }
    auto *owner=llvm::dyn_cast<clang::ObjCInterfaceDecl>(D);
    if(!owner)owner=llvm::dyn_cast<clang::ObjCInterfaceDecl>(D->getDeclContext());
    if(!owner)return fail(range.getBegin(),"expansion requires a concrete class interface");
    MetaEntryKind entryKind=kind==clang::ObjC3MetaAttribute::Derive?MetaEntryKind::Derive:
        kind==clang::ObjC3MetaAttribute::Expand?MetaEntryKind::Macro:MetaEntryKind::PropertyBehavior;
    const MetaPackage *package=nullptr;std::string error;
    auto *entry=state_.packages.Resolve(name.str(),entryKind,&package,error);
    if(!entry)return fail(range.getBegin(),error);
    requests_.push_back({D,owner,entryKind,name.str(),range});
    if(!state_.collecting) {
      if(!state_.classes.contains(key(owner)))
        return fail(range.getBegin(),"class expansion differs from the collected input");
      auto provenance="objc3.meta.expanded.v1:"+package->id+":"+package->version+":"+
          package->fingerprint+":"+std::to_string(unsigned(entryKind))+":"+entry->name;
      D->addAttr(clang::AnnotateAttr::CreateImplicit(sema_->Context,provenance,nullptr,0,range));
    }
    return true;
  }
  static MetaMember member(const clang::NamedDecl *D,clang::QualType type,
                           const clang::ASTContext &context) {
    MetaMember M;M.name=D->getNameAsString();
    clang::PrintingPolicy policy(context.getLangOpts());policy.SuppressScope=false;
    M.type=type.getAsString(policy);
    M.kind=type->isIntegerType()||type->isEnumeralType()?MetaMemberKind::Integer:
        type->isRealFloatingType()?MetaMemberKind::Floating:
        type->isObjCObjectPointerType()?MetaMemberKind::Object:
        type->isBlockPointerType()?MetaMemberKind::Block:
        type->isPointerType()?MetaMemberKind::Pointer:MetaMemberKind::Record;
    M.borrowed=D->hasAttr<clang::ObjCReturnsBorrowedAttr>() ||
        clang::containsObjC3BorrowedType(context,type);
    M.resource=D->hasAttr<clang::ObjCResourceAttr>() ||
        bool(clang::getObjC3ResourceTypeContract(context,type).Cleanup);
    M.weak=type.getObjCLifetime()==clang::Qualifiers::OCL_Weak;
    return M;
  }
  static MetaClass model(clang::ObjCInterfaceDecl *D,const clang::ASTContext &context) {
    MetaClass target;target.name=D->getNameAsString();
    if(auto *super=D->getSuperClass())target.superclass=super->getNameAsString();
    auto authored=[&](const clang::Decl *member) {
      auto &SM=context.getSourceManager();
      return !SM.getBufferName(SM.getSpellingLoc(member->getLocation())).contains(".objc3-meta-");
    };
    for(auto *P:D->properties())if(authored(P)) {
      auto M=member(P,P->getType(),context);M.writable=!P->isReadOnly();
      M.getter=P->getGetterName().getAsString();M.setter=P->getSetterName().getAsString();
      M.atomic=P->isAtomic();M.weak|=bool(P->getPropertyAttributes()&clang::ObjCPropertyAttribute::kind_weak);M.is_class=P->isClassProperty();target.properties.push_back(std::move(M));
    }
    for(auto *I:D->ivars())if(!I->isImplicit() && authored(I))target.ivars.push_back(member(I,I->getType(),context));
    return target;
  }
  static std::string identity(const MetaClass &target) {
    std::string out;auto field=[&](const std::string &value){out+=std::to_string(value.size())+":"+value;};
    field(target.name);field(target.superclass);
    auto members=[&](const auto &values) {
      field(std::to_string(values.size()));
      for(const auto &M:values) {
        field(M.name);field(M.type);field(M.getter);field(M.setter);field(std::to_string(unsigned(M.kind)));
        for(bool value:{M.writable,M.borrowed,M.resource,M.weak,M.atomic,M.is_class})out+=value?'1':'0';
      }
    };
    members(target.properties);members(target.ivars);return out;
  }
  void protocols(clang::ObjCInterfaceDecl *D) {
    if(state_.collecting)return;
    auto found=state_.classes.find(key(D));if(found==state_.classes.end())return;
    llvm::SmallVector<clang::ObjCProtocolDecl *,8> protocols(D->protocol_begin(),D->protocol_end());
    llvm::SmallVector<clang::SourceLocation,8> locations;
    for(auto *P:protocols)locations.push_back(P->getLocation());
    for(const auto &name:found->second.protocols) {
      auto *P=sema_->ObjC().LookupProtocol(&sema_->Context.Idents.get(name),D->getLocation());
      if(!P){fail(D->getLocation(),"required protocol '"+name+"' is not declared");continue;}
      if(std::find(protocols.begin(),protocols.end(),P)==protocols.end()) {
        protocols.push_back(P);locations.push_back(D->getLocation());
      }
    }
    D->setProtocolList(protocols.data(),protocols.size(),locations.data(),sema_->Context);
  }
  std::string members(clang::ObjCContainerDecl *container) {
    if(state_.collecting)return {};
    bool implementation=llvm::isa<clang::ObjCImplementationDecl>(container);
    auto *D=llvm::dyn_cast<clang::ObjCInterfaceDecl>(container);
    if(auto *I=llvm::dyn_cast<clang::ObjCImplementationDecl>(container))D=I->getClassInterface();
    if(!D)return {};
    auto K=key(D);auto found=state_.classes.find(K);if(found==state_.classes.end())return {};
    if(!injected_.insert(K+(implementation?":implementation":":interface")).second) {
      fail(container->getLocation(),"duplicate class expansion");return {};
    }
    return implementation?found->second.implementations:found->second.declarations;
  }
public:
  explicit MetaConsumer(const CompileRequest &request):request_(request),state_(*request.meta_compilation) {}
  void InitializeSema(clang::Sema &S) override {
    sema_=&S;clang::ObjC3MetaCallbacks callbacks;callbacks.Context=this;
    callbacks.Attribute=[](void *p,clang::Sema &,clang::Decl *D,clang::ObjC3MetaAttribute kind,
        llvm::StringRef name,clang::SourceRange range){return static_cast<MetaConsumer *>(p)->attribute(D,kind,name,range);};
    callbacks.Interface=[](void *p,clang::Sema &,clang::ObjCInterfaceDecl *D){static_cast<MetaConsumer *>(p)->protocols(D);};
    callbacks.Members=[](void *p,clang::Sema &,clang::ObjCContainerDecl *D){return static_cast<MetaConsumer *>(p)->members(D);};
    clang::setObjC3MetaCallbacks(S,callbacks);
  }
  void ForgetSema() override {if(sema_)clang::clearObjC3MetaCallbacks(*sema_);sema_=nullptr;}
  void HandleTranslationUnit(clang::ASTContext &context) override {
    auto &source=context.getSourceManager();
    std::stable_sort(requests_.begin(),requests_.end(),[&](const auto &L,const auto &R){
      return source.isBeforeInTranslationUnit(L.range.getBegin(),R.range.getBegin());
    });

    if(!state_.collecting) {
      std::map<std::string,std::vector<std::string>> actual;
      for(const auto &R:requests_)actual[key(R.owner)].push_back(requestIdentity(R));
      for(const auto &[K,C]:state_.classes)if(actual[K]!=C.request_keys)
        fail(clang::SourceLocation(),"expansion requests changed between collection and expansion");
      std::set<clang::ObjCInterfaceDecl *> checked;
      auto &SM=context.getSourceManager();
      for(const auto &R:requests_) {
        if(!checked.insert(R.owner).second)continue;
        auto expansion=state_.classes.find(key(R.owner));
        if(expansion==state_.classes.end() || expansion->second.input_model!=identity(model(R.owner,context)))
          fail(R.owner->getLocation(),"class query inputs changed between collection and expansion");
        std::map<std::string,std::pair<clang::ObjCMethodDecl *,bool>> methods;
        for(auto *M:R.owner->methods()) {
          if(M->isImplicit())continue;
          auto selector=std::string(M->isInstanceMethod()?"-":"+")+M->getSelector().getAsString();
          auto spelling=SM.getSpellingLoc(M->getLocation());
          bool generated=SM.getBufferName(spelling).contains(".objc3-meta-");
          auto [position,inserted]=methods.emplace(selector,std::make_pair(M,generated));
          if(!inserted && (generated||position->second.second))
            fail(M->getLocation(),"generated method conflicts with another declaration: "+selector);
        }
      }
      for(const auto &R:requests_) {
        if(R.kind!=MetaEntryKind::PropertyBehavior)continue;
        auto *P=llvm::cast<clang::ObjCPropertyDecl>(R.target);
        std::string error;const MetaPackage *package=nullptr;
        auto *entry=state_.packages.Resolve(R.name,R.kind,&package,error);
        if(!entry){fail(R.range.getBegin(),error);continue;}
        auto check=[&](clang::ObjCMethodDecl *method) {
          if(method && !MatchesExecutor(method,entry->executor))
            return fail(R.range.getBegin(),"property behavior accessor executor differs from its declared contract: "+entry->executor);
          if(!method || method->isDirectMethod()!=(entry->dispatch=="direct"))
            return fail(R.range.getBegin(),"property behavior dispatch differs from its declared contract");
          if(auto *implementation=R.owner->getImplementation()) {
            auto *body=method->isInstanceMethod()?implementation->getInstanceMethod(method->getSelector()):
                implementation->getClassMethod(method->getSelector());
            if(!body || !body->hasBody())return fail(R.range.getBegin(),"property behavior did not implement its accessor");
            if(body->isDirectMethod()!=(entry->dispatch=="direct") || !MatchesExecutor(body,entry->executor))
              return fail(R.range.getBegin(),"property behavior implementation differs from its declared dispatch/executor contract");
          }
          return true;
        };
        check(P->getGetterMethodDecl());if(!P->isReadOnly())check(P->getSetterMethodDecl());
      }
      return;
    }
    std::set<std::string> seen;
    MetaExpansionContext options{bool(context.getLangOpts().CPlusPlus),
        bool(context.getLangOpts().ObjCAutoRefCount),request_.profile!=SemanticProfile::Compatibility};
    for(const auto &R:requests_) {
      auto *D=R.owner;
      if(!D->isThisDeclarationADefinition()) {fail(R.range.getBegin(),"expansion requires the annotated class definition");continue;}
      auto K=key(D);auto unique=K+":"+std::to_string(unsigned(R.kind))+":"+R.name;
      if(R.kind==MetaEntryKind::PropertyBehavior)unique+=":"+llvm::cast<clang::NamedDecl>(R.target)->getNameAsString();
      if(!seen.insert(unique).second){fail(R.range.getBegin(),"duplicate expansion request '"+R.name+"'");continue;}
      auto target=model(D,context);
      const auto input_model=identity(target);
      if(R.kind==MetaEntryKind::PropertyBehavior) {
        auto property=llvm::cast<clang::ObjCPropertyDecl>(R.target)->getNameAsString();
        std::erase_if(target.properties,[&](const auto &P){return P.name!=property;});
        target.ivars.clear();
      }
      std::string error;const MetaPackage *package=nullptr;
      auto *entry=state_.packages.Resolve(R.name,R.kind,&package,error);
      if(!entry){fail(R.range.getBegin(),error);continue;}
      // Generated accessors use ordinary native actor/executor lowering. The
      // final pass validates their effective affinity, including inherited
      // class contracts and explicit nonisolated declarations, on both owners.
      auto expansion=ExpandMetaProgram(*package,*entry,target,options,error);
      if(!expansion){fail(R.range.getBegin(),error);continue;}
      auto &out=state_.classes[K];out.input_model=input_model;out.request_keys.push_back(requestIdentity(R));
      if(out.declarations.size()+out.implementations.size()+expansion->declarations.size()+
          expansion->implementations.size()>1024*1024) {
        fail(R.range.getBegin(),"combined class expansion exceeds one MiB");continue;
      }
      out.declarations+=expansion->declarations+"\n";
      out.implementations+=expansion->implementations+"\n";
      for(auto &P:expansion->protocols)
        if(std::find(out.protocols.begin(),out.protocols.end(),P)==out.protocols.end())out.protocols.push_back(P);
      out.entries.push_back(std::move(*expansion));
    }
  }
};
}
std::unique_ptr<clang::ASTConsumer> MakeMetaConsumer(const CompileRequest &request) {
  return std::make_unique<MetaConsumer>(request);
}
} // namespace objc3::clang_foundation
