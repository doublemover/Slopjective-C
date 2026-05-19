"""Release-foundation action contracts."""

from __future__ import annotations

from .release_governance_owner_models import ReleaseGovernanceActionContract


RELEASE_FOUNDATION_ACTION_CONTRACTS: tuple[ReleaseGovernanceActionContract, ...] = (
    ReleaseGovernanceActionContract(
        "check-release-foundation-surface",
        "validate the checked-in release-foundation source surface",
        (
            "runner-internal + check-repo-superclean-surface + "
            "python:scripts/check_release_foundation_source_surface.py"
        ),
        "release-foundation",
        "repo",
        (
            "release foundation publishes only from checked-in taxonomy, trust, "
            "payload, and provenance contracts"
        ),
    ),
    ReleaseGovernanceActionContract(
        "check-release-foundation-schema-surface",
        "validate the checked-in release-foundation schema surface",
        "python:scripts/check_release_foundation_schema_surface.py",
        "release-foundation",
        "repo",
        "release manifest, sbom, and attestation artifacts stay on checked-in schemas",
    ),
    ReleaseGovernanceActionContract(
        "build-release-manifest",
        "derive the machine-owned release manifest from repeated runnable package assembly runs",
        "python:scripts/build_objc3c_release_manifest.py",
        "release-foundation",
        "repo",
        (
            "release payload selection and reproducibility proof stay tied to the "
            "live runnable package manifest"
        ),
    ),
    ReleaseGovernanceActionContract(
        "publish-release-provenance",
        "publish the machine-owned release sbom and attestation artifacts",
        "python:scripts/publish_objc3c_release_provenance.py",
        "release-foundation",
        "repo",
        (
            "release provenance publication stays traceable to the live manifest, "
            "package manifest, and release-evidence index"
        ),
    ),
    ReleaseGovernanceActionContract(
        "validate-release-foundation",
        "run the integrated release-foundation workflow",
        "runner-internal release-foundation child actions",
        "release-foundation",
        "nightly",
        (
            "release taxonomy, reproducible package assembly, and provenance "
            "publication stay executable on the live runnable package surface"
        ),
    ),
)


__all__ = ["RELEASE_FOUNDATION_ACTION_CONTRACTS"]
