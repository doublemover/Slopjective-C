#include "tools/objc3c_frontend_c_api_runner_invocation.h"

#include "tools/objc3c_frontend_c_api_runner_compile_options_fields.h"

FrontendCApiContextOwner::FrontendCApiContextOwner()
    : context_(objc3c_frontend_c_context_create()) {}

FrontendCApiContextOwner::~FrontendCApiContextOwner() {
  objc3c_frontend_c_context_destroy(context_);
}

bool FrontendCApiContextOwner::valid() const {
  return context_ != nullptr;
}

objc3c_frontend_c_context_t *FrontendCApiContextOwner::get() const {
  return context_;
}

FrontendCApiRunnerCompileInvocation::FrontendCApiRunnerCompileInvocation(
    const FrontendCApiRunnerOptions &options)
    : input_path_text_(options.input_path.string()),
      out_dir_text_(options.out_dir.string()),
      clang_path_text_(options.clang_path.string()),
      llc_path_text_(options.llc_path.string()),
      runner_options_(options) {
  RefreshBorrowedPointers();
}

const objc3c_frontend_c_compile_options_t *
FrontendCApiRunnerCompileInvocation::compile_options() const {
  return &compile_options_;
}

void FrontendCApiRunnerCompileInvocation::RefreshBorrowedPointers() {
  compile_options_ = {};
  ApplyFrontendCApiRunnerCompilePathInputOptions(
      compile_options_, runner_options_, input_path_text_, out_dir_text_);
  ApplyFrontendCApiRunnerCompileBackendToolchainOptions(
      compile_options_, runner_options_, clang_path_text_, llc_path_text_);
  ApplyFrontendCApiRunnerCompileRuntimeOptions(compile_options_,
                                              runner_options_);
  ApplyFrontendCApiRunnerCompileEmissionOptions(compile_options_,
                                               runner_options_);
}
