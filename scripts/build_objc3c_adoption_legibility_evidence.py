#!/usr/bin/env python3
"""Generate adoption, replay, comparison, and onboarding evidence."""

from __future__ import annotations

from objc3c_adoption_legibility_evidence.cli import main
from objc3c_adoption_legibility_evidence.evidence_loading import (
    AdoptionLegibilityEvidenceInputs,
    load_adoption_legibility_inputs,
    load_contracts,
    load_required_reports,
    run_generation_steps,
    run_step,
)
from objc3c_adoption_legibility_evidence.model import (
    AdoptionLegibilityEvidenceModel,
    build_adoption_legibility_model,
    build_artifact,
    build_publication,
    build_summary,
    generated_from_commands,
)
from objc3c_adoption_legibility_evidence.normalization import (
    adoption_replay_actions,
    adoption_replay_phases,
    comparison_axes,
    comparison_evidence_paths,
    interop_axes,
    package_actions,
)
from objc3c_adoption_legibility_evidence.paths import (
    BLOCKER_METADATA,
    CONTRACT_ID,
    OWNER_CONTRACTS,
    OWNER_SPLIT,
    PACKAGE_BRIDGE,
    PUBLIC_ACTIONS,
    SUMMARY_CONTRACT_ID,
    SUPPORT_STATE,
    AdoptionLegibilityEvidencePaths,
    EvidenceStep,
    RequiredReport,
)
from objc3c_adoption_legibility_evidence.publication import (
    PublishedAdoptionLegibilityEvidence,
    publish_adoption_legibility_evidence,
)
from objc3c_adoption_legibility_evidence.rendering import render_console_lines
from objc3c_adoption_legibility_evidence.validation import expect, status_passes


_PATHS = AdoptionLegibilityEvidencePaths.for_root()
ROOT = _PATHS.root
ARTIFACT_PATH = _PATHS.artifact_path
PUBLICATION_PATH = _PATHS.publication_path
SUMMARY_PATH = _PATHS.summary_path
BOUNDARY_CONTRACT = _PATHS.boundary_contract
PUBLIC_CLAIM_POLICY = _PATHS.public_claim_policy
COMPARISON_SEMANTICS = _PATHS.comparison_semantics
ADOPTION_REPLAY_SEMANTICS = _PATHS.adoption_replay_semantics
ARTIFACT_CONTRACT = _PATHS.artifact_contract
BOUNDARY_SUMMARY = _PATHS.boundary_summary
PUBLIC_CLAIM_SUMMARY = _PATHS.public_claim_summary
COMPARISON_SUMMARY = _PATHS.comparison_summary
ADOPTION_REPLAY_SUMMARY = _PATHS.adoption_replay_summary
ARTIFACT_CONTRACT_SUMMARY = _PATHS.artifact_contract_summary
STEPS = [(step.name, step.command) for step in _PATHS.steps()]


__all__ = [
    "ADOPTION_REPLAY_SEMANTICS",
    "ADOPTION_REPLAY_SUMMARY",
    "ARTIFACT_CONTRACT",
    "ARTIFACT_CONTRACT_SUMMARY",
    "ARTIFACT_PATH",
    "AdoptionLegibilityEvidenceInputs",
    "AdoptionLegibilityEvidenceModel",
    "AdoptionLegibilityEvidencePaths",
    "BLOCKER_METADATA",
    "BOUNDARY_CONTRACT",
    "BOUNDARY_SUMMARY",
    "COMPARISON_SEMANTICS",
    "COMPARISON_SUMMARY",
    "CONTRACT_ID",
    "EvidenceStep",
    "OWNER_CONTRACTS",
    "OWNER_SPLIT",
    "PACKAGE_BRIDGE",
    "PUBLIC_ACTIONS",
    "PUBLICATION_PATH",
    "PUBLIC_CLAIM_POLICY",
    "PUBLIC_CLAIM_SUMMARY",
    "PublishedAdoptionLegibilityEvidence",
    "ROOT",
    "RequiredReport",
    "STEPS",
    "SUMMARY_CONTRACT_ID",
    "SUMMARY_PATH",
    "SUPPORT_STATE",
    "adoption_replay_actions",
    "adoption_replay_phases",
    "build_adoption_legibility_model",
    "build_artifact",
    "build_publication",
    "build_summary",
    "comparison_axes",
    "comparison_evidence_paths",
    "expect",
    "generated_from_commands",
    "interop_axes",
    "load_adoption_legibility_inputs",
    "load_contracts",
    "load_required_reports",
    "main",
    "package_actions",
    "publish_adoption_legibility_evidence",
    "render_console_lines",
    "run_generation_steps",
    "run_step",
    "status_passes",
]


if __name__ == "__main__":
    raise SystemExit(main())
