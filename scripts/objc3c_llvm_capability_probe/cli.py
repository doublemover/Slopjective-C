"""Command-line orchestration for the LLVM capability probe."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.json_io import write_json_file as write_json
from objc3c_tooling.paths import display_path

from .classification import (
    build_capability_demo_compatibility_surface,
    build_sema_type_system_parity_surface,
)
from .commands import (
    probe_executable,
    probe_llc_filetype_obj,
    probe_llvm_config_paths,
    probe_llvm_install_root_paths,
)
from .constants import (
    DEFAULT_SUMMARY_OUT,
    DESCRIPTION,
    PROGRAM_SURFACE_PATH,
    SHOWCASE_PORTFOLIO_PATH,
)
from .reports import build_summary, collect_failures, missing_contract_payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument("--clang", type=Path, default=Path("clang"))
    parser.add_argument("--clangxx", type=Path, default=Path("clang++"))
    parser.add_argument("--llc", type=Path, default=Path("llc"))
    parser.add_argument("--llvm-ar", type=Path, default=Path("llvm-ar"))
    parser.add_argument("--llvm-config", type=Path, default=Path("llvm-config"))
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def build_capability_demo_compatibility(
    *,
    parity_ready: bool,
) -> dict[str, object]:
    if not PROGRAM_SURFACE_PATH.is_file():
        return missing_contract_payload(
            program_surface_path=PROGRAM_SURFACE_PATH,
            showcase_portfolio_path=SHOWCASE_PORTFOLIO_PATH,
            failure=f"missing program surface contract: {display_path(PROGRAM_SURFACE_PATH)}",
        )
    if not SHOWCASE_PORTFOLIO_PATH.is_file():
        return missing_contract_payload(
            program_surface_path=PROGRAM_SURFACE_PATH,
            showcase_portfolio_path=SHOWCASE_PORTFOLIO_PATH,
            failure=f"missing showcase portfolio contract: {display_path(SHOWCASE_PORTFOLIO_PATH)}",
        )
    return build_capability_demo_compatibility_surface(
        program_surface=load_json(PROGRAM_SURFACE_PATH),
        showcase_portfolio=load_json(SHOWCASE_PORTFOLIO_PATH),
        parity_ready=parity_ready,
    )


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    clang_probe = probe_executable(args.clang, role="clang")
    clangxx_probe = probe_executable(args.clangxx, role="clang++")
    llc_probe = probe_executable(args.llc, role="llc")
    llvm_ar_probe = probe_executable(args.llvm_ar, role="llvm-ar")
    llvm_config_probe = probe_executable(args.llvm_config, role="llvm-config")

    llc_features: dict[str, object] = {
        "supports_filetype_obj": False,
    }
    if bool(llc_probe["found"]):
        llc_features = probe_llc_filetype_obj(args.llc)

    llvm_config_features: dict[str, object] = {
        "headers_libraries_discovered": False,
        "discovery_source": "unavailable",
    }
    if bool(llvm_config_probe["found"]):
        llvm_config_features = probe_llvm_config_paths(args.llvm_config)
    else:
        llvm_config_features = probe_llvm_install_root_paths(
            clangxx_probe,
            clang_probe,
            llc_probe,
            llvm_ar_probe,
        )

    sema_type_system_parity = build_sema_type_system_parity_surface(
        clang_probe=clang_probe,
        llc_probe=llc_probe,
        llc_features=llc_features,
    )
    capability_demo_compatibility = build_capability_demo_compatibility(
        parity_ready=bool(sema_type_system_parity["parity_ready"]),
    )
    failures = collect_failures(
        clang_probe=clang_probe,
        clangxx_probe=clangxx_probe,
        llc_probe=llc_probe,
        llvm_ar_probe=llvm_ar_probe,
        llvm_config_probe=llvm_config_probe,
        llc_features=llc_features,
        llvm_config_features=llvm_config_features,
        sema_type_system_parity=sema_type_system_parity,
        capability_demo_compatibility=capability_demo_compatibility,
    )

    summary = build_summary(
        clang_probe=clang_probe,
        clangxx_probe=clangxx_probe,
        llc_probe=llc_probe,
        llvm_ar_probe=llvm_ar_probe,
        llvm_config_probe=llvm_config_probe,
        llc_features=llc_features,
        llvm_config_features=llvm_config_features,
        sema_type_system_parity=sema_type_system_parity,
        capability_demo_compatibility=capability_demo_compatibility,
        failures=failures,
    )
    write_json(args.summary_out, summary)

    if failures:
        for failure in failures:
            print(f"LLVM-CAP-FAIL: {failure}", file=sys.stderr)
        print(f"wrote summary: {display_path(args.summary_out)}", file=sys.stderr)
        return 1

    print("LLVM-CAP-PASS: clang+llc capabilities discovered; sema/type-system parity ready")
    print(f"wrote summary: {display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))
