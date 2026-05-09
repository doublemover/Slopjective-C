"""Storage/reflection runtime ownership acceptance case registry."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_arc_assertions import (
    assert_arc_weak_strong_ownership_profiles,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_payload_assertions import (
    assert_storage_ownership_runtime_payload,
    capture_storage_ownership_reflection_facts,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_sources import (
    build_storage_ownership_reflection_sources,
    compile_storage_ownership_reflection_fixture,
    run_storage_ownership_reflection_probe,
)
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_summary import (
    build_storage_ownership_reflection_result,
)


def check_storage_ownership_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    sources = build_storage_ownership_reflection_sources(run_dir)
    artifacts = compile_storage_ownership_reflection_fixture(sources)
    payload = run_storage_ownership_reflection_probe(clangxx, sources, artifacts)
    facts = capture_storage_ownership_reflection_facts(payload, artifacts)

    assert_storage_ownership_runtime_payload(facts, artifacts)
    assert_arc_weak_strong_ownership_profiles(facts)

    return build_storage_ownership_reflection_result(sources, artifacts, facts)


__all__ = ["check_storage_ownership_reflection_case"]
