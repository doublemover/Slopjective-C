"""Evidence payload construction for runbook documentation sources."""

from __future__ import annotations

from typing import Any

from .runbook_path_normalization import normalize_runbook_source_path
from .runbook_source_catalog import RunbookSourceSpec, runbook_source_specs
from .runbook_source_groups import (
    classify_runbook_source,
    group_runbook_source_specs,
)


def runbook_source_spec_evidence_payload(spec: RunbookSourceSpec) -> dict[str, Any]:
    return {
        "path": normalize_runbook_source_path(spec.path),
        "group": classify_runbook_source(spec),
        "required_token_count": len(spec.required_tokens),
        "forbidden_token_count": len(spec.forbidden_tokens),
    }


def build_runbook_source_evidence_payload(
    specs: tuple[RunbookSourceSpec, ...] | None = None,
) -> dict[str, Any]:
    resolved_specs = runbook_source_specs() if specs is None else specs
    groups = group_runbook_source_specs(resolved_specs)
    return {
        "source_count": len(resolved_specs),
        "sources": [
            runbook_source_spec_evidence_payload(spec) for spec in resolved_specs
        ],
        "groups": [
            {
                "group": group.group_id,
                "label": group.label,
                "source_count": group.source_count,
                "sources": [
                    normalize_runbook_source_path(spec.path)
                    for spec in group.sources
                ],
            }
            for group in groups
        ],
    }


__all__ = (
    "build_runbook_source_evidence_payload",
    "runbook_source_spec_evidence_payload",
)
