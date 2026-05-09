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


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _without_line_comments(text: str) -> str:
    return "\n".join(line for line in text.splitlines() if not line.lstrip().startswith("//"))


def test_native_owner_modules_are_registered_in_cmake_without_include_shards() -> None:
    for cmake_path, owner_sources in OWNER_CMAKE_CONTRACTS.items():
        cmake = _read(cmake_path)
        for source in owner_sources:
            assert source in cmake
        assert ".inc" not in cmake


def test_native_include_shards_remain_private_to_owner_aggregators() -> None:
    include_shards = sorted(SRC_ROOT.glob("**/*.inc"))
    assert include_shards

    included_by_owner = set()
    for aggregator_path, prefix in INCLUDE_SHARD_AGGREGATORS.items():
        aggregator = _read(aggregator_path)
        for shard_path in include_shards:
            if not shard_path.name.startswith(prefix):
                continue
            relative_include = shard_path.relative_to(SRC_ROOT).as_posix()
            assert f'#include "{relative_include}"' in aggregator
            included_by_owner.add(shard_path)

    assert set(include_shards) == included_by_owner

    for header_path in SRC_ROOT.glob("**/*.h"):
        assert ".inc" not in _read(header_path), header_path.relative_to(ROOT).as_posix()


def test_ir_emitter_static_contract_uses_split_context_and_active_artifact_api() -> None:
    ir_source = _read(SRC_ROOT / "ir" / "objc3_ir_emitter.cpp")
    ir_header = _read(SRC_ROOT / "ir" / "objc3_ir_emitter.h")
    artifacts_source = _without_line_comments(
        _read(SRC_ROOT / "artifacts" / "objc3_frontend_artifacts.cpp")
    )

    assert '#include "ir/objc3_ir_emitter_context.h"' in ir_source
    assert "struct FunctionContext" not in ir_source
    assert "struct LoweredMessageSend" not in ir_header
    assert "EmitObjc3IRText(pipeline_result.program.ast, options.lowering" in artifacts_source
    assert "EmitObjc3IRText(pipeline_result.program, options.lowering" not in artifacts_source
