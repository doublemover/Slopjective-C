"""Report shaping for LLVM capability probe summaries."""

from __future__ import annotations

from pathlib import Path
import platform
from typing import Iterable

from objc3c_tooling.paths import display_path

from .constants import MODE


def missing_contract_payload(
    *,
    program_surface_path: Path,
    showcase_portfolio_path: Path,
    failure: str,
) -> dict[str, object]:
    return {
        "program_surface": display_path(program_surface_path),
        "showcase_portfolio": display_path(showcase_portfolio_path),
        "drift_checks": {},
        "examples": [],
        "failures": [failure],
        "ok": False,
    }


def collect_failures(
    *,
    clang_probe: dict[str, object],
    clangxx_probe: dict[str, object],
    llc_probe: dict[str, object],
    llvm_ar_probe: dict[str, object],
    llvm_config_probe: dict[str, object],
    llc_features: dict[str, object],
    llvm_config_features: dict[str, object],
    sema_type_system_parity: dict[str, object],
    capability_demo_compatibility: dict[str, object],
) -> list[str]:
    failures: list[str] = []
    if not bool(clang_probe["found"]):
        failures.append(str(clang_probe.get("diagnostic", "clang executable missing")))
    if not bool(clangxx_probe["found"]):
        failures.append(str(clangxx_probe.get("diagnostic", "clang++ executable missing")))
    if not bool(llc_probe["found"]):
        failures.append(str(llc_probe.get("diagnostic", "llc executable missing")))
    if bool(llc_probe["found"]) and not bool(llc_features["supports_filetype_obj"]):
        failures.append("llc capability probe failed: --filetype=obj support not detected")
    if not bool(llvm_ar_probe["found"]):
        failures.append(str(llvm_ar_probe.get("diagnostic", "llvm-ar executable missing")))
    headers_libraries_discovered = bool(
        llvm_config_features.get("headers_libraries_discovered", False)
    )
    if not headers_libraries_discovered:
        if bool(llvm_config_probe["found"]):
            failures.append("llvm-config capability probe failed: headers/libs discovery unavailable")
        else:
            failures.append(
                "LLVM headers/libs discovery unavailable: llvm-config missing and "
                "installed LLVM include/lib directories were not discovered"
            )
    if not bool(sema_type_system_parity["parity_ready"]):
        failures.append(
            "sema/type-system parity capability unavailable: "
            + ", ".join(str(blocker) for blocker in sema_type_system_parity["blockers"])
        )
    for failure in capability_demo_compatibility.get("failures", []):
        failures.append(f"capability demo compatibility: {failure}")
    return failures


def _tool_record(tool_name: str, probe: dict[str, object]) -> dict[str, object]:
    return {
        "tool_name": tool_name,
        "configured_path": str(probe.get("configured_path", probe.get("path", ""))),
        "resolved_path": str(probe.get("resolved_path", "")),
        "shadowing_status": str(probe.get("shadowing_status", "unreported")),
        "found": bool(probe.get("found")),
        "version": str(probe.get("version", "")),
        "vendor": str(probe.get("vendor", "unknown")),
        "version_headline": str(probe.get("version_headline", "")),
        "diagnostic": str(probe.get("diagnostic", "")),
    }


def _capability_probe(
    *,
    probe_id: str,
    tool_name: str,
    feature: str,
    status: str,
    failure_reason: str = "",
) -> dict[str, object]:
    return {
        "probe_id": probe_id,
        "tool_name": tool_name,
        "feature": feature,
        "status": status,
        "failure_reason": failure_reason,
    }


def build_llvm_support_matrix(
    *,
    clang_probe: dict[str, object],
    clangxx_probe: dict[str, object],
    llc_probe: dict[str, object],
    llvm_ar_probe: dict[str, object],
    llvm_config_probe: dict[str, object],
    llc_features: dict[str, object],
    llvm_config_features: dict[str, object],
    sema_type_system_parity: dict[str, object],
) -> dict[str, object]:
    clang_record = _tool_record("clang", clang_probe)
    clangxx_record = _tool_record("clang++", clangxx_probe)
    llc_record = _tool_record("llc", llc_probe)
    llvm_ar_record = _tool_record("llvm-ar", llvm_ar_probe)
    llvm_config_record = _tool_record("llvm-config", llvm_config_probe)
    llc_found = bool(llc_probe.get("found"))
    llc_supports_obj = bool(llc_features.get("supports_filetype_obj", False))
    llvm_ar_found = bool(llvm_ar_probe.get("found"))
    headers_libraries_discovered = bool(
        llvm_config_features.get("headers_libraries_discovered", False)
    )
    discovery_source = str(llvm_config_features.get("discovery_source", "unavailable"))
    parity_ready = bool(sema_type_system_parity.get("parity_ready", False))
    clangxx_ready = bool(clangxx_probe.get("found"))
    package_capability_ready = parity_ready and llvm_ar_found and headers_libraries_discovered
    native_execution_ready = parity_ready and clangxx_ready and headers_libraries_discovered
    native_object_emission_status = (
        "native_object_emission_supported"
        if llc_found and llc_supports_obj
        else (
            "native_object_emission_missing_llc"
            if not llc_found
            else "native_object_emission_filetype_obj_unavailable"
        )
    )
    supported_features: list[str] = []
    rejected_features: list[dict[str, str]] = []

    if bool(clang_probe.get("found")):
        supported_features.append("semantic-diagnostics")
    else:
        rejected_features.append(
            {
                "feature": "semantic-diagnostics",
                "reason": str(clang_probe.get("diagnostic", "clang executable missing")),
            }
        )

    if clangxx_ready:
        supported_features.append("native-runtime-link-driver")
    else:
        rejected_features.append(
            {
                "feature": "native-runtime-link-driver",
                "reason": str(clangxx_probe.get("diagnostic", "clang++ executable missing")),
            }
        )

    if bool(llc_probe.get("found")) and llc_supports_obj:
        supported_features.append("llvm-direct-object-emission")
    else:
        rejected_features.append(
            {
                "feature": "llvm-direct-object-emission",
                "reason": (
                    str(llc_probe.get("diagnostic", "llc executable missing"))
                    if not bool(llc_probe.get("found"))
                    else "llc missing --filetype=obj support"
                ),
            }
        )

    if llvm_ar_found:
        supported_features.append("package-archive-tool")
    else:
        rejected_features.append(
            {
                "feature": "package-archive-tool",
                "reason": str(llvm_ar_probe.get("diagnostic", "llvm-ar executable missing")),
            }
        )

    if headers_libraries_discovered:
        supported_features.append("headers-libraries-discovery")
    else:
        rejected_features.append(
            {
                "feature": "headers-libraries-discovery",
                "reason": (
                    "installed LLVM include/lib directories were not discovered"
                    if discovery_source == "unavailable"
                    else "llvm-config did not publish both --includedir and --libdir"
                ),
            }
        )

    if parity_ready:
        if package_capability_ready:
            supported_features.append("package-capability-probe")
        else:
            rejected_features.append(
                {
                    "feature": "package-capability-probe",
                    "reason": (
                        "package capability requires llvm-ar and LLVM headers/libs discovery"
                    ),
                }
            )
        if native_execution_ready:
            supported_features.append("native-execution-capability-probe")
        else:
            rejected_features.append(
                {
                    "feature": "native-execution-capability-probe",
                    "reason": (
                        "native execution capability requires clang++ and LLVM headers/libs discovery"
                    ),
                }
            )
    else:
        blocker_text = ", ".join(
            str(blocker) for blocker in sema_type_system_parity.get("blockers", [])
        )
        rejected_features.extend(
            [
                {
                    "feature": "package-capability-probe",
                    "reason": blocker_text or "sema/type-system parity unavailable",
                },
                {
                    "feature": "native-execution-capability-probe",
                    "reason": blocker_text or "sema/type-system parity unavailable",
                },
            ]
        )

    return {
        "contract_id": "objc3c.llvm.version_support_matrix.v1",
        "schema_version": 1,
        "issue_ref": 8232,
        "native_object_emission_contract": {
            "contract_id": "objc3c.llvm.native-object-emission.fail-closed.v1",
            "issue_ref": 8232,
            "required_tool": "llc",
            "required_probe": "llc --filetype=obj",
            "status": native_object_emission_status,
            "missing_llc_status": "native_object_emission_missing_llc",
            "missing_filetype_status": "native_object_emission_filetype_obj_unavailable",
            "hosted_runner_behavior": "fail-closed-no-native-object-success-claim",
            "conformance_minima_behavior": "fail-closed-before-cross-lane-runtime-proof",
            "fallback_policy": "no-clang-fallback-success-claim",
        },
        "host_platform": {
            "system": platform.system().lower(),
            "machine": platform.machine().lower(),
            "python": platform.python_version(),
        },
        "llvm_tool_records": [
            clang_record,
            clangxx_record,
            llc_record,
            llvm_ar_record,
            llvm_config_record,
        ],
        "llvm_capability_probes": [
            _capability_probe(
                probe_id="objc3c.llvm.capability.clang.semantic-diagnostics",
                tool_name="clang",
                feature="semantic-diagnostics",
                status="supported" if bool(clang_probe.get("found")) else "rejected",
                failure_reason=str(clang_probe.get("diagnostic", "")),
            ),
            _capability_probe(
                probe_id="objc3c.llvm.capability.llc.object-emission",
                tool_name="llc",
                feature="llvm-direct-object-emission",
                status="supported"
                if bool(llc_probe.get("found")) and llc_supports_obj
                else "rejected",
                failure_reason=(
                    ""
                    if bool(llc_probe.get("found")) and llc_supports_obj
                    else (
                        str(llc_probe.get("diagnostic", "llc executable missing"))
                        if not bool(llc_probe.get("found"))
                        else "llc missing --filetype=obj support"
                    )
                ),
            ),
            _capability_probe(
                probe_id="objc3c.llvm.capability.clangxx.native-link",
                tool_name="clang++",
                feature="native-runtime-link-driver",
                status="supported" if clangxx_ready else "rejected",
                failure_reason=str(clangxx_probe.get("diagnostic", "")),
            ),
            _capability_probe(
                probe_id="objc3c.llvm.capability.llvm-ar.package-archive",
                tool_name="llvm-ar",
                feature="package-archive-tool",
                status="supported" if llvm_ar_found else "rejected",
                failure_reason=str(llvm_ar_probe.get("diagnostic", "")),
            ),
            _capability_probe(
                probe_id="objc3c.llvm.capability.llvm-config.headers-libs",
                tool_name="llvm-config",
                feature="headers-libraries-discovery",
                status="supported" if headers_libraries_discovered else "rejected",
                failure_reason=(
                    ""
                    if headers_libraries_discovered
                    else (
                        "installed LLVM include/lib directories were not discovered"
                        if discovery_source == "unavailable"
                        else "llvm-config did not publish both --includedir and --libdir"
                    )
                ),
            ),
        ],
        "toolchain_matrix_entries": [
            {
                "entry_id": "objc3c.llvm.host.current",
                "host_platform": f"{platform.system().lower()}-{platform.machine().lower()}",
                "llvm_version": str(llc_probe.get("version") or clang_probe.get("version") or ""),
                "clang_version": str(clang_probe.get("version", "")),
                "clangxx_version": str(clangxx_probe.get("version", "")),
                "llc_version": str(llc_probe.get("version", "")),
                "llvm_ar_version": str(llvm_ar_probe.get("version", "")),
                "llvm_config_version": str(llvm_config_probe.get("version", "")),
                "support_status": "supported"
                if parity_ready and package_capability_ready and native_execution_ready
                else "rejected",
                "object_emission_capability": "supported" if llc_supports_obj else "rejected",
                "package_capability": "supported" if package_capability_ready else "rejected",
                "native_execution_capability": "supported" if native_execution_ready else "rejected",
                "supported_features": supported_features,
                "rejected_features": rejected_features,
                "unsupported_version_behavior": "fail-closed-no-range-claim",
            }
        ],
        "toolchain_resolution": {
            "clang": {
                "configured_path": clang_record["configured_path"],
                "resolved_path": clang_record["resolved_path"],
                "shadowing_status": clang_record["shadowing_status"],
                "diagnostic": clang_record["diagnostic"],
            },
            "llc": {
                "configured_path": llc_record["configured_path"],
                "resolved_path": llc_record["resolved_path"],
                "shadowing_status": llc_record["shadowing_status"],
                "diagnostic": llc_record["diagnostic"],
            },
            "clang++": {
                "configured_path": clangxx_record["configured_path"],
                "resolved_path": clangxx_record["resolved_path"],
                "shadowing_status": clangxx_record["shadowing_status"],
                "diagnostic": clangxx_record["diagnostic"],
            },
            "llvm-ar": {
                "configured_path": llvm_ar_record["configured_path"],
                "resolved_path": llvm_ar_record["resolved_path"],
                "shadowing_status": llvm_ar_record["shadowing_status"],
                "diagnostic": llvm_ar_record["diagnostic"],
            },
            "llvm-config": {
                "configured_path": llvm_config_record["configured_path"],
                "resolved_path": llvm_config_record["resolved_path"],
                "shadowing_status": llvm_config_record["shadowing_status"],
                "diagnostic": llvm_config_record["diagnostic"],
                "includedir": str(llvm_config_features.get("includedir", "")),
                "libdir": str(llvm_config_features.get("libdir", "")),
                "headers_libraries_discovered": headers_libraries_discovered,
                "headers_libraries_discovery_source": discovery_source,
            },
        },
    }


def build_summary(
    *,
    clang_probe: dict[str, object],
    clangxx_probe: dict[str, object],
    llc_probe: dict[str, object],
    llvm_ar_probe: dict[str, object],
    llvm_config_probe: dict[str, object],
    llc_features: dict[str, object],
    llvm_config_features: dict[str, object],
    sema_type_system_parity: dict[str, object],
    capability_demo_compatibility: dict[str, object],
    failures: Iterable[str],
) -> dict[str, object]:
    failure_list = list(failures)
    llvm_support_matrix = build_llvm_support_matrix(
        clang_probe=clang_probe,
        clangxx_probe=clangxx_probe,
        llc_probe=llc_probe,
        llvm_ar_probe=llvm_ar_probe,
        llvm_config_probe=llvm_config_probe,
        llc_features=llc_features,
        llvm_config_features=llvm_config_features,
        sema_type_system_parity=sema_type_system_parity,
    )
    return {
        "mode": MODE,
        "clang": clang_probe,
        "clangxx": clangxx_probe,
        "llc": llc_probe,
        "llvm_ar": llvm_ar_probe,
        "llvm_config": llvm_config_probe,
        "llc_features": llc_features,
        "llvm_config_features": llvm_config_features,
        "llvm_support_matrix": llvm_support_matrix,
        "toolchain_resolution": llvm_support_matrix["toolchain_resolution"],
        "sema_type_system_parity": sema_type_system_parity,
        "capability_demo_compatibility": capability_demo_compatibility,
        "failures": failure_list,
        "ok": not failure_list,
    }
