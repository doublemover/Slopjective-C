from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path

from objc3c_library_cli_parity.capabilities import LLVMCapabilitySummary
from objc3c_library_cli_parity.capabilities import read_capability_summary


@dataclass(frozen=True)
class ResolvedSourceModeRouting:
    effective_backend: str
    effective_clang_path: Path | None
    effective_llc_path: Path | None
    failures: list[str]
    details: dict[str, Any]


def _capability_details(capability_summary: LLVMCapabilitySummary) -> dict[str, Any]:
    return {
        "clang_found": capability_summary.clang_found,
        "llc_found": capability_summary.llc_found,
        "llc_supports_filetype_obj": capability_summary.llc_supports_filetype_obj,
        "llc_supports_target_object_emission": (
            capability_summary.llc_supports_target_object_emission
        ),
        "parity_ready": capability_summary.parity_ready,
        "blockers": list(capability_summary.blockers),
    }


def resolve_source_mode_routing(args: argparse.Namespace) -> ResolvedSourceModeRouting:
    effective_clang_path = args.clang_path
    effective_llc_path = args.llc_path
    effective_backend = args.cli_ir_object_backend
    routing: dict[str, Any] = {
        "requested_cli_ir_object_backend": args.cli_ir_object_backend,
        "effective_ir_object_backend": effective_backend,
        "routed_from_capabilities": args.route_cli_backend_from_capabilities,
        "allow_stale_source_mode_outputs": args.allow_stale_source_mode_outputs,
    }
    capability_failures: list[str] = []
    if args.llvm_capabilities_summary is not None:
        try:
            capability_summary = read_capability_summary(args.llvm_capabilities_summary)
        except ValueError as exc:
            capability_failures.append(f"capability routing fail-closed: {exc}")
            return ResolvedSourceModeRouting(
                effective_backend=effective_backend,
                effective_clang_path=effective_clang_path,
                effective_llc_path=effective_llc_path,
                failures=capability_failures,
                details=routing,
            )
        routing["llvm_capabilities_summary"] = capability_summary.summary_path
        routing["llvm_capabilities"] = _capability_details(capability_summary)
        if not capability_summary.parity_ready:
            blockers = (
                ", ".join(capability_summary.blockers)
                if capability_summary.blockers
                else "unspecified"
            )
            capability_failures.append(
                "capability routing fail-closed: sema/type-system parity capability unavailable: "
                f"{blockers}"
            )
        if args.route_cli_backend_from_capabilities:
            if (
                capability_summary.llc_found
                and capability_summary.llc_supports_filetype_obj
                and capability_summary.llc_supports_target_object_emission
            ):
                effective_backend = "llvm-direct"
            else:
                capability_failures.append(
                    "capability routing fail-closed: routed llvm-direct backend requires llc --filetype=obj target object emission"
                )
        if effective_backend == "clang" and not capability_summary.clang_found:
            capability_failures.append(
                "capability routing fail-closed: clang backend selected but capability summary reports clang unavailable"
            )
        if effective_backend == "llvm-direct" and (
            not capability_summary.llc_found
            or not capability_summary.llc_supports_filetype_obj
            or not capability_summary.llc_supports_target_object_emission
        ):
            capability_failures.append(
                "capability routing fail-closed: llvm-direct backend selected but llc --filetype=obj target object emission is unavailable"
            )
        if effective_clang_path is None:
            effective_clang_path = Path(capability_summary.clang_path)
        if effective_llc_path is None:
            effective_llc_path = Path(capability_summary.llc_path)

    if args.route_cli_backend_from_capabilities and args.llvm_capabilities_summary is None:
        capability_failures.append(
            "capability routing fail-closed: --route-cli-backend-from-capabilities requires --llvm-capabilities-summary"
        )

    routing["effective_ir_object_backend"] = effective_backend
    if effective_clang_path is not None:
        routing["effective_clang_path"] = display_path(effective_clang_path)
    if effective_llc_path is not None:
        routing["effective_llc_path"] = display_path(effective_llc_path)

    return ResolvedSourceModeRouting(
        effective_backend=effective_backend,
        effective_clang_path=effective_clang_path,
        effective_llc_path=effective_llc_path,
        failures=capability_failures,
        details=routing,
    )
