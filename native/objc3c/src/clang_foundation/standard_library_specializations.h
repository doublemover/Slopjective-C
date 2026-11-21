#pragma once
#include "standard_library_headers.h"

namespace objc3::clang_foundation {
inline bool IsStandardLibraryNamespace(const clang::NamespaceDecl *space,
                                      const clang::ASTContext &context) {
  bool standard=false;
  for(const auto *scope=space;scope;scope=llvm::dyn_cast<clang::NamespaceDecl>(scope->getDeclContext()))
    standard |= scope->isStdNamespace();
  if(!standard)return false;
  // ARC can create a builtin std namespace before any real header. Its later
  // redeclarations still prove which namespaces belong to the native library.
  for(const auto *redecl:space->redecls())
    if(IsStandardLibraryDeclaration(redecl,context))return true;
  return false;
}
inline bool IsStandardLibrarySpecialization(const clang::NamedDecl *declaration) {
  if(const auto *variable=llvm::dyn_cast_or_null<clang::VarTemplateSpecializationDecl>(declaration))
    return (llvm::isa<clang::VarTemplatePartialSpecializationDecl>(variable) ||
        variable->getSpecializationKind()==clang::TSK_ExplicitSpecialization) &&
        !IsStandardLibraryDeclaration(variable,variable->getASTContext()) &&
        IsStandardLibraryDeclaration(variable->getSpecializedTemplate(),variable->getASTContext());
  const auto *record=llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(declaration);
  if(!record || IsStandardCoroutineTraitsSpecialization(record) ||
     (!llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(record) &&
      record->getSpecializationKind()!=clang::TSK_ExplicitSpecialization))return false;
  return !IsStandardLibraryDeclaration(record,record->getASTContext()) &&
      IsStandardLibraryDeclaration(record->getSpecializedTemplate(),record->getASTContext());
}
inline const clang::NamedDecl *StandardLibraryTypeArgumentOwner(clang::QualType type,
                                                               unsigned depth,unsigned &budget);
inline const clang::NamedDecl *StandardLibraryArgumentOwner(const clang::TemplateArgument &argument,
                                                           unsigned depth,unsigned &budget) {
  if(depth==64 || !budget)return nullptr;
  --budget;
  if(argument.getKind()==clang::TemplateArgument::Type)
    return StandardLibraryTypeArgumentOwner(argument.getAsType(),depth+1,budget);
  if(argument.getKind()==clang::TemplateArgument::Pack)
    for(const auto &element:argument.pack_elements())
      if(const auto *owner=StandardLibraryArgumentOwner(element,depth+1,budget))return owner;
  return nullptr;
}
inline const clang::NamedDecl *StandardLibraryTypeArgumentOwner(clang::QualType type,
                                                               unsigned depth,unsigned &budget) {
  if(type.isNull() || depth==64 || !budget)return nullptr;
  --budget;
  if(type->isPointerType() || type->isReferenceType())
    return StandardLibraryTypeArgumentOwner(type->getPointeeType(),depth+1,budget);
  if(const auto *array=llvm::dyn_cast<clang::ArrayType>(type.getTypePtr()))
    return StandardLibraryTypeArgumentOwner(array->getElementType(),depth+1,budget);
  if(const auto *record=type->getAsCXXRecordDecl()) {
    const auto *owner=record->getDescribedClassTemplate()?
        static_cast<const clang::NamedDecl *>(record->getDescribedClassTemplate()):record;
    const auto *specialization=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(record);
    if(specialization)owner=specialization->getSpecializedTemplate();
    const auto &context=record->getASTContext();
    if(!owner->getName().empty() && !context.getSourceManager().isInSystemHeader(owner->getLocation()) &&
       !IsStandardLibraryDeclaration(owner,context))return owner;
    if(specialization)
      for(const auto &argument:specialization->getTemplateArgs().asArray())
        if(const auto *nested=StandardLibraryArgumentOwner(argument,depth+1,budget))return nested;
  }
  if(const auto *tag=type->getAs<clang::EnumType>()) {
    const auto *owner=tag->getDecl();
    if(!owner->getName().empty() && !owner->getASTContext().getSourceManager().isInSystemHeader(owner->getLocation()))return owner;
  }
  if(const auto *pattern=type->getAs<clang::TemplateSpecializationType>()) {
    const auto *owner=pattern->getTemplateName().getAsTemplateDecl();
    if(owner && !owner->getName().empty() && !owner->getASTContext().getSourceManager().isInSystemHeader(owner->getLocation()))return owner;
    for(const auto &argument:pattern->template_arguments())
      if(const auto *nested=StandardLibraryArgumentOwner(argument,depth+1,budget))return nested;
  }
  return nullptr;
}
inline const clang::NamedDecl *StandardLibrarySpecializationOwner(const clang::NamedDecl *declaration) {
  if(!IsStandardLibrarySpecialization(declaration))return nullptr;
  const auto *record=llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration);
  const auto *variable=llvm::dyn_cast<clang::VarTemplateSpecializationDecl>(declaration);
  const auto arguments=record?record->getTemplateArgs().asArray():variable->getTemplateArgs().asArray();
  unsigned budget=4096;
  for(const auto &argument:arguments)
    if(const auto *owner=StandardLibraryArgumentOwner(argument,0,budget))return owner;
  return nullptr;
}
inline bool IsPublishedStandardSpecialization(const clang::NamedDecl *declaration) {
  return IsStandardCoroutineTraitsSpecialization(declaration) || IsStandardLibrarySpecialization(declaration);
}
inline const clang::NamedDecl *PublishedStandardSpecializationOwner(const clang::NamedDecl *declaration) {
  if(const auto *owner=CoroutineTraitsReturnOwner(declaration))return owner;
  return StandardLibrarySpecializationOwner(declaration);
}
} // namespace objc3::clang_foundation
