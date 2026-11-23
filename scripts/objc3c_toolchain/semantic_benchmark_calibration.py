"""Pool independent semantic-workload receipts and apply explicit reviewed limits."""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import math
from pathlib import Path
import re
import statistics

from .binary_packages import write_json
from .collection_scaling import (COLLECTION_EVIDENCE_SCOPE, COLLECTION_METRICS, collection_cases,
    collection_phase_timing, collection_sample_cases, collection_scaling_inputs,
    collection_scaling_result)
from .semantic_benchmarks import SCALES, semantic_result, summarize, workload_inventory, workload_names
from .semantic_runtime_oracles import (CURRENT_WORKLOAD_VERSION, completed_task_count,
    validate_delay_controls, validate_task_frame_controls)

SCHEMA = 'objc3c.semantic-benchmark-calibration.v1'
POLICY_SCHEMA = 'objc3c.semantic-benchmark-review.v1'
HOST_FIELDS = ('system', 'kernel', 'cpu_model', 'cpu_quota', 'cpuset', 'memory_limit',
               'affinity', 'container', 'runtime_threads', 'noise_limits')


def _unambiguous_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError('duplicate field in semantic benchmark input: '+key)
        result[key] = value
    return result


def _read(path):
    candidate = Path(path)
    if candidate.is_symlink():
        raise ValueError('semantic benchmark receipt is linked or excessive')
    path = candidate.resolve(strict=True)
    limit = 16 * 1024**2
    if not path.is_file():
        raise ValueError('semantic benchmark input must be a regular file')
    with path.open('rb') as stream:
        data = stream.read(limit + 1)
    if len(data) > limit:
        raise ValueError('semantic benchmark input is excessive')
    return (json.loads(data, object_pairs_hook=_unambiguous_object),
            hashlib.sha256(data).hexdigest(), len(data))


def _host_identity(report):
    host = report.get('host', {})
    if set(HOST_FIELDS)-set(host):
        raise ValueError('semantic benchmark receipt omitted its hosted runner context')
    return {field: host[field] for field in HOST_FIELDS}


def _workload_inputs(report):
    selected = {}
    required = {'scripts/objc3c_toolchain/semantic_benchmarks.py', 'tests/benchmarks/collections.cpp'}
    if report.get('workload_version') == CURRENT_WORKLOAD_VERSION:
        required |= {'tests/benchmarks/runtime_workloads.cpp',
                     'tests/benchmarks/ownership_workloads.mm',
                     'tests/benchmarks/task_frame_workloads.cpp',
                     'scripts/objc3c_toolchain/semantic_runtime_oracles.py'}
    if 'collection_scaling' in report:
        required |= {path.as_posix() for path in collection_scaling_inputs(Path('.'))}
    for path, value in report.get('inputs', {}).items():
        normalized = str(path).replace('\\', '/')
        for suffix in required:
            if normalized.endswith(suffix):
                if 'collection_scaling' in report and not _sha256(value):
                    raise ValueError('semantic benchmark workload input has an invalid digest')
                if suffix in selected and selected[suffix] != value:
                    raise ValueError('semantic benchmark receipt has conflicting workload inputs')
                selected[suffix] = value
    if set(selected) != required:
        raise ValueError('semantic benchmark receipt omitted its workload implementation identity')
    return selected


def _sha256(value):
    return isinstance(value, str) and re.fullmatch(r'[a-f0-9]{64}', value) is not None


def _collection_loaders(records):
    binaries, context = {}, {}
    if not isinstance(records, list) or len(records) != len(COLLECTION_METRICS):
        raise ValueError('collection calibration omitted the measurement loaders')
    for record in records:
        if not isinstance(record, dict) or not isinstance(record.get('loader'), dict):
            raise ValueError('collection calibration has an invalid measurement loader')
        metric, loader = record.get('metric'), record.get('loader', {})
        binary, libraries = loader.get('binary'), loader.get('libraries')
        build_ids = loader.get('build_ids')
        if (metric not in COLLECTION_METRICS or metric in binaries or record.get('sdk_index') != 0 or
                not isinstance(binary, str) or not Path(binary).is_absolute() or
                Path(binary).name != 'collection-scaling-'+metric or not _sha256(loader.get('sha256')) or
                not isinstance(libraries, dict) or not libraries or
                not all(Path(name).is_absolute() and _sha256(digest) for name, digest in libraries.items()) or
                not isinstance(build_ids, dict) or set(build_ids) != set(libraries)):
            raise ValueError('collection calibration has an invalid measurement loader')
        binaries[metric] = binary
        context[metric] = {field: loader[field] for field in ('sha256', 'libraries', 'build_ids')}
    return binaries, context


def _admit_collection(path, report, repetitions):
    extension = report['collection_scaling']
    if (not isinstance(extension, dict) or extension.get('status') != 'PASS' or
            any(extension.get(key) != scope for key, scope in COLLECTION_EVIDENCE_SCOPE.items())):
        raise ValueError('collection calibration has a different measurement scope')
    binaries, context = _collection_loaders(extension.get('loaders'))
    expected = [(f'{repetition}-sdk-0-collection-scaling-{metric}-{family}-{scale}',
                 repetition, metric, family, scale, rounds)
                for repetition in range(repetitions + 1) for metric in COLLECTION_METRICS
                for family, scale, rounds in collection_sample_cases(metric)]
    samples = extension.get('samples')
    commands = [row for row in report['samples']
                if re.match(r'^[0-9]+-sdk-[0-9]+-collection-scaling-', row.get('name', ''))]
    names = [case[0] for case in expected]
    if (not isinstance(samples, list) or not all(isinstance(sample, dict) for sample in samples) or
            [sample.get('sample') for sample in samples] != names or
            [row['name'] for row in commands] != names):
        raise ValueError('collection calibration omitted or reordered raw samples')
    timeline_end = 0
    timed, diagnostics, retained = {}, {}, []
    for sample, command, (name, repetition, metric, family, scale, rounds) in zip(samples, commands, expected):
        start, end = sample.get('start_ns'), sample.get('end_ns')
        wall, cpu, rss = (command.get(key) for key in ('wall_seconds', 'cpu_seconds', 'peak_process_rss_kib'))
        argv = [binaries[metric], family, str(scale), str(rounds)]
        log_name = name+'.log'
        if (sample.get('sdk_index') != 0 or sample.get('metric') != metric or
                sample.get('family') != family or sample.get('scale') != scale or
                sample.get('rounds') != rounds or sample.get('warmup') is not (repetition == 0) or
                type(start) is not int or type(end) is not int or start < timeline_end or end <= start or
                command.get('sdk_index') != 0 or command.get('argv') != argv or
                command.get('exit_code') != 0 or command.get('timed_out') is not False or
                not isinstance(command.get('log'), str) or Path(command['log']).name != log_name or
                type(wall) not in (int, float) or not math.isfinite(wall) or not 0 < wall <= (end - start)/1e9 or
                type(cpu) not in (int, float) or not math.isfinite(cpu) or cpu < 0 or
                type(rss) is not int or rss <= 0):
            raise ValueError('collection calibration has invalid command or timeline evidence: '+name)
        timeline_end = end
        try:
            log, digest, _ = _read(Path(path).parent / log_name)
        except (OSError, ValueError) as error:
            raise ValueError('collection calibration could not read its raw log: '+name) from error
        if digest != command.get('log_sha256') or log != sample.get('phases'):
            raise ValueError('collection calibration log differs from its receipt: '+name)
        phases = collection_scaling_result(json.dumps(log), family, scale, metric, rounds)
        retained.append({**sample, 'log': log_name, 'log_sha256': digest, 'wall_seconds': wall,
                         'cpu_seconds': cpu, 'peak_process_rss_kib': rss})
        if repetition == 0:
            continue
        if metric == 'time':
            timed.setdefault((family, scale), []).append(collection_phase_timing(family, scale, phases))
        else:
            diagnostics.setdefault((family, scale, metric), []).append(phases)
    summaries = [{'sdk_index': 0, 'family': family, 'scale': scale, 'rounds': rounds,
                  'phase_seconds': {phase: summarize(timed[family, scale], phase)
                                    for phase in timed[family, scale][0]}}
                 for family, scale, rounds in collection_cases()]
    if extension.get('summaries') != summaries:
        raise ValueError('collection calibration summary differs from its raw timings')
    signature = hashlib.sha256(json.dumps([
        sample['log_sha256'] for sample in retained if sample['metric'] == 'time'
    ]).encode()).hexdigest()
    return {'context': context, 'identity': signature, 'timed': timed, 'diagnostics': diagnostics,
            'samples': retained}


def _collection_calibration(admitted, labels):
    observations = [row['collection_scaling'] for row in admitted]
    if all(row is None for row in observations):
        return None
    if (any(row is None for row in observations) or
            any(row['context'] != observations[0]['context'] for row in observations[1:])):
        raise ValueError('semantic calibration receipts have different collection contexts')
    if len({row['identity'] for row in observations}) != len(observations):
        raise ValueError('semantic calibration cannot count duplicate collection timings twice')
    pooled = []
    for family, scale, rounds in collection_cases():
        per_receipt = [row['timed'][family, scale] for row in observations]
        timings = {}
        for phase in per_receipt[0][0]:
            medians = [statistics.median(sample[phase] for sample in rows) for rows in per_receipt]
            timings[phase] = {'pooled': summarize([sample for rows in per_receipt for sample in rows], phase),
                'receipt_medians': medians, 'between_receipt_median_range_seconds': max(medians)-min(medians)}
        diagnostics = {}
        for metric in ('allocations', 'service-diagnostics'):
            key = family, scale, metric
            records = [record for row in observations for record in row['diagnostics'].get(key, ())]
            if records:
                diagnostics[metric] = {
                    phase: {field: {'median': statistics.median(record[phase][field] for record in records),
                                    'minimum': min(record[phase][field] for record in records),
                                    'maximum': max(record[phase][field] for record in records)}
                            for field in values if field not in ('completed_operations', 'checksum')}
                    for phase, values in records[0].items()}
        pooled.append({'family': family, 'scale': scale, 'rounds': rounds, 'phase_seconds': timings,
                       'diagnostics': diagnostics})
    return {**COLLECTION_EVIDENCE_SCOPE, 'decision': 'observed-runner-variance-only',
            'loaders': observations[0]['context'], 'observations': pooled,
            'source_samples': [{'label': label, 'samples': row['samples']}
                               for label, row in zip(labels, observations)]}


def _admit(path):
    report, sha256, byte_count = _read(path)
    summaries, samples, sdks = (report.get(key, []) for key in ('summaries', 'samples', 'sdks'))
    repetitions = report.get('repetitions')
    expected = workload_names(report.get('workload_version'))
    if (report.get('schema') != 'objc3c.semantic-benchmarks.v1' or report.get('status') != 'PASS' or
            report.get('correctness') != 'PASS' or
            report.get('timing_decision') != 'advisory-inconclusive-uncalibrated' or
            type(repetitions) is not int or not 3 <= repetitions <= 15 or len(sdks) != 1 or
            len(summaries) != len(expected) or {row.get('workload') for row in summaries} != expected or
            report.get('delay_control', {}).get('status') != 'PASS' or
            report['delay_control'].get('semantic_output_unchanged') is not True):
        raise ValueError('semantic benchmark receipt is incomplete or already makes a timing decision')
    if report['workload_version'] == CURRENT_WORKLOAD_VERSION:
        if report.get('workloads') != workload_inventory(CURRENT_WORKLOAD_VERSION):
            raise ValueError('semantic benchmark receipt changed its workload contract')
        validate_delay_controls(report)
        validate_task_frame_controls(report)
    observations = {}
    for workload in expected:
        workload_rows = [row for row in samples if row.get('workload') == workload and
                         row.get('sdk_index') == 0]
        rows = [row for row in workload_rows if row.get('warmup') is False]
        if (len(workload_rows) != repetitions+1 or
                len([row for row in workload_rows if row.get('warmup') is True]) != 1 or
                len(rows) != repetitions or any(row.get('exit_code') != 0 or row.get('timed_out') or
                type(row.get('wall_seconds')) not in (int, float) or row['wall_seconds'] <= 0 or
                type(row.get('peak_process_rss_kib')) is not int or row['peak_process_rss_kib'] <= 0
                for row in workload_rows)):
            raise ValueError('semantic benchmark receipt omitted measured successful samples')
        field = 'work_seconds' if workload.startswith('runtime-') else 'wall_seconds'
        if workload.startswith('runtime-'):
            for row in workload_rows:
                semantic = semantic_result(json.dumps(row.get('semantic_result')), workload)
                if row.get('work_seconds') != semantic['nanoseconds']/1e9:
                    raise ValueError('semantic benchmark runtime timer differs from its retained sample')
                if completed_task_count(semantic) and row.get('task_completions_per_second') != (
                        completed_task_count(semantic)/row['work_seconds']):
                    raise ValueError('semantic benchmark throughput differs from completed callbacks')
        measured = summarize(rows, field)
        retained = next(row for row in summaries if row['workload'] == workload)
        if (retained.get('sdk_index') != 0 or retained.get('wall') != summarize(rows) or
                (workload.startswith('runtime-') and retained.get('work') != measured) or
                (not workload.startswith('runtime-') and retained.get('wall') != measured)):
            raise ValueError('semantic benchmark retained summary differs from its raw samples')
        if workload.startswith(('runtime-task-', 'runtime-worker-', 'runtime-shared-')) and retained.get(
                'task_completions_per_second') != summarize(rows, 'task_completions_per_second'):
            raise ValueError('semantic benchmark throughput summary differs from completed work')
        observations[workload] = {'field': field, 'rows': rows, 'summary': measured}
    sdk = sdks[0]
    if (not all(isinstance(sdk.get(key), str) and sdk[key] for key in ('seal_sha256', 'source_build_id')) or
            not isinstance(sdk.get('instrumentation'), dict)):
        raise ValueError('semantic benchmark receipt omitted its SDK identity')
    receipt_identity = hashlib.sha256(json.dumps(report, sort_keys=True, separators=(',', ':')).encode()).hexdigest()
    collections = _admit_collection(path, report, repetitions) if 'collection_scaling' in report else None
    return {'sha256': sha256, 'identity': receipt_identity, 'bytes': byte_count,
            'report': report, 'host': _host_identity(report), 'inputs': _workload_inputs(report),
            'sdk': {key: sdk.get(key) for key in ('seal_sha256', 'source_build_id', 'instrumentation')},
            'observations': observations, 'collection_scaling': collections}


def _apply_review(report, policy_path, admitted):
    policy, policy_sha256, _ = _read(policy_path)
    fields = {'schema', 'owner', 'reason', 'expires_on', 'context', 'workloads'}
    if (not isinstance(policy, dict) or set(policy) != fields or
            policy['schema'] != POLICY_SCHEMA or any(
                not isinstance(policy[field], str) or not policy[field].strip()
                for field in ('owner', 'reason', 'expires_on'))):
        raise ValueError('semantic benchmark review requires an owner, reason and expiry')
    today = datetime.now(timezone.utc).date()
    expiry = datetime.strptime(policy['expires_on'], '%Y-%m-%d').date()
    if policy['expires_on'] != expiry.isoformat() or expiry < today:
        raise ValueError('semantic benchmark review has expired or has a noncanonical date')
    context = {key: report[key] for key in ('host', 'inputs', 'workload_version')}
    context['instrumentation'] = report['sdk']['instrumentation']
    if policy['context'] != context:
        raise ValueError('semantic benchmark review has a different measurement context')
    workloads = policy['workloads']
    available = {row['workload']: row for row in report['observations']}
    if not isinstance(workloads, dict) or not workloads or set(workloads) - set(available):
        raise ValueError('semantic benchmark review selects an unknown or empty workload set')
    limits = {'max_median_seconds', 'max_median_range_seconds', 'max_peak_process_rss_kib'}
    rate_key = 'min_median_task_completions_per_second'
    results = []
    for workload, budget in sorted(workloads.items()):
        if (not isinstance(budget, dict) or not limits <= set(budget) or
                set(budget) - limits - {rate_key} or any(
                    type(value) not in (int, float) or value <= 0 or
                    (type(value) is float and not math.isfinite(value))
                    for value in budget.values()) or
                type(budget['max_peak_process_rss_kib']) is not int):
            raise ValueError('invalid reviewed limits for '+workload)
        row = available[workload]
        if rate_key in budget and 'task_completions_per_second' not in row:
            raise ValueError('reviewed throughput requires a task completion workload')

        medians = row['receipt_medians']
        slow = [value > budget['max_median_seconds'] for value in medians]
        noisy = row['between_receipt_median_range_seconds'] > budget['max_median_range_seconds']
        over_memory = row['peak_process_rss_kib']['maximum'] > budget['max_peak_process_rss_kib']
        rates = []
        if rate_key in budget:
            rates = [statistics.median(sample['task_completions_per_second'] for sample in
                        receipt['observations'][workload]['rows']) for receipt in admitted]
        low_rate = [rate < budget[rate_key] for rate in rates]
        rate_regression = bool(low_rate) and all(low_rate)
        if over_memory or (not noisy and (all(slow) or rate_regression)):
            decision = 'regression'
        elif noisy or any(slow) or any(low_rate):
            decision = 'inconclusive'
        else:
            decision = 'pass'
        row['decision'] = decision
        results.append({'workload': workload, 'limits': budget, 'decision': decision,
                        'noisy_elapsed_time': noisy, 'exceeded_process_rss_limit': over_memory,
                        'receipt_median_task_completions_per_second': rates})
    decisions = {row['decision'] for row in results}
    report['status'] = ('FAIL' if 'regression' in decisions else
                        'INCONCLUSIVE' if 'inconclusive' in decisions else 'PASS')
    report['decision'] = 'reviewed-workload-limits'
    report['gate_policy'] = 'Only the workloads selected by the reviewed policy contribute to this decision.'
    report['review'] = {'owner': policy['owner'], 'reason': policy['reason'],
                        'expires_on': policy['expires_on'], 'evaluated_on': today.isoformat(),
                        'policy_sha256': policy_sha256, 'workloads': results}


def calibrate(repo, receipts, output, *, labels=(), policy=None):
    """Describe observed runner variance and optionally check reviewed limits."""
    repo, output = Path(repo).resolve(strict=True), Path(output).resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('semantic calibration requires fresh owned repository output')
    if len(receipts) < 2:
        raise ValueError('semantic calibration requires at least two independent hosted receipts')
    admitted = [_admit(path) for path in receipts]
    if len({row['identity'] for row in admitted}) != len(admitted):
        raise ValueError('semantic calibration cannot count a duplicate receipt twice')
    labels = list(labels) if labels else ['receipt-'+str(index) for index in range(len(admitted))]
    if (len(labels) != len(admitted) or len(set(labels)) != len(labels) or
            any(not isinstance(label, str) or not label or len(label) > 512 for label in labels)):
        raise ValueError('semantic calibration receipt labels are incomplete or ambiguous')
    for key in ('host', 'inputs', 'sdk'):
        if any(row[key] != admitted[0][key] for row in admitted[1:]):
            raise ValueError('semantic calibration receipts have different '+key+' contexts')
    versions = {row['report']['workload_version'] for row in admitted}
    if len(versions) != 1:
        raise ValueError('semantic calibration workload versions differ')
    version = versions.pop()
    expected = workload_names(version)
    repetitions = {row['report']['repetitions'] for row in admitted}
    if len(repetitions) != 1:
        raise ValueError('semantic calibration receipt repetition policies differ')
    observations = []
    for workload in sorted(expected):
        field = admitted[0]['observations'][workload]['field']
        rows = [sample for receipt in admitted for sample in receipt['observations'][workload]['rows']]
        pooled = summarize(rows, field)
        receipt_medians = [receipt['observations'][workload]['summary']['median'] for receipt in admitted]
        rss = [sample['peak_process_rss_kib'] for sample in rows]
        observations.append({'workload': workload, 'measurement': field,
            'pooled': pooled, 'receipt_medians': receipt_medians,
            'between_receipt_median_range_seconds': max(receipt_medians)-min(receipt_medians),
            'largest_observed_deviation_seconds': max(abs(sample[field]-pooled['median']) for sample in rows),
            'peak_process_rss_kib': {'median': statistics.median(rss), 'maximum': max(rss)},
            **({'task_completions_per_second': summarize(rows, 'task_completions_per_second')}
               if workload.startswith(('runtime-task-', 'runtime-worker-', 'runtime-shared-')) else {}),
            'decision': 'observed-runner-variance-only'})
    scaling = []
    for phase in sorted({name.rsplit('-', 1)[0] for name in expected}):
        rows = [next(row for row in observations if row['workload'] == phase+'-'+str(scale))
                for scale in SCALES]
        scaling.append({'phase': phase, 'points': [{'scale': scale,
            'median_seconds': row['pooled']['median'],
            'median_seconds_per_item': row['pooled']['median']/scale}
            for scale, row in zip(SCALES, rows)],
            'adjacent_growth': [rows[index+1]['pooled']['median']/rows[index]['pooled']['median']
                                for index in range(len(rows)-1)]})
    all_samples = [sample for row in admitted for sample in row['report']['samples']]
    report = {'schema': SCHEMA, 'status': 'PASS', 'workload_version': version,
        'decision': 'calibrated-observation-advisory',
        'gate_policy': ('Observed spread and cost do not define a product regression budget. '
                        'Promotion requires reviewed per-workload absolute effects and memory goals.'),
        'independent_receipts': len(admitted), 'samples_per_workload': len(admitted)*repetitions.pop(),
        'host': admitted[0]['host'], 'sdk': admitted[0]['sdk'], 'inputs': admitted[0]['inputs'],
        'source_receipts': [{'label': label, 'sha256': row['sha256'],
                            'receipt_identity': row['identity'], 'bytes': row['bytes']}
                            for label, row in zip(labels, admitted)],
        'observations': observations, 'scaling': scaling,
        'measured_cost': {'commands': len(all_samples),
            'wall_seconds': sum(row.get('wall_seconds', 0) for row in all_samples),
            'cpu_seconds': sum(row.get('cpu_seconds', 0) for row in all_samples),
            'receipt_bytes': sum(row['bytes'] for row in admitted)}}
    collections = _collection_calibration(admitted, labels)
    if collections is not None:
        report['collection_scaling'] = collections
    if policy is not None:
        _apply_review(report, policy, admitted)
    output.mkdir(parents=True)
    write_json(output / 'calibration.json', report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--receipt', type=Path, action='append', required=True)
    parser.add_argument('--label', action='append', default=[])
    parser.add_argument('--policy', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = calibrate(args.repo, args.receipt, args.output, labels=args.label, policy=args.policy)
    print(json.dumps({'status': result['status'], 'decision': result['decision'],
                      'independent_receipts': result['independent_receipts']}))
    return 0 if result['status'] == 'PASS' else 1


if __name__ == '__main__':
    raise SystemExit(main())
