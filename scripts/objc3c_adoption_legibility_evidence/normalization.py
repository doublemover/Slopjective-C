from __future__ import annotations

from typing import Any


def comparison_axes(comparison_semantics: dict[str, Any]) -> list[str]:
    return [
        str(axis.get("axis_id"))
        for axis in comparison_semantics.get("comparison_axes", [])
        if isinstance(axis, dict)
    ]


def comparison_evidence_paths(comparison_semantics: dict[str, Any]) -> list[str]:
    return sorted(
        {
            str(path)
            for axis in comparison_semantics.get("comparison_axes", [])
            if isinstance(axis, dict)
            for path in axis.get("required_evidence", [])
        }
    )


def adoption_replay_phases(adoption_replay_semantics: dict[str, Any]) -> list[str]:
    return [
        str(phase.get("phase_id"))
        for phase in adoption_replay_semantics.get("adoption_replay_phases", [])
        if isinstance(phase, dict)
    ]


def adoption_replay_actions(adoption_replay_semantics: dict[str, Any]) -> list[str]:
    return sorted(
        {
            str(action)
            for phase in adoption_replay_semantics.get("adoption_replay_phases", [])
            if isinstance(phase, dict)
            for action in phase.get("required_actions", [])
        }
    )


def interop_axes(adoption_replay_semantics: dict[str, Any]) -> list[str]:
    return [
        str(axis.get("axis_id"))
        for axis in adoption_replay_semantics.get("interop_guidance_axes", [])
        if isinstance(axis, dict)
    ]


def package_actions(required_actions: list[str]) -> list[str]:
    return [
        action
        for action in required_actions
        if "package" in action or "application" in action or "release" in action or "long-horizon" in action
    ]
