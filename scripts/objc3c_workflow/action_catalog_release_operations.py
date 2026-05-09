"""Release operations action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

RELEASE_OPERATIONS_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-release-operations-surface": ActionSpec("check-release-operations-surface", "validate the checked-in release-operations source surface", "python:scripts/check_release_operations_source_surface.py", validation_tier="repo", guarantee_owner="release operations only publish from the checked-in versioning, upgrade-warning, rollback, and channel policy contracts"),
    "check-release-operations-schema-surface": ActionSpec("check-release-operations-schema-surface", "validate the checked-in release-operations schema surface", "python:scripts/check_release_operations_schema_surface.py", validation_tier="repo", guarantee_owner="update manifest and release-operations artifacts stay on checked-in schema contracts"),
    "build-update-manifest": ActionSpec("build-update-manifest", "derive the machine-owned update manifest from the live packaging-channel and release-foundation outputs", "python:scripts/build_objc3c_update_manifest.py", validation_tier="repo", guarantee_owner="versioned channel metadata stays tied to the live release and packaging surfaces"),
    "publish-release-operations": ActionSpec("publish-release-operations", "publish the machine-owned release-operations report and channel catalog", "python:scripts/publish_objc3c_release_operations_metadata.py", validation_tier="repo", guarantee_owner="release-operations publication stays traceable to the checked-in upgrade-warning, rollback, and channel policy contracts"),
    "validate-release-operations": ActionSpec("validate-release-operations", "run the integrated release-operations workflow", "runner-internal release-operations child actions", validation_tier="nightly", guarantee_owner="versioning, upgrade warnings, rollback guidance, and update metadata stay executable on the live release surfaces"),
    "validate-release-operations-end-to-end": ActionSpec("validate-release-operations-end-to-end", "validate release-operations entrypoints, generated metadata, and packaged channel references end to end", "python:scripts/check_objc3c_release_operations_end_to_end.py", validation_tier="full", guarantee_owner="release-operations metadata stays coherent with the live package-channel artifacts and rollback paths"),
}


__all__ = ["RELEASE_OPERATIONS_ACTION_SPECS"]
