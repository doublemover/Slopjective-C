"""Live runtime command execution for release-candidate conformance."""

from __future__ import annotations

import importlib
import tempfile
from pathlib import Path
from typing import Any

from .config import LIVE_CASE_ROOT
from .import_paths import ensure_import_paths
from .io import repo_rel

ensure_import_paths()

from objc3c_runtime_acceptance.case_result import CaseResult  # noqa: E402
from objc3c_runtime_acceptance.domains import release_claims  # noqa: E402
from objc3c_runtime_acceptance.native_binaries import find_clangxx  # noqa: E402


runtime_acceptance = importlib.import_module("check_objc3c_runtime_acceptance")
_RUNTIME_ACCEPTANCE_EXPORTS = {
    "CaseResult": CaseResult,
    "find_clangxx": find_clangxx,
    "check_claimable_surface_residual_non_claimable_gaps_source_surface_case": (
        release_claims.check_claimable_surface_residual_non_claimable_gaps_source_surface_case
    ),
    "check_strict_profile_feature_claim_source_surface_case": (
        release_claims.check_strict_profile_feature_claim_source_surface_case
    ),
    "check_claimability_semantics_release_policy_case": (
        release_claims.check_claimability_semantics_release_policy_case
    ),
    "check_strict_profile_claim_implementation_case": (
        release_claims.check_strict_profile_claim_implementation_case
    ),
    "check_retired_artifact_rejection_contracts_case": (
        release_claims.check_retired_artifact_rejection_contracts_case
    ),
    "check_claim_publication_dashboard_schema_surface_case": (
        release_claims.check_claim_publication_dashboard_schema_surface_case
    ),
    "check_final_claim_publication_deprecated_path_shutdown_case": (
        release_claims.check_final_claim_publication_deprecated_path_shutdown_case
    ),
    "check_release_candidate_runtime_claim_abi_case": (
        release_claims.check_release_candidate_runtime_claim_abi_case
    ),
    "check_current_release_evidence_owner_payload_case": (
        release_claims.check_current_release_evidence_owner_payload_case
    ),
    "build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface": (
        release_claims.build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface
    ),
    "build_runtime_strict_profile_feature_claim_source_surface": (
        release_claims.build_runtime_strict_profile_feature_claim_source_surface
    ),
    "build_runtime_claimability_semantics_release_policy_surface": (
        release_claims.build_runtime_claimability_semantics_release_policy_surface
    ),
    "build_runtime_strict_profile_claim_implementation_surface": (
        release_claims.build_runtime_strict_profile_claim_implementation_surface
    ),
    "build_runtime_retired_artifact_rejection_contracts_surface": (
        release_claims.build_runtime_retired_artifact_rejection_contracts_surface
    ),
    "build_runtime_claim_publication_dashboard_schema_surface": (
        release_claims.build_runtime_claim_publication_dashboard_schema_surface
    ),
    "build_runtime_final_claim_publication_deprecated_path_shutdown_surface": (
        release_claims.build_runtime_final_claim_publication_deprecated_path_shutdown_surface
    ),
    "build_runtime_release_candidate_claim_abi_surface": (
        release_claims.build_runtime_release_candidate_claim_abi_surface
    ),
    "build_runtime_current_release_evidence_owner_payload_surface": (
        release_claims.build_runtime_current_release_evidence_owner_payload_surface
    ),
}

for _name, _value in _RUNTIME_ACCEPTANCE_EXPORTS.items():
    setattr(runtime_acceptance, _name, _value)


def run_release_candidate_case_commands(
    run_dir: Path,
    runtime_backend: Any,
) -> list[CaseResult]:
    clangxx = runtime_backend.find_clangxx()
    return [
        runtime_backend.check_claimable_surface_residual_non_claimable_gaps_source_surface_case(
            run_dir
        ),
        runtime_backend.check_strict_profile_feature_claim_source_surface_case(run_dir),
        runtime_backend.check_claimability_semantics_release_policy_case(run_dir),
        runtime_backend.check_strict_profile_claim_implementation_case(run_dir),
        runtime_backend.check_retired_artifact_rejection_contracts_case(
            run_dir
        ),
        runtime_backend.check_claim_publication_dashboard_schema_surface_case(run_dir),
        runtime_backend.check_final_claim_publication_deprecated_path_shutdown_case(run_dir),
        runtime_backend.check_release_candidate_runtime_claim_abi_case(clangxx, run_dir),
        runtime_backend.check_current_release_evidence_owner_payload_case(
            clangxx,
            run_dir,
        ),
    ]


def collect_live_results(
    runtime_backend: Any = runtime_acceptance,
) -> tuple[list[CaseResult], str]:
    LIVE_CASE_ROOT.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=LIVE_CASE_ROOT) as tmp_dir:
        run_dir = Path(tmp_dir)
        results = run_release_candidate_case_commands(run_dir, runtime_backend)
        run_dir_rel = repo_rel(run_dir)
    return results, run_dir_rel


__all__ = [
    "collect_live_results",
    "run_release_candidate_case_commands",
    "runtime_acceptance",
]
