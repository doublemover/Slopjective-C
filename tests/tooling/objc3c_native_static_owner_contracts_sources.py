from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC_ROOT = ROOT / "native" / "objc3c" / "src"

OWNER_CMAKE_CONTRACTS = {
    SRC_ROOT / "parse" / "CMakeLists.txt": (
        "objc3_parser_core.cpp",
        "objc3_parser_declaration_surface.cpp",
        "objc3_parser_expression_surface.cpp",
        "objc3_parser_statement_surface.cpp",
    ),
    SRC_ROOT / "sema" / "CMakeLists.txt": (
        "objc3_sema_pass_manager.cpp",
        "objc3_semantic_type_factory.cpp",
        "objc3_semantic_type_relations.cpp",
        "objc3_semantic_type_predicates.cpp",
        "objc3_static_analysis.cpp",
        "objc3_pure_contract.cpp",
    ),
    SRC_ROOT / "ir" / "CMakeLists.txt": (
        "objc3_ir_message_send_lowering.cpp",
        "objc3_ir_message_send_validation.cpp",
        "objc3_ir_module_emission_surface.cpp",
        "objc3_ir_runtime_dispatch_calls.cpp",
        "objc3_ir_runtime_dispatch_declarations.cpp",
        "objc3_ir_type_model.cpp",
        "objc3_ir_emitter.cpp",
    ),
    SRC_ROOT / "driver" / "CMakeLists.txt": (
        "objc3_driver_command_dispatch.cpp",
        "objc3_driver_command_result.cpp",
        "objc3_driver_conformance_validation_paths.cpp",
        "objc3_driver_public_workflow_commands.cpp",
        "objc3_driver_shell.cpp",
        "objc3_compilation_driver.cpp",
    ),
    SRC_ROOT / "tools" / "CMakeLists.txt": (
        "objc3c_frontend_c_api_runner_compile_session.cpp",
        "objc3c_frontend_c_api_runner_output_contract.cpp",
        "objc3c_frontend_c_api_runner_output_paths.cpp",
        "objc3c_frontend_c_api_runner_public_result.cpp",
        "objc3c_frontend_c_api_runner_session.cpp",
    ),
}

INCLUDE_SHARD_AGGREGATORS = {
    SRC_ROOT / "parse" / "objc3_parser_core.cpp": "objc3_parser_core_",
    SRC_ROOT / "sema" / "objc3_semantic_passes.cpp": "objc3_semantic_passes_",
    SRC_ROOT / "sema" / "objc3_sema_pass_manager.cpp": "objc3_sema_pass_manager_",
}


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def without_line_comments(text: str) -> str:
    return "\n".join(
        line for line in text.splitlines() if not line.lstrip().startswith("//")
    )


def owner_cmake_contracts() -> dict[Path, tuple[str, ...]]:
    return OWNER_CMAKE_CONTRACTS


def include_shards() -> list[Path]:
    return sorted(SRC_ROOT.glob("**/*.inc"))


def include_shard_aggregators() -> dict[Path, str]:
    return INCLUDE_SHARD_AGGREGATORS


def header_paths() -> list[Path]:
    return list(SRC_ROOT.glob("**/*.h"))


def repo_relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def ir_emitter_contract_texts() -> tuple[str, str, str]:
    return (
        read_text(SRC_ROOT / "ir" / "objc3_ir_emitter.cpp"),
        read_text(SRC_ROOT / "ir" / "objc3_ir_emitter.h"),
        without_line_comments(
            read_text(SRC_ROOT / "artifacts" / "objc3_frontend_artifacts.cpp")
        ),
    )


def runtime_public_result_texts() -> tuple[str, str, str, str, str]:
    return (
        read_text(SRC_ROOT / "runtime" / "CMakeLists.txt"),
        read_text(SRC_ROOT / "runtime" / "public" / "objc3_runtime_result.h"),
        read_text(
            SRC_ROOT
            / "runtime"
            / "public"
            / "objc3_runtime_registration_status.h"
        ),
        read_text(
            SRC_ROOT / "runtime" / "public" / "objc3_runtime_dispatch_status.h"
        ),
        read_text(
            SRC_ROOT / "runtime" / "public" / "objc3_runtime_dispatch_result.h"
        ),
    )
