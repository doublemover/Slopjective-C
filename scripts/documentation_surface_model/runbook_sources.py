"""Facade exports for runbook documentation sources."""

from __future__ import annotations

from documentation_surface import paths as docs_paths
from documentation_surface.model import DocumentationSurfaceSource

from .runbook_path_normalization import (
    normalize_runbook_source_path,
    runbook_source_path_sort_key,
)
from .runbook_source_catalog import (
    RUNBOOK_SOURCE_SPECS,
    RunbookSourceSpec,
    runbook_source_specs,
)
from .runbook_source_evidence import (
    build_runbook_source_evidence_payload,
    runbook_source_spec_evidence_payload,
)
from .runbook_source_groups import (
    RUNBOOK_GROUP_LABELS,
    RUNBOOK_GROUP_ORDER,
    RunbookSourceGroup,
    classify_runbook_source,
    group_runbook_source_specs,
)
from .source_factory import _source


def runbook_documentation_sources() -> tuple[DocumentationSurfaceSource, ...]:
    return tuple(
        _source(
            spec.path,
            required_tokens=spec.required_tokens,
            forbidden_tokens=spec.forbidden_tokens,
        )
        for spec in RUNBOOK_SOURCE_SPECS
    )


__all__ = (
    "RUNBOOK_GROUP_LABELS",
    "RUNBOOK_GROUP_ORDER",
    "RUNBOOK_SOURCE_SPECS",
    "RunbookSourceGroup",
    "RunbookSourceSpec",
    "build_runbook_source_evidence_payload",
    "classify_runbook_source",
    "docs_paths",
    "group_runbook_source_specs",
    "normalize_runbook_source_path",
    "runbook_documentation_sources",
    "runbook_source_path_sort_key",
    "runbook_source_spec_evidence_payload",
    "runbook_source_specs",
)
