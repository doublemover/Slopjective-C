#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/ast_effect_identity.h"
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
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
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
                  clang::SourceLocation location,
                  const std::string &source_root = {}) {
  return IsModuleOwnedSource(context, location, source_root);
}

bool IsTopLevel(const clang::NamedDecl *declaration) {
  const auto *context = declaration->getDeclContext();
  while (context && llvm::isa<clang::LinkageSpecDecl>(context))
    context = context->getParent();
  return context && context->isTranslationUnit();
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

  if (IsTopLevel(declaration)) {
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

  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(declaration))
    return method->getAccess() == clang::AS_public &&
           IsTopLevel(method->getParent());

  if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration)) {
    const auto *owner =
        llvm::dyn_cast<clang::RecordDecl>(field->getDeclContext());
    return field->getIdentifier() && owner &&
           (!context.getLangOpts().CPlusPlus || IsTopLevel(owner));
  }
  if (llvm::isa<clang::EnumConstantDecl>(declaration))
    return llvm::isa<clang::EnumDecl>(declaration->getDeclContext()) &&
           (!context.getLangOpts().CPlusPlus ||
            IsTopLevel(
                llvm::cast<clang::EnumDecl>(declaration->getDeclContext())));
  if (llvm::isa<clang::ObjCMethodDecl, clang::ObjCPropertyDecl,
                clang::ObjCIvarDecl>(declaration))
    return IsPublicContainer(declaration->getDeclContext(), context,
                             source_root);
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

std::string DeclarationTypeIdentity(const clang::NamedDecl *declaration,
                                    const clang::ASTContext &context) {
  if (const auto *method = llvm::dyn_cast<clang::ObjCMethodDecl>(declaration))
    return ObjCMethodIdentity(method, context);
  if (const auto *property = llvm::dyn_cast<clang::ObjCPropertyDecl>(declaration))
    return CanonicalType(property->getType(), context) + ";objc-property-attributes=" +
           std::to_string(static_cast<unsigned>(property->getPropertyAttributes()));
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
    return CanonicalType(type, context) +
           (context.getLangOpts().CPlusPlus
                ? ";language-linkage=" + std::to_string(static_cast<unsigned>(
                                             function->getLanguageLinkage()))
                : "");
  }
  if (const auto *value = llvm::dyn_cast<clang::ValueDecl>(declaration)) {
    std::string identity = CanonicalType(value->getType(), context);
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(value);
        variable && context.getLangOpts().CPlusPlus)
      identity +=
          ";language-linkage=" +
          std::to_string(static_cast<unsigned>(variable->getLanguageLinkage()));
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
  while (declaration) {
    if (IsTopLevel(declaration))
      return declaration->getQualifiedNameAsString();
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
  return exports.empty() ||
         std::find(exports.begin(), exports.end(), "*") != exports.end() ||
         std::find(exports.begin(), exports.end(),
                   ExportRootName(declaration)) != exports.end();
}

bool ValidateExportNames(const std::vector<std::string> &exports,
                         InterfaceEmissionArtifact &result) {
  std::set<std::string> seen;
  if (exports.size() > 256) {
    AddDiagnostic(result, "O3IFC155",
                  "export policy exceeds 256 declaration names");
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
     if (IsQualifiedRuntimeFrameType(declaration, context_)) {
       UsesRuntimeFrame = true;
       return true;
     }
     const auto stable_id = ReadInterfaceDeclarationStableId(declaration);
     return !stable_id.empty() && resolved_ids_.count(stable_id);
   }

  bool VisitTagType(clang::TagType *type) {
    const auto *declaration = type->getDecl();
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
    // Runtime headers redeclare these compiler-provided language types. Accept
    // only the same native name and type, without additional typedef contracts.
    if (context_.getLangOpts().ObjC && declaration) {
      for (const auto *builtin :
           {context_.getObjCIdDecl(), context_.getObjCClassDecl(),
            context_.getObjCSelDecl()}) {
        if (declaration == builtin ||
            (builtin && !declaration->hasAttrs() &&
             declaration->getIdentifier() == builtin->getIdentifier() &&
             context_.hasSameType(declaration->getUnderlyingType(),
                                  builtin->getUnderlyingType())))
          return true;
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
    // A C async declaration names a frame-returning entry point. Reprinting
    // its logical return type as Objective-C++ would instead select task<T>.
    // The interface schema must model that physical ABI before exporting it.
    if (effects.Async && !context_.getLangOpts().CPlusPlus)
      AddDiagnostic(result_, "O3IFC161",
                    "Objective-C async frame entry ABI is not yet modeled in semantic interfaces");
    UsesCanonicalFunctionEffects |=
        effects.Async || effects.Throws != clang::ObjC3ThrowsKind::None;
    if (!effects.ErrorType.isNull()) TraverseType(effects.ErrorType);
    return true;
  }

  bool UsesCanonicalFunctionEffects = false;
  bool UsesRuntimeFrame = false;

private:
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
                           const std::vector<std::string> &exports = {},
                           const std::string &source_root = {}) {
  TypeDependencyVisitor visitor(context, result, resolved_ids, exports,
                                ReadInterfaceSPIGroup(declaration),
                                source_root);
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
  return visitor.UsesCanonicalFunctionEffects;
}

bool IsBoundedCxxDeclaration(const clang::NamedDecl *declaration,
                             InterfaceEmissionArtifact &result) {
  const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
  bool supported = true;
  if (record && record->isCompleteDefinition()) {
    supported =
        !record->isDependentContext() && !record->getDescribedClassTemplate() &&
        record->getTemplateSpecializationKind() == clang::TSK_Undeclared &&
        record->isAggregate() && !record->isUnion() &&
        record->getNumBases() == 0 && !record->isPolymorphic() &&
        !record->hasUserDeclaredConstructor() &&
        !record->hasUserDeclaredDestructor() &&
        !record->hasUserDeclaredCopyConstructor() &&
        !record->hasUserDeclaredMoveConstructor() &&
        !record->hasUserDeclaredCopyAssignment() &&
        !record->hasUserDeclaredMoveAssignment();
    for (const auto *member : record->decls()) {
      if (member->isImplicit() || llvm::isa<clang::AccessSpecDecl>(member))
        continue;
      const auto *field = llvm::dyn_cast<clang::FieldDecl>(member);
      if (field) {
        supported &= field->getAccess() == clang::AS_public &&
                     !field->isMutable() && !field->hasInClassInitializer();
        continue;
      }
      const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(member);
      supported &= method && !llvm::isa<clang::CXXConversionDecl>(method) &&
                   method->getAccess() == clang::AS_public &&
                   !method->isTemplated() && !method->isVirtual() &&
                   !method->isInlined() && !method->isDefaulted() &&
                   !method->isDeleted() && !method->isConstexpr() &&
                   !method->getTrailingRequiresClause();
      if (method)
        for (const auto *parameter : method->parameters())
          supported &= !parameter->hasDefaultArg();
    }
  }
  if (IsTopLevel(declaration)) {
    supported &=
        !llvm::isa<clang::TemplateDecl, clang::UsingDecl,
                   clang::UsingDirectiveDecl, clang::NamespaceAliasDecl>(
            declaration);
    if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration))
      supported &= space->isAnonymousNamespace();
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && function->isExternallyVisible()) {
      supported &= !function->isTemplated() &&
                   !function->getTrailingRequiresClause() &&
                   !function->isInlined() && !function->isDefaulted();
      for (const auto *parameter : function->parameters())
        supported &= !parameter->hasDefaultArg();
    }
    if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
        variable && variable->isExternallyVisible())
      supported &=
          !variable->isConstexpr() && !variable->isInline() &&
          variable->getTLSKind() == clang::VarDecl::TLS_None &&
          !(variable->hasInit() && variable->mightBeUsableInConstantExpressions(
                                       declaration->getASTContext()));
  }
  if (!supported)
    AddDiagnostic(result, "O3IFC101",
                  "C++ semantic interfaces require global declarations and "
                  "public aggregate records and out-of-line public methods without "
                  "templates, bases, explicit "
                  "special members, defaults, or nested declarations",
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
                          return root->getQualifiedNameAsString() == name;
                        }))
         AddDiagnostic(result_, "O3IFC155",
                       "export selection does not name an eligible public root "
                       "declaration",
                       name);
     }
   }

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
    if (!result_.diagnostics.empty())
      return true;
    if (!SelectedExport(declaration, exports_))
      return true;
    if (context_.getLangOpts().CPlusPlus && !declaration->isImplicit() &&
        IsInMainFile(context_, declaration->getLocation()) &&
        IsTopLevel(declaration) &&
        !IsBoundedCxxDeclaration(declaration, result_))
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
      // Native C typedefs can have no linkage while naming a callable contract.
      // Validate their type even when the current public identity model does
      // not export them, rather than silently accepting unqualified effects.
      if (IsTopLevel(declaration) &&
          IsInMainFile(context_, declaration->getLocation()) &&
          llvm::isa<clang::TypedefNameDecl>(declaration))
        CheckTypeDependencies(declaration, context_, result_, resolved_ids_,
                              uses_runtime_frame_, exports_, source_root_);
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
    if (auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
        variable && variable->isInline()) {
      AddDiagnostic(result_, "O3IFC115",
                    "inline variable definitions cannot be represented without "
                    "their initializer",
                    variable->getQualifiedNameAsString());
      return true;
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
        continue;
      }
      if ((llvm::isa<clang::FunctionDecl, clang::VarDecl>(declaration) &&
           llvm::isa<clang::AsmLabelAttr>(attribute)) ||
          (llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl>(
               declaration) &&
           llvm::isa<clang::ObjCRuntimeNameAttr>(attribute)))
        continue;
      if (llvm::isa<clang::DeprecatedAttr, clang::UnavailableAttr,
                    clang::AvailabilityAttr>(attribute))
        continue;
      if (llvm::isa<clang::ObjCMethodDecl>(declaration) &&
          llvm::isa<clang::ObjCDirectAttr>(attribute))
        continue;
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
        CheckTypeDependencies(representative, context_, result_, resolved_ids_,
                              uses_runtime_frame_, exports_, source_root_);

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
  bool uses_function_effects_ = false;
  bool uses_runtime_frame_ = false;
};

void PrintRootDeclaration(clang::NamedDecl *declaration,
                          clang::ASTContext &context,
                          llvm::raw_ostream &out) {
  const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
  const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
  const bool c_linkage =
      context.getLangOpts().CPlusPlus && ((function && function->isExternC()) ||
                                          (variable && variable->isExternC()));
  if (c_linkage)
    out << "extern \"C\" {\n";
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.PrintAsCanonical = true;
  policy.FullyQualifiedName = true;
  policy.SuppressScope = false;
  policy.SuppressUnwrittenScope = false;
  policy.PolishForDeclaration = true;
  // Member names inside a record must be relative to that record. Fully
  // qualified function spelling would print invalid `Worker::get` inside it.
  if (llvm::isa<clang::CXXRecordDecl>(declaration))
    policy.FullyQualifiedName = false;

  if (llvm::isa<clang::FunctionDecl>(declaration)) {
    policy.TerseOutput = true;
    // Print modeled ownership exactly once, independently of whether the
    // original declaration placed its GNU attribute before or after the type.
    // Declaration polishing retains modeled native availability while the
    // ownership attribute below remains canonical across ARC and MRC.
    policy.SuppressDeclAttributes = false;
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
  if (llvm::isa<clang::ObjCInterfaceDecl, clang::ObjCProtocolDecl>(declaration))
    if (const auto *runtime =
            declaration->getAttr<clang::ObjCRuntimeNameAttr>()) {
      runtime->printPretty(out, policy);
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
  if (c_linkage)
    out << "}\n";
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
  return std::tie(a.stable_id, a.kind, a.qualified_name, a.spi_group,
                  a.native_symbol_name, a.canonical_type_identity,
                  a.effects.is_async, a.effects.is_throwing,
                  a.effects.throws_kind, a.effects.error_type_identity,
                  a.effects.actor_isolation_identity,
                  a.effects.executor_affinity_identity,
                  a.effects.ownership_identity) ==
         std::tie(b.stable_id, b.kind, b.qualified_name, b.spi_group,
                  b.native_symbol_name, b.canonical_type_identity,
                  b.effects.is_async, b.effects.is_throwing,
                  b.effects.throws_kind, b.effects.error_type_identity,
                  b.effects.actor_isolation_identity,
                  b.effects.executor_affinity_identity,
                  b.effects.ownership_identity);
}

}  // namespace

std::string ReadInterfaceSPIGroup(const clang::NamedDecl *declaration) {
  while (declaration && !IsTopLevel(declaration))
    declaration =
        llvm::dyn_cast<clang::NamedDecl>(declaration->getDeclContext());
  if (!declaration)
    return {};
  declaration = Representative(const_cast<clang::NamedDecl *>(declaration));
  for (const auto *a : declaration->specific_attrs<clang::AnnotateAttr>())
    if (a->getAnnotation().starts_with("objc3.spi:"))
      return a->getAnnotation().drop_front(10).str();
  return {};
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
  llvm::SmallString<128> usr;
  if (clang::index::generateUSRForDecl(declaration, usr)) return {};
  // Native USRs encode filenames for C typedefs because those names have no
  // linkage. Module provenance provides the provider identity; physical source
  // and canonical artifact filenames must not change the exported type name.
  if (llvm::isa<clang::TypedefNameDecl>(declaration) && IsTopLevel(declaration))
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
  identity.spi_group = ReadInterfaceSPIGroup(representative);
  identity.native_symbol_name = ReadNativeSymbolName(representative);
  identity.kind = declaration->getDeclKindName();
  identity.qualified_name = declaration->getQualifiedNameAsString();
  if (identity.qualified_name.empty())
    identity.qualified_name = declaration->getNameAsString();
  identity.canonical_type_identity =
      DeclarationTypeIdentity(representative, context);
  if (identity.canonical_type_identity.empty()) return false;
  if (const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(representative))
    identity.canonical_type_identity += method->isStatic()
        ? ";cxx-method=static" : ";cxx-method=instance";
  const auto availability = AvailabilityIdentity(representative);
  if (!availability.empty())
    identity.canonical_type_identity += ";api-availability=" + availability;
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
  if (collector.UsesRuntimeFrame())
    source << "#include <objc3c/runtime/objc3_runtime_services.h>\n";
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
  if (collector.UsesRuntimeFrame())
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
  for (const auto &declaration : sidecar.declarations) {
    if (!declaration.spi_group.empty())
      sidecar.required_semantics.push_back("objc3.api-partitions.v1");
    if (declaration.canonical_type_identity.find(";api-availability=") !=
        std::string::npos)
      sidecar.required_semantics.push_back("objc3.api-availability.v1");
    if (declaration.kind == "CXXMethod")
      sidecar.required_semantics.push_back("objc3.cxx-member-interfaces.v1");
    if (declaration.kind == "ObjCMethod" &&
        declaration.canonical_type_identity.find(";dispatch=direct") != std::string::npos)
      sidecar.required_semantics.push_back("objc3.direct-dispatch.v1");
    const auto &effects = declaration.effects;
    if (effects.is_async || effects.is_throwing || !effects.ownership_identity.empty())
      sidecar.required_semantics.push_back("objc3.canonical-function-effects.v1");
    if (effects.ownership_identity.starts_with("native-return:"))
      sidecar.required_semantics.push_back("objc3.native-return-ownership.v1");
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
      "objc3.direct-dispatch.v1",
      "objc3.cxx-data-interfaces.v1",
      "objc3.cxx-member-interfaces.v1",
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
  AddRuntimeInterfaceFiles(virtual_files, roundtrip_arguments);
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

namespace {
const std::vector<llvm::StringRef> kProjectionSemantics = {
    "objc3.canonical-function-effects.v1",
    "objc3.native-carrier-types.v1",
    "objc3.native-return-ownership.v1",
    "objc3.direct-dispatch.v1",
    "objc3.cxx-data-interfaces.v1",
    "objc3.cxx-member-interfaces.v1",
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
  AddRuntimeInterfaceFiles(files, arguments);
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
  AddRuntimeInterfaceFiles(files, arguments);
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
