"""Release-candidate package case and artifact catalog."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ReleaseCandidateProbeCase:
    case_id: str
    manifest_key: str
    label: str
    executable_name: str

    @property
    def compile_action(self) -> str:
        return f"compile-release-candidate-{self.case_id}-probe"

    @property
    def run_action(self) -> str:
        return f"run-release-candidate-{self.case_id}-probe"


RELEASE_CANDIDATE_PROBES = (
    ReleaseCandidateProbeCase(
        case_id="claim",
        manifest_key="release_candidate_claim_probe",
        label="packaged release-candidate claim ABI probe",
        executable_name="claim_probe.exe",
    ),
    ReleaseCandidateProbeCase(
        case_id="evidence",
        manifest_key="release_candidate_evidence_probe",
        label="packaged release-candidate evidence probe",
        executable_name="evidence_probe.exe",
    ),
)

REQUIRED_MANIFEST_KEYS = (
    "native_executable",
    "compile_wrapper",
    "runtime_library",
    "execution_smoke_script",
    "execution_replay_script",
    "runtime_public_header",
    "runtime_internal_header",
    "release_evidence_dashboard_schema",
    "release_evidence_gate_script",
    "release_evidence_runbook",
    "release_candidate_fixture",
    "release_candidate_claim_probe",
    "release_candidate_evidence_probe",
)

COMPILE_ARTIFACT_FILES = (
    ("manifest", "module.manifest.json"),
    ("registration_manifest", "module.runtime-registration-manifest.json"),
    ("compile_provenance", "module.compile-provenance.json"),
    ("conformance_report", "module.objc3-conformance-report.json"),
    ("conformance_publication", "module.objc3-conformance-publication.json"),
    ("advanced_feature_gate", "module.objc3-advanced-feature-gate.json"),
    ("release_candidate_matrix", "module.objc3-release-candidate-matrix.json"),
    ("object", "module.obj"),
)

EXPECTED_VALIDATE_ARTIFACTS = (
    "module.objc3-advanced-feature-gate.json",
    "module.objc3-conformance-validation.json",
    "module.objc3-dashboard-status.json",
    "module.objc3-release-candidate-matrix.json",
    "module.objc3-release-evidence-operation.json",
)


__all__ = [
    "COMPILE_ARTIFACT_FILES",
    "EXPECTED_VALIDATE_ARTIFACTS",
    "RELEASE_CANDIDATE_PROBES",
    "REQUIRED_MANIFEST_KEYS",
    "ReleaseCandidateProbeCase",
]
