import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

COVERAGE_MAP = ROOT / "tests" / "conformance" / "COVERAGE_MAP.md"
LOWERING_ABI_MANIFEST = ROOT / "tests" / "conformance" / "lowering_abi" / "manifest.json"
LOWERING_ABI_README = ROOT / "tests" / "conformance" / "lowering_abi" / "README.md"
FIXTURE = ROOT / "tests" / "conformance" / "lowering_abi" / "M181-D001.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m181_conformance_mapping_is_documented() -> None:
    coverage_map = _read(COVERAGE_MAP)
    readme = _read(LOWERING_ABI_README)

    assert "| `M181-D001` | `#4497` throws propagation lowering replay contract | `lowering_abi` |" in coverage_map
    assert "M181-D001.json" in readme


def test_m181_conformance_fixture_is_registered() -> None:
    manifest = json.loads(_read(LOWERING_ABI_MANIFEST))
    groups = manifest["groups"]

    group = next(
        item
        for item in groups
        if item["name"] == "m181_lane_d_issue_4497_throws_propagation_lowering_replay_contract"
    )
    assert group["issue"] == 4497
    assert group["files"] == ["M181-D001.json"]


def test_m181_conformance_fixture_fields_are_canonical() -> None:
    fixture = json.loads(_read(FIXTURE))

    assert fixture["id"] == "M181-D001"
    assert fixture["bucket"] == "lowering_abi"
    assert fixture["profile"] == "core"
    assert fixture["tracking"]["milestone"] == "M181"
    assert fixture["tracking"]["milestone_number"] == 266
    assert fixture["tracking"]["lane"] == "D"
    assert fixture["tracking"]["issue"] == 4497
    assert fixture["tracking"]["task"] == "M181-D001"
    assert fixture["tracking"]["sequence"] == 1
