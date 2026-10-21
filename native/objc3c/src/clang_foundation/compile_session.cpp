#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/sdk_identity.h"
#include "clang_foundation/source_identity.h"

#include "clang_foundation/ast_contract_consumer.h"
#include "clang_foundation/interface_artifact_io.h"
#include "clang_foundation/interface_source_import_bridge.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Basic/ObjC3Foundation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Version.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Driver.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/MultiplexConsumer.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Options/Options.h"
#include "clang/Sema/ExternalSemaSource.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/Sema.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"
#include <algorithm>
#include <map>
#include <mutex>
#include <set>

#ifndef OBJC3C_FOUNDATION_DRIVER
#error "The verified patched Clang driver path must be supplied by the build"
#endif
#ifndef OBJC3C_FOUNDATION_RESOURCE_DIR
#error "The matching Clang resource directory must be supplied by the build"
#endif
#ifndef OBJC3C_FOUNDATION_DRIVER_SHA256
#error "The matching Clang driver digest must be supplied by the build"
#endif
#ifndef OBJC3C_FOUNDATION_RESOURCE_SHA256
#error "The matching Clang resource directory digest must be supplied by the build"
#endif
#ifndef OBJC3C_FOUNDATION_PATCH_DIGEST
#error "The sealed source patch identity must be supplied by the build"
#endif

namespace objc3::clang_foundation {
namespace {
constexpr const char *Languages[] = {"c", "c++", "objective-c", "objective-c++"};
constexpr const char *Profiles[] = {
    "compatibility", "strict", "strict-concurrency", "strict-system"};

class Diagnostics final : public clang::DiagnosticConsumer {
 public:
  explicit Diagnostics(CompileResult &result) : result_(result) {}
  void HandleDiagnostic(clang::DiagnosticsEngine::Level level,
                        const clang::Diagnostic &info) override {
    clang::DiagnosticConsumer::HandleDiagnostic(level, info);
    llvm::SmallString<256> message;
    info.FormatDiagnostic(message);
    CompileDiagnostic diagnostic;
    diagnostic.id = info.getID();
    diagnostic.severity = static_cast<unsigned>(level);
    diagnostic.message = message.str().str();
    if (info.hasSourceManager() && info.getLocation().isValid()) {
      const auto &manager = info.getSourceManager();
      auto locate = [&](clang::SourceLocation location) {
        DiagnosticLocation result;
        auto presumed = manager.getPresumedLoc(location);
        if (presumed.isValid()) {
          result.file = presumed.getFilename();
          result.line = presumed.getLine();
          result.column = presumed.getColumn();
        }
        return result;
      };
      diagnostic.spelling = locate(manager.getSpellingLoc(info.getLocation()));
      diagnostic.expansion = locate(manager.getExpansionLoc(info.getLocation()));
    }
    result_.diagnostics.push_back(std::move(diagnostic));
  }
 private:
  CompileResult &result_;
};


class InterfaceImportConsumer final : public clang::ASTConsumer {
public:
  InterfaceImportConsumer(InterfaceImportSession *session, clang::CompilerInstance &compiler,
                          llvm::IntrusiveRefCntPtr<InterfaceSourceImportBridge> lookup,
                          const CompileRequest &request,
                          CompileResult &result)
      : session_(session), compiler_(compiler), lookup_(std::move(lookup)),
        request_(request), result_(result) {}

  void Initialize(clang::ASTContext &context) override {
    const auto &language = context.getLangOpts();
    const auto &predefines = compiler_.getPreprocessorOpts();
    if (session_ && language.ObjC && language.CPlusPlus &&
        (language.ObjCAutoRefCount || language.ObjCWeak) &&
        predefines.UsePredefines &&
        predefines.ObjCXXARCStandardLibrary == clang::ARCXX_libstdcxx) {
      // ASTImporter may pull in the compiler's ARC type-trait specializations.
      // Parse their owning builtin namespace before merging provider ASTs.
      deferred_context_ = &context;
      return;
    }
    ImportDeclarations(context);
  }

  bool HandleTopLevelDecl(clang::DeclGroupRef declarations) override {
    if (deferred_context_)
      for (const auto *declaration : declarations)
        if (const auto *space =
                llvm::dyn_cast<clang::NamespaceDecl>(declaration);
            space && space->getName() == "std" &&
            deferred_context_->getSourceManager().isWrittenInBuiltinFile(
                space->getLocation())) {
          auto *context = deferred_context_;
          deferred_context_ = nullptr;
          ImportDeclarations(*context);
          break;
        }
    return true;
  }

  void ImportDeclarations(clang::ASTContext &context) {
    if (!session_) {
      source_scoped_ = request_.source_directed_imports;
      return;
    }
    auto imported = session_->ImportInto(context, compiler_.getFileManager(),
                                        request_.source_directed_imports);
    result_.interface_import_diagnostics = imported.diagnostics;
    if (!imported.success) {
      std::string message;
      for (const auto &diagnostic : imported.diagnostics) {
        if (!message.empty()) message += "; ";
        message += diagnostic.code + ": " + diagnostic.message;
        if (!diagnostic.subject.empty()) message += " [" + diagnostic.subject + "]";
      }
      result_.failure = "Interface import failed: " + message;
      auto id = context.getDiagnostics().getCustomDiagID(
          clang::DiagnosticsEngine::Error, "%0");
      context.getDiagnostics().Report(id) << result_.failure;
      return;
    }
    result_.imported_declarations = imported.imported_declarations;
    result_.resolved_interface_dependencies =
        std::move(imported.direct_dependencies);
    result_.interface_dependency_order = std::move(imported.dependency_order);
    result_.resolved_module_identities = std::move(imported.resolved_modules);
    result_.resolved_interface_sources = session_->GetSourceArtifacts();
    if (lookup_ && !request_.module_imports.empty()) {
      lookup_->ConfigureSourceRegistry(request_.module_imports, imported,
                                       result_.resolved_interface_sources);
      if (compiler_.hasSema())
        lookup_->InitializeSema(compiler_.getSema());
    }
    if (request_.source_directed_imports) {
      source_scoped_ = true;
      return;
    }
    result_.visible_module_names = std::move(imported.visible_module_names);
    // C IdentifierResolver cannot discover declarations inserted directly
    // by ASTImporter into the TU. Supply only root-visible candidates through
    // Clang's normal ExternalSemaSource fallback.
    if (lookup_)
      for (auto *declaration : imported.visible_lookup_declarations)
        lookup_->AddInitiallyVisible(declaration);
  }

  void HandleTranslationUnit(clang::ASTContext &context) override {
    if (deferred_context_) {
      result_.failure =
          "The compiler ARC prelude did not complete before interface import";
      auto id = context.getDiagnostics().getCustomDiagID(
          clang::DiagnosticsEngine::Error, "%0");
      context.getDiagnostics().Report(id) << result_.failure;
      return;
    }
    if (!source_scoped_) return;
    const auto visible = lookup_->GetVisibleModules();
    result_.visible_module_names = visible;
    result_.resolved_module_identities = lookup_->GetImportedModuleIdentities();
    result_.interface_dependency_order = lookup_->GetDependencyOrder();
    result_.resolved_interface_sources = lookup_->GetResolvedSourceArtifacts();
    result_.interface_input_paths = lookup_->GetResolvedInputPaths();
    result_.imported_declarations = lookup_->GetVisibleDeclarationIdentities();
    result_.resolved_interface_dependencies = lookup_->GetDirectDependencies();
    result_.interface_import_diagnostics = lookup_->GetDiagnostics();
    if (!lookup_->GetLastFailure().empty() && result_.failure.empty())
      result_.failure = lookup_->GetLastFailure().str();
  }

private:
  InterfaceImportSession *session_ = nullptr;
  clang::CompilerInstance &compiler_;
  llvm::IntrusiveRefCntPtr<InterfaceSourceImportBridge> lookup_;
  const CompileRequest &request_;
  CompileResult &result_;
  bool source_scoped_ = false;
  clang::ASTContext *deferred_context_ = nullptr;
};
template <typename NativeAction>
class ContractAction final : public NativeAction {
 public:
  ContractAction(const CompileRequest &request, CompileResult &result,
                InterfaceImportSession *imports,
                llvm::IntrusiveRefCntPtr<InterfaceSourceImportBridge> lookup)
      : request_(request), result_(result), imports_(imports),
        lookup_source_(std::move(lookup)) {}
 protected:
  bool BeginSourceFileAction(clang::CompilerInstance &compiler) override {
    if (!NativeAction::BeginSourceFileAction(compiler)) return false;
    if (!request_.module_imports.empty()) {
      std::vector<std::string> names;
      for (const auto &module : request_.module_imports)
        names.push_back(module.canonical_name);
      compiler.getPreprocessor().setObjC3SourceModuleNames(names);
    }
    if (imports_ || request_.source_directed_imports) {
      if (!lookup_source_)
        lookup_source_ = llvm::makeIntrusiveRefCnt<InterfaceSourceImportBridge>();
      compiler.setExternalSemaSource(lookup_source_);
    }
    return true;
  }

  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance &compiler, llvm::StringRef input) override {
    auto native = NativeAction::CreateASTConsumer(compiler, input);
    if (!native) return nullptr;
    std::vector<std::unique_ptr<clang::ASTConsumer>> consumers;
    // BackendConsumer may clear the AST in HandleTranslationUnit before
    // emitting the native module. Observe that same AST while it is alive.
    if (imports_ || request_.source_directed_imports)
      consumers.push_back(std::make_unique<InterfaceImportConsumer>(
          imports_, compiler, lookup_source_, request_, result_));
    consumers.push_back(MakeContractConsumer(request_, result_));
    consumers.push_back(std::move(native));
    return std::make_unique<clang::MultiplexConsumer>(std::move(consumers));
  }
 private:
  const CompileRequest &request_;
  CompileResult &result_;
  InterfaceImportSession *imports_ = nullptr;
  llvm::IntrusiveRefCntPtr<InterfaceSourceImportBridge> lookup_source_;
};

class ActionFactory final : public clang::tooling::FrontendActionFactory {
 public:
  ActionFactory(const CompileRequest &request, CompileResult &result)
      : request_(request), result_(result) {}
  std::unique_ptr<clang::FrontendAction> create() override {
    if (request_.emit == EmitKind::Object)
      return std::make_unique<ContractAction<clang::EmitObjAction>>(
          request_, result_, import_session_.get(), lookup_source_);
    if (request_.emit == EmitKind::LLVM)
      return std::make_unique<ContractAction<clang::EmitLLVMAction>>(
          request_, result_, import_session_.get(), lookup_source_);
    return std::make_unique<ContractAction<clang::SyntaxOnlyAction>>(
        request_, result_, import_session_.get(), lookup_source_);
  }
  bool runInvocation(std::shared_ptr<clang::CompilerInvocation> invocation,
                     clang::FileManager *files,
                     std::shared_ptr<clang::PCHContainerOperations> containers,
                     clang::DiagnosticConsumer *diagnostics) override {
    const auto &language = invocation->getLangOpts();
    const bool cxx = request_.language == BaseLanguage::Cxx ||
                     request_.language == BaseLanguage::ObjectiveCxx;
    const bool objc = request_.language == BaseLanguage::ObjectiveC ||
                      request_.language == BaseLanguage::ObjectiveCxx;
    if (bool(language.CPlusPlus) != cxx || bool(language.ObjC) != objc ||
        bool(language.ObjCAutoRefCount) != request_.arc ||
        bool(language.ObjC3StrictConcurrency) !=
            (request_.strict_concurrency || request_.profile == SemanticProfile::StrictConcurrency) ||
        bool(language.ObjC3) != request_.objc3 ||
        (request_.objc3 && language.ObjC3Profile != static_cast<unsigned>(request_.profile))) {
      result_.failure = "Driver arguments conflict with the normalized language mode";
      return false;
    }
    if (request_.source_directed_imports && (!language.ObjC || !language.ObjC3)) {
      result_.failure = "Source-directed interface imports require an ObjC3 Objective-C or Objective-C++ translation unit";
      return false;
    }
    result_.target_triple = invocation->getTargetOpts().Triple;
    result_.runtime_abi = language.ObjC ? language.ObjCRuntime.getAsString() : "no-objc-runtime";
    if (!request_.imported_interfaces.empty() || !request_.module_imports.empty() ||
        !request_.interface_search_paths.empty() || request_.source_directed_imports) {
      InterfaceImportTarget target;
      target.target_triple = result_.target_triple;
      target.runtime_abi += result_.runtime_abi;
      target.runtime_abi += language.ObjCAutoRefCount ? ";arc" : ";manual-reference-counting";
      target.extension_abi = std::string("objc3-extension-abi-") +
          std::to_string(CLANG_OBJC3_EXTENSION_ABI_REVISION) + ":" +
          CLANG_OBJC3_PATCH_DIGEST;
      target.language_revision = language.ObjC3
          ? std::to_string(language.ObjC3Revision) : "objc3-disabled";
      static constexpr const char *profiles[] = {
          "compatibility", "strict", "strict-concurrency", "strict-system"};
      const auto profile_index = static_cast<unsigned>(language.ObjC3Profile);
      target.language_profile = ReadInterfaceLanguageProfile(language);
      target.objc = language.ObjC;
      target.objc3 = language.ObjC3;
      target.arc = language.ObjCAutoRefCount;
      target.blocks = language.Blocks;
      target.strict_concurrency = language.ObjC3StrictConcurrency;
      target.objc_runtime = language.ObjC ? language.ObjCRuntime.getAsString() : "";
      target.objc3_profile = profile_index < std::size(profiles)
          ? profiles[profile_index] : "compatibility";
      target.allow_function_overloads = language.CPlusPlus;
      const auto &standard = clang::LangStandard::getLangStandardForKind(language.LangStd);
      const char *import_language = language.ObjC
          ? (language.CPlusPlus ? "objective-c++" : "objective-c")
          : (language.CPlusPlus ? "c++" : "c");
      target.ast_arguments = BuildInterfaceASTArguments(
          result_.driver_arguments, import_language, standard.getName(), target);
      auto working_directory = files->getVirtualFileSystem().getCurrentWorkingDirectory();
      if (!working_directory) {
        result_.failure = "Cannot determine interface import working directory: " +
            working_directory.getError().message();
        return false;
      }
      target.ast_arguments.insert(target.ast_arguments.end(),
          {"-working-directory", *working_directory});
      auto search_paths = request_.interface_search_paths;
      auto make_absolute = [&](std::string &path) {
        llvm::SmallString<256> absolute(path);
        if (path.empty()) {
          result_.failure = "Interface import path must not be empty";
          return false;
        }
        if (auto error = files->getVirtualFileSystem().makeAbsolute(absolute)) {
          result_.failure = "Cannot resolve interface import path: " + error.message();
          return false;
        }
        path = std::string(absolute);
        return true;
      };
      for (auto &path : search_paths)
        if (!make_absolute(path)) return false;
      std::vector<InterfaceContractDiagnostic> import_diagnostics;
      std::vector<InterfaceImportInput> resolved_inputs;
      if (request_.source_directed_imports) {
        if (request_.module_imports.empty() ||
            request_.interface_search_paths.empty() ||
            !request_.imported_interfaces.empty()) {
          result_.failure = "Source-directed imports require a locked module registry and search paths, and cannot use pre-resolved artifact inputs";
          return false;
        }
        auto declared = request_.module_imports;
        for (auto &module : declared) {
          module.initially_visible = false;
          auto &identity = module.locked_identity;
          if (identity.canonical_name.empty()) identity.canonical_name = module.canonical_name;
          if (identity.target_triple.empty()) identity.target_triple = target.target_triple;
          if (identity.runtime_abi.empty()) identity.runtime_abi = target.runtime_abi;
          if (identity.extension_abi.empty()) identity.extension_abi = target.extension_abi;
          if (identity.language_revision.empty()) identity.language_revision = target.language_revision;
          if (identity.language_profile.empty()) identity.language_profile = target.language_profile;
        }
        lookup_source_ = llvm::makeIntrusiveRefCnt<InterfaceSourceImportBridge>();
        lookup_source_->ConfigureOnDemand(search_paths,
            declared, target, *files);
        if (!lookup_source_->GetLastFailure().empty()) {
          result_.failure = lookup_source_->GetLastFailure().str();
          return false;
        }
      }
      if (!request_.module_imports.empty()) {
        if (request_.interface_search_paths.empty() ||
            !request_.imported_interfaces.empty()) {
          result_.failure = "Declared module imports require search paths and cannot be mixed with resolved artifact inputs";
          return false;
        }
        if (request_.source_directed_imports)
          return FrontendActionFactory::runInvocation(std::move(invocation), files,
              std::move(containers), diagnostics);
        auto declared = request_.module_imports;
        for (auto &module : declared) {
          if (request_.source_directed_imports)
            module.initially_visible = false;
          auto &identity = module.locked_identity;
          if (identity.canonical_name.empty()) identity.canonical_name = module.canonical_name;
          // The lock records semantic fingerprint and package selection. The
          // remaining identity fields are fixed by this exact invocation.
          if (identity.target_triple.empty()) identity.target_triple = target.target_triple;
          if (identity.runtime_abi.empty()) identity.runtime_abi = target.runtime_abi;
          if (identity.extension_abi.empty()) identity.extension_abi = target.extension_abi;
          if (identity.language_revision.empty()) identity.language_revision = target.language_revision;
          if (identity.language_profile.empty()) identity.language_profile = target.language_profile;
        }
        auto resolution = ResolveInterfaceModules(
            search_paths, declared, target);
        if (!resolution.success) {
          result_.interface_import_diagnostics = std::move(resolution.diagnostics);
          result_.failure = "Module resolution failed";
          for (const auto &diagnostic : result_.interface_import_diagnostics)
            result_.failure += "; " + diagnostic.code + ": " + diagnostic.message;
          return false;
        }
        resolved_inputs = std::move(resolution.artifacts);
        result_.interface_dependency_order = std::move(resolution.dependency_order);
        if (!request_.source_directed_imports)
          result_.visible_module_names = std::move(resolution.visible_module_names);
      } else {
        resolved_inputs = request_.imported_interfaces;
        for (auto &input : resolved_inputs) {
          if (!make_absolute(input.source_path) ||
              (!input.sidecar_path.empty() && !make_absolute(input.sidecar_path)))
            return false;
        }
      }
      for (const auto &input : resolved_inputs) {
        result_.interface_input_paths.push_back(input.source_path);
        result_.interface_input_paths.push_back(input.sidecar_path.empty()
            ? input.source_path + ".json" : input.sidecar_path);
      }
      import_session_ = InterfaceImportSession::Prepare(
          resolved_inputs, target, import_diagnostics);
      if (!import_session_) {
        result_.interface_import_diagnostics = std::move(import_diagnostics);
        result_.failure = "Interface import preparation failed";
        for (const auto &diagnostic : result_.interface_import_diagnostics) {
          result_.failure += "; " + diagnostic.code + ": " + diagnostic.message;
          if (!diagnostic.subject.empty()) result_.failure += " [" + diagnostic.subject + "]";
        }
        return false;
      }
    }
    return FrontendActionFactory::runInvocation(std::move(invocation), files,
                                               std::move(containers), diagnostics);
  }
 private:
  const CompileRequest &request_;
  CompileResult &result_;
  std::unique_ptr<InterfaceImportSession> import_session_;
  llvm::IntrusiveRefCntPtr<InterfaceSourceImportBridge> lookup_source_;
};

std::string JsonQuote(llvm::StringRef value) {
  std::string out = "\"";
  static constexpr char Hex[] = "0123456789abcdef";
  for (unsigned char ch : value.bytes()) {
    switch (ch) {
    case '\\': out += "\\\\"; break;
    case '\"': out += "\\\""; break;
    case '\n': out += "\\n"; break;
    case '\r': out += "\\r"; break;
    case '\t': out += "\\t"; break;
    default:
      if (ch < 0x20) {
        out += "\\u00";
        out.push_back(Hex[ch >> 4]);
        out.push_back(Hex[ch & 0xf]);
      } else out.push_back(static_cast<char>(ch));
    }
  }
  out.push_back('\"');
  return out;
}

void BuildDependencyRecord(const CompileRequest &request,
                           CompileResult &result) {
  std::string json = "{\"schema\":\"objc3c.dependency-record.v2\",\"toolchain\":{\"compiler\":" +
      JsonQuote(result.compiler_identity) + ",\"driver_path\":" +
      JsonQuote(result.compiler_driver_path) + ",\"driver_sha256\":" +
      JsonQuote(result.compiler_driver_sha256) + ",\"resource_directory\":" +
      JsonQuote(result.resource_directory) + ",\"resource_sha256\":" +
      JsonQuote(result.resource_directory_sha256) + ",\"patch_digest\":" +
      JsonQuote(result.clang_patch_digest) + "},\"target\":" +
      JsonQuote(result.target_triple) + ",\"runtime\":" +
      JsonQuote(result.runtime_abi) + ",\"modules\":[";
  for (std::size_t i = 0; i < result.resolved_module_identities.size(); ++i) {
    if (i) json += ',';
    const auto &module = result.resolved_module_identities[i];
    json += "{\"name\":" + JsonQuote(module.canonical_name) +
      ",\"fingerprint\":" + JsonQuote(module.semantic_fingerprint) +
      ",\"target\":" + JsonQuote(module.target_triple) +
      ",\"runtime\":" + JsonQuote(module.runtime_abi) +
      ",\"extension_abi\":" + JsonQuote(module.extension_abi) +
      ",\"language_revision\":" + JsonQuote(module.language_revision) +
      ",\"language_profile\":" + JsonQuote(module.language_profile) +
      ",\"package\":" + JsonQuote(module.package_identity) + '}';
  }
  json += "],\"dependency_order\":[";
  for (std::size_t i = 0; i < result.interface_dependency_order.size(); ++i) {
    if (i) json += ',';
    json += JsonQuote(result.interface_dependency_order[i]);
  }
  json += "],\"edges\":[";
  bool first_edge = true;
  for (const auto &source : result.resolved_interface_sources)
    for (const auto &dependency : source.dependencies) {
      if (!first_edge) json += ',';
      first_edge = false;
      json += "{\"from\":" + JsonQuote(source.module.canonical_name) +
        ",\"to\":" + JsonQuote(dependency.module.canonical_name) +
        ",\"public\":" + (dependency.public_import ? "true" : "false") +
        ",\"reexported\":" + (dependency.reexported ? "true" : "false") + '}';
    }
  json += "],\"visible_modules\":[";
  for (std::size_t i = 0; i < result.visible_module_names.size(); ++i) {
    if (i) json += ',';
    json += JsonQuote(result.visible_module_names[i]);
  }
  json += "],\"spi_grants\":[";
  std::map<std::string, std::vector<std::string>> grants;
  for (const auto &module : request.module_imports)
    if (std::any_of(result.resolved_interface_dependencies.begin(),
                    result.resolved_interface_dependencies.end(),
                    [&](const auto &d) {
                      return d.module.canonical_name == module.canonical_name;
                    }))
      grants[module.canonical_name] = module.spi_groups;
  for (const auto &input : request.imported_interfaces)
    if (input.direct_import)
      grants[input.locked_identity.canonical_name] = input.spi_groups;
  bool first_grant = true;
  for (auto &[name, groups] : grants) {
    std::sort(groups.begin(), groups.end());
    for (const auto &group : groups) {
      if (!first_grant)
        json += ',';
      first_grant = false;
      json += "{\"module\":" + JsonQuote(name) +
              ",\"group\":" + JsonQuote(group) + '}';
    }
  }
  json += "],\"imported_declarations\":[";
  for (std::size_t i = 0; i < result.imported_declarations.size(); ++i) {
    if (i) json += ',';
    const auto &decl = result.imported_declarations[i];
    json += "{\"id\":" + JsonQuote(decl.stable_id) +
      ",\"kind\":" + JsonQuote(decl.kind) +
      ",\"name\":" + JsonQuote(decl.qualified_name) +
      ",\"type\":" + JsonQuote(decl.canonical_type_identity) +
      ",\"async\":" + (decl.effects.is_async ? "true" : "false") +
      ",\"throws_kind\":" + JsonQuote(decl.effects.throws_kind) +
      ",\"error_type\":" + JsonQuote(decl.effects.error_type_identity) + '}';
  }
  json += "]}";
  result.semantic_dependency_record = std::move(json);
}

struct TemporaryOutput {
  llvm::SmallString<256> path;
  ~TemporaryOutput() { if (!path.empty()) llvm::sys::fs::remove(path); }
};
}  // namespace

CompileResult Compile(const CompileRequest &request) {
  CompileResult result;
  result.compiler_identity = clang::getClangFullRepositoryVersion() +
      " Frontend-inputs=" + CompiledFrontendSourceIdentity();
  result.compiler_driver_path = OBJC3C_FOUNDATION_DRIVER;
  result.compiler_driver_sha256 = OBJC3C_FOUNDATION_DRIVER_SHA256;
  result.resource_directory = OBJC3C_FOUNDATION_RESOURCE_DIR;
  result.resource_directory_sha256 = OBJC3C_FOUNDATION_RESOURCE_SHA256;
  result.clang_patch_digest = OBJC3C_FOUNDATION_PATCH_DIGEST;
  if (!request.compiler_sdk_root.empty() &&
      !VerifyRelocatedSDK(request.compiler_sdk_root,
          OBJC3C_FOUNDATION_DRIVER_SHA256, OBJC3C_FOUNDATION_RESOURCE_SHA256,
          result.compiler_driver_path, result.resource_directory, result.failure))
    return result;
  if (result.compiler_identity.find("ObjC3-patches=" OBJC3C_FOUNDATION_PATCH_DIGEST) ==
      std::string::npos) {
    result.failure = "Linked Clang libraries differ from the sealed compiler SDK";
    return result;
  }
  const bool objc = request.language == BaseLanguage::ObjectiveC ||
                    request.language == BaseLanguage::ObjectiveCxx;
  if (request.input_path.empty() ||
      ((request.objc3 || request.arc || !request.objc_runtime.empty()) &&
       !objc) ||
      (!request.objc3 && request.profile != SemanticProfile::Compatibility) ||
      (request.strict_concurrency && !request.objc3) ||
      (request.emit_semantic_interface &&
       (request.module_name.empty() || request.package_identity.empty())) ||
      (request.projected_arc.has_value() &&
       (!objc || !request.objc3 || !request.emit_semantic_interface)) ||
      (!request.interface_exports.empty() &&
       !request.emit_semantic_interface) ||
      (!request.interface_output_path.empty() &&
       !request.emit_semantic_interface) ||
      (request.emit != EmitKind::Syntax && request.output_path.empty())) {
    result.failure = "Input path, base language, or output request is invalid";
    return result;
  }
  if (static_cast<unsigned>(request.language) >= 4 ||
      static_cast<unsigned>(request.profile) >= 4 ||
      static_cast<unsigned>(request.emit) >= 3) {
    result.failure = "Unknown language, profile, or output kind";
    return result;
  }
  static std::once_flag targets;
  std::call_once(targets, [] {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
  });
  auto physical = llvm::vfs::createPhysicalFileSystem();
  if (!request.working_directory.empty()) {
    if (auto error = physical->setCurrentWorkingDirectory(request.working_directory)) {
      result.failure = error.message();
      return result;
    }
  }
  auto overlay = llvm::makeIntrusiveRefCnt<llvm::vfs::OverlayFileSystem>(std::move(physical));
  auto memory = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
  if (request.source_text) {
    llvm::SmallString<256> virtual_path(request.input_path);
    if (auto error = overlay->makeAbsolute(virtual_path)) {
      result.failure = error.message();
      return result;
    }
    memory->addFile(virtual_path, 0, llvm::MemoryBuffer::getMemBufferCopy(*request.source_text));
  }
  overlay->pushOverlay(memory);
  auto files = llvm::makeIntrusiveRefCnt<clang::FileManager>(clang::FileSystemOptions(), overlay);
  TemporaryOutput temporary;
  llvm::SmallString<256> final_output(request.output_path);
  llvm::SmallString<256> interface_output(request.interface_output_path);
  llvm::SmallString<256> record_output(request.dependency_record_output_path);
  if ((!final_output.empty() && overlay->makeAbsolute(final_output)) ||
      (!interface_output.empty() && overlay->makeAbsolute(interface_output)) ||
      (!record_output.empty() && overlay->makeAbsolute(record_output))) {
    result.failure = "Cannot resolve output paths in the compilation working directory";
    return result;
  }
  auto aliases_record = [&](const std::string &path) {
    if (record_output.empty() || path.empty()) return false;
    llvm::SmallString<256> candidate(path);
    if (overlay->makeAbsolute(candidate)) return true;
    auto normalized_record = record_output;
    llvm::sys::path::remove_dots(normalized_record, true);
    llvm::sys::path::remove_dots(candidate, true);
#if defined(_WIN32)
    if (llvm::StringRef(candidate).equals_insensitive(normalized_record)) return true;
#else
    if (candidate == normalized_record) return true;
#endif
    bool equivalent = false;
    return !llvm::sys::fs::equivalent(candidate, normalized_record, equivalent) && equivalent;
  };
  if (!record_output.empty()) {
    if (aliases_record(request.input_path) || aliases_record(request.output_path) ||
        aliases_record(request.interface_output_path) ||
        (!request.interface_output_path.empty() &&
         aliases_record(request.interface_output_path + ".json"))) {
      result.failure = "Dependency record path conflicts with compilation input or outputs";
      return result;
    }
  }
  if (request.emit != EmitKind::Syntax) {
    auto error = llvm::sys::fs::createUniqueFile(std::string(final_output) + ".objc3c-%%%%%%",
                                               temporary.path);
    if (error) { result.failure = error.message(); return result; }
  }
  // Driver response expansion expects argv[0], including for empty user args.
  llvm::SmallVector<const char *, 32> expanded{result.compiler_driver_path.c_str()};
  for (const auto &argument : request.arguments) expanded.push_back(argument.c_str());
  llvm::BumpPtrAllocator allocator;
  if (auto error = clang::driver::expandResponseFiles(expanded, false, allocator, overlay.get())) {
    result.failure = llvm::toString(std::move(error));
    return result;
  }
  // Read the real driver option table after response-file expansion. Defaults
  // must follow the selected target and preserve explicit runtime choices.
  unsigned missing_index = 0, missing_count = 0;
  auto parsed_arguments = clang::getDriverOptTable().ParseArgs(
      llvm::ArrayRef(expanded).drop_front(), missing_index, missing_count);
  const std::string default_target = llvm::sys::getDefaultTargetTriple();
  const auto selected_triple = request.target_triple.empty()
      ? parsed_arguments.getLastArgValue(clang::options::OPT_target,
                                        default_target)
      : llvm::StringRef(request.target_triple);
  llvm::Triple selected_target(llvm::Triple::normalize(selected_triple));
  auto &arguments = result.driver_arguments;
  arguments = {result.compiler_driver_path};
  // Explicit -std and CRT arguments retain normal Clang driver behavior.
  bool cxx = request.language == BaseLanguage::Cxx || request.language == BaseLanguage::ObjectiveCxx;
  arguments.push_back(cxx ? "-std=c++20" : "-std=c18");
  if (selected_target.isOSWindows()) arguments.push_back("-fms-runtime-lib=dll");
  for (const char *argument : llvm::drop_begin(expanded)) {
    if (argument) arguments.emplace_back(argument);
  }
  arguments.insert(arguments.end(), {"-resource-dir", result.resource_directory,
      "-x", Languages[static_cast<unsigned>(request.language)]});
  if (!request.target_triple.empty()) arguments.push_back("--target=" + request.target_triple);
  if (objc) {
    auto runtime = request.objc_runtime;
    if (runtime.empty() && !parsed_arguments.hasArg(clang::options::OPT_fobjc_runtime_EQ)) {
      if (selected_target.isOSLinux()) runtime = "gnustep-2.2";
      else if (selected_target.isOSWindows()) runtime = "gnustep-2.0";
    }
    auto selected_runtime = runtime.empty()
        ? parsed_arguments.getLastArgValue(clang::options::OPT_fobjc_runtime_EQ)
        : llvm::StringRef(runtime);
    if (request.objc3 && selected_target.isOSLinux() &&
        selected_runtime != "gnustep-2.2") {
      result.failure = "Linux Objective-C 3 requires GNUstep 2.2; rebuild with the current SDK";
      return result;
    }
    if (!runtime.empty()) arguments.push_back("-fobjc-runtime=" + runtime);
  }
  if (objc) arguments.push_back(request.arc ? "-fobjc-arc" : "-fno-objc-arc");
  if (request.objc3) {
    arguments.push_back("-fobjc-version=3");
    arguments.push_back(std::string("-fobjc3-profile=") + Profiles[static_cast<unsigned>(request.profile)]);
    if (request.strict_concurrency) arguments.push_back("-fobjc3-strict-concurrency");
  }
  if (request.emit == EmitKind::Syntax) arguments.push_back("-fsyntax-only");
  else {
    arguments.push_back("-c");
    if (request.emit == EmitKind::LLVM) arguments.insert(arguments.end(), {"-S", "-emit-llvm"});
    arguments.insert(arguments.end(), {"-o", std::string(temporary.path)});
  }
  arguments.push_back(request.input_path);
  Diagnostics diagnostics(result);
  ActionFactory action(request, result);
  clang::tooling::ToolInvocation invocation(arguments, &action, files.get(),
      std::make_shared<clang::PCHContainerOperations>());
  invocation.setDiagnosticConsumer(&diagnostics);
  result.success = invocation.run() && diagnostics.getNumErrors() == 0;
  if (result.success && request.emit_semantic_interface &&
      (!result.interface_artifact || !result.interface_artifact->success)) {
    result.success = false;
    result.failure = "Semantic interface emission failed";
  }
  if (result.success && !result.resolved_module_identities.empty())
    BuildDependencyRecord(request, result);
  if (result.success && std::any_of(result.interface_input_paths.begin(),
          result.interface_input_paths.end(), aliases_record)) {
    result.success = false;
    result.failure = "Dependency record path conflicts with an imported interface artifact";
  }
  if (result.success && !record_output.empty() && result.semantic_dependency_record.empty()) {
    result.success = false;
    result.failure = "No imported semantic dependency record was produced";
  }
  auto publish_products = [&]() {
    if (!interface_output.empty()) {
      auto written = temporary.path.empty()
          ? WriteSemanticInterfaceArtifact(*result.interface_artifact, std::string(interface_output))
          : WriteSemanticInterfaceArtifactWithObject(*result.interface_artifact,
                std::string(interface_output), std::string(temporary.path), std::string(final_output));
      result.interface_artifact->diagnostics.insert(result.interface_artifact->diagnostics.end(),
          written.diagnostics.begin(), written.diagnostics.end());
      if (!written.success) {
        result.success = false;
        result.failure = "Compilation products could not be published";
      }
    } else if (!temporary.path.empty()) {
      auto error = llvm::sys::fs::rename(temporary.path, final_output);
      if (error) { result.success = false; result.failure = error.message(); }
      else temporary.path.clear();
    }
    return result.success;
  };
  if (result.success) {
    if (record_output.empty()) {
      publish_products();
    } else {
      std::string publication_failure;
      if (!PublishCompilationProductsWithDependencyRecord(
              std::string(record_output), result.semantic_dependency_record,
              publish_products, publication_failure)) {
        result.success = false;
        if (result.failure.empty()) result.failure = std::move(publication_failure);
        else result.failure += "; " + publication_failure;
      }
    }
  }
  if (!result.success && result.failure.empty()) result.failure = "Clang compilation failed";
  return result;
}
}  // namespace objc3::clang_foundation
