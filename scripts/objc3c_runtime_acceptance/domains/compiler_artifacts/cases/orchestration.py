"""Public orchestration for compiler artifact acceptance cases."""

from __future__ import annotations

import shutil
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.compile_backends import DIRECT_COMPILE_BACKEND
from objc3c_runtime_acceptance.compile_backends import WRAPPER_COMPILE_BACKEND

from .assertions import expect_artifact_registry_cache_isolation
from .assertions import expect_artifact_registry_distinct_args_not_reused
from .assertions import expect_artifact_registry_same_args_reused
from .assertions import expect_compile_backend_parity_artifacts
from .compilation import compile_direct_for_registry
from .compilation import compile_parity_fixture
from .fixtures import artifact_registry_isolation_layout
from .fixtures import compile_backend_parity_layout
from .payloads import build_artifact_registry_key_isolation_case_result
from .payloads import build_compile_backend_parity_case_result
from .predicates import artifact_registry_cache_evidence
from .predicates import current_registry_miss_event_count
from .predicates import current_registry_reuse_event_count
from .predicates import latest_registry_reuse_event
from .predicates import load_compile_backend_parity_artifacts
from .predicates import registry_event_snapshot


def check_compile_backend_parity_case(run_dir: Path) -> CaseResult:
    layout = compile_backend_parity_layout(run_dir)
    direct_compile = compile_parity_fixture(
        layout,
        layout.direct_dir,
        backend=DIRECT_COMPILE_BACKEND,
        backend_label="direct",
    )
    shutil.rmtree(layout.parity_cache_root, ignore_errors=True)
    wrapper_compile = compile_parity_fixture(
        layout,
        layout.wrapper_dir,
        backend=WRAPPER_COMPILE_BACKEND,
        backend_label="wrapper",
    )

    artifacts = load_compile_backend_parity_artifacts(layout)
    expect_compile_backend_parity_artifacts(artifacts)

    return build_compile_backend_parity_case_result(
        layout,
        direct_compile=direct_compile,
        wrapper_compile=wrapper_compile,
        artifacts=artifacts,
    )


def check_artifact_registry_key_isolation_case(run_dir: Path) -> CaseResult:
    layout = artifact_registry_isolation_layout(run_dir)
    event_snapshot = registry_event_snapshot()

    first_compile = compile_direct_for_registry(
        layout,
        layout.producer_dir,
        layout.args_a,
    )
    second_compile = compile_direct_for_registry(
        layout,
        layout.distinct_args_dir,
        layout.args_b,
    )
    reuse_after_distinct_args = current_registry_reuse_event_count()
    expect_artifact_registry_distinct_args_not_reused(
        event_snapshot.reuse_events,
        reuse_after_distinct_args,
    )

    third_compile = compile_direct_for_registry(
        layout,
        layout.consumer_dir,
        layout.args_a,
    )
    reuse_after_same_args = current_registry_reuse_event_count()
    expect_artifact_registry_same_args_reused(
        layout,
        reuse_before=event_snapshot.reuse_events,
        reuse_after_same_args=reuse_after_same_args,
        reuse_event=latest_registry_reuse_event(),
    )

    cache_evidence = artifact_registry_cache_evidence(layout)
    expect_artifact_registry_cache_isolation(
        cache_evidence.changed_args_key_a,
        cache_evidence.changed_args_key_b,
        cache_evidence.changed_import_surface_key_a,
        cache_evidence.changed_import_surface_key_b,
    )

    return build_artifact_registry_key_isolation_case_result(
        layout,
        first_compile=first_compile,
        second_compile=second_compile,
        third_compile=third_compile,
        event_snapshot=event_snapshot,
        reuse_after_distinct_args=reuse_after_distinct_args,
        reuse_after_same_args=reuse_after_same_args,
        miss_events_after=current_registry_miss_event_count(),
        cache_evidence=cache_evidence,
    )


__all__ = [
    "check_artifact_registry_key_isolation_case",
    "check_compile_backend_parity_case",
]
