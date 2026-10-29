#include "standard_coroutine_types.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/ast_effect_identity.h"
#include "clang_foundation/native_contract_identity.h"
#include "clang_foundation/interface_module_resolver.h"
#include "clang_foundation/module_names.h"
#include "clang_foundation/runtime_interface_types.h"

#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "clang_foundation/interface_codec.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/APValue.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/ObjC3Async.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Basic/ObjC3Foundation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Tooling.h"
#include "clang/UnifiedSymbolResolution/USRGeneration.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/SHA256.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Support/raw_ostream.h"

namespace objc3::clang_foundation {
namespace {

void AddDiagnostic(InterfaceEmissionArtifact &result, llvm::StringRef code,
                   llvm::StringRef message, llvm::StringRef subject = {}) {
  result.diagnostics.push_back({code.str(), message.str(), subject.str()});
}

bool IsExpressionLocalTag(const clang::TagDecl *declaration);
struct FactoryTypeFeatures {
  bool generic=false, closure=false, lambda_result=false, nested=false;
};
std::string FactoryTypeKey(clang::QualType type,const clang::ASTContext &context,FactoryTypeFeatures *features=nullptr);
bool HasOnlyProjectedLocalTypes(clang::QualType type);
std::string UnnamedTemplateRecordKey(const clang::CXXRecordDecl *,const clang::ASTContext &);
std::string UnnamedTemplateTypeKey(clang::QualType,const clang::ASTContext &);
void ReadFactoryArgumentFeatures(const clang::TemplateArgument &,const clang::ASTContext &,FactoryTypeFeatures &);
bool HasFactoryValueArgument(const clang::TemplateArgument &,const clang::ASTContext &);
bool HasFactoryTemplateArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context,FactoryTypeFeatures *features=nullptr);

bool IsForeignImportAttribute(const clang::Attr *attribute) {
  return llvm::isa<clang::SwiftNameAttr,clang::SwiftAsyncNameAttr,
      clang::SwiftAsyncAttr,clang::SwiftErrorAttr,clang::SwiftAsyncErrorAttr,
      clang::SwiftPrivateAttr,clang::SwiftAttrAttr,clang::SwiftNewTypeAttr,
      clang::SwiftBridgeAttr,clang::SwiftBridgedTypedefAttr,
      clang::SwiftImportAsNonGenericAttr,clang::SwiftImportPropertyAsAccessorsAttr,
      clang::SwiftObjCMembersAttr,clang::ObjCBridgeAttr,clang::ObjCBridgeMutableAttr,
      clang::ObjCBridgeRelatedAttr>(attribute);
}

bool IsPublishedNativeAttribute(const clang::Attr *attribute) {
  return llvm::isa<clang::AllocSizeAttr,clang::AllocAlignAttr,clang::AssumeAlignedAttr,clang::NonNullAttr,clang::ReturnsNonNullAttr,clang::FormatAttr,clang::FormatArgAttr,clang::RestrictAttr,clang::SysVABIAttr,clang::PreserveMostAttr,clang::PreserveAllAttr,clang::PreserveNoneAttr,clang::RegCallAttr,clang::VectorCallAttr,clang::AlignedAttr,clang::PackedAttr,clang::NoUniqueAddressAttr,
      clang::VisibilityAttr,clang::TypeVisibilityAttr,clang::NoInlineAttr,
      clang::AlwaysInlineAttr,clang::FlattenAttr,clang::ColdAttr,clang::HotAttr,
      clang::ConstAttr,clang::PureAttr,clang::WarnUnusedResultAttr,clang::UnusedAttr,
      clang::NoReturnAttr,clang::CXX11NoReturnAttr>(attribute);
}
std::string NativeAttributeValue(const clang::Attr *,const clang::NamedDecl *);

std::string DeclarationAttributeKey(const clang::NamedDecl *declaration,bool native=false);

std::string CanonicalType(clang::QualType type,
                          const clang::ASTContext &context) {
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.PrintAsCanonical = true;
  policy.FullyQualifiedName = true;
  policy.SuppressScope = false;
  policy.SuppressUnwrittenScope = false;
  policy.AnonymousTagNameStyle=static_cast<unsigned>(clang::PrintingPolicy::AnonymousTagMode::Plain);
  auto spelling=type.getCanonicalType().getAsString(policy);
  const auto origins=FactoryTypeKey(type,context);
  if(!origins.empty())spelling+=";factory-type-origins="+ComputeInterfaceSourceSha256(origins)+";";
  const auto members=UnnamedTemplateTypeKey(type,context);
  if(!members.empty())spelling+=";unnamed-template-record-origins="+ComputeInterfaceSourceSha256(members)+";";
  return spelling;
}

bool IsInMainFile(const clang::ASTContext &context,
                  clang::SourceLocation location,
                  const std::string &source_root = {}) {
  return IsModuleOwnedSource(context, location, source_root);
}

std::string InterfaceQualifiedName(const clang::NamedDecl *declaration) {
  if (const auto *using_enum=llvm::dyn_cast<clang::UsingEnumDecl>(declaration)) {
    std::string owner;
    if(const auto *parent=llvm::dyn_cast<clang::NamedDecl>(declaration->getDeclContext()))
      owner=InterfaceQualifiedName(parent)+"::";
    return owner+"@using-enum:"+InterfaceQualifiedName(using_enum->getEnumDecl());
  }
  if (const auto *directive=llvm::dyn_cast<clang::UsingDirectiveDecl>(declaration)) {
    std::string owner;
    if (const auto *parent=llvm::dyn_cast<clang::NamedDecl>(declaration->getDeclContext()))
      owner=InterfaceQualifiedName(parent)+"::";
    return owner+"@using:"+InterfaceQualifiedName(directive->getNominatedNamespaceAsWritten());
  }
  clang::PrintingPolicy policy(declaration->getASTContext().getLangOpts());
  policy.SuppressInlineNamespace = static_cast<unsigned>(
      clang::PrintingPolicy::SuppressInlineNamespaceMode::None);
  std::string text;
  llvm::raw_string_ostream stream(text);
  declaration->printQualifiedName(stream, policy);
  return text;
}

std::string HashInterfaceFacts(llvm::StringRef facts) {
  llvm::SHA256 hash; hash.update(facts);
  return llvm::toHex(hash.final(), true);
}

bool IsModeledNamespaceAttribute(const clang::Attr *attribute) {
  if (llvm::isa<clang::DeprecatedAttr, clang::UnavailableAttr, clang::AvailabilityAttr,
                clang::VisibilityAttr, clang::TypeVisibilityAttr, clang::AbiTagAttr>(attribute))
    return true;
  const auto *annotation = llvm::dyn_cast<clang::AnnotateAttr>(attribute);
  return annotation && annotation->getAnnotation().starts_with("objc3.spi:");
}

std::string NamespaceAttributeText(const clang::Attr *attribute,
                                  const clang::ASTContext &context) {
  clang::PrintingPolicy policy(context.getLangOpts());
  std::string text;
  llvm::raw_string_ostream stream(text);
  if (const auto *availability = llvm::dyn_cast<clang::AvailabilityAttr>(attribute)) {
    auto quote = [&](llvm::StringRef value) { stream << '\"'; stream.write_escaped(value); stream << '\"'; };
    stream << "__attribute__((availability(" << availability->getPlatform()->getName();
    if (!availability->getIntroduced().empty()) stream << ", introduced=" << availability->getIntroduced();
    if (!availability->getDeprecated().empty()) stream << ", deprecated=" << availability->getDeprecated();
    if (!availability->getObsoleted().empty()) stream << ", obsoleted=" << availability->getObsoleted();
    if (availability->getUnavailable()) stream << ", unavailable";
    if (availability->getEnvironment()) stream << ", environment=" << availability->getEnvironment()->getName();
    if (availability->getStrict()) stream << ", strict";
    if (!availability->getReplacement().empty()) { stream << ", replacement="; quote(availability->getReplacement()); }
    if (!availability->getMessage().empty()) { stream << ", message="; quote(availability->getMessage()); }
    stream << ")))";
  } else attribute->printPretty(stream, policy);
  return text;
}

std::set<std::string> NamespaceAttributes(const clang::NamespaceDecl *space, bool canonical = true) {
  std::set<std::string> attributes;
  for (const auto *attribute : (canonical ? space->getCanonicalDecl() : space)->attrs())
    if (!attribute->isImplicit())
      attributes.insert(NamespaceAttributeText(attribute, space->getASTContext()));
  return attributes;
}

void PrintNamespaceHead(const clang::NamespaceDecl *space, llvm::raw_ostream &out,
                         bool attributes) {
  out << (space->isInline() ? "inline namespace " : "namespace ");
  for (const auto &attribute : NamespaceAttributes(space, attributes)) out << attribute << ' ';
  out << space->getName();
}

bool IsTopLevel(const clang::NamedDecl *declaration) {
  const auto *context = declaration->getDeclContext();
  while (context) {
    if (llvm::isa<clang::LinkageSpecDecl>(context)) {
      context = context->getParent();
      continue;
    }
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(context)) {
      if (space->isAnonymousNamespace())
        return false;
      context = context->getParent();
      continue;
    }
    break;
  }
  return context && context->isTranslationUnit();
}

bool IsExplicitNamespaceFunctionSpecialization(const clang::FunctionDecl *function) {
  return function && !llvm::isa<clang::CXXMethodDecl>(function) && IsTopLevel(function) &&
      function->getPrimaryTemplate() &&
      function->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization;
}

bool IsExplicitNamespaceClassSpecialization(const clang::ClassTemplateSpecializationDecl *record) {
  return record && IsTopLevel(record) &&
      (llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(record) ||
       record->getSpecializationKind()==clang::TSK_ExplicitSpecialization);
}
bool IsClosedParentPattern(const clang::NamedDecl *declaration) {
  if (!declaration) return false;
  const auto *parent=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration->getDeclContext());
  if (!parent || parent->isDependentContext()) return false;
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) return pattern->isMemberSpecialization();
  if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) return pattern->isMemberSpecialization();
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) return pattern->isMemberSpecialization();
  if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(declaration))
    return !partial->getInstantiatedFromMember() || partial->isMemberSpecialization();
  if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration))
    return !partial->getInstantiatedFromMember() || partial->isMemberSpecialization();
  return false;
}
bool IsClosedParentMemberSpecialization(const clang::NamedDecl *declaration) {
  if (!declaration) return false;
  const auto *parent=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration->getDeclContext());
  if (!parent || parent->isDependentContext()) return false;
  if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration))
    return !llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(record) &&
        record->getSpecializationKind()==clang::TSK_ExplicitSpecialization;
  if (const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration))
    return !llvm::isa<clang::VarTemplatePartialSpecializationDecl>(variable) &&
        variable->getSpecializationKind()==clang::TSK_ExplicitSpecialization;
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
    return function->getPrimaryTemplate() &&
        function->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization;
  return false;
}
bool IsDependentMemberPartial(const clang::NamedDecl *declaration) {
  if (!declaration || !llvm::isa<clang::ClassTemplatePartialSpecializationDecl,
                               clang::VarTemplatePartialSpecializationDecl>(declaration)) return false;
  const auto *parent=llvm::dyn_cast<clang::CXXRecordDecl>(declaration->getDeclContext());
  return parent && parent->isDependentContext();
}
bool IsConcreteMemberClassSpecialization(const clang::ClassTemplateSpecializationDecl *record) {
  if (!record) return false;
  const auto *parent=llvm::dyn_cast<clang::CXXRecordDecl>(record->getDeclContext());
  return parent && !parent->isDependentContext() && !parent->getDescribedClassTemplate() &&
      !llvm::isa<clang::ClassTemplateSpecializationDecl>(parent) &&
      (llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(record) ||
       record->getSpecializationKind()==clang::TSK_ExplicitSpecialization);
}
std::vector<const clang::ClassTemplateSpecializationDecl *> PublishedClassSpecializations(
    const clang::ClassTemplateDecl *pattern) {
  std::vector<const clang::ClassTemplateSpecializationDecl *> result;
  llvm::SmallVector<clang::ClassTemplatePartialSpecializationDecl *,4> partials;
  pattern->getPartialSpecializations(partials);
  for (const auto *partial:partials)
    if (IsExplicitNamespaceClassSpecialization(partial) || IsConcreteMemberClassSpecialization(partial) || IsDependentMemberPartial(partial) || IsClosedParentPattern(partial)) {
      const auto *definition=partial->getDefinition();
      result.push_back(definition?llvm::cast<clang::ClassTemplateSpecializationDecl>(definition):partial);
    }
  for (const auto *specialization:pattern->specializations())
    if (IsExplicitNamespaceClassSpecialization(specialization) || IsConcreteMemberClassSpecialization(specialization)) {
      if (const auto *definition=specialization->getDefinition())
        specialization=llvm::cast<clang::ClassTemplateSpecializationDecl>(definition);
      result.push_back(specialization);
    }
  return result;
}

bool IsConcreteMemberVariableSpecialization(const clang::VarTemplateSpecializationDecl *variable) {
  if (!variable || !variable->isStaticDataMember()) return false;
  const auto *parent=llvm::dyn_cast<clang::CXXRecordDecl>(variable->getDeclContext());
  return parent && !parent->isDependentContext() && !parent->getDescribedClassTemplate() &&
      !llvm::isa<clang::ClassTemplateSpecializationDecl>(parent) &&
      (llvm::isa<clang::VarTemplatePartialSpecializationDecl>(variable) ||
       variable->getSpecializationKind()==clang::TSK_ExplicitSpecialization);
}
std::vector<const clang::VarTemplatePartialSpecializationDecl *> PublishedVariablePartials(
    const clang::VarTemplateDecl *pattern) {
  llvm::SmallVector<clang::VarTemplatePartialSpecializationDecl *,4> partials;
  pattern->getPartialSpecializations(partials);
  std::vector<const clang::VarTemplatePartialSpecializationDecl *> result;
  for (const auto *partial:partials) if (IsTopLevel(partial) || IsConcreteMemberVariableSpecialization(partial) || IsDependentMemberPartial(partial) || IsClosedParentPattern(partial)) {
    if (const auto *definition=partial->getDefinition())
      partial=llvm::cast<clang::VarTemplatePartialSpecializationDecl>(definition);
    result.push_back(partial);
  }
  return result;
}

bool IsExplicitNamespaceVariableSpecialization(const clang::VarTemplateSpecializationDecl *variable) {
  return variable && !llvm::isa<clang::VarTemplatePartialSpecializationDecl>(variable) &&
      IsTopLevel(variable) && variable->getSpecializationKind()==clang::TSK_ExplicitSpecialization;
}

const clang::ClassTemplateDecl *ClassTemplateOwner(const clang::NamedDecl *declaration) {
  const clang::ClassTemplateDecl *owner=nullptr;
  for (const clang::NamedDecl *current=declaration; current;
       current=llvm::dyn_cast<clang::NamedDecl>(current->getDeclContext())) {
    if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(current)) owner=pattern;
    if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(current)) {
      if (const auto *pattern=record->getDescribedClassTemplate()) owner=pattern;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(record))
        owner=specialization->getSpecializedTemplate();
    }
  }
  return owner;
}

// A concrete record retains its ordinary member rows. Only patterns and their
// instantiated descendants are published as part of the nearest record owner.
const clang::NamedDecl *TemplatePublicationOwner(const clang::NamedDecl *declaration) {
  bool pattern=false;
  for (const clang::NamedDecl *current=declaration; current;
       current=llvm::dyn_cast<clang::NamedDecl>(current->getDeclContext())) {
    if (IsStandardCoroutineTraitsSpecialization(current)) return current;
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(current))
      if (const auto *partial=record->getSpecializedTemplateOrPartial().dyn_cast<clang::ClassTemplatePartialSpecializationDecl *>();
          partial && IsStandardCoroutineTraitsSpecialization(partial)) return partial;
    // Expression-local closures keep their lexical body contract. A generic
    // lambda's call template must never turn its implicit class into an owner.
    if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(current);
        record && record->isLambda()) return ClassTemplateOwner(declaration);
    if (current!=declaration && llvm::isa<clang::FunctionDecl>(current))
      return ClassTemplateOwner(declaration);
    pattern |= llvm::isa<clang::TemplateDecl,clang::VarTemplateSpecializationDecl,
        clang::TemplateTypeParmDecl,clang::NonTypeTemplateParmDecl,clang::TemplateTemplateParmDecl>(current);
    if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(current))
      pattern |= function->getDescribedFunctionTemplate() || function->getPrimaryTemplate();
    if (const auto *alias=llvm::dyn_cast<clang::TypeAliasDecl>(current))
      pattern |= alias->getDescribedAliasTemplate()!=nullptr;
    if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(current))
      pattern |= variable->getDescribedVarTemplate()!=nullptr;
    if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(current)) {
      pattern |= record->getDescribedClassTemplate() ||
          llvm::isa<clang::ClassTemplateSpecializationDecl>(record);
      if (pattern && !record->isDependentContext() && !record->getDescribedClassTemplate() &&
          !llvm::isa<clang::ClassTemplateSpecializationDecl>(record)) return record;
    }
  }
  return ClassTemplateOwner(declaration);
}
bool IsDetachedCallableTemplateDefinition(const clang::FunctionDecl *function) {
  const auto *method=llvm::dyn_cast_or_null<clang::CXXMethodDecl>(function);
  return method && method->doesThisDeclarationHaveABody() && method->isOutOfLine() &&
      !method->getLexicalDeclContext()->isRecord() && !method->getPrimaryTemplate() &&
      method->getTemplateSpecializationKind()!=clang::TSK_ImplicitInstantiation &&
      method->getTemplateSpecializationKind()!=clang::TSK_ExplicitSpecialization &&
      (method->getParent()->isDependentContext() || method->getDescribedFunctionTemplate()) &&
      TemplatePublicationOwner(method);
}
bool IsDetachedCallableTemplateRoot(const clang::NamedDecl *declaration) {
  if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    return IsDetachedCallableTemplateDefinition(pattern->getTemplatedDecl());
  const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
  return IsDetachedCallableTemplateDefinition(function) && !function->getDescribedFunctionTemplate();
}
bool IsDetachedStaticDataTemplateDefinition(const clang::VarDecl *variable) {
  return variable && variable->isStaticDataMember() && variable->isOutOfLine() &&
      variable->isThisDeclarationADefinition()!=clang::VarDecl::DeclarationOnly &&
      !variable->getLexicalDeclContext()->isRecord() &&
      !llvm::isa<clang::VarTemplateSpecializationDecl>(variable) &&
      !variable->getInstantiatedFromStaticDataMember() &&
      (variable->getDeclContext()->isDependentContext() || variable->getDescribedVarTemplate()) &&
      TemplatePublicationOwner(variable);
}
bool IsDetachedStaticDataTemplateRoot(const clang::NamedDecl *declaration) {
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    return IsDetachedStaticDataTemplateDefinition(pattern->getTemplatedDecl());
  const auto *variable=llvm::dyn_cast<clang::VarDecl>(declaration);
  return IsDetachedStaticDataTemplateDefinition(variable) && !variable->getDescribedVarTemplate();
}
std::vector<clang::NamedDecl *> DetachedTemplateDefinitions(const clang::NamedDecl *owner,bool data) {
  std::vector<clang::NamedDecl *> result;
  std::set<const clang::Decl *> seen;
  std::set<const clang::CXXRecordDecl *> records;
  auto collect=[&](auto &&self,const clang::CXXRecordDecl *record)->void {
    if (!record) return;
    if (const auto *definition=record->getDefinition()) record=definition;
    if (!record->isCompleteDefinition() || !records.insert(record).second) return;
    for (const auto *member:record->decls()) {
      if (member->isImplicit()) continue;
      const clang::NamedDecl *definition=nullptr;
      const clang::NamedDecl *root=nullptr;
      if (data) {
        const auto *variable=llvm::dyn_cast<clang::VarDecl>(member);
        if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) variable=pattern->getTemplatedDecl();
        if (variable && IsDetachedStaticDataTemplateDefinition(variable->getDefinition())) {
          variable=variable->getDefinition();definition=root=variable;
          if (const auto *pattern=variable->getDescribedVarTemplate()) root=pattern;
        }
      } else {
        const auto *function=llvm::dyn_cast<clang::CXXMethodDecl>(member);
        if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(member))
          function=llvm::dyn_cast<clang::CXXMethodDecl>(pattern->getTemplatedDecl());
        if (function && IsDetachedCallableTemplateDefinition(function->getDefinition())) {
          function=llvm::cast<clang::CXXMethodDecl>(function->getDefinition());definition=root=function;
          if (const auto *pattern=function->getDescribedFunctionTemplate()) root=pattern;
        }
      }
      if (definition) {
        const auto *publication=TemplatePublicationOwner(definition);
        if (!publication || publication->getCanonicalDecl()!=owner->getCanonicalDecl() ||
            !seen.insert(definition->getCanonicalDecl()).second) continue;
        result.push_back(const_cast<clang::NamedDecl *>(root));
      } else if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        self(self,pattern->getTemplatedDecl());
        for (const auto *specialization:PublishedClassSpecializations(pattern)) self(self,specialization);
      } else if (const auto *nested=llvm::dyn_cast<clang::CXXRecordDecl>(member))
        if (!llvm::isa<clang::ClassTemplateSpecializationDecl>(nested) ||
            nested->getTemplateSpecializationKind()!=clang::TSK_ImplicitInstantiation) self(self,nested);
    }
  };
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(owner)) {
    collect(collect,pattern->getTemplatedDecl());
    for (const auto *specialization:PublishedClassSpecializations(pattern)) collect(collect,specialization);
  } else collect(collect,llvm::dyn_cast<clang::CXXRecordDecl>(owner));
  return result;
}

std::vector<clang::NamedDecl *> DetachedCallableTemplateDefinitions(const clang::NamedDecl *owner) {
  return DetachedTemplateDefinitions(owner,false);
}
std::vector<clang::NamedDecl *> DetachedStaticDataTemplateDefinitions(const clang::NamedDecl *owner) {
  return DetachedTemplateDefinitions(owner,true);
}

bool IsDetachedNestedTemplateDefinition(const clang::CXXRecordDecl *record) {
  return record && record->isCompleteDefinition() && !record->isImplicit() &&
      record->getDeclContext()->isRecord() && !record->getLexicalDeclContext()->isRecord() &&
      !llvm::isa<clang::ClassTemplateSpecializationDecl>(record) &&
      (record->getDeclContext()->isDependentContext() || record->getDescribedClassTemplate()) &&
      TemplatePublicationOwner(record);
}
bool IsDetachedNestedTemplateRoot(const clang::NamedDecl *declaration) {
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
    return IsDetachedNestedTemplateDefinition(pattern->getTemplatedDecl());
  const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
  return IsDetachedNestedTemplateDefinition(record) && !record->getDescribedClassTemplate();
}
std::vector<clang::NamedDecl *> DetachedNestedTemplateDefinitions(const clang::NamedDecl *owner) {
  std::vector<clang::NamedDecl *> result;
  std::set<const clang::CXXRecordDecl *> seen;
  auto collect=[&](auto &&self,const clang::CXXRecordDecl *record)->void {
    if (!record) return;
    if (const auto *definition=record->getDefinition()) record=definition;
    if (!record->isCompleteDefinition() || !seen.insert(record).second) return;
    if (IsDetachedNestedTemplateDefinition(record)) {
      const auto *publication=TemplatePublicationOwner(record);
      if (publication && publication->getCanonicalDecl()==owner->getCanonicalDecl()) {
        const clang::NamedDecl *root=record;
        if (const auto *pattern=record->getDescribedClassTemplate()) root=pattern;
        result.push_back(const_cast<clang::NamedDecl *>(root));
      }
    }
    for (const auto *member:record->decls()) {
      if (member->isImplicit()) continue;
      if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        self(self,pattern->getTemplatedDecl());
        for (const auto *specialization:PublishedClassSpecializations(pattern)) self(self,specialization);
      } else if (const auto *nested=llvm::dyn_cast<clang::CXXRecordDecl>(member))
        if (!llvm::isa<clang::ClassTemplateSpecializationDecl>(nested) ||
            nested->getTemplateSpecializationKind()!=clang::TSK_ImplicitInstantiation) self(self,nested);
    }
  };
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(owner)) {
    collect(collect,pattern->getTemplatedDecl());
    for (const auto *specialization:PublishedClassSpecializations(pattern)) collect(collect,specialization);
  } else collect(collect,llvm::dyn_cast<clang::CXXRecordDecl>(owner));
  return result;
}

std::vector<clang::NamedDecl *> ClosedParentMemberSpecializations(const clang::NamedDecl *owner) {
  std::vector<clang::NamedDecl *> result;
  std::set<const clang::CXXRecordDecl *> seen;
  std::set<const clang::Decl *> published;
  auto add=[&](const clang::NamedDecl *declaration) {
    if (IsClosedParentMemberSpecialization(declaration) && published.insert(declaration->getCanonicalDecl()).second)
      result.push_back(const_cast<clang::NamedDecl *>(declaration));
  };
  auto collect=[&](auto &&self,const clang::CXXRecordDecl *record)->void {
    if (!record) return;
    if (const auto *definition=record->getDefinition()) record=definition;
    if (!record->isCompleteDefinition() || !seen.insert(record).second) return;
    for (const auto *member:record->decls()) {
      if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        self(self,pattern->getTemplatedDecl());
        for (const auto *specialization:pattern->specializations()) {
          const auto *definition=specialization->getDefinition();
          auto *selected=definition?llvm::cast<clang::ClassTemplateSpecializationDecl>(definition):specialization;
          add(selected);self(self,selected);
        }
        llvm::SmallVector<clang::ClassTemplatePartialSpecializationDecl *,4> partials;
        pattern->getPartialSpecializations(partials);
        for (const auto *partial:partials) self(self,partial);
      } else if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) {
        for (const auto *function:pattern->specializations()) {
          const auto *selected=function->getMostRecentDecl();
          if (function->isInlined()) if (const auto *definition=function->getDefinition()) selected=definition;
          add(selected);
        }
      } else if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
        for (const auto *specialization:pattern->specializations()) {
          const auto *selected=static_cast<const clang::VarDecl *>(specialization)->getMostRecentDecl();
          if (const auto *definition=specialization->getDefinition()) selected=definition;
          add(selected);
        }
      } else if (const auto *nested=llvm::dyn_cast<clang::CXXRecordDecl>(member)) {
        add(nested);self(self,nested);
      }
    }
  };
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(owner)) {
    collect(collect,pattern->getTemplatedDecl());
    for (const auto *specialization:pattern->specializations()) collect(collect,specialization);
  } else collect(collect,llvm::dyn_cast<clang::CXXRecordDecl>(owner));
  return result;
}

std::vector<clang::NamedDecl *> ClosedParentPatterns(const clang::NamedDecl *owner) {
  std::vector<clang::NamedDecl *> result;
  std::set<const clang::CXXRecordDecl *> seen;
  std::set<const clang::Decl *> published;
  auto add=[&](const clang::NamedDecl *declaration) {
    if (IsClosedParentPattern(declaration) && published.insert(declaration->getCanonicalDecl()).second)
      result.push_back(const_cast<clang::NamedDecl *>(declaration));
  };
  auto collect=[&](auto &&self,const clang::CXXRecordDecl *record)->void {
    if (!record) return;
    if (const auto *definition=record->getDefinition()) record=definition;
    if (!record->isCompleteDefinition() || !seen.insert(record).second) return;
    for (const auto *member:record->decls()) {
      if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        const auto *selected=pattern->getMostRecentDecl();
        if (const auto *definition=pattern->getTemplatedDecl()->getDefinition())
          if (const auto *defined=definition->getDescribedClassTemplate()) selected=defined;
        add(selected);
        self(self,pattern->getTemplatedDecl());
        for (const auto *specialization:pattern->specializations()) {
          const auto *definition=specialization->getDefinition();
          auto *selected=definition?llvm::cast<clang::ClassTemplateSpecializationDecl>(definition):specialization;
          add(selected);self(self,selected);
        }
        llvm::SmallVector<clang::ClassTemplatePartialSpecializationDecl *,4> partials;
        pattern->getPartialSpecializations(partials);
        for (const auto *partial:partials) {add(partial);self(self,partial);}
      } else if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) {
        const auto *selected=pattern->getMostRecentDecl();
        if (const auto *definition=pattern->getTemplatedDecl()->getDefinition())
          if (const auto *defined=definition->getDescribedFunctionTemplate()) selected=defined;
        add(selected);
        for (const auto *function:pattern->specializations()) {
          const auto *selected=function->getMostRecentDecl();
          if (function->isInlined()) if (const auto *definition=function->getDefinition()) selected=definition;
          add(selected);
        }
      } else if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
        const auto *selected=pattern->getMostRecentDecl();
        if (const auto *definition=pattern->getTemplatedDecl()->getDefinition())
          if (const auto *defined=definition->getDescribedVarTemplate()) selected=defined;
        add(selected);
        llvm::SmallVector<clang::VarTemplatePartialSpecializationDecl *,4> partials;
        pattern->getPartialSpecializations(partials);
        for (const auto *partial:partials) add(partial);
        for (const auto *specialization:pattern->specializations()) {
          const auto *selected=static_cast<const clang::VarDecl *>(specialization)->getMostRecentDecl();
          if (const auto *definition=specialization->getDefinition()) selected=definition;
          add(selected);
        }
      } else if (const auto *nested=llvm::dyn_cast<clang::CXXRecordDecl>(member)) {
        add(nested);self(self,nested);
      }
    }
  };
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(owner)) {
    collect(collect,pattern->getTemplatedDecl());
    for (const auto *specialization:pattern->specializations()) collect(collect,specialization);
  } else collect(collect,llvm::dyn_cast<clang::CXXRecordDecl>(owner));
  return result;
}

std::vector<clang::NamedDecl *> DependentMemberPartials(const clang::NamedDecl *owner) {
  std::vector<clang::NamedDecl *> result;
  std::set<const clang::CXXRecordDecl *> seen;
  auto collect=[&](auto &&self,const clang::CXXRecordDecl *record)->void {
    if (!record) return;
    if (const auto *definition=record->getDefinition()) record=definition;
    if (!record->isCompleteDefinition() || !seen.insert(record).second) return;
    for (const auto *member:record->decls()) {
      if (member->isImplicit()) continue;
      if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        self(self,pattern->getTemplatedDecl());
        for (const auto *specialization:PublishedClassSpecializations(pattern)) {
          if (IsDependentMemberPartial(specialization))
            result.push_back(const_cast<clang::ClassTemplateSpecializationDecl *>(specialization));
          self(self,specialization);
        }
      } else if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
        for (const auto *partial:PublishedVariablePartials(pattern))
          if (IsDependentMemberPartial(partial))
            result.push_back(const_cast<clang::VarTemplatePartialSpecializationDecl *>(partial));
      } else if (const auto *nested=llvm::dyn_cast<clang::CXXRecordDecl>(member))
        if (!llvm::isa<clang::ClassTemplateSpecializationDecl>(nested)) self(self,nested);
    }
  };
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(owner)) {
    collect(collect,pattern->getTemplatedDecl());
    for (const auto *specialization:PublishedClassSpecializations(pattern)) collect(collect,specialization);
  } else collect(collect,llvm::dyn_cast<clang::CXXRecordDecl>(owner));
  return result;
}

std::vector<clang::ClassTemplateSpecializationDecl *> ConcreteMemberClassSpecializations(
    const clang::NamedDecl *owner) {
  std::vector<clang::ClassTemplateSpecializationDecl *> result;
  const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(owner);
  if (!record || !record->getDefinition()) return result;
  for (const auto *member:record->getDefinition()->decls())
    if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member))
      for (const auto *specialization:PublishedClassSpecializations(pattern))
        if (IsConcreteMemberClassSpecialization(specialization))
          result.push_back(const_cast<clang::ClassTemplateSpecializationDecl *>(specialization));
  return result;
}

bool IsConcreteMemberFunctionSpecialization(const clang::FunctionDecl *function) {
  const auto *method=llvm::dyn_cast_or_null<clang::CXXMethodDecl>(function);
  if (!method || !method->getPrimaryTemplate() ||
      method->getTemplateSpecializationKind()!=clang::TSK_ExplicitSpecialization) return false;
  const auto *parent=method->getParent();
  return !parent->isDependentContext() && !parent->getDescribedClassTemplate() &&
      !llvm::isa<clang::ClassTemplateSpecializationDecl>(parent);
}
bool IsConcreteSpecialMemberSpecialization(const clang::FunctionDecl *function) {
  return IsConcreteMemberFunctionSpecialization(function) &&
      (llvm::isa<clang::CXXConstructorDecl,clang::CXXConversionDecl>(function) || function->isOverloadedOperator());
}
std::vector<clang::FunctionDecl *> ConcreteMemberFunctionSpecializations(const clang::NamedDecl *owner) {
  std::vector<clang::FunctionDecl *> result;
  const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(owner);
  if (!record || !record->getDefinition()) return result;
  for (const auto *member:record->getDefinition()->decls())
    if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(member))
      for (const auto *function:pattern->specializations())
        if (IsConcreteMemberFunctionSpecialization(function)) {
          const auto *selected=function->getMostRecentDecl();
          if (function->isInlined())
            if (const auto *definition=function->getDefinition()) selected=definition;
          result.push_back(const_cast<clang::FunctionDecl *>(selected));
        }
  return result;
}

std::vector<clang::VarTemplateSpecializationDecl *> ConcreteMemberVariableSpecializations(const clang::NamedDecl *owner) {
  std::vector<clang::VarTemplateSpecializationDecl *> result;
  const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(owner);
  if (!record || !record->getDefinition()) return result;
  for (const auto *member:record->getDefinition()->decls())
    if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
      for (const auto *partial:PublishedVariablePartials(pattern))
        if (IsConcreteMemberVariableSpecialization(partial))
          result.push_back(const_cast<clang::VarTemplatePartialSpecializationDecl *>(partial));
      for (const auto *specialization:pattern->specializations())
        if (IsConcreteMemberVariableSpecialization(specialization)) {
          const auto *selected=static_cast<const clang::VarDecl *>(specialization)->getMostRecentDecl();
          if (const auto *definition=specialization->getDefinition()) selected=definition;
          result.push_back(const_cast<clang::VarTemplateSpecializationDecl *>(
              llvm::cast<clang::VarTemplateSpecializationDecl>(selected)));
        }
    }
  return result;
}

bool HasDirectMemberTemplates(const clang::CXXRecordDecl *record) {
  return record && record->isCompleteDefinition() && llvm::any_of(record->decls(),[](const auto *member) {
    return !member->isImplicit() && llvm::isa<clang::FunctionTemplateDecl,
        clang::TypeAliasTemplateDecl,clang::VarTemplateDecl,clang::ClassTemplateDecl>(member);
  });
}

bool IsPublishedType(const clang::NamedDecl *declaration) {
  if (IsTopLevel(declaration)) return true;
  const auto *owner = llvm::dyn_cast<clang::CXXRecordDecl>(declaration->getDeclContext());
  return owner && IsPublishedType(owner);
}

const char *CxxAccessName(clang::AccessSpecifier access) {
  return access == clang::AS_public ? "public" :
         access == clang::AS_protected ? "protected" :
         access == clang::AS_private ? "private" : "none";
}

bool IsPublicContainer(const clang::DeclContext *context,
                       const clang::ASTContext &ast,
                       const std::string &source_root = {}) {
  if (const auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(context))
    return IsInMainFile(ast, interface->getLocation(), source_root) &&
           interface->isExternallyVisible();
  if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(context))
    return IsInMainFile(ast, protocol->getLocation(), source_root) &&
           protocol->isExternallyVisible();
  if (const auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(context)) {
    const auto *interface = category->getClassInterface();
    return !category->IsClassExtension() && interface &&
           IsInMainFile(ast, category->getLocation(), source_root) &&
           interface->isExternallyVisible();
  }
  return false;
}

bool IsIdentityCandidate(const clang::NamedDecl *declaration,
                         const clang::ASTContext &context,
                         const std::string &source_root = {}) {
  if (declaration->isImplicit() ||
      !IsInMainFile(context, declaration->getLocation(), source_root))
    return false;

  if (llvm::isa<clang::ClassTemplateDecl>(declaration)) return IsTopLevel(declaration);
  if (IsStandardCoroutineTraitsSpecialization(declaration)) return IsTopLevel(declaration);
  if (TemplatePublicationOwner(declaration)) return false;
  if (IsTopLevel(declaration)) {
    if (llvm::isa<clang::ConceptDecl, clang::TypeAliasTemplateDecl>(declaration)) return true;
    if (const auto *alias = llvm::dyn_cast<clang::TypeAliasDecl>(declaration);
        alias && alias->getDescribedAliasTemplate()) return false;
    if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
      return pattern->getTemplatedDecl()->getStorageClass()!=clang::SC_Static;
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
        variable && variable->getDescribedVarTemplate()) return false;
    if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
      return pattern->getTemplatedDecl()->isExternallyVisible();
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && function->getDescribedFunctionTemplate()) return false;
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration))
      return !space->isAnonymousNamespace();
    if (llvm::isa<clang::NamespaceAliasDecl,clang::BaseUsingDecl,clang::UsingDirectiveDecl>(declaration))
      return true;
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
      return !llvm::isa<clang::CXXMethodDecl>(function) &&
             function->isExternallyVisible();
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
      return variable->hasGlobalStorage() && variable->isExternallyVisible();
    // C typedef names have no linkage, but are public module lookup names.
    if (llvm::isa<clang::TypedefNameDecl>(declaration)) return true;
    if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration))
      return tag->isExternallyVisible();
    if (const auto *objc = llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration))
      return objc->isExternallyVisible();
    if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(declaration))
      return protocol->isExternallyVisible();
    if (const auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(declaration))
      return !category->IsClassExtension() &&
             category->getClassInterface() &&
             category->getClassInterface()->isExternallyVisible();
    return false;
  }

  if (llvm::isa<clang::BaseUsingDecl>(declaration))
    if (const auto *owner=llvm::dyn_cast<clang::CXXRecordDecl>(declaration->getDeclContext()))
      return IsPublishedType(owner);
  if (llvm::isa<clang::TypeDecl>(declaration) &&
      llvm::isa<clang::CXXRecordDecl>(declaration->getDeclContext()))
    return IsPublishedType(declaration);
  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(declaration))
    return IsPublishedType(method->getParent());
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
      variable && variable->isStaticDataMember())
    return IsPublishedType(llvm::cast<clang::CXXRecordDecl>(variable->getDeclContext()));

  if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration)) {
    const auto *owner =
        llvm::dyn_cast<clang::RecordDecl>(field->getDeclContext());
    return field->getIdentifier() && owner &&
           (!context.getLangOpts().CPlusPlus || IsPublishedType(owner));
  }
  if (llvm::isa<clang::EnumConstantDecl>(declaration))
    return llvm::isa<clang::EnumDecl>(declaration->getDeclContext()) &&
           (!context.getLangOpts().CPlusPlus ||
            IsPublishedType(
                llvm::cast<clang::EnumDecl>(declaration->getDeclContext())));
  if (llvm::isa<clang::ObjCMethodDecl, clang::ObjCPropertyDecl,
                clang::ObjCIvarDecl>(declaration))
    return IsPublicContainer(declaration->getDeclContext(), context,
                             source_root);
  return false;
}

clang::NamedDecl *Representative(clang::NamedDecl *declaration) {
  if (auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    if (auto *definition = pattern->getTemplatedDecl()->getDefinition())
      return llvm::cast<clang::CXXRecordDecl>(definition)->getDescribedClassTemplate();
    return pattern->getMostRecentDecl();
  }
  if (auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    return pattern->getDefinition() ? pattern->getDefinition() : pattern->getMostRecentDecl();
  if (auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    return pattern->getMostRecentDecl();
  if (auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration))
    return space->getCanonicalDecl();
  if (auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    return function->getMostRecentDecl();
  if (auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
    return variable->getDefinition() ? variable->getDefinition() : variable->getMostRecentDecl();
  if (auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration))
    return tag->getDefinition() ? tag->getDefinition() : tag;
  if (auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration))
    return interface->getDefinition() ? interface->getDefinition() : interface;
  if (auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(declaration))
    return protocol->getDefinition() ? protocol->getDefinition() : protocol;
  if (auto *typedef_name = llvm::dyn_cast<clang::TypedefNameDecl>(declaration))
    return typedef_name->getMostRecentDecl();
  return declaration;
}

bool IsDefinition(const clang::NamedDecl *declaration) {
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    return function->doesThisDeclarationHaveABody();
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
    return variable->isThisDeclarationADefinition();
  if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration))
    return tag->isCompleteDefinition();
  if (const auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration))
    return interface->isThisDeclarationADefinition();
  if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(declaration))
    return protocol->isThisDeclarationADefinition();
  return true;
}

bool PublishesFunctionBody(const clang::FunctionDecl *function) {
  return function && function->getASTContext().getLangOpts().CPlusPlus &&
         (function->isInlined() || function->getDescribedFunctionTemplate() ||
          IsDetachedCallableTemplateDefinition(function->getDefinition())) &&
         !function->isDefaulted() && !function->isDeleted();
}
bool PublishesVariableInitializer(const clang::VarDecl *variable) {
  return variable && variable->getASTContext().getLangOpts().CPlusPlus &&
         (IsDetachedStaticDataTemplateDefinition(variable->getDefinition()) ||
          variable->getDescribedVarTemplate() || llvm::isa<clang::VarTemplatePartialSpecializationDecl>(variable) ||
          variable->isInline() || variable->isConstexpr() ||
          (variable->hasInit() && variable->mightBeUsableInConstantExpressions(
                                      variable->getASTContext())));
}

bool IsExpressionLocalTag(const clang::TagDecl *declaration) {
  for (const auto *scope = declaration->getDeclContext(); scope;
       scope = scope->getParent())
    if (scope->isFunctionOrMethod()) return true;
  return false;
}

// Bind spelling, typed native nodes and every resolved reference. Text alone
// cannot distinguish identically spelled defaults that select different overloads.
// Length delimiters keep user strings from introducing synthetic identity fields.
std::string DeclarationAttributeKey(const clang::NamedDecl *declaration,bool native) {
  if(llvm::isa<clang::NamespaceDecl>(declaration))return {};
  struct Attributes:clang::RecursiveASTVisitor<Attributes> {
    bool native;
    explicit Attributes(bool native):native(native){}
    std::string key;
    unsigned ordinal=0;
    std::set<const clang::Decl *> visited;
    bool TraverseStmt(clang::Stmt *) {return true;}
    bool TraverseDecl(clang::Decl *declaration) {
      if (!declaration || declaration->isImplicit() || !visited.insert(declaration).second) return true;
      if (!clang::RecursiveASTVisitor<Attributes>::TraverseDecl(declaration)) return false;
      const auto *named=llvm::dyn_cast<clang::NamedDecl>(declaration);
      if (!named) return true;
      std::vector<const clang::NamedDecl *> extra;
      auto append=[&](const auto &declarations){extra.insert(extra.end(),declarations.begin(),declarations.end());};
      if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(named))append(PublishedClassSpecializations(pattern));
      if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(named))append(PublishedVariablePartials(pattern));
      append(DetachedCallableTemplateDefinitions(named));append(DetachedStaticDataTemplateDefinitions(named));
      append(DetachedNestedTemplateDefinitions(named));append(ClosedParentMemberSpecializations(named));
      append(ClosedParentPatterns(named));append(DependentMemberPartials(named));
      append(ConcreteMemberClassSpecializations(named));append(ConcreteMemberFunctionSpecializations(named));
      append(ConcreteMemberVariableSpecializations(named));
      std::stable_sort(extra.begin(),extra.end(),[](const auto *left,const auto *right){
        return ReadInterfaceDeclarationStableId(left)<ReadInterfaceDeclarationStableId(right);
      });
      for (const auto *child:extra)
        if (!TraverseDecl(const_cast<clang::NamedDecl *>(child))) return false;
      return true;
    }
    bool VisitNamedDecl(clang::NamedDecl *declaration) {
      const auto slot=ordinal++;
      // ASTImporter can merge the same inherited contract from several
      // redeclarations. Identical metadata is one semantic contract.
      std::set<std::string> values;
      if (native)
        if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
          if (function->getType()->castAs<clang::FunctionType>()->getNoReturnAttr())
            values.insert("native-noreturn-function-type");
      for(const auto *attribute:declaration->attrs())
        if((native ? IsPublishedNativeAttribute(attribute) : IsForeignImportAttribute(attribute)) && !attribute->isImplicit()) {
          if(native) {values.insert(NativeAttributeValue(attribute,declaration));continue;}
          std::string value;llvm::raw_string_ostream out(value);
          clang::PrintingPolicy policy(declaration->getASTContext().getLangOpts());
          attribute->printPretty(out,policy);values.insert(value);
        }
      if(values.empty())return true;
      key+=std::to_string(slot)+":";
      for(const auto &value:values)key+=std::to_string(value.size())+":"+value;
      return true;
    }
  } visitor(native);
  visitor.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));return visitor.key;
}

const clang::CoroutineBodyStmt *PublishedCxxCoroutine(const clang::FunctionDecl *function) {
  if (!function) return nullptr;
  const auto *type=clang::getObjC3CallableFunctionType(function);
  if (type && type->getObjC3FunctionEffects().Async) return nullptr;
  return llvm::dyn_cast_or_null<clang::CoroutineBodyStmt>(function->getBody());
}

// Keep the written body independent of compiler-generated frame temporaries.
// Resolved promise/awaiter customization points get a separate dependency key.
std::vector<const clang::NamedDecl *> CxxCoroutineReferences(const clang::FunctionDecl *function) {
  struct References final : clang::RecursiveASTVisitor<References> {
    const clang::ASTContext &context;
    explicit References(const clang::ASTContext &context):context(context){}
    std::map<std::string,const clang::NamedDecl *> declarations;
    void Add(const clang::NamedDecl *declaration) {
      if (!declaration || declaration->isImplicit()) return;
      if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
        if (function->getBuiltinID()) return;
      const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration->getDeclContext());
      const bool protocol_member=record && record->getIdentifier() &&
          (record->getName()=="coroutine_handle" || record->getName()=="suspend_always" ||
           record->getName()=="suspend_never");
      for (auto *scope=declaration->getDeclContext();scope;scope=scope->getParent()) {
        if (scope->isFunctionOrMethod()) return;
        if (const auto *space=llvm::dyn_cast<clang::NamespaceDecl>(scope))
          if (space->isStdNamespace() && protocol_member &&
              context.getSourceManager().isInSystemHeader(declaration->getLocation())) return;
      }
      declarations.emplace(ReadInterfaceDeclarationStableId(declaration),declaration);
    }
    bool VisitCallExpr(clang::CallExpr *expression) {Add(expression->getDirectCallee());return true;}
    bool VisitCXXConstructExpr(clang::CXXConstructExpr *expression) {Add(expression->getConstructor());return true;}
  } references(function->getASTContext());
  if (const auto *body=PublishedCxxCoroutine(function))
    references.TraverseStmt(const_cast<clang::CoroutineBodyStmt *>(body));
  std::vector<const clang::NamedDecl *> result;
  for (const auto &[key,declaration]:references.declarations) result.push_back(declaration);
  return result;
}

std::string CxxCoroutineContract(const clang::FunctionDecl *function,
                                  const clang::ASTContext &context) {
  const auto *body=PublishedCxxCoroutine(function);
  if (!body) return {};
  std::string key=body->hasDependentPromiseType() ? "dependent-promise" :
      CanonicalType(body->getPromiseDecl()->getType(),context);
  for (const auto *declaration:CxxCoroutineReferences(function)) {
    const auto id=ReadInterfaceDeclarationStableId(declaration);
    key+=std::to_string(id.size())+":"+id;
  }
  return ComputeInterfaceSourceSha256(key);
}

const clang::Stmt *PublishedFunctionBody(const clang::FunctionDecl *function) {
  if (const auto *body=llvm::dyn_cast_or_null<clang::CoroutineBodyStmt>(function->getBody()))
    return body->getBody();
  return function->getBody();
}

const clang::Expr *PublishedAwaitSource(const clang::CoawaitExpr *await,
                                        const clang::FunctionDecl *function,
                                        const clang::ASTContext &context) {
  if (const auto *source=clang::getObjC3AwaitSourceExpression(await))
    return source;
  const auto *type=clang::getObjC3CallableFunctionType(function);
  if (type && type->getObjC3FunctionEffects().Async &&
      !type->getObjC3FunctionEffects().AsyncFrame &&
      IsQualifiedRuntimeTaskHeader(context))
    return await->getOperand();
  return nullptr;
}

bool HasAddressTemplateArgument(const clang::TemplateArgument &,const clang::ASTContext &);

class RuntimeDeclarationFinder final
    : public clang::RecursiveASTVisitor<RuntimeDeclarationFinder> {
public:
  explicit RuntimeDeclarationFinder(const clang::ASTContext &context)
      : context_(context) {}
  bool HasCxxCoroutines=false;
  std::set<std::string> StandardCoroutineHeaders;
  void StandardCoroutineReference(const clang::NamedDecl *declaration) {
    if (const auto *owner=StandardCoroutineHeaderOwner(declaration,context_)) {
      const auto &source=context_.getSourceManager();bool invalid=false;
      const auto bytes=source.getBufferData(source.getFileID(source.getSpellingLoc(owner->getLocation())),&invalid);
      if (!invalid) StandardCoroutineHeaders.insert(ComputeInterfaceSourceSha256(bytes));
    }
  }
  bool Found=false, HasCallableEffects=false, HasAddressArguments=false, HasFactoryArguments=false, HasFactoryValueArguments=false;
  FactoryTypeFeatures FactoryTypes;
  void InspectArguments(llvm::ArrayRef<clang::TemplateArgument> arguments) {
    for (const auto &argument:arguments) {
      ReadFactoryArgumentFeatures(argument,context_,FactoryTypes);
      HasFactoryValueArguments |= HasFactoryValueArgument(argument,context_);
      HasAddressArguments |= HasAddressTemplateArgument(argument,context_);
      HasFactoryArguments |= HasFactoryTemplateArgument(argument,context_,&FactoryTypes);
    }
  }
  void InspectType(clang::QualType type) {
    if (!type.isNull()) {
      if (const auto *record=type->getAsCXXRecordDecl()) StandardCoroutineReference(record);
      if (const auto *specialization=type->getAs<clang::TemplateSpecializationType>())
        StandardCoroutineReference(specialization->getTemplateName().getAsTemplateDecl());
    }
    if(!type.isNull())FactoryTypeKey(type,context_,&FactoryTypes);
    if (!type.isNull())
      if (const auto *record=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(type->getAsCXXRecordDecl()))
        InspectArguments(record->getTemplateArgs().asArray());
  }
  bool VisitTypeLoc(clang::TypeLoc location) {InspectType(location.getType());return true;}
  bool TraverseTemplateArgument(const clang::TemplateArgument &argument) {
    ReadFactoryArgumentFeatures(argument,context_,FactoryTypes);
    HasFactoryValueArguments |= HasFactoryValueArgument(argument,context_);
    HasAddressArguments |= HasAddressTemplateArgument(argument,context_);
    HasFactoryArguments |= HasFactoryTemplateArgument(argument,context_,&FactoryTypes);
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseTemplateArgument(argument);
  }
  bool TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc &argument) {
    ReadFactoryArgumentFeatures(argument.getArgument(),context_,FactoryTypes);
    HasFactoryValueArguments |= HasFactoryValueArgument(argument.getArgument(),context_);
    HasAddressArguments |= HasAddressTemplateArgument(argument.getArgument(),context_);
    HasFactoryArguments |= HasFactoryTemplateArgument(argument.getArgument(),context_,&FactoryTypes);
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseTemplateArgumentLoc(argument);
  }
  bool VisitFunctionProtoType(clang::FunctionProtoType *type) {
    const auto effects=type->getObjC3FunctionEffects();
    HasCallableEffects |= effects.Async || effects.Throws!=clang::ObjC3ThrowsKind::None;
    return true;
  }
  bool TraverseType(clang::QualType type, bool TraverseQualifier=true) {
    InspectType(type);
    // Alias-template specializations can hide their FunctionProtoType from
    // ordinary traversal. Read the desugared callable contract as well.
    const auto effects=ReadCanonicalFunctionEffects(type,context_);
    HasCallableEffects |= effects.is_async || effects.is_throwing;
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseType(type,TraverseQualifier);
  }
  void Reference(const clang::NamedDecl *declaration) {
    if(!declaration)return;
    StandardCoroutineReference(declaration);
    if(const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
      if(const auto *arguments=function->getTemplateSpecializationArgs())InspectArguments(arguments->asArray());
    if(const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration))
      InspectArguments(variable->getTemplateArgs().asArray());
    Found |= IsQualifiedRuntimeServiceDeclaration(declaration,context_) ||
             IsQualifiedRuntimeTaskDeclaration(declaration,context_);
  }
  bool VisitTagType(clang::TagType *type) { Reference(type->getDecl()); return true; }
  bool VisitTypedefType(clang::TypedefType *type) { Reference(type->getDecl()); return true; }
  bool VisitExpr(clang::Expr *expr) { TraverseType(expr->getType()); return true; }
  bool VisitDeclRefExpr(clang::DeclRefExpr *expr) { Reference(expr->getDecl()); return true; }
  bool VisitMemberExpr(clang::MemberExpr *expr) { Reference(expr->getMemberDecl()); return true; }
  bool VisitCXXConstructExpr(clang::CXXConstructExpr *expr) { Reference(expr->getConstructor()); return true; }
  bool TraverseLambdaExpr(clang::LambdaExpr *expression,DataRecursionQueue * = nullptr) {
    if(const auto *pattern=expression->getCallOperator()->getDescribedFunctionTemplate())
      for(const auto *specialization:pattern->specializations())
        if(const auto *arguments=specialization->getTemplateSpecializationArgs())InspectArguments(arguments->asArray());
    lambda_bodies_[expression->getBody()]=expression->getCallOperator();
    TraverseType(expression->getCallOperator()->getType());
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseLambdaExpr(expression,nullptr);
  }
  bool TraverseStmt(clang::Stmt *statement,DataRecursionQueue *queue=nullptr) {
    const auto *previous=function_;
    auto restore=llvm::scope_exit([&]{function_=previous;});
    if(auto found=lambda_bodies_.find(statement);found!=lambda_bodies_.end()) {
      function_=found->second;
      HasCxxCoroutines |= PublishedCxxCoroutine(function_)!=nullptr;
      statement=const_cast<clang::Stmt *>(PublishedFunctionBody(function_));
    }
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseStmt(statement,queue);
  }
  bool TraverseDecl(clang::Decl *declaration) {
    if (const auto *named=llvm::dyn_cast_or_null<clang::NamedDecl>(declaration)) StandardCoroutineReference(named);
    if (const auto *record=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(declaration))
      InspectArguments(record->getTemplateArgs().asArray());
    if (const auto *variable=llvm::dyn_cast_or_null<clang::VarTemplateSpecializationDecl>(declaration))
      InspectArguments(variable->getTemplateArgs().asArray());

    if (auto *function=llvm::dyn_cast_or_null<clang::FunctionDecl>(declaration)) {
      if (const auto *arguments=function->getTemplateSpecializationArgs())
        InspectArguments(arguments->asArray());
      const auto *previous=function_;
      function_=function;
      auto restore=llvm::scope_exit([&] { function_=previous; });
      TraverseType(function->getType());
      for (const auto *parameter:function->parameters())
        if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg())
          TraverseStmt(const_cast<clang::Expr *>(parameter->getDefaultArg()));
      if (PublishesFunctionBody(function)) {
        HasCxxCoroutines |= PublishedCxxCoroutine(function)!=nullptr;
        if (const auto *constructor=llvm::dyn_cast<clang::CXXConstructorDecl>(function))
          for (const auto *initializer:constructor->inits())
            if (initializer->isWritten()) TraverseStmt(initializer->getInit());
        TraverseStmt(const_cast<clang::Stmt *>(PublishedFunctionBody(function)));
      }
      return true;
    }
    if (auto *variable=llvm::dyn_cast_or_null<clang::VarDecl>(declaration);
        variable && variable->hasGlobalStorage() && !variable->isLocalVarDecl()) {
      TraverseType(variable->getType());
      if (PublishesVariableInitializer(variable)) TraverseStmt(variable->getInit());
      return true;
    }
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseDecl(declaration);
  }
  bool TraverseCoroutineBodyStmt(clang::CoroutineBodyStmt *body,
                                 DataRecursionQueue * = nullptr) {
    return TraverseStmt(body->getBody());
  }
  bool TraverseCoawaitExpr(clang::CoawaitExpr *await,DataRecursionQueue * = nullptr) {
    if (const auto *source=PublishedAwaitSource(await,function_,context_)) {
      Found |= !clang::getObjC3AwaitSourceExpression(await);
      return TraverseStmt(const_cast<clang::Expr *>(source));
    }
    return clang::RecursiveASTVisitor<RuntimeDeclarationFinder>::TraverseCoawaitExpr(await,nullptr);
  }
private:
  const clang::ASTContext &context_;
  const clang::FunctionDecl *function_=nullptr;
  std::map<const clang::Stmt *,const clang::FunctionDecl *> lambda_bodies_;
};

void VisitPublishedRuntimeContracts(const clang::NamedDecl *declaration,
                                     RuntimeDeclarationFinder &finder) {
  finder.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
    for (const auto *specialization:PublishedClassSpecializations(pattern))
      finder.TraverseDecl(const_cast<clang::ClassTemplateSpecializationDecl *>(specialization));
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    for (const auto *partial:PublishedVariablePartials(pattern))
      finder.TraverseDecl(const_cast<clang::VarTemplatePartialSpecializationDecl *>(partial));
  for (auto *member:ClosedParentMemberSpecializations(declaration)) finder.TraverseDecl(member);
  for (auto *pattern:ClosedParentPatterns(declaration)) finder.TraverseDecl(pattern);
  for (auto *definition:DetachedCallableTemplateDefinitions(declaration)) finder.TraverseDecl(definition);
  for (auto *definition:DetachedStaticDataTemplateDefinitions(declaration)) finder.TraverseDecl(definition);
  for (auto *definition:DetachedNestedTemplateDefinitions(declaration)) finder.TraverseDecl(definition);
  for (auto *specialization:ConcreteMemberClassSpecializations(declaration)) finder.TraverseDecl(specialization);
  for (auto *specialization:ConcreteMemberFunctionSpecializations(declaration))
    if (specialization->isInlined()) finder.TraverseDecl(specialization);
  for (auto *specialization:ConcreteMemberVariableSpecializations(declaration))
    if (PublishesVariableInitializer(specialization)) finder.TraverseDecl(specialization);
}

bool HasRuntimeDeclarationContract(const clang::NamedDecl *declaration,
                                   const clang::ASTContext &context) {
  // Empty namespace wrappers do not own member runtime contracts.
  if (llvm::isa<clang::NamespaceDecl>(declaration)) return false;
  RuntimeDeclarationFinder finder(context);
  VisitPublishedRuntimeContracts(declaration,finder);
  return finder.Found;
}

bool HasEffectfulTemplateProjection(const clang::NamedDecl *declaration,
                                     const clang::ASTContext &context) {
  const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
  if (!llvm::isa<clang::TemplateDecl>(declaration) && !HasDirectMemberTemplates(record) &&
      !IsExplicitNamespaceFunctionSpecialization(llvm::dyn_cast<clang::FunctionDecl>(declaration)) &&
      !IsExplicitNamespaceVariableSpecialization(llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration)))
    return false;
  RuntimeDeclarationFinder finder(context);
  VisitPublishedRuntimeContracts(declaration,finder);
  return finder.HasCallableEffects;
}

clang::Expr *KeyPathSubscriptSyntax(clang::PseudoObjectExpr *expression) {
  auto *syntax=expression->getSyntacticForm()->IgnoreParenImpCasts();
  auto *base=syntax;
  if (auto *assignment=llvm::dyn_cast<clang::BinaryOperator>(base);
      assignment && assignment->isAssignmentOp())
    base=assignment->getLHS()->IgnoreParenImpCasts();
  if (auto *increment=llvm::dyn_cast<clang::UnaryOperator>(base);
      increment && increment->isIncrementDecrementOp())
    base=increment->getSubExpr()->IgnoreParenImpCasts();
  clang::ObjC3KeyPathInfo info;
  auto *subscript=llvm::dyn_cast<clang::ObjCSubscriptRefExpr>(base);
  return subscript && clang::getObjC3KeyPathInfo(subscript->getKeyExpr()->getType(),info)
      ? syntax : nullptr;
}

struct GenericLambdaBody {
  const clang::FunctionDecl *function;
  std::string key;
};

// Address template values bind declaration and module ownership, never a host
// process address. Fields inherit their enclosing record's ABI owner.
bool AddressDeclarationKey(const clang::ValueDecl *declaration,
                             const clang::ASTContext &context,std::string &key) {
  if (!declaration) return false;
  if (const auto *field=llvm::dyn_cast<clang::FieldDecl>(declaration)) {
    if (!field->getParent()->hasExternalFormalLinkage()) return false;
  } else if (!llvm::isa<clang::FunctionDecl,clang::VarDecl>(declaration) ||
             !declaration->hasExternalFormalLinkage()) return false;
  const auto id=ReadInterfaceDeclarationStableId(declaration);
  if (id.empty()) return false;
  auto add=[&](llvm::StringRef value){key+=std::to_string(value.size())+":"+value.str();};
  add(declaration->getDeclKindName());add(id);add(ReadNativeSymbolName(declaration));
  add(CanonicalType(declaration->getType(),context));
  for (const auto *scope=declaration->getDeclContext();scope;scope=scope->getParent())
    if (const auto *record=llvm::dyn_cast<clang::RecordDecl>(scope)) {
      add(ReadInterfaceDeclarationStableId(record));add(ReadNativeSymbolName(record));
    }
  return true;
}

bool StructuralArgumentKey(const clang::APValue &value,const clang::ASTContext &context,
                           std::string &key,unsigned &budget,unsigned depth=0,
                           std::vector<const clang::NamedDecl *> *references=nullptr) {
  if (!budget || depth > 64) return false;
  --budget;
  auto add = [&](llvm::StringRef text) {
    key += std::to_string(text.size()) + ":" + text.str();
  };
  auto integer = [&](const llvm::APInt &bits) {
    llvm::SmallString<64> text;
    bits.toString(text, 16, false);
    add(std::to_string(bits.getBitWidth()));
    add(text);
  };
  auto element = [&](const clang::APValue &item) {
    return StructuralArgumentKey(item,context,key,budget,depth+1,references);
  };
  add(std::to_string(value.getKind()));
  switch (value.getKind()) {
  case clang::APValue::Int:
    integer(value.getInt());
    return true;
  case clang::APValue::Float:
    // Decimal printing can collapse signed zeros or distinct NaN payloads.
    integer(value.getFloat().bitcastToAPInt());
    return true;
  case clang::APValue::Array:
    add(std::to_string(value.getArraySize()));
    if (value.getArraySize() > budget) return false;
    for (unsigned index = 0; index < value.getArraySize(); ++index)
      if (!element(index < value.getArrayInitializedElts()
                       ? value.getArrayInitializedElt(index)
                       : value.getArrayFiller())) return false;
    return true;
  case clang::APValue::Struct:
    add(std::to_string(value.getStructNumBases()));
    add(std::to_string(value.getStructNumFields()));
    for (unsigned index = 0; index < value.getStructNumBases(); ++index)
      if (!element(value.getStructBase(index))) return false;
    for (unsigned index = 0; index < value.getStructNumFields(); ++index)
      if (!element(value.getStructField(index))) return false;
    return true;
  case clang::APValue::Union:
    if (const auto *field = value.getUnionField()) {
      add(std::to_string(field->getFieldIndex()));
      return element(value.getUnionValue());
    }
    add("no-active-member");
    return true;
  case clang::APValue::LValue: {
    if (value.isNullPointer()) {add("null");return true;}
    const auto *base=value.getLValueBase().dyn_cast<const clang::ValueDecl *>();
    if (!AddressDeclarationKey(base,context,key)) return false;
    if (references) references->push_back(base);
    add(std::to_string(value.getLValueOffset().getQuantity()));
    add(value.isLValueOnePastTheEnd()?"one-past":"within");
    add(value.hasLValuePath()?"path":"no-path");
    if (!value.hasLValuePath()) return true;
    clang::QualType type=base->getType();
    add(std::to_string(value.getLValuePath().size()));
    for (const auto &entry:value.getLValuePath()) {
      if (!budget) return false;
      --budget;
      if (const auto *array=context.getAsArrayType(type)) {
        add("index");add(std::to_string(entry.getAsArrayIndex()));type=array->getElementType();
      } else if (type->isRecordType()) {
        const auto member=entry.getAsBaseOrMember();
        if (const auto *field=llvm::dyn_cast<clang::FieldDecl>(member.getPointer())) {
          add("field");add(ReadInterfaceDeclarationStableId(field));
          type=field->getType();if(references)references->push_back(field);
        } else if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(member.getPointer())) {
          add(member.getInt()?"virtual-base":"base");add(ReadInterfaceDeclarationStableId(record));
          add(ReadNativeSymbolName(record));type=context.getCanonicalTagType(record);
          if(references)references->push_back(record);
        } else return false;
      } else return false;
    }
    return true;
  }
  case clang::APValue::MemberPointer: {
    const auto *member=value.getMemberPointerDecl();
    if (!member) {add("null");return true;}
    if (!AddressDeclarationKey(member,context,key)) return false;
    if(references)references->push_back(member);
    add(value.isMemberPointerToDerivedMember()?"derived":"base");
    add(std::to_string(value.getMemberPointerPath().size()));
    for (const auto *record:value.getMemberPointerPath()) {
      if (!budget) return false;
      --budget;
      add(ReadInterfaceDeclarationStableId(record));add(ReadNativeSymbolName(record));
      if(references)references->push_back(record);
    }
    return true;
  }
  default:
    return false;
  }
}

bool HasAddressTemplateArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context) {
  if (argument.getKind()==clang::TemplateArgument::Declaration &&
      !llvm::isa<clang::TemplateParamObjectDecl>(argument.getAsDecl())) return true;
  const clang::APValue *value=nullptr;
  clang::Expr::EvalResult evaluated;
  if (argument.getKind()==clang::TemplateArgument::Expression) {
    const auto *expression=argument.getAsExpr();
    if (!expression->isValueDependent() && expression->EvaluateAsRValue(evaluated,context) &&
        !evaluated.HasSideEffects) value=&evaluated.Val;
  }
  if (argument.getKind()==clang::TemplateArgument::StructuralValue)value=&argument.getAsStructuralValue();
  if (argument.getKind()==clang::TemplateArgument::Declaration)
    if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))value=&object->getValue();
  if (!value)return false;
  unsigned budget=4096;std::string key;std::vector<const clang::NamedDecl *> references;
  StructuralArgumentKey(*value,context,key,budget,0,&references);
  return !references.empty();
}

bool GenericArgumentKey(const clang::TemplateArgument &argument,
                         const clang::ASTContext &context,std::string &key,
                         unsigned depth=0) {
  if (depth>64 || argument.isDependent()) return false;
  auto add=[&](llvm::StringRef value) {key+=std::to_string(value.size())+":"+value.str();};
  auto type=[&](clang::QualType value,bool allow_factory=false) {
    if (value.isNull() || value->isDependentType())return false;
    const auto origins=FactoryTypeKey(value,context);
    const auto members=UnnamedTemplateTypeKey(value,context);
    if(value->hasUnnamedOrLocalType() || !origins.empty() || !members.empty())
      if(!allow_factory || (origins.empty() && members.empty()) || !HasOnlyProjectedLocalTypes(value))return false;
    add(CanonicalType(value,context));return true;
  };
  add(std::to_string(argument.getKind()));
  switch(argument.getKind()) {
  case clang::TemplateArgument::Type:return type(argument.getAsType(),true);
  case clang::TemplateArgument::Integral: {
    if (!type(argument.getIntegralType(),true)) return false;
    llvm::SmallString<32> value;argument.getAsIntegral().toString(value,10);add(value);return true;
  }
  case clang::TemplateArgument::NullPtr:return type(argument.getNullPtrType(),true);
  case clang::TemplateArgument::Declaration: {
    const auto *declaration=argument.getAsDecl();
    if (const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(declaration)) {
      if (!type(object->getType(),true)) return false;
      unsigned budget=4096;return StructuralArgumentKey(object->getValue(),context,key,budget);
    }
    if (!type(argument.getParamTypeForDecl(),true)) return false;
    return AddressDeclarationKey(declaration,context,key);
  }
  case clang::TemplateArgument::StructuralValue: {
    if (!type(argument.getStructuralValueType(),true)) return false;
    unsigned budget = 4096;
    return StructuralArgumentKey(argument.getAsStructuralValue(),context,key,budget);
  }
  case clang::TemplateArgument::Template: {
    const auto *pattern=argument.getAsTemplate().getAsTemplateDecl();
    if (!pattern || !llvm::isa<clang::ClassTemplateDecl,clang::TypeAliasTemplateDecl,
                              clang::BuiltinTemplateDecl>(pattern)) return false;
    add(pattern->getDeclKindName());
    add(ReadInterfaceDeclarationStableId(pattern));
    add(ReadNativeSymbolName(pattern));
    const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern);
    const auto *member=record?record->getTemplatedDecl():llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext());
    const auto origin=UnnamedTemplateRecordKey(member,context);
    add(origin.empty()?pattern->getQualifiedNameAsString():origin);
    for (const auto *scope=pattern->getDeclContext();scope;scope=scope->getParent())
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(scope)) {
        add(ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate()));
        for (const auto &enclosing:specialization->getTemplateArgs().asArray())
          if (!GenericArgumentKey(enclosing,context,key,depth+1)) return false;
      }
    return true;
  }
  case clang::TemplateArgument::Pack:
    add(std::to_string(argument.pack_size()));
    for (const auto &element:argument.pack_elements())
      if (!GenericArgumentKey(element,context,key,depth+1)) return false;
    return true;
  default:return false;
  }
}

std::string VariablePartialArgumentKey(const clang::VarTemplatePartialSpecializationDecl *partial,
                                       const clang::ASTContext &context) {
  std::string head;llvm::raw_string_ostream stream(head);
  clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
  partial->getTemplateParameters()->print(stream,context,policy);
  for (const auto &argument:partial->getTemplateArgs().asArray()) {
    stream << ";argument=";argument.print(policy,stream,true);
  }
  return ComputeInterfaceSourceSha256(head);
}

bool ClassSpecializationArgumentKey(const clang::ClassTemplateSpecializationDecl *specialization,
                                    const clang::ASTContext &context,std::string &key) {
  if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(specialization)) {
    std::string head;llvm::raw_string_ostream stream(head);
    clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
    partial->getTemplateParameters()->print(stream,context,policy);
    for (const auto &argument:partial->getTemplateArgs().asArray()) {
      stream << ";argument=";argument.print(policy,stream,true);
    }
    key="partial:"+ComputeInterfaceSourceSha256(head);
    return true;
  }
  for (const auto &argument:specialization->getTemplateArgs().asArray())
    if (!GenericArgumentKey(argument,context,key)) return false;
  return true;
}

bool GenericLambdaBodies(const clang::LambdaExpr *expression,
                          const clang::ASTContext &context,
                          std::vector<GenericLambdaBody> &bodies) {
  if (!expression->isGenericLambda()) return true;
  const auto *pattern=expression->getCallOperator()->getDescribedFunctionTemplate();
  if (!pattern) return false;
  std::map<std::string,const clang::FunctionDecl *> ordered;
  for (const auto *specialization:pattern->specializations()) {
    const auto *definition=specialization->getDefinition();
    const auto *arguments=specialization->getTemplateSpecializationArgs();
    if (!definition || !arguments || definition->getType()->isDependentType() || ordered.size()>=256)
      return false;
    std::string key;
    for (const auto &argument:arguments->asArray())
      if (!GenericArgumentKey(argument,context,key)) return false;
    const auto [found,inserted]=ordered.emplace(key,definition);
    if (!inserted && found->second->getCanonicalDecl()!=definition->getCanonicalDecl()) return false;
  }
  for (const auto &[key,function]:ordered) bodies.push_back({function,key});
  return true;
}

const clang::Expr *WrittenDefaultArgument(const clang::ParmVarDecl *parameter) {
  if (!parameter->hasDefaultArg()) return nullptr;
  return parameter->hasUninstantiatedDefaultArg()
      ? parameter->getUninstantiatedDefaultArg() : parameter->getDefaultArg();
}

// ABI ownership binds constraint structure, not defaults or source parameter
// names. Normalized names are used only while printing these constraints.
std::string FunctionTemplateConstraintKey(const clang::FunctionTemplateDecl *pattern) {
  auto &context = pattern->getASTContext();
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.PrintAsCanonical = policy.FullyQualifiedName = true;
  struct Parameters : clang::RecursiveASTVisitor<Parameters> {
    clang::ASTContext &context;
    std::map<const clang::DeclContext *, unsigned> scopes;
    std::map<clang::NamedDecl *, clang::DeclarationName> saved;
    explicit Parameters(clang::ASTContext &context) : context(context) {}
    bool VisitParmVarDecl(clang::ParmVarDecl *parameter) {
      if (saved.count(parameter)) return true;
      auto [scope, inserted] = scopes.emplace(parameter->getDeclContext(), scopes.size());
      saved.emplace(parameter, parameter->getDeclName());
      parameter->setDeclName(&context.Idents.get("p" + std::to_string(scope->second) +
          "_" + std::to_string(parameter->getFunctionScopeIndex())));
      return true;
    }
    bool VisitNamedDecl(clang::NamedDecl *parameter) {
      unsigned depth=0,index=0;
      const char *kind=nullptr;
      if (const auto *type=llvm::dyn_cast<clang::TemplateTypeParmDecl>(parameter)) {
        depth=type->getDepth();index=type->getIndex();kind="type";
      } else if (const auto *value=llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter)) {
        depth=value->getDepth();index=value->getIndex();kind="value";
      } else if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter)) {
        depth=nested->getDepth();index=nested->getIndex();kind="template";
      }
      if (kind && !saved.count(parameter)) {
        saved.emplace(parameter,parameter->getDeclName());
        parameter->setDeclName(&context.Idents.get(std::string("__objc3_parameter_")+kind+"_"+
            std::to_string(depth)+"_"+std::to_string(index)));
      }
      return true;
    }
    bool VisitDeclRefExpr(clang::DeclRefExpr *expression) {
      if (auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(expression->getDecl()))
        VisitParmVarDecl(parameter);
      return true;
    }
    ~Parameters() {
      for (auto &[parameter, name] : saved) parameter->setDeclName(name);
    }
  } parameters(context);
  for (auto *parameter : pattern->getTemplatedDecl()->parameters())
    parameters.VisitParmVarDecl(parameter);
  std::vector<const clang::Expr *> constraints;
  auto heads=[&](auto &&self,const clang::TemplateParameterList *list)->void {
    for (const auto *parameter:*list) {
      parameters.TraverseDecl(const_cast<clang::NamedDecl *>(parameter));
      if (const auto *type=llvm::dyn_cast<clang::TemplateTypeParmDecl>(parameter))
        constraints.push_back(type->getTypeConstraint()
            ?type->getTypeConstraint()->getImmediatelyDeclaredConstraint():nullptr);
      else if (const auto *value=llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter))
        constraints.push_back(value->getPlaceholderTypeConstraint());
      else if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter)) {
        constraints.push_back(nullptr);self(self,nested->getTemplateParameters());
      }
    }
    constraints.push_back(list->getRequiresClause());
  };
  heads(heads,pattern->getTemplateParameters());
  constraints.push_back(pattern->getTemplatedDecl()->getTrailingRequiresClause().ConstraintExpr);
  for (const auto *expression : constraints)
    parameters.TraverseStmt(const_cast<clang::Expr *>(expression));
  std::string key;
  llvm::raw_string_ostream stream(key);
  for (const auto *expression : constraints) {
    stream << "constraint{";
    if (expression) expression->printPretty(stream, nullptr, policy);
    stream << '}';
  }
  return ComputeInterfaceSourceSha256(key);
}

bool ContainsUnnamedLocalType(const clang::Stmt *statement);

// Unnamed member records inherit their template owner's ABI and lexical
// declaration order. Physical source coordinates are never public identity.
std::string UnnamedTemplateRecordKey(const clang::CXXRecordDecl *record,
                                     const clang::ASTContext &context) {
  const auto *owner=ClassTemplateOwner(record);
  if (!owner) return {};
  std::vector<std::string> path;
  bool unnamed=false;
  for (unsigned depth=0;record && depth<64;++depth) {
    if (record->isLambda() || record->isLocalClass()) return {};
    const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(record);
    const auto *pattern=specialization?specialization->getSpecializedTemplate():record->getDescribedClassTemplate();
    std::string arguments;
    if (specialization && !ClassSpecializationArgumentKey(specialization,context,arguments)) return {};
    if (pattern && pattern->getCanonicalDecl()==owner->getCanonicalDecl()) {
      if (!unnamed) return {};
      std::string key=ReadInterfaceDeclarationStableId(owner)+":"+ReadNativeSymbolName(owner)+":"+arguments;
      for (auto i=path.rbegin();i!=path.rend();++i)key+=";"+*i;
      return key;
    }
    if (!record->getIdentifier() && !record->getTypedefNameForAnonDecl()) {
      const auto *parent=llvm::dyn_cast<clang::CXXRecordDecl>(record->getDeclContext());
      if (!parent) return {};
      unsigned index=0;bool found=false;
      for (const auto *member:parent->decls()) {
        const auto *sibling=llvm::dyn_cast<clang::CXXRecordDecl>(member);
        if (!sibling || sibling->isImplicit() || sibling->getIdentifier() ||
            sibling->getTypedefNameForAnonDecl()) continue;
        if (sibling->getCanonicalDecl()==record->getCanonicalDecl()) {found=true;break;}
        ++index;
      }
      if (!found) return {};
      unnamed=true;path.push_back("unnamed:"+std::to_string(index)+":"+std::to_string(static_cast<unsigned>(record->getTagKind())));
    } else path.push_back("named:"+record->getNameAsString()+":"+arguments);
    record=llvm::dyn_cast<clang::CXXRecordDecl>(record->getDeclContext());
  }
  return {};
}

std::string UnnamedTemplateTagKey(const clang::TagDecl *tag,const clang::ASTContext &context) {
  if(const auto *record=llvm::dyn_cast_or_null<clang::CXXRecordDecl>(tag))
    return UnnamedTemplateRecordKey(record,context);
  const auto *enumeration=llvm::dyn_cast_or_null<clang::EnumDecl>(tag);
  if(!enumeration)return {};
  const auto *parent=llvm::dyn_cast<clang::CXXRecordDecl>(enumeration->getDeclContext());
  auto origin=UnnamedTemplateRecordKey(parent,context);
  if(origin.empty())return {};
  unsigned ordinal=0;
  for(const auto *member:parent->decls())
    if(const auto *sibling=llvm::dyn_cast<clang::EnumDecl>(member)) {
      if(sibling->isImplicit())continue;
      if(sibling->getCanonicalDecl()==enumeration->getCanonicalDecl())
        return origin+";enum:"+std::to_string(ordinal)+":"+enumeration->getNameAsString();
      ++ordinal;
    }
  return {};
}

std::string UnnamedTemplateTypeKey(clang::QualType type,const clang::ASTContext &context) {
  struct Origins:clang::RecursiveASTVisitor<Origins> {
    const clang::ASTContext &context;std::string key;
    explicit Origins(const clang::ASTContext &context):context(context) {}
    bool TraverseType(clang::QualType type,bool qualifier=true) {
      return type.isNull() || clang::RecursiveASTVisitor<Origins>::TraverseType(type.getCanonicalType(),qualifier);
    }
    bool TraverseTemplateArgument(const clang::TemplateArgument &argument) {
      if(argument.getKind()==clang::TemplateArgument::Integral)return TraverseType(argument.getIntegralType());
      if(argument.getKind()==clang::TemplateArgument::NullPtr)return TraverseType(argument.getNullPtrType());
      if(argument.getKind()==clang::TemplateArgument::StructuralValue)return TraverseType(argument.getStructuralValueType());
      if(argument.getKind()==clang::TemplateArgument::Declaration) {
        if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))return TraverseType(object->getType());
        return TraverseType(argument.getParamTypeForDecl());
      }
      if(argument.getKind()==clang::TemplateArgument::Template || argument.getKind()==clang::TemplateArgument::TemplateExpansion) {
        if(const auto *pattern=argument.getAsTemplateOrTemplatePattern().getAsTemplateDecl()) {
          const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern);
          const auto *member=record?record->getTemplatedDecl():llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext());
          const auto origin=UnnamedTemplateRecordKey(member,context);
          if(!origin.empty())key+=std::to_string(origin.size())+":"+origin;
        }
        return true;
      }
      return clang::RecursiveASTVisitor<Origins>::TraverseTemplateArgument(argument);
    }
    bool VisitTagType(clang::TagType *type) {
      const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(type->getDecl());
      const auto origin=UnnamedTemplateTagKey(type->getDecl(),context);
      if(!origin.empty())key+=std::to_string(origin.size())+":"+origin;
      if(const auto *specialization=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(record))
        for(const auto &argument:specialization->getTemplateArgs().asArray())TraverseTemplateArgument(argument);
      return true;
    }
  } visitor(context);
  if(!type.isNull())visitor.TraverseType(type);
  return visitor.key;
}

class DeclarationExpressionIdentity final
    : public clang::RecursiveASTVisitor<DeclarationExpressionIdentity> {
public:
  explicit DeclarationExpressionIdentity(const clang::ASTContext &context,
                                         const clang::FunctionDecl *function = nullptr)
      : context_(context), function_(function) {
    if (function) {
      for (const auto *parameter : function->parameters()) Local(parameter);
      for (const auto *redecl : function->redecls())
        if (redecl->getNumParams()==function->getNumParams())
          for (unsigned i=0;i<function->getNumParams();++i)
            locals_.emplace(redecl->getParamDecl(i)->getCanonicalDecl(),
                locals_.at(function->getParamDecl(i)->getCanonicalDecl()));
      OuterTemplateParameters(function);
      if (const auto *pattern = function->getDescribedFunctionTemplate()) {
        for (const auto *parameter : *pattern->getTemplateParameters()) Local(parameter);
        for (const auto *redecl : pattern->redecls())
          if (redecl->getTemplateParameters()->size()==pattern->getTemplateParameters()->size())
            for (unsigned i=0;i<pattern->getTemplateParameters()->size();++i)
              locals_.emplace(redecl->getTemplateParameters()->getParam(i)->getCanonicalDecl(),
                  locals_.at(pattern->getTemplateParameters()->getParam(i)->getCanonicalDecl()));
      }
    }
  }
  void OuterTemplateParameters(const clang::DeclaratorDecl *declaration) {
    auto bind=[&](auto &&self,const clang::TemplateParameterList *head)->void {
      for (const auto *parameter:*head) {
        Local(parameter);
        if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter))
          self(self,nested->getTemplateParameters());
      }
    };
    for (const auto *head:declaration->getTemplateParameterLists()) bind(bind,head);
  }
  void TemplateParameters(const clang::TemplateDecl *pattern) {
    auto bind=[&](auto &&self,const clang::TemplateParameterList *parameters)->void {
      for (const auto *parameter:*parameters) {
        Local(parameter);
        if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter))
          self(self,nested->getTemplateParameters());
      }
    };
    bind(bind,pattern->getTemplateParameters());
    auto alias=[&](auto &&self,const clang::TemplateParameterList *from,
                   const clang::TemplateParameterList *to)->void {
      if (from->size()!=to->size()) return;
      for (unsigned i=0;i<from->size();++i) {
        locals_.emplace(from->getParam(i)->getCanonicalDecl(),
                        locals_.at(to->getParam(i)->getCanonicalDecl()));
        if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(from->getParam(i)))
          if (const auto *target=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(to->getParam(i)))
            self(self,nested->getTemplateParameters(),target->getTemplateParameters());
      }
    };
    if (const auto *redeclarable=llvm::dyn_cast<clang::RedeclarableTemplateDecl>(pattern))
      for (const auto *redecl:redeclarable->redecls())
        alias(alias,redecl->getTemplateParameters(),pattern->getTemplateParameters());
  }
  void Local(const clang::NamedDecl *declaration) {
    const auto *canonical = declaration->getCanonicalDecl();
    if (!locals_.count(canonical)) locals_.emplace(canonical, next_local_slot_++);
  }
  std::string ExpressionType(clang::QualType type) {
    // Dependent init-captures retain an untyped ParenListExpr until native
    // instantiation. Bind that syntax and its children without inventing a type.
    if (type.isNull()) return "<untyped-dependent-expression>";
    const auto *specialization=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(type->getAsCXXRecordDecl());
    if(ClosedClassArguments(specialization))
      // Template spelling sugar may differ for a copied capture after replay.
      // Bind both forms to the same primary owner and converted arguments;
      // the canonical spelling below still preserves value qualifiers.
      TraverseType(context_.getCanonicalTagType(specialization));
    else TraverseType(type);
    clang::PrintingPolicy policy(context_.getLangOpts());
    policy.PrintAsCanonical = true;
    policy.FullyQualifiedName = true;
    policy.SuppressScope = false;
    policy.SuppressUnwrittenScope = false;
    policy.AnonymousTagNameStyle = static_cast<unsigned>(clang::PrintingPolicy::AnonymousTagMode::Plain);
    return type.getCanonicalType().getAsString(policy);
  }
  bool ClosedClassArguments(const clang::ClassTemplateSpecializationDecl *specialization) const {
    if (!specialization) return false;
    const auto *pattern = specialization->getSpecializedTemplate();
    const auto *owner = TemplatePublicationOwner(pattern);
    if (!owner) owner=pattern;
    // An unnamed concrete intermediate record has no independent ABI tag.
    // Its named outer template remains the owner after header replay/import.
    if (ReadNativeSymbolName(owner).empty())
      if (const auto *outer=ClassTemplateOwner(pattern);outer && !ReadNativeSymbolName(outer).empty())owner=outer;
    // Nested patterns inherit the outer primary's ABI owner. Imported nested
    // declarations have no independent tag and are outside the consumer file.
    if (ReadNativeSymbolName(owner).empty() &&
        !context_.getSourceManager().isWrittenInMainFile(owner->getLocation())) return false;
    for (const auto &argument : specialization->getTemplateArgs().asArray()) {
      std::string key;
      if (!GenericArgumentKey(argument,context_,key)) return false;
    }
    return true;
  }
  bool TraverseDeclRefExpr(clang::DeclRefExpr *expression, DataRecursionQueue * = nullptr) {
    if (ClosedClassArguments(llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(
            expression->getDecl()->getDeclContext())))
      // The exact converted class arguments are bound by Reference. Canonical
      // replay can replace a named constant in the qualifier with its value.
      return WalkUpFromDeclRefExpr(expression);
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseDeclRefExpr(expression);
  }
  bool TraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc location,
                                             bool traverse_qualifier) {
    if (const auto *record = location.getType()->getAsCXXRecordDecl();
        ClosedClassArguments(llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(record)))
      return TraverseType(context_.getCanonicalTagType(record));
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseTemplateSpecializationTypeLoc(location,traverse_qualifier);
  }
  bool VisitTagType(clang::TagType *type) {
    const auto *tag=type->getDecl();
    if (const auto *specialization = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(tag);
        ClosedClassArguments(specialization)) Reference(specialization);
    const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(tag);
    if (auto origin=UnnamedTemplateRecordKey(record,context_);!origin.empty()) {
      Add("unnamed-template-record");Add(origin);
    }
    if (!tag->getIdentifier() && IsExpressionLocalTag(tag) && !(record && record->isLambda())) {
      Local(tag);
      Add("unnamed-local-type:"+std::to_string(locals_.at(tag->getCanonicalDecl())));
    }
    return true;
  }
  bool VisitRecordType(clang::RecordType *type) {
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(type->getDecl());
        record && record->isLambda()) {
      Local(record);
      Add("closure-type:" + std::to_string(locals_.at(record->getCanonicalDecl())));
    }
    return true;
  }
  bool VisitLambdaExpr(clang::LambdaExpr *expression) {
    Local(expression->getLambdaClass());
    if (const auto *pattern=expression->getCallOperator()->getDescribedFunctionTemplate())
      for (const auto *parameter:*pattern->getTemplateParameters()) Local(parameter);
    Add("lambda-capture-default:" + std::to_string(expression->getCaptureDefault()));
    for (const auto &capture : expression->captures()) {
      Add("capture:" + std::to_string(capture.getCaptureKind()));
      if (capture.capturesVariable()) Local(capture.getCapturedVar());
    }
    return true;
  }
  bool VisitRequiresExpr(clang::RequiresExpr *expression) {
    for (const auto *parameter : expression->getLocalParameters()) Local(parameter);
    return true;
  }
  bool TraverseLambdaExpr(clang::LambdaExpr *expression,DataRecursionQueue * = nullptr) {
    lambda_bodies_[expression->getBody()]=expression->getCallOperator();
    if (!clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseLambdaExpr(expression,nullptr))
      return false;
    std::vector<GenericLambdaBody> bodies;
    if (!GenericLambdaBodies(expression,context_,bodies)) {Add("unsupported-generic-lambda");return false;}
    for (const auto &body:bodies) {
      Add("generic-lambda-specialization");Add(body.key);
      for (const auto *parameter:body.function->parameters()) Local(parameter);
      Add(ExpressionType(body.function->getType()));
      auto *statement=const_cast<clang::Stmt *>(PublishedFunctionBody(body.function));
      lambda_bodies_[statement]=body.function;
      if (!TraverseStmt(statement)) return false;
    }
    return true;
  }
  bool TraverseStmt(clang::Stmt *statement,DataRecursionQueue *queue=nullptr) {
    const auto *previous=function_;
    auto restore=llvm::scope_exit([&]{function_=previous;});
    if(auto found=lambda_bodies_.find(statement);found!=lambda_bodies_.end()) {
      function_=found->second;
      if (auto contract=CxxCoroutineContract(function_,context_); !contract.empty()) Add(contract);
      statement=const_cast<clang::Stmt *>(PublishedFunctionBody(function_));
    }
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseStmt(statement,queue);
  }
  bool VisitTagDecl(clang::TagDecl *declaration) {
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
        record && record->isLambda()) return true;
    if (IsExpressionLocalTag(declaration)) {
      Local(declaration);
      Add("local-tag:" + std::to_string(locals_.at(declaration->getCanonicalDecl())));
      Add(declaration->clang::Decl::getDeclKindName());
      Add(declaration->getNameAsString());
    }
    return true;
  }
  bool VisitBlockExpr(clang::BlockExpr *expression) {
    const auto *block = expression->getBlockDecl();
    Add("block-this:" + std::to_string(block->capturesCXXThis()));
    Add("block-noescape:" + std::to_string(block->doesNotEscape()));
    for (const auto *parameter : block->parameters()) {
      Local(parameter); Add(ExpressionType(parameter->getType()));
    }
    for (const auto &capture : block->captures()) {
      Reference(capture.getVariable());
      Add("block-capture:" + std::to_string(static_cast<unsigned>(capture.getObjC3CaptureKind())));
      Add("block-byref:" + std::to_string(capture.isByRef()));
      Add("block-nested:" + std::to_string(capture.isNested()));
      Add("block-resource:" + std::to_string(capture.isObjC3ResourceCapture()));
      Add(ExpressionType(capture.getCaptureType()));
    }
    return true;
  }
  bool VisitStmt(clang::Stmt *statement) {
    Add(statement->getStmtClassName()); return true;
  }
  bool TraverseFunctionProtoTypeLoc(clang::FunctionProtoTypeLoc location,
                                     bool = true) {
    // A function-pointer type can carry source-only parameter declarations.
    // ASTImporter reconstructs that TypeLoc without them. Bind only parameters
    // belonging to an actual function; type-only parameters contribute their
    // semantic type. Block and lambda parameter lists are traversed separately.
    if (!WalkUpFromFunctionProtoTypeLoc(location) ||
        !TraverseTypeLoc(location.getReturnLoc())) return false;
    const auto *type=location.getTypePtr();
    if (const auto *error=location.getObjC3ThrowsPayloadInfo())
      if (!TraverseTypeLoc(error->getTypeLoc())) return false;
    for (unsigned index=0;index<type->getNumParams();++index) {
      auto *parameter=location.getParam(index);
      const auto *owner=parameter ?
          llvm::dyn_cast<clang::FunctionDecl>(parameter->getDeclContext()) : nullptr;
      const bool actual=owner &&
          std::find(owner->param_begin(),owner->param_end(),parameter)!=owner->param_end();
      if (actual ? !TraverseDecl(parameter) : !TraverseType(type->getParamType(index)))
        return false;
    }
    for (auto exception:type->exceptions())
      if (!TraverseType(exception)) return false;
    return !type->getNoexceptExpr() || TraverseStmt(type->getNoexceptExpr());
  }
  bool VisitVarDecl(clang::VarDecl *declaration) {
    if (declaration->isLocalVarDeclOrParm()) Local(declaration);
    Add(ExpressionType(declaration->getType()));
    Add("storage:" + std::to_string(declaration->getStorageClass()));
    Add("thread-storage:" + std::to_string(declaration->getTSCSpec()));
    Add("block-byref:" + std::to_string(declaration->hasAttr<clang::BlocksAttr>()));
    Add("init-style:" + std::to_string(declaration->getInitStyle()));
    if (!llvm::isa<clang::ParmVarDecl>(declaration))
      Add("nrvo:" + std::to_string(declaration->isNRVOVariable()));
    return true;
  }
  void Add(llvm::StringRef value) {
    bytes_ += std::to_string(value.size()) + ":" + value.str();
  }
  bool VisitExpr(clang::Expr *expression) {
    Add(expression->getStmtClassName());
    Add(ExpressionType(expression->getType()));
    Add(std::to_string(expression->getValueKind()));
    return true;
  }
  bool VisitCastExpr(clang::CastExpr *expression) {
    Add(expression->getCastKindName());
    return true;
  }
  bool VisitDeclRefExpr(clang::DeclRefExpr *expression) {
    Reference(expression->getDecl()); return true;
  }
  bool UnresolvedReferences(clang::OverloadExpr *expression) {
    std::map<std::string, const clang::NamedDecl *> candidates;
    for (const auto *declaration : expression->decls()) {
      const auto *target = declaration->getUnderlyingDecl();
      candidates.emplace(ReadInterfaceDeclarationStableId(target) + ":" +
                         ReadNativeSymbolName(target), target);
    }
    for (const auto &[key, target] : candidates) {
      Add("unresolved-lookup-candidate");
      Add(key);
      Reference(target);
    }
    return true;
  }
  bool VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr *expression) {
    return UnresolvedReferences(expression);
  }
  bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr *expression) {
    return UnresolvedReferences(expression);
  }
  bool VisitMemberExpr(clang::MemberExpr *expression) {
    Reference(expression->getMemberDecl()); return true;
  }
  bool VisitCXXConstructExpr(clang::CXXConstructExpr *expression) {
    Reference(expression->getConstructor()); return true;
  }
  bool VisitCXXNewExpr(clang::CXXNewExpr *expression) {
    Reference(expression->getOperatorNew());
    Reference(expression->getOperatorDelete());
    return true;
  }
  bool VisitCXXDeleteExpr(clang::CXXDeleteExpr *expression) {
    Reference(expression->getOperatorDelete()); return true;
  }
  bool VisitObjCMessageExpr(clang::ObjCMessageExpr *expression) {
    Reference(expression->getMethodDecl()); return true;
  }
  bool TraverseConceptReference(clang::ConceptReference *reference) {
    if (reference) Reference(reference->getNamedConcept());
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseConceptReference(reference);
  }
  void Reference(const clang::NamedDecl *declaration) {
    if (!declaration) { Add("<none>"); return; }
    if (llvm::isa<clang::ConceptDecl>(declaration)) {
      Add("concept-reference");
      Add(ReadInterfaceDeclarationStableId(declaration));
      Add(ReadCxxConceptOwner(declaration));
      return;
    }
    if (const auto *object = llvm::dyn_cast<clang::TemplateParamObjectDecl>(declaration)) {
      Add("structural-template-object");
      Add(ExpressionType(object->getType()));
      unsigned budget = 4096;
      std::string key;
      if (!StructuralArgumentKey(object->getValue(),context_,key,budget))
        Add("unsupported-structural-value");
      Add(key);
      return;
    }
    // Callee types expose specialized noexcept expressions before their
    // lambda or public template syntax is traversed. Bind those parameters
    // lexically, independently of producer source locations.
    if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(declaration))
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(parameter->getDeclContext());
          function && (function->getDescribedFunctionTemplate() || function->getPrimaryTemplate()))
        Local(parameter);
    if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(declaration))
      if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(parameter->getDeclContext());
          method && method->getParent()->isLambda()) Local(parameter);
    if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration);
        tag && IsExpressionLocalTag(tag)) Local(tag);
    if (const auto found = locals_.find(declaration->getCanonicalDecl()); found != locals_.end()) {
      Add("local:" + std::to_string(found->second)); return;
    }
    if (const auto *owner = llvm::dyn_cast<clang::TagDecl>(declaration->getDeclContext());
        owner && (IsExpressionLocalTag(owner) ||
                  (llvm::isa<clang::CXXRecordDecl>(owner) &&
                   llvm::cast<clang::CXXRecordDecl>(owner)->isLambda()))) {
      Local(owner);
      Add("local-type-member:" + std::to_string(locals_.at(owner->getCanonicalDecl())));
      Add(declaration->getDeclKindName());
      // Native lambda constructor/destructor names contain physical locations.
      // Their operator/type contract already distinguishes closure members.
      const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(owner);
      const bool unnamed_structor=record && !record->getIdentifier() &&
          llvm::isa<clang::CXXConstructorDecl,clang::CXXDestructorDecl>(declaration);
      if ((!record || !record->isLambda()) && !unnamed_structor)
        Add(declaration->getNameAsString());
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
        Add(std::to_string(function->getOverloadedOperator()));
        Add(ExpressionType(function->getType()));
      } else if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration)) {
        unsigned index = 0;
        for (const auto *member : llvm::cast<clang::RecordDecl>(owner)->fields()) {
          if (member == field) break;
          ++index;
        }
        Add(std::to_string(index));
      } else if (const auto *constant = llvm::dyn_cast<clang::EnumConstantDecl>(declaration)) {
        llvm::SmallString<32> value;
        constant->getInitVal().toString(value, 10);
        Add(value);
      }
      return;
    }
    const auto *record_reference=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
    if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))record_reference=pattern->getTemplatedDecl();
    if (!record_reference)record_reference=llvm::dyn_cast<clang::CXXRecordDecl>(declaration->getDeclContext());
    if (auto origin=UnnamedTemplateRecordKey(record_reference,context_);!origin.empty()) {
      Add("unnamed-template-record-member");Add(origin);Add(declaration->getDeclKindName());
      if (!llvm::isa<clang::CXXConstructorDecl,clang::CXXDestructorDecl>(declaration))Add(declaration->getNameAsString());
      const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
      if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) {
        function=pattern->getTemplatedDecl();Add(FunctionTemplateConstraintKey(pattern));
      }
      if (function) {
        Add(ExpressionType(function->getType()));
        if (const auto *arguments=function->getTemplateSpecializationArgs())
          for (const auto &argument:arguments->asArray()) {
            std::string key;
            if (GenericArgumentKey(argument,context_,key))Add(key);
          }
      }
      return;
    }
    for (const clang::NamedDecl *owner=declaration; owner;
         owner=llvm::dyn_cast<clang::NamedDecl>(owner->getDeclContext())) {
      const auto *class_specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(owner);
      if (!class_specialization) continue;
      std::vector<std::string> keys;
      bool complete = true;
      for (const auto &argument : class_specialization->getTemplateArgs().asArray()) {
        std::string key;
        if (!GenericArgumentKey(argument, context_, key)) {complete=false;break;}
        keys.push_back(std::move(key));
      }
      if (complete) {
        Add("class-template-specialization");
        Add(ReadInterfaceDeclarationStableId(class_specialization->getSpecializedTemplate()));
        Add(ReadNativeSymbolName(class_specialization->getSpecializedTemplate()));
        for (const auto &key : keys) Add(key);
      }
    }
    if (const auto *variable = llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration)) {
      if (IsExplicitNamespaceVariableSpecialization(variable)) {
        Add("explicit-variable-specialization");
        Add(ReadInterfaceDeclarationStableId(variable));Add(ReadNativeSymbolName(variable));
      }
      std::vector<std::string> keys;
      bool complete = true;
      for (const auto &argument : variable->getTemplateArgs().asArray()) {
        std::string key;
        if (!GenericArgumentKey(argument, context_, key)) { complete = false; break; }
        keys.push_back(std::move(key));
      }
      if (complete) {
        const auto *pattern = variable->getSpecializedTemplate();
        Add("variable-template-specialization");
        Add(ReadInterfaceDeclarationStableId(pattern));
        Add(ReadNativeSymbolName(pattern));
        for (const auto &key : keys) Add(key);
        return;
      }
    }
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
      if (const auto *pattern = function->getPrimaryTemplate()) {
        if (IsExplicitNamespaceFunctionSpecialization(function)) {
          Add("explicit-function-specialization");
          Add(ReadInterfaceDeclarationStableId(function));Add(ReadNativeSymbolName(function));
        }
        std::vector<std::string> keys;
        const auto *arguments = function->getTemplateSpecializationArgs();
        bool complete = arguments != nullptr;
        if (arguments)
          for (const auto &argument : arguments->asArray()) {
            std::string key;
            if (!GenericArgumentKey(argument, context_, key)) { complete=false; break; }
            keys.push_back(std::move(key));
          }
        if (complete) {
          Add("function-template-specialization");
          Add(ReadInterfaceDeclarationStableId(pattern));
          Add(ReadNativeSymbolName(pattern));
          for (const auto &key : keys) Add(key);
          return;
        }
      }
    Add(ReadInterfaceDeclarationStableId(declaration));
    Add(ReadNativeSymbolName(declaration));
  }
  bool TraverseCoreturnStmt(clang::CoreturnStmt *statement,
                            DataRecursionQueue * = nullptr) {
    const auto *type=clang::getObjC3CallableFunctionType(function_);
    if ((type && type->getObjC3FunctionEffects().Async) || PublishedCxxCoroutine(function_))
      return WalkUpFromCoreturnStmt(statement) && TraverseStmt(statement->getOperand());
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraverseCoreturnStmt(statement,nullptr);
  }
  bool TraverseCoyieldExpr(clang::CoyieldExpr *expression,
                           DataRecursionQueue * = nullptr) {
    if (PublishedCxxCoroutine(function_))
      return expression->getWrittenOperand() && WalkUpFromCoyieldExpr(expression) && TraverseStmt(expression->getWrittenOperand());
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::TraverseCoyieldExpr(expression,nullptr);
  }
  bool TraverseCoawaitExpr(clang::CoawaitExpr *expression,
                           DataRecursionQueue * = nullptr) {
    if (PublishedCxxCoroutine(function_))
      return WalkUpFromCoawaitExpr(expression) && TraverseStmt(expression->getOperand());
    if (const auto *source=PublishedAwaitSource(expression,function_,context_))
      return WalkUpFromCoawaitExpr(expression) &&
             TraverseStmt(const_cast<clang::Expr *>(source));
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraverseCoawaitExpr(expression,nullptr);
  }
  bool TraversePseudoObjectExpr(clang::PseudoObjectExpr *expression,
                                DataRecursionQueue * = nullptr) {
    if (const auto *source=clang::getObjC3IsolationSourceSyntax(expression))
      return TraverseStmt(const_cast<clang::Expr *>(source));
    clang::ObjC3KeyPathApplicationInfo application;
    if (clang::getObjC3KeyPathApplication(expression,application)) {
      Add("keypath-application:"+std::to_string(application.Operation));
      Add(ExpressionType(expression->getType()));
      for (auto *argument:application.Syntax->arguments())
        if (!TraverseStmt(const_cast<clang::Expr *>(argument))) return false;
      return true;
    }
    if (auto *syntax=KeyPathSubscriptSyntax(expression)) {
      Add("keypath-subscript");Add(ExpressionType(expression->getType()));
      return TraverseStmt(syntax);
    }
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraversePseudoObjectExpr(expression,nullptr);
  }
  bool TraverseOpaqueValueExpr(clang::OpaqueValueExpr *expression,
                               DataRecursionQueue * = nullptr) {
    return TraverseStmt(expression->getSourceExpr());
  }
  bool TraverseCompoundLiteralExpr(clang::CompoundLiteralExpr *expression,
                                   DataRecursionQueue * = nullptr) {
    if (const auto *path=clang::getObjC3KeyPathLiteralPath(expression)) {
      Add("keypath-literal");Add(ExpressionType(expression->getType()));Add(path->getString());
      return true;
    }
    return clang::RecursiveASTVisitor<DeclarationExpressionIdentity>::
        TraverseCompoundLiteralExpr(expression,nullptr);
  }
  // Native printers insert protective parentheses for carrier operators. A
  // reparse retains those ParenExpr nodes; they do not change typed semantics.
  bool TraverseParenExpr(clang::ParenExpr *expression,
                         DataRecursionQueue * = nullptr) {
    return TraverseStmt(expression->getSubExpr());
  }
  std::string Get(clang::Stmt *expression) {
    clang::PrintingPolicy policy(context_.getLangOpts());
    const bool unnamed=ContainsUnnamedLocalType(expression);
    // Canonical types erase owned-tag spelling. Keep complete unnamed local
    // definitions in the text binding; typed references use lexical slots.
    policy.PrintAsCanonical = !unnamed;
    policy.IncludeTagDefinition = unnamed;
    policy.AnonymousTagNameStyle = static_cast<unsigned>(
        clang::PrintingPolicy::AnonymousTagMode::Plain);
    std::string printed;
    llvm::raw_string_ostream stream(printed);
    struct CanonicalParens final : clang::PrinterHelper {
      clang::PrintingPolicy policy;
      explicit CanonicalParens(clang::PrintingPolicy policy) : policy(policy) {}
      bool handledStmt(clang::Stmt *statement, llvm::raw_ostream &out) override {
        if (const auto *paren = llvm::dyn_cast<clang::ParenExpr>(statement)) {
          paren->getSubExpr()->printPretty(out, this, policy);
          return true;
        }
        // Native declaration replay may qualify an unqualified static member
        // reference. Bind both spellings to the same declaration spelling;
        // the typed traversal below still binds its owner and exact arguments.
        if (const auto *reference=llvm::dyn_cast<clang::DeclRefExpr>(statement)) {
          // Detached specialization replay qualifies ordinary namespace calls.
          // Keep the spelling independent of that lexical replay context; the
          // typed traversal binds the selected overload and its native symbol.
          if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(reference->getDecl());
              function && function->getIdentifier() && function->getDeclContext()->isFileContext() &&
              !function->getPrimaryTemplate() && !function->getDescribedFunctionTemplate() &&
              !reference->hasExplicitTemplateArgs()) {
            function->printQualifiedName(out,policy);
            return true;
          }
          if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(reference->getDecl());
              variable && variable->isStaticDataMember() && variable->getIdentifier() &&
              !variable->getDescribedVarTemplate() &&
              !llvm::isa<clang::VarTemplateSpecializationDecl>(variable)) {
            variable->printQualifiedName(out,policy);
            return true;
          }
        }
        return false;
      }
    } helper(policy);
    expression->printPretty(stream, &helper, policy);
    Add(printed);
    TraverseStmt(expression);
    llvm::SHA256 hash; hash.update(bytes_);
    return llvm::toHex(hash.final(),true);
  }
private:
  const clang::ASTContext &context_;
  const clang::FunctionDecl *function_;
  std::map<const clang::Stmt *,const clang::FunctionDecl *> lambda_bodies_;
  std::string bytes_;
  std::map<const clang::Decl *, unsigned> locals_;
  unsigned next_local_slot_ = 0;
};

std::string DeclarationExpressionContract(const clang::Expr *expression,
                                           const clang::ASTContext &context) {
  return DeclarationExpressionIdentity(context).Get(const_cast<clang::Expr *>(expression));
}

std::string FunctionTemplateHeadContract(const clang::TemplateDecl *pattern,
                                         const clang::ASTContext &context) {
  const auto *function_pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(pattern);
  const auto *function = function_pattern ? function_pattern->getTemplatedDecl() : nullptr;
  DeclarationExpressionIdentity identity(context, function);
  identity.TemplateParameters(pattern);
  auto expression = [&](const clang::Expr *value) {
    return value ? identity.Get(const_cast<clang::Expr *>(value)) : std::string("none");
  };
  std::string head;
  auto field = [&](llvm::StringRef value) { head += std::to_string(value.size()) + ":" + value.str(); };
  auto parameters=[&](auto &&self,const clang::TemplateParameterList *list)->void {
    field(std::to_string(list->size()));
    for (const auto *parameter:*list) {
    field(parameter->getDeclKindName());
    if (const auto *type = llvm::dyn_cast<clang::TemplateTypeParmDecl>(parameter)) {
      field(type->isParameterPack() ? "pack" : "single");
      field(type->getTypeConstraint()
          ? expression(type->getTypeConstraint()->getImmediatelyDeclaredConstraint()) : "none");
      field(type->hasDefaultArgument()
          ? CanonicalType(type->getDefaultArgument().getArgument().getAsType(), context) : "none");
    } else if (const auto *value = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter)) {
      field(value->isParameterPack() ? "pack" : "single");
      field(CanonicalType(value->getType(), context));
      field(expression(value->getPlaceholderTypeConstraint()));
      if (value->hasDefaultArgument()) {
        const auto &argument = value->getDefaultArgument().getArgument();
        if (argument.getKind() == clang::TemplateArgument::Expression)
          field(expression(argument.getAsExpr()));
        else {
          std::string printed;llvm::raw_string_ostream stream(printed);
          clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
          argument.print(policy, stream, true);field(printed);
        }
      } else field("none");
    } else if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter)) {
      field(nested->isParameterPack()?"pack":"single");
      field(std::to_string(nested->templateParameterKind()));
      self(self,nested->getTemplateParameters());
      if (nested->hasDefaultArgument()) {
        const auto &argument=nested->getDefaultArgument().getArgument();
        std::string key;
        if (GenericArgumentKey(argument,context,key)) field(key);
        else {
          std::string printed;llvm::raw_string_ostream stream(printed);
          clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
          argument.print(policy,stream,true);field(printed);
        }
      } else field("none");
    }
    }
    field(expression(list->getRequiresClause()));
  };
  parameters(parameters,pattern->getTemplateParameters());
  if (function) {
    field(expression(function->getTrailingRequiresClause().ConstraintExpr));
    for (const auto *parameter : function->parameters())
      field(expression(WrittenDefaultArgument(parameter)));
  }
  return ComputeInterfaceSourceSha256(head);
}

std::string ConceptIdentity(const clang::ConceptDecl *declaration,
                            const clang::ASTContext &context) {
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.AnonymousTagNameStyle = static_cast<unsigned>(
      clang::PrintingPolicy::AnonymousTagMode::Plain);
  std::string head;
  llvm::raw_string_ostream stream(head);
  declaration->getTemplateParameters()->print(stream, context, policy);
  DeclarationExpressionIdentity identity(context);
  for (const auto *parameter : *declaration->getTemplateParameters()) identity.Local(parameter);
  return "concept{owner=" + ReadCxxConceptOwner(declaration) +
      ";parameters=" + ComputeInterfaceSourceSha256(head) + ";constraint=" +
      identity.Get(const_cast<clang::Expr *>(declaration->getConstraintExpr())) +
      "};cxx-concepts:v1;";
}

std::string FunctionDefinitionContract(const clang::FunctionDecl *function,
                                        const clang::ASTContext &context) {
  const auto *definition = function->getDefinition();
  if (!definition) return "declaration";
  DeclarationExpressionIdentity identity(context, definition);
  if (const auto *constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(definition))
    for (const auto *initializer : constructor->inits()) {
      if (!initializer->isWritten()) continue;
      if (initializer->isMemberInitializer()) identity.Reference(initializer->getMember());
      else if (initializer->isBaseInitializer())
        identity.Add(CanonicalType(clang::QualType(initializer->getBaseClass(), 0), context));
      identity.Add(identity.Get(initializer->getInit()));
    }
  if (auto contract=CxxCoroutineContract(definition,context); !contract.empty()) identity.Add(contract);
  return identity.Get(const_cast<clang::Stmt *>(PublishedFunctionBody(definition)));
}

std::string NativeAttributeValue(const clang::Attr *attribute,const clang::NamedDecl *declaration) {
  std::string key=std::to_string(attribute->getKind());
  auto add=[&](llvm::StringRef value){key+=";"+std::to_string(value.size())+":"+value.str();};
  if(const auto *alignment=llvm::dyn_cast<clang::AlignedAttr>(attribute)) {
    add(alignment->isAlignas() ? "alignas" : "aligned");
    if(!alignment->isAlignmentDependent())
      add(std::to_string(alignment->getAlignment(declaration->getASTContext())));
    else if(alignment->isAlignmentExpr()) {
      DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
      auto bind_scope=[&](auto &&self,const clang::DeclContext *scope)->void {
        if(!scope)return;
        self(self,scope->getParent());
        if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(scope))
          if(const auto *pattern=record->getDescribedClassTemplate())identity.TemplateParameters(pattern);
        if(const auto *function=llvm::dyn_cast<clang::FunctionDecl>(scope))
          if(const auto *pattern=function->getDescribedFunctionTemplate())identity.TemplateParameters(pattern);
      };
      bind_scope(bind_scope,declaration->getDeclContext());
      if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration))
        if(const auto *pattern=record->getDescribedClassTemplate())identity.TemplateParameters(pattern);
      if(const auto *variable=llvm::dyn_cast<clang::VarDecl>(declaration))
        if(const auto *pattern=variable->getDescribedVarTemplate())identity.TemplateParameters(pattern);
      add(identity.Get(alignment->getAlignmentExpr()));
    } else add(CanonicalType(alignment->getAlignmentType()->getType(),declaration->getASTContext()));
  } else if(const auto *result=llvm::dyn_cast<clang::WarnUnusedResultAttr>(attribute)) {
    add(result->IsCXX11NoDiscard() ? "nodiscard" : "warn-unused-result");add(result->getMessage());
  } else if(const auto *visibility=llvm::dyn_cast<clang::VisibilityAttr>(attribute))
    add(std::to_string(visibility->getVisibility()));
  else if(const auto *visibility=llvm::dyn_cast<clang::TypeVisibilityAttr>(attribute))
    add(std::to_string(visibility->getVisibility()));
  auto parameter=[&](clang::ParamIdx index){add(index.isValid()?std::to_string(index.getASTIndex()):"none");};
  if (const auto *allocator=llvm::dyn_cast<clang::RestrictAttr>(attribute)) {
    if (auto *deallocator=allocator->getDeallocator()) {
      DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
      add(identity.Get(deallocator));parameter(allocator->getDeallocatorPtrArgIndex());
    }
  } else if (const auto *size=llvm::dyn_cast<clang::AllocSizeAttr>(attribute)) {
    parameter(size->getElemSizeParam());parameter(size->getNumElemsParam());
  } else if (const auto *alignment=llvm::dyn_cast<clang::AllocAlignAttr>(attribute)) parameter(alignment->getParamIndex());
  else if (const auto *nonnull=llvm::dyn_cast<clang::NonNullAttr>(attribute)) {
    std::set<unsigned> indices;for(auto index:nonnull->args())indices.insert(index.getASTIndex());
    for(auto index:indices)add(std::to_string(index));
  } else if (const auto *format=llvm::dyn_cast<clang::FormatAttr>(attribute)) {
    add(format->getType()->getName());add(std::to_string(format->getFormatIdx()));add(std::to_string(format->getFirstArg()));
  } else if (const auto *format=llvm::dyn_cast<clang::FormatArgAttr>(attribute)) parameter(format->getFormatIdx());
  else if (const auto *alignment=llvm::dyn_cast<clang::AssumeAlignedAttr>(attribute)) {
    DeclarationExpressionIdentity identity(declaration->getASTContext(),llvm::dyn_cast<clang::FunctionDecl>(declaration));
    auto bind=[&](auto &&self,const clang::DeclContext *scope)->void {
      if(!scope)return;self(self,scope->getParent());
      if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(scope))
        if(const auto *pattern=record->getDescribedClassTemplate())identity.TemplateParameters(pattern);
      if(const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(scope))
        for(auto *parameter:*partial->getTemplateParameters())identity.Local(parameter);
    };
    bind(bind,declaration->getDeclContext());
    add(identity.Get(alignment->getAlignment()));
    add(alignment->getOffset()?identity.Get(alignment->getOffset()):"zero");
  }
  return key;
}

const clang::CXXRecordDecl *FriendOwner(const clang::NamedDecl *declaration) {
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    for (const auto *redecl : function->redecls())
      if (redecl->getFriendObjectKind() != clang::Decl::FOK_None)
        if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(redecl->getLexicalDeclContext()))
          return record;
  return nullptr;
}

bool IsInClassDefaultedComparison(const clang::FunctionDecl *function) {
  if (!function) return false;
  function = function->getMostRecentDecl();
  if (!function->isExplicitlyDefaulted() || !function->getLexicalDeclContext()->isRecord())
    return false;
  const auto effects = ReadCanonicalFunctionEffects(function->getType(), function->getASTContext());
  if (effects.is_async || effects.is_throwing) return false;
  switch (function->getOverloadedOperator()) {
  case clang::OO_EqualEqual: case clang::OO_ExclaimEqual:
  case clang::OO_Less: case clang::OO_LessEqual:
  case clang::OO_Greater: case clang::OO_GreaterEqual:
  case clang::OO_Spaceship: return true;
  default: return false;
  }
}

bool IsConcreteFriend(const clang::FriendDecl *declaration) {
  if (declaration->isPackExpansion() || declaration->getFriendTypeNumTemplateParameterLists())
    return false;
  if (const auto *type = declaration->getFriendType())
    return !type->getType()->isDependentType();
  const auto *function = llvm::dyn_cast_or_null<clang::FunctionDecl>(declaration->getFriendDecl());
  return function && !function->isTemplated() && !function->getTrailingRequiresClause() &&
      (!function->isDefaulted() || IsInClassDefaultedComparison(function));
}

class LambdaExpressionFinder final : public clang::RecursiveASTVisitor<LambdaExpressionFinder> {
public:
  bool VisitLambdaExpr(clang::LambdaExpr *expression) { found = true;generic |= expression->isGenericLambda();return true; }
  bool found = false;
  bool generic = false;
};
bool ContainsLambda(const clang::Stmt *statement) {
  if (!statement) return false;
  LambdaExpressionFinder finder; finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
  return finder.found;
}
template <class Predicate>
bool HasPublishedExpression(const clang::NamedDecl *declaration, const Predicate &contains) {
  for (const auto *member:ClosedParentMemberSpecializations(declaration))
    if (HasPublishedExpression(member,contains)) return true;
  for (const auto *pattern:ClosedParentPatterns(declaration))
    if (HasPublishedExpression(pattern,contains)) return true;
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    const auto *record=pattern->getTemplatedDecl();
    if (const auto *definition=record->getDefinition()) record=definition;
    for (const auto *member : record->decls())
      if (const auto *named = llvm::dyn_cast<clang::NamedDecl>(member))
        if (HasPublishedExpression(named, contains)) return true;
    for (const auto *specialization:PublishedClassSpecializations(pattern))
      if (HasPublishedExpression(specialization,contains)) return true;
    return false;
  }
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    return HasPublishedExpression(pattern->getTemplatedDecl(), contains);
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) {
    for (const auto *partial:PublishedVariablePartials(pattern))
      if (HasPublishedExpression(partial,contains)) return true;
    return HasPublishedExpression(pattern->getTemplatedDecl(), contains);
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
    for (const auto *parameter : function->getMostRecentDecl()->parameters())
      if (const auto *expression = WrittenDefaultArgument(parameter);
          expression && contains(expression)) return true;
    if (PublishesFunctionBody(function))
      if (const auto *definition = function->getDefinition()) {
        if (contains(definition->getBody())) return true;
        if (const auto *constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(definition))
          for (const auto *initializer : constructor->inits())
            if (initializer->isWritten() && contains(initializer->getInit())) return true;
      }
  }
  if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration);
      field && field->hasInClassInitializer()) return contains(field->getInClassInitializer());
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
      PublishesVariableInitializer(variable)) {
    if (const auto *definition=variable->getDefinition()) variable=definition;
    if (variable->hasInit()) return contains(variable->getInit());
  }
  for (const auto *specialization:ConcreteMemberFunctionSpecializations(declaration))
    if (specialization->isInlined() && HasPublishedExpression(specialization,contains)) return true;
  for (const auto *specialization:ConcreteMemberVariableSpecializations(declaration))
    if (HasPublishedExpression(specialization,contains)) return true;
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->getDefinition())
    for (const auto *member : record->getDefinition()->decls()) {
      if (member->isImplicit()) continue;
      const clang::NamedDecl *named = llvm::dyn_cast<clang::NamedDecl>(member);
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member))
        named = friend_decl->getFriendDecl();
      if (named && llvm::isa<clang::FieldDecl, clang::FunctionDecl, clang::VarDecl, clang::CXXRecordDecl, clang::TemplateDecl>(named) &&
          HasPublishedExpression(named, contains)) return true;
    }
  return false;
}
bool HasDeclarationNullableKeyPathWrite(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Literals : clang::RecursiveASTVisitor<Literals> {
      bool found=false;
      bool VisitCompoundLiteralExpr(clang::CompoundLiteralExpr *literal) {
        found |= clang::hasObjC3NullableKeyPathWrite(literal);return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasDeclarationLambda(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration, ContainsLambda);
}
// A direct deduced result belongs to its complete published factory body.
// References/pointers and types originating in other factories remain separate.
bool IsPublishedDeducedFactory(const clang::FunctionDecl *function) {
  if(!function || !function->getDefinition() || !function->getDeclaredReturnType()->getContainedAutoType())return false;
  if(PublishesFunctionBody(function))return true;
  const auto *primary=function->getPrimaryTemplate();
  return primary && function->getTemplateSpecializationKind()!=clang::TSK_ExplicitSpecialization &&
         PublishesFunctionBody(primary->getTemplatedDecl());
}

std::vector<const clang::TagDecl *> PublishedFactoryResults(const clang::FunctionDecl *function) {
  if(!IsPublishedDeducedFactory(function))return {};
  struct Results:clang::RecursiveASTVisitor<Results> {
    const clang::FunctionDecl *function;
    std::vector<const clang::TagDecl *> tags;
    explicit Results(const clang::FunctionDecl *function):function(function) {}
    bool TraverseType(clang::QualType type,bool traverse_qualifier=true) {
      return type.isNull() || clang::RecursiveASTVisitor<Results>::TraverseType(type.getCanonicalType(),traverse_qualifier);
    }
    bool TraverseTemplateArgument(const clang::TemplateArgument &argument) {
      if(argument.getKind()==clang::TemplateArgument::Integral)return TraverseType(argument.getIntegralType());
      if(argument.getKind()==clang::TemplateArgument::NullPtr)return TraverseType(argument.getNullPtrType());
      if(argument.getKind()==clang::TemplateArgument::StructuralValue)return TraverseType(argument.getStructuralValueType());
      if(argument.getKind()==clang::TemplateArgument::Declaration) {
        if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))return TraverseType(object->getType());
        return TraverseType(argument.getParamTypeForDecl());
      }
      return clang::RecursiveASTVisitor<Results>::TraverseTemplateArgument(argument);
    }
    bool VisitTagType(clang::TagType *type) {
      const auto *tag=type->getDecl();
      if(tag->getDefinition() && IsExpressionLocalTag(tag))
        for(const auto *scope=tag->getDeclContext();scope;scope=scope->getParent())
          if(const auto *owner=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
            if(owner->getCanonicalDecl()==function->getCanonicalDecl() &&
                std::none_of(tags.begin(),tags.end(),[&](const auto *prior){return prior->getCanonicalDecl()==tag->getCanonicalDecl();}))tags.push_back(tag);
            break;
          }
      if(const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(tag))
        for(const auto &argument:specialization->getTemplateArgs().asArray())TraverseTemplateArgument(argument);
      return true;
    }
  } visitor(function);
  visitor.TraverseType(function->getReturnType());return visitor.tags;
}

const clang::TagDecl *PublishedFactoryResult(const clang::FunctionDecl *function) {
  const auto results=PublishedFactoryResults(function);
  return results.empty()?nullptr:results.front();
}

const clang::CXXRecordDecl *PublishedClosureResult(const clang::FunctionDecl *function) {
  const auto *record=llvm::dyn_cast_or_null<clang::CXXRecordDecl>(PublishedFactoryResult(function));
  return record && record->isLambda() ? record : nullptr;
}

bool HasDeclarationLocalFactoryResult(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Finder:clang::RecursiveASTVisitor<Finder> {
      bool found=false;
      bool VisitTagDecl(clang::TagDecl *tag) {
        if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(tag);record && record->isLambda())return true;
        for (const auto *scope=tag->getDeclContext();scope;scope=scope->getParent())
          if (const auto *owner=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
            const auto *result=PublishedFactoryResult(owner);
            found |= (result && result->getCanonicalDecl()==tag->getCanonicalDecl()) ||
                (!result && owner->isTemplated() && IsPublishedDeducedFactory(owner));
            break;
          }
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

// Public projections must retain the complete nominal factory origin, not only
// an anonymous spelling or a layout shared by unrelated local declarations.
bool GenericFactoryBindings(const clang::FunctionDecl *function,std::string &key) {
  const auto &context=function->getASTContext();
  auto add=[&](llvm::StringRef value){key+=std::to_string(value.size())+":"+value.str();};
  if(const auto *arguments=function->getTemplateSpecializationArgs()) {
    add("function");
    for(const auto &argument:arguments->asArray())
      if(!GenericArgumentKey(argument,context,key))return false;
  }
  for(const auto *scope=function->getDeclContext();scope;scope=scope->getParent())
    if(const auto *parent=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(scope)) {
      if(parent->isDependentContext())return false;
      add(ReadInterfaceDeclarationStableId(parent->getSpecializedTemplate()));
      for(const auto &argument:parent->getTemplateArgs().asArray())
        if(!GenericArgumentKey(argument,context,key))return false;
    }
  for(const auto *scope=function->getDeclContext();scope;scope=scope->getParent())
    if(const auto *enclosing=llvm::dyn_cast<clang::FunctionDecl>(scope))
      if(const auto *arguments=enclosing->getTemplateSpecializationArgs()) {
        add("enclosing-function");add(ReadInterfaceDeclarationStableId(enclosing));
        for(const auto &argument:arguments->asArray())
          if(!GenericArgumentKey(argument,context,key))return false;
      }
  return true;
}

bool IsWithinFactoryResult(const clang::TagDecl *tag,const clang::TagDecl *result) {
  if(!result)return false;
  for(const clang::DeclContext *scope=tag;scope;scope=scope->getParent()) {
    const auto *enclosing=llvm::dyn_cast<clang::TagDecl>(scope);
    if(!enclosing)return false;
    if(enclosing->getCanonicalDecl()==result->getCanonicalDecl())return true;
  }
  return false;
}

const clang::TagDecl *FactoryResultTagRoot(const clang::FunctionDecl *function,const clang::TagDecl *tag) {
  for(const auto *result:PublishedFactoryResults(function))
    if(IsWithinFactoryResult(tag,result))return result;
  return nullptr;
}

std::string FactoryTagPath(const clang::TagDecl *tag,const clang::TagDecl *root,const clang::ASTContext &context) {
  std::string path;
  for(const clang::DeclContext *scope=tag;scope;scope=scope->getParent()) {
    const auto *part=llvm::cast<clang::TagDecl>(scope);
    const auto name=std::to_string(static_cast<unsigned>(part->getTagKind()))+":"+
        part->getNameAsString()+":"+std::to_string(context.getManglingNumber(part));
    path+=std::to_string(name.size())+":"+name;
    if(part->getCanonicalDecl()==root->getCanonicalDecl())break;
  }
  return path;
}

const clang::FunctionDecl *FactoryResultOrigin(const clang::TagDecl *tag) {
  for(const auto *scope=tag->getDeclContext();scope;scope=scope->getParent())
    if(const auto *function=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
      const clang::FunctionDecl *closure_owner=nullptr;
      if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(function)) {
        if(method->getParent()->isLambda() || IsExpressionLocalTag(method->getParent())) {
          closure_owner=FactoryResultOrigin(method->getParent());
          if(!closure_owner)return nullptr;
        }
      } else if(!IsTopLevel(function))return nullptr;
      const auto *result=FactoryResultTagRoot(function,tag);
      if(!result ||
          function->getType()->isDependentType() || (!closure_owner && !function->isExternallyVisible()))return nullptr;
      bool bound=closure_owner || !ReadNativeSymbolName(function).empty();
      if(const auto *primary=function->getPrimaryTemplate())
        bound |= !ReadNativeSymbolName(primary).empty() || !ReadNativeSymbolName(primary->getTemplatedDecl()).empty();
      if(const auto *owner=TemplatePublicationOwner(function))bound |= !ReadNativeSymbolName(owner).empty();
      std::string bindings;
      return bound && GenericFactoryBindings(function,bindings) ? function : nullptr;
    }
  return nullptr;
}

std::string FactoryOriginKey(const clang::FunctionDecl *function) {
  if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(function);
      method && (method->getParent()->isLambda() || IsExpressionLocalTag(method->getParent()))) {
    const auto *owner=FactoryResultOrigin(method->getParent());
    if(!owner)return {};
    std::string bindings;GenericFactoryBindings(owner,bindings);
    const auto parent=FactoryOriginKey(owner);
    std::string key=std::to_string(parent.size())+":"+parent+":local-call:"+std::to_string(bindings.size())+":"+bindings;
    if(method->getParent()->isLambda())return key+":closure";
    const auto path=FactoryTagPath(method->getParent(),FactoryResultTagRoot(owner,method->getParent()),function->getASTContext());
    key+=":record="+std::to_string(path.size())+":"+path;
    const auto name=method->getNameAsString();key+=":member="+std::to_string(name.size())+":"+name;
    key+=":cv="+std::to_string(method->getMethodQualifiers().getAsOpaqueValue())+":ref="+std::to_string(method->getRefQualifier());
    for(const auto *parameter:function->parameters()) {
      const auto type=CanonicalType(parameter->getType(),function->getASTContext());
      key+=std::to_string(type.size())+":"+type;
    }
    return key;
  }
  return ReadInterfaceDeclarationStableId(function);
}

std::string FactoryTypeKey(clang::QualType type,const clang::ASTContext &context,FactoryTypeFeatures *features) {
  struct Origins:clang::RecursiveASTVisitor<Origins>,FactoryTypeFeatures {
    const clang::ASTContext &context;
    std::string key;
    explicit Origins(const clang::ASTContext &context):context(context) {}
    bool TraverseType(clang::QualType type,bool traverse_qualifier=true) {
      return type.isNull() || clang::RecursiveASTVisitor<Origins>::TraverseType(type.getCanonicalType(),traverse_qualifier);
    }
    bool TraverseTemplateArgument(const clang::TemplateArgument &argument) {
      if(argument.getKind()==clang::TemplateArgument::StructuralValue)
        return TraverseType(argument.getStructuralValueType());
      if(argument.getKind()==clang::TemplateArgument::Declaration) {
        if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))
          return TraverseType(object->getType());
        return TraverseType(argument.getParamTypeForDecl());
      }
      return clang::RecursiveASTVisitor<Origins>::TraverseTemplateArgument(argument);
    }
    bool VisitTagType(clang::TagType *type) {
      const auto *tag=type->getDecl();
      if(const auto *factory=FactoryResultOrigin(tag)) {
        if(const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(tag))closure |= record->isLambda();
        if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(factory))lambda_result |= method->getParent()->isLambda();
        const auto owner=FactoryOriginKey(factory);
        auto identity=owner+":kind="+std::to_string(static_cast<unsigned>(tag->getTagKind()))+
            ":name="+tag->getNameAsString()+":discriminator="+std::to_string(context.getManglingNumber(tag));
        const auto *result=FactoryResultTagRoot(factory,tag);
        if(tag->getCanonicalDecl()!=result->getCanonicalDecl()) {
          nested=true;identity+=":lexical="+FactoryTagPath(tag,result,context);
        }
        const auto *direct=factory->getReturnType()->getAsTagDecl();
        nested |= !direct || direct->getCanonicalDecl()!=result->getCanonicalDecl();
        if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(factory))
          nested |= IsExpressionLocalTag(method->getParent()) && !method->getParent()->isLambda();
        std::string bindings;
        if(!GenericFactoryBindings(factory,bindings))return false;
        if(!bindings.empty()){generic=true;identity+=":bindings="+bindings;}
        key+=std::to_string(identity.size())+":"+identity;
      }
      if(const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(tag))
        for(const auto &argument:specialization->getTemplateArgs().asArray())TraverseTemplateArgument(argument);
      return true;
    }
  } visitor(context);
  if(!type.isNull())visitor.TraverseType(type.getCanonicalType());
  if(features) {
    features->generic |= visitor.generic;
    features->closure |= visitor.closure;
    features->lambda_result |= visitor.lambda_result;
    features->nested |= visitor.nested;
  }
  return visitor.key;
}

bool HasOnlyProjectedLocalTypes(clang::QualType type) {
  struct Qualified:clang::RecursiveASTVisitor<Qualified> {
    bool valid=true;
    bool TraverseType(clang::QualType type,bool traverse_qualifier=true) {
      return type.isNull() || clang::RecursiveASTVisitor<Qualified>::TraverseType(type.getCanonicalType(),traverse_qualifier);
    }
    bool TraverseTemplateArgument(const clang::TemplateArgument &argument) {
      if(argument.getKind()==clang::TemplateArgument::StructuralValue)
        return TraverseType(argument.getStructuralValueType());
      if(argument.getKind()==clang::TemplateArgument::Declaration) {
        if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))
          return TraverseType(object->getType());
        return TraverseType(argument.getParamTypeForDecl());
      }
      return clang::RecursiveASTVisitor<Qualified>::TraverseTemplateArgument(argument);
    }
    bool VisitTagType(clang::TagType *type) {
      const auto *tag=type->getDecl();
      if(IsExpressionLocalTag(tag) || (!tag->getIdentifier() && !tag->getTypedefNameForAnonDecl()))
        valid &= FactoryResultOrigin(tag)!=nullptr ||
            !UnnamedTemplateTagKey(tag,tag->getASTContext()).empty();
      if(const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(tag))
        for(const auto &argument:specialization->getTemplateArgs().asArray())TraverseTemplateArgument(argument);
      return true;
    }
  } visitor;
  visitor.TraverseType(type);return visitor.valid;
}

bool HasFactoryTemplateArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context,FactoryTypeFeatures *features) {
  if(argument.getKind()==clang::TemplateArgument::Type)
    return !FactoryTypeKey(argument.getAsType(),context,features).empty();
  bool found=false;
  if(argument.getKind()==clang::TemplateArgument::Pack)
    for(const auto &element:argument.pack_elements())found |= HasFactoryTemplateArgument(element,context,features);
  return found;
}

void ReadFactoryArgumentFeatures(const clang::TemplateArgument &argument,const clang::ASTContext &context,FactoryTypeFeatures &features) {
  clang::QualType type;
  if(argument.getKind()==clang::TemplateArgument::Type)type=argument.getAsType();
  else if(argument.getKind()==clang::TemplateArgument::StructuralValue)type=argument.getStructuralValueType();
  else if(argument.getKind()==clang::TemplateArgument::Declaration) {
    if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))type=object->getType();
  } else if(argument.getKind()==clang::TemplateArgument::Pack) {
    for(const auto &element:argument.pack_elements())ReadFactoryArgumentFeatures(element,context,features);
  }
  if(!type.isNull())FactoryTypeKey(type,context,&features);
}

bool HasFactoryValueArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context) {
  if(argument.getKind()==clang::TemplateArgument::StructuralValue)
    return !FactoryTypeKey(argument.getStructuralValueType(),context).empty();
  if(argument.getKind()==clang::TemplateArgument::Declaration)
    if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(argument.getAsDecl()))
      return !FactoryTypeKey(object->getType(),context).empty();
  if(argument.getKind()==clang::TemplateArgument::Pack)
    for(const auto &element:argument.pack_elements())
      if(HasFactoryValueArgument(element,context))return true;
  if(argument.getKind()==clang::TemplateArgument::Type)
    if(const auto *specialization=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(argument.getAsType()->getAsCXXRecordDecl()))
      for(const auto &element:specialization->getTemplateArgs().asArray())
        if(HasFactoryValueArgument(element,context))return true;
  return false;
}

std::string FactoryDeclarationTypeKey(const clang::NamedDecl *declaration) {
  if(llvm::isa<clang::NamespaceDecl>(declaration))return {};
  struct References:clang::RecursiveASTVisitor<References> {
    std::string key;
    bool TraverseStmt(clang::Stmt *) {return true;}
    bool TraverseDecl(clang::Decl *declaration) {
      return !declaration || declaration->isImplicit() ||
          clang::RecursiveASTVisitor<References>::TraverseDecl(declaration);
    }
    void Add(clang::QualType type,const clang::ASTContext &context) {
      const auto origins=FactoryTypeKey(type,context);
      if(!origins.empty())key+=std::to_string(origins.size())+":"+origins;
    }
    bool VisitValueDecl(clang::ValueDecl *declaration) {
      Add(declaration->getType(),declaration->getASTContext());return true;
    }
    bool VisitTypedefNameDecl(clang::TypedefNameDecl *declaration) {
      Add(declaration->getUnderlyingType(),declaration->getASTContext());return true;
    }
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *declaration) {
      if(declaration->isCompleteDefinition())
        for(const auto &base:declaration->bases())
          Add(base.getType(),declaration->getASTContext());
      return true;
    }
  } visitor;
  visitor.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));return visitor.key;
}

std::string UnnamedTemplateDeclarationTypeKey(const clang::NamedDecl *declaration) {
  if(llvm::isa<clang::NamespaceDecl>(declaration))return {};
  struct References:clang::RecursiveASTVisitor<References> {
    std::string key;
    bool TraverseStmt(clang::Stmt *) {return true;}
    bool TraverseDecl(clang::Decl *declaration) {
      return !declaration || declaration->isImplicit() ||
          clang::RecursiveASTVisitor<References>::TraverseDecl(declaration);
    }
    void Add(clang::QualType type,const clang::ASTContext &context) {
      const auto origins=UnnamedTemplateTypeKey(type,context);
      if(!origins.empty())key+=std::to_string(origins.size())+":"+origins;
    }
    bool VisitValueDecl(clang::ValueDecl *declaration) {
      // The enclosing function type already binds each parameter. TypeLoc-only
      // parameter declarations are source sugar omitted by ASTImporter.
      if(!llvm::isa<clang::ParmVarDecl>(declaration))Add(declaration->getType(),declaration->getASTContext());
      return true;
    }
    void AddArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context) {
      switch(argument.getKind()) {
      case clang::TemplateArgument::Type:Add(argument.getAsType(),context);break;
      case clang::TemplateArgument::Integral:Add(argument.getIntegralType(),context);break;
      case clang::TemplateArgument::NullPtr:Add(argument.getNullPtrType(),context);break;
      case clang::TemplateArgument::Expression:Add(argument.getAsExpr()->getType(),context);break;
      case clang::TemplateArgument::StructuralValue:Add(argument.getStructuralValueType(),context);break;
      case clang::TemplateArgument::Declaration:Add(argument.getParamTypeForDecl(),context);break;
      case clang::TemplateArgument::Template:case clang::TemplateArgument::TemplateExpansion:
        if(const auto *pattern=argument.getAsTemplateOrTemplatePattern().getAsTemplateDecl()) {
          const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern);
          const auto *owner=record?record->getTemplatedDecl():llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext());
          const auto origin=UnnamedTemplateRecordKey(owner,context);
          if(!origin.empty())key+=std::to_string(origin.size())+":"+origin;
        }
        break;
      default:break;
      }
    }
    bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl *declaration) {
      if(declaration->hasDefaultArgument())AddArgument(declaration->getDefaultArgument().getArgument(),declaration->getASTContext());
      return true;
    }
    bool VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl *declaration) {
      if(declaration->hasDefaultArgument())AddArgument(declaration->getDefaultArgument().getArgument(),declaration->getASTContext());
      return true;
    }
    bool VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl *declaration) {
      if(declaration->hasDefaultArgument())AddArgument(declaration->getDefaultArgument().getArgument(),declaration->getASTContext());
      return true;
    }
    bool VisitObjCMethodDecl(clang::ObjCMethodDecl *declaration) {
      Add(declaration->getReturnType(),declaration->getASTContext());
      for(const auto *parameter:declaration->parameters())Add(parameter->getType(),declaration->getASTContext());
      return true;
    }
    bool VisitObjCPropertyDecl(clang::ObjCPropertyDecl *declaration) {
      Add(declaration->getType(),declaration->getASTContext());return true;
    }
    bool VisitTypedefNameDecl(clang::TypedefNameDecl *declaration) {
      Add(declaration->getUnderlyingType(),declaration->getASTContext());return true;
    }
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *declaration) {
      if(declaration->isCompleteDefinition())
        for(const auto &base:declaration->bases())
          Add(base.getType(),declaration->getASTContext());
      return true;
    }
  } visitor;
  visitor.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));return visitor.key;
}

bool HasDeclarationFactoryTypeProjection(const clang::NamedDecl *declaration) {
  return !FactoryDeclarationTypeKey(declaration).empty();
}

bool HasDeclarationClosureFactory(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Finder:clang::RecursiveASTVisitor<Finder> {
      bool found=false;
      bool VisitLambdaExpr(clang::LambdaExpr *expression) {
        for (const auto *scope=expression->getLambdaClass()->getDeclContext();scope;scope=scope->getParent())
          if (const auto *owner=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
            found |= PublishedClosureResult(owner)==expression->getLambdaClass();break;
          }
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasDeclarationEffectfulLambda(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct Finder:clang::RecursiveASTVisitor<Finder> {
      bool found=false;
      bool VisitLambdaExpr(clang::LambdaExpr *expression) {
        const auto effects=ReadCanonicalFunctionEffects(expression->getCallOperator()->getType(),expression->getLambdaClass()->getASTContext());
        found |= effects.is_async || effects.is_throwing;return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasDeclarationConcept(const clang::NamedDecl *declaration) {
  if (llvm::isa<clang::ConceptDecl>(declaration)) return true;
  struct References : clang::RecursiveASTVisitor<References> {
    bool found = false;
    bool TraverseConceptReference(clang::ConceptReference *reference) {
      found |= reference != nullptr;
      return clang::RecursiveASTVisitor<References>::TraverseConceptReference(reference);
    }
  } visitor;
  if (llvm::isa<clang::TemplateDecl>(declaration) ||
      (llvm::isa<clang::CXXRecordDecl>(declaration) &&
       HasDirectMemberTemplates(llvm::cast<clang::CXXRecordDecl>(declaration)))) {
    visitor.TraverseDecl(const_cast<clang::NamedDecl *>(declaration));
    if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
      for (const auto *specialization:PublishedClassSpecializations(pattern))
        visitor.TraverseDecl(const_cast<clang::ClassTemplateSpecializationDecl *>(specialization));
    if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
      for (const auto *partial:PublishedVariablePartials(pattern))
        visitor.TraverseDecl(const_cast<clang::VarTemplatePartialSpecializationDecl *>(partial));
    for (auto *member:ClosedParentMemberSpecializations(declaration)) visitor.TraverseDecl(member);
    for (auto *pattern:ClosedParentPatterns(declaration)) visitor.TraverseDecl(pattern);
    for (auto *definition:DetachedCallableTemplateDefinitions(declaration)) visitor.TraverseDecl(definition);
    for (auto *definition:DetachedStaticDataTemplateDefinitions(declaration)) visitor.TraverseDecl(definition);
    for (auto *definition:DetachedNestedTemplateDefinitions(declaration)) visitor.TraverseDecl(definition);
    for (auto *specialization:ConcreteMemberClassSpecializations(declaration)) visitor.TraverseDecl(specialization);
    for (auto *specialization:ConcreteMemberFunctionSpecializations(declaration))
      if (specialization->isInlined()) visitor.TraverseDecl(specialization);
    for (auto *specialization:ConcreteMemberVariableSpecializations(declaration))
      if (PublishesVariableInitializer(specialization)) visitor.TraverseDecl(specialization);
    return visitor.found;
  }
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    References references;
    references.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return references.found;
  });
}

bool HasDeclarationGenericLambda(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    LambdaExpressionFinder finder;
    finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return finder.generic;
  });
}

class LocalExpressionFinder final : public clang::RecursiveASTVisitor<LocalExpressionFinder> {
public:
  bool VisitBlockExpr(clang::BlockExpr *) { blocks = true; return true; }
  bool VisitVarDecl(clang::VarDecl *variable) {
    blocks |= variable->hasAttr<clang::BlocksAttr>(); return true;
  }
  bool VisitStmtExpr(clang::StmtExpr *) { statements = true; return true; }
  bool VisitTagDecl(clang::TagDecl *declaration) {
    const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
    const bool local=IsExpressionLocalTag(declaration) && !(record && record->isLambda());
    types |= local;
    unnamed_types |= local && !declaration->getIdentifier();
    union_types |= local && record && record->isUnion();
    return true;
  }
  bool blocks = false;
  bool statements = false;
  bool types = false;
  bool unnamed_types = false;
  bool union_types = false;
};
bool HasDeclarationLocalType(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    LocalExpressionFinder finder;
    finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return finder.types;
  });
}
bool ContainsUnnamedLocalType(const clang::Stmt *statement) {
  LocalExpressionFinder finder;
  finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
  return finder.unnamed_types;
}
bool HasDeclarationUnnamedLocalType(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration,ContainsUnnamedLocalType);
}
bool HasDeclarationUnionValue(const clang::NamedDecl *declaration) {
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->isUnion()) return true;
  return HasPublishedExpression(declaration, [](const clang::Stmt *statement) {
    LocalExpressionFinder finder;
    finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return finder.union_types;
  });
}
bool HasDeclarationLocalExpression(const clang::NamedDecl *declaration, bool block) {
  return HasPublishedExpression(declaration, [=](const clang::Stmt *statement) {
    LocalExpressionFinder finder;
    finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
    return block ? finder.blocks : finder.statements;
  });
}

class ThreadLocalFinder final : public clang::RecursiveASTVisitor<ThreadLocalFinder> {
public:
  bool VisitVarDecl(clang::VarDecl *variable) {
    found |= variable->getTLSKind() != clang::VarDecl::TLS_None;
    return true;
  }
  bool found = false;
};
bool HasDeclarationThreadLocal(const clang::NamedDecl *declaration) {
  if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    return HasDeclarationThreadLocal(pattern->getTemplatedDecl());
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) {
    for (const auto *partial:PublishedVariablePartials(pattern))
      if (HasDeclarationThreadLocal(partial)) return true;
    return HasDeclarationThreadLocal(pattern->getTemplatedDecl());
  }
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    for (const auto *specialization:PublishedClassSpecializations(pattern))
      if (HasDeclarationThreadLocal(specialization)) return true;
    return HasDeclarationThreadLocal(pattern->getTemplatedDecl());
  }
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
    return variable->getTLSKind() != clang::VarDecl::TLS_None;
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
      PublishesFunctionBody(function)) {
    if (const auto *definition = function->getDefinition()) {
      ThreadLocalFinder finder;
      finder.TraverseStmt(definition->getBody());
      if (finder.found) return true;
    }
  }
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->getDefinition())
    for (const auto *member : record->getDefinition()->decls()) {
      if (member->isImplicit() || llvm::isa<clang::VarTemplateSpecializationDecl>(member)) continue;
      if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(member);
          function && function->getTemplateSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *named=llvm::dyn_cast<clang::NamedDecl>(member))
        if (llvm::isa<clang::VarDecl,clang::FunctionDecl,clang::TemplateDecl,clang::CXXRecordDecl>(named) &&
            HasDeclarationThreadLocal(named)) return true;
    }
  return false;
}

bool HasTemplateTemplateParameters(const clang::TemplateParameterList *parameters) {
  return llvm::any_of(*parameters,[](const auto *parameter) {
    return llvm::isa<clang::TemplateTemplateParmDecl>(parameter);
  });
}
bool HasDeclarationTemplateTemplateParameters(const clang::NamedDecl *declaration) {
  if (!declaration) return false;
  if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration))
    if (HasTemplateTemplateParameters(partial->getTemplateParameters())) return true;
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    for (const auto *partial:PublishedVariablePartials(pattern))
      if (HasDeclarationTemplateTemplateParameters(partial)) return true;
  if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(declaration))
    if (HasTemplateTemplateParameters(partial->getTemplateParameters())) return true;
  if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
    for (const auto *specialization:PublishedClassSpecializations(pattern))
      if (HasDeclarationTemplateTemplateParameters(specialization)) return true;
  if (const auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(declaration)) {
    if (HasTemplateTemplateParameters(pattern->getTemplateParameters())) return true;
    return HasDeclarationTemplateTemplateParameters(pattern->getTemplatedDecl());
  }
  if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->getDefinition())
    for (const auto *member:record->getDefinition()->decls()) {
      if (member->isImplicit() || llvm::isa<clang::VarTemplateSpecializationDecl>(member)) continue;
      if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(member);
          function && function->getTemplateSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *named=llvm::dyn_cast<clang::NamedDecl>(member))
        if (HasDeclarationTemplateTemplateParameters(named)) return true;
    }
  return HasPublishedExpression(declaration,[](const clang::Stmt *statement) {
    struct References:clang::RecursiveASTVisitor<References> {
      bool found=false;
      bool VisitLambdaExpr(clang::LambdaExpr *expression) {
        if (const auto *pattern=expression->getCallOperator()->getDescribedFunctionTemplate())
          found |= HasTemplateTemplateParameters(pattern->getTemplateParameters());
        return true;
      }
      bool VisitDeclRefExpr(clang::DeclRefExpr *expression) {
        if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(expression->getDecl()))
          if (const auto *arguments=function->getTemplateSpecializationArgs())
            for (const auto &argument:arguments->asArray())
              found |= argument.getKind()==clang::TemplateArgument::Template;
        return true;
      }
      bool VisitTemplateSpecializationType(clang::TemplateSpecializationType *type) {
        for (const auto &argument:type->template_arguments())
          found |= argument.getKind()==clang::TemplateArgument::Template;
        return true;
      }
    } visitor;
    visitor.TraverseStmt(const_cast<clang::Stmt *>(statement));return visitor.found;
  });
}

bool HasClassLifecycle(const clang::CXXRecordDecl *record) {
  if (!record || !record->isCompleteDefinition())
    return false;
  if (record->isDynamicClass() || record->hasAttr<clang::FinalAttr>() ||
      record->getNumBases() != 0 ||
      record->hasUserDeclaredConstructor() ||
      record->hasUserDeclaredDestructor() ||
      record->hasUserDeclaredCopyAssignment() ||
      record->hasUserDeclaredMoveAssignment())
    return true;
  for (const auto *member : record->decls())
    if (!member->isImplicit()) {
      if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(member)) {
        if (field->getAccess() != clang::AS_public || field->isMutable())
          return true;
      } else if (const auto *method =
                     llvm::dyn_cast<clang::CXXMethodDecl>(member))
        if (method->getAccess() != clang::AS_public || method->isDeleted() ||
            llvm::isa<clang::CXXConversionDecl>(method))
          return true;
    }
  return false;
}

bool HasClassStructure(const clang::CXXRecordDecl *record) {
  if (!record || !record->isCompleteDefinition())
    return false;
  if (record->isDynamicClass() || record->getNumBases() != 0)
    return true;
  for (const auto *method : record->methods())
    if (!method->isImplicit() && method->getMostRecentDecl()->isDefaulted() &&
        !IsInClassDefaultedComparison(method))
      return true;
  return false;
}

bool IsSupportedDefaultedSpecialMember(const clang::CXXMethodDecl *method) {
  if (!method)
    return false;
  method = method->getMostRecentDecl();
  const auto effects =
      ReadCanonicalFunctionEffects(method->getType(), method->getASTContext());
  return !effects.is_async && !effects.is_throwing &&
         method->isExplicitlyDefaulted() &&
         (llvm::isa<clang::CXXConstructorDecl, clang::CXXDestructorDecl>(
              method) ||
          method->isCopyAssignmentOperator() ||
          method->isMoveAssignmentOperator());
}

bool IsOutOfLineDefaultedDefinition(const clang::NamedDecl *declaration) {
  const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(declaration);
  return method && method->isExplicitlyDefaulted() && method->isOutOfLine() &&
         IsSupportedDefaultedSpecialMember(method);
}

std::string RecordLayoutIdentity(const clang::RecordDecl *record,
                                const clang::ASTContext &context) {
  std::string text;
  llvm::raw_string_ostream out(text);
  out << "record{";
  if (llvm::isa<clang::CXXRecordDecl>(record) && record->isUnion())
    out << "union=1;";
  if (record->isCompleteDefinition()) {
    const auto &layout = context.getASTRecordLayout(record);
    out << "size=" << layout.getSize().getQuantity() << ";align="
        << layout.getAlignment().getQuantity() << ';';
    unsigned index = 0;
    for (const auto *field : record->fields()) {
      out << "field=" << (field->getIdentifier()
                                ? field->getNameAsString()
                                : std::string("<anonymous>"))
          << ':' << CanonicalType(field->getType(), context) << '@'
          << layout.getFieldOffset(index++);
      if (field->isBitField())
        out << ":bits=" << field->getBitWidthValue();
      out << ';';
    }
  } else {
    out << "incomplete;";
  }
  out << '}';
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      HasClassLifecycle(cxx)) {
    out << ";cxx-class-lifecycle:v1;trivial=" << cxx->isTrivial()
        << ";trivially-copyable=" << cxx->isTriviallyCopyable()
        << ";standard-layout=" << cxx->isStandardLayout()
        << ";register-passing=" << cxx->canPassInRegisters()
        << ";nontrivial-dtor-for-call="
        << cxx->hasNonTrivialDestructorForCall();
    for (const auto *field : cxx->fields())
      out << ";field-access=" << static_cast<unsigned>(field->getAccess())
          << ":mutable=" << field->isMutable();
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      HasClassStructure(cxx)) {
    out << ";cxx-class-structure:v1;aggregate=" << cxx->isAggregate()
        << ";empty=" << cxx->isEmpty();
    const auto &layout = context.getASTRecordLayout(cxx);
    for (const auto &base : cxx->bases()) {
      const auto *owner = base.getType()->getAsCXXRecordDecl();
      out << ";base=" << CanonicalType(base.getType(), context) << ":access="
          << (base.getAccessSpecifier() == clang::AS_public      ? "public"
              : base.getAccessSpecifier() == clang::AS_protected ? "protected"
                                                                 : "private")
          << ":virtual=" << base.isVirtual() << ":offset="
          << (base.isVirtual() ? layout.getVBaseClassOffset(owner)
                               : layout.getBaseClassOffset(owner))
                 .getQuantity();
    }
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition() &&
      (cxx->isDynamicClass() || cxx->hasAttr<clang::FinalAttr>())) {
    const auto &layout = context.getASTRecordLayout(cxx);
    out << ";cxx-class-dispatch:v1;dynamic=" << cxx->isDynamicClass()
        << ";rtti=" << context.getLangOpts().RTTI
        << ";polymorphic=" << cxx->isPolymorphic()
        << ";abstract=" << cxx->isAbstract()
        << ";final=" << cxx->hasAttr<clang::FinalAttr>()
        << ";nv-size=" << layout.getNonVirtualSize().getQuantity()
        << ";nv-align=" << layout.getNonVirtualAlignment().getQuantity()
        << ";primary-virtual=" << layout.isPrimaryBaseVirtual();
    if (const auto *primary = layout.getPrimaryBase())
      out << ";primary=" << ReadInterfaceDeclarationStableId(primary);
    // Include every transitive virtual base in native layout order, not only
    // the direct bases. This binds diamond sharing and receiver adjustments.
    for (const auto &base : cxx->vbases())
      out << ";vbase=" << CanonicalType(base.getType(), context) << ":offset="
          << layout.getVBaseClassOffset(base.getType()->getAsCXXRecordDecl())
                 .getQuantity();
    // Declaration order determines new virtual slots. The sidecar's sorted
    // declaration table alone cannot distinguish reordered virtual functions.
    for (const auto *method : cxx->methods())
      if (!method->isImplicit() && method->isVirtual())
        out << ";virtual-slot=" << ReadInterfaceDeclarationStableId(method);
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> nested;
    for (const auto *member : cxx->decls())
      if (!member->isImplicit())
        if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(member))
          nested.insert("nested-type=" + ReadInterfaceDeclarationStableId(type) +
                        ":kind=" + type->getDeclKindName() +
                        ":access=" + CxxAccessName(type->getAccess()) + ";");
    // Import dependencies can materialize nested types before their owner.
    // Unlike fields and virtual slots, nested type declaration order is not ABI.
    if (!nested.empty()) out << ";cxx-nested-types:v1;";
    for (const auto &type : nested) out << type;
    std::set<std::string> using_members;
    for (const auto *member:cxx->decls())
      if(const auto *using_decl=llvm::dyn_cast<clang::BaseUsingDecl>(member))
        using_members.insert(ReadInterfaceDeclarationStableId(using_decl)+
                             ":access="+CxxAccessName(using_decl->getAccess()));
    if(!using_members.empty()) out << ";cxx-using:v1;";
    for(const auto &member:using_members) out << "using=" << member << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> expressions;
    for (const auto *field : cxx->fields())
      if (field->hasInClassInitializer())
        expressions.insert("field=" + ReadInterfaceDeclarationStableId(field) + ":" +
            DeclarationExpressionContract(field->getInClassInitializer(), context));
    for (const auto *method : cxx->methods())
      if (!method->isImplicit() && !TemplatePublicationOwner(method)) {
        unsigned index = 0;
        for (const auto *parameter : method->getMostRecentDecl()->parameters()) {
          if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg())
            expressions.insert("method=" + ReadInterfaceDeclarationStableId(method) + ":" +
                std::to_string(index) + ":" +
                DeclarationExpressionContract(parameter->getDefaultArg(), context));
          ++index;
        }
      }
    if (!expressions.empty()) out << ";cxx-declaration-expressions:v1;";
    for (const auto &expression : expressions) out << expression << ';';
    std::set<std::string> definitions;
    for (const auto *method : cxx->methods())
      if (!method->isImplicit() && !TemplatePublicationOwner(method) && PublishesFunctionBody(method))
        definitions.insert(ReadInterfaceDeclarationStableId(method) + ":" +
                           FunctionDefinitionContract(method, context));
    for (const auto *member : cxx->decls())
      if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(member);
          variable && variable->isStaticDataMember() && !TemplatePublicationOwner(variable))
        definitions.insert("static=" + ReadInterfaceDeclarationStableId(variable) +
            ":type=" + CanonicalType(variable->getType(), context) +
            ":inline=" + std::to_string(variable->isInline()) +
            ":constexpr=" + std::to_string(variable->isConstexpr()) +
            ":thread-storage=" + std::to_string(variable->getTSCSpec()) +
            ":access=" + CxxAccessName(variable->getAccess()) +
            ":init=" + (PublishesVariableInitializer(variable) && variable->hasInit()
                ? DeclarationExpressionContract(variable->getInit(), context) : "none"));
    if (!definitions.empty()) out << ";cxx-definitions:v1;";
    for (const auto &definition : definitions) out << "definition=" << definition << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> friends;
    for (const auto *member : cxx->decls())
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member)) {
        if (const auto *type = friend_decl->getFriendType()) {
          friends.insert("type=" + CanonicalType(type->getType(), context));
        } else if (const auto *function =
                       llvm::dyn_cast_or_null<clang::FunctionDecl>(friend_decl->getFriendDecl());
                   function && !IsInClassDefaultedComparison(function)) {
          function = function->getMostRecentDecl();
          friends.insert("function=" + ReadInterfaceDeclarationStableId(function) +
              ":type=" + CanonicalType(function->getType(), context) +
              ":inline=" + std::to_string(function->isInlined()) +
              ":constexpr=" + std::to_string(function->isConstexpr()) +
              ":consteval=" + std::to_string(function->isConsteval()) +
              ":deleted=" + std::to_string(function->isDeleted()) +
              ":lookup=" + std::to_string(function->getIdentifierNamespace()) +
              ":body=" + (PublishesFunctionBody(function)
                  ? FunctionDefinitionContract(function, context) : "declaration"));
        }
      }
    if (!friends.empty()) out << ";cxx-friends:v1;";
    for (const auto &friend_identity : friends) out << "friend=" << friend_identity << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> comparisons;
    auto add = [&](const clang::FunctionDecl *function) {
      if (IsInClassDefaultedComparison(function))
        comparisons.insert(ReadInterfaceDeclarationStableId(function) +
            ":operator=" + std::to_string(function->getOverloadedOperator()) +
            ":consteval=" + std::to_string(function->isConsteval()));
    };
    for (const auto *member : cxx->decls()) {
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(member)) add(function);
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member))
        add(llvm::dyn_cast_or_null<clang::FunctionDecl>(friend_decl->getFriendDecl()));
    }
    if (!comparisons.empty()) out << ";cxx-defaulted-comparisons:v1;";
    for (const auto &comparison : comparisons) out << "comparison=" << comparison << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> defaulted;
    for (const auto *method : cxx->methods())
      if (!method->isImplicit() && IsOutOfLineDefaultedDefinition(method->getMostRecentDecl()))
        defaulted.insert(ReadInterfaceDeclarationStableId(method));
    if (!defaulted.empty()) out << ";cxx-out-of-line-defaulting:v1;";
    for (const auto &member : defaulted) out << "defaulted=" << member << ';';
  }
  if (const auto *cxx = llvm::dyn_cast<clang::CXXRecordDecl>(record);
      cxx && cxx->isCompleteDefinition()) {
    std::set<std::string> immediate;
    auto add = [&](const clang::FunctionDecl *function) {
      if (function && function->getMostRecentDecl()->isExplicitlyDefaulted() && function->isConsteval())
        immediate.insert(ReadInterfaceDeclarationStableId(function));
    };
    for (const auto *member : cxx->decls()) {
      add(llvm::dyn_cast<clang::FunctionDecl>(member));
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member))
        add(llvm::dyn_cast_or_null<clang::FunctionDecl>(friend_decl->getFriendDecl()));
    }
    if (!immediate.empty()) out << ";cxx-immediate-defaulting:v1;";
    for (const auto &member : immediate) out << "immediate=" << member << ';';
  }
  out.flush();
  return text;
}

std::string EnumIdentity(const clang::EnumDecl *enumeration,
                         const clang::ASTContext &context) {
  std::string text = "enum{" + CanonicalType(
      context.getCanonicalTagType(enumeration), context);
  text += std::string(";scoped=") +
          (enumeration->isScoped() ? "true" : "false") +
          ";fixed=" + (enumeration->isFixed() ? "true" : "false");
  // Integer representation participates in enum calling/layout ABI even when
  // the enumerator spellings and values are unchanged (for example short enums).
  if (!enumeration->getIntegerType().isNull())
    text += ";underlying=" + CanonicalType(enumeration->getIntegerType(), context);
  for (const auto *constant : enumeration->enumerators()) {
    llvm::SmallString<32> value;
    constant->getInitVal().toString(value, 10);
    text += ";" + constant->getNameAsString() + "=" + value.str().str();
  }
  return text + '}';
}

std::string ObjCMethodIdentity(const clang::ObjCMethodDecl *method,
                               const clang::ASTContext &context) {
  std::string text;
  llvm::raw_string_ostream out(text);
  out << (method->isInstanceMethod() ? "instance" : "class") << ':'
      << method->getSelector().getAsString() << "->"
      << CanonicalType(method->getReturnType(), context) << '(';
  for (unsigned i = 0; i < method->param_size(); ++i) {
    if (i) out << ',';
    out << CanonicalType(method->getParamDecl(i)->getType(), context);
    const auto lifetime = method->getParamDecl(i)->getType().getObjCLifetime();
    if (lifetime != clang::Qualifiers::OCL_None)
      out << "#lifetime=" << static_cast<unsigned>(lifetime);
  }
  out << ")";
  if (method->isVariadic()) out << ";variadic";
  if (method->isOptional()) out << ";optional";
  if (method->isDirectMethod()) out << ";dispatch=direct";
  if (!method->getObjC3FunctionType().isNull())
    out << ";callable=" << CanonicalType(method->getObjC3FunctionType(), context);
  const auto return_lifetime = method->getReturnType().getObjCLifetime();
  if (return_lifetime != clang::Qualifiers::OCL_None)
    out << ";return-lifetime=" << static_cast<unsigned>(return_lifetime);
  out.flush();
  return text;
}

std::string ObjCContainerIdentity(const clang::NamedDecl *declaration,
                                  const clang::ASTContext &context) {
  if (const auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration)) {
    std::string text = "objc-interface:" + interface->getNameAsString();
    if (const auto *superclass = interface->getSuperClass())
      text += ";super=" + superclass->getNameAsString();
    if (interface->hasAttr<clang::ObjCRootClassAttr>()) text += ";root-class";
    for (const auto *protocol : interface->protocols())
      text += ";protocol=" + protocol->getNameAsString();
    for (const auto *ivar : interface->ivars())
      text += ";ivar=" + ivar->getNameAsString() + ':' +
              CanonicalType(ivar->getType(), context) + "@" +
              std::to_string(static_cast<unsigned>(ivar->getAccessControl()));
    return text;
  }
  if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(declaration)) {
    std::string text = "objc-protocol:" + protocol->getNameAsString();
    for (const auto *adopted : protocol->protocols())
      text += ";protocol=" + adopted->getNameAsString();
    return text;
  }
  if (const auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(declaration)) {
    return "objc-category:" +
           (category->getClassInterface()
                ? category->getClassInterface()->getNameAsString()
                : std::string("<missing-class>")) +
           '(' + category->getNameAsString() + ')';
  }
  return {};
}

struct ClassTemplatePublication {
  bool deduced=false, functions=false, aliases=false, variables=false;
  bool nested=false, thread_local_storage=false, variadic=false, nested_classes=false, effectful=false;
  void Read(const clang::CXXRecordDecl *record) {
    if (!record || !record->isCompleteDefinition()) return;
    for (const auto *member : record->decls()) {
      if (member->isImplicit()) continue;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(member);
      const auto *variable=llvm::dyn_cast<clang::VarDecl>(member);
      if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) {
        functions=true;method=llvm::dyn_cast<clang::CXXMethodDecl>(pattern->getTemplatedDecl());
      } else if (llvm::isa<clang::TypeAliasTemplateDecl>(member)) aliases=true;
      else if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
        variables=nested=true;variable=pattern->getTemplatedDecl();
      } else if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        nested_classes=nested=true;Read(pattern->getTemplatedDecl()->getDefinition());
        for (const auto *specialization:PublishedClassSpecializations(pattern)) Read(specialization);
      } else if (const auto *inner=llvm::dyn_cast<clang::CXXRecordDecl>(member)) {
        nested=true;Read(inner->getDefinition());
      }
      if (method) {
        const auto effects=ReadCanonicalFunctionEffects(method->getType(),method->getASTContext());
        effectful |= (effects.is_async || effects.is_throwing) &&
            (llvm::isa<clang::FunctionTemplateDecl>(member) || record->isDependentContext() ||
             llvm::isa<clang::ClassTemplateSpecializationDecl>(record));
        deduced |= method->getReturnType()->getContainedAutoType()!=nullptr;
        variadic |= method->isVariadic();
      }
      if (variable) {
        deduced |= variable->getType()->getContainedAutoType()!=nullptr;
        thread_local_storage |= variable->getTLSKind()!=clang::VarDecl::TLS_None;
      }
    }
  }
};

std::string DeclarationTypeIdentity(const clang::NamedDecl *declaration,
                                    const clang::ASTContext &context) {
  if (IsStandardCoroutineTraitsSpecialization(declaration)) {
    const auto *record=llvm::cast<clang::ClassTemplateSpecializationDecl>(declaration);
    std::string arguments;
    if (!ClassSpecializationArgumentKey(record,context,arguments)) return {};
    std::string text;llvm::raw_string_ostream stream(text);
    clang::PrintingPolicy policy(context.getLangOpts());
    policy.PrintAsCanonical=true;policy.PolishForDeclaration=true;policy.SuppressScope=true;
    auto *printed=const_cast<clang::ClassTemplateSpecializationDecl *>(record);
    const auto qualifier=printed->getQualifierLoc();
    auto restore=llvm::scope_exit([&]{printed->setQualifierInfo(qualifier);});printed->setQualifierInfo({});
    printed->print(stream,policy);
    return "coroutine-traits{arguments="+arguments+";definition="+ComputeInterfaceSourceSha256(text)+
        "};cxx-coroutine-traits-specializations:v1;";
  }
  if (const auto *concept_decl = llvm::dyn_cast<clang::ConceptDecl>(declaration))
    return ConceptIdentity(concept_decl, context);
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    std::string body;
    llvm::raw_string_ostream stream(body);
    clang::PrintingPolicy policy(context.getLangOpts());
    policy.PrintAsCanonical = true;
    policy.SuppressScope = true;
    policy.AnonymousTagNameStyle=static_cast<unsigned>(clang::PrintingPolicy::AnonymousTagMode::Plain);
    policy.IncludeTagDefinition=HasDeclarationUnnamedLocalType(declaration);
    pattern->getTemplatedDecl()->print(stream, policy);
    const auto origins=FactoryDeclarationTypeKey(pattern);
    if(!origins.empty())body+=";factory-origins="+ComputeInterfaceSourceSha256(origins);
    ClassTemplatePublication members;
    members.Read(pattern->getTemplatedDecl());
    std::map<std::string,std::string> specializations;
    const auto published_specializations=PublishedClassSpecializations(pattern);
    for (const auto *specialization:published_specializations) {
      std::string key,text;
      if (!ClassSpecializationArgumentKey(specialization,context,key)) return {};
      llvm::raw_string_ostream specialization_stream(text);
      auto *printed=const_cast<clang::ClassTemplateSpecializationDecl *>(specialization);
      const auto qualifier=printed->getQualifierLoc();
      auto restore_qualifier=llvm::scope_exit([&]{printed->setQualifierInfo(qualifier);});
      printed->setQualifierInfo({});
      auto specialization_policy=policy;
      specialization_policy.IncludeTagDefinition=HasDeclarationUnnamedLocalType(specialization);
      printed->print(specialization_stream,specialization_policy);
      const auto origins=FactoryDeclarationTypeKey(specialization);
      if(!origins.empty())text+=";factory-origins="+ComputeInterfaceSourceSha256(origins);
      specializations.emplace(key,ComputeInterfaceSourceSha256(text));
      members.Read(specialization);
    }
    for (const auto &[key,digest]:specializations)
      body+=";specialization="+std::to_string(key.size())+":"+key+":"+digest;
    return "class-template{parameters=" + FunctionTemplateHeadContract(pattern, context) +
        ";definition=" + ComputeInterfaceSourceSha256(body) + "};cxx-class-templates:v1;" +
        ";cxx-template-structure:v1;rtti=" + std::to_string(context.getLangOpts().RTTI) + ";" +
        (specializations.empty() || pattern->getDeclContext()->isRecord() ? "" : ";cxx-class-specializations:v1;") +
        (!pattern->getDeclContext()->isRecord() && std::any_of(published_specializations.begin(),published_specializations.end(),
                     [](const auto *specialization){return llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(specialization);})
             ? ";cxx-class-partial-specializations:v1;" : "") +
        (members.deduced ? ";cxx-template-deduction:v1;" : "") +
        (members.effectful ? ";cxx-effectful-template-members:v1;" : "") +
        (members.functions ? ";cxx-member-templates:v1;" : "") +
        (members.aliases ? ";cxx-member-alias-templates:v1;;cxx-alias-templates:v1;" : "") +
        (members.variables ? ";cxx-variable-templates:v1;" : "") +
        (members.nested ? ";cxx-nested-templates:v1;" : "") +
        (members.variadic ? ";cxx-variadic-templates:v1;" : "") +
        (members.thread_local_storage ? ";cxx-thread-local:v1;" : "");
  }
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) {
    const auto *variable = pattern->getTemplatedDecl();
    DeclarationExpressionIdentity identity(context);
    identity.TemplateParameters(pattern);
    std::map<std::string,std::string> partials;
    bool deduced=variable->getType()->getContainedAutoType()!=nullptr;
    for (const auto *partial:PublishedVariablePartials(pattern)) {
      std::string text;llvm::raw_string_ostream stream(text);
      clang::PrintingPolicy policy(context.getLangOpts());policy.PrintAsCanonical=true;
      partial->print(stream,policy);
      partials.emplace(VariablePartialArgumentKey(partial,context),ComputeInterfaceSourceSha256(text));
      deduced |= partial->getType()->getContainedAutoType()!=nullptr;
    }
    std::string partial_contract;
    for (const auto &[key,digest]:partials) partial_contract+=key+":"+digest+";";
    return "variable-template{parameters=" + FunctionTemplateHeadContract(pattern, context) +
        ";type=" + CanonicalType(variable->getType(),context) +
        ";inline=" + std::to_string(variable->isInline()) +
        ";constexpr=" + std::to_string(variable->isConstexpr()) +
        ";thread-storage=" + std::to_string(variable->getTSCSpec()) +
        ";initializer=" + (variable->hasInit() ? identity.Get(const_cast<clang::Expr *>(variable->getInit())) : "default") +
        (partials.empty()?"":";partials="+ComputeInterfaceSourceSha256(partial_contract)) +
        "};cxx-variable-templates:v1;" +
        (partials.empty() || pattern->getDeclContext()->isRecord()?"":";cxx-variable-partial-specializations:v1;") +
        ";cxx-template-structure:v1;" +
        (deduced ? ";cxx-template-deduction:v1;" : "") +
        (HasDeclarationThreadLocal(pattern) ? ";cxx-thread-local:v1;" : "");
  }
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration)) {
    std::string head;
    llvm::raw_string_ostream stream(head);
    clang::PrintingPolicy policy(context.getLangOpts());
    pattern->getTemplateParameters()->print(stream, context, policy);
    DeclarationExpressionIdentity identity(context);
    identity.TemplateParameters(pattern);
    const auto *constraint = pattern->getTemplateParameters()->getRequiresClause();
    return "alias-template{parameters=" + ComputeInterfaceSourceSha256(head) +
        ";constraint=" + (constraint ? identity.Get(const_cast<clang::Expr *>(constraint)) : "none") +
        ";type=" + CanonicalType(pattern->getTemplatedDecl()->getUnderlyingType(), context) +
        "};cxx-alias-templates:v1;";
  }
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) {
    const auto *function = pattern->getTemplatedDecl();
    std::string defaults;
    DeclarationExpressionIdentity identity(context, function);
    for (const auto *parameter : function->parameters())
      if (const auto *expression = WrittenDefaultArgument(parameter))
        defaults += identity.Get(const_cast<clang::Expr *>(expression));
    const auto effects=ReadCanonicalFunctionEffects(function->getType(),context);
    return "function-template{parameters=" + FunctionTemplateHeadContract(pattern, context) +
        ";signature=" + DeclarationTypeIdentity(function, context) +
        (defaults.empty() ? "" : ";cxx-declaration-expressions:v1;template-defaults=" + defaults) +
        "};cxx-function-templates:v2;" +
        ((effects.is_async || effects.is_throwing) ? ";cxx-effectful-function-templates:v1;" : "") +
        (function->isVariadic() ? ";cxx-variadic-templates:v1;" : "") +
        (function->getReturnType()->getContainedAutoType() ? ";cxx-template-deduction:v1;" : "");
  }
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    return ObjCMethodIdentity(method, context);
  if (const auto *property = llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    return CanonicalType(property->getType(), context) + ";objc-property-attributes=" +
           std::to_string(static_cast<unsigned>(property->getPropertyAttributes()));
  if (const auto *using_enum=llvm::dyn_cast<clang::UsingEnumDecl>(declaration))
    return "using-enum{target="+ReadInterfaceDeclarationStableId(using_enum->getEnumDecl())+
           ";access="+CxxAccessName(using_enum->getAccess())+"};cxx-using:v1;";
  if (const auto *using_decl=llvm::dyn_cast<clang::UsingDecl>(declaration)) {
    std::set<std::string> targets;
    bool functions=true;
    for (const auto *shadow:using_decl->shadows()) {
      const auto *target=shadow->getTargetDecl()->getUnderlyingDecl();
      functions &= llvm::isa<clang::FunctionDecl>(target);
      targets.insert(ReadInterfaceDeclarationStableId(target));
    }
    std::string value="using{typename="+std::to_string(using_decl->hasTypename())+
        ";access="+CxxAccessName(using_decl->getAccess())+"};cxx-using:v1;functions="+
        std::to_string(functions)+";";
    for(const auto &target:targets) value+="target="+target+";";
    return value;
  }
  if (const auto *directive=llvm::dyn_cast<clang::UsingDirectiveDecl>(declaration))
    return "using-namespace{target="+
        ReadInterfaceDeclarationStableId(directive->getNominatedNamespaceAsWritten())+
        ";namespace="+ReadInterfaceDeclarationStableId(directive->getNominatedNamespace())+
        "};cxx-using:v1;";
  if (const auto *alias = llvm::dyn_cast<clang::NamespaceAliasDecl>(declaration))
    return "namespace-alias{target=" +
           ReadInterfaceDeclarationStableId(alias->getAliasedNamespace()) +
           ";namespace=" + ReadInterfaceDeclarationStableId(alias->getNamespace()) +
           "};cxx-namespaces:v1;";
  if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration))
    return std::string("namespace{") + space->getQualifiedNameAsString() +
           ";inline=" + std::to_string(space->isInline()) + "}";
  if (const auto *record = llvm::dyn_cast<clang::RecordDecl>(declaration)) {
    std::string value=RecordLayoutIdentity(record, context);
    const auto *cxx=llvm::dyn_cast<clang::CXXRecordDecl>(record);
    if (HasDirectMemberTemplates(cxx)) {
      ClassTemplatePublication members;members.Read(cxx);
      value+=";cxx-concrete-member-templates:v1;";
      if (members.effectful) value+=";cxx-effectful-template-members:v1;";
      if (members.nested_classes)
        value+=";cxx-concrete-nested-templates:v1;;cxx-nested-templates:v1;";
      if (members.functions) value+=";cxx-member-templates:v1;";
      if (members.aliases) value+=";cxx-member-alias-templates:v1;;cxx-alias-templates:v1;";
      if (members.variables) value+=";cxx-variable-templates:v1;";
      if (members.deduced) value+=";cxx-template-deduction:v1;";
      if (members.variadic) value+=";cxx-variadic-templates:v1;";
      if (members.thread_local_storage) value+=";cxx-thread-local:v1;";
      std::set<std::string> patterns;
      for (const auto *member:cxx->decls())
        if (const auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(member))
          patterns.insert(ReadInterfaceDeclarationStableId(pattern)+":access="+
              CxxAccessName(pattern->getAccess())+":"+DeclarationTypeIdentity(pattern,context));
      for (const auto &pattern:patterns) value+="member-template="+pattern+";";
    }
    return value;
  }
  if (const auto *enumeration = llvm::dyn_cast<clang::EnumDecl>(declaration))
    return EnumIdentity(enumeration, context);
  if (llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl,
                clang::ObjCCategoryDecl>(declaration))
    return ObjCContainerIdentity(declaration, context);
  if (const auto *constant = llvm::dyn_cast<clang::EnumConstantDecl>(declaration)) {
    llvm::SmallString<32> value;
    constant->getInitVal().toString(value, 10);
    return CanonicalType(constant->getType(), context) + "=" + value.str().str();
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
    // Retained results have a dedicated ownership identity. Normalize this
    // top-level type bit while keeping nested callable contracts intact.
    clang::QualType type = function->getType();
    if (const auto *proto = type->getAs<clang::FunctionProtoType>()) {
      auto info = proto->getExtProtoInfo();
      info.ExtInfo = info.ExtInfo.withProducesResult(false);
      // Defaulted implicit exception specifications are derived from the bound
      // base/member declarations. Their evaluation is lazy and must not make
      // an interface identity depend on whether the provider used the member.
      if (function->isExplicitlyDefaulted() &&
          function->getExceptionSpecSourceRange().isInvalid())
        info.ExceptionSpec =
            clang::FunctionProtoType::ExceptionSpecInfo(clang::EST_None);
      type = context.getFunctionType(proto->getReturnType(), proto->getParamTypes(), info);
    } else if (const auto *no_proto = type->getAs<clang::FunctionNoProtoType>()) {
      type = context.getFunctionNoProtoType(no_proto->getReturnType(),
          no_proto->getExtInfo().withProducesResult(false));
    }
    std::string identity = CanonicalType(type, context) +
           (context.getLangOpts().CPlusPlus
                ? ";language-linkage=" + std::to_string(static_cast<unsigned>(
                                             function->getLanguageLinkage()))
                : "");
    unsigned index = 0;
    for (const auto *parameter : function->parameters()) {
      if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg())
        identity += ";cxx-declaration-expressions:v1;default=" +
            std::to_string(index) + ":" +
            (function->getDescribedFunctionTemplate()
                ? DeclarationExpressionIdentity(context, function).Get(
                    const_cast<clang::Expr *>(parameter->getDefaultArg()))
                : DeclarationExpressionContract(parameter->getDefaultArg(), context)) + ";";
      ++index;
    }
    if (PublishesFunctionBody(function))
      identity += ";cxx-definitions:v1;inline=1;constexpr=" +
          std::to_string(function->isConstexpr()) + ";consteval=" +
          std::to_string(function->isConsteval()) + ";body=" +
          FunctionDefinitionContract(function, context) + ";";
    return identity;
  }
  if (const auto *value = llvm::dyn_cast<clang::ValueDecl>(declaration)) {
    std::string identity = CanonicalType(value->getType(), context);
    if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(value);
        field && field->hasInClassInitializer())
      identity += ";cxx-declaration-expressions:v1;init=" +
          DeclarationExpressionContract(field->getInClassInitializer(), context) + ";";
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(value);
        variable && context.getLangOpts().CPlusPlus)
      identity +=
          ";language-linkage=" +
          std::to_string(static_cast<unsigned>(variable->getLanguageLinkage()));
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(value);
        variable && variable->isStaticDataMember())
      identity += ";cxx-definitions:v1;static-data=1;access=" +
          std::string(CxxAccessName(variable->getAccess())) + ";";
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(value);
        PublishesVariableInitializer(variable)) {
      identity += ";cxx-definitions:v1;inline=" + std::to_string(variable->isInline()) +
          ";constexpr=" + std::to_string(variable->isConstexpr()) + ";initializer=" +
          (variable->hasInit() ? DeclarationExpressionContract(variable->getInit(), context) : "none") + ";";
    }
    const auto lifetime = value->getType().getObjCLifetime();
    if (lifetime != clang::Qualifiers::OCL_None)
      identity += ";objc-lifetime=" + std::to_string(static_cast<unsigned>(lifetime));
    return identity;
  }
  if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(declaration))
    return CanonicalType(context.getTypeDeclType(type), context);
  return {};
}

std::string ExportRootName(const clang::NamedDecl *declaration) {
  if (const auto *owner=CoroutineTraitsReturnOwner(declaration)) return ExportRootName(owner);
  if (const auto *owner = FriendOwner(declaration)) declaration = owner;
  while (declaration) {
    if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration);
        tag && !tag->getIdentifier())
      if (const auto *alias = tag->getTypedefNameForAnonDecl())
        declaration = alias;
    if (IsTopLevel(declaration))
      return InterfaceQualifiedName(declaration);
    declaration =
        llvm::dyn_cast<clang::NamedDecl>(declaration->getDeclContext());
  }
  return {};
}

// Read native attribute fields rather than source spellings or macro names.
// Length-delimited fields preserve messages, replacement text and platform
// contracts without ambiguity from punctuation or source attribute spelling.
std::string AvailabilityIdentity(const clang::NamedDecl *declaration) {
  std::vector<std::string> attributes;
  for (const auto *attribute : declaration->attrs()) {
    if (attribute->isImplicit())
      continue;
    std::string value;
    auto field = [&](llvm::StringRef text) {
      value += std::to_string(text.size()) + ":" + text.str();
    };
    if (const auto *deprecated =
            llvm::dyn_cast<clang::DeprecatedAttr>(attribute)) {
      field("deprecated");
      field(deprecated->getMessage());
      field(deprecated->getReplacement());
    } else if (const auto *unavailable =
                   llvm::dyn_cast<clang::UnavailableAttr>(attribute)) {
      field("unavailable");
      field(unavailable->getMessage());
    } else if (const auto *availability =
                   llvm::dyn_cast<clang::AvailabilityAttr>(attribute)) {
      field("availability");
      field(availability->getPlatform()->getName());
      field(availability->getIntroduced().normalize().getAsString());
      field(availability->getDeprecated().normalize().getAsString());
      field(availability->getObsoleted().normalize().getAsString());
      field(availability->getUnavailable() ? "true" : "false");
      field(availability->getMessage());
      field(availability->getStrict() ? "true" : "false");
      field(availability->getReplacement());
      field(std::to_string(availability->getPriority()));
      field(availability->getEnvironment()
                ? availability->getEnvironment()->getName()
                : "");
    } else {
      continue;
    }
    // ASTImporter can retain the same availability fact on several inherited
    // redeclarations. Repetition does not change the declaration contract.
    if (std::find(attributes.begin(), attributes.end(), value) == attributes.end())
      attributes.push_back(std::move(value));
  }
  std::string result;
  for (const auto &value : attributes)
    result += std::to_string(value.size()) + ":" + value;
  return result;
}

bool SelectedExport(const clang::NamedDecl *declaration,
                    const std::vector<std::string> &exports) {
  if (exports.empty() ||
      std::find(exports.begin(), exports.end(), "*") != exports.end())
    return true;
  const auto root = ExportRootName(declaration);
  return std::any_of(exports.begin(), exports.end(), [&](const auto &name) {
    // Selecting a namespace admits its public descendants. Selecting a leaf
    // retains the namespace declarations needed to look that leaf up.
    return root == name || root.starts_with(name + "::") ||
           (llvm::isa<clang::NamespaceDecl>(declaration) &&
            name.starts_with(root + "::"));
  });
}

bool ValidateExportNames(const std::vector<std::string> &exports,
                         InterfaceEmissionArtifact &result) {
  std::set<std::string> seen;
  if (exports.size() > kMaxInterfaceExportNames) {
    AddDiagnostic(result, "O3IFC155",
                  "export policy exceeds 4096 declaration names");
    return false;
  }
  for (const auto &name : exports)
    if (name.empty() || name.size() > 256 ||
        std::any_of(name.begin(), name.end(),
                    [](unsigned char c) { return c < 33 || c == 127; }) ||
        !seen.insert(name).second || (name == "*" && exports.size() != 1)) {
      AddDiagnostic(
          result, "O3IFC155",
          "exports require distinct declaration names or a single wildcard");
      return false;
    }
  return true;
}

bool IsBoundedCxxDeclaration(const clang::NamedDecl *, InterfaceEmissionArtifact &,
                             const std::vector<std::string> &);

class TypeDependencyVisitor final
    : public clang::RecursiveASTVisitor<TypeDependencyVisitor> {
 public:
   TypeDependencyVisitor(const clang::ASTContext &context,
                         InterfaceEmissionArtifact &result,
                         const std::set<std::string> &resolved_ids,
                         const std::vector<std::string> &exports,
                         std::string spi_group, const std::string &source_root)
       : context_(context), result_(result), resolved_ids_(resolved_ids),
         exports_(exports), spi_group_(std::move(spi_group)),
         source_root_(source_root) {}

   void CheckDeclarationAttributes(const clang::NamedDecl *declaration) {
     if (!declaration) return;
     for (const auto *attribute:declaration->attrs())
       if (llvm::isa<clang::AlignedAttr,clang::AssumeAlignedAttr,clang::RestrictAttr>(attribute))
         TraverseAttr(const_cast<clang::Attr *>(attribute));
     if (const auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(declaration))
       CheckDeclarationAttributes(pattern->getTemplatedDecl());
   }

   void AllowPublishedFactoryResult(const clang::FunctionDecl *function) {
     for (const auto *tag:PublishedFactoryResults(function)) {
       factory_results_.insert(tag->getCanonicalDecl());
       for (const auto *scope=tag->getDeclContext();scope && !scope->isFunctionOrMethod();scope=scope->getParent())
         if (const auto *parent=llvm::dyn_cast<clang::TagDecl>(scope))
           factory_results_.insert(parent->getCanonicalDecl());
     }
   }

   void CheckVisibility(const clang::NamedDecl *declaration) {
     if (declaration) {
       const auto group = ReadInterfaceSPIGroup(declaration);
       if (!group.empty() &&
           (group != spi_group_ ||
            !IsInMainFile(context_, declaration->getLocation())))
         AddDiagnostic(
             result_, "O3IFC158",
             "exported signature references SPI outside its local group",
             declaration->getQualifiedNameAsString());
     }
     if (declaration && IsInMainFile(context_, declaration->getLocation()) &&
         !SelectedExport(declaration, exports_))
       AddDiagnostic(
           result_, "O3IFC156",
           "public API references a declaration excluded by its export policy",
           declaration->getQualifiedNameAsString());
   }

   bool IsResolved(const clang::NamedDecl *declaration) {
     if (IsStandardCoroutineDeclaration(declaration,context_)) return true;
     if (IsQualifiedRuntimeFrameType(declaration, context_)) {
       UsesRuntimeFrame = true;
       return true;
     }
     if (IsQualifiedRuntimeServiceDeclaration(declaration,context_)) {
       UsesRuntimeFrame=true;
       return true;
     }
     if (IsQualifiedRuntimeTaskDeclaration(declaration,context_)) {
       UsesCxxTaskRuntime=true;
       UsesRuntimeFrame=true;
       return true;
     }
     const auto stable_id = ReadInterfaceDeclarationStableId(declaration);
     return !stable_id.empty() && resolved_ids_.count(stable_id);
   }

  bool TraverseRecordType(clang::RecordType *type, bool traverse_qualifier) {
    clang::ObjC3KeyPathInfo keypath;
    if (clang::getObjC3KeyPathInfo(clang::QualType(type, 0), keypath))
      return TraverseType(keypath.Root) && TraverseType(keypath.Value);
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseRecordType(type, traverse_qualifier);
  }

  void CheckExpressionReference(const clang::NamedDecl *target) {
    if (!target) return;
    target = target->getUnderlyingDecl();
    if (IsStandardCoroutineDeclaration(target,context_)) return;
    if (IsStandardCoroutineTraitsSpecialization(target)) {
      CheckVisibility(target);
      if (!IsInMainFile(context_,target->getLocation()) && !IsResolved(target))
        AddDiagnostic(result_,"O3IFC112","coroutine traits requires its resolved public specialization",target->getQualifiedNameAsString());
      return;
    }
    // C++ default-initialization of the ABI's va_list array can reference its
    // implicit record constructor. It belongs to the same compiler-owned type.
    if (const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(target);
        method && method->isImplicit())
      if (const auto *builtin=context_.getVaListTagDecl();
          builtin && method->getParent()->getCanonicalDecl()==builtin->getCanonicalDecl()) return;
    // Inherited defaults can retain the prototype's parameter declarations.
    // Template parameters are lexical bindings even when their DeclContext
    // is a namespace rather than the definition currently being checked.
    if (llvm::isa<clang::TemplateTypeParmDecl, clang::NonTypeTemplateParmDecl,
                  clang::TemplateTemplateParmDecl>(target)) return;
    if (const auto *specialization = llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(target)) {
      if (IsExplicitNamespaceVariableSpecialization(specialization)) {
        CheckVisibility(specialization);
        if (!IsInMainFile(context_,specialization->getLocation()) && !IsResolved(specialization))
          AddDiagnostic(result_,"O3IFC112","variable specialization requires its resolved public declaration",
                        specialization->getQualifiedNameAsString());
      }
      CheckExpressionReference(specialization->getSpecializedTemplate());
      for (const auto &argument : specialization->getTemplateArgs().asArray())
        if (!CheckGenericArgument(argument)) UnsupportedExpression();
      return;
    }
    if (const auto *pattern = TemplatePublicationOwner(target);
        pattern && pattern!=target &&
        (IsInMainFile(context_,pattern->getLocation()) ||
         resolved_ids_.count(ReadInterfaceDeclarationStableId(pattern)))) {
      CheckExpressionReference(pattern);
      CheckEnclosingClassArguments(target);
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(target))
        if (const auto *arguments = function->getTemplateSpecializationArgs())
          for (const auto &argument : arguments->asArray())
            if (!CheckGenericArgument(argument)) UnsupportedExpression();
      return;
    }
    if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(target)) {
      CheckVisibility(pattern);
      if (!IsTopLevel(pattern) ||
          (!IsInMainFile(context_,pattern->getLocation()) && !IsResolved(pattern)))
        AddDiagnostic(result_, "O3IFC112", "class template requires a public resolved definition",
                      pattern->getQualifiedNameAsString());
      return;
    }
    if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(target)) {
      CheckVisibility(pattern);
      if (!IsTopLevel(pattern) || pattern->getTemplatedDecl()->getStorageClass()==clang::SC_Static ||
          (!IsInMainFile(context_,pattern->getLocation()) && !IsResolved(pattern)))
        AddDiagnostic(result_,"O3IFC112","variable template requires a public resolved definition",
                      pattern->getQualifiedNameAsString());
      return;
    }
    if (llvm::isa<clang::TypeAliasTemplateDecl>(target)) {
      CheckVisibility(target);
      if (!IsTopLevel(target) ||
          (!IsInMainFile(context_, target->getLocation()) && !IsResolved(target)))
        AddDiagnostic(result_, "O3IFC112", "alias template requires a public resolved definition",
                      target->getQualifiedNameAsString());
      return;
    }

    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(target))
      if (const auto *pattern = function->getPrimaryTemplate()) {
        if (IsExplicitNamespaceFunctionSpecialization(function)) {
          CheckVisibility(function);
          if (!IsInMainFile(context_,function->getLocation()) && !IsResolved(function))
            AddDiagnostic(result_,"O3IFC112","function specialization requires its resolved public declaration",
                          function->getQualifiedNameAsString());
        }
        CheckExpressionReference(pattern);
        if (const auto *arguments = function->getTemplateSpecializationArgs())
          for (const auto &argument : arguments->asArray())
            if (!CheckGenericArgument(argument)) UnsupportedExpression();
        return;
      }
    if (llvm::isa<clang::ConceptDecl>(target)) {
      CheckVisibility(target);
      if (!IsTopLevel(target) ||
          (!IsInMainFile(context_, target->getLocation()) && !IsResolved(target)))
        AddDiagnostic(result_, "O3IFC112", "concept reference requires a public resolved definition",
                      target->getQualifiedNameAsString());
      return;
    }
    if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(target))
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(parameter->getDeclContext());
          function && (function->getDescribedFunctionTemplate() || function->getPrimaryTemplate()))
        return;
    if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(target))
      if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(parameter->getDeclContext());
          method && method->getParent()->isLambda()) return;
    if (const auto *object = llvm::dyn_cast<clang::TemplateParamObjectDecl>(target)) {
      TraverseType(object->getType());
      unsigned budget = 4096;
      std::string key;
      std::vector<const clang::NamedDecl *> references;
      if (!StructuralArgumentKey(object->getValue(),context_,key,budget,0,&references))
        AddDiagnostic(result_, "O3IFC101", "unsupported structural template object value");
      for(const auto *reference:references)CheckExpressionReference(reference);
      return;
    }
    // Reserved compiler builtins are supplied by the pinned consumer compiler,
    // not an exported declaration of the publishing package. Ordinary libc
    // declarations retain the normal visibility and dependency checks.
    if (const auto *builtin = llvm::dyn_cast<clang::FunctionDecl>(target);
        builtin && builtin->getBuiltinID() &&
        builtin->getName().starts_with("__builtin_") &&
        builtin->getDeclContext()->getRedeclContext()->isTranslationUnit() && !builtin->hasBody())
      return;
    if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(target);
        method && method->isImplicit()) {
      clang::ObjC3KeyPathInfo info;
      if (clang::getObjC3KeyPathInfo(context_.getCanonicalTagType(method->getParent()), info))
        return;
    }
    if (const auto *owner = llvm::dyn_cast<clang::CXXRecordDecl>(target->getDeclContext());
        owner && owner->isLambda()) return;
    if (in_expression_ && IsLocalExpressionContext(target->getDeclContext())) return;
    if (lambda_locals_.count(target->getCanonicalDecl())) return;
    if (lambda_contexts_.count(
            clang::Decl::castFromDeclContext(target->getDeclContext())->getCanonicalDecl())) return;
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(target->getDeclContext());
        function && body_function_ && function->getCanonicalDecl() == body_function_->getCanonicalDecl())
      return;
    CheckVisibility(target);
    if (target->getDeclContext()->isFunctionOrMethod() ||
        (!target->isExternallyVisible() && IsTopLevel(target)) ||
        (!IsInMainFile(context_, target->getLocation()) && !IsResolved(target)))
      AddDiagnostic(result_, "O3IFC112",
                    "declaration expression references a non-public or unresolved dependency",
                    target->getQualifiedNameAsString());
  }
  void CheckCxxCoroutine(const clang::FunctionDecl *function) {
    if (const auto *body=PublishedCxxCoroutine(function)) {
      if (!body->hasDependentPromiseType()) TraverseType(context_.getCanonicalType(body->getPromiseDecl()->getType()));
      for (const auto *reference:CxxCoroutineReferences(function)) CheckExpressionReference(reference);
    }
  }
  void CheckBody(const clang::FunctionDecl *function) {
    const auto *definition = function->getDefinition();
    if (!definition) return;
    if (IsDetachedCallableTemplateDefinition(definition) && !IsInMainFile(context_,definition->getLocation())) {
      AddDiagnostic(result_,"O3IFC112","detached template body requires a module-owned definition",
                    definition->getQualifiedNameAsString());
      return;
    }
    body_function_ = definition;
    CheckDeclarationAttributes(definition);
    CheckCxxCoroutine(definition);
    const auto *type=clang::getObjC3CallableFunctionType(definition);
    UsesCxxTaskRuntime |= type && type->getObjC3FunctionEffects().Async &&
                         !type->getObjC3FunctionEffects().AsyncFrame;
    if (const auto *constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(definition))
      for (const auto *initializer : constructor->inits())
        if (initializer->isWritten()) TraverseStmt(initializer->getInit());
    TraverseStmt(const_cast<clang::Stmt *>(PublishedFunctionBody(definition)));
    body_function_ = nullptr;
  }
  bool TraverseTemplateName(clang::TemplateName name) {
    if (auto *declaration = name.getAsTemplateDecl()) {
      if (llvm::isa<clang::TypeAliasTemplateDecl>(declaration))
        CheckExpressionReference(declaration);
      else if (llvm::isa<clang::ClassTemplateDecl>(declaration) &&
               (IsInMainFile(context_,declaration->getLocation()) ||
                resolved_ids_.count(ReadInterfaceDeclarationStableId(declaration)) ||
                !ReadNativeSymbolName(declaration).empty()))
        CheckExpressionReference(declaration);
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseTemplateName(name);
  }
  void CheckAliasTemplate(const clang::TypeAliasTemplateDecl *pattern) {
    const auto locals = lambda_locals_;
    const bool previous = public_template_type_;
    public_template_type_=true;
    auto restore = llvm::scope_exit([&] { lambda_locals_ = locals;public_template_type_=previous; });
    for (const auto *parameter : *pattern->getTemplateParameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter : *pattern->getTemplateParameters()) TraverseDecl(parameter);
    TraverseStmt(pattern->getTemplateParameters()->getRequiresClause());
    TraverseType(pattern->getTemplatedDecl()->getUnderlyingType());
  }
  void CheckClassTemplate(const clang::ClassTemplateDecl *pattern) {
    const auto locals = lambda_locals_;
    const bool previous = public_template_type_;
    public_template_type_ = true;
    auto restore = llvm::scope_exit([&] {lambda_locals_=locals;public_template_type_=previous;});
    struct Bindings : clang::RecursiveASTVisitor<Bindings> {
      std::set<const clang::Decl *> &locals;
      explicit Bindings(std::set<const clang::Decl *> &locals) : locals(locals) {}
      bool VisitNamedDecl(clang::NamedDecl *decl) {locals.insert(decl->getCanonicalDecl());return true;}
      bool TraverseStmt(clang::Stmt *) {return true;}
    } bindings(lambda_locals_);
    bindings.TraverseDecl(const_cast<clang::ClassTemplateDecl *>(pattern));
    for (auto *parameter : *pattern->getTemplateParameters()) TraverseDecl(parameter);
    TraverseStmt(pattern->getTemplateParameters()->getRequiresClause());
    CheckTemplateRecord(pattern->getTemplatedDecl());
    for (const auto *specialization:PublishedClassSpecializations(pattern)) {
      bindings.TraverseDecl(const_cast<clang::ClassTemplateSpecializationDecl *>(specialization));
      const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(specialization);
      if (partial) {
        for (auto *parameter:*partial->getTemplateParameters()) TraverseDecl(parameter);
        TraverseStmt(partial->getTemplateParameters()->getRequiresClause());
      }
      for (const auto &argument:specialization->getTemplateArgs().asArray()) {
        if (!(partial?CheckPatternArgument(argument):CheckGenericArgument(argument))) UnsupportedExpression();
      }
      CheckTemplateRecord(specialization);
    }
  }
  void CheckCoroutineTraits(const clang::ClassTemplateSpecializationDecl *record) {
    const auto locals=lambda_locals_;const bool previous=public_template_type_;
    auto restore=llvm::scope_exit([&]{lambda_locals_=locals;public_template_type_=previous;});
    public_template_type_=true;
    const auto *owner=CoroutineTraitsReturnOwner(record);
    if (!owner || (!IsInMainFile(context_,owner->getLocation()) && !IsResolved(owner))) {
      AddDiagnostic(result_,"O3IFC112","coroutine traits return type requires a public module owner",record->getQualifiedNameAsString());return;
    }
    CheckExpressionReference(owner);
    if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(record)) {
      for (auto *parameter:*partial->getTemplateParameters()) lambda_locals_.insert(parameter->getCanonicalDecl());
      for (auto *parameter:*partial->getTemplateParameters()) TraverseDecl(parameter);
      TraverseStmt(partial->getTemplateParameters()->getRequiresClause());
    }
    for (const auto &argument:record->getTemplateArgs().asArray())
      if (!CheckPatternArgument(argument)) UnsupportedExpression();
    CheckTemplateRecord(record);
  }
  void CheckTemplateRecord(const clang::CXXRecordDecl *record) {
    if (const auto *definition=record->getDefinition()) record=definition;
    // A materialized member primary may still be a forward declaration when
    // only one of its partials has been defined. Its origin and each published
    // partial are checked separately; base/layout access requires a definition.
    if (!record->isCompleteDefinition()) return;
    CheckDeclarationAttributes(record);
    if (IsDetachedNestedTemplateDefinition(record) && !IsInMainFile(context_,record->getLocation())) {
      AddDiagnostic(result_,"O3IFC112","detached nested template requires a module-owned definition",
                    record->getQualifiedNameAsString());return;
    }
    for (const auto &base : record->bases()) TraverseType(base.getType());
    for (auto *member : record->decls()) {
      if (member->isImplicit()) continue;
      CheckDeclarationAttributes(llvm::dyn_cast<clang::NamedDecl>(member));
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
      if (const auto *function=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) CheckFunctionTemplate(function);
      else if (const auto *alias=llvm::dyn_cast<clang::TypeAliasTemplateDecl>(member)) CheckAliasTemplate(alias);
      else if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) CheckClassTemplate(pattern);
      else if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) CheckVariableTemplate(pattern);
      else if (const auto *inner=llvm::dyn_cast<clang::CXXRecordDecl>(member)) CheckTemplateRecord(inner);
      else if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(member)) {
        const auto previous_results=factory_results_;
        auto restore_results=llvm::scope_exit([&]{factory_results_=previous_results;});
        AllowPublishedFactoryResult(function);
        TraverseType(function->getType());
        TraverseStmt(const_cast<clang::Expr *>(function->getTrailingRequiresClause().ConstraintExpr));
        for (const auto *parameter : function->parameters())
          if (const auto *value=WrittenDefaultArgument(parameter)) TraverseStmt(const_cast<clang::Expr *>(value));
        if (!IsConcreteMemberFunctionSpecialization(function) || function->isInlined()) CheckBody(function);
      } else if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(member);
                 variable && IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))
        CheckDetachedStaticData(variable->getDefinition());
      else TraverseDecl(member);
    }
    for (const auto *specialization:ConcreteMemberFunctionSpecializations(record))
      CheckConcreteMemberFunctionSpecialization(specialization);
    for (const auto *specialization:ConcreteMemberVariableSpecializations(record))
      if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(specialization))
        CheckVariablePartial(partial);
      else {
        for (const auto &argument:specialization->getTemplateArgs().asArray())
          if (!CheckGenericArgument(argument)) UnsupportedExpression();
        TraverseType(specialization->getType());
        if (PublishesVariableInitializer(specialization) && specialization->hasInit())
          TraverseStmt(const_cast<clang::Expr *>(specialization->getInit()));
      }
  }
  void CheckConcreteMemberFunctionSpecialization(const clang::FunctionDecl *function) {
    const auto previous_results=factory_results_;
    auto restore_results=llvm::scope_exit([&]{factory_results_=previous_results;});
    AllowPublishedFactoryResult(function);
    CheckExpressionReference(function->getPrimaryTemplate());
    for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
      if (!CheckGenericArgument(argument)) UnsupportedExpression();
    TraverseType(function->getType());
    if (function->isInlined()) CheckBody(function);
  }
  void CheckDetachedStaticData(const clang::VarDecl *definition) {
    if (!IsInMainFile(context_,definition->getLocation())) {
      AddDiagnostic(result_,"O3IFC112","detached static template data requires a module-owned definition",
                    definition->getQualifiedNameAsString());return;
    }
    const auto locals=lambda_locals_;
    const bool previous=public_template_type_;
    auto restore=llvm::scope_exit([&]{lambda_locals_=locals;public_template_type_=previous;});
    public_template_type_=true;
    for (const auto *head:definition->getTemplateParameterLists())
      for (auto *parameter:*head) {lambda_locals_.insert(parameter->getCanonicalDecl());TraverseDecl(parameter);}
    TraverseType(definition->getType());public_template_type_=previous;
    if (definition->hasInit()) TraverseStmt(const_cast<clang::Expr *>(definition->getInit()));
  }
  void CheckVariableTemplate(const clang::VarTemplateDecl *pattern) {
    const auto locals=lambda_locals_;
    const bool previous=public_template_type_;public_template_type_=true;
    auto restore=llvm::scope_exit([&]{lambda_locals_=locals;public_template_type_=previous;});
    for (const auto *parameter:*pattern->getTemplateParameters()) lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter:*pattern->getTemplateParameters()) TraverseDecl(parameter);
    TraverseStmt(pattern->getTemplateParameters()->getRequiresClause());
    const auto *variable=pattern->getTemplatedDecl();
    TraverseType(variable->getType());
    public_template_type_=previous;
    if (IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))
      CheckDetachedStaticData(variable->getDefinition());
    else if(variable->hasInit()) TraverseStmt(const_cast<clang::Expr *>(variable->getInit()));
    for (const auto *partial:PublishedVariablePartials(pattern)) CheckVariablePartial(partial);
  }
  void CheckVariablePartial(const clang::VarTemplatePartialSpecializationDecl *partial) {
    const auto locals=lambda_locals_;const bool previous=public_template_type_;
    auto restore=llvm::scope_exit([&]{lambda_locals_=locals;public_template_type_=previous;});
    public_template_type_=true;
    for (const auto *parameter:*partial->getTemplateParameters()) lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter:*partial->getTemplateParameters()) TraverseDecl(parameter);
    TraverseStmt(partial->getTemplateParameters()->getRequiresClause());
    for (const auto &argument:partial->getTemplateArgs().asArray())
      if (!CheckPatternArgument(argument)) UnsupportedExpression();
    TraverseType(partial->getType());public_template_type_=previous;
    if(partial->hasInit()) TraverseStmt(const_cast<clang::Expr *>(partial->getInit()));
  }
  void CheckFunctionTemplate(const clang::FunctionTemplateDecl *pattern) {
    const auto previous_results=factory_results_;
    auto restore_results=llvm::scope_exit([&]{factory_results_=previous_results;});
    AllowPublishedFactoryResult(pattern->getTemplatedDecl());
    const auto locals = lambda_locals_;
    const bool previous = public_template_type_;
    public_template_type_=true;
    auto restore = llvm::scope_exit([&] { lambda_locals_ = locals;public_template_type_=previous; });
    for (const auto *parameter : *pattern->getTemplateParameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter : *pattern->getTemplateParameters()) TraverseDecl(parameter);
    const auto *function = pattern->getTemplatedDecl();
    for (const auto *parameter : function->parameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    TraverseType(function->getType());
    TraverseStmt(pattern->getTemplateParameters()->getRequiresClause());
    TraverseStmt(const_cast<clang::Expr *>(function->getTrailingRequiresClause().ConstraintExpr));
    for (const auto *parameter : function->parameters())
      TraverseStmt(const_cast<clang::Expr *>(WrittenDefaultArgument(parameter)));
    if (const auto *definition = function->getDefinition()) {
      if (const auto *definition_pattern = definition->getDescribedFunctionTemplate())
        for (const auto *parameter : *definition_pattern->getTemplateParameters())
          lambda_locals_.insert(parameter->getCanonicalDecl());
      for (const auto *parameter : definition->parameters())
        lambda_locals_.insert(parameter->getCanonicalDecl());
    }
    public_template_type_=previous;
    CheckBody(function);
  }
  void CheckConcept(const clang::ConceptDecl *declaration) {
    const auto locals = lambda_locals_;
    auto restore = llvm::scope_exit([&] { lambda_locals_ = locals; });
    for (const auto *parameter : *declaration->getTemplateParameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    for (auto *parameter : *declaration->getTemplateParameters())
      TraverseDecl(parameter);
    TraverseStmt(declaration->getTemplateParameters()->getRequiresClause());
    TraverseStmt(const_cast<clang::Expr *>(declaration->getConstraintExpr()));
  }
  bool VisitVarDecl(clang::VarDecl *variable) {
    if (in_expression_) TraverseType(variable->getType());
    if (body_function_)
      for (const auto *attribute : variable->attrs())
        if (!attribute->isImplicit() && !llvm::isa<clang::BlocksAttr>(attribute))
          UnsupportedExpression("local variable attribute has no publication contract");
    return true;
  }
  bool VisitTagDecl(clang::TagDecl *declaration) {
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
        record && record->isLambda()) return true;
    if (IsExpressionLocalTag(declaration) &&
        (!in_expression_ ||
         !IsBoundedCxxDeclaration(declaration, result_, exports_)))
      return UnsupportedExpression();
    return true;
  }
  bool IsLocalExpressionContext(const clang::DeclContext *scope) const {
    for (; scope; scope = scope->getParent()) {
      if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(scope);
          tag && IsExpressionLocalTag(tag)) return true;
      if (body_function_ &&
          clang::Decl::castFromDeclContext(scope)->getCanonicalDecl() ==
              body_function_->getCanonicalDecl()) return true;
      if (lambda_contexts_.count(clang::Decl::castFromDeclContext(scope)->getCanonicalDecl()))
        return true;
    }
    return false;
  }
  bool VisitFieldDecl(clang::FieldDecl *declaration) {
    if (in_expression_) TraverseType(declaration->getType());
    return true;
  }
  bool VisitFunctionDecl(clang::FunctionDecl *declaration) {
    if (in_expression_ && IsLocalExpressionContext(declaration->getDeclContext()))
      TraverseType(declaration->getType());
    return true;
  }
  bool VisitExpr(clang::Expr *expression) {
    TraverseType(expression->getType());
    return true;
  }
  bool VisitDeclRefExpr(clang::DeclRefExpr *expression) {
    CheckExpressionReference(expression->getDecl()); return true;
  }
  bool VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr *expression) {
    for (const auto *declaration : expression->decls())
      CheckExpressionReference(declaration);
    return true;
  }
  bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr *expression) {
    for (const auto *declaration : expression->decls())
      CheckExpressionReference(declaration);
    return true;
  }
  bool VisitMemberExpr(clang::MemberExpr *expression) {
    CheckExpressionReference(expression->getMemberDecl()); return true;
  }
  bool VisitCXXConstructExpr(clang::CXXConstructExpr *expression) {
    CheckExpressionReference(expression->getConstructor()); return true;
  }
  bool VisitCXXNewExpr(clang::CXXNewExpr *expression) {
    CheckExpressionReference(expression->getOperatorNew());
    CheckExpressionReference(expression->getOperatorDelete());
    return true;
  }
  bool VisitCXXDeleteExpr(clang::CXXDeleteExpr *expression) {
    CheckExpressionReference(expression->getOperatorDelete()); return true;
  }
  bool VisitObjCMessageExpr(clang::ObjCMessageExpr *expression) {
    CheckExpressionReference(expression->getMethodDecl()); return true;
  }
  bool TraverseConceptReference(clang::ConceptReference *reference) {
    if (reference) CheckExpressionReference(reference->getNamedConcept());
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseConceptReference(reference);
  }
  bool TraverseRequiresExpr(clang::RequiresExpr *expression, DataRecursionQueue * = nullptr) {
    const auto locals = lambda_locals_;
    auto restore = llvm::scope_exit([&] { lambda_locals_ = locals; });
    for (const auto *parameter : expression->getLocalParameters())
      lambda_locals_.insert(parameter->getCanonicalDecl());
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseRequiresExpr(expression, nullptr);
  }
  void CheckEnclosingClassArguments(const clang::NamedDecl *declaration) {
    for (const clang::NamedDecl *owner=declaration; owner;
         owner=llvm::dyn_cast<clang::NamedDecl>(owner->getDeclContext()))
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(owner))
        for (const auto &argument : specialization->getTemplateArgs().asArray())
          if (!CheckGenericArgument(argument)) UnsupportedExpression();
  }
  bool CheckPatternArgument(const clang::TemplateArgument &argument) {
    if (argument.getKind()==clang::TemplateArgument::Expression) return TraverseStmt(argument.getAsExpr());
    if (argument.getKind()==clang::TemplateArgument::TemplateExpansion)
      return TraverseTemplateName(argument.getAsTemplateOrTemplatePattern());
    if (argument.getKind()==clang::TemplateArgument::Pack) {
      for (const auto &element:argument.pack_elements()) if (!CheckPatternArgument(element)) return false;
      return true;
    }
    return CheckGenericArgument(argument);
  }
  bool CheckGenericArgument(const clang::TemplateArgument &argument) {
    switch (argument.getKind()) {
    case clang::TemplateArgument::Type:
      return TraverseType(argument.getAsType());
    case clang::TemplateArgument::Integral:
      return TraverseType(argument.getIntegralType());
    case clang::TemplateArgument::NullPtr:
      return TraverseType(argument.getNullPtrType());
    case clang::TemplateArgument::StructuralValue: {
      unsigned budget=4096;std::string key;std::vector<const clang::NamedDecl *> references;
      if(!StructuralArgumentKey(argument.getAsStructuralValue(),context_,key,budget,0,&references))return false;
      for(const auto *reference:references)CheckExpressionReference(reference);
      return TraverseType(argument.getStructuralValueType());
    }
    case clang::TemplateArgument::Declaration: {
      const auto *declaration=argument.getAsDecl();std::string key;
      if(const auto *object=llvm::dyn_cast<clang::TemplateParamObjectDecl>(declaration)) {
        unsigned budget=4096;std::vector<const clang::NamedDecl *> references;
        if(!StructuralArgumentKey(object->getValue(),context_,key,budget,0,&references))return false;
        for(const auto *reference:references)CheckExpressionReference(reference);
        return TraverseType(object->getType());
      }
      if(!AddressDeclarationKey(declaration,context_,key))return false;
      CheckExpressionReference(declaration);return TraverseType(argument.getParamTypeForDecl());
    }
    case clang::TemplateArgument::Template: {
      const auto *pattern=argument.getAsTemplate().getAsTemplateDecl();
      if (!pattern) return false;
      if (llvm::isa<clang::BuiltinTemplateDecl,clang::TemplateTemplateParmDecl>(pattern)) return true;
      if (!llvm::isa<clang::ClassTemplateDecl,clang::TypeAliasTemplateDecl>(pattern)) return false;
      CheckExpressionReference(pattern);CheckEnclosingClassArguments(pattern);return true;
    }
    case clang::TemplateArgument::Pack:
      for (const auto &element : argument.pack_elements())
        if (!CheckGenericArgument(element)) return false;
      return true;
    default:
      return false;
    }
  }
  bool TraverseStmt(clang::Stmt *statement, DataRecursionQueue * = nullptr) {
    const auto *previous_function=body_function_;
    const clang::Decl *enclosing_context=nullptr;
    bool inserted_context=false;
    auto restore_function=llvm::scope_exit([&]{
      body_function_=previous_function;
      if(inserted_context)lambda_contexts_.erase(enclosing_context);
    });
    if(auto found=lambda_bodies_.find(statement);found!=lambda_bodies_.end()) {
      if(previous_function) {
        enclosing_context=previous_function->getCanonicalDecl();
        inserted_context=lambda_contexts_.insert(enclosing_context).second;
      }
      body_function_=found->second;
      CheckCxxCoroutine(body_function_);
      statement=const_cast<clang::Stmt *>(PublishedFunctionBody(body_function_));
    }
    const bool previous = in_expression_; in_expression_ = true;
    auto restore = llvm::scope_exit([&] { in_expression_ = previous; });
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseStmt(statement, nullptr);
  }
  bool TraverseCoreturnStmt(clang::CoreturnStmt *statement,
                            DataRecursionQueue * = nullptr) {
    const auto *type=clang::getObjC3CallableFunctionType(body_function_);
    if ((type && type->getObjC3FunctionEffects().Async) || PublishedCxxCoroutine(body_function_))
      return WalkUpFromCoreturnStmt(statement) && TraverseStmt(statement->getOperand());
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::
        TraverseCoreturnStmt(statement,nullptr);
  }
  bool TraverseCoroutineBodyStmt(clang::CoroutineBodyStmt *,
                                 DataRecursionQueue * = nullptr) {
    // The enclosing published function has already been projected to its
    // written body. Nested coroutine callables need their own body contract.
    return UnsupportedExpression("nested coroutine body has no published callable context");
  }
  bool TraverseCoyieldExpr(clang::CoyieldExpr *expression,
                           DataRecursionQueue * = nullptr) {
    if (PublishedCxxCoroutine(body_function_)) {
      const auto *operand=expression->getWrittenOperand();
      if (!operand) return UnsupportedExpression("coroutine yield has no recoverable written operand");
      return WalkUpFromCoyieldExpr(expression) && TraverseStmt(const_cast<clang::Expr *>(operand));
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseCoyieldExpr(expression,nullptr);
  }
  bool TraverseCoawaitExpr(clang::CoawaitExpr *expression,
                           DataRecursionQueue * = nullptr) {
    if (PublishedCxxCoroutine(body_function_))
      return WalkUpFromCoawaitExpr(expression) && TraverseStmt(expression->getOperand());
    if (const auto *source=PublishedAwaitSource(expression,body_function_,context_)) {
      const auto *type=clang::getObjC3AsyncCalleeFunctionType(source);
      UsesCxxTaskRuntime |= !clang::getObjC3AwaitSourceExpression(expression) ||
                           (type && type->getObjC3FunctionEffects().Async &&
                            !type->getObjC3FunctionEffects().AsyncFrame);
      return WalkUpFromCoawaitExpr(expression) &&
             TraverseStmt(const_cast<clang::Expr *>(source));
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::
        TraverseCoawaitExpr(expression,nullptr);
  }
  bool TraversePseudoObjectExpr(clang::PseudoObjectExpr *expression,
                                DataRecursionQueue * = nullptr) {
    if (const auto *source=clang::getObjC3IsolationSourceSyntax(expression))
      return TraverseStmt(const_cast<clang::Expr *>(source));
    clang::ObjC3KeyPathApplicationInfo application;
    if (clang::getObjC3KeyPathApplication(expression, application)) {
      UsesTypedKeyPaths = true;
      for (auto *argument : application.Syntax->arguments())
        if (!TraverseStmt(const_cast<clang::Expr *>(argument))) return false;
      return true;
    }
    if (auto *syntax=KeyPathSubscriptSyntax(expression)) {
      UsesKeyPathSubscripts = UsesTypedKeyPaths = true;
      return TraverseStmt(syntax);
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::
        TraversePseudoObjectExpr(expression,nullptr);
  }
  bool TraverseOpaqueValueExpr(clang::OpaqueValueExpr *expression,
                               DataRecursionQueue * = nullptr) {
    return TraverseStmt(expression->getSourceExpr());
  }
  bool TraverseCompoundLiteralExpr(clang::CompoundLiteralExpr *expression,
                                   DataRecursionQueue * = nullptr) {
    if (clang::getObjC3KeyPathLiteralPath(expression)) {
      // The public spelling is @keypath(Root, path). Its private descriptor
      // and accessors are reconstructed by the pinned consumer compiler.
      UsesTypedKeyPaths = true;
      return TraverseType(expression->getType());
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::
        TraverseCompoundLiteralExpr(expression,nullptr);
  }
  bool TraverseLambdaExpr(clang::LambdaExpr *expression, DataRecursionQueue * = nullptr) {
    std::vector<GenericLambdaBody> bodies;
    if (!GenericLambdaBodies(expression,context_,bodies)) {
      AddDiagnostic(result_,"O3IFC101",
          "generic local lambdas require complete instantiations with stable named types and supported structural, integral, null or pack values");
      return false;
    }
    lambda_bodies_[expression->getBody()]=expression->getCallOperator();
    const auto contexts = lambda_contexts_;
    const auto locals = lambda_locals_;
    lambda_contexts_.insert(expression->getCallOperator()->getCanonicalDecl());
    for (const auto &capture : expression->captures())
      if (capture.capturesVariable() && expression->isInitCapture(&capture))
        lambda_locals_.insert(capture.getCapturedVar()->getCanonicalDecl());
    auto restore = llvm::scope_exit([&] { lambda_contexts_ = contexts; lambda_locals_ = locals; });
    TraverseType(expression->getCallOperator()->getType());
    if (!clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseLambdaExpr(expression,nullptr)) return false;
    for (const auto &body:bodies) {
      lambda_contexts_.insert(body.function->getCanonicalDecl());
      for (const auto &argument : body.function->getTemplateSpecializationArgs()->asArray())
        if (!CheckGenericArgument(argument)) return false;
      for (const auto *parameter:body.function->parameters()) {
        lambda_locals_.insert(parameter->getCanonicalDecl());
        TraverseType(parameter->getType());
      }
      TraverseType(body.function->getType());
      auto *statement=const_cast<clang::Stmt *>(PublishedFunctionBody(body.function));
      lambda_bodies_[statement]=body.function;
      if (!TraverseStmt(statement)) return false;
    }
    return true;
  }
  bool TraverseBlockExpr(clang::BlockExpr *expression, DataRecursionQueue * = nullptr) {
    const auto contexts = lambda_contexts_;
    lambda_contexts_.insert(expression->getBlockDecl());
    auto restore = llvm::scope_exit([&] { lambda_contexts_ = contexts; });
    for (const auto &capture : expression->getBlockDecl()->captures()) {
      CheckExpressionReference(capture.getVariable());
      TraverseType(capture.getCaptureType());
    }
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseBlockExpr(expression, nullptr);
  }
  bool UnsupportedExpression(llvm::StringRef reason={}) {
    std::string message="declaration expressions with local declarations or captures require additional contracts";
    if(!reason.empty())message+="; "+reason.str();
    AddDiagnostic(result_, "O3IFC101",message,
                  body_function_?body_function_->getQualifiedNameAsString():std::string());
    return false;
  }

  void CheckFactoryReference(const clang::FunctionDecl *function) {
    if(const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(function);method && (method->getParent()->isLambda() || IsExpressionLocalTag(method->getParent()))) {
      if(const auto *owner=FactoryResultOrigin(method->getParent()))CheckFactoryReference(owner);
      if(const auto *arguments=function->getTemplateSpecializationArgs())
        for(const auto &argument:arguments->asArray())
          if(!CheckGenericArgument(argument))UnsupportedExpression();
      for(const auto *parameter:function->parameters())TraverseType(parameter->getType());
      return;
    }
    CheckExpressionReference(function);
    CheckEnclosingClassArguments(function);
  }

  bool VisitTagType(clang::TagType *type) {
    const auto *declaration = type->getDecl();
    if (IsStandardCoroutineDeclaration(declaration,context_)) {
      if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration))
        for (const auto &argument:record->getTemplateArgs().asArray())
          if (!CheckPatternArgument(argument)) UnsupportedExpression();
      return true;
    }
    // The target ABI owns this implicit record; it is supplied by the consumer
    // compiler rather than a package. Never recognize a user type by spelling.
    if (const auto *builtin=context_.getVaListTagDecl();
        builtin && declaration->getCanonicalDecl()==builtin->getCanonicalDecl()) return true;
    if (const auto *pattern = TemplatePublicationOwner(declaration);
        !IsExpressionLocalTag(declaration) && pattern && (IsInMainFile(context_,pattern->getLocation()) ||
                    resolved_ids_.count(ReadInterfaceDeclarationStableId(pattern)))) {
      CheckExpressionReference(pattern);
      CheckEnclosingClassArguments(declaration);
      return true;
    }
    if (const auto *factory=FactoryResultOrigin(declaration)) {
      CheckFactoryReference(factory);
      return true;
    }
    if (public_template_type_ && !in_expression_ && !IsPublishedType(declaration) &&
        !IsResolved(declaration))
      AddDiagnostic(result_,"O3IFC112","template type references a non-public declaration",
                    declaration->getQualifiedNameAsString());
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
        record && record->isLambda())
      return in_expression_ || factory_results_.count(record->getCanonicalDecl())
          ? true : UnsupportedExpression("closure type escapes its expression context: "+
              ReadInterfaceDeclarationStableId(llvm::dyn_cast<clang::NamedDecl>(declaration->getDeclContext())));
    if (IsExpressionLocalTag(declaration))
      return (in_expression_ && IsLocalExpressionContext(declaration->getDeclContext())) ||
             factory_results_.count(declaration->getCanonicalDecl())
                 ? true : UnsupportedExpression("local type escapes its callable context");
    CheckVisibility(declaration);
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references a record or enum from an unresolved external dependency",
                    declaration->getQualifiedNameAsString());
    return true;
  }

  bool VisitTypedefType(clang::TypedefType *type) {
    const auto *declaration = type->getDecl();
    if (IsStandardCoroutineDeclaration(declaration,context_)) return true;
    if (const auto *owner=TemplatePublicationOwner(declaration);IsStandardCoroutineTraitsSpecialization(owner)) {
      CheckExpressionReference(owner);CheckEnclosingClassArguments(declaration);
      TraverseType(declaration->getUnderlyingType());return true;
    }
    if (declaration->getCanonicalDecl()==context_.getBuiltinVaListDecl()->getCanonicalDecl()) return true;
    // Runtime headers redeclare these compiler-provided language types. Accept
    // only the same native name and type, without additional typedef contracts.
    if (context_.getLangOpts().ObjC && declaration) {
      for (const auto *builtin :
           {context_.getObjCIdDecl(), context_.getObjCClassDecl(),
            context_.getObjCSelDecl(),
            const_cast<clang::ASTContext &>(context_).getObjCInstanceTypeDecl()}) {
        if (declaration == builtin ||
            (builtin && !declaration->hasAttrs() &&
             declaration->getIdentifier() == builtin->getIdentifier() &&
             context_.hasSameType(declaration->getUnderlyingType(),
                                  builtin->getUnderlyingType())))
          return true;
      }
    }
    const auto contract = clang::getObjC3ResourceTypeContract(
        context_, clang::QualType(type, 0));
    if (const auto *cleanup = contract.getCleanupFunction()) {
      CheckVisibility(cleanup);
      if (!cleanup->isExternallyVisible() ||
          (!IsInMainFile(context_, cleanup->getLocation()) && !IsResolved(cleanup)))
        AddDiagnostic(result_, "O3IFC170",
                      "exported resource cleanup needs an externally visible, resolved function",
                      cleanup->getQualifiedNameAsString());
    }
    if (contract.Invalid) {
      const auto *invalid = contract.Invalid->getInvalid();
      if (NativeResourceSentinelIdentity(invalid, context_) == "!unmodeled")
        AddDiagnostic(result_, "O3IFC170",
                      "resource sentinel has no stable exported constant identity",
                      declaration->getQualifiedNameAsString());
      clang::Expr::EvalResult value;
      if (invalid->EvaluateAsRValue(value, context_) && value.Val.isLValue())
        if (const auto *base = value.Val.getLValueBase().dyn_cast<const clang::ValueDecl *>()) {
          CheckVisibility(base);
          if (!IsInMainFile(context_, base->getLocation()) && !IsResolved(base))
            AddDiagnostic(result_, "O3IFC170", "resource sentinel dependency is unresolved",
                          base->getQualifiedNameAsString());
        }
    }
    CheckVisibility(declaration);
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references a typedef from an unresolved external dependency",
                    declaration->getQualifiedNameAsString());
    return true;
  }

  bool VisitObjCInterfaceType(clang::ObjCInterfaceType *type) {
    const auto *declaration = type->getDecl();
    CheckVisibility(declaration);
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references an Objective-C class from an unresolved external dependency",
                    declaration->getNameAsString());
    return true;
  }

  bool VisitObjCObjectType(clang::ObjCObjectType *type) {
    for (const auto *protocol : type->quals()) {
      CheckVisibility(protocol);
      if (!IsInMainFile(context_, protocol->getLocation()) && !IsResolved(protocol))
        AddDiagnostic(result_, "O3IFC112",
                      "public declaration references a protocol from an unresolved external dependency",
                      protocol->getNameAsString());
    }
    return true;
  }

  bool VisitFunctionProtoType(clang::FunctionProtoType *type) {
    const auto effects = type->getObjC3FunctionEffects();
    // The canonical function type explicitly selects its physical ABI. Never
    // infer the return carrier from the importing translation unit's language.
    UsesRuntimeFrame |= effects.Async && effects.AsyncFrame;
    UsesCanonicalFunctionEffects |=
        effects.Async || effects.Throws != clang::ObjC3ThrowsKind::None;
    if (!effects.ErrorType.isNull()) TraverseType(effects.ErrorType);
    return true;
  }

  bool UsesCanonicalFunctionEffects = false;
  bool UsesRuntimeFrame = false;
  bool UsesCxxTaskRuntime = false;
  bool UsesTypedKeyPaths = false;
  bool UsesKeyPathSubscripts = false;

private:
  bool public_template_type_ = false;
  bool in_expression_ = false;
  std::set<const clang::Decl *> factory_results_;
  std::set<const clang::Decl *> lambda_contexts_;
  std::set<const clang::Decl *> lambda_locals_;
  const clang::FunctionDecl *body_function_ = nullptr;
  std::map<const clang::Stmt *,const clang::FunctionDecl *> lambda_bodies_;
  const clang::ASTContext &context_;
  InterfaceEmissionArtifact &result_;
  const std::set<std::string> &resolved_ids_;
  const std::vector<std::string> &exports_;
  std::string spi_group_;
  std::string source_root_;
  bool IsInMainFile(const clang::ASTContext &context,
                    clang::SourceLocation location) const {
    return IsModuleOwnedSource(context, location, source_root_);
  }
};

bool CheckTypeDependencies(const clang::NamedDecl *declaration,
                           const clang::ASTContext &context,
                           InterfaceEmissionArtifact &result,
                           const std::set<std::string> &resolved_ids,
                           bool &uses_runtime_frame,
                           bool &uses_cxx_task_runtime,
                           const std::vector<std::string> &exports = {},
                           const std::string &source_root = {},
                           bool *uses_typed_keypaths = nullptr,
                           bool *uses_keypath_subscripts = nullptr) {
  const auto first_diagnostic=result.diagnostics.size();
  auto qualify_diagnostics=llvm::scope_exit([&] {
    for(auto index=first_diagnostic;index<result.diagnostics.size();++index)
      if(result.diagnostics[index].subject.empty())
        result.diagnostics[index].subject=declaration->getQualifiedNameAsString();
  });
  TypeDependencyVisitor visitor(context, result, resolved_ids, exports,
                                ReadInterfaceSPIGroup(declaration),
                                source_root);
  visitor.AllowPublishedFactoryResult(llvm::dyn_cast<clang::FunctionDecl>(declaration));
  visitor.CheckDeclarationAttributes(declaration);
  auto check = [&](clang::QualType type) {
    if (type.isNull()) return;
    visitor.TraverseType(type);
    // Clang's type visitor deliberately stops at typedef sugar. Canonical
    // traversal exposes nested callable contracts behind those aliases.
    if (type != type.getCanonicalType())
      visitor.TraverseType(type.getCanonicalType());
  };
  if (const auto *value = llvm::dyn_cast<clang::ValueDecl>(declaration))
    check(value->getType());
  if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(declaration))
    check(context.getTypeDeclType(type));
  if (const auto *concept_decl = llvm::dyn_cast<clang::ConceptDecl>(declaration))
    visitor.CheckConcept(concept_decl);
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration))
    visitor.CheckFunctionTemplate(pattern);
  if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration);
      IsExplicitNamespaceVariableSpecialization(specialization)) {
    visitor.CheckExpressionReference(specialization->getSpecializedTemplate());
    for (const auto &argument:specialization->getTemplateArgs().asArray())
      if (!visitor.CheckGenericArgument(argument))
        AddDiagnostic(result,"O3IFC101","variable specialization argument requires additional publication support",
                      specialization->getQualifiedNameAsString());
  }
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
      IsExplicitNamespaceFunctionSpecialization(function)) {
    visitor.CheckExpressionReference(function->getPrimaryTemplate());
    for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
      if (!visitor.CheckGenericArgument(argument))
        AddDiagnostic(result,"O3IFC101","function specialization argument requires additional publication support",
                      function->getQualifiedNameAsString());
  }
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration))
    visitor.CheckAliasTemplate(pattern);
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    visitor.CheckVariableTemplate(pattern);
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration))
    visitor.CheckClassTemplate(pattern);
  if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration);
      IsStandardCoroutineTraitsSpecialization(record)) visitor.CheckCoroutineTraits(record);
  if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      HasDirectMemberTemplates(record)) visitor.CheckTemplateRecord(record);
  for (const auto *pattern:ClosedParentPatterns(declaration)) {
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern)) visitor.CheckClassTemplate(record);
    else if (const auto *function=llvm::dyn_cast<clang::FunctionTemplateDecl>(pattern)) visitor.CheckFunctionTemplate(function);
    else if (const auto *variable=llvm::dyn_cast<clang::VarTemplateDecl>(pattern)) visitor.CheckVariableTemplate(variable);
    else if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(pattern)) visitor.CheckVariablePartial(partial);
    else if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(pattern)) visitor.CheckClassTemplate(partial->getSpecializedTemplate());
  }
  for (const auto *specialization:ClosedParentMemberSpecializations(declaration)) {
    const auto *parent=llvm::cast<clang::ClassTemplateSpecializationDecl>(specialization->getDeclContext());
    for (const auto &argument:parent->getTemplateArgs().asArray())
      if (!visitor.CheckGenericArgument(argument))
        AddDiagnostic(result,"O3IFC101","closed parent argument requires additional publication support",specialization->getQualifiedNameAsString());
    if (const auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(specialization)) visitor.CheckTemplateRecord(record);
    else if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(specialization))
      visitor.CheckConcreteMemberFunctionSpecialization(function);
    else if (const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(specialization)) {
      for (const auto &argument:variable->getTemplateArgs().asArray())
        if (!visitor.CheckGenericArgument(argument))
          AddDiagnostic(result,"O3IFC101","closed member argument requires additional publication support",variable->getQualifiedNameAsString());
      visitor.TraverseType(variable->getType());
      if (PublishesVariableInitializer(variable) && variable->hasInit())
        visitor.TraverseStmt(const_cast<clang::Expr *>(variable->getInit()));
    }
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    for (const auto *parameter : function->parameters())
      if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg())
        visitor.TraverseStmt(const_cast<clang::Expr *>(parameter->getDefaultArg()));
  if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration);
      field && field->hasInClassInitializer())
    visitor.TraverseStmt(field->getInClassInitializer());
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
      PublishesFunctionBody(function)) visitor.CheckBody(function);
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
      PublishesVariableInitializer(variable) && variable->hasInit())
    visitor.TraverseStmt(const_cast<clang::Expr *>(variable->getInit()));
  auto check_reference=[&](const clang::NamedDecl *target) {
    visitor.CheckVisibility(target);
    if (!IsInMainFile(context,target->getLocation(),source_root) && !visitor.IsResolved(target))
      AddDiagnostic(result,"O3IFC112","using target has no resolved interface identity",
                    target->getQualifiedNameAsString());
  };
  if(const auto *using_enum=llvm::dyn_cast<clang::UsingEnumDecl>(declaration))
    check_reference(using_enum->getEnumDecl());
  if(const auto *using_decl=llvm::dyn_cast<clang::UsingDecl>(declaration))
    for(const auto *shadow:using_decl->shadows()) check_reference(shadow->getTargetDecl());
  if(const auto *directive=llvm::dyn_cast<clang::UsingDirectiveDecl>(declaration))
    check_reference(directive->getNominatedNamespaceAsWritten());
  if (const auto *alias = llvm::dyn_cast<clang::NamespaceAliasDecl>(declaration)) {
    const auto *target = alias->getAliasedNamespace();
    visitor.CheckVisibility(target);
    if (!IsInMainFile(context, target->getLocation(), source_root) &&
        !visitor.IsResolved(target))
      AddDiagnostic(result, "O3IFC112", "namespace alias target has no resolved interface identity",
                    target->getQualifiedNameAsString());
  }
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->isCompleteDefinition())
    for (const auto &base : record->bases())
      check(base.getType());
  if (const auto *property = llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    check(property->getType());
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration)) {
    check(method->getObjC3FunctionType());
    check(method->getReturnType());
    for (unsigned i = 0; i < method->param_size(); ++i)
      check(method->getParamDecl(i)->getType());
  }
  if (const auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration)) {
    visitor.CheckVisibility(interface->getSuperClass());
    for (const auto *protocol : interface->protocols())
      visitor.CheckVisibility(protocol);
    if (const auto *superclass = interface->getSuperClass())
      if (!IsInMainFile(context, superclass->getLocation(), source_root) &&
          !visitor.IsResolved(superclass))
        AddDiagnostic(result, "O3IFC112",
                      "Objective-C superclass dependency has no resolved interface identity",
                      superclass->getNameAsString());
    for (const auto *protocol : interface->protocols())
      if (!IsInMainFile(context, protocol->getLocation(), source_root) &&
          !visitor.IsResolved(protocol))
        AddDiagnostic(result, "O3IFC112",
                      "adopted Objective-C protocol dependency has no resolved interface identity",
                      protocol->getNameAsString());
    for (const auto *ivar : interface->ivars()) check(ivar->getType());
  }
  if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(declaration))
    for (const auto *adopted : protocol->protocols()) {
      visitor.CheckVisibility(adopted);
      if (!IsInMainFile(context, adopted->getLocation(), source_root) &&
          !visitor.IsResolved(adopted))
        AddDiagnostic(result, "O3IFC112",
                      "adopted Objective-C protocol dependency has no resolved interface identity",
                      adopted->getNameAsString());
    }
  if (const auto *category =
          llvm::dyn_cast<clang::ObjCCategoryDecl>(declaration))
    visitor.CheckVisibility(category->getClassInterface());
  uses_runtime_frame |= visitor.UsesRuntimeFrame;
  uses_cxx_task_runtime |= visitor.UsesCxxTaskRuntime;
  if (uses_typed_keypaths) *uses_typed_keypaths |= visitor.UsesTypedKeyPaths;
  if (uses_keypath_subscripts) *uses_keypath_subscripts |= visitor.UsesKeyPathSubscripts;
  if (visitor.UsesCxxTaskRuntime && !IsQualifiedRuntimeTaskHeader(context))
    AddDiagnostic(result, "O3IFC112",
                  "published bodies using C++ tasks require the exact SDK task runtime header",
                  declaration->getQualifiedNameAsString());
  return visitor.UsesCanonicalFunctionEffects;
}

// Member patterns are owned and printed with the enclosing primary class.
// Their native specialization mangling inherits that class's module ABI tag.
// Authored detached callables and static data use explicit definition-set
// contracts; member specializations require separate publication support.
bool IsBoundedNestedTemplateParameters(const clang::TemplateParameterList *parameters,
                                        unsigned depth=0);

bool IsBoundedClassMemberAlias(const clang::TypeAliasTemplateDecl *pattern) {
  if (!IsBoundedNestedTemplateParameters(pattern->getTemplateParameters())) return false;
  for (const auto *attribute : pattern->getTemplatedDecl()->attrs())
    if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  return true;
}

bool IsBoundedClassMemberTemplate(const clang::FunctionTemplateDecl *pattern) {
  const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(pattern->getTemplatedDecl());
  if (!method || (!method->doesThisDeclarationHaveABody() &&
                  !IsDetachedCallableTemplateDefinition(method->getDefinition()))) return false;
  if (!IsBoundedNestedTemplateParameters(pattern->getTemplateParameters())) return false;
  for (const auto *attribute : method->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (tag->tags_size()!=1 || !tag->tags_begin()->starts_with("__objc3_m_") ||
          ReadNativeSymbolName(pattern)!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  for (const auto *parameter : method->parameters())
    for (const auto *attribute : parameter->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  return true;
}

bool IsBoundedNestedTemplateParameters(const clang::TemplateParameterList *parameters,
                                        unsigned depth) {
  if (depth>64) return false;
  for (const auto *parameter:*parameters) {
    for (const auto *attribute:parameter->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
    if (const auto *nested=llvm::dyn_cast<clang::TemplateTemplateParmDecl>(parameter)) {
      if (nested->templateParameterKind()!=clang::TNK_Type_template ||
          !IsBoundedNestedTemplateParameters(nested->getTemplateParameters(),depth+1)) return false;
    }
  }
  return true;
}

bool IsBoundedVariablePartial(const clang::VarTemplatePartialSpecializationDecl *partial) {
  if (!IsBoundedNestedTemplateParameters(partial->getTemplateParameters()) ||
      (partial->isStaticDataMember() && !IsConcreteMemberVariableSpecialization(partial) && !IsDependentMemberPartial(partial) && !IsClosedParentPattern(partial)) ||
      (partial->getStorageClass()==clang::SC_Static && !IsConcreteMemberVariableSpecialization(partial) && !IsDependentMemberPartial(partial) && !IsClosedParentPattern(partial)) ||
      partial->getStorageClass()==clang::SC_PrivateExtern ||
      partial->isThisDeclarationADefinition()==clang::VarDecl::DeclarationOnly) return false;
  for (const auto *attribute:partial->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if(tag->tags_size()!=1 || ReadNativeSymbolName(partial)!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  return true;
}

bool IsBoundedClassMemberVariable(const clang::VarTemplateDecl *pattern) {
  const auto *variable=pattern->getTemplatedDecl();
  if (!IsBoundedNestedTemplateParameters(pattern->getTemplateParameters()) ||
      !variable->isStaticDataMember() ||
      (((!variable->isInline() && !variable->isConstexpr()) || !variable->hasInit()) &&
       !IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))) return false;
  for (const auto *attribute:variable->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (tag->tags_size()!=1 || !tag->tags_begin()->starts_with("__objc3_m_") ||
          ReadNativeSymbolName(pattern)!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  return true;
}

bool IsBoundedTemplateRecord(const clang::CXXRecordDecl *record,
                            const clang::ClassTemplateDecl *pattern=nullptr);
bool IsBoundedPublishedClassTemplate(const clang::ClassTemplateDecl *pattern);
bool IsBoundedTemplateRecord(const clang::CXXRecordDecl *record,
                            const clang::ClassTemplateDecl *pattern) {
  if (pattern && !IsBoundedNestedTemplateParameters(pattern->getTemplateParameters())) return false;
  const auto *body=record->getDefinition();
  if (!body || !body->isCompleteDefinition() || body->isUnion() ||
      (!body->getIdentifier() && UnnamedTemplateRecordKey(body,body->getASTContext()).empty())) return false;
  if (record->getDeclContext()->isRecord() && !body->getLexicalDeclContext()->isRecord() &&
      !IsDetachedNestedTemplateDefinition(body) &&
      !IsConcreteMemberClassSpecialization(llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(body)) &&
      !IsDependentMemberPartial(body) && !IsClosedParentMemberSpecialization(body) && !IsClosedParentPattern(body)) return false;
  if ((body->getNumBases() || body->isPolymorphic()) && !body->getASTContext().getLangOpts().RTTI) return false;
  for (const auto *attribute : body->attrs()) {
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (!pattern || tag->tags_size()!=1 || !tag->tags_begin()->starts_with("__objc3_m_") ||
          ReadNativeSymbolName(pattern)!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute) && !llvm::isa<clang::FinalAttr>(attribute)) return false;
  }
  for (const auto *member : body->decls()) {
    if (member->isImplicit() || llvm::isa<clang::AccessSpecDecl>(member)) continue;
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation) continue;
    if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(member);
        (IsDependentMemberPartial(partial) || IsClosedParentPattern(partial))) {
      if (!IsBoundedTemplateRecord(partial,partial->getSpecializedTemplate()) ||
          !IsBoundedNestedTemplateParameters(partial->getTemplateParameters())) return false;
      continue;
    }
    if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
        IsClosedParentMemberSpecialization(specialization)) {
      std::string key;
      if (!IsBoundedTemplateRecord(specialization,specialization->getSpecializedTemplate()) ||
          !ClassSpecializationArgumentKey(specialization,specialization->getASTContext(),key)) return false;
      continue;
    }
    if (llvm::isa<clang::ClassTemplateSpecializationDecl>(member)) return false;
    if (const auto *function=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) {
      if (!IsBoundedClassMemberTemplate(function)) return false;
    } else if (const auto *alias=llvm::dyn_cast<clang::TypeAliasTemplateDecl>(member)) {
      if (!IsBoundedClassMemberAlias(alias)) return false;
    } else if (const auto *inner=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
      if (!IsBoundedPublishedClassTemplate(inner)) return false;
    } else if (const auto *inner=llvm::dyn_cast<clang::CXXRecordDecl>(member)) {
      if (!IsBoundedTemplateRecord(inner)) return false;
    } else if (const auto *value=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
      const auto *variable=value->getTemplatedDecl();
      if (!IsBoundedNestedTemplateParameters(value->getTemplateParameters()) ||
          !variable->isStaticDataMember() ||
          (((!variable->isInline() && !variable->isConstexpr()) || !variable->hasInit()) &&
           !IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))) return false;
      for (const auto *attribute : variable->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
      for (const auto *partial:PublishedVariablePartials(value))
        if (!IsBoundedVariablePartial(partial)) return false;
    } else if (const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(member)) {
      if (IsClosedParentMemberSpecialization(method)) continue;
      if (method->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization ||
          (!method->doesThisDeclarationHaveABody() &&
           !IsDetachedCallableTemplateDefinition(method->getDefinition()) && !method->isDefaulted() &&
           !method->isDeleted() && !method->isPureVirtual())) return false;
    } else if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(member)) {
      if (!variable->isStaticDataMember() ||
          (!variable->isInline() && !variable->isConstexpr() &&
           !IsDetachedStaticDataTemplateDefinition(variable->getDefinition()))) return false;
    } else if (!llvm::isa<clang::FieldDecl,clang::TypedefNameDecl,clang::EnumDecl,clang::BaseUsingDecl>(member)) return false;
    for (const auto *attribute : member->attrs())
      if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute) && !llvm::isa<clang::OverrideAttr,clang::FinalAttr>(attribute)) return false;
  }
  return true;
}

bool IsBoundedPublishedClassTemplate(const clang::ClassTemplateDecl *pattern) {
  if (!IsBoundedNestedTemplateParameters(pattern->getTemplateParameters()) ||
      !IsBoundedTemplateRecord(pattern->getTemplatedDecl(),pattern)) return false;
  for (const auto *specialization:PublishedClassSpecializations(pattern)) {
    if (!IsBoundedTemplateRecord(specialization,pattern)) return false;
    if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(specialization))
      if (!IsBoundedNestedTemplateParameters(partial->getTemplateParameters())) return false;
    std::string key;
    if (!ClassSpecializationArgumentKey(specialization,specialization->getASTContext(),key)) return false;
  }
  return true;
}

bool IsBoundedConcreteMemberFunctionSpecialization(const clang::FunctionDecl *function) {
  if (!IsConcreteMemberFunctionSpecialization(function) ||
      (function->isInlined() && !function->getDefinition())) return false;
  for (const auto *attribute:function->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (tag->tags_size()!=1 || ReadNativeSymbolName(function->getPrimaryTemplate())!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  for (const auto *parameter:function->parameters())
    for (const auto *attribute:parameter->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  std::string key;
  for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
    if (!GenericArgumentKey(argument,function->getASTContext(),key)) return false;
  return true;
}

bool IsBoundedConcreteMemberVariableSpecialization(const clang::VarTemplateSpecializationDecl *variable) {
  if (!IsConcreteMemberVariableSpecialization(variable)) return false;
  if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(variable))
    return IsBoundedVariablePartial(partial);
  if (variable->getType()->isDependentType()) return false;
  for (const auto *attribute:variable->attrs())
    if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute)) {
      if (tag->tags_size()!=1 || ReadNativeSymbolName(variable->getSpecializedTemplate())!=*tag->tags_begin()) return false;
    } else if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
  std::string key;
  for (const auto &argument:variable->getTemplateArgs().asArray())
    if (!GenericArgumentKey(argument,variable->getASTContext(),key)) return false;
  return true;
}

bool IsBoundedClosedParentMembers(const clang::NamedDecl *owner) {
  for (const auto *specialization:ClosedParentMemberSpecializations(owner)) {
    const auto *parent=llvm::cast<clang::ClassTemplateSpecializationDecl>(specialization->getDeclContext());
    std::string arguments;
    if (!ClassSpecializationArgumentKey(parent,parent->getASTContext(),arguments)) return false;
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(specialization)) {
      if (!IsBoundedTemplateRecord(record,record->getSpecializedTemplate()) ||
          !ClassSpecializationArgumentKey(record,record->getASTContext(),arguments)) return false;
    } else if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(specialization)) {
      if (function->isInlined() && !function->getDefinition()) return false;
      for (const auto *attribute:function->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
      for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
        if (!GenericArgumentKey(argument,function->getASTContext(),arguments)) return false;
    } else if (const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(specialization)) {
      if (variable->getType()->isDependentType()) return false;
      for (const auto *attribute:variable->attrs()) if (!attribute->isImplicit() && !IsPublishedNativeAttribute(attribute) && !IsForeignImportAttribute(attribute)) return false;
      for (const auto &argument:variable->getTemplateArgs().asArray())
        if (!GenericArgumentKey(argument,variable->getASTContext(),arguments)) return false;
    }
  }
  return true;
}

bool IsBoundedClosedParentPatterns(const clang::NamedDecl *owner) {
  for (const auto *pattern:ClosedParentPatterns(owner)) {
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateDecl>(pattern)) {
      if (!IsBoundedPublishedClassTemplate(record)) return false;
    } else if (const auto *function=llvm::dyn_cast<clang::FunctionTemplateDecl>(pattern)) {
      if (!IsBoundedClassMemberTemplate(function)) return false;
    } else if (const auto *variable=llvm::dyn_cast<clang::VarTemplateDecl>(pattern)) {
      if (!IsBoundedClassMemberVariable(variable)) return false;
    } else if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(pattern)) {
      if (!IsBoundedVariablePartial(partial)) return false;
    } else if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(pattern)) {
      std::string key;
      if (!IsBoundedTemplateRecord(partial,partial->getSpecializedTemplate()) ||
          !IsBoundedNestedTemplateParameters(partial->getTemplateParameters()) ||
          !ClassSpecializationArgumentKey(partial,partial->getASTContext(),key)) return false;
    }
  }
  return true;
}

bool IsBoundedCxxDeclaration(const clang::NamedDecl *declaration,
                             InterfaceEmissionArtifact &result,
                             const std::vector<std::string> &exports = {}) {
  const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
  if (IsStandardCoroutineTraitsSpecialization(record)) {
    const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(record);
    if (record->isCompleteDefinition() && CoroutineTraitsReturnOwner(record) &&
        IsBoundedTemplateRecord(record) && (!partial || IsBoundedNestedTemplateParameters(partial->getTemplateParameters()))) return true;
    AddDiagnostic(result,"O3IFC101","coroutine traits specialization requires a complete bounded definition and named return owner",record->getQualifiedNameAsString());return false;
  }
  bool supported = IsBoundedClosedParentMembers(declaration) && IsBoundedClosedParentPatterns(declaration);
  if (record && record->isCompleteDefinition()) {
    bool dependent_factory_local=false;
    if (IsExpressionLocalTag(record))
      for (const auto *scope=record->getDeclContext();scope;scope=scope->getParent())
        if (const auto *owner=llvm::dyn_cast<clang::FunctionDecl>(scope)) {
          dependent_factory_local=IsPublishedDeducedFactory(owner);break;
        }
    supported &=
        (!record->isDependentContext() || dependent_factory_local) && !record->getDescribedClassTemplate() &&
        (record->getTemplateSpecializationKind() == clang::TSK_Undeclared ||
         (record->getTemplateSpecializationKind() == clang::TSK_ImplicitInstantiation &&
          dependent_factory_local && FactoryResultOrigin(record))) &&
        (!record->isDynamicClass() || record->getASTContext().getLangOpts().RTTI);
    for (const auto *member : record->decls()) {
      if (member->isImplicit() || llvm::isa<clang::AccessSpecDecl, clang::BlockDecl>(member))
        continue;
      if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(member)) {
        supported &= IsBoundedPublishedClassTemplate(pattern);continue;
      }
      if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(member)) {
        supported &= IsBoundedClassMemberTemplate(pattern);
        for (const auto *specialization:pattern->specializations())
          if (specialization->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization)
            supported &= IsBoundedConcreteMemberFunctionSpecialization(specialization);
        continue;
      }
      if (const auto *pattern=llvm::dyn_cast<clang::TypeAliasTemplateDecl>(member)) {
        supported &= IsBoundedClassMemberAlias(pattern);continue;
      }
      if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(member)) {
        supported &= IsBoundedClassMemberVariable(pattern);
        for (const auto *partial:PublishedVariablePartials(pattern))
          supported &= IsBoundedConcreteMemberVariableSpecialization(partial);
        for (const auto *specialization:pattern->specializations())
          if (specialization->getSpecializationKind()==clang::TSK_ExplicitSpecialization)
            supported &= IsBoundedConcreteMemberVariableSpecialization(specialization);
        continue;
      }
      if (const auto *method=llvm::dyn_cast<clang::CXXMethodDecl>(member);
          method && TemplatePublicationOwner(method)) continue;
      if (const auto *variable=llvm::dyn_cast<clang::VarDecl>(member);
          variable && TemplatePublicationOwner(variable)) continue;
      const auto *field = llvm::dyn_cast<clang::FieldDecl>(member);
      if (field) continue;
      if (llvm::isa<clang::BaseUsingDecl>(member)) continue;
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member)) {
        supported &= IsConcreteFriend(friend_decl);
        if (const auto *function = llvm::dyn_cast_or_null<clang::FunctionDecl>(friend_decl->getFriendDecl()))
          for (const auto *redecl : function->redecls())
            if (const auto *owner = llvm::dyn_cast<clang::CXXRecordDecl>(redecl->getLexicalDeclContext());
                owner && redecl->getFriendObjectKind() != clang::Decl::FOK_None) {
              if (!SelectedExport(owner, exports))
                AddDiagnostic(result, "O3IFC156", "friend binding references an excluded owning record",
                              owner->getQualifiedNameAsString());
              if (ReadInterfaceSPIGroup(owner) != ReadInterfaceSPIGroup(record))
                AddDiagnostic(result, "O3IFC157", "shared friend owners require the same SPI policy",
                              function->getQualifiedNameAsString());
            }
        continue;
      }
      if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(member)) {
        supported &= variable->isStaticDataMember() && !variable->getDescribedVarTemplate();
        continue;
      }
      if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(member);
          IsConcreteMemberClassSpecialization(specialization)) {
        supported &= IsBoundedTemplateRecord(specialization,specialization->getSpecializedTemplate());continue;
      }
      if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(member)) {
        if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(type))
          supported &= (tag->getIdentifier() || tag->getTypedefNameForAnonDecl() || IsExpressionLocalTag(tag));
        supported &= IsBoundedCxxDeclaration(type, result, exports);
        continue;
      }
      const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(member);
      supported &=
          method && (!method->isTemplated() ||
              (dependent_factory_local && !method->getDescribedFunctionTemplate() && !method->getPrimaryTemplate())) &&
          (!method->isDefaulted() || IsSupportedDefaultedSpecialMember(method) ||
           IsInClassDefaultedComparison(method)) &&
          !method->getTrailingRequiresClause();
      if (const auto *ctor =
              llvm::dyn_cast_or_null<clang::CXXConstructorDecl>(method))
        supported &= ctor->getExplicitSpecifier().getExpr() == nullptr;
      if (const auto *conversion =
              llvm::dyn_cast_or_null<clang::CXXConversionDecl>(method))
        supported &= conversion->getExplicitSpecifier().getExpr() == nullptr;
    }
  }
  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(declaration))
    if (method->getMostRecentDecl()->isDefaulted())
      supported &= IsSupportedDefaultedSpecialMember(method) || IsInClassDefaultedComparison(method);
  if (IsTopLevel(declaration)) {
    supported &=
        !llvm::isa<clang::TemplateDecl>(declaration) ||
        llvm::isa<clang::ConceptDecl, clang::FunctionTemplateDecl, clang::TypeAliasTemplateDecl,
                   clang::VarTemplateDecl, clang::ClassTemplateDecl>(declaration);
    if (const auto *pattern=llvm::dyn_cast<clang::TemplateDecl>(declaration))
      supported &= IsBoundedNestedTemplateParameters(pattern->getTemplateParameters());
    if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration)) {
      supported &= IsExplicitNamespaceVariableSpecialization(specialization) &&
          specialization->isExternallyVisible() && !specialization->getType()->isDependentType();
      std::string key;
      for (const auto &argument:specialization->getTemplateArgs().asArray())
        supported &= GenericArgumentKey(argument,specialization->getASTContext(),key);
    }
    if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
      supported &= IsBoundedTemplateRecord(pattern->getTemplatedDecl(),pattern);
      for (const auto *specialization:PublishedClassSpecializations(pattern)) {
        supported &= IsBoundedTemplateRecord(specialization,pattern);
        std::string key;
        supported &= ClassSpecializationArgumentKey(specialization,declaration->getASTContext(),key);
        if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(specialization))
          supported &= IsBoundedNestedTemplateParameters(partial->getTemplateParameters());
      }
    }
    if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(declaration)) {
      const auto *definition=pattern->getTemplatedDecl()->getDefinition();
      const auto *variable=definition ? definition : pattern->getTemplatedDecl();
      supported &= variable->isThisDeclarationADefinition()!=clang::VarDecl::DeclarationOnly &&
          !variable->isStaticDataMember();
      for (const auto *partial:PublishedVariablePartials(pattern)) supported &= IsBoundedVariablePartial(partial);
      for (const auto *attribute:variable->attrs())
        if (const auto *tag=llvm::dyn_cast<clang::AbiTagAttr>(attribute))
          supported &= tag->tags_size()==1 && tag->tags_begin()->starts_with("__objc3_m_") &&
              ReadNativeSymbolName(pattern)==*tag->tags_begin();
        else supported &= (attribute->isImplicit() || IsPublishedNativeAttribute(attribute) || IsForeignImportAttribute(attribute));
    }
    if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration)) {
      const auto *alias = pattern->getTemplatedDecl();
      for (const auto *attribute : alias->attrs())
        if (const auto *annotation=llvm::dyn_cast<clang::AnnotateAttr>(attribute);
            annotation && annotation->getAnnotation().starts_with("objc3.alias-owner:"))
          supported &= annotation->args_size()==0 &&
              annotation->getAnnotation().drop_front(sizeof("objc3.alias-owner:")-1)==ReadNativeSymbolName(pattern);
        else supported &= (attribute->isImplicit() || IsPublishedNativeAttribute(attribute) || IsForeignImportAttribute(attribute));
    }
    if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) {
      const auto *function = pattern->getTemplatedDecl();
      supported &= function->getDefinition() != nullptr &&
          !llvm::isa<clang::CXXMethodDecl>(function) && !function->isDeleted() &&
          !function->isDefaulted();
      for (const auto *attribute : function->attrs()) {
        if (const auto *tag = llvm::dyn_cast<clang::AbiTagAttr>(attribute))
          supported &= tag->tags_size() == 1 &&
              tag->tags_begin()->starts_with("__objc3_m_") &&
              ReadNativeSymbolName(pattern) == *tag->tags_begin();
        else
          supported &= (attribute->isImplicit() || IsPublishedNativeAttribute(attribute) || IsForeignImportAttribute(attribute)) ||
              llvm::isa<clang::DeprecatedAttr, clang::UnavailableAttr,
                        clang::AvailabilityAttr>(attribute);
      }
      for (const auto *parameter : function->parameters())
        for (const auto *attribute : parameter->attrs()) supported &= (attribute->isImplicit() || IsPublishedNativeAttribute(attribute) || IsForeignImportAttribute(attribute));
    }
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration)) {
      const auto original = NamespaceAttributes(space);
      for (const auto *attribute : space->attrs())
        if (!attribute->isImplicit()) {
          supported &= IsModeledNamespaceAttribute(attribute);
          // Namespace defaults must be stable across extensions. Applying a
          // later attribute retroactively can change earlier symbol visibility.
          if (!attribute->isInherited())
            supported &= original.count(NamespaceAttributeText(attribute, space->getASTContext()));
        }
    }
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && function->isExternallyVisible()) {
      bool specialization=IsExplicitNamespaceFunctionSpecialization(function);
      if (specialization) {
        std::string key;
        specialization &= !function->isDeleted() && !function->isDefaulted() &&
            function->getTemplateSpecializationArgs();
        if (specialization)
          for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
            specialization &= GenericArgumentKey(argument,function->getASTContext(),key);
      }
      supported &= ((!function->getPrimaryTemplate() && !function->isTemplated()) || specialization) &&
                   !function->getTrailingRequiresClause() &&
                   (!function->isDefaulted() || IsInClassDefaultedComparison(function));
    }
  }
  if (!supported)
    AddDiagnostic(result, "O3IFC101",
                  "C++ declaration exceeds the implemented semantic interface contract; "
                  "public dependent templates and unnamed nested types require "
                  "dedicated publication support",
                  declaration->getQualifiedNameAsString());
  return supported;
}

class InterfaceCollector final
    : public clang::RecursiveASTVisitor<InterfaceCollector> {
 public:
   InterfaceCollector(clang::ASTContext &context,
                      InterfaceEmissionArtifact &result,
                      const std::set<std::string> &resolved_ids,
                      const std::vector<std::string> &exports = {},
                      const std::string &source_root = {})
       : context_(context), result_(result), resolved_ids_(resolved_ids),
         exports_(exports), source_root_(source_root) {}

   // Function bodies and initializers do not contribute public declarations.
   bool TraverseStmt(clang::Stmt *) { return true; }

   void ValidateSelection() {
     for (const auto &name : exports_) {
       if (name == "*")
         continue;
       if (std::none_of(root_declarations_.begin(), root_declarations_.end(),
                        [&](const auto *root) {
                          return InterfaceQualifiedName(root) == name;
                        }))
         AddDiagnostic(result_, "O3IFC155",
                       "export selection does not name an eligible public root "
                       "declaration",
                       name);
     }
   }

  bool VisitImportDecl(clang::ImportDecl *declaration) {
    if (IsQualifiedStandardLibraryImport(declaration, context_))
      return true;
    if (IsInMainFile(context_, declaration->getLocation())) {
      const auto *module = declaration->getImportedModule();
      AddDiagnostic(result_, "O3IFC111",
                    "source contains a module import but its resolved interface dependency identity is unavailable",
                    module ? module->getFullModuleName() : "<unknown-module>");
    }
    return true;
  }

  bool VisitNamedDecl(clang::NamedDecl *declaration) {
    if (!result_.diagnostics.empty())
      return true;
    if (llvm::isa<clang::TemplateTypeParmDecl,clang::NonTypeTemplateParmDecl,
                  clang::TemplateTemplateParmDecl>(declaration)) return true;
    // The standard header supplies these namespace owners. Authored traits
    // customizations publish independently through their return-type root.
    if (const auto *space=llvm::dyn_cast<clang::NamespaceDecl>(declaration);
        space && IsStandardCoroutineNamespace(space,context_)) {
      if (IsInMainFile(context_,space->getLocation()))
        for (const auto *attribute:space->attrs())
          if (!attribute->isImplicit() && !attribute->isInherited())
            AddDiagnostic(result_,"O3IFC101","a package cannot change standard coroutine namespace attributes",space->getQualifiedNameAsString());
      return true;
    }
    if (IsDetachedCallableTemplateRoot(declaration) || IsDetachedStaticDataTemplateRoot(declaration) ||
        IsDetachedNestedTemplateRoot(declaration)) {
      if (IsInMainFile(context_,declaration->getLocation()) && SelectedExport(declaration,exports_))
        root_declarations_.push_back(declaration);
      return true;
    }
    if (IsClosedParentPattern(declaration)) {
      if (!declaration->getLexicalDeclContext()->isRecord() &&
          IsInMainFile(context_,declaration->getLocation()) && SelectedExport(declaration,exports_))
        root_declarations_.push_back(declaration);
      return true;
    }
    if (IsClosedParentMemberSpecialization(declaration)) {
      if (!declaration->getLexicalDeclContext()->isRecord() &&
          IsInMainFile(context_,declaration->getLocation()) && SelectedExport(declaration,exports_))
        root_declarations_.push_back(declaration);
      return true;
    }
    // Instantiating an enclosing class materializes its member partial patterns.
    // They remain owned by the authored dependent pattern, without replay roots
    // or diagnostics for a new explicit source specialization.
    if (const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration);
        partial && partial->getInstantiatedFromMember() && !partial->isMemberSpecialization()) return true;
    if (const auto *partial=llvm::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(declaration);
        partial && partial->getInstantiatedFromMember() && !partial->isMemberSpecialization()) return true;
    if (IsDependentMemberPartial(declaration)) {
      if (!declaration->getLexicalDeclContext()->isRecord() &&
          IsInMainFile(context_,declaration->getLocation()) && SelectedExport(declaration,exports_))
        root_declarations_.push_back(declaration);
      return true;
    }
    if (auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration);
        IsConcreteMemberClassSpecialization(specialization) &&
        !specialization->getLexicalDeclContext()->isRecord()) {
      if (IsInMainFile(context_,specialization->getLocation()) && SelectedExport(specialization,exports_))
        root_declarations_.push_back(specialization);
      return true;
    }
    if (auto *specialization=llvm::dyn_cast<clang::FunctionDecl>(declaration);
        IsConcreteMemberFunctionSpecialization(specialization)) {
      if (!specialization->getLexicalDeclContext()->isRecord() &&
          IsInMainFile(context_,specialization->getLocation()) && SelectedExport(specialization,exports_))
        root_declarations_.push_back(specialization);
      return true;
    }
    if (auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration);
        IsConcreteMemberVariableSpecialization(specialization)) {
      if (!specialization->getLexicalDeclContext()->isRecord() &&
          IsInMainFile(context_,specialization->getLocation()) && SelectedExport(specialization,exports_))
        root_declarations_.push_back(specialization);
      return true;
    }
    if (const auto *owner=TemplatePublicationOwner(declaration); owner && owner!=declaration) {
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
          function && function->getTemplateSpecializationKind()==clang::TSK_ExplicitSpecialization &&
          IsInMainFile(context_,function->getLocation()) && SelectedExport(function,exports_))
        AddDiagnostic(result_, "O3IFC101", "explicit member template specializations require dedicated publication support",
                      function->getQualifiedNameAsString());
      if (const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration);
          variable && variable->getSpecializationKind()!=clang::TSK_ImplicitInstantiation &&
          IsInMainFile(context_,variable->getLocation()) && SelectedExport(variable,exports_))
        AddDiagnostic(result_, "O3IFC101", "explicit member variable template specializations require dedicated publication support",
                      variable->getQualifiedNameAsString());
      if (auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration);
          IsExplicitNamespaceClassSpecialization(specialization) &&
          IsInMainFile(context_,specialization->getLocation()) && SelectedExport(specialization,exports_)) {
        root_declarations_.push_back(specialization);
        return true;
      }
      if (const auto *specialization = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration);
          specialization && IsInMainFile(context_,specialization->getLocation()) &&
          SelectedExport(specialization,exports_) &&
          (specialization->getSpecializationKind()==clang::TSK_ExplicitSpecialization ||
                             llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(specialization)))
        if (!IsConcreteMemberClassSpecialization(specialization))
          AddDiagnostic(result_, "O3IFC101", "explicit class template specializations require dedicated publication support",
                      declaration->getQualifiedNameAsString());
      return true;
    }
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && function->getDescribedFunctionTemplate()) return true;
    if (const auto *alias = llvm::dyn_cast<clang::TypeAliasDecl>(declaration);
        alias && alias->getDescribedAliasTemplate()) return true;
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
        variable && variable->getDescribedVarTemplate()) return true;
    if (auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration)) {
      if (IsTopLevel(partial) && IsInMainFile(context_,partial->getLocation()) && SelectedExport(partial,exports_))
        root_declarations_.push_back(partial);
      return true;
    }
    if (const auto *specialization = llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration);
        specialization && specialization->getSpecializationKind()==clang::TSK_ImplicitInstantiation)
      return true;
    if (!SelectedExport(declaration, exports_))
      return true;
    if (context_.getLangOpts().CPlusPlus && !declaration->isImplicit() &&
        IsInMainFile(context_, declaration->getLocation()) &&
        IsTopLevel(declaration) &&
        !IsBoundedCxxDeclaration(declaration, result_, exports_))
      return true;
    if (const auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(declaration)) {
      if (category->IsClassExtension() &&
          IsInMainFile(context_, category->getLocation())) {
        AddDiagnostic(result_, "O3IFC113",
                      "Objective-C class extension layout is not yet represented in the public interface contract",
                      category->getNameAsString());
        return true;
      }
    }
    if (!IsIdentityCandidate(declaration, context_, source_root_)) {
      if (const auto *parameter = llvm::dyn_cast<clang::ParmVarDecl>(declaration)) {
        const auto *owner = llvm::dyn_cast<clang::NamedDecl>(
            clang::Decl::castFromDeclContext(parameter->getDeclContext()));
        if (owner && IsIdentityCandidate(owner, context_, source_root_))
          for (const auto *attribute : parameter->attrs()) {
            if (attribute->isImplicit() ||
                IsForeignImportAttribute(attribute) || IsPublishedNativeAttribute(attribute) ||
                llvm::isa<clang::NoEscapeAttr, clang::NSConsumedAttr, clang::CFConsumedAttr>(attribute))
              continue;
            AddDiagnostic(result_, "O3IFC116",
                (llvm::Twine("public parameter has an attribute not modeled in the interface: ") +
                 attribute->getSpelling()).str(), owner->getQualifiedNameAsString());
          }
      }
      // Native C typedefs can have no linkage while naming a callable contract.
      // Validate their type even when the current public identity model does
      // not export them, rather than silently accepting unqualified effects.
      if (IsTopLevel(declaration) &&
          IsInMainFile(context_, declaration->getLocation()) &&
          llvm::isa<clang::TypedefNameDecl>(declaration))
        CheckTypeDependencies(declaration, context_, result_, resolved_ids_,
                              uses_runtime_frame_, uses_cxx_async_bodies_, exports_, source_root_,
                              &uses_typed_keypaths_, &uses_keypath_subscripts_);
      return true;
    }
    if (auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration)) {
      if (method->getBody()) return true;
    }
    if (auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(declaration)) {
      if (category->IsClassExtension()) {
        AddDiagnostic(result_, "O3IFC113",
                      "Objective-C class extensions are not exported as public interfaces",
                      category->getClassInterface()
                          ? category->getClassInterface()->getNameAsString()
                          : "<unknown-class>");
        return true;
      }
    }
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && !context_.getLangOpts().CPlusPlus && function->isInlineSpecified()) {
      AddDiagnostic(result_, "O3IFC115", "C inline function linkage requires a dedicated definition contract",
                    function->getQualifiedNameAsString());
      return true;
    }
    for (const auto *attribute : declaration->attrs()) {
      if (attribute->isImplicit() || IsForeignImportAttribute(attribute) ||
          (!llvm::isa<clang::NamespaceDecl>(declaration) && IsPublishedNativeAttribute(attribute))) continue;
      if (const auto *annotation = llvm::dyn_cast<clang::AnnotateAttr>(attribute);
          annotation && llvm::isa<clang::ConceptDecl>(declaration) &&
          annotation->getAnnotation().starts_with("objc3.concept-owner:") &&
          annotation->args_size() == 0) continue;
      if (const auto *annotation =
              llvm::dyn_cast<clang::AnnotateAttr>(attribute);
          annotation &&
          annotation->getAnnotation().starts_with("objc3.module-name:") &&
          llvm::isa<clang::ObjCCategoryDecl>(declaration) &&
          annotation->args_size() == 0)
        continue;
      if (const auto *annotation =
              llvm::dyn_cast<clang::AnnotateAttr>(attribute);
          annotation && annotation->getAnnotation().starts_with("objc3.spi:")) {
        const auto group = annotation->getAnnotation().drop_front(10).str();
        unsigned count = 0;
        for (const auto *a : declaration->specific_attrs<clang::AnnotateAttr>())
          count += a->getAnnotation().starts_with("objc3.spi:");
        if (!IsTopLevel(declaration) ||
            llvm::isa<clang::ObjCCategoryDecl>(declaration) ||
            !IsValidSPIGroup(group) || count != 1 || annotation->args_size()) {
          AddDiagnostic(result_, "O3IFC157",
                        "SPI requires one valid group on an exported root; "
                        "categories and members cannot define SPI groups",
                        declaration->getQualifiedNameAsString());
          return true;
        }
        for (const auto *scope = declaration->getDeclContext(); scope; scope = scope->getParent())
          if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(scope)) {
            const auto parent_group = ReadInterfaceSPIGroup(space);
            if (!parent_group.empty() && parent_group != group)
              AddDiagnostic(result_, "O3IFC157", "nested SPI root conflicts with its namespace group",
                            declaration->getQualifiedNameAsString());
          }
        continue;
      }
      if (llvm::isa<clang::NamespaceDecl>(declaration) && IsModeledNamespaceAttribute(attribute))
        continue;
      if (llvm::isa<clang::CXXRecordDecl>(declaration)) {
        if (const auto *tag = llvm::dyn_cast<clang::AbiTagAttr>(attribute);
            tag && tag->tags_size() == 1 &&
            tag->tags_begin()->starts_with("__objc3_m_") &&
            ReadNativeSymbolName(declaration) == *tag->tags_begin())
          continue;
        if (llvm::isa<clang::FinalAttr>(attribute))
          continue;
      }
      if (llvm::isa<clang::CXXMethodDecl>(declaration) &&
          llvm::isa<clang::FinalAttr, clang::OverrideAttr>(attribute))
        continue;
      if ((llvm::isa<clang::FunctionDecl, clang::VarDecl>(declaration) &&
           llvm::isa<clang::AsmLabelAttr>(attribute)) ||
          (llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl>(
               declaration) &&
           llvm::isa<clang::ObjCRuntimeNameAttr>(attribute)))
        continue;
      if (llvm::isa<clang::ObjCRequiresCleanupAttr, clang::ObjCResourceFamilyAttr,
                    clang::ObjCInvalidAttr, clang::ObjCRetainableFamilyAttr,
                    clang::ObjCNSObjectAttr>(attribute) &&
          llvm::isa<clang::TypedefNameDecl>(declaration))
        continue;
      if (llvm::isa<clang::ObjCFamilyRetainAttr, clang::ObjCFamilyReleaseAttr,
                    clang::ObjCFamilyAutoreleaseAttr>(attribute) &&
          llvm::isa<clang::FunctionDecl>(declaration))
        continue;
      if (llvm::isa<clang::ObjCReturnsBorrowedAttr, clang::NoThrowAttr>(attribute) &&
          llvm::isa<clang::FunctionDecl, clang::ObjCMethodDecl>(declaration))
        continue;
      if (llvm::isa<clang::NoEscapeAttr, clang::NSConsumedAttr, clang::CFConsumedAttr>(attribute) &&
          llvm::isa<clang::ParmVarDecl>(declaration))
        continue;
      if (llvm::isa<clang::NSConsumesSelfAttr>(attribute) &&
          llvm::isa<clang::ObjCMethodDecl>(declaration))
        continue;
      if (llvm::isa<clang::ObjCExecutorAttr, clang::ObjCNonisolatedAttr,
                    clang::ObjCUnsafeSendableAttr, clang::ObjCTaskSpawnAttr,
                    clang::ObjCTaskDetachedAttr, clang::ObjCTaskGroupAttr>(attribute))
        continue;
      if (llvm::isa<clang::DeprecatedAttr, clang::UnavailableAttr,
                    clang::AvailabilityAttr>(attribute))
        continue;
      if (llvm::isa<clang::ObjCMethodDecl>(declaration) &&
          llvm::isa<clang::ObjCDirectAttr>(attribute))
        continue;
      if (llvm::isa<clang::FunctionDecl, clang::ObjCMethodDecl>(declaration) &&
          llvm::isa<clang::NSReturnsRetainedAttr, clang::NSReturnsNotRetainedAttr,
                    clang::NSReturnsAutoreleasedAttr>(attribute))
        continue;
      if (llvm::isa<clang::ObjCInterfaceDecl>(declaration) &&
          llvm::isa<clang::ObjCRootClassAttr>(attribute))
        continue;
      AddDiagnostic(result_, "O3IFC116",
                    (llvm::Twine("public declaration has an attribute not yet modeled in the sidecar: ") +
                     attribute->getSpelling()).str(),
                    declaration->getQualifiedNameAsString());
      return true;
    }

    // Preserve declaration/redeclaration order, including incomplete tag
    // declarations needed by signatures that precede the eventual definition.
    // The sidecar still records one semantic identity per USR below.
    if ((IsTopLevel(declaration) || IsOutOfLineDefaultedDefinition(declaration)) &&
        (!FriendOwner(declaration) ||
         (llvm::isa<clang::FunctionDecl>(declaration) &&
          !declaration->getLexicalDeclContext()->isRecord())) &&
        !llvm::isa<clang::ObjCMethodDecl, clang::ObjCPropertyDecl,
                   clang::ObjCIvarDecl, clang::FieldDecl,
                   clang::EnumConstantDecl>(declaration))
      if (!llvm::isa<clang::NamespaceDecl>(declaration) ||
          namespace_roots_.insert(ReadInterfaceDeclarationStableId(declaration)).second)
        // An unnamed tag belongs to its typedef's declaration group. Printing
        // it separately loses that ownership and invents a named tag when the
        // typedef is reparsed. The typedef printer emits its owned definition.
        if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration);
            !tag || tag->getIdentifier() || !tag->getTypedefNameForAnonDecl())
          root_declarations_.push_back(declaration);

    InterfaceDeclarationIdentity identity;
    if (!ReadInterfaceDeclarationIdentity(declaration, context_, identity)) {
      AddDiagnostic(result_, identity.stable_id.empty() ? "O3IFC117" : "O3IFC118",
                    identity.stable_id.empty()
                        ? "Clang could not derive a stable declaration USR"
                        : "declaration has no canonical type identity supported by the interface contract",
                    identity.stable_id.empty()
                        ? declaration->getQualifiedNameAsString() : identity.stable_id);
      return true;
    }
    if (identity.effects.ownership_identity.find("!unmodeled") != std::string::npos) {
      AddDiagnostic(result_, "O3IFC170", "public ownership contract has no stable interface representation",
                    identity.stable_id);
      return true;
    }
    if (!IsValidAsyncABIIdentity(identity.effects)) {
      AddDiagnostic(result_, "O3IFC025", "public async type has no complete physical ABI identity",
                    identity.stable_id);
      return true;
    }
    auto *representative = Representative(declaration);
    const std::string stable_id = identity.stable_id;
    uses_function_effects_ |=
        CheckTypeDependencies(representative, context_, result_, resolved_ids_,
                              uses_runtime_frame_, uses_cxx_async_bodies_, exports_, source_root_,
                              &uses_typed_keypaths_, &uses_keypath_subscripts_);

    const auto found = declarations_.find(stable_id);
    if (found == declarations_.end() ||
        (!IsDefinition(found->second) && IsDefinition(representative)))
      declarations_[stable_id] = representative;
    if (const auto prior = identities_.find(stable_id);
        prior != identities_.end() &&
        prior->second.qualified_name != identity.qualified_name) {
      AddDiagnostic(result_, "O3IFC180",
                    "one native declaration cannot publish multiple lookup names",
                    identity.qualified_name);
      return true;
    }
    identities_[stable_id] = std::move(identity);
    return true;
  }

  std::vector<clang::NamedDecl *> RootDeclarations() const {
    auto roots = root_declarations_;
    const auto &source_manager = context_.getSourceManager();
    std::stable_sort(roots.begin(), roots.end(),
                     [&](const auto *a, const auto *b) {
                       return source_manager.isBeforeInTranslationUnit(
                           source_manager.getExpansionLoc(a->getLocation()),
                           source_manager.getExpansionLoc(b->getLocation()));
                     });
    return roots;
  }

  std::vector<InterfaceDeclarationIdentity> Identities() const {
    std::vector<InterfaceDeclarationIdentity> result;
    result.reserve(identities_.size());
    for (const auto &[_, identity] : identities_) result.push_back(identity);
    return result;
  }

  bool UsesCanonicalFunctionEffects() const { return uses_function_effects_; }
  bool UsesRuntimeFrame() const { return uses_runtime_frame_; }
  bool UsesCxxAsyncBodies() const { return uses_cxx_async_bodies_; }
  bool UsesTypedKeyPaths() const { return uses_typed_keypaths_; }
  bool UsesKeyPathSubscripts() const { return uses_keypath_subscripts_; }

private:
  clang::ASTContext &context_;
  InterfaceEmissionArtifact &result_;
  const std::set<std::string> &resolved_ids_;
  const std::vector<std::string> exports_;
  std::string source_root_;
  bool IsInMainFile(const clang::ASTContext &context,
                    clang::SourceLocation location) const {
    return IsModuleOwnedSource(context, location, source_root_);
  }
  std::map<std::string, clang::NamedDecl *> declarations_;
  std::map<std::string, InterfaceDeclarationIdentity> identities_;
  std::vector<clang::NamedDecl *> root_declarations_;
  std::set<std::string> namespace_roots_;
  bool uses_function_effects_ = false;
  bool uses_runtime_frame_ = false;
  bool uses_cxx_async_bodies_ = false;
  bool uses_typed_keypaths_ = false;
  bool uses_keypath_subscripts_ = false;
};

class LocalDefinitionPrintingCallbacks final : public clang::PrintingCallbacks {
public:
  explicit LocalDefinitionPrintingCallbacks(const clang::NamedDecl *declaration) {
    if (const auto *scope = llvm::dyn_cast<clang::DeclContext>(declaration))
      visible_.insert(scope);
    for (const auto *scope = declaration->getDeclContext(); scope;
         scope = scope->getParent()) visible_.insert(scope);
  }
  bool isScopeVisible(const clang::DeclContext *scope) const override {
    const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(scope);
    return visible_.count(scope) || scope->isFunctionOrMethod() ||
           (record && record->isLambda());
  }
private:
  std::set<const clang::DeclContext *> visible_;
};

void PrintUnscopedRootDeclaration(clang::NamedDecl *declaration,
                          clang::ASTContext &context,
                          llvm::raw_ostream &out, bool declaration_only = false) {
  const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
  const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
  const auto original_storage=variable?variable->getStorageClass():clang::SC_None;
  bool external_specialization=false;
  auto restore_storage=llvm::scope_exit([&]{
    if (external_specialization) const_cast<clang::VarDecl *>(variable)->setStorageClass(original_storage);
  });
  const bool c_linkage =
      context.getLangOpts().CPlusPlus && ((function && function->isExternC()) ||
                                          (variable && variable->isExternC()));
  if (c_linkage)
    out << "extern \"C\" {\n";
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.PrintAsCanonical = false;
  policy.FullyQualifiedName = true;
  policy.SuppressScope = false;
  policy.SuppressUnwrittenScope = false;
  policy.PolishForDeclaration = true;
  policy.IncludeTagDefinition = HasDeclarationUnnamedLocalType(declaration);
  LocalDefinitionPrintingCallbacks local_scopes(declaration);
  if (HasDeclarationLocalType(declaration)) {
    // A local type can be qualified by its local record or enum, but never
    // by a function spelling such as `api(int)::Value`. Enclosing scopes are
    // already visible where this root's canonical definition is printed.
    policy.Callbacks = &local_scopes;
    policy.FullyQualifiedName = false;
  }
  if (const auto *alias = llvm::dyn_cast<clang::TypedefNameDecl>(declaration))
    if (const auto *tag = alias->getUnderlyingType()->getAs<clang::TagType>();
        tag && !tag->getDecl()->getIdentifier() &&
        tag->getDecl()->getTypedefNameForAnonDecl() == alias)
      policy.IncludeTagDefinition = true;
  // Member names inside a record must be relative to that record. Fully
  // qualified function spelling would print invalid `Worker::get` inside it.
  if (llvm::isa<clang::CXXRecordDecl, clang::ClassTemplateDecl>(declaration) ||
      IsDetachedCallableTemplateRoot(declaration) || IsDetachedStaticDataTemplateRoot(declaration) ||
      declaration->getDeclContext()->getRedeclContext()->isNamespace())
    policy.FullyQualifiedName = false;

  if (llvm::isa<clang::FunctionDecl>(declaration)) {
    policy.TerseOutput = declaration_only || !PublishesFunctionBody(function);
    // Print modeled ownership exactly once, independently of whether the
    // original declaration placed its GNU attribute before or after the type.
    // Declaration polishing retains modeled native availability while the
    // ownership attribute below remains canonical across ARC and MRC.
    policy.SuppressDeclAttributes = false;
  }
  if (llvm::isa<clang::VarDecl>(declaration)) {
    policy.SuppressInitializers = !PublishesVariableInitializer(variable);
    if (policy.SuppressInitializers &&
        IsExplicitNamespaceVariableSpecialization(llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(variable)) &&
        !variable->isInline()) {
      // Clang's existing declaration-only specialization extension carries no
      // initializer and requires no default constructor. Scope its extension
      // diagnostic to generated replay; ordinary source diagnostics stay intact.
      external_specialization=true;
      const_cast<clang::VarDecl *>(variable)->setStorageClass(clang::SC_Extern);
      out << "#pragma clang diagnostic push\n"
             "#pragma clang diagnostic ignored \"-Wexplicit-specialization-storage-class\"\n";
    } else if (policy.SuppressInitializers && variable->getStorageClass() == clang::SC_None &&
               !IsConcreteMemberVariableSpecialization(llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(variable)) &&
               !IsClosedParentMemberSpecialization(variable))
      out << "extern ";
  }
  // Clang's declaration-polishing policy omits non-keyword attributes.
  // Emit this modeled class contract through its native attribute printer.
  if (const auto *interface =
          llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration))
    if (const auto *root = interface->getAttr<clang::ObjCRootClassAttr>()) {
      root->printPretty(out, policy);
      out << '\n';
    }
  if (llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl>(declaration))
    if (const auto *runtime =
            declaration->getAttr<clang::ObjCRuntimeNameAttr>()) {
      runtime->printPretty(out, policy);
      out << '\n';
    }
  auto *body_function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
  clang::AttrVec saved_attributes;
  std::vector<std::pair<clang::ParmVarDecl *, bool>> saved_defaults;
  const bool has_label_definition = !declaration_only && body_function &&
      ((PublishesFunctionBody(body_function) && body_function->doesThisDeclarationHaveABody()) ||
       IsOutOfLineDefaultedDefinition(body_function)) &&
      body_function->hasAttr<clang::AsmLabelAttr>();
  auto restore_definition = llvm::scope_exit([&] {
    if (has_label_definition) {
      body_function->dropAttrs();
      body_function->setAttrs(saved_attributes);
    }
    for (const auto &[parameter, inherited] : saved_defaults)
      parameter->setHasInheritedDefaultArg(inherited);
  });
  if (has_label_definition) {
    // Reparsed canonical source already has the label declaration. Do not
    // accumulate another declaration on every serialization round trip.
    const bool needs_label_declaration = !body_function->getPreviousDecl() ||
        !body_function->getPreviousDecl()->hasAttr<clang::AsmLabelAttr>();
    saved_attributes = body_function->getAttrs();
    body_function->dropAttr<clang::AsmLabelAttr>();
    for (auto *parameter : body_function->parameters())
      if (needs_label_declaration && parameter->hasDefaultArg()) {
        saved_defaults.emplace_back(parameter, parameter->hasInheritedDefaultArg());
        parameter->setHasInheritedDefaultArg();
      }
  }
  declaration->print(out, policy);
  if (llvm::isa<clang::FunctionDecl>(declaration) && !PublishesFunctionBody(function)) {
    // Use one suffix for the declaration attribute in MRC and the canonical
    // produces-result function type bit in ARC.
    const auto ownership = ReadNativeReturnOwnership(declaration);
    if (ownership == "native-return:retained") out << " __attribute__((ns_returns_retained))";
    else if (ownership == "native-return:not-retained") out << " __attribute__((ns_returns_not_retained))";
    else if (ownership == "native-return:autoreleased") out << " __attribute__((ns_returns_autoreleased))";
  }
  if (llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl,
                clang::ObjCCategoryDecl>(declaration)) {
    out << '\n';
    return;
  }
  out << ";\n";
  if (external_specialization) out << "#pragma clang diagnostic pop\n";
  if (c_linkage)
    out << "}\n";
}

void PrintRootDeclaration(clang::NamedDecl *declaration,
                          clang::ASTContext &context,
                          llvm::raw_ostream &out, bool declaration_only = false) {
  // A synthesized label declaration is its own canonical root, including its
  // namespace wrappers. Reparsing otherwise splits one wrapper into two roots.
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
      !declaration_only && PublishesFunctionBody(function) &&
      function->doesThisDeclarationHaveABody() && function->hasAttr<clang::AsmLabelAttr>() &&
      (!function->getPreviousDecl() || !function->getPreviousDecl()->hasAttr<clang::AsmLabelAttr>()))
    PrintRootDeclaration(declaration, context, out, true);
  std::vector<const clang::NamespaceDecl *> scopes;
  for (const auto *dc = declaration->getDeclContext(); dc; dc = dc->getParent())
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(dc))
      scopes.push_back(space);
  std::reverse(scopes.begin(), scopes.end());
  for (const auto *space : scopes) {
    if (IsStandardCoroutineTraitsSpecialization(declaration))
      out << (space->isInline()?"inline namespace ":"namespace ") << space->getName();
    else PrintNamespaceHead(space, out, false);
    out << " {\n";
  }
  if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration)) {
    PrintNamespaceHead(space, out, true);
    out << " {}\n";
  }
  else {
    auto *record=llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
    const auto record_qualifier=record?record->getQualifierLoc():clang::NestedNameSpecifierLoc();
    auto restore_record=llvm::scope_exit([&]{if(record)record->setQualifierInfo(record_qualifier);});
    if(record && !scopes.empty() && !IsDetachedNestedTemplateDefinition(record) &&
       !IsConcreteMemberClassSpecialization(llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(record)) &&
       !IsDependentMemberPartial(record) && !IsClosedParentMemberSpecialization(record) && !IsClosedParentPattern(record))
      record->setQualifierInfo({});
    auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
    const auto qualifier = function ? function->getQualifierLoc()
                                    : clang::NestedNameSpecifierLoc();
    auto restore = llvm::make_scope_exit([&] {
      if (function) function->setQualifierInfo(qualifier);
    });
    // Out-of-line source definitions retain an explicit qualifier even with
    // FullyQualifiedName disabled. The enclosing wrappers already supply it.
    if (function && !scopes.empty() && !IsDetachedCallableTemplateDefinition(function) &&
        !IsConcreteMemberFunctionSpecialization(function) && !IsClosedParentMemberSpecialization(function))
      function->setQualifierInfo({});
    PrintUnscopedRootDeclaration(declaration, context, out, declaration_only);
  }
  for (const auto *space : scopes) {
    (void)space;
    out << "}\n";
  }
}

std::vector<std::string> RoundTripArguments(const clang::ASTContext &context) {
  const auto &lang = context.getLangOpts();
  const auto &standard = clang::LangStandard::getLangStandardForKind(lang.LangStd);
  std::vector<std::string> args = {
      "-fsyntax-only", "--target=" + context.getTargetInfo().getTriple().str(),
      std::string("-x"),
      lang.ObjC ? (lang.CPlusPlus ? "objective-c++" : "objective-c")
                : (lang.CPlusPlus ? "c++" : "c"),
      "-std=" + std::string(standard.getName())};
  if (lang.ObjC) {
    args.push_back("-fobjc-runtime=" + lang.ObjCRuntime.getAsString());
    if (lang.ObjCAutoRefCount) args.push_back("-fobjc-arc");
  }
  // Reuse the producer's compiler resource headers for complete C++ runtime
  // reparses. clang-tool is a virtual driver name, so its default executable
  // search cannot locate an installed SDK's builtin headers after relocation.
  const auto &source=context.getSourceManager();
  for (auto it=source.fileinfo_begin();it!=source.fileinfo_end();++it) {
    if (!it->second->OrigEntry) continue;
    const auto name=it->second->OrigEntry->getName();
    const auto leaf=llvm::sys::path::filename(name);
    const auto includes=llvm::sys::path::parent_path(name);
    if ((leaf=="stddef.h" || leaf.starts_with("__stddef_")) &&
        llvm::sys::path::filename(includes)=="include") {
      args.push_back("-resource-dir="+llvm::sys::path::parent_path(includes).str());
      break;
    }
  }
  if (lang.CoroAlignedAllocation) args.push_back("-fcoro-aligned-allocation");
  if (lang.Blocks) args.push_back("-fblocks");
  if (lang.ShortEnums) args.push_back("-fshort-enums");
  if (lang.ObjC3) {
    args.push_back("-fobjc-version=3");
    static constexpr const char *profiles[] = {
        "compatibility", "strict", "strict-concurrency", "strict-system"};
    const auto index = static_cast<unsigned>(lang.ObjC3Profile);
    if (index < std::size(profiles))
      args.push_back(std::string("-fobjc3-profile=") + profiles[index]);
    if (lang.ObjC3StrictConcurrency)
      args.push_back("-fobjc3-strict-concurrency");
  }
  return args;
}

bool SameDeclaration(const InterfaceDeclarationIdentity &a,
                     const InterfaceDeclarationIdentity &b) {
  return std::tie(a.stable_id, a.kind, a.qualified_name, a.export_root, a.spi_group,
                  a.native_symbol_name, a.canonical_type_identity,
                  a.effects.is_async, a.effects.is_throwing,
                  a.effects.throws_kind, a.effects.error_type_identity,
                  a.effects.actor_isolation_identity,
                  a.effects.executor_affinity_identity,
                  a.effects.ownership_identity,
                  a.effects.async_abi_identity) ==
         std::tie(b.stable_id, b.kind, b.qualified_name, b.export_root, b.spi_group,
                  b.native_symbol_name, b.canonical_type_identity,
                  b.effects.is_async, b.effects.is_throwing,
                  b.effects.throws_kind, b.effects.error_type_identity,
                  b.effects.actor_isolation_identity,
                  b.effects.executor_affinity_identity,
                  b.effects.ownership_identity,
                  b.effects.async_abi_identity);
}

}  // namespace

std::string ReadInterfaceExportRoot(const clang::NamedDecl *declaration) {
  return ExportRootName(declaration);
}

std::string ReadInterfaceSPIGroup(const clang::NamedDecl *declaration) {
  if (const auto *owner = FriendOwner(declaration)) declaration = owner;
  while (declaration && !IsTopLevel(declaration))
    declaration = llvm::dyn_cast<clang::NamedDecl>(declaration->getDeclContext());
  for (const clang::DeclContext *scope = declaration ? declaration->getDeclContext() : nullptr;
       declaration;) {
    const auto *root = Representative(const_cast<clang::NamedDecl *>(declaration));
    for (const auto *attribute : root->specific_attrs<clang::AnnotateAttr>())
      if (attribute->getAnnotation().starts_with("objc3.spi:"))
        return attribute->getAnnotation().drop_front(10).str();
    declaration = nullptr;
    for (; scope; scope = scope->getParent())
      if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(scope)) {
        declaration = space;
        scope = scope->getParent();
        break;
      }
  }
  return {};
}

std::vector<clang::NamedDecl *> ReadDetachedCallableTemplateDefinitions(const clang::NamedDecl *owner) {
  return DetachedCallableTemplateDefinitions(owner);
}
std::vector<clang::NamedDecl *> ReadDetachedStaticDataTemplateDefinitions(const clang::NamedDecl *owner) {
  return DetachedStaticDataTemplateDefinitions(owner);
}
std::vector<clang::NamedDecl *> ReadClosedParentPatterns(const clang::NamedDecl *owner) {
  return ClosedParentPatterns(owner);
}
std::vector<clang::NamedDecl *> ReadClosedParentMemberSpecializations(const clang::NamedDecl *owner) {
  return ClosedParentMemberSpecializations(owner);
}
std::vector<clang::NamedDecl *> ReadDependentMemberPartials(const clang::NamedDecl *owner) {
  return DependentMemberPartials(owner);
}
std::vector<clang::NamedDecl *> ReadDetachedNestedTemplateDefinitions(const clang::NamedDecl *owner) {
  return DetachedNestedTemplateDefinitions(owner);
}
std::vector<clang::ClassTemplateSpecializationDecl *> ReadConcreteMemberClassSpecializations(
    const clang::NamedDecl *owner) {
  return ConcreteMemberClassSpecializations(owner);
}
std::vector<clang::FunctionDecl *> ReadConcreteMemberFunctionSpecializations(const clang::NamedDecl *owner) {
  return ConcreteMemberFunctionSpecializations(owner);
}
std::vector<clang::VarTemplateSpecializationDecl *> ReadConcreteMemberVariableSpecializations(const clang::NamedDecl *owner) {
  return ConcreteMemberVariableSpecializations(owner);
}

std::string ReadInterfaceLanguageProfile(const clang::LangOptions &lang) {
  static constexpr const char *profiles[] = {
      "compatibility", "strict", "strict-concurrency", "strict-system"};
  const auto index = static_cast<unsigned>(lang.ObjC3Profile);
  const auto &standard =
      clang::LangStandard::getLangStandardForKind(lang.LangStd);
  return std::string("objc3=") + (lang.ObjC3 ? "enabled" : "disabled") +
         ";profile=" +
         (index < std::size(profiles) ? profiles[index] : "unknown") +
         ";strict-concurrency=" +
         (lang.ObjC3StrictConcurrency ? "true" : "false") +
         ";arc=" + (lang.ObjCAutoRefCount ? "true" : "false") + ";language=" +
         (lang.ObjC ? (lang.CPlusPlus ? "objective-c++" : "objective-c")
                    : (lang.CPlusPlus ? "c++" : "c")) +
         ";standard=" + standard.getName() +
         ";blocks=" + (lang.Blocks ? "true" : "false");
}

std::string ReadInterfaceDeclarationStableId(
    const clang::NamedDecl *declaration) {
  if (!declaration) return {};
  if (const auto *alias = llvm::dyn_cast<clang::TypeAliasDecl>(declaration))
    if (const auto *pattern = alias->getDescribedAliasTemplate())
      return ReadInterfaceDeclarationStableId(pattern);
  if (const auto *pattern = llvm::dyn_cast<clang::ClassTemplateDecl>(declaration)) {
    if (!UnnamedTemplateRecordKey(pattern->getTemplatedDecl(),pattern->getASTContext()).empty()) {
      llvm::SmallString<128> usr;
      if (clang::index::generateUSRForDecl(pattern,usr))return {};
      return usr.str().str();
    }
    return "c:@ClassTemplate@" + pattern->getQualifiedNameAsString();
  }
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration))
    if (const auto *pattern = record->getDescribedClassTemplate())
      return ReadInterfaceDeclarationStableId(pattern);
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(declaration)) {
    if (!UnnamedTemplateRecordKey(llvm::dyn_cast<clang::CXXRecordDecl>(pattern->getDeclContext()),pattern->getASTContext()).empty()) {
      llvm::SmallString<128> usr;
      if (clang::index::generateUSRForDecl(pattern,usr))return {};
      return usr.str().str();
    }
    return "c:@AliasTemplate@" + pattern->getQualifiedNameAsString();
  }
  if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
    if (const auto *pattern=variable->getDescribedVarTemplate()) return ReadInterfaceDeclarationStableId(pattern);
  if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration);
      IsExplicitNamespaceVariableSpecialization(specialization)) {
    std::string key;
    for (const auto &argument:specialization->getTemplateArgs().asArray())
      if (!GenericArgumentKey(argument,specialization->getASTContext(),key)) return {};
    return "c:@VariableTemplateSpecialization@"+
        ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate())+"@arguments@"+
        ComputeInterfaceSourceSha256(key);
  }
  if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(declaration))
    return "c:@VariableTemplate@" + pattern->getQualifiedNameAsString();
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    if (const auto *pattern = function->getDescribedFunctionTemplate())
      return ReadInterfaceDeclarationStableId(pattern);
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
      IsExplicitNamespaceFunctionSpecialization(function)) {
    const auto *arguments=function->getTemplateSpecializationArgs();
    if (!arguments) return {};
    std::string key;
    for (const auto &argument:arguments->asArray())
      if (!GenericArgumentKey(argument,function->getASTContext(),key)) return {};
    return "c:@FunctionTemplateSpecialization@"+
        ReadInterfaceDeclarationStableId(function->getPrimaryTemplate())+"@arguments@"+
        ComputeInterfaceSourceSha256(key);
  }
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(declaration)) {
    llvm::SmallString<128> usr;
    if (clang::index::generateUSRForDecl(pattern, usr)) return {};
    return usr.str().str() + "@constraints@" + FunctionTemplateConstraintKey(pattern);
  }
  if(const auto *using_enum=llvm::dyn_cast<clang::UsingEnumDecl>(declaration)) {
    const auto *owner=llvm::dyn_cast<clang::NamedDecl>(using_enum->getDeclContext());
    return "c:@UsingEnum@"+(owner?ReadInterfaceDeclarationStableId(owner):std::string("TU"))+
           "@"+ReadInterfaceDeclarationStableId(using_enum->getEnumDecl());
  }
  if(const auto *directive=llvm::dyn_cast<clang::UsingDirectiveDecl>(declaration)) {
    const auto *owner=llvm::dyn_cast<clang::NamedDecl>(directive->getDeclContext());
    return "c:@UDirective@"+(owner?ReadInterfaceDeclarationStableId(owner):std::string("TU"))+
           "@"+ReadInterfaceDeclarationStableId(directive->getNominatedNamespaceAsWritten());
  }
  llvm::SmallString<128> usr;
  if (clang::index::generateUSRForDecl(declaration, usr)) return {};
  // Native USRs encode filenames for C typedefs because those names have no
  // linkage. Module provenance provides the provider identity; physical source
  // and canonical artifact filenames must not change the exported type name.
  if (llvm::isa<clang::TypedefNameDecl>(declaration) &&
      declaration->getDeclContext()->getRedeclContext()->isTranslationUnit())
    return "c:@T@" + declaration->getNameAsString();
  if (llvm::isa<clang::TypedefDecl>(declaration))
    if (const auto *space=llvm::dyn_cast<clang::NamespaceDecl>(
            declaration->getDeclContext()->getRedeclContext()))
      return ReadInterfaceDeclarationStableId(space)+"@T@"+declaration->getNameAsString();
  if(const auto *using_decl=llvm::dyn_cast<clang::UsingDecl>(declaration)) {
    std::set<std::string> targets;
    for(const auto *shadow:using_decl->shadows())
      targets.insert(ReadInterfaceDeclarationStableId(shadow->getTargetDecl()->getUnderlyingDecl()));
    std::string key;
    for(const auto &target:targets) key+=std::to_string(target.size())+":"+target;
    return usr.str().str()+"@targets@"+ComputeInterfaceSourceSha256(key);
  }
  return usr.str().str();
}

bool ReadInterfaceDeclarationIdentity(
    clang::NamedDecl *declaration, const clang::ASTContext &context,
    InterfaceDeclarationIdentity &identity) {
  identity = {};
  if (!declaration) return false;
  const auto stable_id = ReadInterfaceDeclarationStableId(declaration);
  if (stable_id.empty()) return false;
  auto *representative = Representative(declaration);
  identity.stable_id = stable_id;
  identity.spi_group = ReadInterfaceSPIGroup(representative);
  identity.native_symbol_name = ReadNativeSymbolName(representative);
  identity.kind = declaration->getDeclKindName();
  identity.export_root = ExportRootName(declaration);
  identity.qualified_name = InterfaceQualifiedName(declaration);
  if (identity.qualified_name.empty())
    identity.qualified_name = declaration->getNameAsString();
  identity.canonical_type_identity =
      DeclarationTypeIdentity(representative, context);
  if (identity.canonical_type_identity.empty()) return false;
  if (HasEffectfulTemplateProjection(representative,context))
    identity.canonical_type_identity+=";cxx-template-callable-projections:v1;";
  if (!llvm::isa<clang::NamespaceDecl>(representative)) {
    RuntimeDeclarationFinder arguments(context);VisitPublishedRuntimeContracts(representative,arguments);
    if (arguments.HasCxxCoroutines)
      identity.canonical_type_identity+=";cxx-coroutine-bodies:v1;";
    if (!arguments.StandardCoroutineHeaders.empty()) {
      std::string headers;for (const auto &header:arguments.StandardCoroutineHeaders) headers+=header;
      identity.canonical_type_identity+=";cxx-standard-coroutines:v1;headers="+ComputeInterfaceSourceSha256(headers)+";";
    }
    if (arguments.FactoryTypes.nested)
      identity.canonical_type_identity+=";cxx-nested-factory-types:v1;";
    if (arguments.FactoryTypes.lambda_result)
      identity.canonical_type_identity+=";cxx-lambda-factory-results:v1;";
    if (arguments.FactoryTypes.closure)
      identity.canonical_type_identity+=";cxx-closure-result-projections:v1;";
    if (arguments.HasFactoryValueArguments)
      identity.canonical_type_identity+=";cxx-factory-value-arguments:v1;";
    if (arguments.FactoryTypes.generic)
      identity.canonical_type_identity+=";cxx-generic-factory-projections:v1;";
    if (arguments.HasAddressArguments)
      identity.canonical_type_identity+=";cxx-address-template-arguments:v1;";
    if (arguments.HasFactoryArguments ||
        (llvm::isa<clang::TemplateDecl>(representative) && HasDeclarationFactoryTypeProjection(representative)))
      identity.canonical_type_identity+=";cxx-factory-template-arguments:v1;";
  }
  if (llvm::isa<clang::TypeAliasTemplateDecl>(representative) &&
      !identity.native_symbol_name.empty())
    identity.canonical_type_identity+=";cxx-nominal-alias-templates:v1;owner="+
        identity.native_symbol_name+";";
  if (const auto *specialization=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(representative);
      IsExplicitNamespaceVariableSpecialization(specialization)) {
    std::string key;
    for (const auto &argument:specialization->getTemplateArgs().asArray())
      if (!GenericArgumentKey(argument,context,key)) return false;
    if (!specialization->isInline() || !specialization->hasInit())
      identity.canonical_type_identity+=";cxx-variable-specialization-storage:v1;delivery="+
          std::string(specialization->isInline()?"inline-zero":"external")+";";
    identity.canonical_type_identity+=";cxx-variable-specializations:v1;;cxx-variable-templates:v1;primary="+
        ReadInterfaceDeclarationStableId(specialization->getSpecializedTemplate())+";arguments="+
        ComputeInterfaceSourceSha256(key)+";";
  }
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(representative);
      IsExplicitNamespaceFunctionSpecialization(function)) {
    std::string key;
    for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
      if (!GenericArgumentKey(argument,context,key)) return false;
    identity.canonical_type_identity+=";cxx-function-specializations:v1;;cxx-function-templates:v2;primary="+
        ReadInterfaceDeclarationStableId(function->getPrimaryTemplate())+";arguments="+
        ComputeInterfaceSourceSha256(key)+";";
  }
  const auto closed_patterns=ClosedParentPatterns(representative);
  if (!closed_patterns.empty()) {
    std::set<std::string> definitions;
    for (const auto *pattern:closed_patterns) {
      std::string parent_arguments,text;
      const auto *parent=llvm::cast<clang::ClassTemplateSpecializationDecl>(pattern->getDeclContext());
      if (!ClassSpecializationArgumentKey(parent,context,parent_arguments)) return false;
      llvm::raw_string_ostream stream(text);
      PrintRootDeclaration(const_cast<clang::NamedDecl *>(pattern),const_cast<clang::ASTContext &>(context),stream);
      definitions.insert(ReadInterfaceDeclarationStableId(pattern)+":parent="+parent_arguments+
          ":definition="+ComputeInterfaceSourceSha256(text));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-closed-parent-patterns:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto closed_members=ClosedParentMemberSpecializations(representative);
  if (!closed_members.empty()) {
    std::set<std::string> definitions;
    for (const auto *member:closed_members) {
      std::string parent_arguments,text;
      const auto *parent=llvm::cast<clang::ClassTemplateSpecializationDecl>(member->getDeclContext());
      if (!ClassSpecializationArgumentKey(parent,context,parent_arguments)) return false;
      llvm::raw_string_ostream stream(text);
      PrintRootDeclaration(const_cast<clang::NamedDecl *>(member),const_cast<clang::ASTContext &>(context),stream);
      std::string body;
      if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(member); function && function->isInlined())
        body=FunctionDefinitionContract(function,context);
      definitions.insert(ReadInterfaceDeclarationStableId(member)+":parent="+parent_arguments+
          ":declaration="+ComputeInterfaceSourceSha256(text)+":body="+body);
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-closed-parent-members:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto dependent_partials=DependentMemberPartials(representative);
  if (!dependent_partials.empty()) {
    std::set<std::string> definitions;
    for (const auto *partial:dependent_partials) {
      std::string text;llvm::raw_string_ostream stream(text);
      clang::PrintingPolicy policy(context.getLangOpts());
      policy.PrintAsCanonical=true;policy.PolishForDeclaration=true;
      partial->print(stream,policy);
      definitions.insert(ReadInterfaceDeclarationStableId(partial)+":"+ComputeInterfaceSourceSha256(text));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-dependent-member-partials:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto member_variables=ConcreteMemberVariableSpecializations(representative);
  if (!member_variables.empty()) {
    std::set<std::string> specializations;
    for (const auto *variable:member_variables) {
      std::string arguments;
      const auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(variable);
      if (partial) arguments=VariablePartialArgumentKey(partial,context);
      else for (const auto &argument:variable->getTemplateArgs().asArray())
        if (!GenericArgumentKey(argument,context,arguments)) return false;
      DeclarationExpressionIdentity initializer(context);
      if (partial) for (const auto *parameter:*partial->getTemplateParameters()) initializer.Local(parameter);
      specializations.insert(ReadInterfaceDeclarationStableId(variable->getSpecializedTemplate())+
          ":arguments="+ComputeInterfaceSourceSha256(arguments)+":type="+CanonicalType(variable->getType(),context)+
          ":symbol="+(partial?ReadNativeSymbolName(variable->getSpecializedTemplate()):ReadNativeSymbolName(variable))+
          ":inline="+std::to_string(variable->isInline())+":constexpr="+std::to_string(variable->isConstexpr())+
          ":tls="+std::to_string(variable->getTSCSpec())+":delivery="+
          ((partial || PublishesVariableInitializer(variable))?"consumer":"provider")+":init="+
          ((partial || PublishesVariableInitializer(variable)) && variable->hasInit()
              ?initializer.Get(const_cast<clang::Expr *>(variable->getInit())):"none"));
    }
    std::string contract;
    for (const auto &specialization:specializations) contract+=std::to_string(specialization.size())+":"+specialization;
    identity.canonical_type_identity+=";cxx-concrete-member-variable-specializations:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto member_functions=ConcreteMemberFunctionSpecializations(representative);
  if (!member_functions.empty()) {
    std::set<std::string> specializations;
    for (const auto *function:member_functions) {
      std::string arguments;
      for (const auto &argument:function->getTemplateSpecializationArgs()->asArray())
        if (!GenericArgumentKey(argument,context,arguments)) return false;
      std::string method_properties=":method-kind="+std::string(function->clang::Decl::getDeclKindName());
      if (const auto *constructor=llvm::dyn_cast<clang::CXXConstructorDecl>(function))
        method_properties+=":explicit="+std::to_string(constructor->getExplicitSpecifier().isExplicit());
      if (const auto *conversion=llvm::dyn_cast<clang::CXXConversionDecl>(function))
        method_properties+=":explicit="+std::to_string(conversion->getExplicitSpecifier().isExplicit());
      specializations.insert(ReadInterfaceDeclarationStableId(function->getPrimaryTemplate())+method_properties+
          ":arguments="+ComputeInterfaceSourceSha256(arguments)+":type="+CanonicalType(function->getType(),context)+
          ":symbol="+ReadNativeSymbolName(function)+":inline="+std::to_string(function->isInlined())+
          ":constexpr="+std::to_string(function->isConstexpr())+":consteval="+std::to_string(function->isConsteval())+
          ":deleted="+std::to_string(function->isDeleted())+":body="+
          (function->isInlined()?FunctionDefinitionContract(function,context):"provider"));
    }
    std::string contract;
    for (const auto &specialization:specializations) contract+=std::to_string(specialization.size())+":"+specialization;
    identity.canonical_type_identity+=";cxx-concrete-member-function-specializations:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
    if (std::any_of(member_functions.begin(),member_functions.end(),IsConcreteSpecialMemberSpecialization))
      identity.canonical_type_identity+=";cxx-concrete-special-member-specializations:v1;";
  }
  if (!ConcreteMemberClassSpecializations(representative).empty())
    identity.canonical_type_identity+=";cxx-concrete-member-class-specializations:v1;";
  const auto detached_records=DetachedNestedTemplateDefinitions(representative);
  if (!detached_records.empty()) {
    std::set<std::string> definitions;
    for (const auto *definition:detached_records) {
      std::string text;llvm::raw_string_ostream stream(text);
      clang::PrintingPolicy policy(context.getLangOpts());
      policy.PrintAsCanonical=true;policy.FullyQualifiedName=false;policy.PolishForDeclaration=true;
      definition->print(stream,policy);
      definitions.insert(ReadInterfaceDeclarationStableId(definition)+":"+ComputeInterfaceSourceSha256(text));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-detached-nested-templates:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto detached_data=DetachedStaticDataTemplateDefinitions(representative);
  if (!detached_data.empty()) {
    std::set<std::string> definitions;
    for (const auto *definition:detached_data) {
      const auto *variable=llvm::dyn_cast<clang::VarDecl>(definition);
      if (const auto *pattern=llvm::dyn_cast<clang::VarTemplateDecl>(definition)) variable=pattern->getTemplatedDecl();
      if (!variable) return false;
      DeclarationExpressionIdentity initializer(context);
      initializer.OuterTemplateParameters(variable);
      if (const auto *pattern=variable->getDescribedVarTemplate()) initializer.TemplateParameters(pattern);
      definitions.insert(ReadInterfaceDeclarationStableId(definition)+":"+CanonicalType(variable->getType(),context)+
          ":inline="+std::to_string(variable->isInline())+":constexpr="+std::to_string(variable->isConstexpr())+
          ":tls="+std::to_string(variable->getTSCSpec())+":init="+
          (variable->hasInit()?initializer.Get(const_cast<clang::Expr *>(variable->getInit())):"zero"));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-detached-static-data-templates:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  const auto detached=DetachedCallableTemplateDefinitions(representative);
  if (!detached.empty()) {
    std::set<std::string> definitions;
    for (const auto *definition:detached) {
      const auto *function=llvm::dyn_cast<clang::FunctionDecl>(definition);
      if (const auto *pattern=llvm::dyn_cast<clang::FunctionTemplateDecl>(definition))
        function=pattern->getTemplatedDecl();
      if (!function) return false;
      definitions.insert(ReadInterfaceDeclarationStableId(definition)+":"+
          CanonicalType(function->getType(),context)+":inline="+std::to_string(function->isInlined())+
          ":constexpr="+std::to_string(function->isConstexpr())+":consteval="+std::to_string(function->isConsteval())+
          ":body="+FunctionDefinitionContract(function,context));
    }
    std::string contract;
    for (const auto &definition:definitions) contract+=std::to_string(definition.size())+":"+definition;
    identity.canonical_type_identity+=";cxx-detached-callable-templates:v1;definitions="+
        ComputeInterfaceSourceSha256(contract)+";";
  }
  if (const auto origins=UnnamedTemplateDeclarationTypeKey(representative);!origins.empty())
    identity.canonical_type_identity+=";cxx-unnamed-template-projections:v1;origins="+ComputeInterfaceSourceSha256(origins)+";";
  if (HasDeclarationFactoryTypeProjection(representative))
    identity.canonical_type_identity+=";cxx-factory-type-projections:v1;";
  if (HasDeclarationLocalFactoryResult(representative))
    identity.canonical_type_identity+=";cxx-local-factory-results:v1;";
  if (HasDeclarationClosureFactory(representative))
    identity.canonical_type_identity+=";cxx-closure-factories:v1;";
  if (HasDeclarationEffectfulLambda(representative))
    identity.canonical_type_identity+=";cxx-effectful-lambdas:v1;";
  if (HasDeclarationTemplateTemplateParameters(representative))
    identity.canonical_type_identity+=";cxx-template-template-parameters:v1;";
  if (HasDeclarationNullableKeyPathWrite(representative))
    identity.canonical_type_identity += ";keypath-nullable-writes:v1;";
  if (HasDeclarationConcept(representative) && !llvm::isa<clang::ConceptDecl>(representative))
    identity.canonical_type_identity += ";cxx-concepts:v1;";
  if (const auto *method =
          llvm::dyn_cast<clang::CXXMethodDecl>(representative)) {
    identity.canonical_type_identity += method->isStatic()
        ? ";cxx-method=static" : ";cxx-method=instance";
    if (HasClassLifecycle(method->getParent())) {
      identity.canonical_type_identity +=
          ";cxx-class-lifecycle:v1;access=" +
          std::to_string(static_cast<unsigned>(method->getAccess())) +
          ";deleted=" +
          (method->isDeleted() ? std::string("1") : std::string("0"));
      if (const auto *ctor = llvm::dyn_cast<clang::CXXConstructorDecl>(method))
        identity.canonical_type_identity +=
            ctor->isExplicit() ? ";explicit=1" : ";explicit=0";
      if (const auto *conversion =
              llvm::dyn_cast<clang::CXXConversionDecl>(method))
        identity.canonical_type_identity +=
            conversion->isExplicit() ? ";explicit=1" : ";explicit=0";
    }
  }
  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(representative);
      method && method->isDefaulted() && !IsInClassDefaultedComparison(method))
    identity.canonical_type_identity +=
        ";cxx-class-structure:v1;defaulted=1;constexpr=" +
        (method->isOutOfLine() ? std::to_string(method->isConstexpr()) : std::string("derived")) +
        ";exception=" + std::string(method->getExceptionSpecSourceRange().isInvalid()
                        ? "derived"
                        : "explicit") +
        ";user-provided=" + std::to_string(method->isUserProvided()) +
        ";inline=" + std::to_string(method->isInlined());
  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(representative);
      method && method->isVirtual()) {
    identity.canonical_type_identity +=
        ";cxx-class-dispatch:v1;virtual=1;pure=" +
        std::to_string(method->isPureVirtual()) +
        ";final=" + std::to_string(method->hasAttr<clang::FinalAttr>());
    for (const auto *overridden : method->overridden_methods())
      identity.canonical_type_identity +=
          ";override=" + ReadInterfaceDeclarationStableId(overridden);
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(representative);
      IsInClassDefaultedComparison(function)) {
    identity.canonical_type_identity += ";cxx-defaulted-comparisons:v1;operator=" +
        std::to_string(function->getOverloadedOperator()) + ";consteval=" +
        std::to_string(function->isConsteval()) + ";deleted=" +
        std::to_string(function->isDeleted()) + ";inline=" +
        std::to_string(function->isInlined()) + ";user-provided=" +
        std::to_string(function->isUserProvided()) + ";";
    if (const auto *owner = FriendOwner(function))
      identity.canonical_type_identity += "friend-owner=" + ReadInterfaceDeclarationStableId(owner) + ";";
  }
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(representative))
    if (const auto *owner = FriendOwner(function); owner && !IsInClassDefaultedComparison(function))
      identity.canonical_type_identity += ";cxx-friends:v1;owner=" +
          ReadInterfaceDeclarationStableId(owner) + ";deleted=" +
          std::to_string(function->isDeleted()) + ";lookup=" +
          std::to_string(function->getIdentifierNamespace()) + ";";
  if (IsOutOfLineDefaultedDefinition(representative))
    identity.canonical_type_identity += ";cxx-out-of-line-defaulting:v1;";
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(representative);
      function && function->isExplicitlyDefaulted() && function->isConsteval())
    identity.canonical_type_identity += ";cxx-immediate-defaulting:v1;consteval=1;";
  if (HasDeclarationGenericLambda(representative))
    identity.canonical_type_identity += ";cxx-generic-local-lambdas:v2;";
  if (HasDeclarationUnionValue(representative))
    identity.canonical_type_identity += ";cxx-union-values:v1;";
  if (HasDeclarationLambda(representative))
    identity.canonical_type_identity += ";cxx-lambda-expressions:v1;";
  if (HasDeclarationLocalExpression(representative, true))
    identity.canonical_type_identity += ";cxx-block-expressions:v1;";
  if (HasDeclarationLocalExpression(representative, false))
    identity.canonical_type_identity += ";cxx-statement-expressions:v1;";
  if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(representative);
      context.getLangOpts().CPlusPlus && PublishesFunctionBody(function)) {
    const auto *type=clang::getObjC3CallableFunctionType(function);
    if (type && type->getObjC3FunctionEffects().Async)
      identity.canonical_type_identity += ";cxx-async-definitions:v1;";
  }
  if (HasRuntimeDeclarationContract(representative,context))
    identity.canonical_type_identity += ";sdk-runtime-declarations:v1;";
  if (HasDeclarationLocalType(representative))
    identity.canonical_type_identity += ";cxx-local-types:v1;";
  if (HasDeclarationUnnamedLocalType(representative))
    identity.canonical_type_identity += ";cxx-unnamed-local-types:v1;";
  if (context.getLangOpts().CPlusPlus && HasDeclarationThreadLocal(representative)) {
    identity.canonical_type_identity += ";cxx-thread-local:v1;";
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(representative))
      identity.canonical_type_identity += "specifier=" + std::to_string(variable->getTSCSpec()) +
          ";kind=" + std::to_string(variable->getTLSKind()) + ";";
  }
  bool namespace_attributes = false;
  if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(representative)) {
    std::string facts;
    for (const auto &attribute : NamespaceAttributes(space)) {
      namespace_attributes = true;
      facts += std::to_string(attribute.size()) + ":" + attribute;
    }
    if (!facts.empty()) identity.canonical_type_identity += ";namespace-attributes=" + HashInterfaceFacts(facts);
  }
  for (const auto *scope = representative->getDeclContext(); scope; scope = scope->getParent())
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(scope))
      namespace_attributes |= !NamespaceAttributes(space).empty();
  if (namespace_attributes)
    identity.canonical_type_identity += ";cxx-namespace-attributes:v1;visibility=" +
        std::to_string(representative->getVisibility()) + ";";
  std::vector<const clang::NamespaceDecl *> scopes;
  if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration))
    scopes.push_back(space);
  for (const auto *dc = declaration->getDeclContext(); dc; dc = dc->getParent())
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(dc))
      scopes.push_back(space);
  if (!scopes.empty()) {
    std::reverse(scopes.begin(), scopes.end());
    identity.canonical_type_identity += ";cxx-namespaces:v1;";
    for (const auto *space : scopes)
      identity.canonical_type_identity +=
          "scope=" + ReadInterfaceDeclarationStableId(space) +
          ":inline=" + std::to_string(space->isInline()) + ";";
  }
  if (llvm::isa<clang::TypeDecl>(declaration) &&
      llvm::isa<clang::CXXRecordDecl>(declaration->getDeclContext()))
    identity.canonical_type_identity +=
        std::string(";cxx-nested-types:v1;access=") +
        CxxAccessName(declaration->getAccess());
  const auto availability = AvailabilityIdentity(representative);
  if (!availability.empty())
    identity.canonical_type_identity += ";api-availability=" + HashInterfaceFacts(availability);
  if (auto *value = llvm::dyn_cast<clang::ValueDecl>(representative)) {
    identity.effects = ReadCanonicalFunctionEffects(value->getType(), context);
    const auto lifetime = value->getType().getObjCLifetime();
    if (lifetime != clang::Qualifiers::OCL_None)
      identity.effects.ownership_identity =
          std::to_string(static_cast<unsigned>(lifetime));
  }
  if (auto *type = llvm::dyn_cast<clang::TypeDecl>(representative))
    identity.effects = ReadCanonicalFunctionEffects(
        context.getTypeDeclType(type), context);
  if (auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(representative))
    identity.effects = ReadCanonicalFunctionEffects(
        method->getObjC3FunctionType(), context);
  if (auto *property = llvm::dyn_cast<clang::ObjCPropertyDecl>(representative))
    identity.effects = ReadCanonicalFunctionEffects(property->getType(), context);
  if (const auto *pattern = llvm::dyn_cast<clang::FunctionTemplateDecl>(representative)) {
    const auto *function = pattern->getTemplatedDecl();
    identity.effects = ReadCanonicalFunctionEffects(function->getType(), context);
    const auto availability = AvailabilityIdentity(function);
    if (!availability.empty())
      identity.canonical_type_identity += ";api-availability=" + HashInterfaceFacts(availability);
  }
  if (const auto *pattern = llvm::dyn_cast<clang::TypeAliasTemplateDecl>(representative))
    identity.effects = ReadCanonicalFunctionEffects(pattern->getTemplatedDecl()->getUnderlyingType(), context);
  if (const auto *pattern = llvm::dyn_cast<clang::VarTemplateDecl>(representative)) {
    const auto type = pattern->getTemplatedDecl()->getType();
    identity.effects = ReadCanonicalFunctionEffects(type, context);
    if (const auto lifetime = type.getObjCLifetime(); lifetime != clang::Qualifiers::OCL_None)
      identity.effects.ownership_identity = std::to_string(static_cast<unsigned>(lifetime));
  }
  const auto return_ownership = ReadNativeReturnOwnership(representative);
  if (!return_ownership.empty()) identity.effects.ownership_identity = return_ownership;
  ReadNativeDeclarationContracts(representative, context, identity.effects);
  const auto native_attributes=DeclarationAttributeKey(representative,true);
  if(!native_attributes.empty())
    identity.canonical_type_identity+=";native-declaration-attributes:v1;contracts="+
        ComputeInterfaceSourceSha256(native_attributes)+";";
  const auto foreign_attributes=DeclarationAttributeKey(representative);
  if(!foreign_attributes.empty())
    identity.canonical_type_identity+=";foreign-import-attributes:v1;contracts="+
        ComputeInterfaceSourceSha256(foreign_attributes)+";";

  return true;
}

namespace {
bool ValidateOwnershipProjection(clang::ASTContext &context,
                                 llvm::StringRef producer_sidecar,
                                 InterfaceEmissionArtifact &result);
}

InterfaceEmissionArtifact EmitSemanticInterface(
    clang::ASTContext &context, llvm::StringRef canonical_module_name,
    llvm::StringRef package_identity,
    const std::vector<InterfaceDependency> &resolved_dependencies,
    const std::vector<InterfaceSourceArtifact> &dependency_sources,
    llvm::StringRef ownership_projection,
    const std::vector<std::string> &exports, const std::string &source_root) {
  InterfaceEmissionArtifact result;
  if (canonical_module_name.empty() || package_identity.empty()) {
    AddDiagnostic(result, "O3IFC100",
                  "canonical module name and package identity are required");
    return result;
  }
  const auto &lang = context.getLangOpts();
  if (lang.CPlusPlus && !ApplyCxxConceptOwners(context, canonical_module_name.str(),
                                             package_identity.str(), source_root)) {
    AddDiagnostic(result, "O3IFC160", "concept owner conflicts with its semantic module");
    return result;
  }
  std::map<std::string, const InterfaceSourceArtifact *> sources_by_name;
  std::set<std::string> resolved_declaration_ids;
  for (const auto &source : dependency_sources) {
    auto [it, inserted] = sources_by_name.emplace(source.module.canonical_name, &source);
    if (!inserted && (it->second->module.semantic_fingerprint !=
                      source.module.semantic_fingerprint ||
                      it->second->source_bytes != source.source_bytes)) {
      AddDiagnostic(result, "O3IFC142",
                    "one dependency name resolves to conflicting source artifacts",
                    source.module.canonical_name);
      return result;
    }
    for (const auto &decl : source.declarations)
      resolved_declaration_ids.insert(decl.stable_id);
  }
  for (const auto &dependency : resolved_dependencies) {
    auto found = sources_by_name.find(dependency.module.canonical_name);
    if (found == sources_by_name.end() ||
        found->second->module.semantic_fingerprint != dependency.module.semantic_fingerprint ||
        found->second->module.package_identity != dependency.module.package_identity) {
      AddDiagnostic(result, "O3IFC143",
                    "resolved dependency has no matching canonical source artifact",
                    dependency.module.canonical_name);
      return result;
    }
  }
  result.identity.canonical_name = canonical_module_name.str();
  result.identity.target_triple = context.getTargetInfo().getTriple().str();
  result.identity.runtime_abi =
      (lang.ObjC ? lang.ObjCRuntime.getAsString() : std::string("no-objc-runtime")) +
      (lang.ObjCAutoRefCount ? ";arc" : ";manual-reference-counting");
  result.identity.extension_abi =
      std::string("objc3-extension-abi-") +
      std::to_string(CLANG_OBJC3_EXTENSION_ABI_REVISION) + ":" +
      clang::getObjC3PatchDigest().str();
  result.identity.language_revision = lang.ObjC3
      ? std::to_string(lang.ObjC3Revision)
      : "objc3-disabled";
  result.identity.language_profile = ReadInterfaceLanguageProfile(lang);
  result.identity.package_identity = package_identity.str();

  if (!ValidateExportNames(exports, result))
    return result;
  InterfaceCollector collector(context, result, resolved_declaration_ids,
                               exports, source_root);
  collector.TraverseDecl(context.getTranslationUnitDecl());
  collector.ValidateSelection();
  if (!result.diagnostics.empty()) return result;

  result.source_bytes = "/* Generated canonical Clang interface. */\n#pragma once\n";
  llvm::raw_string_ostream source(result.source_bytes);
  for (auto *root:collector.RootDeclarations()) {
    RuntimeDeclarationFinder finder(context);VisitPublishedRuntimeContracts(root,finder);
    if (finder.HasCxxCoroutines || !finder.StandardCoroutineHeaders.empty()) {source << "#include <coroutine>\n";break;}
  }
  if (collector.UsesCxxAsyncBodies())
    source << "#include <objc3c/runtime/objc3_task.hpp>\n";
  else if (collector.UsesRuntimeFrame())
    source << "#include <objc3c/runtime/objc3_runtime_services.h>\n";
  if (collector.UsesTypedKeyPaths())
    source << "#include <objc3c/runtime/objc3_keypath.h>\n";
  auto ordered_dependencies = resolved_dependencies;
  std::sort(ordered_dependencies.begin(), ordered_dependencies.end(),
      [](const auto &a, const auto &b) {
        return a.module.canonical_name < b.module.canonical_name;
      });
  for (const auto &dependency : ordered_dependencies)
    source << "#include \""
           << BuildInterfaceIncludePath(dependency.module.canonical_name)
           << "\"\n";
  for (auto *declaration : collector.RootDeclarations()) {
    if (auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration)) {
      if (variable->getStorageClass() == clang::SC_PrivateExtern) {
        AddDiagnostic(result, "O3IFC119",
                      "private-extern storage cannot be represented as a public interface",
                      variable->getQualifiedNameAsString());
        continue;
      }
    }
    PrintRootDeclaration(declaration, context, source);
  }
  source.flush();
  if (!result.diagnostics.empty()) {
    result.source_bytes.clear();
    return result;
  }

  if (!ownership_projection.empty() &&
      (!resolved_dependencies.empty() ||
       !ValidateOwnershipProjection(context, ownership_projection, result))) {
    if (result.diagnostics.empty())
      AddDiagnostic(result, "O3IFC150",
                    "ownership projections cannot have dependencies");
    result.source_bytes.clear();
    return result;
  }
  SemanticInterfaceSidecar sidecar;
  if (collector.UsesRuntimeFrame() || collector.UsesCxxAsyncBodies())
    sidecar.required_semantics.push_back(RuntimeInterfaceSemantic().str());
  sidecar.ownership_projection = ownership_projection.str();
  if (!ownership_projection.empty())
    sidecar.required_semantics.push_back("objc3.ownership-projection.v1");
  sidecar.module = result.identity;
  sidecar.declarations = collector.Identities();
  sidecar.dependencies = resolved_dependencies;
  for (const auto &d : sidecar.declarations) {
    if (d.native_symbol_name.empty())
      continue;
    if (!HasValidModuleOwnedName(sidecar.module, d)) {
      AddDiagnostic(
          result, "O3IFC160",
          "native symbol name differs from its module ownership contract",
          d.stable_id);
      result.source_bytes.clear();
      return result;
    }
    sidecar.required_semantics.push_back("objc3.module-owned-names.v1");
  }
  if (lang.CPlusPlus)
    sidecar.required_semantics.push_back("objc3.cxx-data-interfaces.v1");
  if (collector.UsesCanonicalFunctionEffects())
    sidecar.required_semantics.push_back("objc3.canonical-function-effects.v1");
  if (collector.UsesTypedKeyPaths())
    sidecar.required_semantics.push_back("objc3.typed-keypaths.v1");
  if (collector.UsesKeyPathSubscripts())
    sidecar.required_semantics.push_back("objc3.keypath-subscripts.v1");
  for (const auto &declaration : sidecar.declarations) {
    if (HasNullableKeyPathWriteIdentity(declaration))
      sidecar.required_semantics.push_back(kNullableKeyPathWriteSemantic);
    if (!declaration.spi_group.empty())
      sidecar.required_semantics.push_back("objc3.api-partitions.v1");
    if (declaration.canonical_type_identity.find(";api-availability=") !=
        std::string::npos)
      sidecar.required_semantics.push_back("objc3.api-availability.v1");
    if (HasCxxUsingIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxUsingSemantic);
    if (HasCxxDeclarationExpressionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxDeclarationExpressionSemantic);
    if (HasCxxDefinitionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxDefinitionSemantic);
    if (HasCxxDefaultedComparisonIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxDefaultedComparisonSemantic);
    if (HasCxxOutOfLineDefaultingIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxOutOfLineDefaultingSemantic);
    if (HasCxxImmediateDefaultingIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxImmediateDefaultingSemantic);
    if (HasCxxNamespaceAttributeIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxNamespaceAttributeSemantic);
    if (HasCxxGenericLocalLambdaIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxGenericLocalLambdaSemantic);
    if (HasCxxUnionValueIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxUnionValueSemantic);
    if (HasCxxConceptIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxConceptSemantic);
    if (HasCxxFunctionTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxFunctionTemplateSemantic);
    if (HasCxxAliasTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxAliasTemplateSemantic);
    if (HasCxxVariableTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxVariableTemplateSemantic);
    if (HasCxxClassTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClassTemplateSemantic);
    if (HasCxxTemplateStructureIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxTemplateStructureSemantic);
    if (HasCxxTemplateDeductionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxTemplateDeductionSemantic);
    if (HasCxxMemberTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxMemberTemplateSemantic);
    if (HasCxxMemberAliasTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxMemberAliasTemplateSemantic);
    if (HasNativeDeclarationAttributeIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeDeclarationAttributeSemantic);
    if (HasCxxUnnamedTemplateProjectionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxUnnamedTemplateProjectionSemantic);
    if (HasCxxStandardCoroutineIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxStandardCoroutineSemantic);
    if (HasCxxCoroutineBodyIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxCoroutineBodySemantic);
    if (HasForeignImportAttributeIdentity(declaration))
      sidecar.required_semantics.push_back(kForeignImportAttributeSemantic);
    if (HasCxxNestedFactoryTypeIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxNestedFactoryTypeSemantic);
    if (HasCxxLambdaFactoryResultIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxLambdaFactoryResultSemantic);
    if (HasCxxClosureResultProjectionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClosureResultProjectionSemantic);
    if (HasCxxFactoryValueArgumentIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxFactoryValueArgumentSemantic);
    if (HasCxxGenericFactoryProjectionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxGenericFactoryProjectionSemantic);
    if (HasCxxFactoryTemplateArgumentIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxFactoryTemplateArgumentSemantic);
    if (HasCxxFactoryTypeProjectionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxFactoryTypeProjectionSemantic);
    if (HasCxxLocalFactoryResultIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxLocalFactoryResultSemantic);
    if (HasCxxClosureFactoryIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClosureFactorySemantic);
    if (HasCxxEffectfulLambdaIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxEffectfulLambdaSemantic);
    if (HasCxxAddressTemplateArgumentIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxAddressTemplateArgumentSemantic);
    if (HasCxxTemplateCallableProjectionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxTemplateCallableProjectionSemantic);
    if (HasCxxEffectfulTemplateMemberIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxEffectfulTemplateMemberSemantic);
    if (HasCxxEffectfulFunctionTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxEffectfulFunctionTemplateSemantic);
    if (HasCxxClosedParentPatternIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClosedParentPatternSemantic);
    if (HasCxxClosedParentMemberIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClosedParentMemberSemantic);
    if (HasCxxDependentMemberPartialIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxDependentMemberPartialSemantic);
    if (HasCxxConcreteSpecialMemberSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxConcreteSpecialMemberSpecializationSemantic);
    if (HasCxxConcreteMemberVariableSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxConcreteMemberVariableSpecializationSemantic);
    if (HasCxxConcreteMemberFunctionSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxConcreteMemberFunctionSpecializationSemantic);
    if (HasCxxConcreteMemberClassSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxConcreteMemberClassSpecializationSemantic);
    if (HasCxxDetachedNestedTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxDetachedNestedTemplateSemantic);
    if (HasCxxDetachedStaticDataTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxDetachedStaticDataTemplateSemantic);
    if (HasCxxDetachedCallableTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxDetachedCallableTemplateSemantic);
    if (HasCxxVariableSpecializationStorageIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxVariableSpecializationStorageSemantic);
    if (HasCxxVariablePartialSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxVariablePartialSpecializationSemantic);
    if (HasCxxClassPartialSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClassPartialSpecializationSemantic);
    if (HasCxxClassSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClassSpecializationSemantic);
    if (HasCxxVariableSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxVariableSpecializationSemantic);
    if (HasCxxFunctionSpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxFunctionSpecializationSemantic);
    if (HasCxxTemplateTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxTemplateTemplateSemantic);
    if (HasCxxNominalAliasTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxNominalAliasTemplateSemantic);
    if (HasCxxConcreteNestedTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxConcreteNestedTemplateSemantic);
    if (HasCxxConcreteMemberTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxConcreteMemberTemplateSemantic);
    if (HasCxxVariadicTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxVariadicTemplateSemantic);
    if (HasCxxNestedTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxNestedTemplateSemantic);
    if (HasCxxLambdaExpressionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxLambdaExpressionSemantic);
    if (HasCxxThreadLocalIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxThreadLocalSemantic);
    if (HasCxxFriendIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxFriendSemantic);
    if (HasCxxBlockExpressionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxBlockExpressionSemantic);
    if (HasCxxStatementExpressionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxStatementExpressionSemantic);
    if (HasSdkRuntimeDeclarationIdentity(declaration))
      sidecar.required_semantics.push_back(kSdkRuntimeDeclarationSemantic);
    if (HasCxxAsyncDefinitionIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxAsyncDefinitionSemantic);
    if (HasCxxLocalTypeIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxLocalTypeSemantic);
    if (HasCxxUnnamedLocalTypeIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxUnnamedLocalTypeSemantic);
    if (HasCxxNestedTypeIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxNestedTypeSemantic);
    if (HasCxxNamespaceIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxNamespaceSemantic);
    if (HasCxxClassDispatchIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClassDispatchSemantic);
    if (HasCxxClassStructureIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClassStructureSemantic);
    if (HasCxxClassLifecycleIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxClassLifecycleSemantic);
    if (IsCxxMemberDeclarationKind(declaration.kind))
      sidecar.required_semantics.push_back("objc3.cxx-member-interfaces.v1");
    if (declaration.kind == "ObjCMethod" &&
        declaration.canonical_type_identity.find(";dispatch=direct") != std::string::npos)
      sidecar.required_semantics.push_back("objc3.direct-dispatch.v1");
    const auto &effects = declaration.effects;
    if (effects.is_async || effects.is_throwing || !effects.ownership_identity.empty() ||
        !effects.async_abi_identity.empty())
      sidecar.required_semantics.push_back("objc3.canonical-function-effects.v1");
    if (!effects.async_abi_identity.empty())
      sidecar.required_semantics.push_back(kAsyncEntryABISemantic);
    if (effects.ownership_identity.starts_with("native-return:"))
      sidecar.required_semantics.push_back("objc3.native-return-ownership.v1");
    if (HasNativeOwnershipContracts(effects))
      sidecar.required_semantics.push_back(kNativeOwnershipContractSemantic);
    if (HasNativeIsolationContracts(effects))
      sidecar.required_semantics.push_back(kNativeIsolationContractSemantic);
    if (declaration.canonical_type_identity.find("KeyPath<") != std::string::npos)
      sidecar.required_semantics.push_back("objc3.typed-keypaths.v1");
    if (declaration.canonical_type_identity.find("Optional(") !=
            std::string::npos ||
        declaration.canonical_type_identity.find("Result(") !=
            std::string::npos)
      sidecar.required_semantics.push_back("objc3.native-carrier-types.v1");
  }
  std::sort(sidecar.required_semantics.begin(), sidecar.required_semantics.end());
  sidecar.required_semantics.erase(
      std::unique(sidecar.required_semantics.begin(), sidecar.required_semantics.end()),
      sidecar.required_semantics.end());
  const auto fingerprint_input = BuildSemanticFingerprintInput(sidecar);
  result.identity.semantic_fingerprint = ComputeInterfaceSourceSha256(fingerprint_input);
  sidecar.module = result.identity;
  sidecar.canonical_source_sha256 = ComputeInterfaceSourceSha256(result.source_bytes);

  const std::vector<llvm::StringRef> supported_semantics = {
      "objc3.canonical-function-effects.v1",
      "objc3.native-carrier-types.v1",
      "objc3.native-return-ownership.v1",
      "objc3.native-ownership-contracts.v1",
      "objc3.isolation-contracts.v1",
      "objc3.typed-keypaths.v1",
    "objc3.keypath-subscripts.v1",
    kNullableKeyPathWriteSemantic,
      "objc3.async-entry-abi.v1",
      "objc3.direct-dispatch.v1",
      "objc3.cxx-data-interfaces.v1",
      "objc3.cxx-member-interfaces.v1",
      kCxxClassLifecycleSemantic,
      kCxxClassStructureSemantic,
      kCxxClassDispatchSemantic,
      kCxxNamespaceSemantic,
    kCxxNestedTypeSemantic,
    kCxxUsingSemantic,
    kCxxDeclarationExpressionSemantic,
    kCxxDefinitionSemantic,
    kCxxDefaultedComparisonSemantic,
    kCxxOutOfLineDefaultingSemantic,
    kCxxImmediateDefaultingSemantic,
    kCxxNamespaceAttributeSemantic,
    kCxxLambdaExpressionSemantic,
    kCxxThreadLocalSemantic,
    kCxxFriendSemantic,
    kCxxBlockExpressionSemantic,
    kCxxStatementExpressionSemantic,
    kCxxLocalTypeSemantic,
    kCxxUnnamedLocalTypeSemantic,
    kCxxGenericLocalLambdaSemantic,
    kCxxUnionValueSemantic,
    kCxxConceptSemantic,
    kCxxFunctionTemplateSemantic,
    kCxxAliasTemplateSemantic,
    kCxxVariableTemplateSemantic,
    kCxxClassTemplateSemantic,
    kCxxTemplateStructureSemantic,
    kCxxTemplateDeductionSemantic,
    kCxxMemberTemplateSemantic,
    kCxxMemberAliasTemplateSemantic,
    kCxxNestedTemplateSemantic,
    kCxxVariadicTemplateSemantic,
    kCxxConcreteMemberTemplateSemantic,
    kCxxConcreteNestedTemplateSemantic,
    kCxxTemplateTemplateSemantic,
    kCxxNominalAliasTemplateSemantic,
    kCxxFunctionSpecializationSemantic,
    kCxxVariableSpecializationSemantic,
    kCxxClassSpecializationSemantic,
    kCxxClassPartialSpecializationSemantic,
    kCxxVariablePartialSpecializationSemantic,
    kCxxVariableSpecializationStorageSemantic,
    kCxxDetachedCallableTemplateSemantic,
    kCxxDetachedStaticDataTemplateSemantic,
    kCxxDetachedNestedTemplateSemantic,
    kCxxConcreteMemberClassSpecializationSemantic,
    kCxxConcreteMemberFunctionSpecializationSemantic,
    kCxxConcreteMemberVariableSpecializationSemantic,
    kCxxConcreteSpecialMemberSpecializationSemantic,
    kCxxDependentMemberPartialSemantic,
    kCxxClosedParentMemberSemantic,
    kCxxClosedParentPatternSemantic,
    kCxxEffectfulFunctionTemplateSemantic,
    kCxxEffectfulTemplateMemberSemantic,
    kCxxTemplateCallableProjectionSemantic,
    kCxxAddressTemplateArgumentSemantic,
    kNativeDeclarationAttributeSemantic,
    kCxxCoroutineBodySemantic,
    kCxxUnnamedTemplateProjectionSemantic,
    kCxxStandardCoroutineSemantic,
    kForeignImportAttributeSemantic,
    kCxxNestedFactoryTypeSemantic,
    kCxxLambdaFactoryResultSemantic,
    kCxxClosureResultProjectionSemantic,
    kCxxFactoryValueArgumentSemantic,
    kCxxGenericFactoryProjectionSemantic,
    kCxxFactoryTemplateArgumentSemantic,
    kCxxFactoryTypeProjectionSemantic,
    kCxxLocalFactoryResultSemantic,
    kCxxClosureFactorySemantic,
    kCxxEffectfulLambdaSemantic,
    kCxxAsyncDefinitionSemantic,
    kSdkRuntimeDeclarationSemantic,
      "objc3.ownership-projection.v1",
      "objc3.api-availability.v1",
      "objc3.api-partitions.v1",
      "objc3.module-owned-names.v1",
      RuntimeInterfaceSemantic()};
  auto encoded = EncodeSemanticInterfaceSidecar(sidecar, supported_semantics);
  if (!encoded) {
    AddDiagnostic(result, "O3IFC120",
                  llvm::toString(encoded.takeError()), canonical_module_name);
    result.source_bytes.clear();
    return result;
  }

  const auto input_file =
      lang.ObjC ? (lang.CPlusPlus ? "objc3-interface-roundtrip.mm"
                                  : "objc3-interface-roundtrip.m")
                : (lang.CPlusPlus ? "objc3-interface-roundtrip.cpp"
                                  : "objc3-interface-roundtrip.c");
  clang::tooling::FileContentMappings virtual_files;
  for (const auto &entry : sources_by_name)
    virtual_files.emplace_back(entry.second->virtual_include_path.empty()
            ? BuildInterfaceIncludePath(entry.first)
            : entry.second->virtual_include_path,
        entry.second->source_bytes);
  auto roundtrip_arguments = RoundTripArguments(context);
  AddRuntimeInterfaceFiles(virtual_files, roundtrip_arguments,
                           collector.UsesCxxAsyncBodies());
  // Nested canonical dependencies use the same root-relative include spelling.
  roundtrip_arguments.insert(roundtrip_arguments.end(), {"-iquote", "."});
  auto roundtrip = clang::tooling::buildASTFromCodeWithArgs(
      result.source_bytes, roundtrip_arguments, input_file,
      "clang-tool", std::make_shared<clang::PCHContainerOperations>(),
      clang::tooling::getClangStripDependencyFileAdjuster(), virtual_files,
      nullptr, llvm::vfs::createPhysicalFileSystem());
  if (!roundtrip || roundtrip->getDiagnostics().hasErrorOccurred()) {
    AddDiagnostic(result, "O3IFC121",
                  "Clang rejected the generated interface when reparsed with the original language, target, runtime, and ARC settings",
                  canonical_module_name);
    result.source_bytes.clear();
    return result;
  }
  InterfaceEmissionArtifact roundtrip_result;
  InterfaceCollector roundtrip_collector(roundtrip->getASTContext(),
                                         roundtrip_result, resolved_declaration_ids);
  roundtrip_collector.TraverseDecl(
      roundtrip->getASTContext().getTranslationUnitDecl());
  if (!roundtrip_result.diagnostics.empty()) {
    result.diagnostics = std::move(roundtrip_result.diagnostics);
    result.source_bytes.clear();
    return result;
  }
  auto emitted_identities = collector.Identities();
  auto reparsed_identities = roundtrip_collector.Identities();
  auto by_id = [](const auto &a, const auto &b) { return a.stable_id < b.stable_id; };
  std::sort(emitted_identities.begin(), emitted_identities.end(), by_id);
  std::sort(reparsed_identities.begin(), reparsed_identities.end(), by_id);
  if (emitted_identities.size() != reparsed_identities.size()) {
    std::string difference;
    for (const auto &identity : emitted_identities)
      if (std::none_of(reparsed_identities.begin(), reparsed_identities.end(),
          [&](const auto &other) { return identity.stable_id == other.stable_id; }))
        difference += " missing=" + identity.stable_id;
    for (const auto &identity : reparsed_identities)
      if (std::none_of(emitted_identities.begin(), emitted_identities.end(),
          [&](const auto &other) { return identity.stable_id == other.stable_id; }))
        difference += " unexpected=" + identity.stable_id;
    AddDiagnostic(result, "O3IFC122",
                  "reparsed interface declaration set differs from source AST:" + difference,
                  canonical_module_name);
    result.source_bytes.clear();
    return result;
  }
  for (std::size_t i = 0; i < emitted_identities.size(); ++i) {
    if (!SameDeclaration(emitted_identities[i], reparsed_identities[i])) {
      AddDiagnostic(result, "O3IFC123",
                    "reparsed declaration USR or canonical signature differs from source AST: " +
                        emitted_identities[i].canonical_type_identity +
                        " [native=" + emitted_identities[i].native_symbol_name +
                        "] -> " + reparsed_identities[i].canonical_type_identity +
                        " [native=" + reparsed_identities[i].native_symbol_name + "]",
                    emitted_identities[i].stable_id);
      result.source_bytes.clear();
      return result;
    }
  }

  result.sidecar_json = std::move(*encoded);
  result.success = true;
  return result;
}

namespace {
const std::vector<llvm::StringRef> kProjectionSemantics = {
    "objc3.canonical-function-effects.v1",
    "objc3.native-carrier-types.v1",
    "objc3.native-return-ownership.v1",
    "objc3.native-ownership-contracts.v1",
    "objc3.isolation-contracts.v1",
    "objc3.typed-keypaths.v1",
    "objc3.keypath-subscripts.v1",
    kNullableKeyPathWriteSemantic,
    "objc3.async-entry-abi.v1",
    "objc3.direct-dispatch.v1",
    "objc3.cxx-data-interfaces.v1",
    "objc3.cxx-member-interfaces.v1",
    kCxxClassLifecycleSemantic,
    kCxxClassStructureSemantic,
    kCxxClassDispatchSemantic,
    kCxxNamespaceSemantic,
    kCxxNestedTypeSemantic,
    kCxxUsingSemantic,
    kCxxDeclarationExpressionSemantic,
    kCxxDefinitionSemantic,
    kCxxDefaultedComparisonSemantic,
    kCxxOutOfLineDefaultingSemantic,
    kCxxImmediateDefaultingSemantic,
    kCxxNamespaceAttributeSemantic,
    kCxxLambdaExpressionSemantic,
    kCxxThreadLocalSemantic,
    kCxxFriendSemantic,
    kCxxBlockExpressionSemantic,
    kCxxStatementExpressionSemantic,
    kCxxLocalTypeSemantic,
    kCxxUnnamedLocalTypeSemantic,
    kCxxGenericLocalLambdaSemantic,
    kCxxUnionValueSemantic,
    kCxxConceptSemantic,
    kCxxFunctionTemplateSemantic,
    kCxxAliasTemplateSemantic,
    kCxxVariableTemplateSemantic,
    kCxxClassTemplateSemantic,
    kCxxTemplateStructureSemantic,
    kCxxTemplateDeductionSemantic,
    kCxxMemberTemplateSemantic,
    kCxxMemberAliasTemplateSemantic,
    kCxxNestedTemplateSemantic,
    kCxxVariadicTemplateSemantic,
    kCxxConcreteMemberTemplateSemantic,
    kCxxConcreteNestedTemplateSemantic,
    kCxxTemplateTemplateSemantic,
    kCxxNominalAliasTemplateSemantic,
    kCxxFunctionSpecializationSemantic,
    kCxxVariableSpecializationSemantic,
    kCxxClassSpecializationSemantic,
    kCxxClassPartialSpecializationSemantic,
    kCxxVariablePartialSpecializationSemantic,
    kCxxVariableSpecializationStorageSemantic,
    kCxxDetachedCallableTemplateSemantic,
    kCxxDetachedStaticDataTemplateSemantic,
    kCxxDetachedNestedTemplateSemantic,
    kCxxConcreteMemberClassSpecializationSemantic,
    kCxxConcreteMemberFunctionSpecializationSemantic,
    kCxxConcreteMemberVariableSpecializationSemantic,
    kCxxConcreteSpecialMemberSpecializationSemantic,
    kCxxDependentMemberPartialSemantic,
    kCxxClosedParentMemberSemantic,
    kCxxClosedParentPatternSemantic,
    kCxxEffectfulFunctionTemplateSemantic,
    kCxxEffectfulTemplateMemberSemantic,
    kCxxTemplateCallableProjectionSemantic,
    kCxxAddressTemplateArgumentSemantic,
    kNativeDeclarationAttributeSemantic,
    kCxxCoroutineBodySemantic,
    kCxxUnnamedTemplateProjectionSemantic,
    kCxxStandardCoroutineSemantic,
    kForeignImportAttributeSemantic,
    kCxxNestedFactoryTypeSemantic,
    kCxxLambdaFactoryResultSemantic,
    kCxxClosureResultProjectionSemantic,
    kCxxFactoryValueArgumentSemantic,
    kCxxGenericFactoryProjectionSemantic,
    kCxxFactoryTemplateArgumentSemantic,
    kCxxFactoryTypeProjectionSemantic,
    kCxxLocalFactoryResultSemantic,
    kCxxClosureFactorySemantic,
    kCxxEffectfulLambdaSemantic,
    kCxxAsyncDefinitionSemantic,
    kSdkRuntimeDeclarationSemantic,
    "objc3.ownership-projection.v1",
    "objc3.api-availability.v1",
    "objc3.api-partitions.v1",
    "objc3.module-owned-names.v1",
    RuntimeInterfaceSemantic()};

bool ProjectionMode(const clang::ASTContext &context) {
  const auto &lang = context.getLangOpts();
  llvm::StringRef standard =
      clang::LangStandard::getLangStandardForKind(lang.LangStd).getName();
  const bool supported_standard =
      lang.CPlusPlus ? (standard == "c++20" || standard == "c++23")
                     : standard == "c17";
  return supported_standard && lang.ObjC && lang.ObjC3 &&
         lang.ObjCRuntime.getAsString() == "gnustep-2.2" &&
         static_cast<unsigned>(lang.ObjC3Profile) == 0 &&
         context.getTargetInfo().getTriple().str() ==
             "x86_64-unknown-linux-gnu";
}

bool ProjectionType(clang::QualType type, bool result) {
  if (type.isNull() || type.isVolatileQualified() || type->isAtomicType())
    return false;
  const auto lifetime = type.getObjCLifetime();
  if (lifetime != clang::Qualifiers::OCL_None &&
      lifetime != clang::Qualifiers::OCL_Strong)
    return false;
  type = type.getCanonicalType();
  if (type->isObjCIdType())
    return true;
  if (type->isVoidType())
    return result;
  if (type->isRealType() && !type->isVectorType())
    return true;
  if (const auto *pointer = type->getAs<clang::PointerType>()) {
    const auto pointee = pointer->getPointeeType();
    return !pointee.isVolatileQualified() && !pointee->isAtomicType() &&
           (pointee->isVoidType() || pointee->isRealType());
  }
  return false;
}

bool ProjectionAPI(clang::ASTContext &context,
                   InterfaceEmissionArtifact &result,
                   const std::vector<std::string> &exports = {},
                   const std::string &source_root = {}) {
  if (!ProjectionMode(context)) {
    AddDiagnostic(result, "O3IFC150",
                  "ownership projections require the qualified ObjC3 Linux "
                  "GNUstep 2.2 compatibility mode");
    return false;
  }
  const std::set<std::string> no_dependencies;
  if (!ValidateExportNames(exports, result))
    return false;
  InterfaceCollector collector(context, result, no_dependencies, exports,
                               source_root);
  collector.TraverseDecl(context.getTranslationUnitDecl());
  collector.ValidateSelection();
  if (!result.diagnostics.empty())
    return false;
  const auto roots = collector.RootDeclarations();
  if (roots.empty()) {
    AddDiagnostic(result, "O3IFC150",
                  "ownership projection requires a nonempty C function API");
    return false;
  }
  for (const auto *root : roots) {
    const auto *function = llvm::dyn_cast<clang::FunctionDecl>(root);
    const auto *prototype =
        function ? function->getType()->getAs<clang::FunctionProtoType>()
                 : nullptr;
    bool valid = function && prototype &&
                 (!context.getLangOpts().CPlusPlus || function->isExternC()) &&
                 !prototype->isVariadic() &&
                 prototype->getCallConv() == clang::CC_C &&
                 prototype->getExceptionSpecType() == clang::EST_None &&
                 !prototype->getObjC3FunctionEffects().Async &&
                 prototype->getObjC3FunctionEffects().Throws ==
                     clang::ObjC3ThrowsKind::None &&
                 ProjectionType(function->getReturnType(), true);
    if (valid && function->getReturnType()->isObjCIdType())
      valid = !ReadNativeReturnOwnership(function).empty();
    if (valid)
      for (const auto *parameter : function->parameters())
        valid &= ProjectionType(parameter->getType(), false) &&
                 !parameter->hasAttrs() && !parameter->hasDefaultArg();
    if (!valid) {
      AddDiagnostic(result, "O3IFC151",
                    "ownership projection accepts only prototyped C ABI "
                    "functions with borrowed parameters, scalar/pointer types, "
                    "and explicit native ownership for id returns",
                    root->getQualifiedNameAsString());
      return false;
    }
  }
  return true;
}

std::vector<std::string> ProjectionArguments(const clang::ASTContext &context,
                                             bool arc) {
  auto args = RoundTripArguments(context);
  args.erase(std::remove(args.begin(), args.end(), "-fobjc-arc"), args.end());
  args.push_back(arc ? "-fobjc-arc" : "-fno-objc-arc");
  return args;
}

bool ValidateOwnershipProjection(clang::ASTContext &context,
                                 llvm::StringRef producer_sidecar,
                                 InterfaceEmissionArtifact &result) {
  if (!ProjectionAPI(context, result))
    return false;
  auto original = DecodeSemanticInterfaceSidecar(
      producer_sidecar, result.source_bytes, kProjectionSemantics);
  if (!original) {
    AddDiagnostic(result, "O3IFC152",
                  "invalid ownership projection producer: " +
                      llvm::toString(original.takeError()));
    return false;
  }
  if (!original->ownership_projection.empty() ||
      !original->dependencies.empty() ||
      original->module.canonical_name !=
          result.identity.canonical_name + ".__ownership_producer" ||
      original->module.package_identity != result.identity.package_identity) {
    AddDiagnostic(result, "O3IFC152",
                  "ownership projection producer identity, dependency closure, "
                  "or nesting is invalid");
    return false;
  }
  auto arguments =
      ProjectionArguments(context, !context.getLangOpts().ObjCAutoRefCount);
  clang::tooling::FileContentMappings files;
  AddRuntimeInterfaceFiles(files, arguments, result.source_bytes.find(
      "#include <objc3c/runtime/objc3_task.hpp>") != std::string::npos);
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      result.source_bytes, arguments,
      context.getLangOpts().CPlusPlus ? "ownership-producer.mm"
                                      : "ownership-producer.m",
      "clang-tool", std::make_shared<clang::PCHContainerOperations>(),
      clang::tooling::getClangStripDependencyFileAdjuster(), files);
  if (!ast || ast->getDiagnostics().hasErrorOccurred() ||
      !ProjectionAPI(ast->getASTContext(), result)) {
    if (result.diagnostics.empty())
      AddDiagnostic(result, "O3IFC153",
                    "ownership projection producer cannot be reconstructed");
    return false;
  }
  auto regenerated = EmitSemanticInterface(ast->getASTContext(),
                                           original->module.canonical_name,
                                           original->module.package_identity);
  if (!regenerated.success || regenerated.source_bytes != result.source_bytes ||
      regenerated.sidecar_json != producer_sidecar) {
    AddDiagnostic(result, "O3IFC154",
                  "ownership projection does not reproduce its canonical "
                  "opposite-mode producer contract");
    return false;
  }
  return true;
}
} // namespace

InterfaceEmissionArtifact EmitOwnershipProjection(
    clang::ASTContext &producer, llvm::StringRef canonical_module_name,
    llvm::StringRef package_identity, bool consumer_arc,
    const std::vector<InterfaceDependency> &dependencies,
    const std::vector<std::string> &exports, const std::string &source_root) {
  InterfaceEmissionArtifact result;
  if (producer.getLangOpts().ObjCAutoRefCount == consumer_arc ||
      !dependencies.empty()) {
    AddDiagnostic(result, "O3IFC150",
                  "ownership projection requires opposite ownership modes and "
                  "a closed provider API");
    return result;
  }
  if (!ProjectionAPI(producer, result, exports, source_root))
    return result;
  auto original = EmitSemanticInterface(
      producer, canonical_module_name.str() + ".__ownership_producer",
      package_identity, {}, {}, {}, exports, source_root);
  if (!original.success)
    return original;
  auto arguments = ProjectionArguments(producer, consumer_arc);
  clang::tooling::FileContentMappings files;
  AddRuntimeInterfaceFiles(files, arguments, original.source_bytes.find(
      "#include <objc3c/runtime/objc3_task.hpp>") != std::string::npos);
  auto ast = clang::tooling::buildASTFromCodeWithArgs(
      original.source_bytes, arguments,
      producer.getLangOpts().CPlusPlus ? "ownership-consumer.mm"
                                       : "ownership-consumer.m",
      "clang-tool", std::make_shared<clang::PCHContainerOperations>(),
      clang::tooling::getClangStripDependencyFileAdjuster(), files);
  if (!ast || ast->getDiagnostics().hasErrorOccurred()) {
    AddDiagnostic(result, "O3IFC153",
                  "ownership projection cannot be parsed in the consumer mode");
    return result;
  }
  result =
      EmitSemanticInterface(ast->getASTContext(), canonical_module_name,
                            package_identity, {}, {}, original.sidecar_json);
  if (result.success && result.source_bytes != original.source_bytes) {
    result.success = false;
    result.source_bytes.clear();
    result.sidecar_json.clear();
    AddDiagnostic(
        result, "O3IFC154",
        "ownership projection changed its native declaration contract");
  }
  return result;
}

}  // namespace objc3::clang_foundation
