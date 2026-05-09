"""Core public command-surface action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_PUBLIC_COMMAND_ACTION_SPECS: dict[str, ActionSpec] = {
    "build-public-command-surface": ActionSpec("build-public-command-surface", "build the generated public command-surface appendix", "python:scripts/render_objc3c_public_command_surface.py"),
    "check-public-command-surface": ActionSpec("check-public-command-surface", "check the generated public command-surface appendix for drift", "python:scripts/render_objc3c_public_command_surface.py --check", validation_tier="docs", guarantee_owner="operator-facing machine appendix stays in sync with the live workflow runner and package scripts"),
    "build-public-command-contract": ActionSpec("build-public-command-contract", "build the canonical public command contract artifact", "python:scripts/build_objc3c_public_command_contract.py"),
    "check-public-command-contract": ActionSpec("check-public-command-contract", "check the canonical public command contract artifact for drift", "python:scripts/build_objc3c_public_command_contract.py --check"),
    "check-public-command-budget": ActionSpec("check-public-command-budget", "check the public command budget and appendix sync against the canonical command contract", "python:scripts/check_objc3c_public_command_budget.py"),
}

__all__ = ["CORE_PUBLIC_COMMAND_ACTION_SPECS"]
