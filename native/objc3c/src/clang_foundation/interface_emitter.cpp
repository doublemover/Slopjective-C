#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/interface_module_resolver.h"
#include "clang_foundation/ast_effect_identity.h"

#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Basic/ObjC3Foundation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/UnifiedSymbolResolution/USRGeneration.h"
#include "clang/Tooling/Tooling.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/SHA256.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Support/raw_ostream.h"

namespace objc3::clang_foundation {
namespace {

void AddDiagnostic(InterfaceEmissionArtifact &result, llvm::StringRef code,
                   llvm::StringRef message, llvm::StringRef subject = {}) {
  result.diagnostics.push_back({code.str(), message.str(), subject.str()});
}

std::string CanonicalType(clang::QualType type,
                          const clang::ASTContext &context) {
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.PrintAsCanonical = true;
  policy.FullyQualifiedName = true;
  policy.SuppressScope = false;
  policy.SuppressUnwrittenScope = false;
  return type.getCanonicalType().getAsString(policy);
}

bool IsInMainFile(const clang::ASTContext &context,
                  clang::SourceLocation location) {
  return location.isValid() &&
         context.getSourceManager().isWrittenInMainFile(location);
}

bool IsTopLevel(const clang::NamedDecl *declaration) {
  const auto *context = declaration->getDeclContext();
  return context && context->isTranslationUnit();
}

bool IsPublicContainer(const clang::DeclContext *context,
                       const clang::ASTContext &ast) {
  if (const auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(context))
    return IsInMainFile(ast, interface->getLocation()) &&
           interface->isExternallyVisible();
  if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(context))
    return IsInMainFile(ast, protocol->getLocation()) &&
           protocol->isExternallyVisible();
  if (const auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(context)) {
    const auto *interface = category->getClassInterface();
    return !category->IsClassExtension() && interface &&
           IsInMainFile(ast, category->getLocation()) &&
           interface->isExternallyVisible();
  }
  return false;
}

bool IsIdentityCandidate(const clang::NamedDecl *declaration,
                         const clang::ASTContext &context) {
  if (declaration->isImplicit() ||
      !IsInMainFile(context, declaration->getLocation()))
    return false;

  if (IsTopLevel(declaration)) {
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
      return !llvm::isa<clang::CXXMethodDecl>(function) &&
             function->isExternallyVisible();
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
      return variable->hasGlobalStorage() && variable->isExternallyVisible();
    // C typedef names have no linkage, but are public module lookup names.
    if (llvm::isa<clang::TypedefNameDecl>(declaration)) return true;
    if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(declaration))
      return !llvm::isa<clang::CXXRecordDecl>(tag) &&
             tag->isExternallyVisible();
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

  if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration))
    return !llvm::isa<clang::CXXRecordDecl>(field->getDeclContext()) &&
           field->getIdentifier() != nullptr;
  if (llvm::isa<clang::EnumConstantDecl>(declaration))
    return llvm::isa<clang::EnumDecl>(declaration->getDeclContext());
  if (llvm::isa<clang::ObjCMethodDecl, clang::ObjCPropertyDecl,
                clang::ObjCIvarDecl>(declaration))
    return IsPublicContainer(declaration->getDeclContext(), context);
  return false;
}

clang::NamedDecl *Representative(clang::NamedDecl *declaration) {
  if (auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration))
    return function->getMostRecentDecl();
  if (auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration))
    return variable->getMostRecentDecl();
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

std::string RecordLayoutIdentity(const clang::RecordDecl *record,
                                const clang::ASTContext &context) {
  std::string text;
  llvm::raw_string_ostream out(text);
  out << "record{";
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
  out.flush();
  return text;
}

std::string EnumIdentity(const clang::EnumDecl *enumeration,
                         const clang::ASTContext &context) {
  std::string text = "enum{" + CanonicalType(
      context.getCanonicalTagType(enumeration), context);
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

std::string DeclarationTypeIdentity(const clang::NamedDecl *declaration,
                                    const clang::ASTContext &context) {
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    return ObjCMethodIdentity(method, context);
  if (const auto *property = llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    return CanonicalType(property->getType(), context) + ";objc-property-attributes=" +
           std::to_string(static_cast<unsigned>(property->getPropertyAttributesAsWritten()));
  if (const auto *record = llvm::dyn_cast<clang::RecordDecl>(declaration))
    return RecordLayoutIdentity(record, context);
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
    // ARC represents retained returns in ExtInfo, MRC in a declaration
    // attribute. Ownership has its own identity field, so normalize this one
    // top-level bit while keeping nested callable return contracts intact.
    clang::QualType type = function->getType();
    if (const auto *proto = type->getAs<clang::FunctionProtoType>()) {
      auto info = proto->getExtProtoInfo();
      info.ExtInfo = info.ExtInfo.withProducesResult(false);
      type = context.getFunctionType(proto->getReturnType(), proto->getParamTypes(), info);
    } else if (const auto *no_proto = type->getAs<clang::FunctionNoProtoType>()) {
      type = context.getFunctionNoProtoType(no_proto->getReturnType(),
          no_proto->getExtInfo().withProducesResult(false));
    }
    return CanonicalType(type, context);
  }
  if (const auto *value = llvm::dyn_cast<clang::ValueDecl>(declaration)) {
    std::string identity = CanonicalType(value->getType(), context);
    const auto lifetime = value->getType().getObjCLifetime();
    if (lifetime != clang::Qualifiers::OCL_None)
      identity += ";objc-lifetime=" + std::to_string(static_cast<unsigned>(lifetime));
    return identity;
  }
  if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(declaration))
    return CanonicalType(context.getTypeDeclType(type), context);
  return {};
}

class TypeDependencyVisitor final
    : public clang::RecursiveASTVisitor<TypeDependencyVisitor> {
 public:
  TypeDependencyVisitor(const clang::ASTContext &context,
                        InterfaceEmissionArtifact &result,
                        const std::set<std::string> &resolved_ids)
      : context_(context), result_(result), resolved_ids_(resolved_ids) {}

  bool IsResolved(const clang::NamedDecl *declaration) const {
    const auto stable_id = ReadInterfaceDeclarationStableId(declaration);
    return !stable_id.empty() && resolved_ids_.count(stable_id);
  }

  bool VisitTagType(clang::TagType *type) {
    const auto *declaration = type->getDecl();
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references a record or enum from an unresolved external dependency",
                    declaration->getQualifiedNameAsString());
    return true;
  }

  bool VisitTypedefType(clang::TypedefType *type) {
    const auto *declaration = type->getDecl();
    // Compiler-provided id/Class/SEL are native language types, with no
    // external module to resolve. Compare declarations, never spellings.
    if (context_.getLangOpts().ObjC && declaration &&
        (declaration == context_.getObjCIdDecl() ||
         declaration == context_.getObjCClassDecl() ||
         declaration == context_.getObjCSelDecl()))
      return true;
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references a typedef from an unresolved external dependency",
                    declaration->getQualifiedNameAsString());
    return true;
  }

  bool VisitObjCInterfaceType(clang::ObjCInterfaceType *type) {
    const auto *declaration = type->getDecl();
    if (declaration && !IsInMainFile(context_, declaration->getLocation()) &&
        !IsResolved(declaration))
      AddDiagnostic(result_, "O3IFC112",
                    "public declaration references an Objective-C class from an unresolved external dependency",
                    declaration->getNameAsString());
    return true;
  }

  bool VisitObjCObjectType(clang::ObjCObjectType *type) {
    for (const auto *protocol : type->quals())
      if (!IsInMainFile(context_, protocol->getLocation()) && !IsResolved(protocol))
        AddDiagnostic(result_, "O3IFC112",
                      "public declaration references a protocol from an unresolved external dependency",
                      protocol->getNameAsString());
    return true;
  }

  bool VisitFunctionProtoType(clang::FunctionProtoType *type) {
    const auto effects = type->getObjC3FunctionEffects();
    UsesCanonicalFunctionEffects |=
        effects.Async || effects.Throws != clang::ObjC3ThrowsKind::None;
    if (!effects.ErrorType.isNull()) TraverseType(effects.ErrorType);
    return true;
  }

  bool UsesCanonicalFunctionEffects = false;

 private:
  const clang::ASTContext &context_;
  InterfaceEmissionArtifact &result_;
  const std::set<std::string> &resolved_ids_;
};

bool CheckTypeDependencies(const clang::NamedDecl *declaration,
                           const clang::ASTContext &context,
                           InterfaceEmissionArtifact &result,
                           const std::set<std::string> &resolved_ids) {
  TypeDependencyVisitor visitor(context, result, resolved_ids);
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
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration)) {
    check(method->getObjC3FunctionType());
    check(method->getReturnType());
    for (unsigned i = 0; i < method->param_size(); ++i)
      check(method->getParamDecl(i)->getType());
  }
  if (const auto *interface = llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration)) {
    if (const auto *superclass = interface->getSuperClass())
      if (!IsInMainFile(context, superclass->getLocation()))
        AddDiagnostic(result, "O3IFC112",
                      "Objective-C superclass dependency has no resolved interface identity",
                      superclass->getNameAsString());
    for (const auto *protocol : interface->protocols())
      if (!IsInMainFile(context, protocol->getLocation()))
        AddDiagnostic(result, "O3IFC112",
                      "adopted Objective-C protocol dependency has no resolved interface identity",
                      protocol->getNameAsString());
    for (const auto *ivar : interface->ivars()) check(ivar->getType());
  }
  if (const auto *protocol = llvm::dyn_cast<clang::ObjCProtocolDecl>(declaration))
    for (const auto *adopted : protocol->protocols())
      if (!IsInMainFile(context, adopted->getLocation()))
        AddDiagnostic(result, "O3IFC112",
                      "adopted Objective-C protocol dependency has no resolved interface identity",
                      adopted->getNameAsString());
  return visitor.UsesCanonicalFunctionEffects;
}

class InterfaceCollector final
    : public clang::RecursiveASTVisitor<InterfaceCollector> {
 public:
  InterfaceCollector(clang::ASTContext &context,
                     InterfaceEmissionArtifact &result,
                     const std::set<std::string> &resolved_ids)
      : context_(context), result_(result), resolved_ids_(resolved_ids) {}

  bool VisitImportDecl(clang::ImportDecl *declaration) {
    if (IsInMainFile(context_, declaration->getLocation())) {
      const auto *module = declaration->getImportedModule();
      AddDiagnostic(result_, "O3IFC111",
                    "source contains a module import but its resolved interface dependency identity is unavailable",
                    module ? module->getFullModuleName() : "<unknown-module>");
    }
    return true;
  }

  bool VisitNamedDecl(clang::NamedDecl *declaration) {
    if (const auto *category = llvm::dyn_cast<clang::ObjCCategoryDecl>(declaration)) {
      if (category->IsClassExtension() &&
          IsInMainFile(context_, category->getLocation())) {
        AddDiagnostic(result_, "O3IFC113",
                      "Objective-C class extension layout is not yet represented in the public interface contract",
                      category->getNameAsString());
        return true;
      }
    }
    if (!IsIdentityCandidate(declaration, context_)) {
      // Native C typedefs can have no linkage while naming a callable contract.
      // Validate their type even when the current public identity model does
      // not export them, rather than silently accepting unqualified effects.
      if (IsTopLevel(declaration) &&
          IsInMainFile(context_, declaration->getLocation()) &&
          llvm::isa<clang::TypedefNameDecl>(declaration))
        CheckTypeDependencies(declaration, context_, result_, resolved_ids_);
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
    if (auto *ivar = llvm::dyn_cast<clang::ObjCIvarDecl>(declaration)) {
      if (ivar->getAccessControl() != clang::ObjCIvarDecl::None) {
        AddDiagnostic(result_, "O3IFC114",
                      "explicit Objective-C ivar access is not preserved by Clang's interface printer",
                      ivar->getNameAsString());
        return true;
      }
    }
    if (auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
      if (function->isInlineSpecified()) {
        AddDiagnostic(result_, "O3IFC115",
                      "inline function body removal may change linkage and is unsupported",
                      function->getQualifiedNameAsString());
        return true;
      }
    }
    for (const auto *attribute : declaration->attrs()) {
      if (attribute->isImplicit()) continue;
      if (llvm::isa<clang::FunctionDecl>(declaration) &&
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
    if (IsTopLevel(declaration) &&
        !llvm::isa<clang::ObjCMethodDecl, clang::ObjCPropertyDecl,
                   clang::ObjCIvarDecl, clang::FieldDecl,
                   clang::EnumConstantDecl>(declaration))
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
    auto *representative = Representative(declaration);
    const std::string stable_id = identity.stable_id;
    uses_function_effects_ |=
        CheckTypeDependencies(representative, context_, result_, resolved_ids_);

    const auto found = declarations_.find(stable_id);
    if (found == declarations_.end() ||
        (!IsDefinition(found->second) && IsDefinition(representative)))
      declarations_[stable_id] = representative;
    identities_[stable_id] = std::move(identity);
    return true;
  }

  std::vector<clang::NamedDecl *> RootDeclarations() const {
    auto roots = root_declarations_;
    const auto &source_manager = context_.getSourceManager();
    std::sort(roots.begin(), roots.end(), [&](const auto *a, const auto *b) {
      const auto a_offset = source_manager.getFileOffset(a->getLocation());
      const auto b_offset = source_manager.getFileOffset(b->getLocation());
      if (a_offset != b_offset) return a_offset < b_offset;
      return a->getQualifiedNameAsString() < b->getQualifiedNameAsString();
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

 private:
  clang::ASTContext &context_;
  InterfaceEmissionArtifact &result_;
  const std::set<std::string> &resolved_ids_;
  std::map<std::string, clang::NamedDecl *> declarations_;
  std::map<std::string, InterfaceDeclarationIdentity> identities_;
  std::vector<clang::NamedDecl *> root_declarations_;
  bool uses_function_effects_ = false;
};

void PrintRootDeclaration(clang::NamedDecl *declaration,
                          clang::ASTContext &context,
                          llvm::raw_ostream &out) {
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.PrintAsCanonical = true;
  policy.FullyQualifiedName = true;
  policy.SuppressScope = false;
  policy.SuppressUnwrittenScope = false;
  policy.PolishForDeclaration = true;
  if (llvm::isa<clang::FunctionDecl>(declaration)) {
    policy.TerseOutput = true;
    // Print modeled ownership exactly once, independently of whether the
    // original declaration placed its GNU attribute before or after the type.
    policy.SuppressDeclAttributes = true;
  }
  if (llvm::isa<clang::VarDecl>(declaration)) {
    policy.SuppressInitializers = true;
    const auto *variable = llvm::cast<clang::VarDecl>(declaration);
    if (variable->getStorageClass() == clang::SC_None)
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
  declaration->print(out, policy);
  if (llvm::isa<clang::FunctionDecl>(declaration)) {
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
}

std::vector<std::string> RoundTripArguments(const clang::ASTContext &context) {
  const auto &lang = context.getLangOpts();
  const auto &standard = clang::LangStandard::getLangStandardForKind(lang.LangStd);
  std::vector<std::string> args = {
      "-fsyntax-only", "--target=" + context.getTargetInfo().getTriple().str(),
      std::string("-x"), lang.ObjC ? "objective-c" : "c",
      "-std=" + std::string(standard.getName())};
  if (lang.ObjC) {
    args.push_back("-fobjc-runtime=" + lang.ObjCRuntime.getAsString());
    if (lang.ObjCAutoRefCount) args.push_back("-fobjc-arc");
  }
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
  return std::tie(a.stable_id, a.kind, a.qualified_name,
                  a.canonical_type_identity,
                  a.effects.is_async, a.effects.is_throwing,
                  a.effects.throws_kind, a.effects.error_type_identity,
                  a.effects.actor_isolation_identity,
                  a.effects.executor_affinity_identity,
                  a.effects.ownership_identity) ==
         std::tie(b.stable_id, b.kind, b.qualified_name,
                  b.canonical_type_identity,
                  b.effects.is_async, b.effects.is_throwing,
                  b.effects.throws_kind, b.effects.error_type_identity,
                  b.effects.actor_isolation_identity,
                  b.effects.executor_affinity_identity,
                  b.effects.ownership_identity);
}

}  // namespace

std::string ReadInterfaceDeclarationStableId(
    const clang::NamedDecl *declaration) {
  if (!declaration) return {};
  llvm::SmallString<128> usr;
  if (clang::index::generateUSRForDecl(declaration, usr)) return {};
  // Native USRs encode filenames for C typedefs because those names have no
  // linkage. Module provenance provides the provider identity; physical source
  // and canonical artifact filenames must not change the exported type name.
  if (!declaration->getASTContext().getLangOpts().CPlusPlus &&
      llvm::isa<clang::TypedefNameDecl>(declaration) &&
      declaration->getDeclContext()->isTranslationUnit())
    return "c:@T@" + declaration->getNameAsString();
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
  identity.kind = declaration->getDeclKindName();
  identity.qualified_name = declaration->getQualifiedNameAsString();
  if (identity.qualified_name.empty())
    identity.qualified_name = declaration->getNameAsString();
  identity.canonical_type_identity =
      DeclarationTypeIdentity(representative, context);
  if (identity.canonical_type_identity.empty()) return false;
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
  const auto return_ownership = ReadNativeReturnOwnership(representative);
  if (!return_ownership.empty()) identity.effects.ownership_identity = return_ownership;
  return true;
}

InterfaceEmissionArtifact EmitSemanticInterface(
    clang::ASTContext &context, llvm::StringRef canonical_module_name,
    llvm::StringRef package_identity,
    const std::vector<InterfaceDependency> &resolved_dependencies,
    const std::vector<InterfaceSourceArtifact> &dependency_sources) {
  InterfaceEmissionArtifact result;
  if (canonical_module_name.empty() || package_identity.empty()) {
    AddDiagnostic(result, "O3IFC100",
                  "canonical module name and package identity are required");
    return result;
  }
  const auto &lang = context.getLangOpts();
  if (lang.CPlusPlus) {
    AddDiagnostic(result, "O3IFC101",
                  "Objective-C++/C++ interface emission is not yet supported",
                  canonical_module_name);
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
      CLANG_OBJC3_PATCH_DIGEST;
  result.identity.language_revision = lang.ObjC3
      ? std::to_string(lang.ObjC3Revision)
      : "objc3-disabled";
  static constexpr const char *profiles[] = {
      "compatibility", "strict", "strict-concurrency", "strict-system"};
  const auto profile_index = static_cast<unsigned>(lang.ObjC3Profile);
  result.identity.language_profile =
      std::string("objc3=") + (lang.ObjC3 ? "enabled" : "disabled") +
      ";profile=" + (profile_index < std::size(profiles)
                           ? profiles[profile_index]
                           : "unknown") +
      ";strict-concurrency=" + (lang.ObjC3StrictConcurrency ? "true" : "false") +
      ";arc=" + (lang.ObjCAutoRefCount ? "true" : "false");
  result.identity.package_identity = package_identity.str();

  InterfaceCollector collector(context, result, resolved_declaration_ids);
  collector.TraverseDecl(context.getTranslationUnitDecl());
  if (!result.diagnostics.empty()) return result;

  result.source_bytes = "/* Generated canonical Clang interface. */\n#pragma once\n";
  llvm::raw_string_ostream source(result.source_bytes);
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

  SemanticInterfaceSidecar sidecar;
  sidecar.module = result.identity;
  sidecar.declarations = collector.Identities();
  sidecar.dependencies = resolved_dependencies;
  if (collector.UsesCanonicalFunctionEffects())
    sidecar.required_semantics.push_back("objc3.canonical-function-effects.v1");
  for (const auto &declaration : sidecar.declarations) {
    const auto &effects = declaration.effects;
    if (effects.is_async || effects.is_throwing || !effects.ownership_identity.empty())
      sidecar.required_semantics.push_back("objc3.canonical-function-effects.v1");
    if (effects.ownership_identity.starts_with("native-return:"))
      sidecar.required_semantics.push_back("objc3.native-return-ownership.v1");
    if (declaration.canonical_type_identity.find("__objc3_optional(") != std::string::npos ||
        declaration.canonical_type_identity.find("__objc3_result(") != std::string::npos)
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
      "objc3.canonical-function-effects.v1", "objc3.native-carrier-types.v1",
      "objc3.native-return-ownership.v1"};
  auto encoded = EncodeSemanticInterfaceSidecar(sidecar, supported_semantics);
  if (!encoded) {
    AddDiagnostic(result, "O3IFC120",
                  llvm::toString(encoded.takeError()), canonical_module_name);
    result.source_bytes.clear();
    return result;
  }

  const auto input_file = lang.ObjC ? "objc3-interface-roundtrip.m"
                                    : "objc3-interface-roundtrip.c";
  clang::tooling::FileContentMappings virtual_files;
  for (const auto &entry : sources_by_name)
    virtual_files.emplace_back(entry.second->virtual_include_path.empty()
            ? BuildInterfaceIncludePath(entry.first)
            : entry.second->virtual_include_path,
        entry.second->source_bytes);
  auto roundtrip_arguments = RoundTripArguments(context);
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
    AddDiagnostic(result, "O3IFC122",
                  "reparsed interface declaration set differs from source AST",
                  canonical_module_name);
    result.source_bytes.clear();
    return result;
  }
  for (std::size_t i = 0; i < emitted_identities.size(); ++i) {
    if (!SameDeclaration(emitted_identities[i], reparsed_identities[i])) {
      AddDiagnostic(result, "O3IFC123",
                    "reparsed declaration USR or canonical signature differs from source AST: " +
                        emitted_identities[i].canonical_type_identity + " -> " +
                        reparsed_identities[i].canonical_type_identity,
                    emitted_identities[i].stable_id);
      result.source_bytes.clear();
      return result;
    }
  }

  result.sidecar_json = std::move(*encoded);
  result.success = true;
  return result;
}

}  // namespace objc3::clang_foundation
