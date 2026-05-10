from __future__ import annotations

from objc3c_frontend_library_entrypoint_extraction_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_in_order,
)
from objc3c_frontend_library_entrypoint_extraction_sources import (
    cli_frontend_texts,
    frontend_anchor_source,
    public_api_header_texts,
)


def assert_frontend_anchor_compile_entrypoints_are_pipeline_backed() -> None:
    source = frontend_anchor_source()

    assert_contains_all(
        source,
        [
            '#include "libobjc3c_frontend/objc3_cli_frontend.h"',
            '#include "io/objc3_process.h"',
            "CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options)",
            "OBJC3C_FRONTEND_STATUS_DIAGNOSTICS",
            "OBJC3C_FRONTEND_STATUS_EMIT_ERROR",
            "OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT",
            "RunIRCompileLLVMDirect(",
            "NormalizeLanguageVersion(options.language_version)",
            (
                "ValidateSupportedLanguageVersion("
                "options->language_version, language_version_error)"
            ),
            "unsupported compile_options.language_version:",
            "Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);",
            (
                "frontend_options.language_profile = "
                "Objc3FrontendLanguageProfile::kCanonical;"
            ),
            (
                "Objc3FrontendCompileProduct product = "
                "CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);"
            ),
            (
                "std::vector<std::string> emit_diagnostics = "
                "product.artifact_bundle.post_pipeline_diagnostics;"
            ),
            "product.pipeline_result.stage_diagnostics",
        ],
    )
    assert_excludes_all(
        source,
        [
            "libobjc3c_frontend compile entrypoints are scaffolded only",
            "NormalizeCompatibilityMode(options.compatibility_mode)",
            (
                "ValidateSupportedCompatibilityMode("
                "options->compatibility_mode, compatibility_mode_error)"
            ),
            "unsupported compile_options.compatibility_mode:",
            "OBJC3C_FRONTEND_COMPATIBILITY_MODE_LEGACY",
        ],
    )

    assert_in_order(
        source,
        [
            "result->lex = BuildStageSummary(",
            "result->parse =",
            "result->sema = BuildStageSummary(",
            "result->lower = BuildStageSummary(",
            "result->emit = BuildStageSummary(",
        ],
    )

    assert_in_order(
        source,
        [
            (
                "if (!ValidateSupportedLanguageVersion("
                "options->language_version, language_version_error)) {"
            ),
            "return SetUsageError(context, result, language_version_error);",
            "if (IsMissingBorrowedPath(options->input_path)) {",
        ],
    )


def assert_cli_frontend_exports_reusable_pipeline_compile_product() -> None:
    header, source = cli_frontend_texts()

    assert_contains_all(
        header,
        [
            "struct Objc3FrontendCompileProduct",
            "Objc3FrontendPipelineResult pipeline_result;",
            "Objc3FrontendArtifactBundle artifact_bundle;",
            "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(",
        ],
    )
    assert_contains_all(
        source,
        [
            "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(",
            "product.pipeline_result = RunObjc3FrontendPipeline(source, options);",
            (
                "product.artifact_bundle = "
                "BuildObjc3FrontendArtifacts(input_path, product.pipeline_result, options);"
            ),
            (
                "Objc3FrontendCompileProduct product = "
                "CompileObjc3SourceWithPipeline(input_path, source, options);"
            ),
            "return std::move(product.artifact_bundle);",
        ],
    )


def assert_public_api_documents_pipeline_backed_compile_behavior() -> None:
    frontend_header, options_header = public_api_header_texts()

    assert_contains_all(
        frontend_header,
        [
            "Pipeline-backed behavior:",
            "Runs lexer/parser/sema/lower/emit through the extracted frontend pipeline.",
        ],
    )
    assert_excludes_all(frontend_header, ["Current implementation status: scaffolded"])
    assert_contains_all(
        options_header,
        [
            "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3 3u",
            (
                "#define OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT "
                "OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3"
            ),
            "objc3c_frontend_borrowed_path_t input_path;",
            "objc3c_frontend_borrowed_text_t source_text;",
            "uint8_t language_version;",
        ],
    )
    assert_excludes_all(options_header, ["const char *input_path;"])
