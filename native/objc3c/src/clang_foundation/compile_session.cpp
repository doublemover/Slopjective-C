#include "clang_foundation/compile_session.h"

#include <mutex>
#include "clang_foundation/ast_contract_consumer.h"
#include "clang_foundation/interface_artifact_io.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Version.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Driver.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/MultiplexConsumer.h"
#include "clang/Options/Options.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Option/ArgList.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"

#ifndef OBJC3C_FOUNDATION_DRIVER
#error "The verified patched Clang driver path must be supplied by the build"
#endif
#ifndef OBJC3C_FOUNDATION_RESOURCE_DIR
#error "The matching Clang resource directory must be supplied by the build"
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

template <typename NativeAction>
class ContractAction final : public NativeAction {
 public:
  ContractAction(const CompileRequest &request, CompileResult &result)
      : request_(request), result_(result) {}
 protected:
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance &compiler, llvm::StringRef input) override {
    auto native = NativeAction::CreateASTConsumer(compiler, input);
    if (!native) return nullptr;
    std::vector<std::unique_ptr<clang::ASTConsumer>> consumers;
    // BackendConsumer may clear the AST in HandleTranslationUnit before
    // emitting the native module. Observe that same AST while it is alive.
    consumers.push_back(MakeContractConsumer(request_, result_));
    consumers.push_back(std::move(native));
    return std::make_unique<clang::MultiplexConsumer>(std::move(consumers));
  }
 private:
  const CompileRequest &request_;
  CompileResult &result_;
};

class ActionFactory final : public clang::tooling::FrontendActionFactory {
 public:
  ActionFactory(const CompileRequest &request, CompileResult &result)
      : request_(request), result_(result) {}
  std::unique_ptr<clang::FrontendAction> create() override {
    if (request_.emit == EmitKind::Object)
      return std::make_unique<ContractAction<clang::EmitObjAction>>(request_, result_);
    if (request_.emit == EmitKind::LLVM)
      return std::make_unique<ContractAction<clang::EmitLLVMAction>>(request_, result_);
    return std::make_unique<ContractAction<clang::SyntaxOnlyAction>>(request_, result_);
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
    result_.target_triple = invocation->getTargetOpts().Triple;
    result_.runtime_abi = language.ObjC ? language.ObjCRuntime.getAsString() : "no-objc-runtime";
    return FrontendActionFactory::runInvocation(std::move(invocation), files,
                                               std::move(containers), diagnostics);
  }
 private:
  const CompileRequest &request_;
  CompileResult &result_;
};

struct TemporaryOutput {
  llvm::SmallString<256> path;
  ~TemporaryOutput() { if (!path.empty()) llvm::sys::fs::remove(path); }
};
}  // namespace

CompileResult Compile(const CompileRequest &request) {
  CompileResult result;
  result.compiler_identity = clang::getClangFullRepositoryVersion();
  if (result.compiler_identity.find("ObjC3-patches=" OBJC3C_FOUNDATION_PATCH_DIGEST) ==
      std::string::npos) {
    result.failure = "Linked Clang libraries differ from the sealed compiler SDK";
    return result;
  }
  const bool objc = request.language == BaseLanguage::ObjectiveC ||
                    request.language == BaseLanguage::ObjectiveCxx;
  if (request.input_path.empty() ||
      ((request.objc3 || request.arc || !request.objc_runtime.empty()) && !objc) ||
      (!request.objc3 && request.profile != SemanticProfile::Compatibility) ||
      (request.strict_concurrency && !request.objc3) ||
      (request.emit_semantic_interface &&
       (request.module_name.empty() || request.package_identity.empty())) ||
      (!request.interface_output_path.empty() && !request.emit_semantic_interface) ||
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
  if ((!final_output.empty() && overlay->makeAbsolute(final_output)) ||
      (!interface_output.empty() && overlay->makeAbsolute(interface_output))) {
    result.failure = "Cannot resolve output paths in the compilation working directory";
    return result;
  }
  if (request.emit != EmitKind::Syntax) {
    auto error = llvm::sys::fs::createUniqueFile(std::string(final_output) + ".objc3c-%%%%%%",
                                               temporary.path);
    if (error) { result.failure = error.message(); return result; }
  }
  // Driver response expansion expects argv[0], including for empty user args.
  llvm::SmallVector<const char *, 32> expanded{OBJC3C_FOUNDATION_DRIVER};
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
  arguments = {OBJC3C_FOUNDATION_DRIVER};
  // Explicit -std and CRT arguments retain normal Clang driver behavior.
  bool cxx = request.language == BaseLanguage::Cxx || request.language == BaseLanguage::ObjectiveCxx;
  arguments.push_back(cxx ? "-std=c++20" : "-std=c18");
  if (selected_target.isOSWindows()) arguments.push_back("-fms-runtime-lib=dll");
  for (const char *argument : llvm::drop_begin(expanded)) {
    if (argument) arguments.emplace_back(argument);
  }
  arguments.insert(arguments.end(), {"-resource-dir", OBJC3C_FOUNDATION_RESOURCE_DIR,
      "-x", Languages[static_cast<unsigned>(request.language)]});
  if (!request.target_triple.empty()) arguments.push_back("--target=" + request.target_triple);
  if (objc) {
    auto runtime = request.objc_runtime;
    if (runtime.empty() && !parsed_arguments.hasArg(clang::options::OPT_fobjc_runtime_EQ) &&
        (selected_target.isOSWindows() || selected_target.isOSLinux()))
      runtime = "gnustep-2.0";
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
  if (result.success && !interface_output.empty()) {
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
  } else if (result.success && !temporary.path.empty()) {
    auto error = llvm::sys::fs::rename(temporary.path, final_output);
    if (error) { result.success = false; result.failure = error.message(); }
    else temporary.path.clear();
  }
  if (!result.success && result.failure.empty()) result.failure = "Clang compilation failed";
  return result;
}
}  // namespace objc3::clang_foundation
