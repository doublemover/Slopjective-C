#include "standard_library_specializations.h"
#include "standard_coroutine_types.h"
#include "standard_library_headers.h"
#include "interface_reparse_options.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/DirectoryLookup.h"
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

class InterfaceCollector final
    : public clang::DynamicRecursiveASTVisitor {
 public:
   InterfaceCollector(clang::ASTContext &context,
                      InterfaceEmissionArtifact &result,
                      const std::set<std::string> &resolved_ids,
                      const std::vector<std::string> &exports = {},
                      const std::string &source_root = {})
       : context_(context), result_(result), resolved_ids_(resolved_ids),
         exports_(exports), source_root_(source_root) {}

   // Function bodies and initializers do not contribute public declarations.
   bool TraverseStmt(clang::Stmt *) override { return true; }

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

  bool VisitImportDecl(clang::ImportDecl *declaration) override {
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

  bool VisitNamedDecl(clang::NamedDecl *declaration) override {
    if (!result_.diagnostics.empty())
      return true;
    if (llvm::isa<clang::TemplateTypeParmDecl,clang::NonTypeTemplateParmDecl,
                  clang::TemplateTemplateParmDecl>(declaration)) return true;
    // The standard header supplies these namespace owners. Authored traits
    // customizations publish independently through their return-type root.
    if (const auto *space=llvm::dyn_cast<clang::NamespaceDecl>(declaration);
        space && (IsStandardCoroutineNamespace(space,context_) || IsStandardLibraryNamespace(space,context_))) {
      if (IsInMainFile(context_,space->getLocation()))
        for (const auto *attribute:space->attrs())
          if (!attribute->isImplicit() && !attribute->isInherited())
            AddDiagnostic(result_,"O3IFC101","a package cannot change native standard-library namespace attributes",space->getQualifiedNameAsString());
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
    if (auto *partial=llvm::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration);
        partial && !IsStandardLibrarySpecialization(partial)) {
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
    if (const auto *function=llvm::dyn_cast<clang::FunctionDecl>(declaration);
        function && !context_.getLangOpts().CPlusPlus && function->isInlined() &&
        context_.getLangOpts().GNUInline) {
      AddDiagnostic(result_,"O3IFC115","semantic C inline interfaces require ISO inline semantics",
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
      if (llvm::isa<clang::ObjCReturnsInnerPointerAttr>(attribute) &&
          llvm::isa<clang::ObjCMethodDecl, clang::ObjCPropertyDecl>(declaration))
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
                          llvm::raw_ostream &out, bool declaration_only) {
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
  auto *c_inline=llvm::dyn_cast<clang::FunctionDecl>(declaration);
  const bool project_c_inline=!context.getLangOpts().CPlusPlus && PublishesFunctionBody(c_inline);
  const auto c_inline_storage=c_inline?c_inline->getStorageClass():clang::SC_None;
  const bool c_inline_specified=c_inline && c_inline->isInlineSpecified();
  const bool c_inline_inherited=c_inline && c_inline->isInlined();
  if(project_c_inline) {
    c_inline->setStorageClass(clang::SC_None);
    c_inline->setInlineSpecified(true);
  }
  auto restore_c_inline=llvm::scope_exit([&] {
    if(project_c_inline) {
      c_inline->setStorageClass(c_inline_storage);
      c_inline->setInlineSpecified(c_inline_specified);
      c_inline->setImplicitlyInline(c_inline_inherited);
    }
  });
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
                          llvm::raw_ostream &out, bool declaration_only) {
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
    if (IsPublishedStandardSpecialization(declaration))
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
    auto restore = llvm::scope_exit([&] {
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

std::vector<std::string> RoundTripArguments(const clang::ASTContext &context,
                                             const clang::Preprocessor *preprocessor=nullptr) {
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
  // Reparse against the producer's effective search order, including a
  // relocated SDK or another native standard library. These physical paths
  // are transient compiler inputs and never become interface identities.
  if(preprocessor)AppendInterfaceHeaderSearchArguments(*preprocessor,args);
  args.push_back(lang.Exceptions ? "-fexceptions" : "-fno-exceptions");
  if (lang.CPlusPlus)
    args.insert(args.end(), {"-Xclang", lang.CXXExceptions
        ? "-fcxx-exceptions" : "-fno-cxx-exceptions"});
  if (lang.ObjC) {
    args.push_back(lang.ObjCExceptions ? "-fobjc-exceptions" : "-fno-objc-exceptions");
  }
  if(lang.CPlusPlus && !lang.RTTI)args.push_back("-fno-rtti");
  if(lang.CPlusPlus && !lang.ThreadsafeStatics)args.push_back("-fno-threadsafe-statics");
  if(lang.CPlusPlus) {
    static constexpr const char *policies[]={"all","thread-local","none"};
    const auto policy=static_cast<unsigned>(lang.getRegisterStaticDestructors());
    assert(policy<std::size(policies));
    args.push_back(std::string("-fc++-static-destructors=")+policies[policy]);
  }
  if (lang.CPlusPlus20) args.push_back(lang.CoroAlignedAllocation
      ? "-fcoro-aligned-allocation" : "-fno-coro-aligned-allocation");
  if (lang.Blocks) args.push_back("-fblocks");
  if (lang.ShortEnums) args.push_back("-fshort-enums");
  args.push_back(lang.CharIsSigned ? "-fsigned-char" : "-funsigned-char");
  if (lang.WCharSize) {
    const char *underlying = lang.WCharSize == 1 ? "char" : lang.WCharSize == 2 ? "short" : "int";
    args.insert(args.end(), {"-Xclang", std::string("-fwchar-type=") + underlying});
  }
  if (!lang.WCharIsSigned) args.insert(args.end(), {"-Xclang", "-fno-signed-wchar"});
  switch (lang.getClangABICompat()) {
#define ABI_VER_MAJOR_MINOR(Major, Minor) \
  case clang::LangOptions::ClangABI::Ver##Major##_##Minor: \
    args.push_back("-fclang-abi-compat=" #Major "." #Minor); break;
#define ABI_VER_MAJOR(Major) \
  case clang::LangOptions::ClangABI::Ver##Major: \
    args.push_back("-fclang-abi-compat=" #Major ".0"); break;
#define ABI_VER_LATEST(Latest) case clang::LangOptions::ClangABI::Latest: break;
#include "clang/Basic/ABIVersions.def"
  }
  const auto &target = context.getTargetInfo();
  if (!target.getTargetOpts().CPU.empty())
    args.insert(args.end(), {"-Xclang", "-target-cpu", "-Xclang", target.getTargetOpts().CPU});
  if (!target.getTargetOpts().ABI.empty())
    args.insert(args.end(), {"-Xclang", "-target-abi", "-Xclang", target.getTargetOpts().ABI});
  for (const auto &feature : target.getTargetOpts().FeaturesAsWritten)
    args.insert(args.end(), {"-Xclang", "-target-feature", "-Xclang", feature});
  if (lang.PackStruct) args.push_back("-fpack-struct=" + std::to_string(lang.PackStruct));
  if (lang.MaxTypeAlign) args.push_back("-fmax-type-align=" + std::to_string(lang.MaxTypeAlign));
  if (lang.AlignDouble) args.push_back("-malign-double");
  if (lang.LongDoubleSize) args.push_back("-mlong-double-" + std::to_string(lang.LongDoubleSize));

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

}  // namespace emission_detail

using namespace emission_detail;


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


namespace {
bool ValidateOwnershipProjection(clang::ASTContext &context,
                                 llvm::StringRef producer_sidecar,
                                 InterfaceEmissionArtifact &result,
                                 const clang::Preprocessor *preprocessor);
}

InterfaceEmissionArtifact EmitSemanticInterface(
    clang::ASTContext &context, llvm::StringRef canonical_module_name,
    llvm::StringRef package_identity,
    const std::vector<InterfaceDependency> &resolved_dependencies,
    const std::vector<InterfaceSourceArtifact> &dependency_sources,
    llvm::StringRef ownership_projection,
    const std::vector<std::string> &exports, const std::string &source_root,
    const clang::Preprocessor *preprocessor) {
  StandardLibraryHeaderCache standard_header_cache(&context,preprocessor);
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
  result.identity.language_profile = ReadInterfaceLanguageProfile(context);
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
  std::set<std::string> standard_headers;
  for(auto *root:collector.RootDeclarations())
    for(const auto &header:NeededStandardLibraryHeaders(root,context))standard_headers.insert(header);
  for(const auto &header:standard_headers)source << "#include <" << header << ">\n";
  for (auto *root:collector.RootDeclarations()) {
    if (NeedsStandardCoroutineInclude(root, context)) {source << "#include <coroutine>\n";break;}
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
       !ValidateOwnershipProjection(context, ownership_projection, result, preprocessor))) {
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
    if (HasNativeDependentAwaitIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeDependentAwaitSemantic);
    if (HasNSObjectStorageIdentity(declaration))
      sidecar.required_semantics.push_back(kNSObjectStorageSemantic);
    if (HasAutomaticOwnerUnwindIdentity(declaration))
      sidecar.required_semantics.push_back(kAutomaticOwnerUnwindSemantic);
    if (HasNativeArrayLifetimeIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeArrayLifetimeSemantic);
    if (HasNativeSubobjectIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeSubobjectSemantic);
    if (HasCxxIncompleteTemplateIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxIncompleteTemplateSemantic);
    if (HasNativeAllocationIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeAllocationSemantic);
    if (HasNativeDisposalIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeDisposalSemantic);
    if (HasInitializedOwnerIdentity(declaration))
      sidecar.required_semantics.push_back(kInitializedOwnerSemantic);
    if (HasNativeFinallyIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeFinallySemantic);
    if (HasManagedRuntimeScopeIdentity(declaration))
      sidecar.required_semantics.push_back(kManagedRuntimeScopeSemantic);
    if (HasBorrowedReceiverResultIdentity(declaration))
      sidecar.required_semantics.push_back(kBorrowedReceiverResultSemantic);
    if (HasBorrowedInteriorStorageIdentity(declaration))
      sidecar.required_semantics.push_back(kBorrowedInteriorStorageSemantic);
    if (HasBorrowedCallableOwnerIdentity(declaration))
      sidecar.required_semantics.push_back(kBorrowedCallableOwnerSemantic);
    if (HasAsyncConsumedOwnershipIdentity(declaration))
      sidecar.required_semantics.push_back(kAsyncConsumedOwnershipSemantic);
    if (HasDependentKeyPathTypeIdentity(declaration))
      sidecar.required_semantics.push_back(kDependentKeyPathTypeSemantic);
    if (HasDependentKeyPathLiteralIdentity(declaration))
      sidecar.required_semantics.push_back(kDependentKeyPathLiteralSemantic);
    if (HasCheckedKeyPathReadIdentity(declaration))
      sidecar.required_semantics.push_back(kCheckedKeyPathReadSemantic);
    if (HasNativeStorageLifetimeIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeStorageLifetimeSemantic);
    if (HasAtomicManagedPropertyIdentity(declaration))
      sidecar.required_semantics.push_back(kAtomicManagedPropertySemantic);
    if (HasNativeARCRecordPayloadIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeARCRecordPayloadSemantic);
    if (HasNativeCarrierLambdaIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeCarrierLambdaSemantic);
    if (HasNativeByrefStorageIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeByrefStorageSemantic);
    if (HasNativeBlockAlignmentIdentity(declaration))
      sidecar.required_semantics.push_back(kNativeBlockAlignmentSemantic);
    if (HasMixedCarrierCallIdentity(declaration))
      sidecar.required_semantics.push_back(kMixedCarrierCallSemantic);
    if (HasCppCarrierRuntimeIdentity(declaration))
      sidecar.required_semantics.push_back(kCppCarrierRuntimeSemantic);
    if (HasCheckedCppRecordResultIdentity(declaration))
      sidecar.required_semantics.push_back(kCheckedCppRecordResultSemantic);
    if (HasCppCarrierStorageIdentity(declaration))
      sidecar.required_semantics.push_back(kCppCarrierStorageSemantic);
    if (HasCheckedCppMessageIdentity(declaration))
      sidecar.required_semantics.push_back(kCheckedCppMessageSemantic);
    if (HasCppCarrierCompositionIdentity(declaration))
      sidecar.required_semantics.push_back(kCppCarrierCompositionSemantic);
    if (HasCppCarrierCallIdentity(declaration))
      sidecar.required_semantics.push_back(kCppCarrierCallSemantic);
    if (HasResourceKeyPathValueIdentity(declaration))
      sidecar.required_semantics.push_back(kResourceKeyPathValueSemantic);
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
    if (HasCInlineDefinitionIdentity(declaration))
      sidecar.required_semantics.push_back(kCInlineDefinitionSemantic);
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
    if (HasCxxStandardLibraryIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxStandardLibrarySemantic);
    if (HasCxxStandardLibrarySpecializationIdentity(declaration))
      sidecar.required_semantics.push_back(kCxxStandardLibrarySpecializationSemantic);
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
    if (!effects.async_abi_identity.empty() || !effects.actor_isolation_identity.empty() ||
        !effects.executor_affinity_identity.empty())
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

  const auto supported_semantics = SupportedSemanticInterfaceCapabilities();
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
  auto roundtrip_arguments = RoundTripArguments(context, preprocessor);
  bool complete_standard_headers=!standard_headers.empty();
  for(const auto &entry:sources_by_name)
    complete_standard_headers |= std::any_of(entry.second->declarations.begin(),entry.second->declarations.end(),HasCxxStandardLibraryIdentity);
  AddRuntimeInterfaceFiles(virtual_files, roundtrip_arguments,
                           collector.UsesCxxAsyncBodies(),complete_standard_headers);
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
  standard_header_cache.preprocessors.emplace(&roundtrip->getASTContext(),&roundtrip->getPreprocessor());
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
const auto kProjectionSemantics = SupportedSemanticInterfaceCapabilities();

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
                                             bool arc,
                                             const clang::Preprocessor *preprocessor) {
  auto args = RoundTripArguments(context, preprocessor);
  args.erase(std::remove(args.begin(), args.end(), "-fobjc-arc"), args.end());
  args.push_back(arc ? "-fobjc-arc" : "-fno-objc-arc");
  return args;
}

bool ValidateOwnershipProjection(clang::ASTContext &context,
                                 llvm::StringRef producer_sidecar,
                                 InterfaceEmissionArtifact &result,
                                 const clang::Preprocessor *preprocessor) {
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
      ProjectionArguments(context, !context.getLangOpts().ObjCAutoRefCount, preprocessor);
  clang::tooling::FileContentMappings files;
  AddRuntimeInterfaceFiles(files, arguments, result.source_bytes.find(
      "#include <objc3c/runtime/objc3_task.hpp>") != std::string::npos,
      std::find(original->required_semantics.begin(),original->required_semantics.end(),kCxxStandardLibrarySemantic)!=original->required_semantics.end());
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
                                           original->module.package_identity, {}, {}, {}, {}, {}, &ast->getPreprocessor());
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
    const std::vector<std::string> &exports, const std::string &source_root,
    const clang::Preprocessor *preprocessor) {
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
      package_identity, {}, {}, {}, exports, source_root, preprocessor);
  if (!original.success)
    return original;
  auto arguments = ProjectionArguments(producer, consumer_arc, preprocessor);
  clang::tooling::FileContentMappings files;
  auto original_contract=DecodeSemanticInterfaceSidecar(original.sidecar_json,original.source_bytes,kProjectionSemantics);
  if(!original_contract) {
    AddDiagnostic(result,"O3IFC152",llvm::toString(original_contract.takeError()));return result;
  }
  AddRuntimeInterfaceFiles(files, arguments, original.source_bytes.find(
      "#include <objc3c/runtime/objc3_task.hpp>") != std::string::npos,
      std::find(original_contract->required_semantics.begin(),original_contract->required_semantics.end(),kCxxStandardLibrarySemantic)!=original_contract->required_semantics.end());
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
                            package_identity, {}, {}, original.sidecar_json, {}, {}, &ast->getPreprocessor());
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
