"""Compiler artifact case predicates and artifact readers."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.registry import (
    ACCEPTANCE_ARTIFACT_REGISTRY,
)

from ..catalog import ARTIFACT_REGISTRY_IMPORT_SURFACE_A_NAME
from ..catalog import ARTIFACT_REGISTRY_IMPORT_SURFACE_B_NAME
from ..catalog import COMPILE_BACKEND_PARITY_TRUTHFULNESS_FIELDS
from ..data import ArtifactRegistryCacheEvidence
from ..data import RegistryEventSnapshot
from ..predicates import artifact_registry_cache_key
from ..predicates import read_json_object
from ..predicates import registry_event_snapshot
from .expected_artifacts import compile_provenance_path
from .fixtures import artifact_registry_import_surface_args
from .models import ArtifactRegistryIsolationLayout
from .models import CompileBackendParityArtifacts
from .models import CompileBackendParityLayout
from .models import JsonObject


def load_compile_backend_parity_artifacts(
    layout: CompileBackendParityLayout,
) -> CompileBackendParityArtifacts:
    direct_provenance = read_json_object(
        compile_provenance_path(layout.direct_dir)
    )
    wrapper_provenance = read_json_object(
        compile_provenance_path(layout.wrapper_dir)
    )
    return CompileBackendParityArtifacts(
        direct_provenance=direct_provenance,
        wrapper_provenance=wrapper_provenance,
        direct_truthfulness=_optional_json_object_field(
            direct_provenance,
            "compile_output_truthfulness",
        ),
        wrapper_truthfulness=_optional_json_object_field(
            wrapper_provenance,
            "compile_output_truthfulness",
        ),
        compared_truthfulness_fields=tuple(
            COMPILE_BACKEND_PARITY_TRUTHFULNESS_FIELDS
        ),
    )


def artifact_registry_cache_evidence(
    layout: ArtifactRegistryIsolationLayout,
) -> ArtifactRegistryCacheEvidence:
    key_a, key_payload_a = artifact_registry_cache_key(
        layout.fixture,
        extra_args=list(layout.args_a),
    )
    key_b, key_payload_b = artifact_registry_cache_key(
        layout.fixture,
        extra_args=list(layout.args_b),
    )
    key_import_a, key_payload_import_a = artifact_registry_cache_key(
        layout.fixture,
        extra_args=list(
            artifact_registry_import_surface_args(
                layout,
                ARTIFACT_REGISTRY_IMPORT_SURFACE_A_NAME,
            )
        ),
    )
    key_import_b, key_payload_import_b = artifact_registry_cache_key(
        layout.fixture,
        extra_args=list(
            artifact_registry_import_surface_args(
                layout,
                ARTIFACT_REGISTRY_IMPORT_SURFACE_B_NAME,
            )
        ),
    )
    return ArtifactRegistryCacheEvidence(
        changed_args_key_a=key_a,
        changed_args_key_b=key_b,
        changed_args_payload_a=key_payload_a,
        changed_args_payload_b=key_payload_b,
        changed_import_surface_key_a=key_import_a,
        changed_import_surface_key_b=key_import_b,
        changed_import_surface_payload_a=key_payload_import_a,
        changed_import_surface_payload_b=key_payload_import_b,
    )


def current_registry_reuse_event_count() -> int:
    return len(ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events)


def current_registry_miss_event_count() -> int:
    return len(ACCEPTANCE_ARTIFACT_REGISTRY.miss_events)


def latest_registry_reuse_event() -> dict[str, Any]:
    return ACCEPTANCE_ARTIFACT_REGISTRY.reuse_events[-1]


def _optional_json_object_field(payload: JsonObject, field: str) -> JsonObject:
    value = payload.get(field, {})
    if isinstance(value, dict):
        return value
    raise TypeError(f"expected JSON object for {field}")


__all__ = [
    "artifact_registry_cache_evidence",
    "current_registry_miss_event_count",
    "current_registry_reuse_event_count",
    "latest_registry_reuse_event",
    "load_compile_backend_parity_artifacts",
    "registry_event_snapshot",
]
