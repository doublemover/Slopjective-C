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


def blocks(path, module, operations=('plan', 'run', 'publish', 'verify')):
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
        selected = set(re.findall(
            r'-m ' + re.escape(module) + r' (' + '|'.join(map(re.escape, operations)) + r')\b', block))
        for operation in selected:
            commands.setdefault(operation, []).append(block)
    return commands


@pytest.mark.parametrize('status', [0, 1, 7, 124])
def test_retained_coverage_diagnosis_preserves_the_aggregate_exit_status(tmp_path, status):
    commands = blocks(ROOT / '.github/workflows/native-assurance.yml',
                      'scripts.objc3c_assurance.coverage_matrix', ('aggregate',))['aggregate']
    diagnostic = [command for command in commands if 'Retained coverage aggregate' in command]
    assert len(diagnostic) == 1
    recorder = tmp_path / 'record.py'
    recorder.write_text('import json,os,sys\n'
                        'from pathlib import Path\n'
                        'Path(os.environ["ARGV_FILE"]).write_text(json.dumps(sys.argv[1:]))\n'
                        'raise SystemExit(int(os.environ["AGGREGATE_STATUS"]))\n')
    output = tmp_path / 'argv.json'
    result = subprocess.run(['bash', '-e', '-o', 'pipefail', '-c',
        'python3() { "$TEST_PYTHON" "$ARGV_RECORDER" "$@"; }\n' + diagnostic[0]],
        env={**os.environ, 'TEST_PYTHON': sys.executable, 'ARGV_RECORDER': str(recorder),
             'ARGV_FILE': str(output), 'AGGREGATE_STATUS': str(status)},
        capture_output=True, text=True, timeout=15)
    assert result.returncode == status, result.stdout + result.stderr
    assert json.loads(output.read_text()) == [
        '-B', '-m', 'scripts.objc3c_assurance.coverage_matrix', 'aggregate',
        '--plan', 'out/native-assurance/selection/plan.json',
        '--producer-root', 'out/coverage-matrix/producer-result',
        '--partitions-root', 'out/coverage-matrix/partitions',
        '--output', 'out/native-assurance/results']
    assert ('::error' if status else '::notice') in result.stdout


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
    for operation, variants in commands.items():
        for index, command in enumerate(variants):
            output = tmp_path / (operation + '-' + str(index) + '.jsonl')
            subprocess.run(['bash', '-e', '-o', 'pipefail', '-c', prefix + command], check=True,
                env={**os.environ, 'TEST_PYTHON': sys.executable, 'ARGV_RECORDER': str(recorder),
                     'ARGV_FILE': str(output), 'GITHUB_EVENT_NAME': event, 'GITHUB_SHA': SHA,
                     'SOURCE_SHA': called_sha, 'NATIVE_SHARD': 'core', 'GITHUB_WORKSPACE': str(tmp_path),
                     'PLAN_RESULT': 'success', 'PRODUCER_RESULT': 'success',
                     'CONSUMERS_RESULT': 'success', 'ASAN_PRODUCER_RESULT': 'success',
                     'INSTRUMENTED_CONSUMERS_RESULT': 'success', 'FUZZ_RESULT': 'success',
                     'COVERAGE_PRODUCER_RESULT': 'success',
                     'COVERAGE_PARTITIONS_RESULT': 'success', 'CORE_RESULT': 'success',
                     'COVERAGE_RESULT': 'success'},
                timeout=15)
            recorded = [json.loads(line) for line in output.read_text().splitlines()]
            matching = [argv for argv in recorded if module in argv and operation in argv]
            assert len(matching) == 1
            argv = matching[0]
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
