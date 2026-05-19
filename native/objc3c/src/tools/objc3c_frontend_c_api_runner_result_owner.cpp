#include "tools/objc3c_frontend_c_api_runner_result_owner.h"

FrontendCApiCompileResultOwner::~FrontendCApiCompileResultOwner() {
  objc3c_frontend_c_owned_result_destroy(result_);
}

objc3c_frontend_c_result_t **FrontendCApiCompileResultOwner::out_param() {
  if (result_ != nullptr) {
    objc3c_frontend_c_owned_result_destroy(result_);
    result_ = nullptr;
    snapshot_ = {};
  }
  return &result_;
}

bool FrontendCApiCompileResultOwner::valid() const {
  return result_ != nullptr;
}

const objc3c_frontend_c_compile_result_t &FrontendCApiCompileResultOwner::view()
    const {
  RefreshSnapshot();
  return snapshot_;
}

void FrontendCApiCompileResultOwner::RefreshSnapshot() const {
  snapshot_ = {};
  snapshot_.status = objc3c_frontend_c_owned_result_status(result_);
  snapshot_.process_exit_code =
      objc3c_frontend_c_owned_result_process_exit_code(result_);
  snapshot_.success = objc3c_frontend_c_owned_result_success(result_);
  snapshot_.semantic_skipped =
      objc3c_frontend_c_owned_result_semantic_skipped(result_);
  snapshot_.lex = objc3c_frontend_c_owned_result_stage_summary(
      result_,
      OBJC3C_FRONTEND_STAGE_LEX);
  snapshot_.parse = objc3c_frontend_c_owned_result_stage_summary(
      result_,
      OBJC3C_FRONTEND_STAGE_PARSE);
  snapshot_.sema = objc3c_frontend_c_owned_result_stage_summary(
      result_,
      OBJC3C_FRONTEND_STAGE_SEMA);
  snapshot_.lower = objc3c_frontend_c_owned_result_stage_summary(
      result_,
      OBJC3C_FRONTEND_STAGE_LOWER);
  snapshot_.emit = objc3c_frontend_c_owned_result_stage_summary(
      result_,
      OBJC3C_FRONTEND_STAGE_EMIT);
  snapshot_.error_message =
      const_cast<objc3c_frontend_c_string_t *>(
          objc3c_frontend_c_owned_result_error_message(result_));
  snapshot_.diagnostics_path =
      const_cast<objc3c_frontend_c_string_t *>(
          objc3c_frontend_c_owned_result_artifact_path(
              result_,
              OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS));
  snapshot_.manifest_path =
      const_cast<objc3c_frontend_c_string_t *>(
          objc3c_frontend_c_owned_result_artifact_path(
              result_,
              OBJC3C_FRONTEND_ARTIFACT_MANIFEST));
  snapshot_.runtime_metadata_path =
      const_cast<objc3c_frontend_c_string_t *>(
          objc3c_frontend_c_owned_result_artifact_path(
              result_,
              OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA));
  snapshot_.ir_path =
      const_cast<objc3c_frontend_c_string_t *>(
          objc3c_frontend_c_owned_result_artifact_path(
              result_,
              OBJC3C_FRONTEND_ARTIFACT_IR));
  snapshot_.object_path =
      const_cast<objc3c_frontend_c_string_t *>(
          objc3c_frontend_c_owned_result_artifact_path(
              result_,
              OBJC3C_FRONTEND_ARTIFACT_OBJECT));
}
