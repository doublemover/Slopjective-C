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


def test_m194_sema_atomics_memory_order_section_marker_exists() -> None:
    fragment = _read(SEMA_DOC_FRAGMENT)
    for text in (
        "## M194 sema/type atomics and memory-order mapping",
        "atomics packet 1.1 deterministic sema/type memory-order architecture anchors",
        "m194_sema_type_atomic_memory_order_architecture_packet",
        "atomics packet 1.2 deterministic sema/type memory-order isolation anchors",
        "m194_sema_type_atomic_memory_order_isolation_packet",
        "enum class Objc3SemaAtomicMemoryOrder : std::uint8_t {",
        "struct Objc3AtomicMemoryOrderMappingSummary {",
        "MapAssignmentOperatorToAtomicMemoryOrder(...)",
        "FormatAtomicMemoryOrderMappingHint(...)",
        "BuildAtomicMemoryOrderMappingSummary(...)",
        "result.atomic_memory_order_mapping = BuildAtomicMemoryOrderMappingSummary(*input.program);",
        "result.deterministic_atomic_memory_order_mapping = result.atomic_memory_order_mapping.deterministic;",
        "result.parity_surface.atomic_memory_order_mapping = result.atomic_memory_order_mapping;",
        "result.parity_surface.deterministic_atomic_memory_order_mapping = result.deterministic_atomic_memory_order_mapping;",
        "result.parity_surface.ready =",
        "deterministic_atomic_memory_order_mapping",
        "atomic_memory_order_mapping_total",
        "atomic_relaxed_ops",
        "atomic_acquire_ops",
        "atomic_release_ops",
        "atomic_acq_rel_ops",
        "atomic_seq_cst_ops",
        "atomic_unmapped_ops",
        "python -m pytest tests/tooling/test_objc3c_m194_sema_atomics_memory_order_contract.py -q",
    ):
        assert text in fragment


def test_m194_sema_atomics_memory_order_source_anchor_mapping() -> None:
    sema_contract_source = _read(SEMA_CONTRACT_SOURCE)
    sema_passes_header = _read(SEMA_PASSES_HEADER)
    sema_passes_source = _read(SEMA_PASSES_SOURCE)
    sema_pass_manager_contract = _read(SEMA_PASS_MANAGER_CONTRACT)
    sema_pass_manager_source = _read(SEMA_PASS_MANAGER_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    for marker in (
        "enum class Objc3SemaAtomicMemoryOrder : std::uint8_t {",
        "struct Objc3AtomicMemoryOrderMappingSummary {",
        "std::size_t total() const { return relaxed + acquire + release + acq_rel + seq_cst + unsupported; }",
    ):
        assert marker in sema_contract_source

    assert "Objc3AtomicMemoryOrderMappingSummary BuildAtomicMemoryOrderMappingSummary(const Objc3ParsedProgram &program);" in sema_passes_header

    for marker in (
        "static Objc3SemaAtomicMemoryOrder MapAssignmentOperatorToAtomicMemoryOrder(const std::string &op) {",
        "static std::string FormatAtomicMemoryOrderMappingHint(const std::string &op) {",
        "static void CollectAtomicMemoryOrderMappingsInStatement(const Stmt *stmt,",
        "Objc3AtomicMemoryOrderMappingSummary BuildAtomicMemoryOrderMappingSummary(const Objc3ParsedProgram &program) {",
    ):
        assert marker in sema_passes_source

    for marker in (
        "Objc3AtomicMemoryOrderMappingSummary atomic_memory_order_mapping;",
        "bool deterministic_atomic_memory_order_mapping = false;",
        "surface.deterministic_type_metadata_handoff && surface.deterministic_atomic_memory_order_mapping &&",
    ):
        assert marker in sema_pass_manager_contract

    for marker in (
        "result.atomic_memory_order_mapping = BuildAtomicMemoryOrderMappingSummary(*input.program);",
        "result.deterministic_atomic_memory_order_mapping = result.atomic_memory_order_mapping.deterministic;",
        "result.parity_surface.atomic_memory_order_mapping = result.atomic_memory_order_mapping;",
        "result.parity_surface.deterministic_atomic_memory_order_mapping = result.deterministic_atomic_memory_order_mapping;",
    ):
        assert marker in sema_pass_manager_source

    for marker in (
        '\\"deterministic_atomic_memory_order_mapping\\":',
        '\\"atomic_memory_order_mapping_total\\":',
        '\\"atomic_relaxed_ops\\":',
        '\\"atomic_acquire_ops\\":',
        '\\"atomic_release_ops\\":',
        '\\"atomic_acq_rel_ops\\":',
        '\\"atomic_seq_cst_ops\\":',
        '\\"atomic_unmapped_ops\\":',
    ):
        assert marker in frontend_artifacts_source
