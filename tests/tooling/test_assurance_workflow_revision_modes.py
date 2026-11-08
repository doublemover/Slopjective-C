"""Execute the workflow command blocks so every phase uses the same source mode."""
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import textwrap

import pytest

ROOT = Path(__file__).resolve().parents[2]
SHA = 'a' * 40
CALLED_SHA = 'b' * 40


def blocks(path, module):
    source = path.read_text()
    commands = {}
    lines = source.splitlines()
    for index, line in enumerate(lines):
        match = re.fullmatch(r'(\s*)run: \|', line)
        if not match:
            continue
        indentation = len(match.group(1))
        end = index + 1
        while end < len(lines) and (not lines[end].strip() or
                len(lines[end]) - len(lines[end].lstrip()) > indentation):
            end += 1
        block = textwrap.dedent('\n'.join(lines[index + 1:end]))
        operation = re.search(r'-m ' + re.escape(module) + r' (plan|run|publish|verify)\b', block)
        if operation:
            assert operation[1] not in commands
            commands[operation[1]] = block
    return commands


@pytest.mark.parametrize('module,workflow,operations', [
    ('scripts.objc3c_assurance', 'native-assurance.yml', {'plan', 'run', 'publish', 'verify'}),
    ('scripts.objc3c_implementation_assurance', 'implementation-analysis.yml', {'plan', 'run', 'verify'}),
])
@pytest.mark.parametrize('event,called_sha', [
    ('pull_request', ''), ('workflow_dispatch', ''), ('schedule', ''),
    ('workflow_dispatch', CALLED_SHA),
])
def test_each_workflow_phase_preserves_the_authoritative_revision_mode(
        tmp_path, module, workflow, operations, event, called_sha):
    if module.endswith('implementation_assurance'):
        source = (ROOT / '.github/workflows' / workflow).read_text()
        job_environment = source.split('\n  analysis:\n', 1)[1].split('    steps:', 1)[0]
        assert 'SOURCE_SHA: ${{ inputs.source_sha }}' in job_environment
    commands = blocks(ROOT / '.github/workflows' / workflow, module)
    assert set(commands) == operations
    recorder = tmp_path / 'record.py'
    recorder.write_text('import json,os,sys\nwith open(os.environ["ARGV_FILE"],"a") as f:f.write(json.dumps(sys.argv[1:])+"\\n")\n')
    prefix = '''source() { :; }
git() { :; }
ulimit() { :; }
python3() { "$TEST_PYTHON" "$ARGV_RECORDER" "$@"; }
'''
    for operation, command in commands.items():
        output = tmp_path / (operation + '.jsonl')
        subprocess.run(['bash', '-e', '-o', 'pipefail', '-c', prefix + command], check=True,
            env={**os.environ, 'TEST_PYTHON': sys.executable, 'ARGV_RECORDER': str(recorder),
                 'ARGV_FILE': str(output), 'GITHUB_EVENT_NAME': event, 'GITHUB_SHA': SHA,
                 'SOURCE_SHA': called_sha, 'NATIVE_SHARD': 'core', 'GITHUB_WORKSPACE': str(tmp_path)},
            timeout=15)
        argv = json.loads(output.read_text().strip())
        if module.endswith('implementation_assurance') and called_sha:
            expected = CALLED_SHA
        elif event in ('workflow_dispatch', 'schedule'):
            expected = SHA
        else:
            expected = None
        if expected is None:
            assert '--periodic-sha' not in argv
        else:
            assert argv[argv.index('--periodic-sha') + 1] == expected
