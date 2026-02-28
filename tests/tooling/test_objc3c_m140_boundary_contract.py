from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
CLI_FRONTEND_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.h"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_CPP = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_CPP = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_frontend_library_entrypoints_route_through_extracted_compile_product() -> None:
    header = _read(CLI_FRONTEND_HEADER)
    anchor = _read(FRONTEND_ANCHOR)

    assert "struct Objc3FrontendCompileProduct" in header
    assert "Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(" in header
    assert "CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options)" in anchor
    assert "BuildDiagnosticsJson" in anchor


def test_sema_pass_manager_exposes_deterministic_type_metadata_handoff() -> None:
    contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    source = _read(SEMA_PASS_MANAGER_CPP)

    assert "Objc3SemanticTypeMetadataHandoff type_metadata_handoff;" in contract
    assert "bool deterministic_type_metadata_handoff = false;" in contract
    assert "IsMonotonicObjc3SemaDiagnosticsAfterPass(" in contract

    assert "result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);" in source
    assert "result.deterministic_type_metadata_handoff =" in source


def test_lowering_to_ir_boundary_replay_contract_is_explicit() -> None:
    contract_h = _read(LOWERING_CONTRACT_HEADER)
    contract_cpp = _read(LOWERING_CONTRACT_CPP)
    emitter = _read(IR_EMITTER_CPP)

    assert "struct Objc3LoweringIRBoundary" in contract_h
    assert "Objc3LoweringIRBoundaryReplayKey(" in contract_h

    assert "TryBuildObjc3LoweringIRBoundary(" in contract_cpp
    assert "Objc3LoweringIRBoundaryReplayKey(" in contract_cpp

    assert "TryBuildObjc3LoweringIRBoundary(lowering_contract, lowering_ir_boundary_, boundary_error_)" in emitter
    assert "ValidateMessageSendArityContract(" in emitter
    assert "; lowering_ir_boundary = " in emitter
