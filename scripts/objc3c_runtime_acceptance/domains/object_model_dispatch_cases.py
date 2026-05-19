"""Object Model canonical dispatch linked-runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.object_model_dispatch_artifact_assertions import (
    assert_canonical_dispatch_compile_artifacts,
)
from objc3c_runtime_acceptance.domains.object_model_dispatch_payload_assertions import (
    assert_canonical_dispatch_payload,
    build_canonical_dispatch_summary,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

_EXPORTED_CASE_NAMES = ["check_canonical_dispatch_case"]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_canonical_dispatch_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "canonical-dispatch"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_canonical_runnable_object_runtime_library.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        fixture,
        case_dir / "compile",
    )
    probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "runtime_canonical_runnable_object_probe.cpp"
    )
    exe_path = case_dir / "runtime_canonical_runnable_object_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "canonical dispatch probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    assert_canonical_dispatch_payload(payload)
    assert_canonical_dispatch_compile_artifacts(manifest, ll_text)

    return CaseResult(
        case_id="canonical-dispatch",
        probe="tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_canonical_dispatch_summary(payload),
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
