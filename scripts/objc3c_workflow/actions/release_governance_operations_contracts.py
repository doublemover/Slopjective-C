"""Release-operations action contracts."""

from __future__ import annotations

from .release_governance_owner_models import ReleaseGovernanceActionContract


RELEASE_OPERATIONS_ACTION_CONTRACTS: tuple[ReleaseGovernanceActionContract, ...] = (
    ReleaseGovernanceActionContract(
        "check-release-operations-surface",
        "validate the checked-in release-operations source surface",
        "python:scripts/check_release_operations_source_surface.py",
        "release-operations",
        "repo",
        (
            "release operations publish only from checked-in versioning, upgrade, "
            "diagnostics, and channel policy contracts"
        ),
    ),
    ReleaseGovernanceActionContract(
        "check-release-operations-schema-surface",
        "validate the checked-in release-operations schema surface",
        "python:scripts/check_release_operations_schema_surface.py",
        "release-operations",
        "repo",
        "update manifest and upgrade-support artifacts stay on checked-in schemas",
    ),
    ReleaseGovernanceActionContract(
        "build-update-manifest",
        "derive the machine-owned update and release-channel manifests from existing release, package-channel, and platform-support artifacts",
        "python:scripts/build_objc3c_update_manifest.py",
        "release-operations",
        "repo",
        "versioned channel metadata and local provenance fail closed when required upstream artifacts are absent",
    ),
    ReleaseGovernanceActionContract(
        "publish-release-operations",
        "publish the machine-owned upgrade-support report, channel catalog, and release-channel evidence",
        "python:scripts/publish_objc3c_release_operations_metadata.py",
        "release-operations",
        "repo",
        (
            "release-operations publication stays traceable to checked-in upgrade, "
            "rollback, diagnostics, and channel policy contracts"
        ),
    ),
    ReleaseGovernanceActionContract(
        "validate-release-operations",
        "run the integrated release-operations workflow",
        "runner-internal release-operations child actions",
        "release-operations",
        "nightly",
        (
            "versioning, upgrade warnings, rollback guidance, update metadata, channel manifests, and "
            "public action ownership stay executable on live release surfaces"
        ),
    ),
    ReleaseGovernanceActionContract(
        "validate-release-operations-end-to-end",
        "validate release-operations entrypoints, generated metadata, and packaged channel references end to end",
        "python:scripts/check_objc3c_release_operations_end_to_end.py",
        "release-operations",
        "full",
        (
            "release-operations metadata stays coherent with live package-channel "
            "artifacts, local provenance, and fail-closed rollback paths"
        ),
    ),
)


__all__ = ["RELEASE_OPERATIONS_ACTION_CONTRACTS"]
