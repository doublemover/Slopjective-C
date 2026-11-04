"""Interrupted scans retain identical verified native actions without false green."""
import copy
import json
from pathlib import Path
import sys

import pytest

from scripts.objc3c_toolchain.analysis_checkpoint import run_unit, successful_action
from scripts.objc3c_toolchain.binary_packages import digest


@pytest.fixture
def checkpoint(tmp_path):
    root = tmp_path / 'previous';root.mkdir()
    current_root = tmp_path / 'current';current_root.mkdir()
    tool = tmp_path / 'matched-tool';tool.write_bytes(b'exact matched tool')
    source, header = tmp_path / 'source.cpp', tmp_path / 'header.h'
    source.write_text('int function();');header.write_text('header')
    config = root / 'checks.json';config.write_text('{"Checks":"reviewed-check"}')
    current_config = current_root / 'checks.json';current_config.write_bytes(config.read_bytes())
    identity = 'a'*64;name = 'frontend-'+identity[:16]
    directory = root / name;directory.mkdir()
    args = ['clang++', '-DREAL_FLAG=1', '-c', str(source), '-o', str(tmp_path / 'source.o')]
    row = {'identity': identity, 'owner': 'frontend', 'file': str(source), 'directory': str(tmp_path),
        'arguments': args, 'analysis_arguments': args, 'output': str(tmp_path / 'source.o'),
        'response_files': {}, 'build_launcher': {}, 'dependencies': [str(source), str(header)]}
    (directory / 'compile_commands.json').write_text(json.dumps([
        {'file': str(source), 'directory': str(tmp_path), 'arguments': args}]))
    logs = root / 'logs';logs.mkdir();log = logs / (name+'.log');log.write_text('')
    command = {'name': name, 'argv': [str(tool), '-p', str(directory), str(source), '--config-file='+str(config),
        '--warnings-as-errors=*', '--extra-arg=-resource-dir=/matched/resources',
        '--export-fixes='+str(directory / 'diagnostics.yaml')], 'exit_code': 0, 'expected_exit_code': 0,
        'timed_out': False, 'status': 'PASS', 'log': str(log), 'log_sha256': digest(log)}
    previous = {'schema': 'objc3c.implementation-analysis.v1', 'status': 'FAIL', 'policy': {'checks': ['reviewed-check']},
        'tool': {'path': str(tool), 'sha256': digest(tool)}, 'tool_graph': {'libraries': {}},
        'inventory': [copy.deepcopy(row)], 'results': [{'identity': identity, 'status': 'PASS'}],
        'commands': [command], 'inputs': {str(path): digest(path) for path in (source, header, tool, config)}}
    current = {key: copy.deepcopy(previous[key]) for key in ('policy', 'tool', 'tool_graph')}
    current.update(configuration=str(current_config), resource_directory='/matched/resources', repository_output=str(tmp_path))
    receipt = root / 'qualification.json';receipt.write_text(json.dumps(previous))
    return receipt, previous, current, row, args, header, log


def test_failed_campaign_reuses_only_its_completed_identical_native_action(checkpoint):
    receipt, previous, current, row, args, _, _ = checkpoint
    result = successful_action(receipt, previous, current, row, arguments=args)
    assert result['status'] == 'PASS' and result['execution_reused'] is True
    assert result['checkpoint_sha256'] == digest(receipt)
    assert previous['status'] == 'FAIL'


@pytest.mark.parametrize('mutation', ['header', 'flags', 'checker', 'tool', 'failed', 'missing-result'])
def test_changed_action_inputs_or_unfinished_work_are_cache_misses(checkpoint, mutation):
    receipt, previous, current, row, args, header, _ = checkpoint
    if mutation == 'header': header.write_text('changed header')
    elif mutation == 'flags': args = [*args, '-DWRONG_FLAG=1']
    elif mutation == 'checker': current['policy']['checks'].append('new-check')
    elif mutation == 'tool': current['tool']['sha256'] = 'b'*64
    elif mutation == 'failed': previous['results'][0]['status'] = 'FAIL'
    else: previous['results'] = []
    assert successful_action(receipt, previous, current, row, arguments=args) is None


@pytest.mark.parametrize('mutation', ['log', 'argv', 'database', 'timeout', 'duplicate', 'config', 'running'])
def test_corrupt_checkpoint_proof_cannot_pass(checkpoint, mutation):
    receipt, previous, current, row, args, _, log = checkpoint
    if mutation == 'log': log.write_text('changed diagnostic')
    elif mutation == 'argv': previous['commands'][0]['argv'].append('--disable-checks')
    elif mutation == 'database':
        path = receipt.parent / ('frontend-'+'a'*16) / 'compile_commands.json';path.write_text('[]')
    elif mutation == 'timeout': previous['commands'][0]['timed_out'] = True
    elif mutation == 'duplicate': previous['commands'].append(previous['commands'][0])
    elif mutation == 'config': (receipt.parent / 'checks.json').write_text('different checks')
    else: previous['status'] = 'running'
    with pytest.raises(ValueError): successful_action(receipt, previous, current, row, arguments=args)


def test_spawned_unit_process_retains_actual_command_failure(tmp_path):
    result, commands = run_unit({'directory': str(tmp_path), 'environment': {}, 'identity': 'unit',
        'name': 'native-control', 'timeout': 10, 'argv': [sys.executable, '-c', 'print("forced failure");raise SystemExit(1)']})
    assert result['status'] == 'FAIL' and commands[0]['exit_code'] == 1
    assert json.loads((tmp_path / 'qualification.json').read_text())['status'] == 'FAIL'
