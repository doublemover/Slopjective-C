#include "libobjc3c_frontend/c_api.h"

#include <type_traits>

static_assert(OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u, "unexpected c api wrapper abi version");
static_assert(std::is_same_v<objc3c_frontend_c_context_t, objc3c_frontend_context_t>,
              "context alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,
              "compile options alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_compile_result_t, objc3c_frontend_compile_result_t>,
              "compile result alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_artifact_kind_t, objc3c_frontend_artifact_kind_t>,
              "artifact kind alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_string_t, objc3c_frontend_string_t>,
              "string alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_string_view_t, objc3c_frontend_string_view_t>,
              "string view alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_stage_summary_t, objc3c_frontend_stage_summary_t>,
              "stage summary alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_version_t, objc3c_frontend_version_t>,
              "version alias mismatch");

extern "C" OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_api_abi_version(void) {
  return OBJC3C_FRONTEND_C_API_ABI_VERSION;
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_is_abi_compatible(uint32_t requested_abi_version) {
  return objc3c_frontend_is_abi_compatible(requested_abi_version);
}

extern "C" OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_abi_version(void) {
  return objc3c_frontend_abi_version();
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_version_t objc3c_frontend_c_version(void) {
  return objc3c_frontend_version();
}

extern "C" OBJC3C_FRONTEND_API const char *objc3c_frontend_c_version_string(void) {
  return objc3c_frontend_version_string();
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_context_t *objc3c_frontend_c_context_create(void) {
  return objc3c_frontend_context_create();
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_context_destroy(objc3c_frontend_c_context_t *context) {
  objc3c_frontend_context_destroy(context);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_compile_file(context, options, result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_compile_source(context, options, result);
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_result_destroy(
    objc3c_frontend_c_compile_result_t *result) {
  objc3c_frontend_result_destroy(result);
}

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_artifact_path(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_result_artifact_path(result, artifact_kind);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_artifact_path_view(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_string_view(
      objc3c_frontend_c_result_artifact_path(result, artifact_kind));
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_result_has_artifact(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  const objc3c_frontend_c_string_view_t view =
      objc3c_frontend_c_result_artifact_path_view(result, artifact_kind);
  return view.data != nullptr && view.size != 0u ? 1u : 0u;
}

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_error_message(
    const objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_result_error_message(result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_error_message_view(
    const objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_c_string_view(
      objc3c_frontend_c_result_error_message(result));
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t objc3c_frontend_c_string_view(
    const objc3c_frontend_c_string_t *string) {
  return objc3c_frontend_string_view(string);
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_string_release(
    objc3c_frontend_c_string_t *string) {
  objc3c_frontend_string_release(string);
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_is_well_formed(
    const objc3c_frontend_c_stage_summary_t *summary,
    objc3c_frontend_c_stage_id_t expected_stage) {
  switch (expected_stage) {
    case OBJC3C_FRONTEND_STAGE_LEX:
    case OBJC3C_FRONTEND_STAGE_PARSE:
    case OBJC3C_FRONTEND_STAGE_SEMA:
    case OBJC3C_FRONTEND_STAGE_LOWER:
    case OBJC3C_FRONTEND_STAGE_EMIT:
      break;
    default:
      return 0u;
  }
  if (summary == nullptr || summary->stage != expected_stage) {
    return 0u;
  }
  if (summary->attempted > 1u || summary->skipped > 1u ||
      summary->reserved != 0u) {
    return 0u;
  }
  if (summary->attempted != 0u && summary->skipped != 0u) {
    return 0u;
  }
  const uint64_t severity_total =
      static_cast<uint64_t>(summary->diagnostics_notes) +
      static_cast<uint64_t>(summary->diagnostics_warnings) +
      static_cast<uint64_t>(summary->diagnostics_errors) +
      static_cast<uint64_t>(summary->diagnostics_fatals);
  return severity_total == summary->diagnostics_total ? 1u : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_has_diagnostics(
    const objc3c_frontend_c_stage_summary_t *summary) {
  return summary != nullptr && summary->diagnostics_total != 0u ? 1u : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_has_errors(
    const objc3c_frontend_c_stage_summary_t *summary) {
  return summary != nullptr &&
                 (summary->diagnostics_errors != 0u ||
                  summary->diagnostics_fatals != 0u)
             ? 1u
             : 0u;
}

extern "C" OBJC3C_FRONTEND_API size_t objc3c_frontend_c_copy_last_error(
    const objc3c_frontend_c_context_t *context,
    char *buffer,
    size_t buffer_size) {
  return objc3c_frontend_copy_last_error(context, buffer, buffer_size);
}
