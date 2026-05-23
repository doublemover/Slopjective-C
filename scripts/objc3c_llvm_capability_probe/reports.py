"""Report shaping for LLVM capability probe summaries."""

from __future__ import annotations

from pathlib import Path
import platform
import re
from typing import Iterable

from objc3c_tooling.paths import display_path

from .constants import MODE

MIN_SUPPORTED_LLVM_VERSION = (19, 1)


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
    toolchain_identity: dict[str, object],
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
    if not bool(toolchain_identity.get("claimable", False)):
        diagnostics = [
            str(item)
            for item in toolchain_identity.get("diagnostics", [])
            if str(item)
        ]
        failures.append(
            "LLVM toolchain identity rejected: "
            + ("; ".join(diagnostics) if diagnostics else "coherent root/version proof unavailable")
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


def _normalized_root(path: Path) -> str:
    return path.resolve().as_posix().rstrip("/").lower()


def _tool_install_root(record: dict[str, object]) -> str:
    resolved = str(record.get("resolved_path", ""))
    if not resolved:
        return ""
    path = Path(resolved)
    if path.parent.name.lower() != "bin":
        return ""
    return _normalized_root(path.parent.parent)


def _headers_libraries_root(features: dict[str, object]) -> str:
    if not bool(features.get("headers_libraries_discovered", False)):
        return ""
    includedir = str(features.get("includedir", ""))
    libdir = str(features.get("libdir", ""))
    if not includedir or not libdir:
        return ""
    include_path = Path(includedir)
    lib_path = Path(libdir)
    if include_path.name.lower() != "include" or lib_path.name.lower() != "lib":
        return ""
    try:
        if include_path.parent.resolve() != lib_path.parent.resolve():
            return ""
    except OSError:
        return ""
    return _normalized_root(include_path.parent)


def _configured_path_is_absolute(record: dict[str, object]) -> bool:
    configured = str(record.get("configured_path", record.get("path", "")))
    return bool(configured) and Path(configured).is_absolute()


def _version_family(version: str) -> tuple[int, int] | None:
    match = re.match(r"^\s*([0-9]+)(?:\.([0-9]+))?", version)
    if not match:
        return None
    return int(match.group(1)), int(match.group(2) or 0)


def build_toolchain_identity(
    *,
    clang_probe: dict[str, object],
    clangxx_probe: dict[str, object],
    llc_probe: dict[str, object],
    llvm_ar_probe: dict[str, object],
    llvm_config_probe: dict[str, object],
    llvm_config_features: dict[str, object],
) -> dict[str, object]:
    tool_records = [
        ("clang", clang_probe),
        ("clang++", clangxx_probe),
        ("llc", llc_probe),
        ("llvm-ar", llvm_ar_probe),
    ]
    if bool(llvm_config_probe.get("found")):
        tool_records.append(("llvm-config", llvm_config_probe))

    root_records: list[dict[str, object]] = []
    for tool_name, record in tool_records:
        root = _tool_install_root(record)
        if root:
            root_records.append(
                {
                    "tool_name": tool_name,
                    "install_root": root,
                    "authority": str(record.get("shadowing_status", "unreported")),
                    "configured_absolute": _configured_path_is_absolute(record),
                }
            )
    headers_root = _headers_libraries_root(llvm_config_features)
    if headers_root:
        root_records.append(
            {
                "tool_name": "headers-libs",
                "install_root": headers_root,
                "authority": str(llvm_config_features.get("discovery_source", "unavailable")),
                "configured_absolute": _configured_path_is_absolute(llvm_config_probe),
            }
        )

    diagnostics: list[str] = []
    tool_roots = {
        str(record["install_root"])
        for record in root_records
        if record["tool_name"] != "headers-libs"
    }
    root_status = "coherent"
    if len(tool_roots) > 1:
        root_status = "mixed"
        diagnostics.append("LLVM executable tools resolved from multiple install roots")
    elif tool_roots and headers_root and headers_root not in tool_roots:
        enforce_headers_root = _configured_path_is_absolute(llvm_config_probe) or any(
            bool(record.get("configured_absolute"))
            for record in root_records
            if record["tool_name"] != "headers-libs"
        )
        if enforce_headers_root:
            root_status = "mixed"
            diagnostics.append("LLVM headers/libs resolved from a different install root")
        else:
            root_status = "coherent-with-advisory-header-root"
    elif not root_records:
        root_status = "insufficient-root-evidence"

    version_records: list[dict[str, object]] = []
    missing_versions: list[str] = []
    version_families: set[tuple[int, int]] = set()
    unsupported_versions: list[str] = []
    for tool_name, record in tool_records:
        if not bool(record.get("found")):
            continue
        version = str(record.get("version", ""))
        family = _version_family(version)
        version_records.append(
            {
                "tool_name": tool_name,
                "version": version,
                "version_family": ".".join(str(part) for part in family) if family else "",
            }
        )
        if family is None:
            missing_versions.append(tool_name)
            continue
        version_families.add(family)
        if family < MIN_SUPPORTED_LLVM_VERSION:
            unsupported_versions.append(f"{tool_name} {version}")

    if missing_versions:
        version_status = "unresolved"
        diagnostics.append(
            "LLVM required tool versions were unresolved: " + ", ".join(sorted(missing_versions))
        )
    elif len(version_families) > 1:
        version_status = "mismatched"
        diagnostics.append("LLVM required tool versions resolved to different major/minor families")
    elif unsupported_versions:
        version_status = "unsupported"
        diagnostics.append(
            "LLVM required tool versions are below the minimum supported family: "
            + ", ".join(sorted(unsupported_versions))
        )
    elif version_records:
        version_status = "coherent"
    else:
        version_status = "unavailable"

    claimable = root_status != "mixed" and version_status == "coherent"
    return {
        "contract_id": "objc3c.llvm.coherent-toolchain-identity.v1",
        "minimum_supported_version_family": ".".join(str(part) for part in MIN_SUPPORTED_LLVM_VERSION),
        "root_status": root_status,
        "version_status": version_status,
        "claimable": claimable,
        "rejection_policy": "fail-closed-before-object-package-execution-platform-claim",
        "mixed_root_status": "native_object_emission_mixed_toolchain_root",
        "mismatched_version_status": "native_object_emission_mismatched_tool_versions",
        "unsupported_version_status": "native_object_emission_unsupported_tool_version",
        "unresolved_version_status": "native_object_emission_unresolved_tool_version",
        "root_records": root_records,
        "version_records": version_records,
        "diagnostics": diagnostics,
    }


def _native_object_emission_identity_failure_status(
    toolchain_identity: dict[str, object],
) -> str:
    if str(toolchain_identity.get("root_status")) == "mixed":
        return str(
            toolchain_identity.get(
                "mixed_root_status",
                "native_object_emission_mixed_toolchain_root",
            )
        )
    version_status = str(toolchain_identity.get("version_status"))
    if version_status == "mismatched":
        return str(
            toolchain_identity.get(
                "mismatched_version_status",
                "native_object_emission_mismatched_tool_versions",
            )
        )
    if version_status == "unsupported":
        return str(
            toolchain_identity.get(
                "unsupported_version_status",
                "native_object_emission_unsupported_tool_version",
            )
        )
    return str(
        toolchain_identity.get(
            "unresolved_version_status",
            "native_object_emission_unresolved_tool_version",
        )
    )


def build_llvm_support_matrix(
    *,
    clang_probe: dict[str, object],
    clangxx_probe: dict[str, object],
    llc_probe: dict[str, object],
    llvm_ar_probe: dict[str, object],
    llvm_config_probe: dict[str, object],
    llc_features: dict[str, object],
    llvm_config_features: dict[str, object],
    toolchain_identity: dict[str, object],
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
    toolchain_claimable = bool(toolchain_identity.get("claimable", False))
    clangxx_ready = bool(clangxx_probe.get("found"))
    object_emission_ready = llc_found and llc_supports_obj and toolchain_claimable
    package_capability_ready = (
        parity_ready and llvm_ar_found and headers_libraries_discovered and toolchain_claimable
    )
    native_execution_ready = (
        parity_ready and clangxx_ready and headers_libraries_discovered and toolchain_claimable
    )
    native_object_emission_status = (
        "native_object_emission_supported"
        if object_emission_ready
        else (
            "native_object_emission_missing_llc"
            if not llc_found
            else (
                "native_object_emission_filetype_obj_unavailable"
                if not llc_supports_obj
                else _native_object_emission_identity_failure_status(toolchain_identity)
            )
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

    if object_emission_ready:
        supported_features.append("llvm-direct-object-emission")
    else:
        rejected_features.append(
            {
                "feature": "llvm-direct-object-emission",
                "reason": (
                    str(llc_probe.get("diagnostic", "llc executable missing"))
                    if not bool(llc_probe.get("found"))
                    else (
                        "llc missing --filetype=obj support"
                        if not llc_supports_obj
                        else "; ".join(
                            str(item)
                            for item in toolchain_identity.get("diagnostics", [])
                            if str(item)
                        )
                        or "coherent LLVM toolchain identity unavailable"
                    )
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

    if toolchain_claimable:
        supported_features.append("coherent-llvm-toolchain-identity")
    else:
        rejected_features.append(
            {
                "feature": "coherent-llvm-toolchain-identity",
                "reason": "; ".join(
                    str(item)
                    for item in toolchain_identity.get("diagnostics", [])
                    if str(item)
                )
                or "coherent LLVM root/version proof unavailable",
            }
        )

    if parity_ready and toolchain_claimable:
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
        if parity_ready:
            blocker_text = "; ".join(
                str(item)
                for item in toolchain_identity.get("diagnostics", [])
                if str(item)
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
            "mixed_toolchain_status": "native_object_emission_mixed_toolchain_root",
            "mismatched_version_status": "native_object_emission_mismatched_tool_versions",
            "unsupported_version_status": "native_object_emission_unsupported_tool_version",
            "unresolved_version_status": "native_object_emission_unresolved_tool_version",
            "hosted_runner_behavior": "fail-closed-no-native-object-success-claim",
            "conformance_minima_behavior": "fail-closed-before-cross-lane-runtime-proof",
            "fallback_policy": "no-clang-fallback-success-claim",
            "coherent_toolchain_policy": "no-mixed-root-or-mismatched-version-success-claim",
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
        "toolchain_identity": toolchain_identity,
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
                if object_emission_ready
                else "rejected",
                failure_reason=(
                    ""
                    if object_emission_ready
                    else (
                        str(llc_probe.get("diagnostic", "llc executable missing"))
                        if not bool(llc_probe.get("found"))
                        else (
                            "llc missing --filetype=obj support"
                            if not llc_supports_obj
                            else "; ".join(
                                str(item)
                                for item in toolchain_identity.get("diagnostics", [])
                                if str(item)
                            )
                            or "coherent LLVM toolchain identity unavailable"
                        )
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
                "object_emission_capability": "supported" if object_emission_ready else "rejected",
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
    toolchain_identity: dict[str, object],
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
        toolchain_identity=toolchain_identity,
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
        "toolchain_identity": toolchain_identity,
        "llvm_support_matrix": llvm_support_matrix,
        "toolchain_resolution": llvm_support_matrix["toolchain_resolution"],
        "sema_type_system_parity": sema_type_system_parity,
        "capability_demo_compatibility": capability_demo_compatibility,
        "failures": failure_list,
        "ok": not failure_list,
    }
