"""Shared data models for new-work proposal publication."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Mapping


JsonObject = dict[str, Any]


@dataclass(frozen=True)
class ProposalPolicyInputs:
    template: Mapping[str, Any]
    governance_policy: Mapping[str, Any]
    waiver_registry: Mapping[str, Any]
    extension_policy: Mapping[str, Any]


@dataclass(frozen=True)
class ProposalValidationResult:
    resolved: JsonObject
    failures: list[str]


@dataclass(frozen=True)
class ProposalRenderResult:
    title: str
    body: str
    payload: JsonObject
