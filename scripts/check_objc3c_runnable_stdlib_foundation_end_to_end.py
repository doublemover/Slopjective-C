#!/usr/bin/env python3
"""Validate the staged runnable stdlib foundation surface end to end from the package root."""

from __future__ import annotations

try:
    from objc3c_runnable_stdlib_foundation_e2e import (
        PACKAGE_CONTRACT_ID,
        PACKAGE_PS1,
        PWSH,
        REPORT_PATH,
        ROOT,
        SUMMARY_CONTRACT_ID,
        StdlibCompileResult,
        StdlibPackageSurface,
        build_summary_payload,
        compile_stdlib_modules,
        expect,
        load_stdlib_package_surface,
        main,
        run_package_command,
        write_summary,
    )
except ModuleNotFoundError:
    from scripts.objc3c_runnable_stdlib_foundation_e2e import (
        PACKAGE_CONTRACT_ID,
        PACKAGE_PS1,
        PWSH,
        REPORT_PATH,
        ROOT,
        SUMMARY_CONTRACT_ID,
        StdlibCompileResult,
        StdlibPackageSurface,
        build_summary_payload,
        compile_stdlib_modules,
        expect,
        load_stdlib_package_surface,
        main,
        run_package_command,
        write_summary,
    )


__all__ = [
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "StdlibCompileResult",
    "StdlibPackageSurface",
    "build_summary_payload",
    "compile_stdlib_modules",
    "expect",
    "load_stdlib_package_surface",
    "main",
    "run_package_command",
    "write_summary",
]


if __name__ == "__main__":
    raise SystemExit(main())
