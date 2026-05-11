"""Compiler artifact case payload construction."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_result import CaseResult

from ..data import ArtifactRegistryCacheEvidence
from ..data import ArtifactRegistryCaseEvidence
from ..data import CompileBackendParityEvidence
from ..data import RegistryEventSnapshot
from ..payloads import artifact_registry_key_isolation_case_result
from ..payloads import compile_backend_parity_case_result
from .models import ArtifactRegistryIsolationLayout
from .models import CompileBackendParityArtifacts
from .models import CompileBackendParityLayout
from .models import CompileBackendRun


def build_compile_backend_parity_case_result(
    layout: CompileBackendParityLayout,
    *,
    direct_compile: CompileBackendRun,
    wrapper_compile: CompileBackendRun,
    artifacts: CompileBackendParityArtifacts,
) -> CaseResult:
    return compile_backend_parity_case_result(
        CompileBackendParityEvidence(
            fixture=layout.fixture,
            direct_backend=direct_compile.selected_backend,
            wrapper_backend=wrapper_compile.selected_backend,
            direct_provenance=artifacts.direct_provenance,
            direct_truthfulness=artifacts.direct_truthfulness,
            compared_truthfulness_fields=list(artifacts.compared_truthfulness_fields),
        )
    )


def build_artifact_registry_key_isolation_case_result(
    layout: ArtifactRegistryIsolationLayout,
    *,
    first_compile: CompileBackendRun,
    second_compile: CompileBackendRun,
    third_compile: CompileBackendRun,
    event_snapshot: RegistryEventSnapshot,
    reuse_after_distinct_args: int,
    reuse_after_same_args: int,
    miss_events_after: int,
    cache_evidence: ArtifactRegistryCacheEvidence,
) -> CaseResult:
    return artifact_registry_key_isolation_case_result(
        ArtifactRegistryCaseEvidence(
            fixture=layout.fixture,
            first_result=first_compile.result,
            second_result=second_compile.result,
            third_result=third_compile.result,
            reuse_events_before=event_snapshot.reuse_events,
            miss_events_before=event_snapshot.miss_events,
            reuse_events_after_distinct_args=reuse_after_distinct_args,
            reuse_events_after_same_args=reuse_after_same_args,
            miss_events_after=miss_events_after,
            cache=cache_evidence,
        )
    )


__all__ = [
    "build_artifact_registry_key_isolation_case_result",
    "build_compile_backend_parity_case_result",
]
