"""Bounded public-runtime operation histories; no production state is inspected."""
from __future__ import annotations

import hashlib
import json
import random
import time

VERSION = 1
LIMITS = {'operations': 96, 'tasks': 8, 'jobs': 8, 'groups': 1, 'actors': 1, 'worker_threads': 1}
OK, NOT_READY, CLOSED = 0, 5, 6
OPCODES = ('task', 'waiter', 'retain-task', 'release-task', 'add', 'close-group',
    'cancel-group', 'actor-job', 'close-actor', 'cancel-actor', 'drain',
    'join-group', 'join-actor', 'next', 'observe')


def protocol(value):
    validate(value)
    return json.dumps([[OPCODES.index(name), index] for name, index in value['operations']])+'\n'


def coverage_domain():
    """Reviewed 64-seed grammar domain, distinct from runtime source coverage."""
    transitions, pairs = set(), set()
    for seed in range(64):
        model = Model();model.run(generate(seed)['operations'])
        transitions.update(model.transitions);pairs.update(model.pairs)
    if len(transitions) != 20 or len(pairs) != 33:
        raise ValueError('runtime grammar domain changed; review its coverage inventory')
    return transitions, pairs


def generate(seed):
    rng = random.Random(seed)
    tasks, jobs = rng.randrange(2, 7), rng.randrange(2, 7)
    operations = []
    def add(name, index=0): operations.append([name, index])
    for index in range(tasks):
        add('task', index)
        if rng.randrange(2): add('waiter', index)
        add('retain-task', index);add('release-task', index)
        if index == tasks-1 and rng.randrange(2): add('close-group')
        add('add', index)
    if rng.randrange(2): add('cancel-group')
    for index in range(jobs):
        if index == jobs-1 and rng.randrange(2): add('close-actor')
        add('actor-job', index)
    if rng.randrange(2): add('cancel-actor')
    add('close-group');add('close-actor')
    add('drain')
    for index in range(tasks): add('waiter', index);add('observe', index)
    # The final ID is a deliberately rejected, caller-owned context.
    add('actor-job', jobs)
    add('join-group');add('join-actor')
    for _ in range(tasks+1): add('next')
    value = {'schema': 'objc3c.runtime-schedule-history.v1', 'model_version': VERSION,
        'seed': seed, 'limits': LIMITS, 'schedule': 'single gated serial epoch then quiescent observations',
        'operations': operations}
    validate(value)
    return value


def validate(value):
    if (value.get('schema') != 'objc3c.runtime-schedule-history.v1' or
            value.get('model_version') != VERSION or value.get('limits') != LIMITS or
            not isinstance(value.get('seed'), int) or isinstance(value['seed'], bool) or
            not 0 < len(value.get('operations', [])) <= LIMITS['operations']):
        raise ValueError('unreviewed or unbounded runtime schedule history')
    # Evaluate the full history before any native compile/execution. A removed
    # producer or unsafe release invalidates a shrink candidate.
    Model().run(value['operations'])


def signature(value):
    validate(value)
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


class Model:
    def __init__(self):
        self.tasks, self.jobs = {}, {}
        self.group = set();self.consumed = set()
        self.group_closed = self.actor_closed = self.group_cancelled = self.actor_cancelled = False
        self.drained = False
        self.transitions = set();self.pairs = set();self.previous = None

    def step(self, name, index, *, selected=None):
        if type(index) is not int or not 0 <= index < 8:
            raise ValueError('runtime schedule index escaped bounded logical slots')
        status, output = OK, -1
        if name == 'task':
            if index in self.tasks or self.drained: raise ValueError('task create order/lifetime is invalid')
            self.tasks[index] = {'references': 1, 'waiters': 0, 'cancel': False, 'counts': [0, 0, 0, 0, 0]}
        elif name in ('waiter', 'retain-task', 'release-task', 'add', 'observe'):
            if index not in self.tasks or not self.tasks[index]['references']:
                raise ValueError('runtime history accesses an unretained task')
            task = self.tasks[index]
            if name == 'retain-task': task['references'] += 1
            elif name == 'release-task':
                if task['references'] <= 1: raise ValueError('pilot keeps the final task handle through quiescence')
                task['references'] -= 1
            elif name == 'waiter':
                task['waiters'] += 1
                if self.drained: task['counts'][3] += 1
            elif name == 'add':
                if index in self.group: raise ValueError('duplicate registration is outside this pilot')
                if self.group_closed: status = CLOSED
                else:
                    self.group.add(index)
                    if self.group_cancelled: task['cancel'] = True
            else: status = OK if self.drained else NOT_READY
        elif name == 'actor-job':
            if index in self.jobs: raise ValueError('duplicate logical actor context')
            rejected = self.actor_closed
            self.jobs[index] = [0, int(rejected), int(rejected)]  # runs, destruction, caller cleanup
            if rejected: status = CLOSED
        elif name == 'close-group': self.group_closed = True
        elif name == 'cancel-group':
            self.group_cancelled = True
            if not self.drained:
                for task in self.group: self.tasks[task]['cancel'] = True
        elif name == 'close-actor': self.actor_closed = True
        elif name == 'cancel-actor': self.actor_closed = self.actor_cancelled = True
        elif name == 'drain':
            if self.drained or not self.group_closed or not self.actor_closed:
                raise ValueError('pilot requires one explicitly closed epoch')
            self.drained = True
            for task in self.tasks.values():
                task['counts'] = [1, 1, 1, task['waiters'], int(task['cancel'])]
            for job in self.jobs.values():
                if not job[2]: job[:2] = [int(not self.actor_cancelled), 1]
        elif name in ('join-group', 'join-actor'):
            if not self.drained: raise ValueError('worker host joins require a drained epoch')
        elif name == 'next':
            if not self.drained: raise ValueError('pilot consumes group results after drain')
            allowed = self.group-self.consumed
            if allowed:
                output = min(allowed) if selected is None else selected
                if output not in allowed: raise ValueError('completion-order result was absent or consumed twice')
                self.consumed.add(output)
            else: status = CLOSED
        else: raise ValueError('unmodeled runtime operation')
        transition = name+':'+('rejected' if status else 'accepted')+':'+('drained' if self.drained else 'queued')
        self.transitions.add(transition)
        if self.previous is not None: self.pairs.add((self.previous, transition))
        self.previous = transition
        return {'operation': name, 'id': index, 'status': status, 'output': output,
            'tasks': [self.tasks.get(i, {'counts': [0]*5})['counts'][:] for i in range(8)],
            'jobs': [self.jobs.get(i, [0]*3)[:] for i in range(8)], 'actor_overlap': 0}

    def run(self, operations):
        if not 0 < len(operations) <= LIMITS['operations']: raise ValueError('unbounded operation sequence')
        result = []
        for operation in operations:
            if not isinstance(operation, list) or len(operation) != 2: raise ValueError('malformed runtime operation')
            result.append(self.step(*operation))
        if not self.drained or any(task['references'] != 1 for task in self.tasks.values()):
            raise ValueError('history omitted quiescence or leaked caller-owned task handles')
        return result


def check(value, observed):
    validate(value)
    if len(observed) != len(value['operations']): raise ValueError('runtime trace omitted operations')
    model = Model()
    for operation, actual in zip(value['operations'], observed):
        selected = actual.get('output') if operation[0] == 'next' else None
        expected = model.step(*operation, selected=selected)
        # JSON true/false cannot stand in for an observed integer counter.
        if json.dumps(actual, sort_keys=True) != json.dumps(expected, sort_keys=True):
            raise ValueError('runtime semantic/lifetime mismatch at '+operation[0])
    return {'transitions': sorted(model.transitions), 'adjacent_pairs': sorted(map(list, model.pairs)),
            'operations': len(observed), 'status': 'PASS'}


def shrink(value, reproduces, *, trials=32, seconds=30):
    """Delete operation ranges only when the public history remains valid."""
    validate(value)
    current = dict(value, operations=[op[:] for op in value['operations']])
    attempts = native = 0
    deadline = time.monotonic()+seconds
    width = max(1, len(current['operations'])//2)
    while width:
        changed = False
        for index in range(0, len(current['operations']), width):
            if native >= trials or attempts >= 256 or time.monotonic() >= deadline:
                return current, {'native_trials': native, 'candidate_attempts': attempts, 'minimality': 'budget-bounded'}
            candidate = dict(current, operations=current['operations'][:index]+current['operations'][index+width:])
            attempts += 1
            try: validate(candidate)
            except ValueError: continue
            native += 1
            if reproduces(candidate): current, changed = candidate, True;break
        if not changed: width //= 2
    return current, {'native_trials': native, 'candidate_attempts': attempts, 'minimality': 'valid contiguous deletion fixed point within budget'}
