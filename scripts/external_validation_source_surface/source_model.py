"""Expected source, trust, intake, quarantine, and artifact contracts."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


SOURCE_SURFACE_CONTRACT_ID = "objc3c.external_validation.source.surface.v1"
SCHEMA_VERSION = 1
SUMMARY_CONTRACT_ID = "objc3c.external_validation.source.surface.summary.v1"

EXPECTED_FAMILY_IDS = (
    "intake-normalization-boundary",
    "independent-replay-proofs",
    "packaged-reproducibility-surface",
    "support-claim-gating",
)

EXPECTED_SOURCE_FAMILY_PATHS = {
    "intake-normalization-boundary": (
        "docs/runbooks/objc3c_external_validation.md",
        "tests/tooling/fixtures/external_validation/README.md",
        "tests/tooling/fixtures/external_validation/source_surface.json",
        "tests/tooling/fixtures/external_validation/trust_policy.json",
        "tests/tooling/fixtures/external_validation/intake_manifest.json",
        "tests/tooling/fixtures/external_validation/quarantine_manifest.json",
        "tests/tooling/fixtures/external_validation/repro_corpus.json",
        "tests/tooling/fixtures/external_validation/support_claim_gate.json",
        "tests/tooling/fixtures/external_validation/artifact_surface.json",
        "tests/tooling/fixtures/external_validation/workflow_surface.json",
        "tests/tooling/fixtures/objc3c",
        "tests/conformance/corpus_surface.json",
        "tests/conformance/longitudinal_suites.json",
        "docs/runbooks/objc3c_conformance_corpus.md",
    ),
    "independent-replay-proofs": (
        "scripts/check_objc3c_parser_replay_proof.ps1",
        "scripts/check_objc3c_diagnostics_replay_proof.ps1",
        "scripts/check_objc3c_lowering_replay_proof.ps1",
        "scripts/check_objc3c_execution_replay_proof.ps1",
        "tests/conformance/parser/manifest.json",
        "tests/conformance/diagnostics/manifest.json",
        "tests/conformance/lowering_abi/manifest.json",
        "tests/conformance/module_roundtrip/manifest.json",
        "tests/tooling/fixtures/objc3c",
    ),
    "packaged-reproducibility-surface": (
        "scripts/check_objc3c_conformance_corpus_integration.py",
        "scripts/check_objc3c_runnable_conformance_corpus_end_to_end.py",
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "docs/runbooks/objc3c_conformance_corpus.md",
        "tests/conformance/corpus_surface.json",
    ),
    "support-claim-gating": (
        "tests/tooling/fixtures/external_validation/repro_corpus.json",
        "tests/tooling/fixtures/external_validation/support_claim_gate.json",
        "tests/tooling/fixtures/external_validation/claim_gate_negative/local_only_evidence.json",
        "tests/tooling/fixtures/external_validation/claim_gate_negative/missing_evidence.json",
        "tests/tooling/fixtures/external_validation/claim_gate_negative/non_reproducible_evidence.json",
        "tests/tooling/fixtures/external_validation/claim_gate_negative/stale_evidence.json",
        "scripts/check_objc3c_external_support_claim_gate.py",
        "tests/tooling/fixtures/adoption_legibility/public_claim_policy.json",
        "docs/runbooks/objc3c_external_validation.md",
    ),
}

EXPECTED_CONTRACT_IDS = {
    "trust_policy": "objc3c.external_validation.trust.policy.v1",
    "intake_manifest": "objc3c.external_validation.intake.manifest.v1",
    "quarantine_manifest": "objc3c.external_validation.quarantine.manifest.v1",
    "repro_corpus": "objc3c.external_validation.repro_corpus.v1",
    "support_claim_gate": "objc3c.external_validation.support_claim_gate.v1",
    "artifact_surface": "objc3c.external_validation.artifact.surface.v1",
    "workflow_surface": "objc3c.external_validation.workflow.surface.v1",
}

EXPECTED_TRUST_STATES = ("candidate", "accepted", "quarantined", "rejected")
EXPECTED_PUBLISHABLE_TRUST_STATES = ("accepted",)
EXPECTED_REQUIRED_PROVENANCE_FIELDS = (
    "origin",
    "captured_at_utc",
    "captured_by",
    "license",
    "normalized_from",
)
EXPECTED_INTAKE_SURFACES = ("conformance-case", "replay-contract")
EXPECTED_INTAKE_FAMILIES = ("parser", "module-roundtrip")
EXPECTED_QUARANTINE_TRUST_STATES = ("quarantined", "rejected")
EXPECTED_DISCLOSURE_MODES = ("internal-only", "redacted-summary", "blocked")
EXPECTED_ESCALATION_TARGETS = ("license-review", "maintainer-review", "security-review")


@dataclass(frozen=True)
class SourceSurfaceValidation:
    required_paths: dict[str, str]
    checked_roots: tuple[str, ...]
    family_summaries: list[dict[str, Any]]
    intake_entry_summaries: list[dict[str, Any]]
    quarantine_entry_summaries: list[dict[str, Any]]
    checked_paths: list[str]
