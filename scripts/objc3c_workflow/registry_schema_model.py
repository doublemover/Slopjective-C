"""Schema index model for workflow registry payloads."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class WorkflowSchemaSpec:
    schema_id: str
    schema_path: str
    payload_surface: str
    owner_surface: str
    schema_owner_surface: str
    capability_truth_scope: str
    public_contract: bool
    schema_index_schema_id: str
    schema_index_owner_surface: str
    constants_owner_surface: str
    model_owner_surface: str
    catalog_owner_surface: str
    payload_owner_surface: str
    index_facade_surface: str
    registry_store_owner_surface: str
    registry_view_owner_surface: str
