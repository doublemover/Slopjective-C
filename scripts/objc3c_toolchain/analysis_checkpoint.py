"""Reuse only independently verified identical successful analysis actions."""
from __future__ import annotations

import json
from pathlib import Path
import re

from .binary_packages import digest
from .llvm_ir_execution import Execution, StageFailure
from .sanitizer_policy import require_clean_diagnostic


def analysis_argv(tool, directory, source, configuration, resource_directory):
    directory = Path(directory)
    return list(map(str, [tool, '-p', directory, source, '--config-file='+str(configuration),
        '--warnings-as-errors=*', '--enable-check-profile',
        '--extra-arg=-resource-dir='+str(resource_directory),
        '--export-fixes='+str(directory / 'diagnostics.yaml')]))


def tidy_diagnostics(text):
    """Retain blocker locations/checkers without treating tool prose as data."""
    text = re.sub(r'\x1b\[[0-9;]*m', '', text)
    pattern = re.compile(
        r'^(?P<path>[^\n]+?):(?P<line>\d+):(?P<column>\d+): '
        r'(?P<severity>fatal error|warning|error): (?P<message>.*?)'
        r'(?: \[(?P<checks>[A-Za-z0-9_., -]+)\])?$', re.M)
    diagnostics = []
    for match in pattern.finditer(text):
        checks = [check.strip() for check in (match['checks'] or '').split(',')
                  if check.strip() and check.strip() != '-warnings-as-errors']
        for check in checks or ['clang-diagnostic-'+match['severity'].replace(' ', '-')]:
            diagnostics.append({'path': match['path'], 'line': int(match['line']),
                'column': int(match['column']), 'severity': match['severity'],
                'message': match['message'], 'check': check})
    return diagnostics


def run_unit(job):
    """One spawned process per worker: no pre-exec callback in a threaded fork."""
    root = Path(job['directory'])
    report = {'schema': 'objc3c.implementation-analysis-unit.v1', 'status': 'running', 'commands': []}
    executor = Execution(root, job['environment'], report, timeout=job['timeout'])
    result = {'identity': job['identity'], 'status': 'PASS', 'execution_reused': False}
    try:
        executor.run(job['name'], job['argv'])
        report['status'] = 'PASS'
    except StageFailure as error:
        report.update(status='FAIL', error=str(error))
        result.update(status='FAIL', classification=error.classification, error=str(error))
    finally:
        executor.save()
    commands = report['commands']
    diagnostics = tidy_diagnostics(Path(commands[-1]['log']).read_text(errors='replace')) if commands else []
    result['diagnostics'] = diagnostics
    if diagnostics:
        result.update(status='FAIL', classification='finding',
                      error='implementation analysis reported blocker diagnostics')
        report.update(status='FAIL', error=result['error'])
        executor.save()
    return result, report['commands']


def successful_action(checkpoint, previous, current, row, *, arguments, _depth=0):
    """A failed/partial campaign may contain completed, immutable TU actions.

    This reuses analysis, never compiler objects or a prior aggregate PASS.
    Changed effective source/header/flag/tool/checker inputs are cache misses.
    Corrupted or unowned proof files are errors, never accepted cache hits.
    """
    root = checkpoint.parent.resolve(strict=True)
    if previous.get('schema') != 'objc3c.implementation-analysis.v1' or previous.get('status') not in ('PASS', 'FAIL'):
        raise ValueError('analysis checkpoint has no terminal source-owned attempt')
    if (previous.get('policy') != current['policy'] or previous.get('tool') != current['tool'] or
            previous.get('tool_graph', {}).get('libraries') != current['tool_graph']['libraries']):
        return None
    result = [item for item in previous.get('results', []) if item.get('identity') == row['identity']]
    if len(result) != 1 or result[0].get('status') != 'PASS': return None
    if result[0].get('execution_reused'):
        origin = Path(result[0]['checkpoint']).resolve(strict=True)
        if (_depth >= 4 or origin == checkpoint or not origin.is_relative_to(Path(current['repository_output'])) or
                origin.stat().st_size > 64*1024**2 or digest(origin) != result[0].get('checkpoint_sha256')):
            raise ValueError('analysis checkpoint origin is cyclic, escaped, changed or excessive')
        return successful_action(origin, json.loads(origin.read_text()), current, row,
                                 arguments=arguments, _depth=_depth+1)
    old = [item for item in previous.get('inventory', []) if item.get('identity') == row['identity']]
    if len(old) != 1: raise ValueError('analysis checkpoint has missing/duplicate TU identity')
    old = old[0]
    for key in ('file', 'directory', 'arguments', 'output', 'response_files', 'build_launcher', 'dependencies'):
        if old.get(key) != row.get(key): return None
    if old.get('analysis_arguments') != arguments: return None
    if old.get('pch_projection', []) != row.get('pch_projection', []): return None
    dependencies = [*row['dependencies'], *row['response_files'], *row['build_launcher'],
                    current['tool']['path'], *current['tool_graph']['libraries']]
    for projection in row.get('pch_projection', []):
        dependencies += [projection[key] for key in ('bootstrap_pch', 'textual_wrapper', 'matched_pch')]
    for path in dependencies:
        if previous.get('inputs', {}).get(path) != digest(Path(path)): return None
    name = row['owner']+'-'+row['identity'][:16]
    directory = root / name
    database = directory / 'compile_commands.json'
    expected_database = [{'file': row['file'], 'directory': row['directory'], 'arguments': arguments}]
    if database.is_symlink() or json.loads(database.read_text()) != expected_database:
        raise ValueError('analysis checkpoint compilation database changed')
    commands = [command for command in previous.get('commands', []) if command.get('name') == name]
    if len(commands) != 1: raise ValueError('analysis checkpoint has no unique completed native command')
    command = commands[0]
    config = root / 'checks.json'
    current_config = Path(current['configuration'])
    if (config.is_symlink() or previous['inputs'].get(str(config)) != digest(config) or
            config.read_bytes() != current_config.read_bytes()):
        raise ValueError('analysis checkpoint checker configuration changed')
    expected = analysis_argv(current['tool']['path'], directory, row['file'], config,
                             current['resource_directory'])
    log = Path(command['log'])
    if (command.get('argv') != expected or command.get('exit_code') != 0 or command.get('timed_out') or
            command.get('expected_exit_code') != 0 or command.get('status') != 'PASS' or
            log.is_symlink() or not log.resolve(strict=True).is_relative_to(root) or
            digest(log) != command.get('log_sha256')):
        raise ValueError('analysis checkpoint command/log proof changed or did not complete')
    text = log.read_text(errors='replace')
    require_clean_diagnostic(text)
    if tidy_diagnostics(text):
        raise ValueError('analysis checkpoint retained blocker diagnostics as a successful action')
    return {'identity': row['identity'], 'status': 'PASS', 'execution_reused': True,
        'checkpoint': str(checkpoint), 'checkpoint_sha256': digest(checkpoint),
        'native_command': command, 'database_sha256': digest(database), 'checker_sha256': digest(config)}
