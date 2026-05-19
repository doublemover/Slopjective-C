"""Assertions for the runnable storage/reflection E2E checker."""

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
    box_entry = probe_payload.get("box_entry", {})
    implementation_surface = probe_payload.get("implementation_surface", {})
    current_value = probe_payload.get("current_value_property", {})
    weak_value = probe_payload.get("weak_value_property", {})
    guarded_value = probe_payload.get("guarded_value_property", {})

    expect(box_entry.get("found") == 1, "expected packaged Box class lookup to succeed")
    expect(
        box_entry.get("runtime_property_accessor_count", 0) >= 5,
        "expected packaged Box to expose five runtime-backed accessors",
    )
    expect(
        box_entry.get("runtime_instance_size_bytes", 0) >= 40,
        "expected packaged Box instance layout to preserve five storage slots",
    )
    expect(
        implementation_surface.get("property_registry_ready") == 1,
        "expected packaged implementation snapshot to report property registry readiness",
    )
    expect(
        implementation_surface.get("runtime_accessor_dispatch_ready") == 1,
        "expected packaged implementation snapshot to report accessor dispatch readiness",
    )
    expect(
        implementation_surface.get("reflection_query_ready") == 1,
        "expected packaged implementation snapshot to report reflection readiness",
    )
    expect(
        implementation_surface.get("deterministic") == 1,
        "expected packaged implementation snapshot to report deterministic handoff",
    )
    expect(
        current_value.get("found") == 1,
        "expected packaged currentValue property lookup to succeed",
    )
    expect(
        current_value.get("has_runtime_getter") == 1
        and current_value.get("has_runtime_setter") == 1,
        "expected packaged currentValue accessors to stay runtime-backed",
    )
    expect(
        weak_value.get("ownership_runtime_hook_profile") == "objc-weak-side-table",
        "expected packaged weakValue hook profile to preserve objc weak side-table handling",
    )
    expect(
        guarded_value.get("ownership_runtime_hook_profile") == "objc-unowned-safe-guard",
        "expected packaged guardedValue hook profile to preserve safe-unowned guarding",
    )


__all__ = [
    "expect",
    "expect_compile_artifacts",
    "expect_manifest_contract",
    "expect_manifest_files",
    "expect_probe_payload",
]
