"""Independent completed-work and lifetime oracles for sustained runtime workloads."""
from __future__ import annotations

import json
import math

CURRENT_WORKLOAD_VERSION = 6
OWNERSHIP_FAMILIES = ('arc-ownership', 'mrc-ownership')
GENERATED_TASK_FAMILIES = ('task-coroutine', 'task-coroutine-cancel')
TASK_FRAME_MUTATIONS = {
    'checksum': ('ledger.checksum += value;', 'ledger.checksum += value + 1;'),
    'frame-destruction': ('++ledger.destroys;++ledger.operations;', '++ledger.operations;'),
    'completion': ('++ledger.completed;++ledger.operations;', '++ledger.operations;'),
}
FAMILIES = ('text', 'generic-array', 'task-group-actor', 'task-group-cancel', 'worker-tasks',
            'shared-pool-tasks', *OWNERSHIP_FAMILIES, *GENERATED_TASK_FAMILIES)


def completed_task_count(result):
    return result.get('completed_tasks', result.get('callbacks', 0))


def ownership_flags(family):
    if family not in OWNERSHIP_FAMILIES:
        raise ValueError('unreviewed ownership benchmark family')
    return ['-x', 'objective-c++', '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2',
            '-fblocks', '-fobjc-weak', '-fobjc-arc' if family == 'arc-ownership' else '-fno-objc-arc']


def expected_work(family, scale, *, rounds=16, seed=17):
    if family not in FAMILIES or scale not in (32, 256, 2048):
        raise ValueError('unreviewed semantic runtime workload')
    common = {'live_values': 0, 'resumes': 0, 'callbacks': 0}
    if family == 'text':
        return {**common, 'checksum': rounds*461*scale, 'completed_operations': rounds*(4*scale+12),
                'payload_copies': 0, 'destructions': 0}
    if family == 'generic-array':
        return {**common, 'checksum': rounds*(scale*seed+scale*(scale-1)),
                'completed_operations': rounds*(2*scale+5)}
    if family in OWNERSHIP_FAMILIES:
        return {**common, 'checksum': rounds*(scale*seed+scale*(scale-1)//2),
            'completed_operations': rounds*8*scale, 'payload_copies': 0,
            'destructions': rounds*scale, 'allocations': rounds*scale,
            'retained_aliases': rounds*scale, 'released_aliases': rounds*scale,
            'block_copies': rounds*scale, 'block_invocations': rounds*scale, 'weak_zeroed': rounds*scale}
    if family in GENERATED_TASK_FAMILIES:
        cancel = family == 'task-coroutine-cancel'
        return {**common, 'checksum': rounds*(scale*seed+scale*(scale-1)//2)*(1 if cancel else 2),
            'completed_operations': rounds*6*scale, 'payload_copies': 0,
            'destructions': rounds*scale, 'resumes': rounds*2*scale,
            'frame_leases_created': rounds*scale, 'completed_tasks': rounds*scale,
            'cancelled': rounds*scale if cancel else 0}
    tasks = {**common, 'checksum': rounds*(scale*seed+scale*(scale-1)//2),
            'completed_operations': rounds*4*scale, 'payload_copies': 0,
            'destructions': rounds*scale, 'resumes': rounds*2*scale, 'callbacks': rounds*scale}
    if family == 'task-group-cancel': tasks['cancelled'] = rounds*scale
    elif family == 'worker-tasks': tasks['worker_mask'] = 15
    elif family == 'shared-pool-tasks': tasks['pool_workers'] = 4
    return tasks


def runtime_workload_result(text, family, scale):
    try:
        result = json.loads(text)
    except ValueError as error:
        raise ValueError('runtime workload omitted its semantic/lifetime oracle') from error
    expected = expected_work(family, scale)
    keys = {*expected, 'nanoseconds', 'payload_copies', 'destructions'}
    if family == 'shared-pool-tasks': keys.add('worker_mask')
    if (not isinstance(result, dict) or set(result) != keys or
            any(type(value) is not int for value in result.values()) or
            any(result[key] != value for key, value in expected.items()) or
            result['nanoseconds'] <= 0):
        raise ValueError('runtime workload semantic work, lifetime or timer is invalid')
    if family == 'shared-pool-tasks' and not 1 <= result['worker_mask'] <= 15:
        raise ValueError('shared pool did not execute on its admitted workers')
    if family == 'generic-array' and (
            result['payload_copies'] != result['destructions'] or
            not 2*16*scale <= result['payload_copies'] <= 34*16*scale):
        raise ValueError('generic runtime workload leaked values or exceeded its bounded chunk copy contract')
    return result


def task_frame_control_result(text, family, fault):
    if family not in GENERATED_TASK_FAMILIES or fault not in TASK_FRAME_MUTATIONS:
        raise ValueError('unreviewed generated-frame semantic control')
    result = json.loads(text)
    expected = expected_work(family, 32)
    field = {'checksum': 'checksum', 'frame-destruction': 'destructions', 'completion': 'completed_tasks'}[fault]
    expected[field] = expected[field]+16*32 if fault == 'checksum' else 0
    if (not isinstance(result, dict) or set(result) != {*expected, 'nanoseconds'} or
            any(type(value) is not int for value in result.values()) or result['nanoseconds'] <= 0 or
            any(result[key] != value for key, value in expected.items())):
        raise ValueError('generated-frame control failed for an unrelated semantic reason')
    try:
        runtime_workload_result(text, family, 32)
    except ValueError:
        return result
    raise ValueError('generated-frame oracle accepted its wrong-code control')


def validate_task_frame_controls(report):
    controls = report.get('task_frame_controls', [])
    expected = {(family, fault) for family in GENERATED_TASK_FAMILIES for fault in TASK_FRAME_MUTATIONS}
    if (len(controls) != len(expected) or
            {(row.get('family'), row.get('fault')) for row in controls} != expected):
        raise ValueError('generated-frame paired semantic controls are incomplete')
    for control in controls:
        rows = [row for row in report.get('samples', []) if row.get('name') == control.get('sample')]
        if (control.get('status') != 'PASS' or control.get('classification') != 'semantic-mismatch' or
                len(rows) != 1 or type(rows[0].get('exit_code')) is not int or rows[0]['exit_code'] != 0 or
                rows[0].get('timed_out') is not False or type(rows[0].get('sdk_index')) is not int or
                rows[0]['sdk_index'] != len(report['sdks'])-1):
            raise ValueError('generated-frame control omitted its actual clean native command')
        task_frame_control_result(json.dumps(control.get('semantic_result')), control['family'], control['fault'])


def validate_delay_controls(report):
    controls = report.get('runtime_delay_controls', [])
    if (len(controls) != len(FAMILIES) or {row.get('family') for row in controls} != set(FAMILIES) or
            any(row.get('status') != 'PASS' or row.get('semantic_output_unchanged') is not True or
                row.get('minimum_control_effect_seconds') != 16*0.001/2 or
                type(row.get('median_effect_seconds')) not in (int, float) or
                not math.isfinite(row['median_effect_seconds']) or
                row['median_effect_seconds'] < row['minimum_control_effect_seconds'] or
                len(row.get('samples_seconds', [])) != report['repetitions'] or
                any(type(value) not in (int, float) or not math.isfinite(value) or value <= 0
                    for value in row['samples_seconds']) for row in controls)):
        raise ValueError('extended runtime paired delay controls are incomplete')
