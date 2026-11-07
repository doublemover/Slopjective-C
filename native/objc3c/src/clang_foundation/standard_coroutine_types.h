#ifndef OBJC3_CLANG_FOUNDATION_STANDARD_COROUTINE_TYPES_H
#define OBJC3_CLANG_FOUNDATION_STANDARD_COROUTINE_TYPES_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/Path.h"

namespace objc3::clang_foundation {
// These declarations are supplied by the consumer's standard library. A user
// declaration merely placed in std is still an ordinary checked dependency.
inline bool IsStandardCoroutineDeclaration(const clang::NamedDecl *declaration,
                                           const clang::ASTContext &context) {
  if (!declaration) return false;
  const auto &source=context.getSourceManager();
  auto location=source.getSpellingLoc(declaration->getLocation());
  if (location.isInvalid() || !source.isInSystemHeader(location) ||
      llvm::sys::path::filename(source.getFilename(location))!="coroutine") return false;
  for (const auto *scope=declaration->getDeclContext();scope;scope=scope->getParent())
    if (const auto *space=llvm::dyn_cast<clang::NamespaceDecl>(scope)) {
      if (space->isStdNamespace()) return true;
      if (!space->isInline()) return false;
    }
  return false;
}
inline bool IsStandardCoroutineNamespace(const clang::NamespaceDecl *space,
                                         const clang::ASTContext &context) {
  const auto *root=space;
  while (root && !root->isStdNamespace()) {
    if (!root->isInline()) return false;
    root=llvm::dyn_cast<clang::NamespaceDecl>(root->getDeclContext());
  }
  if (!root) return false;
  auto contains=[&](auto &&self,const clang::NamespaceDecl *scope)->bool {
    for (const auto *redecl:scope->redecls())
      for (const auto *declaration:redecl->decls()) {
        if (const auto *pattern=llvm::dyn_cast<clang::ClassTemplateDecl>(declaration);
            pattern && pattern->getName()=="coroutine_handle" && IsStandardCoroutineDeclaration(pattern,context)) {
          for (const auto *owner=pattern->getDeclContext();owner;owner=owner->getParent())
            if (const auto *name=llvm::dyn_cast<clang::NamespaceDecl>(owner);
                name && name->getCanonicalDecl()==space->getCanonicalDecl()) return true;
        }
        if (const auto *nested=llvm::dyn_cast<clang::NamespaceDecl>(declaration);
            nested && nested->isInline() && self(self,nested)) return true;
      }
    return false;
  };
  return contains(contains,root);
}
inline bool IsStandardCoroutineTraitsSpecialization(const clang::NamedDecl *declaration) {
  const auto *record=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(declaration);
  if (!record || (!llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(record) &&
      record->getSpecializationKind()!=clang::TSK_ExplicitSpecialization)) return false;
  const auto *pattern=record->getSpecializedTemplate();
  return pattern->getName()=="coroutine_traits" &&
      IsStandardCoroutineDeclaration(pattern,record->getASTContext());
}
inline const clang::NamedDecl *CoroutineTraitsReturnOwner(const clang::NamedDecl *declaration) {
  if (!IsStandardCoroutineTraitsSpecialization(declaration)) return nullptr;
  const auto *record=llvm::cast<clang::ClassTemplateSpecializationDecl>(declaration);
  auto arguments=record->getTemplateArgs().asArray();
  if (arguments.empty() || arguments.front().getKind()!=clang::TemplateArgument::Type) return nullptr;
  auto type=arguments.front().getAsType();
  if (const auto *result=type->getAsCXXRecordDecl()) {
    if (const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(result))
      return specialization->getSpecializedTemplate();
    return result->getDescribedClassTemplate() ?
        static_cast<const clang::NamedDecl *>(result->getDescribedClassTemplate()) : result;
  }
  if (const auto *result=type->getAs<clang::TemplateSpecializationType>())
    return result->getTemplateName().getAsTemplateDecl();
  return nullptr;
}
inline const clang::NamedDecl *StandardCoroutineHeaderOwner(const clang::NamedDecl *declaration,
                                                            const clang::ASTContext &context) {
  if (IsStandardCoroutineDeclaration(declaration,context)) return declaration;
  for (const auto *current=declaration;current;current=llvm::dyn_cast<clang::NamedDecl>(current->getDeclContext()))
    if (const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(current)) {
      const auto *pattern=record->getSpecializedTemplate();
      if (pattern->getName()=="coroutine_traits" && IsStandardCoroutineDeclaration(pattern,context)) return pattern;
    }
  return nullptr;
}
} // namespace objc3::clang_foundation
#endif
