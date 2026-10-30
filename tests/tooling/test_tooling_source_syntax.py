"""Catch broken Python anywhere in tooling discovery before selecting test lanes."""
import ast
from pathlib import Path


def test_every_tooling_test_module_parses():
    files = sorted(Path(__file__).parent.glob('test_*.py'))
    assert files, 'tooling discovery unexpectedly selected no Python modules'
    for path in files:
        ast.parse(path.read_text(), filename=str(path))
