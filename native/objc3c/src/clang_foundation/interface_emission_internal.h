#pragma once

// Private emission implementation contract. This header is not an SDK API.

#include <set>
#include <string>
#include <vector>

#include "clang_foundation/interface_emitter.h"
#include "clang/Basic/Specifiers.h"

namespace clang {
class APValue;
class ASTContext;
class Attr;
class CXXMethodDecl;
class CXXRecordDecl;
class ClassTemplateDecl;
class ClassTemplateSpecializationDecl;
class CoawaitExpr;
class ConceptDecl;
class CoroutineBodyStmt;
class DeclContext;
class EnumDecl;
class Expr;
class FriendDecl;
class FunctionDecl;
class FunctionTemplateDecl;
class LambdaExpr;
class NamedDecl;
class NamespaceDecl;
class ObjCMethodDecl;
class ParmVarDecl;
class PseudoObjectExpr;
class QualType;
class RecordDecl;
class SourceLocation;
class Stmt;
class TagDecl;
class TemplateArgument;
class TemplateDecl;
class TemplateParameterList;
class TypeAliasTemplateDecl;
class ValueDecl;
class VarDecl;
class VarTemplateDecl;
class VarTemplatePartialSpecializationDecl;
class VarTemplateSpecializationDecl;
}

namespace llvm {
class raw_ostream;
}

namespace objc3::clang_foundation::emission_detail {

struct FactoryTypeFeatures {
  bool generic=false, closure=false, lambda_result=false, nested=false;
};

struct GenericLambdaBody {
  const clang::FunctionDecl *function;
  std::string key;
};

void AddDiagnostic(InterfaceEmissionArtifact &result, llvm::StringRef code,
                   llvm::StringRef message, llvm::StringRef subject = {});

bool AddressDeclarationKey(const clang::ValueDecl *declaration,
                             const clang::ASTContext &context,std::string &key);

std::string AvailabilityIdentity(const clang::NamedDecl *declaration);

std::string CanonicalType(clang::QualType type,
                          const clang::ASTContext &context);

bool CheckTypeDependencies(const clang::NamedDecl *declaration,
                           const clang::ASTContext &context,
                           InterfaceEmissionArtifact &result,
                           const std::set<std::string> &resolved_ids,
                           bool &uses_runtime_frame,
                           bool &uses_cxx_task_runtime,
                           const std::vector<std::string> &exports = {},
                           const std::string &source_root = {},
                           bool *uses_typed_keypaths = nullptr,
                           bool *uses_keypath_subscripts = nullptr);

bool ClassSpecializationArgumentKey(const clang::ClassTemplateSpecializationDecl *specialization,
                                    const clang::ASTContext &context,std::string &key);

const clang::ClassTemplateDecl *ClassTemplateOwner(const clang::NamedDecl *declaration);

std::vector<clang::NamedDecl *> ClosedParentMemberSpecializations(const clang::NamedDecl *owner);

std::vector<clang::NamedDecl *> ClosedParentPatterns(const clang::NamedDecl *owner);

std::string ConceptIdentity(const clang::ConceptDecl *declaration,
                            const clang::ASTContext &context);

std::vector<clang::ClassTemplateSpecializationDecl *> ConcreteMemberClassSpecializations(
    const clang::NamedDecl *owner);

std::vector<clang::FunctionDecl *> ConcreteMemberFunctionSpecializations(const clang::NamedDecl *owner);

std::vector<clang::VarTemplateSpecializationDecl *> ConcreteMemberVariableSpecializations(const clang::NamedDecl *owner);

bool ContainsLambda(const clang::Stmt *statement);

bool ContainsUnnamedLocalType(const clang::Stmt *statement);

const char *CxxAccessName(clang::AccessSpecifier access);

std::string CxxCoroutineContract(const clang::FunctionDecl *function,
                                  const clang::ASTContext &context);

std::vector<const clang::NamedDecl *> CxxCoroutineReferences(const clang::FunctionDecl *function);

std::string DeclarationAttributeKey(const clang::NamedDecl *declaration,bool native=false);

std::string DeclarationExpressionContract(const clang::Expr *expression,
                                           const clang::ASTContext &context);

std::string DeclarationTypeIdentity(const clang::NamedDecl *declaration,
                                    const clang::ASTContext &context);

std::vector<clang::NamedDecl *> DependentMemberPartials(const clang::NamedDecl *owner);

std::vector<clang::NamedDecl *> DetachedCallableTemplateDefinitions(const clang::NamedDecl *owner);

std::vector<clang::NamedDecl *> DetachedNestedTemplateDefinitions(const clang::NamedDecl *owner);

std::vector<clang::NamedDecl *> DetachedStaticDataTemplateDefinitions(const clang::NamedDecl *owner);

std::vector<clang::NamedDecl *> DetachedTemplateDefinitions(const clang::NamedDecl *owner,bool data);

std::string EnumIdentity(const clang::EnumDecl *enumeration,
                         const clang::ASTContext &context);

std::string ExportRootName(const clang::NamedDecl *declaration);

std::string FactoryDeclarationTypeKey(const clang::NamedDecl *declaration);

std::string FactoryOriginKey(const clang::FunctionDecl *function);

const clang::FunctionDecl *FactoryResultOrigin(const clang::TagDecl *tag);

const clang::TagDecl *FactoryResultTagRoot(const clang::FunctionDecl *function,const clang::TagDecl *tag);

std::string FactoryTagPath(const clang::TagDecl *tag,const clang::TagDecl *root,const clang::ASTContext &context);

std::string FactoryTypeKey(clang::QualType type,const clang::ASTContext &context,FactoryTypeFeatures *features=nullptr);

const clang::CXXRecordDecl *FriendOwner(const clang::NamedDecl *declaration);

std::string FunctionDefinitionContract(const clang::FunctionDecl *function,
                                        const clang::ASTContext &context);

std::string FunctionTemplateConstraintKey(const clang::FunctionTemplateDecl *pattern);

std::string FunctionTemplateHeadContract(const clang::TemplateDecl *pattern,
                                         const clang::ASTContext &context);

bool GenericArgumentKey(const clang::TemplateArgument &argument,
                         const clang::ASTContext &context,std::string &key,
                         unsigned depth=0);

bool GenericFactoryBindings(const clang::FunctionDecl *function,std::string &key);

bool GenericLambdaBodies(const clang::LambdaExpr *expression,
                          const clang::ASTContext &context,
                          std::vector<GenericLambdaBody> &bodies);

bool HasAddressTemplateArgument(const clang::TemplateArgument &,const clang::ASTContext &);

bool HasClassLifecycle(const clang::CXXRecordDecl *record);

bool HasClassStructure(const clang::CXXRecordDecl *record);

bool HasDeclarationClosureFactory(const clang::NamedDecl *declaration);

bool HasDeclarationConcept(const clang::NamedDecl *declaration);

bool HasDeclarationEffectfulLambda(const clang::NamedDecl *declaration);

bool HasDeclarationFactoryTypeProjection(const clang::NamedDecl *declaration);

bool HasDeclarationGenericLambda(const clang::NamedDecl *declaration);

bool HasDeclarationLambda(const clang::NamedDecl *declaration);

bool HasDeclarationLocalExpression(const clang::NamedDecl *declaration, bool block);

bool HasDeclarationLocalFactoryResult(const clang::NamedDecl *declaration);

bool HasDeclarationLocalType(const clang::NamedDecl *declaration);

bool HasDeclarationCheckedCppRecordResult(const clang::NamedDecl *declaration);
bool HasDeclarationCppCarrierStorage(const clang::NamedDecl *declaration);
bool HasDeclarationCheckedCppMessage(const clang::NamedDecl *declaration);
bool HasDeclarationCppCarrierComposition(const clang::NamedDecl *declaration);
bool HasDeclarationCppCarrierCall(const clang::NamedDecl *declaration);
bool HasDeclarationResourceKeyPathValue(const clang::NamedDecl *declaration);
bool HasDeclarationNullableKeyPathWrite(const clang::NamedDecl *declaration);
bool HasDeclarationCheckedKeyPathRead(const clang::NamedDecl *declaration);
bool HasDeclarationDependentKeyPathLiteral(const clang::NamedDecl *declaration);
bool HasDeclarationDependentKeyPathTypeBody(const clang::NamedDecl *declaration);
bool HasDeclarationNativeDependentAwait(const clang::NamedDecl *);
bool HasDeclarationNSObjectStorage(const clang::NamedDecl *);
bool HasDeclarationAutomaticOwnerUnwind(const clang::NamedDecl *declaration);
bool HasDeclarationNativeArrayLifetime(const clang::NamedDecl *declaration);
bool HasDeclarationNativeSubobject(const clang::NamedDecl *declaration);
bool HasDeclarationNativeAllocation(const clang::NamedDecl *declaration);
bool HasDeclarationNativeDisposal(const clang::NamedDecl *declaration);
bool HasDeclarationInitializedOwner(const clang::NamedDecl *declaration);
bool HasDeclarationNativeFinally(const clang::NamedDecl *declaration);
bool HasDeclarationManagedRuntimeScope(const clang::NamedDecl *declaration);
bool HasDeclarationBorrowedReceiverResult(const clang::NamedDecl *declaration);
bool HasDeclarationBorrowedInteriorStorage(const clang::NamedDecl *declaration);
bool HasDeclarationBorrowedCallableOwner(const clang::NamedDecl *declaration);
bool HasDeclarationAsyncConsumedOwnership(const clang::NamedDecl *declaration);

bool HasDeclarationTemplateTemplateParameters(const clang::NamedDecl *declaration);

bool HasDeclarationThreadLocal(const clang::NamedDecl *declaration);

bool HasDeclarationUnionValue(const clang::NamedDecl *declaration);

bool HasDeclarationUnnamedLocalType(const clang::NamedDecl *declaration);

bool HasDirectMemberTemplates(const clang::CXXRecordDecl *record);

bool HasEffectfulTemplateProjection(const clang::NamedDecl *declaration,
                                     const clang::ASTContext &context);

bool HasFactoryTemplateArgument(const clang::TemplateArgument &argument,const clang::ASTContext &context,FactoryTypeFeatures *features=nullptr);

bool HasFactoryValueArgument(const clang::TemplateArgument &,const clang::ASTContext &);

bool HasOnlyProjectedLocalTypes(clang::QualType type);

bool HasRuntimeDeclarationContract(const clang::NamedDecl *declaration,
                                   const clang::ASTContext &context);

bool HasTemplateTemplateParameters(const clang::TemplateParameterList *parameters);

std::string HashInterfaceFacts(llvm::StringRef facts);

std::string InterfaceQualifiedName(const clang::NamedDecl *declaration);

bool IsBoundedClassMemberAlias(const clang::TypeAliasTemplateDecl *pattern);

bool IsBoundedClassMemberTemplate(const clang::FunctionTemplateDecl *pattern);

bool IsBoundedClassMemberVariable(const clang::VarTemplateDecl *pattern);

bool IsBoundedClosedParentMembers(const clang::NamedDecl *owner);

bool IsBoundedClosedParentPatterns(const clang::NamedDecl *owner);

bool IsBoundedConcreteMemberFunctionSpecialization(const clang::FunctionDecl *function);

bool IsBoundedConcreteMemberVariableSpecialization(const clang::VarTemplateSpecializationDecl *variable);

bool IsBoundedCxxDeclaration(const clang::NamedDecl *declaration,
                             InterfaceEmissionArtifact &result,
                             const std::vector<std::string> &exports = {});

bool IsBoundedNestedTemplateParameters(const clang::TemplateParameterList *parameters,
                                        unsigned depth=0);

bool IsBoundedPublishedClassTemplate(const clang::ClassTemplateDecl *pattern);

bool IsBoundedTemplateRecord(const clang::CXXRecordDecl *record,
                            const clang::ClassTemplateDecl *pattern=nullptr);

bool IsBoundedVariablePartial(const clang::VarTemplatePartialSpecializationDecl *partial);

bool IsClosedParentMemberSpecialization(const clang::NamedDecl *declaration);

bool IsClosedParentPattern(const clang::NamedDecl *declaration);

bool IsConcreteFriend(const clang::FriendDecl *declaration);

bool IsConcreteMemberClassSpecialization(const clang::ClassTemplateSpecializationDecl *record);

bool IsConcreteMemberFunctionSpecialization(const clang::FunctionDecl *function);

bool IsConcreteMemberVariableSpecialization(const clang::VarTemplateSpecializationDecl *variable);

bool IsConcreteSpecialMemberSpecialization(const clang::FunctionDecl *function);

bool IsDefinition(const clang::NamedDecl *declaration);

bool IsDependentMemberPartial(const clang::NamedDecl *declaration);

bool IsDetachedCallableTemplateDefinition(const clang::FunctionDecl *function);

bool IsDetachedCallableTemplateRoot(const clang::NamedDecl *declaration);

bool IsDetachedNestedTemplateDefinition(const clang::CXXRecordDecl *record);

bool IsDetachedNestedTemplateRoot(const clang::NamedDecl *declaration);

bool IsDetachedStaticDataTemplateDefinition(const clang::VarDecl *variable);

bool IsDetachedStaticDataTemplateRoot(const clang::NamedDecl *declaration);

bool IsExplicitNamespaceClassSpecialization(const clang::ClassTemplateSpecializationDecl *record);

bool IsExplicitNamespaceFunctionSpecialization(const clang::FunctionDecl *function);

bool IsExplicitNamespaceVariableSpecialization(const clang::VarTemplateSpecializationDecl *variable);

bool IsExpressionLocalTag(const clang::TagDecl *declaration);

bool IsForeignImportAttribute(const clang::Attr *attribute);

bool IsIdentityCandidate(const clang::NamedDecl *declaration,
                         const clang::ASTContext &context,
                         const std::string &source_root = {});

bool IsInClassDefaultedComparison(const clang::FunctionDecl *function);

bool IsInMainFile(const clang::ASTContext &context,
                  clang::SourceLocation location,
                  const std::string &source_root = {});

bool IsModeledNamespaceAttribute(const clang::Attr *attribute);

bool IsOutOfLineDefaultedDefinition(const clang::NamedDecl *declaration);

bool IsPublicContainer(const clang::DeclContext *context,
                       const clang::ASTContext &ast,
                       const std::string &source_root = {});

bool IsPublishedDeducedFactory(const clang::FunctionDecl *function);

bool IsPublishedNativeAttribute(const clang::Attr *attribute);
bool IsPublishedSemanticAttribute(const clang::Attr *attribute);

bool IsPublishedType(const clang::NamedDecl *declaration);

bool IsSupportedDefaultedSpecialMember(const clang::CXXMethodDecl *method);

bool IsTopLevel(const clang::NamedDecl *declaration);

bool IsWithinFactoryResult(const clang::TagDecl *tag,const clang::TagDecl *result);

clang::Expr *KeyPathSubscriptSyntax(clang::PseudoObjectExpr *expression);

std::string NamespaceAttributeText(const clang::Attr *attribute,
                                  const clang::ASTContext &context);

std::set<std::string> NamespaceAttributes(const clang::NamespaceDecl *space, bool canonical = true);

std::string NativeAttributeValue(const clang::Attr *,const clang::NamedDecl *);

std::string ObjCContainerIdentity(const clang::NamedDecl *declaration,
                                  const clang::ASTContext &context);

std::string ObjCMethodIdentity(const clang::ObjCMethodDecl *method,
                               const clang::ASTContext &context);

void PrintNamespaceHead(const clang::NamespaceDecl *space, llvm::raw_ostream &out,
                         bool attributes);

void PrintRootDeclaration(clang::NamedDecl *declaration,
                          clang::ASTContext &context,
                          llvm::raw_ostream &out, bool declaration_only = false);

void PrintUnscopedRootDeclaration(clang::NamedDecl *declaration,
                          clang::ASTContext &context,
                          llvm::raw_ostream &out, bool declaration_only = false);

const clang::Expr *PublishedAwaitSource(const clang::CoawaitExpr *await,
                                        const clang::FunctionDecl *function,
                                        const clang::ASTContext &context);

std::vector<const clang::ClassTemplateSpecializationDecl *> PublishedClassSpecializations(
    const clang::ClassTemplateDecl *pattern);

const clang::CXXRecordDecl *PublishedClosureResult(const clang::FunctionDecl *function);

const clang::CoroutineBodyStmt *PublishedCxxCoroutine(const clang::FunctionDecl *function);

const clang::TagDecl *PublishedFactoryResult(const clang::FunctionDecl *function);

std::vector<const clang::TagDecl *> PublishedFactoryResults(const clang::FunctionDecl *function);

const clang::Stmt *PublishedFunctionBody(const clang::FunctionDecl *function);

std::vector<const clang::VarTemplatePartialSpecializationDecl *> PublishedVariablePartials(
    const clang::VarTemplateDecl *pattern);

bool PublishesFunctionBody(const clang::FunctionDecl *function);

bool PublishesVariableInitializer(const clang::VarDecl *variable);

void ReadFactoryArgumentFeatures(const clang::TemplateArgument &,const clang::ASTContext &,FactoryTypeFeatures &);

std::string RecordLayoutIdentity(const clang::RecordDecl *record,
                                const clang::ASTContext &context);

clang::NamedDecl *Representative(clang::NamedDecl *declaration);

std::vector<std::string> RoundTripArguments(const clang::ASTContext &context);

bool SameDeclaration(const InterfaceDeclarationIdentity &a,
                     const InterfaceDeclarationIdentity &b);

bool SelectedExport(const clang::NamedDecl *declaration,
                    const std::vector<std::string> &exports);

bool StructuralArgumentKey(const clang::APValue &value,const clang::ASTContext &context,
                           std::string &key,unsigned &budget,unsigned depth=0,
                           std::vector<const clang::NamedDecl *> *references=nullptr);

const clang::NamedDecl *TemplatePublicationOwner(const clang::NamedDecl *declaration);

std::string UnnamedTemplateDeclarationTypeKey(const clang::NamedDecl *declaration);

std::string UnnamedTemplateRecordKey(const clang::CXXRecordDecl *,const clang::ASTContext &);

std::string UnnamedTemplateTagKey(const clang::TagDecl *tag,const clang::ASTContext &context);

std::string UnnamedTemplateTypeKey(clang::QualType,const clang::ASTContext &);

bool ValidateExportNames(const std::vector<std::string> &exports,
                         InterfaceEmissionArtifact &result);

std::string VariablePartialArgumentKey(const clang::VarTemplatePartialSpecializationDecl *partial,
                                       const clang::ASTContext &context);

const clang::Expr *WrittenDefaultArgument(const clang::ParmVarDecl *parameter);

std::vector<std::string> NeededStandardLibraryHeaders(const clang::NamedDecl *declaration,
                                                       const clang::ASTContext &context);

bool NeedsStandardCoroutineInclude(const clang::NamedDecl *declaration,
                                   const clang::ASTContext &context);

}  // namespace objc3::clang_foundation::emission_detail
