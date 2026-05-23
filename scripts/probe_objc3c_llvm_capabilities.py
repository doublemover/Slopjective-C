#!/usr/bin/env python3
"""Deterministically probe LLVM and sema/type-system parity capability surfaces."""

from __future__ import annotations

import subprocess

if __package__:
    from .objc3c_llvm_capability_probe.classification import (
        build_capability_demo_compatibility_surface,
        build_sema_type_system_parity_surface,
    )
    from .objc3c_llvm_capability_probe.cli import main, parse_args, run
    from .objc3c_llvm_capability_probe.commands import (
        probe_executable,
        probe_llc_filetype_obj,
        probe_llvm_config_paths,
        probe_llvm_install_root_paths,
        run_command,
    )
    from .objc3c_llvm_capability_probe.reports import build_toolchain_identity
    from .objc3c_llvm_capability_probe.constants import (
        MODE,
        PROGRAM_SURFACE_PATH,
        ROOT,
        SHOWCASE_PORTFOLIO_PATH,
    )
    from .objc3c_llvm_capability_probe.parsing import first_non_empty_line
else:
    from objc3c_llvm_capability_probe.classification import (
        build_capability_demo_compatibility_surface,
        build_sema_type_system_parity_surface,
    )
    from objc3c_llvm_capability_probe.cli import main, parse_args, run
    from objc3c_llvm_capability_probe.commands import (
        probe_executable,
        probe_llc_filetype_obj,
        probe_llvm_config_paths,
        probe_llvm_install_root_paths,
        run_command,
    )
    from objc3c_llvm_capability_probe.reports import build_toolchain_identity
    from objc3c_llvm_capability_probe.constants import (
        MODE,
        PROGRAM_SURFACE_PATH,
        ROOT,
        SHOWCASE_PORTFOLIO_PATH,
    )
    from objc3c_llvm_capability_probe.parsing import first_non_empty_line

__all__ = [
    "MODE",
    "PROGRAM_SURFACE_PATH",
    "ROOT",
    "SHOWCASE_PORTFOLIO_PATH",
    "build_capability_demo_compatibility_surface",
    "build_sema_type_system_parity_surface",
    "build_toolchain_identity",
    "first_non_empty_line",
    "main",
    "parse_args",
    "probe_executable",
    "probe_llc_filetype_obj",
    "probe_llvm_config_paths",
    "probe_llvm_install_root_paths",
    "run",
    "run_command",
    "subprocess",
]


if __name__ == "__main__":
    main()
