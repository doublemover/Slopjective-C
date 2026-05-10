"""Artifact registry evidence events and summary serialization."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from .artifact_registry_keys import ARTIFACT_REGISTRY_REUSE_MODEL
from .artifact_registry_keys import ARTIFACT_REGISTRY_SUMMARY_CONTRACT_ID


@dataclass(frozen=True)
class ArtifactRegistryMissEvent:
    cache_key_sha256: str
    case: str | None
    fixture: object
    extra_args: object
    reuse_policy: str
    reason: str

    def payload(self) -> dict[str, Any]:
        return {
            "cache_key_sha256": self.cache_key_sha256,
            "case": self.case,
            "fixture": self.fixture,
            "extra_args": self.extra_args,
            "reuse_policy": self.reuse_policy,
            "reason": self.reason,
        }


@dataclass(frozen=True)
class ArtifactRegistryReuseEvent:
    cache_key_sha256: str
    producer_case: object
    consumer_case: str | None
    fixture: object
    extra_args: object
    producer_dir: object
    consumer_dir: str
    reuse_policy: str
    artifact_paths: list[str]

    def payload(self) -> dict[str, Any]:
        return {
            "cache_key_sha256": self.cache_key_sha256,
            "producer_case": self.producer_case,
            "consumer_case": self.consumer_case,
            "fixture": self.fixture,
            "extra_args": self.extra_args,
            "producer_dir": self.producer_dir,
            "consumer_dir": self.consumer_dir,
            "reuse_policy": self.reuse_policy,
            "artifact_paths": self.artifact_paths,
        }


@dataclass(frozen=True)
class ArtifactRegistryEntry:
    cache_key_sha256: str
    producer_case: str | None
    fixture: object
    extra_args: object
    producer_dir: str
    reuse_policy: str
    artifact_paths: list[str]

    def payload(self) -> dict[str, Any]:
        return {
            "cache_key_sha256": self.cache_key_sha256,
            "producer_case": self.producer_case,
            "fixture": self.fixture,
            "extra_args": self.extra_args,
            "producer_dir": self.producer_dir,
            "reuse_policy": self.reuse_policy,
            "artifact_paths": self.artifact_paths,
        }


class ArtifactRegistryEvidence:
    def __init__(self) -> None:
        self.entries: dict[str, dict[str, Any]] = {}
        self.reuse_events: list[dict[str, Any]] = []
        self.miss_events: list[dict[str, Any]] = []

    def record_miss(
        self,
        *,
        cache_key_sha256: str,
        current_case: str | None,
        fixture: object,
        extra_args: object,
        reuse_policy: str,
        reason: str,
    ) -> None:
        self.miss_events.append(
            ArtifactRegistryMissEvent(
                cache_key_sha256=cache_key_sha256,
                case=current_case,
                fixture=fixture,
                extra_args=extra_args,
                reuse_policy=reuse_policy,
                reason=reason,
            ).payload()
        )

    def record_reuse(
        self,
        *,
        cache_key_sha256: str,
        producer_case: object,
        consumer_case: str | None,
        fixture: object,
        extra_args: object,
        producer_dir: object,
        consumer_dir: str,
        reuse_policy: str,
        artifact_paths: list[str],
    ) -> dict[str, Any]:
        event = ArtifactRegistryReuseEvent(
            cache_key_sha256=cache_key_sha256,
            producer_case=producer_case,
            consumer_case=consumer_case,
            fixture=fixture,
            extra_args=extra_args,
            producer_dir=producer_dir,
            consumer_dir=consumer_dir,
            reuse_policy=reuse_policy,
            artifact_paths=artifact_paths,
        ).payload()
        self.reuse_events.append(event)
        return event

    def register_entry(
        self,
        *,
        cache_key_sha256: str,
        producer_case: str | None,
        fixture: object,
        extra_args: object,
        producer_dir: str,
        reuse_policy: str,
        artifact_paths: list[str],
    ) -> None:
        self.entries.setdefault(
            cache_key_sha256,
            ArtifactRegistryEntry(
                cache_key_sha256=cache_key_sha256,
                producer_case=producer_case,
                fixture=fixture,
                extra_args=extra_args,
                producer_dir=producer_dir,
                reuse_policy=reuse_policy,
                artifact_paths=artifact_paths,
            ).payload(),
        )

    def summary(self) -> dict[str, Any]:
        return {
            "contract_id": ARTIFACT_REGISTRY_SUMMARY_CONTRACT_ID,
            "entry_count": len(self.entries),
            "reuse_count": len(self.reuse_events),
            "miss_count": len(self.miss_events),
            "entries": list(self.entries.values()),
            "reuse_events": self.reuse_events,
            "miss_events": self.miss_events,
            "reuse_model": ARTIFACT_REGISTRY_REUSE_MODEL,
        }


__all__ = [
    "ArtifactRegistryEntry",
    "ArtifactRegistryEvidence",
    "ArtifactRegistryMissEvent",
    "ArtifactRegistryReuseEvent",
]
