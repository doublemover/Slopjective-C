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
          context->object_path = object_out.generic_string();
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

  const bool wants_emit_stage = options->emit_ir != 0 || options->emit_object != 0;
  const bool emit_attempted = lower_attempted && wants_emit_stage;
  const bool emit_skipped = !emit_attempted;
  result->semantic_skipped = product.pipeline_result.integration_surface.built ? 0u : 1u;

  result->lex = BuildStageSummary(OBJC3C_FRONTEND_STAGE_LEX, true, false, product.pipeline_result.stage_diagnostics.lexer);
  result->parse =
      BuildStageSummary(OBJC3C_FRONTEND_STAGE_PARSE, true, false, product.pipeline_result.stage_diagnostics.parser);
  result->sema = BuildStageSummary(OBJC3C_FRONTEND_STAGE_SEMA,
                                   sema_attempted,
                                   !sema_attempted,
                                   product.pipeline_result.stage_diagnostics.semantic);
  result->lower = BuildStageSummary(OBJC3C_FRONTEND_STAGE_LOWER, lower_attempted, !lower_attempted, {});
  result->emit = BuildStageSummary(OBJC3C_FRONTEND_STAGE_EMIT, emit_attempted, emit_skipped, emit_diagnostics);

  PopulateResultPaths(context, result);
  return result->status;
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_is_abi_compatible(uint32_t requested_abi_version) {
  return requested_abi_version >= OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION &&
                 requested_abi_version <= OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION
             ? 1u
             : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint32_t objc3c_frontend_abi_version(void) {
  return OBJC3C_FRONTEND_ABI_VERSION;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_version_t objc3c_frontend_version(void) {
  const objc3c_frontend_version_t version = OBJC3C_FRONTEND_VERSION_INIT;
  return version;
}

extern "C" OBJC3C_FRONTEND_API const char *objc3c_frontend_version_string(void) {
  return OBJC3C_FRONTEND_VERSION_STRING;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_context_t *objc3c_frontend_context_create(void) {
  objc3c_frontend_context_t *context = new (std::nothrow) objc3c_frontend_context_t();
  if (context != nullptr) {
    context->last_error.clear();
  }
  return context;
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_context_destroy(objc3c_frontend_context_t *context) {
  delete context;
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
  if (!ValidateSupportedLanguageVersion(options->language_version, language_version_error)) {
    return SetUsageError(context, result, language_version_error);
  }
  if (IsMissingBorrowedPath(options->input_path)) {
    return SetUsageError(context, result, "compile_file requires compile_options.input_path.");
  }

  std::string source_text;
  std::string io_error;
  const std::filesystem::path input_path = BorrowedPathToFilesystemPath(options->input_path);
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
  if (!ValidateSupportedLanguageVersion(options->language_version, language_version_error)) {
    return SetUsageError(context, result, language_version_error);
  }
  if (IsMissingBorrowedText(options->source_text)) {
    return SetUsageError(context, result, "compile_source requires compile_options.source_text.");
  }
  const std::filesystem::path input_path = ResolveInputPath(*options);
  return CompileObjc3SourceImpl(context, input_path, std::string(options->source_text), options, result);
}

extern "C" OBJC3C_FRONTEND_API size_t objc3c_frontend_copy_last_error(
    const objc3c_frontend_context_t *context,
    char *buffer,
    size_t buffer_size) {
  std::unique_lock<std::mutex> lock;
  if (context != nullptr) {
    lock = std::unique_lock<std::mutex>(context->mutex);
  }
  const std::string message = context == nullptr ? "" : context->last_error;
  const size_t required = message.size() + 1;

  if (buffer == nullptr || buffer_size == 0) {
    return required;
  }

  const size_t bytes_to_copy = std::min(required - 1, buffer_size - 1);
  if (bytes_to_copy > 0) {
    std::memcpy(buffer, message.data(), bytes_to_copy);
  }
  buffer[bytes_to_copy] = '\0';
  return required;
}

