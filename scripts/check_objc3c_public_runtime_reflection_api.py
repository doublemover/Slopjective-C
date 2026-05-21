#!/usr/bin/env python3
"""Validate the public runtime reflection C API contract."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object, write_json_file
from objc3c_tooling.paths import ROOT, repo_rel


CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "objc3c"
    / "public_runtime_reflection_api_contract.json"
)
HEADER_PATH = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "runtime"
    / "public"
    / "objc3_runtime_reflection.h"
)
UMBRELLA_PATH = HEADER_PATH.with_name("objc3_runtime_api.h")
IMPLEMENTATION_PATH = HEADER_PATH.with_suffix(".cpp")
CMAKE_PATH = ROOT / "native" / "objc3c" / "src" / "runtime" / "CMakeLists.txt"
PROBE_PATH = ROOT / "tests" / "tooling" / "runtime" / "public_runtime_reflection_api_probe.cpp"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "public-runtime-reflection-api.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _require_contains(
    failures: list[str],
    label: str,
    text: str,
    token: str,
) -> None:
    if token not in text:
        failures.append(f"{label} is missing token: {token}")


def _require_absent(
    failures: list[str],
    label: str,
    text: str,
    token: str,
) -> None:
    if token in text:
        failures.append(f"{label} must not contain token: {token}")


def validate_public_runtime_reflection_api() -> dict[str, Any]:
    failures: list[str] = []
    contract = require_json_object(CONTRACT_PATH)
    header = _read(HEADER_PATH)
    umbrella = _read(UMBRELLA_PATH)
    implementation = _read(IMPLEMENTATION_PATH)
    cmake = _read(CMAKE_PATH)
    probe = _read(PROBE_PATH)

    if contract.get("contract_id") != "objc3c.runtime.public.reflection.api.v1":
        failures.append("public runtime reflection contract_id drifted")
    if contract.get("issue") != 8174:
        failures.append("public runtime reflection issue binding drifted")
    if contract.get("support_claim") != "objc3c.behavior.runtime.public-reflection-api":
        failures.append("public runtime reflection support claim drifted")

    _require_contains(
        failures,
        repo_rel(UMBRELLA_PATH),
        umbrella,
        '#include "runtime/public/objc3_runtime_reflection.h"',
    )
    _require_contains(
        failures,
        repo_rel(CMAKE_PATH),
        cmake,
        "public/objc3_runtime_reflection.cpp",
    )
    _require_contains(
        failures,
        repo_rel(CMAKE_PATH),
        cmake,
        "public/objc3_runtime_reflection.h",
    )
    _require_contains(
        failures,
        repo_rel(HEADER_PATH),
        header,
        "OBJC3_RUNTIME_REFLECTION_ABI_VERSION",
    )

    entrypoints = contract.get("entrypoints", [])
    if not isinstance(entrypoints, list) or not entrypoints:
        failures.append("public runtime reflection entrypoints are missing")
        entrypoints = []
    for raw_symbol in entrypoints:
        symbol = str(raw_symbol)
        _require_contains(failures, repo_rel(HEADER_PATH), header, f"{symbol}(")
        _require_contains(
            failures,
            repo_rel(IMPLEMENTATION_PATH),
            implementation,
            f"{symbol}(",
        )
        _require_contains(failures, repo_rel(PROBE_PATH), probe, f"{symbol}(")

    snapshot_types = contract.get("snapshot_types", [])
    if not isinstance(snapshot_types, list) or not snapshot_types:
        failures.append("public runtime reflection snapshot types are missing")
        snapshot_types = []
    for raw_snapshot in snapshot_types:
        snapshot = str(raw_snapshot)
        _require_contains(
            failures,
            repo_rel(HEADER_PATH),
            header,
            f"typedef struct {snapshot}",
        )

    status_codes = contract.get("status_codes", [])
    if not isinstance(status_codes, list) or not status_codes:
        failures.append("public runtime reflection status codes are missing")
        status_codes = []
    for raw_status in status_codes:
        status = str(raw_status)
        _require_contains(failures, repo_rel(HEADER_PATH), header, status)
        _require_contains(
            failures,
            repo_rel(IMPLEMENTATION_PATH),
            implementation,
            status,
        )

    deterministic_enumeration = contract.get("deterministic_enumeration", [])
    if (
        not isinstance(deterministic_enumeration, list)
        or len(deterministic_enumeration) != 4
    ):
        failures.append(
            "public runtime reflection deterministic enumeration contract drifted"
        )
    _require_contains(
        failures,
        repo_rel(HEADER_PATH),
        header,
        "OBJC3_RUNTIME_REFLECTION_ABI_VERSION 3u",
    )

    for raw_symbol in contract.get("forbidden_public_symbols", []):
        _require_absent(
            failures,
            repo_rel(HEADER_PATH),
            header,
            str(raw_symbol),
        )

    for token in (
        "ProcessRuntimeState()",
        "std::lock_guard<std::mutex> lock(state.mutex)",
        "state.realized_class_nodes",
        "state.realized_class_nodes[static_cast<std::size_t>(index)]",
        "node->runtime_property_accessors[static_cast<std::size_t>(index)]",
        "node.attached_category_records",
        "node->attached_category_records[static_cast<std::size_t>(index)]",
        "state.selector_slots[static_cast<std::size_t>(index)]",
        "FindRuntimePropertyAccessorByNameUnlocked",
        "ProtocolExistsByNameUnlocked",
        "QueryRealizedClassProtocolConformanceUnlocked",
        "FindSelectorSlotByCanonicalSpellingUnlocked",
        "PublicRuntimeReflectionSurfaceRecord",
        "snapshot.issue_ref = 8174",
        "snapshot.creates_dynamic_runtime_state = 0",
        "snapshot.exposes_private_testing_snapshot = 0",
        "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT",
        "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY",
        "OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA",
        "kUnsupportedMetadataPolicy",
    ):
        _require_contains(failures, repo_rel(IMPLEMENTATION_PATH), implementation, token)

    for forbidden in (
        "AppendDynamicSelectorSlotUnlocked",
        "LookupSelectorUnlocked(selector)",
        "owner_identity;",
    ):
        _require_absent(
            failures,
            repo_rel(IMPLEMENTATION_PATH) if forbidden != "owner_identity;" else repo_rel(HEADER_PATH),
            implementation if forbidden != "owner_identity;" else header,
            forbidden,
        )

    _require_contains(
        failures,
        repo_rel(PROBE_PATH),
        probe,
        '#include "runtime/public/objc3_runtime_api.h"',
    )
    _require_absent(failures, repo_rel(PROBE_PATH), probe, "_for_testing")

    return {
        "contract_id": "objc3c.runtime.public.reflection.api.validation.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract_path": repo_rel(CONTRACT_PATH),
        "header_path": repo_rel(HEADER_PATH),
        "implementation_path": repo_rel(IMPLEMENTATION_PATH),
        "probe_path": repo_rel(PROBE_PATH),
        "support_claim": contract.get("support_claim"),
        "entrypoint_count": len(entrypoints),
        "snapshot_type_count": len(snapshot_types),
        "status_code_count": len(status_codes),
        "failures": failures,
    }


def main() -> int:
    report = validate_public_runtime_reflection_api()
    write_json_file(REPORT_PATH, report)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print(f"objc3c-public-runtime-reflection-api: {report['status']}")
    for failure in report["failures"]:
        print(f"failure: {failure}")
    return 0 if report["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
