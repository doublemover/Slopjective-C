"""Registration runtime acceptance lifecycle cases."""

from __future__ import annotations

from pathlib import Path
from time import perf_counter

from objc3c_runtime_acceptance.case_result import CaseResult

from .registration_lifecycle_compile_surface_assertions import (
    assert_registration_descriptor_contract,
)
from .registration_lifecycle_runtime_assertions import (
    assert_registration_lifecycle_payload,
)
from .registration_lifecycle_sources import compile_registration_lifecycle_fixture
from .registration_lifecycle_sources import installation_lifecycle_case_dir
from .registration_lifecycle_sources import link_registration_lifecycle_probe
from .registration_lifecycle_sources import run_registration_lifecycle_probe
from .registration_lifecycle_sources import write_probe_fixture_config
from .registration_lifecycle_summary import RegistrationLifecycleTimings
from .registration_lifecycle_summary import build_registration_lifecycle_summary
from ..runtime_contract_registration import (
    INSTALLATION_LIFECYCLE_FIXTURE,
    INSTALLATION_LIFECYCLE_PROBE,
)

_EXPORTED_CASE_NAMES = ["check_installation_lifecycle_case"]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_installation_lifecycle_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_started = perf_counter()
    case_dir = installation_lifecycle_case_dir(run_dir)

    fixture_build = compile_registration_lifecycle_fixture(case_dir)
    assert_registration_descriptor_contract(fixture_build.registration_descriptor)
    probe_fixture_config = write_probe_fixture_config(
        case_dir,
        fixture_build.registration_descriptor,
    )
    probe_link = link_registration_lifecycle_probe(
        clangxx,
        case_dir,
        fixture_build.object_path,
        probe_fixture_config,
    )
    probe_run = run_registration_lifecycle_probe(probe_link.executable_path)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    assert_registration_lifecycle_payload(probe_run.payload)

    return CaseResult(
        case_id="installation-lifecycle",
        probe=INSTALLATION_LIFECYCLE_PROBE,
        fixture=INSTALLATION_LIFECYCLE_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_registration_lifecycle_summary(
            probe_run.payload,
            RegistrationLifecycleTimings(
                fixture_compile_ms=fixture_build.fixture_compile_ms,
                probe_link_ms=probe_link.probe_link_ms,
                probe_run_ms=probe_run.probe_run_ms,
                case_total_ms=case_total_ms,
            ),
        ),
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
