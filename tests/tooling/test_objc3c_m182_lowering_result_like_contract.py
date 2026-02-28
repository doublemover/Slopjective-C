from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LOWERING_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "40-lowering.md"
LOWERING_CONTRACT_HEADER = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m182_lowering_result_like_section_exists() -> None:
    fragment = _read(LOWERING_DOC_FRAGMENT)
    for text in (
        "## Result-like lowering artifact contract (M182-C001)",
        "kObjc3ResultLikeLoweringLaneContract",
        "Objc3ResultLikeLoweringContract",
        "IsValidObjc3ResultLikeLoweringContract(...)",
        "Objc3ResultLikeLoweringReplayKey(...)",
        "deterministic_result_like_lowering_handoff",
        "result_like_lowering = result_like_sites=<N>",
        "lane_contract=m182-result-like-lowering-v1",
        "normalized_sites + branch_merge_sites == result_like_sites",
        "python -m pytest tests/tooling/test_objc3c_m182_lowering_result_like_contract.py -q",
    ):
        assert text in fragment


def test_m182_lowering_result_like_markers_map_to_sources() -> None:
    header = _read(LOWERING_CONTRACT_HEADER)
    source = _read(LOWERING_CONTRACT_SOURCE)

    for marker in (
        "kObjc3ResultLikeLoweringLaneContract",
        "struct Objc3ResultLikeLoweringContract",
        "std::size_t result_like_sites = 0;",
        "std::size_t result_success_sites = 0;",
        "std::size_t result_failure_sites = 0;",
        "std::size_t result_branch_sites = 0;",
        "std::size_t result_payload_sites = 0;",
        "std::size_t normalized_sites = 0;",
        "std::size_t branch_merge_sites = 0;",
        "std::size_t contract_violation_sites = 0;",
        "bool IsValidObjc3ResultLikeLoweringContract(",
        "std::string Objc3ResultLikeLoweringReplayKey(",
    ):
        assert marker in header

    for marker in (
        "IsValidObjc3ResultLikeLoweringContract(",
        "Objc3ResultLikeLoweringReplayKey(",
        "contract.normalized_sites + contract.branch_merge_sites !=",
        "contract.contract_violation_sites > 0 && contract.deterministic",
        '"result_like_sites="',
        '";result_success_sites="',
        '";result_failure_sites="',
        '";result_branch_sites="',
        '";result_payload_sites="',
        '";normalized_sites="',
        '";branch_merge_sites="',
        '";contract_violation_sites="',
        "kObjc3ResultLikeLoweringLaneContract",
    ):
        assert marker in source
