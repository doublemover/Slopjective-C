#include "tools/objc3c_frontend_c_api_runner_stage_selection.h"

std::string LastAttemptedFrontendCApiStageName(
    const objc3c_frontend_c_compile_result_t &result) {
  if (result.emit.attempted != 0) {
    return "emit";
  }
  if (result.lower.attempted != 0) {
    return "lower";
  }
  if (result.sema.attempted != 0) {
    return "sema";
  }
  if (result.parse.attempted != 0) {
    return "parse";
  }
  if (result.lex.attempted != 0) {
    return "lex";
  }
  return "";
}

std::string BlockingFrontendCApiStageName(
    const objc3c_frontend_c_compile_result_t &result) {
  if (result.lex.diagnostics_errors != 0 ||
      result.lex.diagnostics_fatals != 0) {
    return "lex";
  }
  if (result.parse.diagnostics_errors != 0 ||
      result.parse.diagnostics_fatals != 0) {
    return "parse";
  }
  if (result.sema.diagnostics_errors != 0 ||
      result.sema.diagnostics_fatals != 0) {
    return "sema";
  }
  if (result.lower.diagnostics_errors != 0 ||
      result.lower.diagnostics_fatals != 0) {
    return "lower";
  }
  if (result.emit.diagnostics_errors != 0 ||
      result.emit.diagnostics_fatals != 0 || result.process_exit_code != 0) {
    return "emit";
  }
  return LastAttemptedFrontendCApiStageName(result);
}
