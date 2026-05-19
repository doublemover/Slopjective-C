"""Object Model realization lookup linked-runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..runtime_contract_object_model import (
    REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
    RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = ["check_realization_lookup_reflection_runtime_case"]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_realization_lookup_reflection_runtime_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "realization-lookup-reflection-runtime"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "canonical_runnable_sample_set.objc3"
    )
    obj_path, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = (
        case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )

    probe = ROOT / REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE
    exe_path = case_dir / "object_model_lookup_reflection_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(
        run_probe(exe_path), "realization lookup reflection runtime probe"
    )
    aggregate = payload.get("aggregate", {})
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

    expect(payload.get("widget_found") == 1, "expected Widget class lookup to succeed")
    expect(payload.get("traced_value") == 13, "expected tracedValue to return 13")
    expect(payload.get("count_value") == 41, "expected count to reload the written value")
    expect(
        payload.get("count_property_found") == 1,
        "expected count property reflection lookup to succeed",
    )
    expect(payload.get("tracer_conforms") == 1, "expected Widget to conform to Tracer")
    expect(
        aggregate.get("realized_class_count") == 2,
        "expected aggregate realized class count to report the two live instance-class nodes",
    )
    expect(
        aggregate.get("reflectable_property_count") == 4,
        "expected aggregate reflectable property count to report the four live Widget/Base property accessors",
    )
    expect(
        aggregate.get("attached_category_count") == 1,
        "expected aggregate attached category count to report the single live attached category",
    )
    expect(
        aggregate.get("protocol_conformance_edge_count", 0) >= 2,
        "expected aggregate protocol conformance edge count to stay live",
    )
    expect(
        aggregate.get("method_cache_entry_count", 0) >= 4,
        "expected aggregate method cache entry count to reflect the executed dispatches",
    )
    expect(
        aggregate.get("last_class_query_found") == 1
        and aggregate.get("last_queried_class_name") == "Widget"
        and aggregate.get("last_resolved_class_name") == "Widget",
        "expected aggregate state to preserve the last class lookup",
    )
    expect(
        aggregate.get("last_property_query_found") == 1
        and aggregate.get("last_property_query_inherited") == 0
        and aggregate.get("last_queried_property_name") == "count"
        and aggregate.get("last_resolved_property_class_name") == "Widget",
        "expected aggregate state to preserve the last property lookup",
    )
    expect(
        aggregate.get("last_protocol_query_class_found") == 1
        and aggregate.get("last_protocol_query_protocol_found") == 1
        and aggregate.get("last_protocol_query_conforms") == 1
        and aggregate.get("last_queried_protocol_class_name") == "Widget"
        and aggregate.get("last_queried_protocol_name") == "Tracer",
        "expected aggregate state to preserve the last protocol-conformance query",
    )
    expect(
        bool(aggregate.get("last_resolved_class_owner_identity"))
        and bool(aggregate.get("last_resolved_property_owner_identity"))
        and bool(aggregate.get("last_matched_protocol_owner_identity")),
        "expected aggregate state to preserve the last resolved owner identities",
    )
    expect(
        registration_manifest.get("class_descriptor_count") == 4
        and registration_manifest.get("property_descriptor_count") == 8
        and registration_manifest.get("category_descriptor_count") == 2,
        "expected canonical sample-set registration manifests to keep the broader emitted descriptor counts",
    )
    implementation_surface = manifest.get(
        "runtime_realization_lookup_reflection_implementation_surface", {}
    )
    expect(
        implementation_surface.get("contract_id")
        == RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "expected compile manifest to publish the realization lookup/reflection implementation surface",
    )

    return CaseResult(
        case_id="realization-lookup-reflection-runtime",
        probe=REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
        fixture="tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "realized_class_count": aggregate["realized_class_count"],
            "reflectable_property_count": aggregate["reflectable_property_count"],
            "method_cache_entry_count": aggregate["method_cache_entry_count"],
            "last_queried_protocol_name": aggregate["last_queried_protocol_name"],
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
