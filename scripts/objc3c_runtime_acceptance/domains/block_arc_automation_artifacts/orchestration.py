"""Public block/ARC storage automation artifact orchestration."""

from __future__ import annotations

from pathlib import Path

from .catalog import (
    ARC_AUTORELEASE_RETURN_FIXTURE,
    ARC_CLEANUP_SCOPE_FIXTURE,
    ARC_IMPLICIT_CLEANUP_FIXTURE,
    ARC_INFERENCE_FIXTURE,
    ARC_METHOD_FAMILY_FIXTURE,
    ARC_MODE_FIXTURE,
    NONOWNING_CAPTURE_FIXTURE,
    OWNED_CAPTURE_FIXTURE,
)
from .fixtures import (
    compile_fixture_artifact,
    compile_manifest_only_fixture,
    compile_negative_diagnostics,
)
from .models import BlockArcAutomationArtifacts
from .payloads import build_block_arc_automation_artifacts


def load_block_arc_automation_artifacts(
    case_dir: Path,
) -> BlockArcAutomationArtifacts:
    return build_block_arc_automation_artifacts(
        owned_capture=compile_fixture_artifact(OWNED_CAPTURE_FIXTURE, case_dir),
        nonowning_capture=compile_fixture_artifact(NONOWNING_CAPTURE_FIXTURE, case_dir),
        arc_mode=compile_fixture_artifact(ARC_MODE_FIXTURE, case_dir),
        arc_inference=compile_fixture_artifact(ARC_INFERENCE_FIXTURE, case_dir),
        arc_cleanup_scope_manifest=compile_manifest_only_fixture(
            ARC_CLEANUP_SCOPE_FIXTURE,
            case_dir,
        ),
        arc_implicit_cleanup_manifest=compile_manifest_only_fixture(
            ARC_IMPLICIT_CLEANUP_FIXTURE,
            case_dir,
        ),
        arc_autorelease_return=compile_fixture_artifact(
            ARC_AUTORELEASE_RETURN_FIXTURE,
            case_dir,
        ),
        arc_method_family=compile_fixture_artifact(
            ARC_METHOD_FAMILY_FIXTURE,
            case_dir,
        ),
        negative_batch=compile_negative_diagnostics(case_dir),
    )


__all__ = ["load_block_arc_automation_artifacts"]
