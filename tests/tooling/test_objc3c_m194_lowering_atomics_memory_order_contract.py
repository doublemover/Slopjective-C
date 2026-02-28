from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m194_lowering_runtime_atomic_memory_order_mapping_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M194 lowering/runtime atomics and memory-order mapping",
        "tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/",
        "tmp/reports/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/",
        "tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/module.diagnostics.json",
        "tmp/reports/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m194/lowering-runtime-atomics-memory-order-mapping/atomic-memory-order-source-anchors.txt",
        "kObjc3AtomicMemoryOrderRelaxed = \"relaxed\"",
        "kObjc3AtomicMemoryOrderAcquire = \"acquire\"",
        "kObjc3AtomicMemoryOrderRelease = \"release\"",
        "kObjc3AtomicMemoryOrderAcqRel = \"acq_rel\"",
        "kObjc3AtomicMemoryOrderSeqCst = \"seq_cst\"",
        "TryParseObjc3AtomicMemoryOrder(...)",
        "Objc3AtomicMemoryOrderToLLVMOrdering(...)",
        "Objc3AtomicMemoryOrderMappingReplayKey()",
        "acquire_release",
        "monotonic",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        '\"lowering\":{\"runtime_dispatch_symbol\":\"<symbol>\",\"runtime_dispatch_arg_slots\":<N>,\"selector_global_ordering\":\"lexicographic\"}',
        "python -m pytest tests/tooling/test_objc3c_m194_lowering_atomics_memory_order_contract.py -q",
    ):
        assert text in fragment


def test_m194_lowering_runtime_atomic_memory_order_mapping_source_anchor_mapping() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    lowering_contract_header = _read(LOWERING_CONTRACT_HEADER)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    ir_emitter_source = _read(IR_EMITTER_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    mapped_source_anchors = (
        (
            'inline constexpr const char *kObjc3AtomicMemoryOrderRelaxed = "relaxed";',
            lowering_contract_header,
            'inline constexpr const char *kObjc3AtomicMemoryOrderRelaxed = "relaxed";',
        ),
        (
            'inline constexpr const char *kObjc3AtomicMemoryOrderAcquire = "acquire";',
            lowering_contract_header,
            'inline constexpr const char *kObjc3AtomicMemoryOrderAcquire = "acquire";',
        ),
        (
            'inline constexpr const char *kObjc3AtomicMemoryOrderRelease = "release";',
            lowering_contract_header,
            'inline constexpr const char *kObjc3AtomicMemoryOrderRelease = "release";',
        ),
        (
            'inline constexpr const char *kObjc3AtomicMemoryOrderAcqRel = "acq_rel";',
            lowering_contract_header,
            'inline constexpr const char *kObjc3AtomicMemoryOrderAcqRel = "acq_rel";',
        ),
        (
            'inline constexpr const char *kObjc3AtomicMemoryOrderSeqCst = "seq_cst";',
            lowering_contract_header,
            'inline constexpr const char *kObjc3AtomicMemoryOrderSeqCst = "seq_cst";',
        ),
        (
            "enum class Objc3AtomicMemoryOrder : std::uint8_t {",
            lowering_contract_header,
            "enum class Objc3AtomicMemoryOrder : std::uint8_t {",
        ),
        (
            "bool TryParseObjc3AtomicMemoryOrder(const std::string &token, Objc3AtomicMemoryOrder &order) {",
            lowering_contract_source,
            "bool TryParseObjc3AtomicMemoryOrder(const std::string &token, Objc3AtomicMemoryOrder &order) {",
        ),
        (
            'if (token == kObjc3AtomicMemoryOrderAcqRel || token == "acquire_release") {',
            lowering_contract_source,
            'if (token == kObjc3AtomicMemoryOrderAcqRel || token == "acquire_release") {',
        ),
        (
            "const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order) {",
            lowering_contract_source,
            "const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order) {",
        ),
        ('return "monotonic";', lowering_contract_source, 'return "monotonic";'),
        ('return "acquire";', lowering_contract_source, 'return "acquire";'),
        ('return "release";', lowering_contract_source, 'return "release";'),
        ('return "acq_rel";', lowering_contract_source, 'return "acq_rel";'),
        ('return "seq_cst";', lowering_contract_source, 'return "seq_cst";'),
        (
            "std::string Objc3AtomicMemoryOrderMappingReplayKey() {",
            lowering_contract_source,
            "std::string Objc3AtomicMemoryOrderMappingReplayKey() {",
        ),
        (
            "std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Relaxed)) + \"=\" +",
            lowering_contract_source,
            "std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Relaxed)) + \"=\" +",
        ),
        (
            'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";',
            ir_emitter_source,
            'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";',
        ),
        (
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
            ir_emitter_source,
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
        ),
        (
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
            frontend_artifacts_source,
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        ),
    )

    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text
