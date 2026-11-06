#include "standard_library_specializations.h"
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
#include "clang/AST/DynamicRecursiveASTVisitor.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Basic/ObjC3Foundation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/APFloat.h"
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

#include "clang_foundation/interface_emission_internal.h"

namespace objc3::clang_foundation {
namespace emission_detail {

void AddDiagnostic(InterfaceEmissionArtifact &result, llvm::StringRef code,
                   llvm::StringRef message, llvm::StringRef subject) {
  result.diagnostics.push_back({code.str(), message.str(), subject.str()});
}


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
  return llvm::isa<clang::CFReturnsRetainedAttr,clang::CFReturnsNotRetainedAttr,clang::OwnershipAttr,clang::ArgumentWithTypeTagAttr,clang::TypeTagForDatatypeAttr,clang::FormatMatchesAttr,clang::DiagnoseAsBuiltinAttr,clang::PassObjectSizeAttr,clang::LifetimeBoundAttr,clang::NoEscapeAttr,clang::SentinelAttr,clang::CallbackAttr,clang::DiagnoseIfAttr,clang::EnableIfAttr,clang::OverloadableAttr,clang::ReturnsTwiceAttr,clang::NoThrowAttr,clang::DisableTailCallsAttr,clang::NotTailCalledAttr,clang::NoMergeAttr,clang::NoDuplicateAttr,clang::OptimizeNoneAttr,clang::LeafAttr,clang::AllocSizeAttr,clang::AllocAlignAttr,clang::AssumeAlignedAttr,clang::NonNullAttr,clang::ReturnsNonNullAttr,clang::FormatAttr,clang::FormatArgAttr,clang::RestrictAttr,clang::SysVABIAttr,clang::PreserveMostAttr,clang::PreserveAllAttr,clang::PreserveNoneAttr,clang::RegCallAttr,clang::VectorCallAttr,clang::AlignedAttr,clang::PackedAttr,clang::NoUniqueAddressAttr,
      clang::VisibilityAttr,clang::TypeVisibilityAttr,clang::NoInlineAttr,
      clang::AlwaysInlineAttr,clang::FlattenAttr,clang::ColdAttr,clang::HotAttr,
      clang::ConstAttr,clang::PureAttr,clang::WarnUnusedResultAttr,clang::UnusedAttr,
      clang::NoReturnAttr,clang::CXX11NoReturnAttr>(attribute);
}


// These attributes already have dedicated lifetime, borrowed-result, task or
// isolation identities. Template admission must retain those contracts just as
// it retains their ordinary declaration counterparts.
bool IsPublishedSemanticAttribute(const clang::Attr *attribute) {
  return IsPublishedNativeAttribute(attribute) ||
      llvm::isa<clang::NSReturnsRetainedAttr,clang::NSReturnsNotRetainedAttr,
          clang::NSReturnsAutoreleasedAttr,clang::NSConsumedAttr,clang::CFConsumedAttr,
          clang::NSConsumesSelfAttr,clang::ObjCReturnsBorrowedAttr,
          clang::ObjCExecutorAttr,clang::ObjCNonisolatedAttr,
          clang::ObjCUnsafeSendableAttr,clang::ObjCTaskSpawnAttr,
          clang::ObjCTaskDetachedAttr,clang::ObjCTaskGroupAttr,
          clang::ObjCFamilyRetainAttr,clang::ObjCFamilyReleaseAttr,
          clang::ObjCFamilyAutoreleaseAttr>(attribute);
}


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
                  const std::string &source_root) {
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

std::set<std::string> NamespaceAttributes(const clang::NamespaceDecl *space, bool canonical) {
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
    if (IsPublishedStandardSpecialization(current)) return current;
    if (const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(current))
      if (const auto *partial=variable->getSpecializedTemplateOrPartial().dyn_cast<clang::VarTemplatePartialSpecializationDecl *>();
          partial && IsStandardLibrarySpecialization(partial))return partial;
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(current))
      if (const auto *partial=record->getSpecializedTemplateOrPartial().dyn_cast<clang::ClassTemplatePartialSpecializationDecl *>();
          partial && IsPublishedStandardSpecialization(partial)) return partial;
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
                       const std::string &source_root) {
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
                         const std::string &source_root) {
  if (declaration->isImplicit() ||
      !IsInMainFile(context, declaration->getLocation(), source_root))
    return false;

  if (llvm::isa<clang::ClassTemplateDecl>(declaration)) return IsTopLevel(declaration);
  if (IsPublishedStandardSpecialization(declaration)) return IsTopLevel(declaration);
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
  if (auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
    if (!function->getASTContext().getLangOpts().CPlusPlus)
      if (auto *definition=function->getDefinition();definition && definition->isInlined())
        return definition;
    return function->getMostRecentDecl();
  }
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
  if (!function || function->isDefaulted() || function->isDeleted()) return false;
  if (!function->getASTContext().getLangOpts().CPlusPlus)
    return function->getDefinition() && function->getDefinition()->isInlined();
  return function->isInlined() || function->getDescribedFunctionTemplate() ||
      IsDetachedCallableTemplateDefinition(function->getDefinition());
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

// Identity probes share Clang's dynamic walk instead of instantiating its full
// CRTP traversal for each probe. Visitors that customize queue/walk-up semantics
// retain RecursiveASTVisitor below.
// Bind spelling, typed native nodes and every resolved reference. Text alone
// cannot distinguish identically spelled defaults that select different overloads.
// Length delimiters keep user strings from introducing synthetic identity fields.
std::string DeclarationAttributeKey(const clang::NamedDecl *declaration,bool native) {
  if(llvm::isa<clang::NamespaceDecl>(declaration))return {};
  struct Attributes:clang::DynamicRecursiveASTVisitor {
    bool native;
    explicit Attributes(bool native):native(native){}
    std::string key;
    unsigned ordinal=0;
    std::set<const clang::Decl *> visited;
    bool TraverseStmt(clang::Stmt *) override {return true;}
    bool TraverseDecl(clang::Decl *declaration) override {
      if (!declaration || declaration->isImplicit() || !visited.insert(declaration).second) return true;
      if (!clang::DynamicRecursiveASTVisitor::TraverseDecl(declaration)) return false;
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
    bool VisitNamedDecl(clang::NamedDecl *declaration) override {
      const auto slot=ordinal++;
      // ASTImporter can merge the same inherited contract from several
      // redeclarations. Identical metadata is one semantic contract.
      std::set<std::string> values;
      std::string ordered_conditions;
      if(native)
        for(const auto *attribute:declaration->specific_attrs<clang::EnableIfAttr>()) {
          const auto value=NativeAttributeValue(attribute,declaration);
          ordered_conditions+=std::to_string(value.size())+":"+value;
        }
      if(!ordered_conditions.empty())values.insert("ordered-enable-if:"+ordered_conditions);
      if (native)
        if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration))
          if (function->getType()->castAs<clang::FunctionType>()->getNoReturnAttr())
            values.insert("native-noreturn-function-type");
      for(const auto *attribute:declaration->attrs())
        if((native ? IsPublishedNativeAttribute(attribute) : IsForeignImportAttribute(attribute)) && !attribute->isImplicit()) {
          if(native) {if(!llvm::isa<clang::EnableIfAttr>(attribute))values.insert(NativeAttributeValue(attribute,declaration));continue;}
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
  struct References final : clang::DynamicRecursiveASTVisitor {
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
        if (const auto *owner=llvm::dyn_cast<clang::NamedDecl>(scope);
            owner && IsQualifiedRuntimeTaskDeclaration(owner,context)) return;
        if (scope->isFunctionOrMethod()) return;
        if (const auto *space=llvm::dyn_cast<clang::NamespaceDecl>(scope))
          if (space->isStdNamespace() && protocol_member &&
              context.getSourceManager().isInSystemHeader(declaration->getLocation())) return;
      }
      declarations.emplace(ReadInterfaceDeclarationStableId(declaration),declaration);
    }
    bool VisitCallExpr(clang::CallExpr *expression) override {Add(expression->getDirectCallee());return true;}
    bool VisitCXXConstructExpr(clang::CXXConstructExpr *expression) override {Add(expression->getConstructor());return true;}
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

}  // namespace emission_detail

}  // namespace objc3::clang_foundation
