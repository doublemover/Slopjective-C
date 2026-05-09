"""Object Model dispatch fast-path linked-runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.object_model_fast_path_probe_assertions import (
    assert_dispatch_fast_path_probe_payload,
)
from objc3c_runtime_acceptance.domains.object_model_fast_path_surface_assertions import (
    assert_dispatch_fast_path_compile_surfaces,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

_EXPORTED_CASE_NAMES = ["check_live_dispatch_fast_path_case"]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_live_dispatch_fast_path_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "dispatch-fast-path"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "live_dispatch_fast_path_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        fixture, case_dir / "compile"
    )
    probe = ROOT / "tests" / "tooling" / "runtime" / "live_dispatch_fast_path_probe.cpp"
    exe_path = case_dir / "live_dispatch_fast_path_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_key_value_output(run_probe(exe_path), "dispatch fast-path probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest_path = (
        case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )

    assert_dispatch_fast_path_probe_payload(payload)
    assert_dispatch_fast_path_compile_surfaces(
        ll_text,
        manifest,
        registration_manifest,
    )

    return CaseResult(
        case_id="dispatch-fast-path",
        probe="tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        fixture="tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(
                registration_manifest_path.relative_to(ROOT)
            ).replace("\\", "/"),
            "baseline_cache_entry_count": payload.get("baseline_cache_entry_count"),
            "baseline_fast_path_seed_count": payload.get(
                "baseline_fast_path_seed_count"
            ),
            "mixed_first_dispatch_path": payload.get(
                "mixed_first_dispatch_state_last_dispatch_path"
            ),
            "mixed_first_implementation_kind": payload.get(
                "mixed_first_dispatch_state_last_implementation_kind"
            ),
            "mixed_second_dispatch_path": payload.get(
                "mixed_second_dispatch_state_last_dispatch_path"
            ),
            "mixed_second_implementation_kind": payload.get(
                "mixed_second_dispatch_state_last_implementation_kind"
            ),
            "strict_error_first_dispatch_path": payload.get(
                "strict_error_first_dispatch_state_last_dispatch_path"
            ),
            "strict_error_first_implementation_kind": payload.get(
                "strict_error_first_dispatch_state_last_implementation_kind"
            ),
            "strict_error_second_dispatch_path": payload.get(
                "strict_error_second_dispatch_state_last_dispatch_path"
            ),
            "strict_error_second_implementation_kind": payload.get(
                "strict_error_second_dispatch_state_last_implementation_kind"
            ),
            "mixed_first_live_dispatch_count": payload.get(
                "mixed_first_state_live_dispatch_count"
            ),
            "mixed_second_live_dispatch_count": payload.get(
                "mixed_second_state_live_dispatch_count"
            ),
            "strict_error_first_strict_dispatch_error_count": payload.get(
                "strict_error_first_state_strict_dispatch_error_count"
            ),
            "strict_error_second_strict_dispatch_error_count": payload.get(
                "strict_error_second_state_strict_dispatch_error_count"
            ),
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
