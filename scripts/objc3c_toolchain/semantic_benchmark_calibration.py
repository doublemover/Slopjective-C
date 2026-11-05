"""Pool independent hosted semantic-workload receipts without inventing limits."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import statistics

from .binary_packages import digest, write_json
from .semantic_benchmarks import SCALES, runtime_result, summarize

SCHEMA = 'objc3c.semantic-benchmark-calibration.v1'
HOST_FIELDS = ('system', 'kernel', 'cpu_model', 'cpu_quota', 'cpuset', 'memory_limit',
               'affinity', 'container', 'runtime_threads', 'noise_limits')
EXPECTED = {phase+'-'+str(scale) for phase in ('syntax', 'object', 'runtime') for scale in SCALES}


def _read(path):
    candidate = Path(path)
    if candidate.is_symlink():
        raise ValueError('semantic benchmark receipt is linked or excessive')
    path = candidate.resolve(strict=True)
    if path.stat().st_size > 16*1024**2:
        raise ValueError('semantic benchmark receipt is linked or excessive')
    return path, json.loads(path.read_text())


def _host_identity(report):
    host = report.get('host', {})
    if set(HOST_FIELDS)-set(host):
        raise ValueError('semantic benchmark receipt omitted its hosted runner context')
    return {field: host[field] for field in HOST_FIELDS}


def _workload_inputs(report):
    selected = {}
    for path, value in report.get('inputs', {}).items():
        normalized = str(path).replace('\\', '/')
        for suffix in ('scripts/objc3c_toolchain/semantic_benchmarks.py',
                       'tests/benchmarks/collections.cpp'):
            if normalized.endswith(suffix):
                if suffix in selected and selected[suffix] != value:
                    raise ValueError('semantic benchmark receipt has conflicting workload inputs')
                selected[suffix] = value
    if set(selected) != {'scripts/objc3c_toolchain/semantic_benchmarks.py',
                         'tests/benchmarks/collections.cpp'}:
        raise ValueError('semantic benchmark receipt omitted its workload implementation identity')
    return selected


def _admit(path):
    path, report = _read(path)
    summaries, samples, sdks = (report.get(key, []) for key in ('summaries', 'samples', 'sdks'))
    repetitions = report.get('repetitions')
    if (report.get('schema') != 'objc3c.semantic-benchmarks.v1' or report.get('status') != 'PASS' or
            report.get('correctness') != 'PASS' or report.get('workload_version') != 1 or
            report.get('timing_decision') != 'advisory-inconclusive-uncalibrated' or
            type(repetitions) is not int or not 3 <= repetitions <= 15 or len(sdks) != 1 or
            len(summaries) != len(EXPECTED) or {row.get('workload') for row in summaries} != EXPECTED or
            report.get('delay_control', {}).get('status') != 'PASS' or
            report['delay_control'].get('semantic_output_unchanged') is not True):
        raise ValueError('semantic benchmark receipt is incomplete or already makes a timing decision')
    observations = {}
    for workload in EXPECTED:
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
            scale = int(workload.split('-')[1])
            for row in workload_rows:
                semantic = runtime_result(json.dumps(row.get('semantic_result')), scale)
                if row.get('work_seconds') != semantic['nanoseconds']/1e9:
                    raise ValueError('semantic benchmark runtime timer differs from its retained sample')
        measured = summarize(rows, field)
        retained = next(row for row in summaries if row['workload'] == workload)
        if (retained.get('sdk_index') != 0 or retained.get('wall') != summarize(rows) or
                (workload.startswith('runtime-') and retained.get('work') != measured) or
                (not workload.startswith('runtime-') and retained.get('wall') != measured)):
            raise ValueError('semantic benchmark retained summary differs from its raw samples')
        observations[workload] = {'field': field, 'rows': rows, 'summary': measured}
    sdk = sdks[0]
    if (not all(isinstance(sdk.get(key), str) and sdk[key] for key in ('seal_sha256', 'source_build_id')) or
            not isinstance(sdk.get('instrumentation'), dict)):
        raise ValueError('semantic benchmark receipt omitted its SDK identity')
    receipt_identity = hashlib.sha256(json.dumps(report, sort_keys=True, separators=(',', ':')).encode()).hexdigest()
    return {'path': path, 'sha256': digest(path), 'identity': receipt_identity, 'bytes': path.stat().st_size,
            'report': report, 'host': _host_identity(report), 'inputs': _workload_inputs(report),
            'sdk': {key: sdk.get(key) for key in ('seal_sha256', 'source_build_id', 'instrumentation')},
            'observations': observations}


def calibrate(repo, receipts, output, *, labels=()):
    """Describe observed runner variance; product budgets remain an explicit review."""
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
    repetitions = {row['report']['repetitions'] for row in admitted}
    if len(repetitions) != 1:
        raise ValueError('semantic calibration receipt repetition policies differ')
    observations = []
    for workload in sorted(EXPECTED):
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
            'decision': 'observed-runner-variance-only'})
    scaling = []
    for phase in ('syntax', 'object', 'runtime'):
        rows = [next(row for row in observations if row['workload'] == phase+'-'+str(scale))
                for scale in SCALES]
        scaling.append({'phase': phase, 'points': [{'scale': scale,
            'median_seconds': row['pooled']['median'],
            'median_seconds_per_item': row['pooled']['median']/scale}
            for scale, row in zip(SCALES, rows)],
            'adjacent_growth': [rows[index+1]['pooled']['median']/rows[index]['pooled']['median']
                                for index in range(len(rows)-1)]})
    all_samples = [sample for row in admitted for sample in row['report']['samples']]
    report = {'schema': SCHEMA, 'status': 'PASS', 'workload_version': 1,
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
    output.mkdir(parents=True)
    write_json(output / 'calibration.json', report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--receipt', type=Path, action='append', required=True)
    parser.add_argument('--label', action='append', default=[])
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = calibrate(args.repo, args.receipt, args.output, labels=args.label)
    print(json.dumps({'status': result['status'], 'decision': result['decision'],
                      'independent_receipts': result['independent_receipts']}))


if __name__ == '__main__':
    main()
