"""Object Model metaclass and canonical sample linked-runtime case registry."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_artifacts import (
    assert_canonical_sample_lowering_artifacts,
    assert_canonical_sample_registration_manifest,
    assert_metaclass_graph_compile_artifacts,
    assert_metaclass_graph_negative_diagnostics,
    compile_canonical_sample_set_fixture,
    compile_metaclass_graph_root_class_fixture,
)
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_probe_assertions import (
    assert_canonical_sample_property_reflection,
    assert_canonical_sample_protocol_queries,
    assert_canonical_sample_runtime_values,
    assert_canonical_sample_widget_realization,
    assert_metaclass_graph_probe_payload,
    capture_canonical_sample_probe_facts,
)
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_sources import (
    build_canonical_sample_set_sources,
    build_metaclass_graph_root_class_sources,
)
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_summaries import (
    build_canonical_sample_set_summary,
    build_metaclass_graph_root_class_summary,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

_EXPORTED_CASE_NAMES = [
    "check_metaclass_graph_root_class_case",
    "check_canonical_sample_set_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_metaclass_graph_root_class_case(clangxx: str, run_dir: Path) -> CaseResult:
    sources = build_metaclass_graph_root_class_sources(run_dir)
    artifacts = compile_metaclass_graph_root_class_fixture(sources)
    compile_probe(clangxx, sources.probe, sources.exe_path, [artifacts.obj_path])
    payload = parse_json_output(run_probe(sources.exe_path), sources.probe_label)
    facts = assert_metaclass_graph_probe_payload(payload)
    assert_metaclass_graph_compile_artifacts(artifacts)
    negative_batch = assert_metaclass_graph_negative_diagnostics(sources)

    return CaseResult(
        case_id=sources.case_id,
        probe=sources.probe_summary_path(),
        fixture=sources.fixture_summary_path(),
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_metaclass_graph_root_class_summary(
            sources,
            artifacts,
            facts,
            negative_batch,
        ),
    )


def check_canonical_sample_set_case(clangxx: str, run_dir: Path) -> CaseResult:
    sources = build_canonical_sample_set_sources(run_dir)
    artifacts = compile_canonical_sample_set_fixture(sources)
    compile_probe(clangxx, sources.probe, sources.exe_path, [artifacts.obj_path])
    payload = parse_json_output(run_probe(sources.exe_path), sources.probe_label)
    facts = capture_canonical_sample_probe_facts(payload)
    assert_canonical_sample_widget_realization(facts)
    assert_canonical_sample_registration_manifest(artifacts)
    assert_canonical_sample_runtime_values(facts)
    assert_canonical_sample_protocol_queries(facts)
    assert_canonical_sample_property_reflection(facts)
    assert_canonical_sample_lowering_artifacts(artifacts)

    return CaseResult(
        case_id=sources.case_id,
        probe=sources.probe_summary_path(),
        fixture=sources.fixture_summary_path(),
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_canonical_sample_set_summary(artifacts, facts),
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
