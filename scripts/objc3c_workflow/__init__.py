"""Canonical objc3c workflow package."""

from __future__ import annotations

from .path_bootstrap import install_workflow_import_roots

install_workflow_import_roots()

from .action_spec import ActionSpec


def __getattr__(name: str):
    # Small Linux tooling owners must not resolve every optional action's host
    # tools merely because Python imports their parent package.
    if name == "ACTION_SPECS":
        from .action_catalog import ACTION_SPECS
        globals()[name] = ACTION_SPECS
        return ACTION_SPECS
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")

__all__ = ["ACTION_SPECS", "ActionSpec"]
