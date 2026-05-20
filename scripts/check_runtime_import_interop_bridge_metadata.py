#!/usr/bin/env python3
"""Validate active interop bridge metadata in runtime import surfaces."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

BRIDGE_MEMBER = "objc_interop_header_module_and_bridge_generation"
FOREIGN_MEMBER = "objc_interop_foreign_surface_interface_and_module_preservation"
FFI_MEMBER = "objc_interop_ffi_metadata_interface_preservation"

CANONICAL_HEADER = "module.interop-bridge.h"
CANONICAL_MODULE = "module.interop-bridge.modulemap"
CANONICAL_BRIDGE = "module.interop-bridge.json"


def _is_ready(surface: dict[str, Any], *keys: str) -> bool:
    return all(surface.get(key) is True for key in keys)


def _validate_artifact_path(
    value: object,
    expected: str,
    label: str,
    failures: list[str],
) -> None:
    if not isinstance(value, str) or not value:
        failures.append(f"{label} artifact path is missing")
        return
    path = Path(value)
    if path.is_absolute() or "\\" in value:
        failures.append(f"{label} artifact path is not portable-relative")
    if any(part in {".", ".."} for part in path.parts):
        failures.append(f"{label} artifact path traverses directories")
    if value != expected:
        failures.append(f"{label} artifact path is not canonical")


def validate_import_surface(payload: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    bridge = payload.get(BRIDGE_MEMBER)
    if not isinstance(bridge, dict):
        return failures
    if not _is_ready(
        bridge,
        "runtime_generation_ready",
        "cross_module_packaging_ready",
    ):
        return failures

    foreign = payload.get(FOREIGN_MEMBER)
    ffi = payload.get(FFI_MEMBER)
    if not isinstance(foreign, dict) or not _is_ready(
        foreign,
        "runtime_import_artifact_ready",
        "separate_compilation_preservation_ready",
        "deterministic",
    ):
        failures.append("active bridge packet has no ready foreign surface packet")
    if not isinstance(ffi, dict) or not _is_ready(
        ffi,
        "runtime_import_artifact_ready",
        "separate_compilation_preservation_ready",
        "deterministic",
    ):
        failures.append("active bridge packet has no ready ffi preservation packet")

    bridge_callables = bridge.get("local_foreign_callable_count")
    if not isinstance(bridge_callables, int) or bridge_callables <= 0:
        failures.append("active bridge packet has no foreign callable metadata")
    elif isinstance(foreign, dict) and isinstance(ffi, dict):
        if foreign.get("local_foreign_callable_count") != bridge_callables:
            failures.append("bridge foreign callable count differs from foreign packet")
        if ffi.get("local_foreign_callable_count") != bridge_callables:
            failures.append("bridge foreign callable count differs from ffi packet")

    if not bridge.get("deterministic"):
        failures.append("active bridge packet is not deterministic")
    if not bridge.get("replay_key") or not bridge.get("preservation_replay_key"):
        failures.append("active bridge packet has incomplete replay metadata")
    if not bridge.get("local_import_module_name_count"):
        failures.append("active bridge packet has no import-module metadata")
    if bridge.get("local_header_name_annotation_count", 0) < (bridge_callables or 0):
        failures.append("active bridge packet does not cover every header annotation")
    if (
        bridge.get("local_cpp_name_annotation_count", 0) == 0
        and bridge.get("local_swift_name_annotation_count", 0) == 0
    ):
        failures.append("active bridge packet has no C++ or Swift-facing metadata")

    _validate_artifact_path(
        bridge.get("header_artifact_relative_path"),
        CANONICAL_HEADER,
        "header",
        failures,
    )
    _validate_artifact_path(
        bridge.get("module_artifact_relative_path"),
        CANONICAL_MODULE,
        "module",
        failures,
    )
    _validate_artifact_path(
        bridge.get("bridge_artifact_relative_path"),
        CANONICAL_BRIDGE,
        "bridge",
        failures,
    )
    return failures


def main(argv: list[str]) -> int:
    if not argv:
        print("usage: check_runtime_import_interop_bridge_metadata.py <surface.json>...")
        return 2
    all_failures: list[str] = []
    for raw_path in argv:
        path = Path(raw_path)
        payload = json.loads(path.read_text(encoding="utf-8"))
        failures = validate_import_surface(payload)
        all_failures.extend(f"{path}: {failure}" for failure in failures)
    if all_failures:
        print("\n".join(all_failures), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
