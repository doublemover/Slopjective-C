from __future__ import annotations

import pytest
import os
from pathlib import Path
import subprocess
import sys

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


def test_independent_linux_helpers_import_without_optional_action_tools(tmp_path):
    root = Path(__file__).resolve().parents[2]
    source = ('import sys;sys.path.insert(0, '+repr(str(root))+');'
        'from scripts.objc3c_workflow import environment_tool_lookup;'
        'from scripts.objc3c_workflow.command_result_acceptance import accepted_action;'
        'assert accepted_action("lint", 0).accepted;'
        'assert "scripts.objc3c_workflow.action_catalog" not in sys.modules;'
        'print("independent Linux helpers imported")')
    result = subprocess.run([sys.executable, '-I', '-c', source], cwd=tmp_path,
        env={**os.environ, 'PATH': ''}, capture_output=True, text=True, timeout=30)
    assert result.returncode == 0, result.stderr
    assert result.stdout.strip() == 'independent Linux helpers imported'


@pytest.mark.parametrize('tool', ['pwsh', 'npx'])
def test_optional_action_tools_are_required_at_execution_and_never_launched_unresolved(monkeypatch, tool):
    from scripts.objc3c_workflow.command_execution_policy import command_for_execution
    monkeypatch.setattr(lookup.shutil, 'which', lambda name: '/tools/'+name)
    assert command_for_execution([tool, 'argument']) == ('/tools/'+tool, 'argument')
    monkeypatch.setattr(lookup.shutil, 'which', lambda name: None)
    with pytest.raises(RuntimeError, match='required workflow tool not found'):
        command_for_execution([tool, 'argument'])
