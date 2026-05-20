from __future__ import annotations

import copy
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.stress_source_surface_check.surface_validators import validate_claim_gate

FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "stress"


def _load_json(path: Path) -> dict[str, object]:
    return json.loads(path.read_text(encoding="utf-8"))


def _fixture_payloads() -> tuple[dict[str, object], dict[str, object], dict[str, object], dict[str, object]]:
    return (
        _load_json(FIXTURE_ROOT / "claim_gate.json"),
        _load_json(FIXTURE_ROOT / "source_surface.json"),
        _load_json(FIXTURE_ROOT / "artifact_surface.json"),
        _load_json(FIXTURE_ROOT / "workflow_surface.json"),
    )


def test_stress_claim_gate_catalogs_supported_and_partial_claims() -> None:
    claim_gate, surface, artifact_surface, workflow_surface = _fixture_payloads()

    summaries = validate_claim_gate(
        claim_gate,
        surface=surface,
        artifact_surface=artifact_surface,
        workflow_surface=workflow_surface,
    )

    assert [summary["claim_id"] for summary in summaries] == [
        "deterministic-malformed-input-fuzz",
        "bounded-lowering-runtime-and-differential-stress",
    ]
    assert {summary["status"] for summary in summaries} == {"supported", "partial"}
    assert all(summary["evidence_report_count"] > 0 for summary in summaries)
    assert all(summary["durable_input_count"] > 0 for summary in summaries)


def test_stress_claim_gate_rejects_tmp_source_of_truth() -> None:
    claim_gate, surface, artifact_surface, workflow_surface = _fixture_payloads()
    mutated = copy.deepcopy(claim_gate)
    claims = mutated["claims"]
    assert isinstance(claims, list)
    first_claim = claims[0]
    assert isinstance(first_claim, dict)
    first_claim["durable_inputs"] = ["tmp/reports/stress/source-surface-summary.json"]

    with pytest.raises(RuntimeError, match="uses tmp as source of truth"):
        validate_claim_gate(
            mutated,
            surface=surface,
            artifact_surface=artifact_surface,
            workflow_surface=workflow_surface,
        )


def test_stress_claim_gate_rejects_non_workflow_evidence_report() -> None:
    claim_gate, surface, artifact_surface, workflow_surface = _fixture_payloads()
    mutated = copy.deepcopy(claim_gate)
    claims = mutated["claims"]
    assert isinstance(claims, list)
    first_claim = claims[0]
    assert isinstance(first_claim, dict)
    first_claim["evidence_reports"] = ["tmp/reports/stress/private-only-summary.json"]

    with pytest.raises(RuntimeError, match="evidence report is not workflow-required"):
        validate_claim_gate(
            mutated,
            surface=surface,
            artifact_surface=artifact_surface,
            workflow_surface=workflow_surface,
        )
