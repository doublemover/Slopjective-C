"""Exercise the actual pytest plugin in a child interpreter."""
import json
import os
from pathlib import Path
import subprocess
import sys

import pytest

REPO = Path(__file__).resolve().parents[2]


@pytest.mark.parametrize('mode', ['pass', 'failed', 'collection-error', 'empty'])
def test_actual_collection_phase_outcomes_and_skip_reasons(tmp_path, mode):
    source = tmp_path / 'test_cases.py'
    if mode == 'collection-error':
        source.write_text('raise RuntimeError("broken collection")')
    elif mode == 'empty':
        source.write_text('# no tests')
    else:
        source.write_text('import pytest\n'
            'def test_good(): pass\n'
            '@pytest.mark.skip(reason="native host unavailable; tracked capability exclusion")\n'
            'def test_explicit_skip(): pass\n'
            '@pytest.mark.xfail(reason="tracked expected rejection")\n'
            'def test_expected_failure(): assert False\n'
            'def test_unselected(): pass\n' +
            ('def test_bad(): assert False, "native mock boundary failed"\n' if mode == 'failed' else ''))
    receipt = tmp_path / 'inventory.json'
    result = subprocess.run([sys.executable, '-m', 'pytest', '-q', '-k', 'not unselected',
        '-p', 'scripts.objc3c_toolchain.pytest_inventory', '--execution-inventory', str(receipt), str(source)],
        cwd=tmp_path, env={**os.environ, 'PYTHONPATH': str(REPO), 'PYTEST_DISABLE_PLUGIN_AUTOLOAD': '1'},
        capture_output=True, text=True, timeout=30)
    assert receipt.is_file(), result.stdout + result.stderr
    data = json.loads(receipt.read_text())
    assert data['execution'] == 'unit-contract' and data['exit_code'] == result.returncode
    assert data['complete'] == (mode == 'pass')
    if mode in ('pass', 'failed'):
        assert any('unselected' in name for name in data['deselected'])
        assert not any('unselected' in name for name in data['selected'])
        assert any('tracked capability exclusion' in row.get('reason', '') for row in data['reports'])
        assert any(row.get('xfail') == 'tracked expected rejection' for row in data['reports'])
        assert all('phase' in row for row in data['reports'])
    if mode == 'collection-error':
        assert 'broken collection' in data['collection_errors'][0]['reason']
