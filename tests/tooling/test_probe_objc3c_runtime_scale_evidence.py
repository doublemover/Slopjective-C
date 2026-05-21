from __future__ import annotations

import copy
import importlib.util
import json
import sys
from pathlib import Path

import pytest


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

SCRIPT_PATH = ROOT / "scripts" / "probe_objc3c_runtime_scale_evidence.py"
SPEC = importlib.util.spec_from_file_location("probe_objc3c_runtime_scale_evidence", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
probe = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = probe
SPEC.loader.exec_module(probe)


def _checked_in_summary() -> dict[str, object]:
    return probe.build_scale_evidence_summary(
        workload_manifest=probe.load_json(probe.WORKLOAD_MANIFEST),
        replay_contract=probe.load_json(probe.REPLAY_CONTRACT),
        metadata_resilience_contract=probe.load_json(probe.METADATA_RESILIENCE_CONTRACT),
        stress_sanitizer_contract=probe.load_json(probe.STRESS_SANITIZER_CONTRACT),
        budget_model=probe.load_json(probe.BUDGET_MODEL),
        parser_sema_fuzz_manifest=probe.load_json(probe.PARSER_SEMA_FUZZ_MANIFEST),
        lowering_runtime_stress_manifest=probe.load_json(probe.LOWERING_RUNTIME_STRESS_MANIFEST),
    )


def test_runtime_scale_evidence_summary_is_deterministic_for_checked_in_contracts() -> None:
    first = _checked_in_summary()
    second = _checked_in_summary()

    assert first == second
    assert first["status"] == "PASS"
    assert first["support_authority"] is False
    assert first["summary_counts"] == {
        "stress_scale": 8,
        "sanitizer": 4,
        "metadata_fuzz": 1,
        "parser_sema_fuzz_cases": 7,
        "lowering_runtime_compile_cases": 15,
        "lowering_runtime_execution_cases": 3,
        "lowering_runtime_semantic_provenance_cases": 2,
    }

    rows = first["evidence_rows"]
    assert isinstance(rows, list)
    assert len(rows) == 13
    probe_ids = [row["deterministic_probe_id"] for row in rows if isinstance(row, dict)]
    assert len(probe_ids) == len(set(probe_ids))
    assert all(row["support_authority"] is False for row in rows if isinstance(row, dict))
    dispatch_row = next(
        row
        for row in rows
        if isinstance(row, dict) and row.get("stress_id") == "dispatch-cache-replay-scale"
    )
    assert dispatch_row["budget_metric_id"] == "dispatch_wall_clock_ms"
    assert str(dispatch_row["fixture_sha256"]).startswith("sha256:")
    assert str(dispatch_row["probe_sha256"]).startswith("sha256:")


def test_runtime_scale_evidence_rejects_support_authority_overclaim() -> None:
    stress_contract = copy.deepcopy(probe.load_json(probe.STRESS_SANITIZER_CONTRACT))
    stress_contract["stress_scale_contracts"][0]["support_authority"] = True

    summary = probe.build_scale_evidence_summary(
        workload_manifest=probe.load_json(probe.WORKLOAD_MANIFEST),
        replay_contract=probe.load_json(probe.REPLAY_CONTRACT),
        metadata_resilience_contract=probe.load_json(probe.METADATA_RESILIENCE_CONTRACT),
        stress_sanitizer_contract=stress_contract,
        budget_model=probe.load_json(probe.BUDGET_MODEL),
        parser_sema_fuzz_manifest=probe.load_json(probe.PARSER_SEMA_FUZZ_MANIFEST),
        lowering_runtime_stress_manifest=probe.load_json(probe.LOWERING_RUNTIME_STRESS_MANIFEST),
    )

    assert summary["status"] == "FAIL"
    assert any("must be provenance-only" in failure for failure in summary["failures"])


def test_runtime_scale_evidence_rejects_missing_budget_link() -> None:
    budget_model = copy.deepcopy(probe.load_json(probe.BUDGET_MODEL))
    linkage = budget_model["runtime_contract_linkage"]
    linkage["workload_budget_links"] = [
        row for row in linkage["workload_budget_links"] if row["workload_id"] != "dispatch-cache"
    ]

    summary = probe.build_scale_evidence_summary(
        workload_manifest=probe.load_json(probe.WORKLOAD_MANIFEST),
        replay_contract=probe.load_json(probe.REPLAY_CONTRACT),
        metadata_resilience_contract=probe.load_json(probe.METADATA_RESILIENCE_CONTRACT),
        stress_sanitizer_contract=probe.load_json(probe.STRESS_SANITIZER_CONTRACT),
        budget_model=budget_model,
        parser_sema_fuzz_manifest=probe.load_json(probe.PARSER_SEMA_FUZZ_MANIFEST),
        lowering_runtime_stress_manifest=probe.load_json(probe.LOWERING_RUNTIME_STRESS_MANIFEST),
    )

    assert summary["status"] == "FAIL"
    assert any("dispatch-cache missing budget metric link" in failure for failure in summary["failures"])


def test_runtime_scale_evidence_cli_writes_tmp_report(tmp_path: Path) -> None:
    summary_out = ROOT / "tmp" / "reports" / "runtime-performance" / "pytest-scale-evidence-summary.json"
    if summary_out.exists():
        summary_out.unlink()

    assert probe.main(["--summary-out", str(summary_out)]) == 0

    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c.runtime.performance.scale.evidence.summary.v1"
    assert payload["status"] == "PASS"
    assert payload["summary_counts"]["stress_scale"] == 8
