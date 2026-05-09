#include "tools/objc3c_frontend_c_api_runner_invocation.h"

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
  compile_options_.input_path = input_path_text_.c_str();
  compile_options_.out_dir = out_dir_text_.c_str();
  compile_options_.emit_prefix = runner_options_.emit_prefix.c_str();
  compile_options_.clang_path =
      runner_options_.emit_object &&
              runner_options_.ir_object_backend ==
                  OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG
          ? clang_path_text_.c_str()
          : nullptr;
  compile_options_.llc_path =
      runner_options_.emit_object &&
              runner_options_.ir_object_backend ==
                  OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
          ? llc_path_text_.c_str()
          : nullptr;
  compile_options_.runtime_dispatch_symbol =
      runner_options_.runtime_dispatch_symbol.empty()
          ? nullptr
          : runner_options_.runtime_dispatch_symbol.c_str();
  compile_options_.max_message_send_args =
      runner_options_.max_message_send_args;
  compile_options_.translation_unit_registration_order_ordinal =
      runner_options_.translation_unit_registration_order_ordinal;
  compile_options_.emit_manifest = runner_options_.emit_manifest ? 1u : 0u;
  compile_options_.emit_ir = runner_options_.emit_ir ? 1u : 0u;
  compile_options_.emit_object = runner_options_.emit_object ? 1u : 0u;
  compile_options_.ir_object_backend = runner_options_.ir_object_backend;
}
