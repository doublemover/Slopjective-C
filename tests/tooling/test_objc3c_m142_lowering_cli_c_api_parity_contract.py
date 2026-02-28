from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m142_lowering_cli_c_api_parity_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Frontend lowering parity harness artifacts (M142-E001)",
        "Lane-C lowering/LLVM IR/runtime-ABI parity anchors (`M142-C001`):",
        "lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "runtime_dispatch_decl = declare i32 @<symbol>(i32, ptr[, i32...])",
        "simd_vector_lowering = <canonical replay key>",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        '"lowering_vector_abi":{"replay_key":"<canonical replay key>"}',
        "python -m pytest tests/tooling/test_objc3c_m142_lowering_cli_c_api_parity_contract.py -q",
    ):
        assert text in fragment


def test_m142_lowering_cli_c_api_parity_markers_map_to_sources() -> None:
    ir_emitter_source = _read(IR_EMITTER_SOURCE)
    lowering_contract_header = _read(LOWERING_CONTRACT_HEADER)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    for marker in (
        "TryBuildObjc3LoweringIRBoundary(lowering_contract, lowering_ir_boundary_, boundary_error_)",
        'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";',
        'out << "; runtime_dispatch_decl = " << Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_) << "\\n";',
        'out << "; simd_vector_lowering = " << Objc3SimdVectorTypeLoweringReplayKey() << "\\n";',
        'out << Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_) << "\\n\\n";',
    ):
        assert marker in ir_emitter_source

    for marker in (
        'inline constexpr const char *kObjc3SimdVectorLaneContract = "2,4,8,16";',
        "std::string Objc3SimdVectorTypeLoweringReplayKey();",
        "std::string Objc3SimdVectorTypeLoweringReplayKey() {",
    ):
        assert marker in lowering_contract_header or marker in lowering_contract_source

    for marker in (
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        'manifest << "  \\"lowering_vector_abi\\":{\\"replay_key\\":\\"" << Objc3SimdVectorTypeLoweringReplayKey()',
        '<< "\\",\\"lane_contract\\":\\"" << kObjc3SimdVectorLaneContract',
    ):
        assert marker in frontend_artifacts_source
