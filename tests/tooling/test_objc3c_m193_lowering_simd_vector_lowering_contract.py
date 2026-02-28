from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m193_lowering_runtime_simd_vector_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M193 lowering/runtime SIMD/vector type lowering",
        "tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/",
        "tmp/reports/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/",
        "tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/module.diagnostics.json",
        "tmp/reports/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m193/lowering-runtime-simd-vector-type-lowering/simd-vector-source-anchors.txt",
        "kObjc3SimdVectorLaneContract = \"2,4,8,16\"",
        "kObjc3SimdVectorBaseI32 = \"i32\"",
        "kObjc3SimdVectorBaseBool = \"bool\"",
        "IsSupportedObjc3SimdVectorLaneCount(...)",
        "TryBuildObjc3SimdVectorLLVMType(...)",
        "Objc3SimdVectorTypeLoweringReplayKey()",
        "simd_vector_lowering =",
        "simd_vector_function_signatures =",
        "\"vector_signature_surface\"",
        "\"lowering_vector_abi\"",
        "\"lane_contract\":\"2,4,8,16\"",
        "python -m pytest tests/tooling/test_objc3c_m193_lowering_simd_vector_lowering_contract.py -q",
    ):
        assert text in fragment


def test_m193_lowering_runtime_simd_vector_source_anchor_mapping() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    lowering_contract_header = _read(LOWERING_CONTRACT_HEADER)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    ir_emitter_source = _read(IR_EMITTER_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    mapped_source_anchors = (
        (
            'inline constexpr const char *kObjc3SimdVectorLaneContract = "2,4,8,16";',
            lowering_contract_header,
            'inline constexpr const char *kObjc3SimdVectorLaneContract = "2,4,8,16";',
        ),
        (
            'inline constexpr const char *kObjc3SimdVectorBaseI32 = "i32";',
            lowering_contract_header,
            'inline constexpr const char *kObjc3SimdVectorBaseI32 = "i32";',
        ),
        (
            'inline constexpr const char *kObjc3SimdVectorBaseBool = "bool";',
            lowering_contract_header,
            'inline constexpr const char *kObjc3SimdVectorBaseBool = "bool";',
        ),
        (
            "bool IsSupportedObjc3SimdVectorLaneCount(unsigned lane_count) {",
            lowering_contract_source,
            "bool IsSupportedObjc3SimdVectorLaneCount(unsigned lane_count) {",
        ),
        (
            "bool TryBuildObjc3SimdVectorLLVMType(const std::string &base_spelling, unsigned lane_count, std::string &llvm_type) {",
            lowering_contract_source,
            "bool TryBuildObjc3SimdVectorLLVMType(const std::string &base_spelling, unsigned lane_count, std::string &llvm_type) {",
        ),
        (
            "std::string Objc3SimdVectorTypeLoweringReplayKey() {",
            lowering_contract_source,
            "std::string Objc3SimdVectorTypeLoweringReplayKey() {",
        ),
        (
            "static std::size_t CountVectorSignatureFunctions(const Objc3Program &program) {",
            ir_emitter_source,
            "static std::size_t CountVectorSignatureFunctions(const Objc3Program &program) {",
        ),
        (
            'out << "; simd_vector_lowering = " << Objc3SimdVectorTypeLoweringReplayKey() << "\\n";',
            ir_emitter_source,
            'out << "; simd_vector_lowering = " << Objc3SimdVectorTypeLoweringReplayKey() << "\\n";',
        ),
        (
            'out << "; simd_vector_function_signatures = " << vector_signature_function_count_ << "\\n";',
            ir_emitter_source,
            'out << "; simd_vector_function_signatures = " << vector_signature_function_count_ << "\\n";',
        ),
        (
            'manifest << "      \\"vector_signature_surface\\":{\\"vector_signature_functions\\":" << vector_signature_functions',
            frontend_artifacts_source,
            'manifest << "      \\"vector_signature_surface\\":{\\"vector_signature_functions\\":" << vector_signature_functions',
        ),
        (
            'manifest << "  \\"lowering_vector_abi\\":{\\"replay_key\\":\\"" << Objc3SimdVectorTypeLoweringReplayKey()',
            frontend_artifacts_source,
            'manifest << "  \\"lowering_vector_abi\\":{\\"replay_key\\":\\"" << Objc3SimdVectorTypeLoweringReplayKey()',
        ),
        (
            '<< "\\",\\"lane_contract\\":\\"" << kObjc3SimdVectorLaneContract',
            frontend_artifacts_source,
            '<< "\\",\\"lane_contract\\":\\"" << kObjc3SimdVectorLaneContract',
        ),
    )

    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text
