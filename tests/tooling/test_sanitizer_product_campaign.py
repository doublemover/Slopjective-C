"""Retain all product-owner outcomes with bounded concurrency and exact profiles."""
import threading
import time

import pytest

from scripts.objc3c_toolchain.sanitizer_products import execute_owners
from scripts.objc3c_toolchain.runtime_instrumentation import used_receipts


def test_parallel_campaign_keeps_failed_owners_and_completes_independent_work(tmp_path):
    active = peak = 0
    completed = []
    lock = threading.Lock()
    def callback(output):
        nonlocal active, peak
        with lock:
            active += 1
            peak = max(peak, active)
        time.sleep(0.01)
        with lock:
            active -= 1
            completed.append(output.name)
        if output.name == 'broken':
            raise ValueError('retained actual product failure')
        return {'status': 'PASS'}
    tasks = [(name, callback) for name in ('first', 'broken', 'third', 'fourth')]
    report = {'owners': []}
    with pytest.raises(ValueError, match='failed owners'):
        execute_owners(tasks, tmp_path, report, 2)
    assert set(completed) == {name for name, _ in tasks}
    assert 1 < peak <= 2
    assert {row['name'] for row in report['owners']} == set(completed)
    assert next(row for row in report['owners'] if row['name'] == 'broken')['error'] == 'retained actual product failure'


def test_campaign_cannot_succeed_with_empty_or_duplicate_owners(tmp_path):
    for tasks in ([], [('same', None), ('same', None)]):
        with pytest.raises(ValueError, match='unique nonempty'):
            execute_owners(tasks, tmp_path, {'owners': []}, 1)


def test_shared_profile_receipts_describe_only_the_owners_actual_optimizations():
    class Profiles:
        def receipts(self):
            return [{'sanitizer': sanitizer, 'optimization': optimization}
                    for sanitizer in ('address,undefined', 'thread') for optimization in ('O0', 'O1', 'O2')]
    assert len(used_receipts(Profiles(), ('O0', 'O2'))) == 4
    assert len(used_receipts(Profiles(), ('O1',))) == 2
