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
#include "llvm/ADT/ScopeExit.h"
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

  if (IsTopLevel(declaration)) {
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
         function->isInlined() && !function->isDefaulted() && !function->isDeleted();
}
bool PublishesVariableInitializer(const clang::VarDecl *variable) {
  return variable && variable->getASTContext().getLangOpts().CPlusPlus &&
         (variable->isInline() || variable->isConstexpr() ||
          (variable->hasInit() && variable->mightBeUsableInConstantExpressions(
                                      variable->getASTContext())));
}

// Bind spelling, typed native nodes and every resolved reference. Text alone
// cannot distinguish identically spelled defaults that select different overloads.
// Length delimiters keep user strings from introducing synthetic identity fields.
class DeclarationExpressionIdentity final
    : public clang::RecursiveASTVisitor<DeclarationExpressionIdentity> {
public:
  explicit DeclarationExpressionIdentity(const clang::ASTContext &context,
                                         const clang::FunctionDecl *function = nullptr)
      : context_(context) {
    if (function)
      for (const auto *parameter : function->parameters()) Local(parameter);
  }
  void Local(const clang::NamedDecl *declaration) {
    const auto *canonical = declaration->getCanonicalDecl();
    if (!locals_.count(canonical)) locals_.emplace(canonical, locals_.size());
  }
  std::string ExpressionType(clang::QualType type) {
    TraverseType(type);
    clang::PrintingPolicy policy(context_.getLangOpts());
    policy.PrintAsCanonical = true;
    policy.FullyQualifiedName = true;
    policy.SuppressScope = false;
    policy.SuppressUnwrittenScope = false;
    policy.AnonymousTagNameStyle = static_cast<unsigned>(clang::PrintingPolicy::AnonymousTagMode::Plain);
    return type.getCanonicalType().getAsString(policy);
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
    Add("lambda-capture-default:" + std::to_string(expression->getCaptureDefault()));
    for (const auto &capture : expression->captures()) {
      Add("capture:" + std::to_string(capture.getCaptureKind()));
      if (capture.capturesVariable()) Local(capture.getCapturedVar());
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
  void Reference(const clang::NamedDecl *declaration) {
    if (!declaration) { Add("<none>"); return; }
    if (const auto found = locals_.find(declaration->getCanonicalDecl()); found != locals_.end()) {
      Add("local:" + std::to_string(found->second)); return;
    }
    if (const auto *owner = llvm::dyn_cast<clang::CXXRecordDecl>(declaration->getDeclContext());
        owner && owner->isLambda()) {
      Local(owner);
      Add("closure-member:" + std::to_string(locals_.at(owner->getCanonicalDecl())));
      Add(declaration->getDeclKindName());
      if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
        Add(std::to_string(function->getOverloadedOperator()));
        Add(ExpressionType(function->getType()));
      } else if (const auto *field = llvm::dyn_cast<clang::FieldDecl>(declaration)) {
        unsigned index = 0;
        for (const auto *member : owner->fields()) { if (member == field) break; ++index; }
        Add(std::to_string(index));
      }
      return;
    }
    Add(ReadInterfaceDeclarationStableId(declaration));
    Add(ReadNativeSymbolName(declaration));
  }
  // Native printers insert protective parentheses for carrier operators. A
  // reparse retains those ParenExpr nodes; they do not change typed semantics.
  bool TraverseParenExpr(clang::ParenExpr *expression,
                         DataRecursionQueue * = nullptr) {
    return TraverseStmt(expression->getSubExpr());
  }
  std::string Get(clang::Stmt *expression) {
    clang::PrintingPolicy policy(context_.getLangOpts());
    policy.PrintAsCanonical = true;
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
        return false;
      }
    } helper(policy);
    expression->printPretty(stream, &helper, policy);
    Add(printed);
    TraverseStmt(expression);
    llvm::SHA256 hash; hash.update(bytes_);
    return llvm::toHex(hash.final(), true);
  }
private:
  const clang::ASTContext &context_;
  std::string bytes_;
  std::map<const clang::Decl *, unsigned> locals_;
};

std::string DeclarationExpressionContract(const clang::Expr *expression,
                                           const clang::ASTContext &context) {
  return DeclarationExpressionIdentity(context).Get(const_cast<clang::Expr *>(expression));
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
  return identity.Get(definition->getBody());
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
  bool VisitLambdaExpr(clang::LambdaExpr *) { found = true; return true; }
  bool found = false;
};
bool ContainsLambda(const clang::Stmt *statement) {
  if (!statement) return false;
  LambdaExpressionFinder finder; finder.TraverseStmt(const_cast<clang::Stmt *>(statement));
  return finder.found;
}
template <class Predicate>
bool HasPublishedExpression(const clang::NamedDecl *declaration, const Predicate &contains) {
  if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration)) {
    for (const auto *parameter : function->getMostRecentDecl()->parameters())
      if (parameter->hasDefaultArg() && !parameter->hasUninstantiatedDefaultArg() &&
          contains(parameter->getDefaultArg())) return true;
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
      PublishesVariableInitializer(variable) && variable->hasInit()) return contains(variable->getInit());
  if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
      record && record->isCompleteDefinition())
    for (const auto *member : record->decls()) {
      if (member->isImplicit()) continue;
      const clang::NamedDecl *named = llvm::dyn_cast<clang::NamedDecl>(member);
      if (const auto *friend_decl = llvm::dyn_cast<clang::FriendDecl>(member))
        named = friend_decl->getFriendDecl();
      if (named && llvm::isa<clang::FieldDecl, clang::FunctionDecl, clang::VarDecl>(named) &&
          HasPublishedExpression(named, contains)) return true;
    }
  return false;
}
bool HasDeclarationLambda(const clang::NamedDecl *declaration) {
  return HasPublishedExpression(declaration, ContainsLambda);
}

class LocalExpressionFinder final : public clang::RecursiveASTVisitor<LocalExpressionFinder> {
public:
  bool VisitBlockExpr(clang::BlockExpr *) { blocks = true; return true; }
  bool VisitVarDecl(clang::VarDecl *variable) {
    blocks |= variable->hasAttr<clang::BlocksAttr>(); return true;
  }
  bool VisitStmtExpr(clang::StmtExpr *) { statements = true; return true; }
  bool blocks = false;
  bool statements = false;
};
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
      record && record->isCompleteDefinition())
    for (const auto *member : record->decls())
      if (!member->isImplicit())
        if (const auto *named = llvm::dyn_cast<clang::NamedDecl>(member))
          if (llvm::isa<clang::VarDecl, clang::FunctionDecl>(named) &&
              HasDeclarationThreadLocal(named)) return true;
  return false;
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
      if (!method->isImplicit()) {
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
      if (!method->isImplicit() && PublishesFunctionBody(method))
        definitions.insert(ReadInterfaceDeclarationStableId(method) + ":" +
                           FunctionDefinitionContract(method, context));
    for (const auto *member : cxx->decls())
      if (const auto *variable = llvm::dyn_cast<clang::VarDecl>(member);
          variable && variable->isStaticDataMember())
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

std::string DeclarationTypeIdentity(const clang::NamedDecl *declaration,
                                    const clang::ASTContext &context) {
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
            DeclarationExpressionContract(parameter->getDefaultArg(), context) + ";";
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
  if (const auto *owner = FriendOwner(declaration)) declaration = owner;
  while (declaration) {
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

  bool TraverseRecordType(clang::RecordType *type, bool traverse_qualifier) {
    clang::ObjC3KeyPathInfo keypath;
    if (clang::getObjC3KeyPathInfo(clang::QualType(type, 0), keypath))
      return TraverseType(keypath.Root) && TraverseType(keypath.Value);
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseRecordType(type, traverse_qualifier);
  }

  void CheckExpressionReference(const clang::NamedDecl *target) {
    if (!target) return;
    target = target->getUnderlyingDecl();
    if (const auto *owner = llvm::dyn_cast<clang::CXXRecordDecl>(target->getDeclContext());
        owner && owner->isLambda()) return;
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
  void CheckBody(const clang::FunctionDecl *function) {
    const auto *definition = function->getDefinition();
    if (!definition) return;
    body_function_ = definition;
    if (const auto *constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(definition))
      for (const auto *initializer : constructor->inits())
        if (initializer->isWritten()) TraverseStmt(initializer->getInit());
    TraverseStmt(definition->getBody());
    body_function_ = nullptr;
  }
  bool VisitVarDecl(clang::VarDecl *variable) {
    if (body_function_)
      for (const auto *attribute : variable->attrs())
        if (!attribute->isImplicit() && !llvm::isa<clang::BlocksAttr>(attribute)) UnsupportedExpression();
    return true;
  }
  bool VisitTagDecl(clang::TagDecl *declaration) {
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
        record && record->isLambda()) return true;
    if (body_function_ && declaration->getDeclContext()->isFunctionOrMethod())
      return UnsupportedExpression();
    return true;
  }
  bool VisitExpr(clang::Expr *expression) {
    TraverseType(expression->getType());
    return true;
  }
  bool VisitDeclRefExpr(clang::DeclRefExpr *expression) {
    CheckExpressionReference(expression->getDecl()); return true;
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
  bool TraverseStmt(clang::Stmt *statement, DataRecursionQueue * = nullptr) {
    const bool previous = in_expression_; in_expression_ = true;
    auto restore = llvm::scope_exit([&] { in_expression_ = previous; });
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseStmt(statement, nullptr);
  }
  bool TraverseLambdaExpr(clang::LambdaExpr *expression, DataRecursionQueue * = nullptr) {
    if (expression->isGenericLambda()) return UnsupportedExpression();
    const auto contexts = lambda_contexts_;
    const auto locals = lambda_locals_;
    lambda_contexts_.insert(expression->getCallOperator()->getCanonicalDecl());
    for (const auto &capture : expression->captures())
      if (capture.capturesVariable() && expression->isInitCapture(&capture))
        lambda_locals_.insert(capture.getCapturedVar()->getCanonicalDecl());
    auto restore = llvm::scope_exit([&] { lambda_contexts_ = contexts; lambda_locals_ = locals; });
    return clang::RecursiveASTVisitor<TypeDependencyVisitor>::TraverseLambdaExpr(expression, nullptr);
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
  bool UnsupportedExpression() {
    AddDiagnostic(result_, "O3IFC101",
                  "declaration expressions with local declarations or captures require additional contracts");
    return false;
  }

  bool VisitTagType(clang::TagType *type) {
    const auto *declaration = type->getDecl();
    if (const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
        record && record->isLambda())
      return in_expression_ ? true : UnsupportedExpression();
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

private:
  bool in_expression_ = false;
  std::set<const clang::Decl *> lambda_contexts_;
  std::set<const clang::Decl *> lambda_locals_;
  const clang::FunctionDecl *body_function_ = nullptr;
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
  return visitor.UsesCanonicalFunctionEffects;
}

bool IsBoundedCxxDeclaration(const clang::NamedDecl *declaration,
                             InterfaceEmissionArtifact &result,
                             const std::vector<std::string> &exports = {}) {
  const auto *record = llvm::dyn_cast<clang::CXXRecordDecl>(declaration);
  bool supported = true;
  if (record && record->isCompleteDefinition()) {
    supported =
        !record->isDependentContext() && !record->getDescribedClassTemplate() &&
        record->getTemplateSpecializationKind() == clang::TSK_Undeclared &&
        !record->isUnion() &&
        (!record->isDynamicClass() || record->getASTContext().getLangOpts().RTTI);
    for (const auto *member : record->decls()) {
      if (member->isImplicit() || llvm::isa<clang::AccessSpecDecl, clang::BlockDecl>(member))
        continue;
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
      if (const auto *type = llvm::dyn_cast<clang::TypeDecl>(member)) {
        if (const auto *tag = llvm::dyn_cast<clang::TagDecl>(type))
          supported &= (tag->getIdentifier() || tag->getTypedefNameForAnonDecl());
        supported &= IsBoundedCxxDeclaration(type, result, exports);
        continue;
      }
      const auto *method = llvm::dyn_cast<clang::CXXMethodDecl>(member);
      supported &=
          method && !method->isTemplated() &&
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
        !llvm::isa<clang::TemplateDecl>(declaration);
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
      supported &= !function->isTemplated() &&
                   !function->getTrailingRequiresClause() &&
                   (!function->isDefaulted() || IsInClassDefaultedComparison(function));
    }
  }
  if (!supported)
    AddDiagnostic(result, "O3IFC101",
                  "C++ semantic interfaces require global or named namespace declarations and "
                  "records with native bases and virtual dispatch, out-of-line "
                  "members, deleted "
                  "declarations or native defaulted special members, without "
                  "templates, "
                  "local declaration expressions or unnamed nested types",
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
                llvm::isa<clang::NoEscapeAttr, clang::NSConsumedAttr>(attribute))
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
    if (const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && !context_.getLangOpts().CPlusPlus && function->isInlineSpecified()) {
      AddDiagnostic(result_, "O3IFC115", "C inline function linkage requires a dedicated definition contract",
                    function->getQualifiedNameAsString());
      return true;
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
                    clang::ObjCInvalidAttr>(attribute) &&
          llvm::isa<clang::TypedefNameDecl>(declaration))
        continue;
      if (llvm::isa<clang::ObjCReturnsBorrowedAttr, clang::NoThrowAttr>(attribute) &&
          llvm::isa<clang::FunctionDecl, clang::ObjCMethodDecl>(declaration))
        continue;
      if (llvm::isa<clang::NoEscapeAttr, clang::NSConsumedAttr>(attribute) &&
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
                              uses_runtime_frame_, exports_, source_root_);

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
};

void PrintUnscopedRootDeclaration(clang::NamedDecl *declaration,
                          clang::ASTContext &context,
                          llvm::raw_ostream &out, bool declaration_only = false) {
  const auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
  const auto *variable = llvm::dyn_cast<clang::VarDecl>(declaration);
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
  // Member names inside a record must be relative to that record. Fully
  // qualified function spelling would print invalid `Worker::get` inside it.
  if (llvm::isa<clang::CXXRecordDecl>(declaration) ||
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
    if (policy.SuppressInitializers && variable->getStorageClass() == clang::SC_None)
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
    if (has_label_definition) body_function->setAttrs(saved_attributes);
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
    PrintNamespaceHead(space, out, false);
    out << " {\n";
  }
  if (const auto *space = llvm::dyn_cast<clang::NamespaceDecl>(declaration)) {
    PrintNamespaceHead(space, out, true);
    out << " {}\n";
  }
  else {
    auto *function = llvm::dyn_cast<clang::FunctionDecl>(declaration);
    const auto qualifier = function ? function->getQualifierLoc()
                                    : clang::NestedNameSpecifierLoc();
    auto restore = llvm::make_scope_exit([&] {
      if (function) function->setQualifierInfo(qualifier);
    });
    // Out-of-line source definitions retain an explicit qualifier even with
    // FullyQualifiedName disabled. The enclosing wrappers already supply it.
    if (function && !scopes.empty()) function->setQualifierInfo({});
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
                  a.effects.ownership_identity,
                  a.effects.async_abi_identity) ==
         std::tie(b.stable_id, b.kind, b.qualified_name, b.spi_group,
                  b.native_symbol_name, b.canonical_type_identity,
                  b.effects.is_async, b.effects.is_throwing,
                  b.effects.throws_kind, b.effects.error_type_identity,
                  b.effects.actor_isolation_identity,
                  b.effects.executor_affinity_identity,
                  b.effects.ownership_identity,
                  b.effects.async_abi_identity);
}

}  // namespace

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
  identity.qualified_name = InterfaceQualifiedName(declaration);
  if (identity.qualified_name.empty())
    identity.qualified_name = declaration->getNameAsString();
  identity.canonical_type_identity =
      DeclarationTypeIdentity(representative, context);
  if (identity.canonical_type_identity.empty()) return false;
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
  if (HasDeclarationLambda(representative))
    identity.canonical_type_identity += ";cxx-lambda-expressions:v1;";
  if (HasDeclarationLocalExpression(representative, true))
    identity.canonical_type_identity += ";cxx-block-expressions:v1;";
  if (HasDeclarationLocalExpression(representative, false))
    identity.canonical_type_identity += ";cxx-statement-expressions:v1;";
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
  const auto return_ownership = ReadNativeReturnOwnership(representative);
  if (!return_ownership.empty()) identity.effects.ownership_identity = return_ownership;
  ReadNativeDeclarationContracts(representative, context, identity.effects);
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
