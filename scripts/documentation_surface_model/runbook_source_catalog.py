"""Runbook documentation source catalog for the documentation surface model."""

from __future__ import annotations

from .runbook_source_catalog_core import core_runbook_source_specs
from .runbook_source_catalog_developer import developer_runbook_source_specs
from .runbook_source_catalog_performance import performance_runbook_source_specs
from .runbook_source_catalog_public import public_runbook_source_specs
from .runbook_source_types import RunbookSourceSpec


RUNBOOK_SOURCE_SPECS: tuple[RunbookSourceSpec, ...] = (
    *core_runbook_source_specs(),
    *developer_runbook_source_specs(),
    *performance_runbook_source_specs(),
    *public_runbook_source_specs(),
)


def runbook_source_specs() -> tuple[RunbookSourceSpec, ...]:
    return RUNBOOK_SOURCE_SPECS


__all__ = (
    "RUNBOOK_SOURCE_SPECS",
    "RunbookSourceSpec",
    "runbook_source_specs",
)
