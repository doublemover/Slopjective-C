"""Schema index model for workflow registry payloads."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class WorkflowSchemaSpec:
    schema_id: str
    schema_path: str
    payload_surface: str
    owner_surface: str
    capability_truth_scope: str
    public_contract: bool
