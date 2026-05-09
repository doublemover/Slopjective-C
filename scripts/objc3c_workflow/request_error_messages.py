"""Workflow request error message construction."""

from __future__ import annotations


def unknown_action_message(action: str) -> str:
    return f"unknown action: {action}"


def unknown_package_script_message(package_script: str) -> str:
    return f"unknown package script: {package_script}"


__all__ = ["unknown_action_message", "unknown_package_script_message"]
