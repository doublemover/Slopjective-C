        } else if (!toolchain_runtime_core_feature_ready) {
          result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
          result->process_exit_code = 125;
          result->success = 0;
          const std::string emit_error =
              "error:1:1: LLVM object emission failed: toolchain/runtime core feature fail-closed: " +
              toolchain_runtime_core_feature_reason + " [O3E002]";
          emit_diagnostics.push_back(emit_error);
          objc3c_frontend_set_error(context, emit_error.c_str());
        } else {
          objc3c::frontend::SetFrontendContextObjectPath(
              context, object_out.generic_string());
        }
      }
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_DIAGNOSTICS) {
    if (!product.artifact_bundle.diagnostics.empty()) {
      objc3c_frontend_set_error(context, product.artifact_bundle.diagnostics.front().c_str());
    } else {
      objc3c_frontend_set_error(context, "compilation reported diagnostics.");
    }
  } else if (result->status == OBJC3C_FRONTEND_STATUS_OK) {
    objc3c_frontend_set_error(context, "");
  }

  const bool wants_emit_stage = artifact_plan.wants_emit_stage;
  const bool emit_attempted = lower_attempted && wants_emit_stage;
  const bool emit_skipped = !emit_attempted;
  result->semantic_skipped = product.pipeline_result.integration_surface.built ? 0u : 1u;

  result->lex = objc3c::frontend::BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_LEX,
      true,
      false,
      product.pipeline_result.stage_diagnostics.lexer);
  result->parse =
      objc3c::frontend::BuildFrontendStageSummary(
          OBJC3C_FRONTEND_STAGE_PARSE,
          true,
          false,
          product.pipeline_result.stage_diagnostics.parser);
  result->sema = objc3c::frontend::BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_SEMA,
      sema_attempted,
      !sema_attempted,
      product.pipeline_result.stage_diagnostics.semantic);
  result->lower = objc3c::frontend::BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_LOWER, lower_attempted, !lower_attempted, {});
  result->emit = objc3c::frontend::BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_EMIT, emit_attempted, emit_skipped, emit_diagnostics);

  std::string result_ownership_error;
  if (!PopulateResultPaths(context, result, result_ownership_error)) {
    result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
    result->process_exit_code = 2;
    result->success = 0;
    objc3c_frontend_set_error(context, result_ownership_error.c_str());
  }
  return result->status;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_file(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  if (result == nullptr) {
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  if (context == nullptr) {
    return SetUsageErrorWithoutContext(result, "compile_file requires a frontend context.");
  }
  if (options == nullptr) {
    return SetUsageError(context, result, "compile_file requires compile options.");
  }
  std::lock_guard<std::mutex> lock(context->mutex);
  std::string language_version_error;
  if (!objc3c::frontend::ValidateSupportedFrontendLanguageVersion(
          options->language_version, language_version_error)) {
    return SetUsageError(context, result, language_version_error);
  }
  std::string compile_options_error;
  if (!objc3c::frontend::ValidateFrontendCompileFileOptions(
          *options, compile_options_error)) {
    return SetUsageError(context, result, compile_options_error);
  }

  std::string source_text;
  std::string io_error;
  const std::filesystem::path input_path =
      objc3c::frontend::BorrowedFrontendPathToFilesystemPath(
          options->input_path);
  if (!objc3c::support::TryReadTextFile(input_path,
                                        source_text,
                                        io_error,
                                        "failed to open input source '" + input_path.string() + "'",
                                        "failed while reading input source '" + input_path.string() + "'")) {
    return SetUsageError(context, result, io_error);
  }
  return CompileObjc3SourceImpl(context, input_path, source_text, options, result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_source(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  if (result == nullptr) {
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  if (context == nullptr) {
    return SetUsageErrorWithoutContext(result, "compile_source requires a frontend context.");
  }
  if (options == nullptr) {
    return SetUsageError(context, result, "compile_source requires compile options.");
  }
  std::lock_guard<std::mutex> lock(context->mutex);
  std::string language_version_error;
  if (!objc3c::frontend::ValidateSupportedFrontendLanguageVersion(
          options->language_version, language_version_error)) {
    return SetUsageError(context, result, language_version_error);
  }
  std::string compile_options_error;
  if (!objc3c::frontend::ValidateFrontendCompileSourceOptions(
          *options, compile_options_error)) {
    return SetUsageError(context, result, compile_options_error);
  }
  const std::filesystem::path input_path =
      objc3c::frontend::ResolveFrontendInputPath(*options);
  return CompileObjc3SourceImpl(context, input_path, std::string(options->source_text), options, result);
}

