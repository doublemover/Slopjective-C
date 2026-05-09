"""Interop packaging imported-runtime replay acceptance case."""

from __future__ import annotations

from pathlib import Path
from time import perf_counter

from ..case_result import CaseResult
from ..paths import ROOT
from ..probes import compile_probe, parse_json_output, run_probe
from ..runtime_contract_interop import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_artifacts import (
    compile_imported_runtime_packaging_fixture_pair,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_link_plan import (
    assert_imported_runtime_link_plan_contract,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_probe import (
    assert_imported_runtime_probe_payload,
)

_EXPORTED_CASE_NAMES = [
    "check_imported_runtime_packaging_replay_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_imported_runtime_packaging_replay_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "imported-runtime-packaging-replay"
    probe = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_PROBE)

    artifacts = compile_imported_runtime_packaging_fixture_pair(case_dir)
    assert_imported_runtime_link_plan_contract(artifacts)

    exe_path = case_dir / "import_module_execution_matrix_probe.exe"
    probe_link_started = perf_counter()
    compile_probe(
        clangxx,
        probe,
        exe_path,
        [artifacts.provider_obj, artifacts.consumer_obj],
    )
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)
    probe_run_started = perf_counter()
    payload = parse_json_output(
        run_probe(exe_path), "imported runtime cross-module packaging probe"
    )
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    provider_identity = artifacts.provider_registration_manifest[
        "translation_unit_identity_key"
    ]
    consumer_identity = artifacts.consumer_registration_manifest[
        "translation_unit_identity_key"
    ]
    assert_imported_runtime_probe_payload(
        payload,
        artifacts.link_plan,
        provider_identity,
        consumer_identity,
    )

    return CaseResult(
        case_id="imported-runtime-packaging-replay",
        probe=IMPORTED_RUNTIME_PACKAGING_PROBE,
        fixture=IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "provider_fixture": IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            "provider_import_surface": str(
                artifacts.provider_import_surface.relative_to(ROOT)
            ).replace("\\", "/"),
            "link_plan": str(artifacts.link_plan_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "provider_translation_unit_identity_key": provider_identity,
            "consumer_translation_unit_identity_key": consumer_identity,
            "provider_compile_ms": artifacts.provider_compile_ms,
            "consumer_compile_ms": artifacts.consumer_compile_ms,
            "probe_link_ms": probe_link_ms,
            "probe_run_ms": probe_run_ms,
            "case_total_ms": case_total_ms,
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
