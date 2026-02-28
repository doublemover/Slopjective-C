from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SEMA_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
SEMA_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMA_PASSES_HEADER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
SEMA_PASSES_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMA_PASS_MANAGER_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
SEMA_PASS_MANAGER_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m193_sema_simd_vector_section_marker_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M193 sema/type SIMD/vector type lowering",
        "simd vector packet 1.1 deterministic sema/type vector architecture anchors",
        "m193_sema_type_simd_vector_architecture_packet",
        "simd vector packet 1.2 deterministic sema/type vector isolation anchors",
        "m193_sema_type_simd_vector_isolation_packet",
        "struct Objc3VectorTypeLoweringSummary {",
        "MakeSemanticTypeFromParam(...)",
        "MakeSemanticTypeFromFunctionReturn(...)",
        "MakeSemanticTypeFromFunctionInfoParam(...)",
        "IsSameSemanticType(...)",
        "SemanticTypeName(...)",
        "RecordVectorTypeLoweringAnnotation(...)",
        "BuildVectorTypeLoweringSummary(...)",
        "result.vector_type_lowering = BuildVectorTypeLoweringSummary(result.integration_surface);",
        "result.deterministic_vector_type_lowering = result.vector_type_lowering.deterministic;",
        "result.parity_surface.vector_type_lowering = result.vector_type_lowering;",
        "result.parity_surface.deterministic_vector_type_lowering = result.deterministic_vector_type_lowering;",
        "deterministic_vector_type_lowering",
        "vector_type_lowering_total",
        "vector_return_annotations",
        "vector_param_annotations",
        "vector_i32_annotations",
        "vector_bool_annotations",
        "vector_lane2_annotations",
        "vector_lane4_annotations",
        "vector_lane8_annotations",
        "vector_lane16_annotations",
        "vector_unsupported_annotations",
        "python -m pytest tests/tooling/test_objc3c_m193_sema_simd_vector_lowering_contract.py -q",
    ):
        assert text in fragment


def test_m193_sema_simd_vector_source_anchor_mapping() -> None:
    sema_contract_source = _read(SEMA_CONTRACT_SOURCE)
    sema_passes_header = _read(SEMA_PASSES_HEADER)
    sema_passes_source = _read(SEMA_PASSES_SOURCE)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    for marker in (
        "struct Objc3VectorTypeLoweringSummary {",
        "std::size_t total() const { return return_annotations + param_annotations; }",
        "std::vector<bool> param_is_vector;",
        "std::vector<std::string> param_vector_base_spelling;",
        "std::vector<unsigned> param_vector_lane_count;",
        "bool return_is_vector = false;",
        "unsigned return_vector_lane_count = 1;",
    ):
        assert marker in sema_contract_source

    assert "Objc3VectorTypeLoweringSummary BuildVectorTypeLoweringSummary(const Objc3SemanticIntegrationSurface &surface);" in sema_passes_header

    for marker in (
        "struct SemanticTypeInfo {",
        "static SemanticTypeInfo MakeSemanticTypeFromParam(const FuncParam &param) {",
        "static SemanticTypeInfo MakeSemanticTypeFromFunctionReturn(const FunctionDecl &fn) {",
        "static SemanticTypeInfo MakeSemanticTypeFromFunctionInfoParam(const FunctionInfo &fn, std::size_t index) {",
        "static bool IsSameSemanticType(const SemanticTypeInfo &lhs, const SemanticTypeInfo &rhs) {",
        "static std::string SemanticTypeName(const SemanticTypeInfo &info) {",
        "static void RecordVectorTypeLoweringAnnotation(ValueType base_type, unsigned lane_count, bool is_return,",
        "Objc3VectorTypeLoweringSummary BuildVectorTypeLoweringSummary(const Objc3SemanticIntegrationSurface &surface) {",
        "existing.return_is_vector == fn.return_vector_spelling",
        "metadata.param_is_vector = source.param_is_vector;",
        "metadata.param_vector_base_spelling = source.param_vector_base_spelling;",
        "metadata.param_vector_lane_count = source.param_vector_lane_count;",
        "metadata.return_is_vector = source.return_is_vector;",
        "metadata.return_vector_base_spelling = source.return_vector_base_spelling;",
        "metadata.return_vector_lane_count = source.return_vector_lane_count;",
        "metadata.param_is_vector.size() == metadata.arity",
        "metadata.param_vector_base_spelling.size() == metadata.arity",
        "metadata.param_vector_lane_count.size() == metadata.arity",
    ):
        assert marker in sema_passes_source

    for marker in (
        "Objc3VectorTypeLoweringSummary vector_type_lowering;",
        "bool deterministic_vector_type_lowering = false;",
        "surface.deterministic_vector_type_lowering &&",
        "surface.vector_type_lowering.deterministic &&",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.vector_type_lowering = BuildVectorTypeLoweringSummary(result.integration_surface);",
        "result.deterministic_vector_type_lowering = result.vector_type_lowering.deterministic;",
        "result.parity_surface.vector_type_lowering = result.vector_type_lowering;",
        "result.parity_surface.deterministic_vector_type_lowering = result.deterministic_vector_type_lowering;",
        "result.parity_surface.deterministic_vector_type_lowering &&",
        "result.parity_surface.vector_type_lowering.deterministic &&",
    ):
        assert marker in sema_pass_manager_source

    for marker in (
        '\\"deterministic_vector_type_lowering\\":',
        '\\"vector_type_lowering_total\\":',
        '\\"vector_return_annotations\\":',
        '\\"vector_param_annotations\\":',
        '\\"vector_i32_annotations\\":',
        '\\"vector_bool_annotations\\":',
        '\\"vector_lane2_annotations\\":',
        '\\"vector_lane4_annotations\\":',
        '\\"vector_lane8_annotations\\":',
        '\\"vector_lane16_annotations\\":',
        '\\"vector_unsupported_annotations\\":',
    ):
        assert marker in frontend_artifacts_source
