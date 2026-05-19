"""CaseResult summary helpers for Object Model metaclass sample cases."""

from __future__ import annotations

from typing import Any

from ..paths import ROOT
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_artifacts import (
    CanonicalSampleCompileArtifacts,
    MetaclassGraphCompileArtifacts,
)
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_probe_assertions import (
    CanonicalSampleProbeFacts,
    MetaclassGraphProbeFacts,
)
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_sources import (
    MetaclassGraphRootClassSources,
)


def _relative_to_root(path: Any) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def build_metaclass_graph_root_class_summary(
    sources: MetaclassGraphRootClassSources,
    artifacts: MetaclassGraphCompileArtifacts,
    facts: MetaclassGraphProbeFacts,
    negative_diagnostics_batch: Any,
) -> dict[str, Any]:
    return {
        "compile_manifest": _relative_to_root(artifacts.manifest_path),
        "llvm_ir": _relative_to_root(artifacts.ll_path),
        "positive_execution_fixture": sources.positive_execution_fixture,
        "negative_execution_fixture": sources.negative_execution_fixture,
        "realized_class_count": facts.graph_state.get("realized_class_count"),
        "root_class_count": facts.graph_state.get("root_class_count"),
        "metaclass_edge_count": facts.graph_state.get("metaclass_edge_count"),
        "receiver_class_binding_count": facts.graph_state.get(
            "receiver_class_binding_count"
        ),
        "root_base_identity": facts.root_entry.get("base_identity"),
        "widget_base_identity": facts.widget_entry.get("base_identity"),
        "negative_diagnostics_batch": negative_diagnostics_batch,
    }


def build_canonical_sample_set_summary(
    artifacts: CanonicalSampleCompileArtifacts,
    facts: CanonicalSampleProbeFacts,
) -> dict[str, Any]:
    return {
        "widget_base_identity": facts.widget_entry.get("base_identity"),
        "traced_value": facts.payload["traced_value"],
        "inherited_value": facts.payload["inherited_value"],
        "class_value": facts.payload["class_value"],
        "shared_value": facts.payload["shared_value"],
        "property_descriptor_count": artifacts.registration_manifest.get(
            "property_descriptor_count"
        ),
    }


__all__ = [
    "build_canonical_sample_set_summary",
    "build_metaclass_graph_root_class_summary",
]
