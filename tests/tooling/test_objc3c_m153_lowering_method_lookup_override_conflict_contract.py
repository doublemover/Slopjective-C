from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m153_lowering_method_lookup_override_conflict_section_exists() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## Method lookup/override/conflict lowering artifact contract (M153-C001)",
        "kObjc3MethodLookupOverrideConflictLaneContract",
        "Objc3MethodLookupOverrideConflictContract",
        "lane_contract=m153-method-lookup-override-conflict-v1",
        "python -m pytest tests/tooling/test_objc3c_m153_lowering_method_lookup_override_conflict_contract.py -q",
    ):
        assert text in fragment


def test_m153_lowering_method_lookup_override_conflict_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)

    for marker in (
        "kObjc3MethodLookupOverrideConflictLaneContract",
        "struct Objc3MethodLookupOverrideConflictContract",
        "std::size_t method_lookup_sites = 0;",
        "std::size_t method_lookup_hits = 0;",
        "std::size_t method_lookup_misses = 0;",
        "std::size_t override_lookup_sites = 0;",
        "std::size_t override_lookup_hits = 0;",
        "std::size_t override_lookup_misses = 0;",
        "std::size_t override_conflicts = 0;",
        "std::size_t unresolved_base_interfaces = 0;",
        "bool IsValidObjc3MethodLookupOverrideConflictContract(",
        "std::string Objc3MethodLookupOverrideConflictReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3MethodLookupOverrideConflictContract(",
        "Objc3MethodLookupOverrideConflictReplayKey(",
        '"method_lookup_sites="',
        '";override_conflicts="',
        '";unresolved_base_interfaces="',
        '";lane_contract=" + kObjc3MethodLookupOverrideConflictLaneContract',
    ):
        assert marker in source
