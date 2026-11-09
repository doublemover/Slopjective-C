#pragma once
#include "clang_foundation/interface_contract.h"
#include "clang_foundation/interface_codec.h"
#include "standard_coroutine_types.h"
#include "runtime_interface_types.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/TypeLoc.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/UnifiedSymbolResolution/USRGeneration.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace objc3::clang_foundation {
// System-header status alone is insufficient. The declaration must belong to
// the native standard library and its file must be reached through a public
// C++ standard header. Main-file customizations remain package declarations.
inline bool HasStandardLibraryHeaderAncestor(clang::FileID file,
                                              const clang::SourceManager &source,
                                              bool *over_budget=nullptr,
                                              bool include_runtime_roots=false) {
  unsigned depth=0;
  for(;file.isValid() && depth!=128;++depth) {
    auto location=source.getLocForStartOfFile(file);
    const auto name=llvm::sys::path::filename(source.getFilename(location));
    if(source.isInSystemHeader(location) && IsStandardLibraryHeaderName(name.str()))return true;
    // A pinned runtime header may include C prerequisites before a C++ header
    // reaches them. Include guards retain the first include parent. Bind those
    // prerequisites in both inventories without treating arbitrary SDK-looking
    // files as standard-library declarations.
    if(include_runtime_roots && (name=="objc3_runtime_services.h" ||
        name=="objc3_task.hpp" || name=="objc3_keypath.h")) {
      bool invalid=false;const auto bytes=source.getBufferData(file,&invalid);
      const auto expected=name=="objc3_runtime_services.h"?RuntimeInterfaceHeaderSource():
          name=="objc3_task.hpp"?RuntimeTaskHeaderSource():RuntimeKeyPathHeaderSource();
      if(!invalid && bytes==expected)return true;
    }
    auto included=source.getIncludeLoc(file);
    if(included.isInvalid())break;
    auto next=source.getFileID(source.getSpellingLoc(included));
    if(next==file)break;
    file=next;
  }
  if(depth==128 && file.isValid() && over_budget)*over_budget=true;
  return false;
}
inline bool IsStandardLibraryDeclaration(const clang::NamedDecl *declaration,
                                          const clang::ASTContext &context) {
  if(!declaration || !context.getLangOpts().CPlusPlus ||
      IsStandardCoroutineDeclaration(declaration,context))return false;
  const auto &source=context.getSourceManager();
  auto location=source.getSpellingLoc(declaration->getLocation());
  if(location.isInvalid() || !source.isInSystemHeader(location) ||
      !HasStandardLibraryHeaderAncestor(source.getFileID(location),source))return false;
  bool namespaced=false;
  for(const auto *scope=declaration->getDeclContext();scope;scope=scope->getParent())
    if(const auto *space=llvm::dyn_cast<clang::NamespaceDecl>(scope)) {
      namespaced=true;
      if(space->isStdNamespace() || space->getName().starts_with("__gnu_"))return true;
    }
  // C declarations reexported by <cstdio>/<cstring>/... and global allocation
  // functions from <new> are supplied by the same checked native header set.
  return !namespaced;
}
struct StandardLibraryHeaderManifest {
  std::vector<std::string> headers;
  std::set<std::string> contents;
  bool valid=true;
};
// An emission/import operation observes a stable header prelude. Cache its
// content inventory for that operation, never across AST lifetimes or parser
// operations: ASTContext addresses can be reused and later includes can grow
// the closure. Nested operations own independent caches.
class StandardLibraryHeaderCache {
public:
  inline static thread_local StandardLibraryHeaderCache *active=nullptr;
  std::map<const clang::ASTContext *,StandardLibraryHeaderManifest> manifests;
  std::map<const clang::ASTContext *,const clang::Preprocessor *> preprocessors;
  StandardLibraryHeaderCache(const clang::ASTContext *context=nullptr,
                            const clang::Preprocessor *preprocessor=nullptr):previous(active) {
    active=this;if(context && preprocessor)preprocessors.emplace(context,preprocessor);
  }
  ~StandardLibraryHeaderCache() {active=previous;}
  StandardLibraryHeaderCache(const StandardLibraryHeaderCache &)=delete;
  StandardLibraryHeaderCache &operator=(const StandardLibraryHeaderCache &)=delete;
private:
  StandardLibraryHeaderCache *previous;
};

inline std::string StandardHeaderTokenIdentity(llvm::StringRef text,
                                               const clang::LangOptions &language) {
  // AST printers may insert a space to avoid concatenating unary operators
  // with preceding tokens. Token identity retains literal contents and token
  // boundaries without treating that incidental formatting as a new contract.
  clang::Lexer lexer(clang::SourceLocation::getFromRawEncoding(1),language,
                     text.begin(),text.begin(),text.end());
  std::string result;clang::Token token;
  do {
    lexer.LexFromRawLexer(token);
    if(token.is(clang::tok::eof))break;
    const auto spelling=text.substr(token.getLocation().getRawEncoding()-1,token.getLength());
    result+=std::to_string(token.getKind())+":"+std::to_string(spelling.size())+":"+spelling.str();
  } while(true);
  return result;
}

class StandardHeaderWrittenExceptionSpecs {
  clang::ASTContext &context;
  std::vector<std::pair<clang::FunctionDecl *,clang::QualType>> saved;
  llvm::SmallPtrSet<const clang::Decl *,32> visited;
  void Visit(clang::Decl *declaration) {
    if(!declaration || declaration->isImplicit() || !visited.insert(declaration).second)return;
    for(auto *redecl:declaration->redecls())
      if(redecl!=declaration)Visit(redecl);
    if(auto *friend_declaration=llvm::dyn_cast<clang::FriendDecl>(declaration))
      Visit(friend_declaration->getFriendDecl());
    if(auto *instantiation=llvm::dyn_cast<clang::ExplicitInstantiationDecl>(declaration)) {
      const auto &source=context.getSourceManager();
      auto location=source.getSpellingLoc(instantiation->getLocation());
      if(location.isValid() && source.isInSystemHeader(location) &&
          HasStandardLibraryHeaderAncestor(source.getFileID(location),source))
        Visit(instantiation->getSpecialization());
    }
    if(auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && IsStandardLibraryDeclaration(function,context)) {
      const auto *type=function->getType()->getAs<clang::FunctionProtoType>();
      const auto *source=function->getTypeSourceInfo();
      const auto location=source?source->getTypeLoc().getAs<clang::FunctionProtoTypeLoc>():clang::FunctionProtoTypeLoc();
      if(type && location && type->getExceptionSpecType()!=clang::EST_None &&
          location.getExceptionSpecRange().isInvalid()) {
        auto info=type->getExtProtoInfo();info.ExceptionSpec={};
        saved.emplace_back(function,function->getType());
        function->setType(context.getFunctionType(type->getReturnType(),type->getParamTypes(),info));
      }
    }
    if(auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(declaration)) {
      Visit(pattern->getTemplatedDecl());
      if(auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern);
          record && IsStandardLibraryDeclaration(record,context))
        for(auto *specialization:record->specializations())
          for(auto *member:specialization->decls())
            if(llvm::isa<clang::FriendDecl>(member))Visit(member);
    }
    if(auto *scope=llvm::dyn_cast<clang::DeclContext>(declaration))for(auto *child:scope->decls())Visit(child);
  }
public:
  explicit StandardHeaderWrittenExceptionSpecs(const clang::ASTContext &context)
      :context(const_cast<clang::ASTContext &>(context)) {Visit(context.getTranslationUnitDecl());}
  ~StandardHeaderWrittenExceptionSpecs() {for(auto [function,type]:saved)function->setType(type);}
  StandardHeaderWrittenExceptionSpecs(const StandardHeaderWrittenExceptionSpecs &)=delete;
  StandardHeaderWrittenExceptionSpecs &operator=(const StandardHeaderWrittenExceptionSpecs &)=delete;
};

// Bind macro definitions at the end of each header, before later source or
// headers can change them. Hash one configuration per physical header; that
// lets independent provider closures combine without including header guards.
inline std::string StandardHeaderMacroProfile(clang::Preprocessor &preprocessor,
                                               clang::FileID file,llvm::StringRef bytes) {
  std::set<std::string> names;
  clang::Lexer lexer(clang::SourceLocation::getFromRawEncoding(1),preprocessor.getLangOpts(),
                     bytes.begin(),bytes.begin(),bytes.end());clang::Token token;
  do {
    lexer.LexFromRawLexer(token);if(token.is(clang::tok::eof))break;
    if(!token.is(clang::tok::raw_identifier))continue;
    const auto name=token.getRawIdentifier();
    // Collect query names and possible compiler/caller configuration inputs.
    // Ordinary identifiers have no macro definition to record.
    names.insert(name.str());
  } while(true);
  std::string contents;
  const auto end=preprocessor.getSourceManager().getLocForEndOfFile(file);
  for(const auto &name:names) {
    auto *identifier=preprocessor.getIdentifierInfo(name);
    const auto latest=preprocessor.getMacroDefinition(identifier);
    if(const auto *info=latest.getMacroInfo();info && info->isUsedForHeaderGuard())continue;
    const auto definition=preprocessor.getMacroDefinitionAtLoc(identifier,end);
    const auto *info=definition.getMacroInfo();if(!info || info->isBuiltinMacro())continue;
    const auto location=info->getDefinitionLoc();
    const bool implementation=name.starts_with("_GLIBCXX") || name.starts_with("_LIBCPP") ||
        name.starts_with("__USE_") || name=="_GNU_SOURCE" || name=="_DEFAULT_SOURCE" ||
        name=="_POSIX_C_SOURCE" || name=="_XOPEN_SOURCE" || name=="_FILE_OFFSET_BITS" ||
        name=="_TIME_BITS" || name=="_FORTIFY_SOURCE" || name=="NDEBUG";
    // System headers also publish feature-test and helper macros as outputs.
    // Their presence can grow when independent headers are combined. Bind
    // implementation configuration and every referenced compiler/caller macro,
    // while declaration reuse separately checks native written definitions.
    const auto &source=preprocessor.getSourceManager();
    const bool compiler=location.isValid() && (source.isWrittenInBuiltinFile(location) ||
                                              source.isWrittenInCommandLineFile(location));
    if(!implementation && !compiler && location.isValid() && source.isInSystemHeader(location))continue;
    std::string value=info->isFunctionLike()?"function:":"object:";
    value+=info->isC99Varargs()?"c99:":info->isGNUVarargs()?"gnu:":"fixed:";
    for(const auto *parameter:info->params())value+=parameter->getName().str()+",";
    value+=";";
    for(const auto &part:info->tokens()) {
      const auto spelling=preprocessor.getSpelling(part);
      value+=std::to_string(part.getKind())+":"+std::to_string(spelling.size())+":"+spelling;
    }
    const auto entry=name+"="+value;contents+=std::to_string(entry.size())+":"+entry;
  }
  // Feature-query macros are builtins without replacement tokens. Evaluate
  // the header's referenced names through the pinned compiler's authority.
  // Keep query state independent of template instantiation and AST printing.
  if(names.count("__has_feature") || names.count("__has_extension")) {
    using namespace clang;
    const auto &PP=preprocessor;
    const auto &LangOpts=PP.getLangOpts();
    std::map<std::string,bool> features,extensions;
    auto referenced=[&](const char *name) {
      return names.count(name) || names.count(std::string("__")+name+"__");
    };
#define FEATURE(Name, Predicate) if(referenced(#Name))features[#Name]=bool(Predicate);
#define EXTENSION(Name, Predicate) if(referenced(#Name))extensions[#Name]=bool(Predicate);
#include "clang/Basic/Features.def"
    std::set<std::string> queries;
    for(const auto &[name,value]:features)queries.insert(name);
    for(const auto &[name,value]:extensions)queries.insert(name);
    const bool allow_extensions=PP.getDiagnostics().getExtensionHandlingBehavior()<diag::Severity::Error;
    for(const auto &name:queries) {
      if(names.count("__has_feature"))contents+=";feature:"+name+"="+std::to_string(features[name]);
      if(names.count("__has_extension"))contents+=";extension:"+name+"="+
          std::to_string(features[name] || (allow_extensions && extensions[name]));
    }
  }
  return ComputeInterfaceSourceSha256(contents);
}
// Bind the actual C++ include closure, including target/configuration and C
// headers reached through it. Header order and SDK relocation are irrelevant;
// adding a native header is an explicit dependency change. Header guards can
// make the first include parent differ, so no semantic key uses that parent.
inline StandardLibraryHeaderManifest ReadStandardLibraryHeaderManifest(
    const clang::ASTContext &context) {
  auto *cache=StandardLibraryHeaderCache::active;
  if(cache)if(auto found=cache->manifests.find(&context);found!=cache->manifests.end())return found->second;
  StandardLibraryHeaderManifest result;
  if(!context.getLangOpts().CPlusPlus)return result;
  const clang::Preprocessor *preprocessor=nullptr;
  if(cache)if(auto found=cache->preprocessors.find(&context);found!=cache->preprocessors.end())preprocessor=found->second;
  if(!preprocessor){result.valid=false;return result;}
  const auto &source=context.getSourceManager();std::set<std::string> headers;
  // PCM source entries are lazy. Inventory the complete imported file set,
  // rather than whichever locations an earlier AST traversal happened to load.
  if(context.getLangOpts().ClangModules)
    for(unsigned i=0;i<source.loaded_sloc_entry_size();++i) {
      bool invalid=false;
      (void)source.getLoadedSLocEntry(i,&invalid);
      if(invalid)result.valid=false;
    }
  for(auto it=source.fileinfo_begin();it!=source.fileinfo_end();++it) {
    if(!it->second->OrigEntry)continue;
    const auto file=source.translateFile(*it->second->OrigEntry);
    if(file.isInvalid())continue;
    auto location=source.getLocForStartOfFile(file);
    bool over_budget=false;
    const bool included=HasStandardLibraryHeaderAncestor(file,source,&over_budget,true);
    if(over_budget)result.valid=false;
    if(!source.isInSystemHeader(location) || !included)continue;
    const auto name=llvm::sys::path::filename(source.getFilename(location)).str();
    if(IsStandardLibraryHeaderName(name))headers.insert(name);
    bool invalid=false;auto bytes=source.getBufferData(file,&invalid);
    if(invalid){result.valid=false;continue;}
    const auto key=name+"="+ComputeInterfaceSourceSha256(bytes);
    result.contents.insert(key);
    result.contents.insert(key+"@configuration="+StandardHeaderMacroProfile(
        const_cast<clang::Preprocessor &>(*preprocessor),file,bytes));
  }
  result.headers.assign(headers.begin(),headers.end());
  result.valid &= !result.headers.empty() && result.headers.size()<=128 && !result.contents.empty();
  if(cache)cache->manifests.emplace(&context,result);
  return result;
}
} // namespace objc3::clang_foundation
