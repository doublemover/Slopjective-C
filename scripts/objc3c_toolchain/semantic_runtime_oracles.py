"""Independent completed-work and lifetime oracles for sustained runtime workloads."""
from __future__ import annotations

import json
import math

CURRENT_WORKLOAD_VERSION = 3
FAMILIES = ('text', 'generic-array', 'task-group-actor', 'task-group-cancel', 'worker-tasks')


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
    tasks = {**common, 'checksum': rounds*(scale*seed+scale*(scale-1)//2),
            'completed_operations': rounds*4*scale, 'payload_copies': 0,
            'destructions': rounds*scale, 'resumes': rounds*2*scale, 'callbacks': rounds*scale}
    if family == 'task-group-cancel': tasks['cancelled'] = rounds*scale
    elif family == 'worker-tasks': tasks['worker_mask'] = 15
    return tasks


def runtime_workload_result(text, family, scale):
    try:
        result = json.loads(text)
    except ValueError as error:
        raise ValueError('runtime workload omitted its semantic/lifetime oracle') from error
    expected = expected_work(family, scale)
    keys = {*expected, 'nanoseconds', 'payload_copies', 'destructions'}
    if (not isinstance(result, dict) or set(result) != keys or
            any(type(value) is not int for value in result.values()) or
            any(result[key] != value for key, value in expected.items()) or
            result['nanoseconds'] <= 0):
        raise ValueError('runtime workload semantic work, lifetime or timer is invalid')
    if family == 'generic-array' and (
            result['payload_copies'] != result['destructions'] or
            not 2*16*scale <= result['payload_copies'] <= 34*16*scale):
        raise ValueError('generic runtime workload leaked values or exceeded its bounded chunk copy contract')
    return result


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
