from __future__ import annotations

import importlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_long_horizon_operations_evidence.evidence_loading import LongHorizonEvidenceInputs
from objc3c_long_horizon_operations_evidence.model import build_long_horizon_model
from objc3c_long_horizon_operations_evidence.paths import (
    ARTIFACT_CONTRACT_ID,
    BLOCKER_METADATA,
    LongHorizonEvidencePaths,
    OWNER_CONTRACTS,
    SUMMARY_CONTRACT_ID,
)
from objc3c_long_horizon_operations_evidence.rendering import render_console_lines


OWNER_MODULES = (
    "objc3c_long_horizon_operations_evidence.paths",
    "objc3c_long_horizon_operations_evidence.evidence_loading",
    "objc3c_long_horizon_operations_evidence.validation",
    "objc3c_long_horizon_operations_evidence.model",
    "objc3c_long_horizon_operations_evidence.rendering",
    "objc3c_long_horizon_operations_evidence.publication",
)


def test_long_horizon_owner_modules_are_explicit() -> None:
    for module_name in OWNER_MODULES:
        assert importlib.import_module(module_name)


def test_long_horizon_owner_contracts_are_source_owned() -> None:
    assert set(OWNER_CONTRACTS) == {
        "deprecation_owner",
        "revert_owner",
        "cadence_owner",
        "publication_owner",
    }
    assert set(BLOCKER_METADATA) == {
        "deprecation_support_policy",
        "revert_readiness",
        "aging_release_cadence",
        "metadata_publication",
    }
    for contract in OWNER_CONTRACTS.values():
        assert contract["source_contract"].startswith(("tests/tooling/fixtures/long_horizon_operations/", "scripts/"))
        assert contract["blocker_projection"].startswith("claim_audit.blocker_metadata.")


def test_long_horizon_entrypoint_delegates_to_owner_modules() -> None:
    script_text = (ROOT / "scripts" / "build_objc3c_long_horizon_operations_evidence.py").read_text(encoding="utf-8")
    assert "objc3c_long_horizon_operations_evidence.cli" in script_text
    assert "run_timed" not in script_text
    assert "write_json_file" not in script_text
    assert "load_json" not in script_text


def test_long_horizon_model_preserves_public_contract(tmp_path: Path) -> None:
    paths = LongHorizonEvidencePaths.for_root(tmp_path)
    inputs = LongHorizonEvidenceInputs(
        steps=[{"name": "boundary-inventory", "exit_code": 0}],
        reports={
            "conversion": {"conversion_replay_requirements": ["replay candidate over stable"]},
            "aging": {"publication_freshness_metric_count": 4},
        },
        update_manifest={
            "current_version": "3.0.0",
            "supported_major_line": 3,
            "default_channel": "stable",
            "supported_platform_ids": ["win-x64"],
            "channels": [
                {"channel_id": "stable", "version": "3.0.0"},
                {"channel_id": "candidate", "version": "3.0.1-rc.1"},
            ],
        },
        upgrade_support_report={
            "revert_guidance": [{"channel_id": "stable"}],
        },
        failures=[],
    )

    model = build_long_horizon_model(paths, inputs)

    assert model.artifact["contract_id"] == ARTIFACT_CONTRACT_ID
    assert model.summary["contract_id"] == SUMMARY_CONTRACT_ID
    assert model.artifact["owner_contracts"] == OWNER_CONTRACTS
    assert model.artifact["claim_audit"]["blocker_metadata"] == BLOCKER_METADATA
    assert model.artifact["support_window"]["current_version"] == "3.0.0"
    assert model.artifact["upgrade_replay"]["target_version"] == "3.0.1-rc.1"
    assert model.artifact["upgrade_replay"]["requirements"] == ["replay candidate over stable"]
    assert model.artifact["revert_readiness"]["channels"] == ["stable"]
    assert model.artifact["aging_regression"]["freshness_budget"]["publication_freshness_metric_count"] == 4
    assert model.summary["runner_path"] == "scripts/build_objc3c_long_horizon_operations_evidence.py"
    assert model.summary["owner_contract_count"] == 4
    assert model.summary["blocker_metadata_count"] == 4
    assert model.summary["revert_channel_count"] == 1


def test_long_horizon_rendering_preserves_console_contract(tmp_path: Path) -> None:
    paths = LongHorizonEvidencePaths.for_root(tmp_path)
    inputs = LongHorizonEvidenceInputs(
        steps=[],
        reports={},
        update_manifest={},
        upgrade_support_report={},
        failures=["missing update manifest"],
    )
    model = build_long_horizon_model(paths, inputs)

    lines = render_console_lines(
        model,
        published=type(
            "Published",
            (),
            {
                "summary_path": "tmp/reports/long-horizon-operations/evidence-summary.json",
                "artifact_path": "tmp/artifacts/long-horizon-operations/long-horizon-operations-evidence.json",
            },
        )(),
    )

    assert lines == [
        "summary_path: tmp/reports/long-horizon-operations/evidence-summary.json",
        "artifact_path: tmp/artifacts/long-horizon-operations/long-horizon-operations-evidence.json",
        "objc3c-long-horizon-evidence: FAIL",
    ]
