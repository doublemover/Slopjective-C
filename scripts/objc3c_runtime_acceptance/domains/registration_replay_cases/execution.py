"""Registration replay probe execution."""

from __future__ import annotations

from pathlib import Path
from time import perf_counter

from ...paths import ROOT
from ...probes import compile_probe, parse_json_output, run_probe
from .catalog import PROBE, PROBE_EXECUTABLE_NAME
from .models import RegistrationReplayArtifacts, RegistrationReplayProbeRun


def execute_registration_replay_probe(
    clangxx: str,
    case_dir: Path,
    artifacts: RegistrationReplayArtifacts,
) -> RegistrationReplayProbeRun:
    exe_path = case_dir / PROBE_EXECUTABLE_NAME
    probe_link_started = perf_counter()
    compile_probe(
        clangxx,
        ROOT / Path(PROBE),
        exe_path,
        [artifacts.provider_obj, artifacts.consumer_obj],
    )
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)

    probe_run_started = perf_counter()
    payload = parse_json_output(
        run_probe(exe_path),
        "multi-image registration reset/replay probe",
    )
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)

    return RegistrationReplayProbeRun(
        payload=payload,
        probe_link_ms=probe_link_ms,
        probe_run_ms=probe_run_ms,
    )


__all__ = ["execute_registration_replay_probe"]
