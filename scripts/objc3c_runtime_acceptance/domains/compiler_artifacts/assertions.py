"""Compiler artifact acceptance assertions."""

from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.compile_backends import DIRECT_COMPILE_BACKEND
from objc3c_runtime_acceptance.expectation_matching import expect

from .predicates import artifact_cache_keys_are_distinct
from .predicates import registry_reused_exactly_once
from .predicates import registry_reuse_count_unchanged
from .predicates import reuse_event_matches_paths
from .predicates import selected_backend_is_direct


def expect_backend_compile_success(
    backend_label: str,
    result: subprocess.CompletedProcess[str],
) -> None:
    if result.returncode != 0:
        raise RuntimeError(
            f"{backend_label} compile backend failed during parity check:\nSTDOUT:\n"
            + result.stdout
            + "\nSTDERR:\n"
            + result.stderr
        )


def expect_compile_truthfulness_parity(
    direct_truthfulness: dict[str, Any],
    wrapper_truthfulness: dict[str, Any],
    compared_fields: list[str],
) -> None:
    for field in compared_fields:
        if direct_truthfulness.get(field) != wrapper_truthfulness.get(field):
            raise RuntimeError(
                "direct compile backend truthfulness drifted from wrapper for "
                f"{field}: direct={direct_truthfulness.get(field)!r} "
                f"wrapper={wrapper_truthfulness.get(field)!r}"
            )


def expect_artifact_digest_parity(
    direct_provenance: dict[str, Any],
    wrapper_provenance: dict[str, Any],
) -> None:
    if (
        direct_provenance.get("artifact_set_digest_sha256")
        != wrapper_provenance.get("artifact_set_digest_sha256")
    ):
        raise RuntimeError(
            "direct compile backend artifact digest drifted from wrapper output"
        )


def expect_direct_compile_backend_provenance(
    direct_provenance: dict[str, Any],
) -> None:
    if direct_provenance.get("compile_backend") != DIRECT_COMPILE_BACKEND:
        raise RuntimeError("direct compile backend did not stamp direct provenance")


def expect_artifact_registry_compile_success(
    fixture: Path,
    result: subprocess.CompletedProcess[str],
) -> None:
    if result.returncode != 0:
        raise RuntimeError(
            "artifact registry key isolation compile failed for "
            f"{fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )


def expect_direct_artifact_registry_backend(selected_backend: str) -> None:
    expect(
        selected_backend_is_direct(selected_backend),
        "expected artifact registry key isolation to use direct-native backend",
    )


def expect_no_distinct_args_reuse(
    reuse_before: int,
    reuse_after_distinct_args: int,
) -> None:
    expect(
        registry_reuse_count_unchanged(reuse_before, reuse_after_distinct_args),
        "artifact registry reused stale outputs after bootstrap ordinal changed",
    )


def expect_same_args_reuse(
    reuse_before: int,
    reuse_after_same_args: int,
) -> None:
    expect(
        registry_reused_exactly_once(reuse_before, reuse_after_same_args),
        "artifact registry did not reuse immutable outputs when the key was unchanged",
    )


def expect_reuse_event_paths(
    reuse_event: dict[str, Any],
    *,
    producer_dir: Path,
    consumer_dir: Path,
) -> None:
    expect(
        reuse_event_matches_paths(
            reuse_event,
            producer_dir=producer_dir,
            consumer_dir=consumer_dir,
        ),
        "artifact registry reused from an unexpected producer or consumer directory",
    )


def expect_distinct_artifact_cache_keys(
    changed_args_key_a: str,
    changed_args_key_b: str,
    changed_import_surface_key_a: str,
    changed_import_surface_key_b: str,
) -> None:
    expect(
        artifact_cache_keys_are_distinct(
            changed_args_key_a,
            changed_args_key_b,
            changed_import_surface_key_a,
            changed_import_surface_key_b,
        ),
        "artifact registry key did not distinguish changed args or changed import surfaces",
    )
