#include "libobjc3c_frontend/public/c_api_owned_result.h"

#include <new>

#include "libobjc3c_frontend/c_api.h"
#include "libobjc3c_frontend/internal/c_api_owned_result_storage.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"

namespace objc3c::frontend::c_api {

objc3c_frontend_c_compile_result_t *MutableOwnedResultStorage(
    objc3c_frontend_c_result_t *result) {
  return result == nullptr ? nullptr : &result->storage;
}

const objc3c_frontend_c_compile_result_t *BorrowOwnedResultStorage(
    const objc3c_frontend_c_result_t *result) {
  return result == nullptr ? nullptr : &result->storage;
}

}  // namespace objc3c::frontend::c_api

namespace {

using FrontendOwnedCompileEntrypoint = objc3c_frontend_c_status_t (*)(
    objc3c_frontend_c_context_t *,
    const objc3c_frontend_c_compile_options_t *,
    objc3c_frontend_c_compile_result_t *);

objc3c_frontend_c_status_t CompileOwnedFrontendResult(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_result_t **out_result,
    FrontendOwnedCompileEntrypoint compile) {
  if (out_result == nullptr) {
    objc3c::frontend::SetFrontendContextError(
        context, "owned C API compile requires an output result handle.");
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  *out_result = nullptr;

  objc3c_frontend_c_result_t *result =
      new (std::nothrow) objc3c_frontend_c_result_t();
  if (result == nullptr) {
    objc3c::frontend::SetFrontendContextError(
        context, "owned C API compile could not allocate result handle.");
    return OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  }

  const objc3c_frontend_c_status_t status =
      compile(context, options,
              objc3c::frontend::c_api::MutableOwnedResultStorage(result));
  *out_result = result;
  return status;
}

}  // namespace

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_file_owned(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_result_t **out_result) {
  return CompileOwnedFrontendResult(context, options, out_result,
                                    objc3c_frontend_c_compile_file);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_source_owned(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_result_t **out_result) {
  return CompileOwnedFrontendResult(context, options, out_result,
                                    objc3c_frontend_c_compile_source);
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_owned_result_destroy(
    objc3c_frontend_c_result_t *result) {
  if (result == nullptr) {
    return;
  }
  objc3c_frontend_c_result_destroy(
      objc3c::frontend::c_api::MutableOwnedResultStorage(result));
  delete result;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_owned_result_status(
    const objc3c_frontend_c_result_t *result) {
  const objc3c_frontend_c_compile_result_t *storage =
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result);
  return storage == nullptr ? OBJC3C_FRONTEND_STATUS_USAGE_ERROR
                            : storage->status;
}

extern "C" OBJC3C_FRONTEND_API int32_t
objc3c_frontend_c_owned_result_process_exit_code(
    const objc3c_frontend_c_result_t *result) {
  const objc3c_frontend_c_compile_result_t *storage =
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result);
  return storage == nullptr ? 2 : storage->process_exit_code;
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_owned_result_success(
    const objc3c_frontend_c_result_t *result) {
  const objc3c_frontend_c_compile_result_t *storage =
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result);
  return storage == nullptr ? 0u : storage->success;
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_owned_result_semantic_skipped(
    const objc3c_frontend_c_result_t *result) {
  const objc3c_frontend_c_compile_result_t *storage =
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result);
  return storage == nullptr ? 0u : storage->semantic_skipped;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_stage_summary_t
objc3c_frontend_c_owned_result_stage_summary(
    const objc3c_frontend_c_result_t *result,
    objc3c_frontend_c_stage_id_t stage) {
  const objc3c_frontend_c_compile_result_t *storage =
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result);
  if (storage == nullptr) {
    return {};
  }

  switch (stage) {
    case OBJC3C_FRONTEND_STAGE_LEX:
      return storage->lex;
    case OBJC3C_FRONTEND_STAGE_PARSE:
      return storage->parse;
    case OBJC3C_FRONTEND_STAGE_SEMA:
      return storage->sema;
    case OBJC3C_FRONTEND_STAGE_LOWER:
      return storage->lower;
    case OBJC3C_FRONTEND_STAGE_EMIT:
      return storage->emit;
  }
  return {};
}

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_owned_result_error_message(
    const objc3c_frontend_c_result_t *result) {
  return objc3c_frontend_c_result_error_message(
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result));
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_owned_result_error_message_view(
    const objc3c_frontend_c_result_t *result) {
  return objc3c_frontend_c_result_error_message_view(
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result));
}

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_owned_result_artifact_path(
    const objc3c_frontend_c_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_artifact_path(
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result),
      artifact_kind);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_owned_result_artifact_path_view(
    const objc3c_frontend_c_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_artifact_path_view(
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result),
      artifact_kind);
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_owned_result_has_artifact(
    const objc3c_frontend_c_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_has_artifact(
      objc3c::frontend::c_api::BorrowOwnedResultStorage(result),
      artifact_kind);
}
