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
from objc3c_shared.json_io import load_json_object
from scripts.objc3c_workflow.actions.release_governance_distribution_credibility_owner_contracts import (
    DISTRIBUTION_CREDIBILITY_OWNER_CONTRACTS,
    require_distribution_credibility_owner_contract,
)


OWNER_MODULES = (
    "objc3c_distribution_credibility_dashboard.paths",
    "objc3c_distribution_credibility_dashboard.input_loading",
    "objc3c_distribution_credibility_dashboard.validation",
    "objc3c_distribution_credibility_dashboard.probes",
    "objc3c_distribution_credibility_dashboard.model",
    "objc3c_distribution_credibility_dashboard.rendering",
    "objc3c_distribution_credibility_dashboard.publication",
    "objc3c_distribution_credibility_dashboard.trust_report_publication",
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


def test_distribution_credibility_trust_report_entrypoint_delegates_to_owner_modules() -> None:
    script_text = (ROOT / "scripts" / "publish_objc3c_distribution_trust_report.py").read_text(encoding="utf-8")
    assert "objc3c_distribution_credibility_dashboard.trust_report_publication" in script_text
    assert "shutil.copyfile" not in script_text
    assert "write_json_file" not in script_text
    assert "datetime.now" not in script_text


def test_distribution_credibility_dashboard_model_preserves_public_contract() -> None:
    paths = DistributionCredibilityDashboardPaths.for_root(ROOT)
    inputs = DistributionCredibilityDashboardInputs(
        source_surface={"contract_id": "objc3c.distribution.credibility.source.surface.v1"},
        trust_architecture={
            "contract_id": "objc3c.distribution.credibility.trust.signal.architecture.v1",
            "upstream_surfaces": [
                "release-foundation",
                "packaging-channels",
                "package-ecosystem",
                "release-operations",
                "release-evidence",
            ],
            "required_signal_order": [
                "release-foundation-lineage",
                "package-channel-install-smoke",
                "release-operations-metadata",
                "release-evidence-gate",
            ],
            "trust_signals": [
                {
                    "signal_id": "release-foundation-lineage",
                    "artifact_source": "tmp/artifacts/release-foundation",
                    "claim_boundary": "manifest lineage stays attached",
                },
                {
                    "signal_id": "package-channel-install-smoke",
                    "artifact_source": "tmp/reports/package-channels",
                    "claim_boundary": "install smoke stays attached",
                },
                {
                    "signal_id": "release-operations-metadata",
                    "artifact_source": "tmp/reports/release-operations",
                    "claim_boundary": "operation metadata stays attached",
                },
                {
                    "signal_id": "release-evidence-gate",
                    "artifact_source": "tmp/reports/release_evidence",
                    "claim_boundary": "evidence index stays attached",
                },
            ],
        },
        install_doc_surface={
            "contract_id": "objc3c.distribution.credibility.install.release.doc.surface.v1",
            "primary_docs": ["README.md", "docs/install.md", "docs/verify.md"],
            "release_docs": ["docs/release.md", "docs/rollback.md", "docs/evidence.md"],
        },
        operator_policy={"states": ["ready", "degraded", "blocked"]},
        release_drill_policy={
            "required_drill_steps": [
                "stage-package-channels",
                "verify-install-smoke",
                "verify-clean-package-install",
                "verify-rollback-guidance",
                "verify-update-manifest-coherence",
                "verify-release-evidence-index",
            ]
        },
        schema_surface={
            "contract_id": "objc3c.distribution.credibility.schema.surface.v1",
            "dashboard_schema": "schemas/dashboard.schema.json",
            "trust_report_schema": "schemas/trust-report.schema.json",
        },
        artifact_surface={
            "contract_id": "objc3c.distribution.credibility.artifact.surface.v1",
            "source_surface_summary": "tmp/reports/distribution-credibility/source-surface-summary.json",
            "schema_surface_summary": "tmp/reports/distribution-credibility/schema-surface-summary.json",
            "dashboard_summary": "tmp/reports/distribution-credibility/dashboard-summary.json",
            "publication_summary": "tmp/reports/distribution-credibility/publication-summary.json",
            "integration_summary": "tmp/reports/distribution-credibility/integration-summary.json",
            "end_to_end_summary": "tmp/reports/distribution-credibility/end-to-end-summary.json",
            "dashboard_artifact": (
                "tmp/artifacts/distribution-credibility/dashboard/distribution-credibility-dashboard.json"
            ),
            "trust_report_json": (
                "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.json"
            ),
            "trust_report_markdown": (
                "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.md"
            ),
        },
        workflow_surface={
            "contract_id": "objc3c.distribution.credibility.workflow.surface.v1",
            "validate_action": "validate-distribution-credibility",
            "integrated_required_steps": [
                "validate-release-operations",
                "validate-package-install-distribution",
                "check-distribution-credibility-surface",
                "check-distribution-credibility-schema-surface",
                "build-distribution-credibility-dashboard",
                "publish-distribution-credibility",
            ],
        },
        release_manifest={
            "contract_id": "objc3c.release.foundation.manifest.v1",
            "primary_package_manifest_sha256": "abc123",
            "release_version": "v0.12",
            "reproducibility_match": True,
            "release_evidence_index_sha256": "def456",
        },
        package_channels={
            "status": "PASS",
            "owner_policy": {
                "hard_cutover_guardrails": {
                    "unsupported_host_success_allowed": False,
                },
            },
            "portable_archive": "portable.zip",
            "installer_archive": "installer.zip",
            "offline_archive": "offline.zip",
        },
        package_install_distribution={
            "contract_id": "objc3c.package_ecosystem.install_distribution_credibility.summary.v1",
            "status": "PASS",
            "network_policy": "no-network-during-validation",
            "hosted_registry_support": "unsupported-fail-closed-if-claimed",
            "offline_restore_support": "local-cache-digest-checked",
            "clean_start": {"stale_artifacts_allowed": False},
            "generated_paths": [
                "tmp/artifacts/package-ecosystem/install-validation/clean-root/objc3c-install-receipt.json"
            ],
            "missing_public_actions": [],
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
    assert payload["required_drill_steps"] == [
        "stage-package-channels",
        "verify-install-smoke",
        "verify-clean-package-install",
        "verify-rollback-guidance",
        "verify-update-manifest-coherence",
        "verify-release-evidence-index",
    ]
    assert payload["install_docs"] == ["README.md", "docs/install.md", "docs/verify.md"]
    assert payload["release_docs"] == ["docs/release.md", "docs/rollback.md", "docs/evidence.md"]
    assert payload["operator_states"] == ["ready", "degraded", "blocked"]
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
    assert payload["upstream_reports"]["package_install_distribution"] == (
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
    )
    package_signal = next(
        signal
        for signal in payload["trust_signals"]
        if signal["signal_id"] == "package-channel-install-smoke"
    )
    assert package_signal["supporting_source_paths"] == [
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
    ]

    inputs.package_install_distribution["hosted_registry_support"] = "supported"
    blocked_payload = dashboard_summary_payload(
        build_dashboard_model(paths, inputs),
        generated_at_utc=datetime(2026, 5, 9, tzinfo=timezone.utc),
    )
    assert blocked_payload["status"] == "FAIL"
    assert blocked_payload["trust_state"] == "blocked"
    assert "package install hosted registry support widened" in blocked_payload["failures"]
    blocked_package_signal = next(
        signal
        for signal in blocked_payload["trust_signals"]
        if signal["signal_id"] == "package-channel-install-smoke"
    )
    assert blocked_package_signal["status"] == "FAIL"

    inputs.package_install_distribution["hosted_registry_support"] = "unsupported-fail-closed-if-claimed"
    inputs.package_channels["owner_policy"]["hard_cutover_guardrails"][
        "unsupported_host_success_allowed"
    ] = True
    unsupported_platform_payload = dashboard_summary_payload(
        build_dashboard_model(paths, inputs),
        generated_at_utc=datetime(2026, 5, 9, tzinfo=timezone.utc),
    )
    assert unsupported_platform_payload["status"] == "FAIL"
    assert unsupported_platform_payload["trust_state"] == "blocked"
    assert "package channel unsupported host guardrail widened" in unsupported_platform_payload["failures"]


def test_distribution_credibility_actions_have_trust_and_release_drill_owner_contracts() -> None:
    assert set(DISTRIBUTION_CREDIBILITY_OWNER_CONTRACTS) == {
        "check-distribution-credibility-surface",
        "check-distribution-credibility-schema-surface",
        "build-distribution-credibility-dashboard",
        "publish-distribution-credibility",
        "validate-distribution-credibility",
        "validate-distribution-credibility-end-to-end",
    }

    owner_roles = {
        require_distribution_credibility_owner_contract(action_name).owner_role
        for action_name in DISTRIBUTION_CREDIBILITY_OWNER_CONTRACTS
    }
    assert {
        "distribution-credibility-source-owner",
        "distribution-credibility-schema-owner",
        "distribution-credibility-dashboard-owner",
        "distribution-credibility-trust-owner",
        "distribution-credibility-gate-owner",
        "distribution-credibility-release-drill-owner",
    } == owner_roles

    for action_name in DISTRIBUTION_CREDIBILITY_OWNER_CONTRACTS:
        contract = require_distribution_credibility_owner_contract(action_name)
        assert contract.source_contracts
        assert contract.required_artifacts
        assert contract.claim_boundary
        assert not contract.evidence_log_allowed
        assert not contract.wrapper_only_allowed
        assert "evidence-log" in " ".join(contract.blocking_conditions)
        assert "wrapper-only" in " ".join(contract.blocking_conditions)


def test_distribution_credibility_fixtures_reject_evidence_log_trust_evidence() -> None:
    fixture_root = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility"
    workflow_surface = load_json_object(fixture_root / "workflow_surface.json")
    trust_architecture = load_json_object(fixture_root / "trust_signal_architecture.json")
    release_drill = load_json_object(fixture_root / "release_drill_policy.json")
    artifact_surface = load_json_object(fixture_root / "artifact_surface.json")

    owner_policy = workflow_surface["owner_policy"]
    assert owner_policy["trust_owner"] == "distribution-credibility-trust-owner"
    assert owner_policy["release_drill_owner"] == "distribution-credibility-release-drill-owner"
    assert owner_policy["evidence_log_allowed"] is False
    assert owner_policy["wrapper_only_allowed"] is False

    trust_owner = trust_architecture["owner_contracts"]["trust_owner"]
    assert trust_owner["evidence_log_allowed"] is False
    assert "not independent manual badges" in trust_owner["claim_boundary"]

    release_drill_owner = release_drill["owner_contract"]
    assert release_drill_owner["evidence_log_allowed"] is False
    assert release_drill_owner["wrapper_only_allowed"] is False
    assert "cannot substitute for the drill" in release_drill_owner["claim_boundary"]

    claim_policy = artifact_surface["artifact_claim_policy"]
    assert claim_policy["evidence_log_allowed"] is False
    assert claim_policy["wrapper_only_allowed"] is False
    assert "release-drill" in claim_policy["trust_report_boundary"]
