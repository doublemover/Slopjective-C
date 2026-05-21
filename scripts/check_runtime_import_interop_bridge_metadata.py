#!/usr/bin/env python3
"""Validate active interop bridge metadata in runtime import surfaces."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]

BRIDGE_MEMBER = "objc_interop_header_module_and_bridge_generation"
FOREIGN_MEMBER = "objc_interop_foreign_surface_interface_and_module_preservation"
FFI_MEMBER = "objc_interop_ffi_metadata_interface_preservation"
BRIDGE_SURFACES_MEMBER = "bridge_surfaces"
UNSUPPORTED_TOPOLOGIES_MEMBER = "unsupported_topologies"

CANONICAL_HEADER = "module.interop-bridge.h"
CANONICAL_MODULE = "module.interop-bridge.modulemap"
CANONICAL_BRIDGE = "module.interop-bridge.json"
REQUIRED_BRIDGE_LANGUAGES = {"c", "objc2", "swift", "cpp"}
REQUIRED_UNSUPPORTED_TOPOLOGIES = {
    "objc2-source-compatibility",
    "swift-full-abi-callable-import",
    "cpp-template-instantiation-import",
    "unchecked-abi-alignment-fallback",
}
REPLAYABLE_COMMAND_PREFIX = "npm run objc3c -- "


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


def _is_portable_relative_path(value: object) -> bool:
    if not isinstance(value, str) or not value:
        return False
    path = Path(value)
    if path.is_absolute() or "\\" in value:
        return False
    return all(part not in {"", ".", ".."} for part in path.parts)


def _validate_existing_anchor(value: object, label: str, failures: list[str]) -> None:
    if not _is_portable_relative_path(value):
        failures.append(f"{label} is not portable-relative")
        return
    if not (ROOT / str(value)).is_file():
        failures.append(f"{label} does not exist")


def _as_list(value: object) -> list[Any]:
    return value if isinstance(value, list) else []


def _as_object(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _is_power_of_two(value: int) -> bool:
    return value > 0 and value & (value - 1) == 0


def _validate_bridge_surface(
    surface: dict[str, Any],
    seen_ids: set[str],
    failures: list[str],
) -> int:
    language = str(surface.get("language", ""))
    surface_id = str(surface.get("surface_id", ""))
    support_state = str(surface.get("support_state", ""))
    bridge_kind = str(surface.get("bridge_kind", ""))

    if language not in REQUIRED_BRIDGE_LANGUAGES:
        failures.append(f"bridge surface has unsupported language: {language}")
    if not surface_id or surface_id in seen_ids:
        failures.append(f"{language} bridge surface id is missing or duplicated")
    seen_ids.add(surface_id)
    if support_state not in {"supported", "reserved", "rejected"}:
        failures.append(f"{language} bridge surface support state is invalid")
    if support_state != "supported" and surface.get("support_claim"):
        failures.append(f"{language} unsupported bridge surface publishes a support claim")
    if surface.get("fail_closed") is not True:
        failures.append(f"{language} bridge surface must fail closed")
    if "fallback" in bridge_kind:
        failures.append(f"{language} bridge surface cannot be modeled as a fallback")

    _validate_existing_anchor(surface.get("source_fixture"), f"{language} source fixture", failures)
    if not isinstance(surface.get("source_range"), str) or not surface.get("source_range"):
        failures.append(f"{language} bridge surface source range is missing")

    callables = [value for value in _as_list(surface.get("callable_symbols")) if isinstance(value, str) and value]
    if not callables:
        failures.append(f"{language} bridge surface has no callable symbols")

    abi = _as_object(surface.get("abi"))
    alignment = abi.get("alignment")
    if not isinstance(alignment, int) or not _is_power_of_two(alignment):
        failures.append(f"{language} bridge surface ABI alignment must be a power of two")
    for key in ("calling_convention", "ownership", "error_model", "async_model", "object_identity"):
        if not isinstance(abi.get(key), str) or not abi.get(key) or abi.get(key) == "implicit":
            failures.append(f"{language} bridge surface ABI {key} is not explicit")

    linkage = _as_object(surface.get("linkage"))
    if not isinstance(linkage.get("symbol_owner"), str) or not linkage.get("symbol_owner"):
        failures.append(f"{language} bridge surface symbol owner is missing")
    for key in ("symbol", "linkage_kind", "visibility"):
        if not isinstance(linkage.get(key), str) or not linkage.get(key):
            failures.append(f"{language} bridge surface linkage {key} is missing")
    if linkage.get("header_artifact") != CANONICAL_HEADER:
        failures.append(f"{language} bridge surface header linkage is not canonical")
    if linkage.get("module_artifact") != CANONICAL_MODULE:
        failures.append(f"{language} bridge surface module linkage is not canonical")

    evidence = _as_object(surface.get("evidence_contract"))
    anchors = _as_list(evidence.get("anchors"))
    commands = _as_list(evidence.get("public_commands"))
    if support_state == "supported":
        support_claim = evidence.get("support_claim")
        if not isinstance(support_claim, str) or not support_claim.startswith("objc3c.behavior."):
            failures.append(f"{language} supported bridge surface has no behavior support claim")
        if not anchors:
            failures.append(f"{language} supported bridge surface has no evidence anchors")
        if not commands:
            failures.append(f"{language} supported bridge surface has no replayable public commands")
    else:
        if anchors or commands:
            failures.append(f"{language} unsupported bridge surface cannot publish executable evidence")
    for anchor in anchors:
        _validate_existing_anchor(anchor, f"{language} evidence anchor", failures)
    for command in commands:
        if not isinstance(command, str) or not command.startswith(REPLAYABLE_COMMAND_PREFIX):
            failures.append(f"{language} evidence command is not replayable public command")

    return len(callables)


def _validate_bridge_surfaces(bridge: dict[str, Any], failures: list[str]) -> None:
    surfaces = [_as_object(surface) for surface in _as_list(bridge.get(BRIDGE_SURFACES_MEMBER))]
    if not surfaces:
        failures.append("active bridge packet has no concrete bridge surfaces")
        return

    languages = {str(surface.get("language", "")) for surface in surfaces}
    if languages != REQUIRED_BRIDGE_LANGUAGES:
        failures.append("active bridge packet does not cover C, ObjC2, Swift, and C++ bridge surfaces")

    seen_ids: set[str] = set()
    derived_callable_count = 0
    for surface in surfaces:
        derived_callable_count += _validate_bridge_surface(surface, seen_ids, failures)

    if bridge.get("local_foreign_callable_count") != derived_callable_count:
        failures.append("active bridge packet callable count is not derived from bridge surfaces")

    swift_callable_count = sum(
        len(_as_list(surface.get("callable_symbols")))
        for surface in surfaces
        if surface.get("language") == "swift"
    )
    cpp_callable_count = sum(
        len(_as_list(surface.get("callable_symbols")))
        for surface in surfaces
        if surface.get("language") == "cpp"
    )
    if bridge.get("local_swift_name_annotation_count", 0) < swift_callable_count:
        failures.append("active bridge packet undercounts Swift bridge callable metadata")
    if bridge.get("local_cpp_name_annotation_count", 0) < cpp_callable_count:
        failures.append("active bridge packet undercounts C++ bridge callable metadata")


def _validate_unsupported_topologies(bridge: dict[str, Any], failures: list[str]) -> None:
    topologies = [_as_object(entry) for entry in _as_list(bridge.get(UNSUPPORTED_TOPOLOGIES_MEMBER))]
    ids = {str(entry.get("topology_id", "")) for entry in topologies}
    if not REQUIRED_UNSUPPORTED_TOPOLOGIES <= ids:
        failures.append("active bridge packet has incomplete unsupported topology rejection metadata")
    if len(ids) != len(topologies):
        failures.append("active bridge packet has duplicated unsupported topology rejection metadata")
    for entry in topologies:
        topology_id = str(entry.get("topology_id", ""))
        if entry.get("public_state") != "rejected":
            failures.append(f"{topology_id} unsupported topology must be rejected")
        if entry.get("fallback_allowed") is not False:
            failures.append(f"{topology_id} unsupported topology allows fallback")
        diagnostic = entry.get("diagnostic")
        if not isinstance(diagnostic, str) or not diagnostic.startswith("O3"):
            failures.append(f"{topology_id} unsupported topology has no stable diagnostic")


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
    _validate_bridge_surfaces(bridge, failures)
    _validate_unsupported_topologies(bridge, failures)
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
