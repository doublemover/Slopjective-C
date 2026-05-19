"""Public command surface runbook source catalog rows."""

from __future__ import annotations

from documentation_surface import paths as docs_paths

from .runbook_source_types import RunbookSourceSpec


def public_runbook_source_specs() -> tuple[RunbookSourceSpec, ...]:
    return (
        RunbookSourceSpec(
            docs_paths.PUBLIC_COMMAND_SURFACE_PATH,
            required_tokens=(
                "operator-facing appendix",
                "## Operator Notes",
                "Treat this file as a generated machine-facing appendix",
                "Canonical user-facing command names come from `package.json`",
                "`native/objc3c/`, `scripts/`, and `tests/` are the live implementation roots",
            ),
        ),
    )


__all__ = ("public_runbook_source_specs",)
