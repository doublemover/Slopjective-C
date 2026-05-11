"""Registration replay payload and result assembly."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from ...paths import ROOT
from .catalog import PROVIDER_FIXTURE
from .models import (
    RegistrationReplayArtifacts,
    RegistrationReplayIdentities,
    RegistrationReplayProbeRun,
)


def registration_replay_identities(
    artifacts: RegistrationReplayArtifacts,
) -> RegistrationReplayIdentities:
    return RegistrationReplayIdentities(
        provider=artifacts.provider_registration_manifest[
            "translation_unit_identity_key"
        ],
        consumer=artifacts.consumer_registration_manifest[
            "translation_unit_identity_key"
        ],
    )


def build_registration_replay_summary(
    artifacts: RegistrationReplayArtifacts,
    probe_run: RegistrationReplayProbeRun,
    identities: RegistrationReplayIdentities,
    case_total_ms: int,
) -> dict[str, Any]:
    return {
        "provider_fixture": PROVIDER_FIXTURE,
        "provider_import_surface": _repo_relative_path(
            artifacts.provider_import_surface
        ),
        "link_plan": _repo_relative_path(artifacts.link_plan_path),
        "provider_compile_ms": artifacts.provider_compile_ms,
        "consumer_compile_ms": artifacts.consumer_compile_ms,
        "probe_link_ms": probe_run.probe_link_ms,
        "probe_run_ms": probe_run.probe_run_ms,
        "case_total_ms": case_total_ms,
        "module_image_count": artifacts.link_plan.get("module_image_count"),
        "first_replay_generation": probe_run.payload["first_replay_generation"],
        "second_replay_generation": probe_run.payload["second_replay_generation"],
        "blocked_replay_status": probe_run.payload["replay_without_reset_status"],
        "provider_translation_unit_identity_key": identities.provider,
        "consumer_translation_unit_identity_key": identities.consumer,
    }


def _repo_relative_path(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


__all__ = [
    "build_registration_replay_summary",
    "registration_replay_identities",
]
