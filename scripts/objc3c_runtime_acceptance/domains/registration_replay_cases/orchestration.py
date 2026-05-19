"""Registration replay public case orchestration."""

from __future__ import annotations

from pathlib import Path
from time import perf_counter

from ...case_result import CaseResult
from .assertions import (
    assert_registration_replay_link_plan,
    assert_registration_replay_payload,
)
from .catalog import CASE_ID, CLAIM_CLASS, FIXTURE, PROBE
from .execution import execute_registration_replay_probe
from .fixtures import compile_registration_replay_fixture_pair
from .payloads import (
    build_registration_replay_summary,
    registration_replay_identities,
)


def check_multi_image_registration_reset_replay_case(
    clangxx: str,
    run_dir: Path,
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / CASE_ID

    artifacts = compile_registration_replay_fixture_pair(case_dir)
    assert_registration_replay_link_plan(artifacts)

    probe_run = execute_registration_replay_probe(clangxx, case_dir, artifacts)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    identities = registration_replay_identities(artifacts)
    assert_registration_replay_payload(probe_run.payload, identities)

    return CaseResult(
        case_id=CASE_ID,
        probe=PROBE,
        fixture=FIXTURE,
        claim_class=CLAIM_CLASS,
        passed=True,
        summary=build_registration_replay_summary(
            artifacts,
            probe_run,
            identities,
            case_total_ms,
        ),
    )


__all__ = ["check_multi_image_registration_reset_replay_case"]
