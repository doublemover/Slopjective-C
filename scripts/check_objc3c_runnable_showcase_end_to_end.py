#!/usr/bin/env python3
"""Validate showcase examples end to end from the staged runnable toolchain bundle."""

from __future__ import annotations

try:
    from objc3c_runnable_showcase_e2e import (
        PACKAGE_CONTRACT_ID,
        PACKAGE_PS1,
        PWSH,
        REPORT_PATH,
        ROOT,
        SUMMARY_CONTRACT_ID,
        ShowcaseExecutionResult,
        ShowcaseExampleResult,
        ShowcasePackageSurface,
        build_summary_payload,
        expect,
        load_showcase_package_surface,
        main,
        run_package_command,
        run_showcase_examples,
        validate_showcase_demo_packages,
        write_summary,
    )
except ModuleNotFoundError as exc:
    if exc.name != "objc3c_runnable_showcase_e2e":
        raise
    from scripts.objc3c_runnable_showcase_e2e import (
        PACKAGE_CONTRACT_ID,
        PACKAGE_PS1,
        PWSH,
        REPORT_PATH,
        ROOT,
        SUMMARY_CONTRACT_ID,
        ShowcaseExecutionResult,
        ShowcaseExampleResult,
        ShowcasePackageSurface,
        build_summary_payload,
        expect,
        load_showcase_package_surface,
        main,
        run_package_command,
        run_showcase_examples,
        validate_showcase_demo_packages,
        write_summary,
    )


__all__ = [
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "ShowcaseExecutionResult",
    "ShowcaseExampleResult",
    "ShowcasePackageSurface",
    "build_summary_payload",
    "expect",
    "load_showcase_package_surface",
    "main",
    "run_package_command",
    "run_showcase_examples",
    "validate_showcase_demo_packages",
    "write_summary",
]


if __name__ == "__main__":
    raise SystemExit(main())
