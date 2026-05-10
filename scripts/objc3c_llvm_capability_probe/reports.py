"""Report shaping for LLVM capability probe summaries."""

from __future__ import annotations

from pathlib import Path
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
    llc_probe: dict[str, object],
    llc_features: dict[str, object],
    sema_type_system_parity: dict[str, object],
    capability_demo_compatibility: dict[str, object],
) -> list[str]:
    failures: list[str] = []
    if not bool(clang_probe["found"]):
        failures.append(str(clang_probe.get("diagnostic", "clang executable missing")))
    if not bool(llc_probe["found"]):
        failures.append(str(llc_probe.get("diagnostic", "llc executable missing")))
    if bool(llc_probe["found"]) and not bool(llc_features["supports_filetype_obj"]):
        failures.append("llc capability probe failed: --filetype=obj support not detected")
    if not bool(sema_type_system_parity["parity_ready"]):
        failures.append(
            "sema/type-system parity capability unavailable: "
            + ", ".join(str(blocker) for blocker in sema_type_system_parity["blockers"])
        )
    for failure in capability_demo_compatibility.get("failures", []):
        failures.append(f"capability demo compatibility: {failure}")
    return failures


def build_summary(
    *,
    clang_probe: dict[str, object],
    llc_probe: dict[str, object],
    llc_features: dict[str, object],
    sema_type_system_parity: dict[str, object],
    capability_demo_compatibility: dict[str, object],
    failures: Iterable[str],
) -> dict[str, object]:
    failure_list = list(failures)
    return {
        "mode": MODE,
        "clang": clang_probe,
        "llc": llc_probe,
        "llc_features": llc_features,
        "sema_type_system_parity": sema_type_system_parity,
        "capability_demo_compatibility": capability_demo_compatibility,
        "failures": failure_list,
        "ok": not failure_list,
    }
