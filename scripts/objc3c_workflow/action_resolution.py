"""Registered action resolution facade for workflow dispatch."""

from __future__ import annotations

from scripts.objc3c_workflow.action_acceptance import resolve_registered_action
from scripts.objc3c_workflow.action_execution import execute_registered_handler


__all__ = ["execute_registered_handler", "resolve_registered_action"]
