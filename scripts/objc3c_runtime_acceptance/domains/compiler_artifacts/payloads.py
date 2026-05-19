"""Compiler artifact acceptance payload shaping."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.compile_backends import DEFAULT_COMPILE_BACKEND
from objc3c_runtime_acceptance.progress_format import repo_display_path

from .catalog import ARTIFACT_REGISTRY_CLAIM_CLASS
from .catalog import ARTIFACT_REGISTRY_KEY_ISOLATION_CASE_ID
from .catalog import ARTIFACT_REGISTRY_KEY_ISOLATION_PROBE
from .catalog import ARTIFACT_REGISTRY_NEGATIVE_REUSE_MODEL
from .catalog import COMPILE_BACKEND_PARITY_CASE_ID
from .catalog import COMPILE_BACKEND_PARITY_CLAIM_CLASS
from .catalog import COMPILE_BACKEND_PARITY_PROBE
from .data import ArtifactRegistryCaseEvidence
from .data import CompileBackendParityEvidence


def compile_backend_parity_case_result(
    evidence: CompileBackendParityEvidence,
) -> CaseResult:
    return CaseResult(
        case_id=COMPILE_BACKEND_PARITY_CASE_ID,
        probe=COMPILE_BACKEND_PARITY_PROBE,
        fixture=repo_display_path(evidence.fixture),
        claim_class=COMPILE_BACKEND_PARITY_CLAIM_CLASS,
        passed=True,
        summary={
            "direct_backend": evidence.direct_backend,
            "wrapper_backend": evidence.wrapper_backend,
            "default_backend": DEFAULT_COMPILE_BACKEND,
            "artifact_set_digest_sha256": evidence.direct_provenance.get(
                "artifact_set_digest_sha256"
            ),
            "truthfulness_fields_compared": evidence.compared_truthfulness_fields,
            "provenance_contract_id": evidence.direct_provenance.get("contract_id"),
            "compile_output_truthfulness_contract_id": (
                evidence.direct_truthfulness.get("contract_id")
            ),
        },
    )


def artifact_registry_key_isolation_case_result(
    evidence: ArtifactRegistryCaseEvidence,
) -> CaseResult:
    cache = evidence.cache
    return CaseResult(
        case_id=ARTIFACT_REGISTRY_KEY_ISOLATION_CASE_ID,
        probe=ARTIFACT_REGISTRY_KEY_ISOLATION_PROBE,
        fixture=repo_display_path(evidence.fixture),
        claim_class=ARTIFACT_REGISTRY_CLAIM_CLASS,
        passed=True,
        summary={
            "producer_returncode": evidence.first_result.returncode,
            "distinct_args_returncode": evidence.second_result.returncode,
            "same_args_reuse_returncode": evidence.third_result.returncode,
            "reuse_events_before": evidence.reuse_events_before,
            "reuse_events_after_distinct_args": (
                evidence.reuse_events_after_distinct_args
            ),
            "reuse_events_after_same_args": evidence.reuse_events_after_same_args,
            "miss_events_added": evidence.miss_events_after
            - evidence.miss_events_before,
            "changed_args_key_a": cache.changed_args_key_a,
            "changed_args_key_b": cache.changed_args_key_b,
            "changed_args_payload_a": cache.changed_args_payload_a,
            "changed_args_payload_b": cache.changed_args_payload_b,
            "changed_import_surface_key_a": cache.changed_import_surface_key_a,
            "changed_import_surface_key_b": cache.changed_import_surface_key_b,
            "changed_import_surface_payload_a": (
                cache.changed_import_surface_payload_a
            ),
            "changed_import_surface_payload_b": (
                cache.changed_import_surface_payload_b
            ),
            "negative_reuse_model": ARTIFACT_REGISTRY_NEGATIVE_REUSE_MODEL,
        },
    )
