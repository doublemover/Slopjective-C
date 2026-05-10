#!/usr/bin/env python3
"""Validate template-derived bonus experiences end to end from the staged runnable bundle."""

from __future__ import annotations

try:
    from objc3c_runnable_bonus_experience_e2e import (
        PACKAGE_CONTRACT_ID,
        PACKAGE_PS1,
        PWSH,
        REPORT_PATH,
        ROOT,
        SUMMARY_CONTRACT_ID,
        BonusPackageSurface,
        ExampleRunResult,
        build_summary_payload,
        expect,
        load_bonus_package_surface,
        main,
        materialize_and_run_examples,
        run_capability_probe,
        run_package_command,
        write_summary,
    )
except ModuleNotFoundError:
    from scripts.objc3c_runnable_bonus_experience_e2e import (
        PACKAGE_CONTRACT_ID,
        PACKAGE_PS1,
        PWSH,
        REPORT_PATH,
        ROOT,
        SUMMARY_CONTRACT_ID,
        BonusPackageSurface,
        ExampleRunResult,
        build_summary_payload,
        expect,
        load_bonus_package_surface,
        main,
        materialize_and_run_examples,
        run_capability_probe,
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
    "BonusPackageSurface",
    "ExampleRunResult",
    "build_summary_payload",
    "expect",
    "load_bonus_package_surface",
    "main",
    "materialize_and_run_examples",
    "run_capability_probe",
    "run_package_command",
    "write_summary",
]


if __name__ == "__main__":
    raise SystemExit(main())
