from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_b003_type_system_objc3_forms_core_feature_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_b003_type_system_objc3_forms_core_feature_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m227-type-system-objc3-forms-core-feature-b003-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 15
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_scaffold_apply_call_drops_from_integration_builder(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_semantic_passes.cpp"
    drift_source.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
        ).read_text(encoding="utf-8").replace(
            "  ApplyTypeFormScaffoldSummaryToIdClassSelObjectPointerTypeCheckingSummary(summary);\n",
            "",
            2,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["semantic_passes"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B003-SRC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_summary_drops_canonical_ready_field(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_contract = tmp_path / "objc3_sema_contract.h"
    drift_contract.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
        ).read_text(encoding="utf-8").replace(
            "  bool canonical_type_form_scaffold_ready = false;\n",
            "",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["sema_contract"] = drift_contract
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B003-CNT-08" for failure in payload["failures"])
