"""One disjoint product-owner catalogue for local and distributed qualification."""
from __future__ import annotations

import json
from pathlib import Path
import re

OWNERS = ('structured', 'native-async', 'coroutines', 'c-frame-bridge',
    *(kind+'-'+sanitizer+'-'+optimization for sanitizer in ('address-undefined', 'thread')
      for optimization in ('O0', 'O2') for kind in ('features', 'factories')),
    'installed-ownership')
TRANSPORTS = ('source', 'pch')
SPLIT_FEATURES = ('features-address-undefined-O0', 'features-address-undefined-O2', 'features-thread-O2')


def owner_tasks(owner: str) -> tuple[str, ...]:
    return tuple(f'{owner}-{transport}' for transport in TRANSPORTS) if owner in SPLIT_FEATURES else (owner,)


PARTITION_TASKS = tuple(task for owner in OWNERS for task in owner_tasks(owner))
TASK_OWNERS = {task: owner for owner in OWNERS for task in (owner, *owner_tasks(owner))}
PARTITIONS = (
    ('structured', 'native-async', 'coroutines'),
    ('features-address-undefined-O0-source',),
    ('features-address-undefined-O2-source',),
    ('c-frame-bridge', 'installed-ownership'),
    ('features-address-undefined-O0-pch',),
    ('features-address-undefined-O2-pch',),
    ('factories-address-undefined-O0', 'factories-address-undefined-O2', 'factories-thread-O0', 'factories-thread-O2'),
    ('features-thread-O2-source',),
    ('features-thread-O0',),
    ('features-thread-O2-pch',),
)
GROUPS = tuple('asan-products-partition-'+str(index) for index in range(len(PARTITIONS)))


def select(index: int | None = None) -> tuple[str, ...]:
    assigned = [name for part in PARTITIONS for name in part]
    if len(assigned) != len(set(assigned)) or set(assigned) != set(PARTITION_TASKS):
        raise ValueError('sanitizer product partitions overlap or omit catalogue tasks')
    if index is None:
        return OWNERS
    if type(index) is not int or not 0 <= index < len(PARTITIONS):
        raise ValueError(f'sanitizer product partition must be an index in 0..{len(PARTITIONS) - 1}')
    return PARTITIONS[index]


def profiles(owners) -> tuple[tuple[str, str], ...]:
    required = set()
    for task in owners:
        owner = TASK_OWNERS.get(task)
        if owner is None:
            raise ValueError('unknown sanitizer product owner: '+task)
        if owner in ('structured', 'native-async', 'coroutines'):
            required.update((mode, opt) for mode in ('address,undefined', 'thread') for opt in ('O0', 'O2'))
        elif owner == 'c-frame-bridge':
            required.update((mode, 'O1') for mode in ('address,undefined', 'thread'))
        elif owner.startswith(('features-', 'factories-')):
            mode, opt = owner.split('-', 1)[1].rsplit('-', 1)
            required.add((mode.replace('address-undefined', 'address,undefined'), opt))
    return tuple(sorted(required))


def evidence_owners(owners) -> int:
    return sum(2 if name == 'c-frame-bridge' else 1 for name in owners)


CASE_SCHEMA = 'objc3c.sanitizer-feature-transport.v1'
CASE_RECEIPT = 'case-inventory.json'
SHA256 = re.compile('[0-9a-f]{64}')


def feature_scope(task: str) -> dict:
    owner = TASK_OWNERS[task]
    mode, optimization = owner.removeprefix('features-').rsplit('-', 1)
    return {'owner': owner, 'sanitizer': mode.replace('address-undefined', 'address,undefined'),
            'optimization': optimization,
            'transports': TRANSPORTS if task == owner else (task.removeprefix(owner + '-'),)}


def feature_cases(scope: dict) -> tuple[str, ...]:
    from .run_linux_feature_batch import configurations

    return tuple(case.name for case in configurations(transports=scope['transports'])
                 if case.optimization == scope['optimization'])


def feature_receipt(task: str, output: Path, report: dict) -> dict:
    from .sdk_example import digest

    scope = feature_scope(task)
    expected = feature_cases(scope)
    if (report.get('status') != 'pass' or report.get('sanitizer') != scope['sanitizer'] or
            report['cases'] != [{'name': name, 'status': 'pass'} for name in expected]):
        raise ValueError('sanitizer feature batch differs from its assigned cases: '+task)
    rows = {row['name']: row for row in report['commands']}
    if len(rows) != len(report['commands']):
        raise ValueError('sanitizer feature batch contains duplicate commands: '+task)
    executions = []
    for name in expected:
        command = rows.get(name+'-execute', {})
        if (command.get('phase') != 'execute' or command.get('status') != 'PASS' or
                command.get('exit_code') != 0 or command.get('timed_out') is not False):
            raise ValueError('sanitizer feature batch omitted a passing case execution: '+name)
        executions.append({'name': name, 'status': 'PASS', 'log_sha256': command['log_sha256']})
    return {'schema': CASE_SCHEMA, 'task': task, **scope, 'status': 'PASS',
            'batch_sha256': digest(output / 'batch.json'), 'cases': executions}


def verify_feature_receipt(campaign: Path, task: str) -> None:
    from .sdk_example import digest

    output = campaign / task
    try:
        report = json.loads((output / CASE_RECEIPT).read_bytes())
        batch_sha256 = digest(output / 'batch.json')
    except (OSError, ValueError) as error:
        raise ValueError('sanitizer feature transport receipt is missing or invalid: '+task) from error
    scope = feature_scope(task)
    expected = feature_cases(scope)
    if (report.get('schema') != CASE_SCHEMA or report.get('status') != 'PASS' or report.get('task') != task or
            any(report.get(key) != value for key, value in {**scope, 'transports': list(scope['transports'])}.items()) or
            report.get('batch_sha256') != batch_sha256 or
            tuple(row.get('name') for row in report.get('cases', ())) != expected):
        raise ValueError('sanitizer feature transport or case identity changed: '+task)
    for row in report['cases']:
        signature = row.get('log_sha256', '')
        log = output / (row['name']+'-execute.log')
        if (row.get('status') != 'PASS' or not isinstance(signature, str) or not SHA256.fullmatch(signature) or
                not log.is_file() or log.is_symlink() or signature != digest(log)):
            raise ValueError('sanitizer feature case outcome or log identity changed: '+row['name'])


SOURCE_FILES = tuple('scripts/objc3c_toolchain/'+name+'.py' for name in (
    'sanitizer_products', 'run_linux_feature_batch', 'run_linux_factory_batch',
    'qualify_native_async_sanitizers', 'qualify_task_coroutine_sanitizers',
    'qualify_structured_sanitizers', 'qualify_async_c_abi_sanitizers',
    'product_evidence', 'sanitizer_policy', 'sanitizer_partitions',
    'runtime_instrumentation', 'qualify_async_c_abi', 'sdk_example')) + (
    'scripts/qualify_linux_ownership_sanitizers.py',)
