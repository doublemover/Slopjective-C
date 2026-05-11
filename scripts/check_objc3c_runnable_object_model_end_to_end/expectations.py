"""Assertions for the runnable object-model E2E checker."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import normalize_rel_path

from .config import MANIFEST_KEYS, PACKAGE_CONTRACT_ID
from .models import CompileArtifacts


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def expect_manifest_contract(manifest: dict[str, Any]) -> None:
    expect(
        manifest.get("contract_id") == PACKAGE_CONTRACT_ID,
        "runnable toolchain package manifest published the wrong contract id",
    )


def expect_manifest_files(manifest: dict[str, Any], package_root: Path) -> None:
    for manifest_key in MANIFEST_KEYS:
        relative_path = manifest.get(manifest_key)
        expect(
            isinstance(relative_path, str) and relative_path,
            f"package manifest did not publish {manifest_key}",
        )
        candidate = package_root / normalize_rel_path(relative_path)
        expect(
            candidate.is_file(),
            f"packaged runnable toolchain missing {manifest_key} at {relative_path}",
        )


def expect_compile_artifacts(compile_artifacts: CompileArtifacts) -> None:
    for artifact_path in compile_artifacts.as_dict().values():
        expect(
            artifact_path.is_file(),
            f"packaged compile wrapper did not publish {artifact_path}",
        )


def expect_probe_payload(probe_payload: dict[str, Any]) -> None:
    aggregate = probe_payload.get("aggregate", {})
    expect(
        probe_payload.get("widget_found") == 1,
        "expected packaged Widget class lookup to succeed",
    )
    expect(
        probe_payload.get("traced_value") == 13,
        "expected packaged tracedValue to return 13",
    )
    expect(
        probe_payload.get("count_value") == 41,
        "expected packaged count to reload 41",
    )
    expect(
        probe_payload.get("count_property_found") == 1,
        "expected packaged count property lookup to succeed",
    )
    expect(
        probe_payload.get("tracer_conforms") == 1,
        "expected packaged Widget to conform to Tracer",
    )
    expect(
        aggregate.get("realized_class_count") == 2,
        "expected packaged aggregate realized class count to report two live classes",
    )
    expect(
        aggregate.get("reflectable_property_count") == 4,
        "expected packaged aggregate property count to report four live accessors",
    )
    expect(
        aggregate.get("attached_category_count") == 1,
        "expected packaged aggregate category count to report one attached category",
    )
    expect(
        aggregate.get("last_queried_class_name") == "Widget",
        "expected packaged aggregate class lookup to preserve Widget",
    )
    expect(
        aggregate.get("last_queried_property_name") == "count",
        "expected packaged aggregate property lookup to preserve count",
    )
    expect(
        aggregate.get("last_queried_protocol_name") == "Tracer",
        "expected packaged aggregate protocol lookup to preserve Tracer",
    )


__all__ = [
    "expect",
    "expect_compile_artifacts",
    "expect_manifest_contract",
    "expect_manifest_files",
    "expect_probe_payload",
]
