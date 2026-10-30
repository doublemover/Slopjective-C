from __future__ import annotations

import pytest

from scripts.objc3c_workflow import environment_tool_lookup as lookup
from scripts.objc3c_workflow.environment_tool_registry import WorkflowToolSpec


def test_required_tool_uses_the_first_available_declared_candidate(monkeypatch):
    monkeypatch.setattr(lookup.shutil, 'which', lambda name: '/tools/clang' if name == 'clang' else None)
    spec = WorkflowToolSpec('compiler', ('missing', 'clang'))
    assert lookup.resolve_required_tool(spec) == '/tools/clang'


def test_missing_required_tool_fails_instead_of_returning_an_unresolved_name(monkeypatch):
    monkeypatch.setattr(lookup.shutil, 'which', lambda name: None)
    with pytest.raises(RuntimeError, match='required workflow tool not found for compiler'):
        lookup.resolve_required_tool(WorkflowToolSpec('compiler', ('missing',)))


def test_unknown_registry_entry_is_rejected():
    with pytest.raises(KeyError, match='unknown required workflow tool'):
        lookup.required_workflow_tool('__unregistered__')
