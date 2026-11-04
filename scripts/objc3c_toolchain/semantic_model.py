"""Independent, bounded value/lifetime model for defined ObjC3 programs."""
from __future__ import annotations

from dataclasses import dataclass
import random

VERSION = 1
OPERATIONS = ('some', 'none', 'copy_optional', 'alias_optional', 'self_optional',
              'ok', 'err', 'step', 'copy_result', 'new', 'copy_object', 'clear',
              'scope', 'partial_throw')


@dataclass(frozen=True)
class Operation:
    kind: str
    slot: int = 0
    value: int = 0
    other: int = 1


def validate(program: list[Operation]):
    if not program or len(program) > 64:
        raise ValueError('semantic program must contain 1..64 operations')
    allocated = 0
    for op in program:
        if (not isinstance(op, Operation) or op.kind not in OPERATIONS or
                type(op.slot) is not int or type(op.other) is not int or type(op.value) is not int or
                op.slot not in (0, 1) or op.other not in (0, 1) or not -20 <= op.value <= 20):
            raise ValueError('unsupported or out-of-range typed operation')
        allocated += op.kind == 'new'
    if allocated > 32:
        raise ValueError('semantic program exceeds its logical allocation bound')


def generate(seed: int, length=24) -> list[Operation]:
    """Curated transitions precede random combinations; seeds never imply coverage."""
    if not 15 <= length <= 64:
        raise ValueError('generated length must be 15..64')
    rng = random.Random(seed)
    program = [Operation('some', value=7), Operation('copy_optional', slot=1, other=0),
        Operation('none'), Operation('alias_optional', value=3), Operation('self_optional'),
        Operation('ok', value=4), Operation('step', value=2), Operation('err', slot=1, value=9),
        Operation('copy_result', other=1), Operation('step', value=3), Operation('new'), Operation('copy_object', slot=1, other=0),
        Operation('clear'), Operation('scope'), Operation('partial_throw')]
    while len(program) < length:
        program.append(Operation(rng.choice(OPERATIONS), rng.randrange(2), rng.randint(-20, 20), rng.randrange(2)))
    validate(program)
    return program


def interpret(program: list[Operation], *, bias=0) -> dict:
    """Track logical owners and visible cleanup, never compiler retain counts."""
    validate(program)
    if bias not in (0, 1):
        raise ValueError('unmodeled provider behavior')
    count = sum(op.kind == 'new' for op in program)
    optional = [(0, 0), (0, 0)]
    result = [(0, 0), (0, 0)]
    objects = [0, 0]
    destroyed = [0] * (count + 1)
    states, events = [], []
    next_id = 0

    def assign(slot, value):
        previous = objects[slot]
        objects[slot] = value
        if previous and previous not in objects:
            destroyed[previous] += 1

    def snapshot():
        row = [value for arm in optional for value in arm]
        row += [value for arm in result for value in arm]
        row += objects
        for key in range(1, count + 1):
            row += [int(key in objects), destroyed[key]]
        states.append(row)

    for index, op in enumerate(program):
        slot, value, other = op.slot, op.value, op.other
        if op.kind in ('some', 'alias_optional'):
            optional[slot] = (1, value)
        elif op.kind == 'none':
            optional[slot] = (0, 0)
        elif op.kind == 'copy_optional':
            optional[slot] = optional[other]
        elif op.kind == 'self_optional':
            pass
        elif op.kind in ('ok', 'err'):
            result[slot] = (int(op.kind == 'ok'), value)
        elif op.kind == 'copy_result':
            result[slot] = result[other]
        elif op.kind == 'step':
            arm, old = result[slot]
            result[slot] = (arm, old + value + bias if arm else old)
        elif op.kind == 'new':
            next_id += 1
            assign(slot, next_id)
        elif op.kind == 'copy_object':
            assign(slot, objects[other])
        elif op.kind == 'clear':
            assign(slot, 0)
        elif op.kind == 'scope':
            events += [1000+index, 2000+index, 3000+index, 4000+index]
        elif op.kind == 'partial_throw':
            events += [1000+index, 2000+index, 3000+index, 4000+index, 5000+index]
        snapshot()
    assign(1, 0)
    assign(0, 0)
    snapshot()
    return {'states': states, 'events': events}


def transform(program: list[Operation], relation: str) -> list[Operation]:
    validate(program)
    if relation not in ('rename', 'comments', 'pure-grouping'):
        raise ValueError('metamorphic relation lacks reviewed preconditions')
    # These source transformations preserve this typed program. No reordering,
    # floating-point equivalence or ownership-transfer rewrite is admitted.
    return list(program)


def candidates(program: list[Operation]):
    """Typed deletion shrinker: remaining allocations are renumbered on render."""
    validate(program)
    for size in (max(1, len(program)//2), 1):
        for start in range(0, len(program), size):
            candidate = program[:start] + program[start+size:]
            if candidate:
                validate(candidate)
                yield candidate


def mismatch(expected: dict, actual: dict) -> str | None:
    if not isinstance(actual, dict) or set(actual) != {'states', 'events'}:
        return 'output-schema'
    if (not isinstance(actual['states'], list) or not isinstance(actual['events'], list) or
            any(type(event) is not int for event in actual['events']) or
            any(not isinstance(row, list) or any(type(value) is not int for value in row)
                for row in actual['states'])):
        return 'output-schema'
    if actual['states'] != expected['states']:
        return 'value-or-lifetime'
    if actual['events'] != expected['events']:
        return 'cleanup-order'
    return None


def coverage(programs: list[list[Operation]]) -> dict:
    optional, result, pairs = set(), set(), set()
    for program in programs:
        states = interpret(program)['states']
        previous = [0] * 8
        for state in states:
            optional.update((previous[index], state[index]) for index in (0, 2))
            result.update((previous[index], state[index]) for index in (4, 6))
            previous = state
        pairs.update((left.kind, right.kind) for left, right in zip(program, program[1:]))
    required = {(0, 0), (0, 1), (1, 0), (1, 1)}
    if optional != required or result != required:
        raise ValueError('semantic seed bank lost required active-arm transitions')
    return {'operations': sorted({op.kind for program in programs for op in program}),
            'optional_tag_transitions': sorted(optional), 'result_tag_transitions': sorted(result),
            'operation_pairs': sorted(pairs)}
