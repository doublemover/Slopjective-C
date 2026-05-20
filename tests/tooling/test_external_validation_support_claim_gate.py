from __future__ import annotations

import copy
import sys
from pathlib import Path
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object, write_json_file
from objc3c_tooling.paths import repo_rel
from scripts import check_objc3c_external_support_claim_gate as gate


NEGATIVE_FIXTURE_ROOT = (
    ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "claim_gate_negative"
)


def _load_corpus() -> dict[str, Any]:
    return load_json_object(gate.REPRO_CORPUS)


def _load_negative_case(path: Path) -> dict[str, Any]:
    return load_json_object(path)


def _set_dotted(entry: dict[str, Any], dotted_key: str, value: Any) -> None:
    target = entry
    parts = dotted_key.split(".")
    for part in parts[:-1]:
        nested = target.get(part)
        assert isinstance(nested, dict)
        target = nested
    target[parts[-1]] = value


def _apply_negative_case(corpus: dict[str, Any], case: dict[str, Any]) -> dict[str, Any]:
    mutated = copy.deepcopy(corpus)
    entries = mutated["entries"]
    assert isinstance(entries, list)
    by_fixture_id = {
        entry["fixture_id"]: entry
        for entry in entries
        if isinstance(entry, dict) and isinstance(entry.get("fixture_id"), str)
    }
    for mutation in case["mutations"]:
        entry = by_fixture_id[mutation["fixture_id"]]
        for dotted_key, value in mutation["set"].items():
            _set_dotted(entry, dotted_key, value)
    return mutated


def test_external_support_claim_gate_writes_summary(tmp_path: Path) -> None:
    summary_path = (
        ROOT
        / "tmp"
        / "tests"
        / "external-support-claim-gate"
        / tmp_path.name
        / "support-claim-gate-summary.json"
    )

    assert gate.main(["--summary", str(summary_path)]) == 0
    summary = load_json_object(summary_path)

    assert summary["contract_id"] == gate.SUMMARY_CONTRACT_ID
    assert summary["status"] == "PASS"
    assert summary["repro_corpus"] == "tests/tooling/fixtures/external_validation/repro_corpus.json"
    assert summary["claim_gate"] == "tests/tooling/fixtures/external_validation/support_claim_gate.json"
    assert summary["fixture_count"] == 3
    assert summary["claim_binding_count"] == 2
    assert summary["checked_paths"] == sorted(summary["checked_paths"])
    assert "tests/tooling/fixtures/external_validation/claim_gate_negative" in summary["checked_paths"]


@pytest.mark.parametrize(
    "fixture_name",
    [
        "local_only_evidence.json",
        "missing_evidence.json",
        "non_reproducible_evidence.json",
        "stale_evidence.json",
    ],
)
def test_external_support_claim_gate_negative_fixtures_fail_closed(
    tmp_path: Path,
    fixture_name: str,
) -> None:
    case = _load_negative_case(NEGATIVE_FIXTURE_ROOT / fixture_name)
    corpus = _apply_negative_case(_load_corpus(), case)
    output_root = ROOT / "tmp" / "tests" / "external-support-claim-gate" / tmp_path.name
    corpus_path = output_root / fixture_name
    gate_path = output_root / f"{Path(fixture_name).stem}-gate.json"
    gate_payload = load_json_object(gate.CLAIM_GATE)
    gate_payload["repro_corpus"] = repo_rel(corpus_path)
    write_json_file(corpus_path, corpus, sort_keys=True)
    write_json_file(gate_path, gate_payload, sort_keys=True)

    with pytest.raises(gate.ClaimGateFailure) as exc_info:
        gate.validate_external_claim_gate(
            corpus_path=corpus_path,
            gate_path=gate_path,
        )

    assert exc_info.value.diagnostic_code == case["expected_diagnostic_code"]
