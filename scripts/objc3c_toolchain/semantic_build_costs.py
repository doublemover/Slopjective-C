"""Summarize existing native build/install receipts without rerunning qualification."""
from __future__ import annotations

import argparse
import hashlib
import json
import math
from pathlib import Path
import statistics

from .binary_packages import digest, write_json

SCHEMA = 'objc3c.semantic-build-costs.v1'
INSTALL_PHASES = {'archive_admission', 'extraction', 'seal_verification_and_publication', 'activation', 'cleanup'}
BUILD_STATES = {'cold-build-directory', 'incremental-exact-inputs',
                'invalidation-different-inputs', 'existing-unqualified-tree'}


def _identity(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def _seconds(value):
    if type(value) not in (int, float) or not math.isfinite(value) or value < 0:
        raise ValueError('operational measurement omitted a finite nonnegative duration')
    return value


def _signature(value):
    return isinstance(value, str) and len(value) == 64 and all(char in '0123456789abcdef' for char in value)


def _read(path):
    path = Path(path).absolute()
    if path.is_symlink() or not path.is_file() or path.stat().st_size > 8*1024**2:
        raise ValueError('operational receipt is linked, missing or excessive')
    result = json.loads(path.read_text())
    if not isinstance(result, dict): raise ValueError('operational receipt must be an object')
    return path, result


def foundation_record(path):
    path, report = _read(path)
    context = report.get('cost_context', {})
    phases = report.get('phases', [])
    names = {row.get('phase') for row in phases}
    if (report.get('status') != 'passed' or not isinstance(report.get('source_identity'), str) or not report['source_identity'] or
            context.get('schema') != 'objc3c.foundation-cost-context.v1' or
            not _signature(context.get('configuration_input_identity')) or
            not isinstance(context.get('build_config_digest'), str) or not context['build_config_digest'] or
            not isinstance(context.get('host_environment'), dict) or not context['host_environment'] or
            not isinstance(context.get('host'), dict) or not context['host'].get('system') or
            names not in ({'configure', 'build', 'test'}, {'configure', 'build', 'test', 'stage'}) or
            context.get('initial_state') not in BUILD_STATES or
            type(context.get('initial_entry_count')) is not int or context['initial_entry_count'] < 0 or
            not isinstance(context.get('cache_scope'), str) or not context['cache_scope']):
        raise ValueError('foundation receipt omitted its actual phases, host or initial tree state')
    cache = context.get('initial_cache_files', {})
    if (set(cache) != {'CMakeCache.txt', 'build.ninja', '.ninja_log'} or
            any(value is not None and not _signature(value) for value in cache.values())):
        raise ValueError('foundation initial cache evidence is incomplete')
    state = context['initial_state']
    if state == 'cold-build-directory' and (any(cache.values()) or context.get('initial_entry_count') != 0):
        raise ValueError('foundation called an existing configured tree cold')
    if state == 'incremental-exact-inputs' and (
            not cache['CMakeCache.txt'] or not cache['build.ninja'] or
            not _signature(context.get('prior_report_sha256')) or context.get('prior_status') != 'passed' or
            context.get('prior_source_identity') != report['source_identity'] or
            context.get('prior_configuration_input_identity') != context['configuration_input_identity']):
        raise ValueError('foundation claimed exact-input reuse without a passing prior tree')
    if state == 'invalidation-different-inputs' and (
            context.get('prior_status') != 'passed' or not context.get('prior_source_identity') or
            context['prior_source_identity'] == report['source_identity'] and (
                context.get('prior_configuration_input_identity') is None or
                context['prior_configuration_input_identity'] == context['configuration_input_identity'])):
        raise ValueError('foundation invalidation lacks a changed prior identity')
    preparation = context.get('source_preparation', {})
    if type(preparation.get('performed')) is not bool:
        raise ValueError('foundation source preparation state is missing')
    _seconds(preparation.get('seconds'))
    phase_costs, commands = {}, []
    for index, row in enumerate(phases):
        if (type(row.get('exit_code')) is not int or row['exit_code'] != 0 or row.get('timed_out') or
                not row.get('command') or not row.get('plan_identity') or not _signature(row.get('log_sha256'))):
            raise ValueError('foundation cost requires actual passing recorded commands and log identities')
        if not isinstance(row.get('log'), str): raise ValueError('foundation omitted its command log')
        log_name = Path(row['log']).name
        if not log_name or log_name in ('.', '..'):
            raise ValueError('foundation log escaped its owned report')
        if path.name == 'foundation.json':
            log = path.parent / ('phase-'+str(index)+'.log')
        elif path.name == 'foundation-build-report.json':
            run = Path(report['run_directory']).name
            if run in ('', '.', '..'): raise ValueError('foundation run identity is missing')
            log = path.parent / 'foundation-runs' / run / log_name
        elif path.name == 'report.json':
            log = path.parent / log_name
        else:
            raise ValueError('foundation receipt is not in an owned history or native artifact layout')
        if log.is_symlink() or not log.is_file() or digest(log) != row['log_sha256']:
            raise ValueError('foundation command log changed after measurement')
        seconds = _seconds(row.get('elapsed_seconds'))
        phase_costs[row['phase']] = phase_costs.get(row['phase'], 0)+seconds
        commands.append({'phase': row['phase'], 'plan_identity': row['plan_identity'],
                         'argv': row['command'], 'seconds': seconds, 'log_sha256': row['log_sha256']})
    elapsed = _seconds(report.get('elapsed_seconds'))
    if elapsed+0.00001 < sum(phase_costs.values()):
        raise ValueError('foundation phase durations exceed the recorded owner interval')
    key = {name: context[name] for name in ('build_config_digest', 'instrumentation', 'host', 'host_environment')}
    key.update(source_identity=report['source_identity'],
               configuration_input_identity=context['configuration_input_identity'],
               phase_commands=[{'phase': row['phase'], 'argv': row['command']} for row in phases])
    return {'kind': 'foundation', 'receipt_sha256': digest(path), 'identity': _identity(report),
        'context_sha256': _identity(key), 'context': key, 'state': state,
        'source_preparation': preparation, 'phases_seconds': phase_costs, 'commands': commands,
        'elapsed_seconds': elapsed, 'cache_scope': context['cache_scope']}


def installation_record(path):
    path, report = _read(path)
    installed = report.get('installed', report)
    cost = installed.get('cost', {})
    if (report.get('status') != 'PASS' or installed.get('status') != 'PASS' or installed.get('activated') is not True or
            cost.get('schema') != 'objc3c.sdk-install-cost.v1' or not cost.get('source_build_id') or
            not _signature(cost.get('archive_sha256')) or not _signature(cost.get('seal_sha256')) or
            installed.get('seal_sha256') != cost['seal_sha256'] or
            type(cost.get('archive_bytes')) is not int or cost['archive_bytes'] <= 0 or
            type(cost.get('prefix_reused')) is not bool or not cost.get('host') or
            not _signature(cost.get('host', {}).get('decoder', {}).get('sha256')) or
            set(cost.get('phases_seconds', {})) != INSTALL_PHASES):
        raise ValueError('installation receipt omitted activated, archive-bound phase measurements')
    for key in ('archive_sha256', 'archive_bytes', 'source_build_id', 'seal_sha256'):
        if key in report and report[key] != cost[key]:
            raise ValueError('installation cost differs from the qualified delivery identity')
    for seconds in cost['phases_seconds'].values(): _seconds(seconds)
    elapsed = _seconds(cost.get('elapsed_seconds'))
    if elapsed <= 0 or elapsed+0.00001 < sum(cost['phases_seconds'].values()):
        raise ValueError('installation phases exceed their actual interval')
    context = {key: cost[key] for key in ('source_build_id', 'seal_sha256', 'archive_sha256', 'archive_bytes', 'host')}
    return {'kind': 'installation', 'receipt_sha256': digest(path), 'identity': _identity(report),
        'context_sha256': _identity(context), 'context': context,
        'state': 'existing-prefix-readmitted' if cost['prefix_reused'] else 'new-prefix-installed',
        'phases_seconds': cost['phases_seconds'], 'elapsed_seconds': elapsed,
        'cleanup_warnings': installed.get('cleanup_warnings', []),
        'cache_scope': 'Existing prefixes still decode and verify the admitted archive; no zero-work reuse claim.'}


def snapshot(*, foundations=(), installations=()):
    foundations, installations = list(foundations), list(installations)
    if not 0 < len(foundations)+len(installations) <= 64:
        raise ValueError('operational measurement receipts must be nonempty and bounded')
    rows = [*(foundation_record(path) for path in foundations),
            *(installation_record(path) for path in installations)]
    if not rows or len(rows) > 64 or len({row['identity'] for row in rows}) != len(rows):
        raise ValueError('operational measurement receipts must be nonempty, bounded and independent')
    groups = []
    for kind, context, state in sorted({(row['kind'], row['context_sha256'], row['state']) for row in rows}):
        selected = [row for row in rows if (row['kind'], row['context_sha256'], row['state']) == (kind, context, state)]
        summaries = {}
        for phase in sorted({phase for row in selected for phase in row['phases_seconds']}):
            values = [row['phases_seconds'][phase] for row in selected]
            center = statistics.median(values)
            summaries[phase] = {'samples': len(values), 'median_seconds': center,
                'minimum_seconds': min(values), 'maximum_seconds': max(values),
                'median_absolute_deviation_seconds': statistics.median(abs(value-center) for value in values)}
        groups.append({'kind': kind, 'context_sha256': context, 'state': state, 'observations': len(selected),
                       'phases': summaries, 'decision': 'observations-only-no-performance-pass'})
    report = {'schema': SCHEMA, 'status': 'PASS', 'decision': 'advisory-inconclusive-uncalibrated',
        'records': rows, 'groups': groups,
        'scope': 'Existing successful native build/install receipts. This collector runs no build, installer or archive qualification.',
        'limits': 'Build-directory state is separate from compiler/filesystem caches. Different input/host/instrumentation contexts are never pooled. No timing threshold or complexity claim.',
        'remaining': 'Collect repeated matching runner observations and review product latency/throughput/memory goals before any performance gate.'}
    return report


def collect(repo, output, *, foundations=(), installations=()):
    repo, output = Path(repo).resolve(strict=True), Path(output).resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('operational measurement needs fresh owned repository output')
    report = snapshot(foundations=foundations, installations=installations)
    output.mkdir(parents=True);write_json(output / 'qualification.json', report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--foundation', dest='foundations', type=Path, action='append', default=[])
    parser.add_argument('--installation', dest='installations', type=Path, action='append', default=[])
    result = collect(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'decision': result['decision'], 'records': len(result['records'])}))


if __name__ == '__main__':
    main()
