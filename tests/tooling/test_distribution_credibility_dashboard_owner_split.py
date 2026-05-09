from __future__ import annotations

import importlib
import sys
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_distribution_credibility_dashboard.input_loading import DistributionCredibilityDashboardInputs
from objc3c_distribution_credibility_dashboard.model import build_dashboard_model
from objc3c_distribution_credibility_dashboard.paths import (
    DistributionCredibilityDashboardPaths,
    SUMMARY_CONTRACT_ID,
)
from objc3c_distribution_credibility_dashboard.rendering import dashboard_summary_payload


OWNER_MODULES = (
    "objc3c_distribution_credibility_dashboard.paths",
    "objc3c_distribution_credibility_dashboard.input_loading",
    "objc3c_distribution_credibility_dashboard.validation",
    "objc3c_distribution_credibility_dashboard.probes",
    "objc3c_distribution_credibility_dashboard.model",
    "objc3c_distribution_credibility_dashboard.rendering",
    "objc3c_distribution_credibility_dashboard.publication",
)


def test_distribution_credibility_dashboard_owner_modules_are_explicit() -> None:
    for module_name in OWNER_MODULES:
        assert importlib.import_module(module_name)


def test_distribution_credibility_dashboard_entrypoint_delegates_to_owner_modules() -> None:
    script_text = (ROOT / "scripts" / "build_objc3c_distribution_credibility_dashboard.py").read_text(encoding="utf-8")
    assert "objc3c_distribution_credibility_dashboard.cli" in script_text
    assert "run_capture" not in script_text
    assert "write_json_file" not in script_text
    assert "datetime.now" not in script_text


def test_distribution_credibility_dashboard_model_preserves_public_contract() -> None:
    paths = DistributionCredibilityDashboardPaths.for_root(ROOT)
    inputs = DistributionCredibilityDashboardInputs(
        source_surface={"contract_id": "objc3c.distribution.credibility.source.surface.v1"},
        trust_architecture={"contract_id": "objc3c.distribution.credibility.trust.signal.architecture.v1"},
        install_doc_surface={
            "contract_id": "objc3c.distribution.credibility.install.release.doc.surface.v1",
            "primary_docs": ["docs/install.md"],
            "release_docs": ["docs/release.md"],
        },
        operator_policy={"states": ["ready", "degraded", "blocked"]},
        release_drill_policy={"required_drill_steps": ["fetch", "verify", "install", "rollback", "record"]},
        schema_surface={
            "contract_id": "objc3c.distribution.credibility.schema.surface.v1",
            "dashboard_schema": "schemas/dashboard.schema.json",
            "trust_report_schema": "schemas/trust-report.schema.json",
        },
        artifact_surface={"contract_id": "objc3c.distribution.credibility.artifact.surface.v1"},
        release_manifest={
            "contract_id": "objc3c.release.foundation.manifest.v1",
            "primary_package_manifest_sha256": "abc123",
            "release_version": "v0.12",
            "reproducibility_match": True,
            "release_evidence_index_sha256": "def456",
        },
        package_channels={
            "status": "PASS",
            "portable_archive": "portable.zip",
            "installer_archive": "installer.zip",
            "offline_archive": "offline.zip",
        },
        release_operations_publication={"status": "PASS", "warning_count": 2},
        release_operations_end_to_end={"status": "PASS"},
        release_evidence={"schema_id": "objc3-conformance-evidence-index/v1"},
    )

    model = build_dashboard_model(paths, inputs)
    payload = dashboard_summary_payload(
        model,
        generated_at_utc=datetime(2026, 5, 9, tzinfo=timezone.utc),
    )

    assert payload["contract_id"] == SUMMARY_CONTRACT_ID
    assert payload["status"] == "PASS"
    assert payload["trust_state"] == "ready"
    assert payload["release_id"] == "abc123"
    assert payload["release_version"] == "v0.12"
    assert payload["warning_count"] == 2
    assert payload["required_drill_steps"] == ["fetch", "verify", "install", "rollback", "record"]
    assert payload["install_docs"] == ["docs/install.md"]
    assert payload["release_docs"] == ["docs/release.md"]
    assert payload["failures"] == []
    assert [signal["signal_id"] for signal in payload["trust_signals"]] == [
        "release-foundation-lineage",
        "package-channel-install-smoke",
        "release-operations-metadata",
        "release-evidence-gate",
    ]
    assert payload["upstream_reports"]["release_operations_end_to_end"] == (
        "tmp/reports/release-operations/end-to-end-summary.json"
    )
