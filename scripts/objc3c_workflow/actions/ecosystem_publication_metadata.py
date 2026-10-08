"""Package and feed metadata for ecosystem publication actions."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class PublicationFeedMetadata:
    feed_name: str
    action_names: tuple[str, ...]
    owner: str


PACKAGE_FEED_METADATA: dict[str, PublicationFeedMetadata] = {
    "package-ecosystem": PublicationFeedMetadata(
        "package-ecosystem",
        (
            "build-package-lock",
            "package-sign",
            "package-verify",
            "validate-package-security-hardening",
            "validate-direct-import-module-syntax",
            "validate-package-manager-model",
            "validate-package-authoring",
            "validate-package-mirror",
            "validate-package-registry-model",
            "package-registry-resolve",
            "validate-package-network-publication",
            "validate-package-install-distribution",
            "package-publish",
            "package-install",
            "package-update",
            "package-uninstall",
            "package-rollback",
            "validate-package-operations",
            "validate-package-ecosystem",
            "validate-runnable-package-ecosystem",
        ),
        "direct import, hosted registry/model, network publication, signing and verification, package operations, install distribution, and runnable package ecosystem validation",
    ),
    "long-horizon-operations": PublicationFeedMetadata(
        "long-horizon-operations",
        (
            "validate-long-horizon-operations",
            "publish-long-horizon-operations",
        ),
        "long-horizon operations metadata publication",
    ),
    "adoption-legibility": PublicationFeedMetadata(
        "adoption-legibility",
        (
            "validate-adoption-legibility",
            "publish-adoption-legibility",
        ),
        "adoption legibility metadata publication",
    ),
    "governance-sustainability": PublicationFeedMetadata(
        "governance-sustainability",
        (
            "validate-governance-sustainability",
            "publish-governance-sustainability",
        ),
        "governance sustainability metadata publication",
    ),
    "planning-publication": PublicationFeedMetadata(
        "planning-publication",
        (
            "publish-planning-issues",
            "check-planning-publication-drift",
        ),
        "planning issue publication and drift audit",
    ),
}

ACTION_FEEDS: dict[str, str] = {
    action_name: feed.feed_name
    for feed in PACKAGE_FEED_METADATA.values()
    for action_name in feed.action_names
}
