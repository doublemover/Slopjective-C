from __future__ import annotations

from .constants import (
    EXPECTED_STAGES,
    FIXTURE_KINDS,
    NATIVE_ROOT,
    REQUIRED_TREE,
    RETIRED_SURFACE_TAGS,
)
from .models import BehaviorFixture


def validate_behavior_fixture(fixture: BehaviorFixture) -> None:
    metadata = fixture.metadata
    if metadata.get("schema_version") != 1:
        raise RuntimeError(f"schema_version must be 1 in {fixture.relative_metadata}")
    if metadata.get("fixture") != fixture.source_path.name:
        raise RuntimeError(
            f"fixture field must match source filename in {fixture.relative_metadata}"
        )
    if metadata.get("origin") != "hand-authored":
        raise RuntimeError(
            f"native behavior fixture must be hand-authored in {fixture.relative_metadata}"
        )

    owner_phase = fixture.owner_phase
    behavior_family = fixture.behavior_family
    if owner_phase not in REQUIRED_TREE:
        raise RuntimeError(f"unknown owner_phase '{owner_phase}' in {fixture.relative_metadata}")
    if behavior_family not in REQUIRED_TREE[owner_phase]:
        raise RuntimeError(
            f"unknown behavior_family '{behavior_family}' for phase '{owner_phase}' "
            f"in {fixture.relative_metadata}"
        )

    relative_parts = fixture.source_path.relative_to(NATIVE_ROOT).parts
    if len(relative_parts) < 3:
        raise RuntimeError(f"fixture is not under a behavior family: {fixture.relative_source}")
    if relative_parts[0] != owner_phase or relative_parts[1] != behavior_family:
        raise RuntimeError(
            f"fixture path and metadata owner disagree: {fixture.relative_source} "
            f"vs {owner_phase}/{behavior_family}"
        )

    expected_stage = fixture.expected_stage
    if expected_stage not in EXPECTED_STAGES:
        raise RuntimeError(f"unknown expected stage '{expected_stage}' in {fixture.relative_metadata}")
    if fixture.fixture_kind not in FIXTURE_KINDS:
        raise RuntimeError(f"unknown fixture_kind '{fixture.fixture_kind}' in {fixture.relative_metadata}")
    if fixture.is_strict:
        if not fixture.expected_diagnostic_code:
            raise RuntimeError(
                f"strict fixture must declare diagnostic code: {fixture.relative_metadata}"
            )
        if not fixture.required_tokens:
            raise RuntimeError(
                f"strict fixture must declare diagnostic tokens: {fixture.relative_metadata}"
            )
    else:
        if fixture.expected_diagnostic_code:
            raise RuntimeError(
                f"positive fixture must not declare diagnostic code: {fixture.relative_metadata}"
            )
        if fixture.required_tokens:
            raise RuntimeError(
                f"positive fixture must not declare diagnostic tokens: {fixture.relative_metadata}"
            )

    retired_tags = set(fixture.retired_surface_tags)
    if len(retired_tags) != len(fixture.retired_surface_tags):
        raise RuntimeError(
            f"retired_surface_tags must not contain duplicates in {fixture.relative_metadata}"
        )
    unknown_tags = retired_tags - RETIRED_SURFACE_TAGS
    if unknown_tags:
        raise RuntimeError(
            f"unknown retired_surface_tags {sorted(unknown_tags)!r} in {fixture.relative_metadata}"
        )
    if retired_tags and not fixture.is_strict:
        raise RuntimeError(f"retired surfaces must be strict fixtures: {fixture.relative_metadata}")

    execution = fixture.execution
    requires_live_runtime_dispatch = execution.get("requires_live_runtime_dispatch")
    if requires_live_runtime_dispatch is not None and not isinstance(
        requires_live_runtime_dispatch,
        bool,
    ):
        raise RuntimeError(
            f"requires_live_runtime_dispatch must be a boolean in {fixture.relative_metadata}"
        )
    expected_exit_code = execution.get("expected_exit_code")
    if expected_exit_code is not None and (
        not isinstance(expected_exit_code, int) or isinstance(expected_exit_code, bool)
    ):
        raise RuntimeError(
            f"expected_exit_code must be an integer in {fixture.relative_metadata}"
        )
