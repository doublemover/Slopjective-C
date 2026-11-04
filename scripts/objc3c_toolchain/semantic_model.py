"""Independent, bounded value/lifetime model for defined ObjC3 programs."""
from __future__ import annotations

from dataclasses import asdict, dataclass
import hashlib
import itertools
import json
import random

VERSION = 2
RELATIONS = {'rename': 'local-alpha-rename-reserved-prefix.v1',
             'comments': 'comment-insertion-token-boundaries.v1',
             'pure-grouping': 'bounded-integer-add-zero-grouping.v1'}
OPERATIONS = ('some', 'none', 'copy_optional', 'alias_optional', 'self_optional',
              'ok', 'err', 'step', 'copy_result', 'new', 'copy_object', 'clear',
              'scope', 'partial_throw')


@dataclass(frozen=True)
class Operation:
    kind: str
    slot: int = 0
    value: int = 0
    other: int = 1


class EligibilityFailure(ValueError):
    def __init__(self, message, classification='generator-invalid'):
        super().__init__(message)
        self.classification = classification


def validate(program: list[Operation]):
    if not program or len(program) > 64:
        raise EligibilityFailure('semantic program must contain 1..64 operations')
    allocated = 0
    for op in program:
        if not isinstance(op, Operation) or op.kind not in OPERATIONS:
            raise EligibilityFailure('unsupported typed operation', 'unsupported')
        if (
                type(op.slot) is not int or type(op.other) is not int or type(op.value) is not int or
                op.slot not in (0, 1) or op.other not in (0, 1) or not -20 <= op.value <= 20):
            raise EligibilityFailure('unsupported or out-of-range typed operation')
        allocated += op.kind == 'new'
    if allocated > 32:
        raise EligibilityFailure('semantic program exceeds its logical allocation bound')


def eligibility(program, *, transport='source', optimization='O2', standard='c++20',
                relation='', mutant='', bias=0, sanitized=False, expected_mismatch=None):
    """Inspectable eligibility claims for this closed, bounded witness only."""
    validate(program)
    if (transport not in ('source', 'pch', 'interface') or optimization not in ('O0', 'O2', 'thinlto') or
            standard not in ('c++20', 'c++23') or relation not in ('', *RELATIONS) or
            mutant not in ('', 'tag', 'defer', 'destructor', 'provider', 'error') or
            type(bias) is not int or bias not in (0, 1) or type(sanitized) is not bool or
            expected_mismatch not in (None, 'value-or-lifetime', 'cleanup-order')):
        raise EligibilityFailure('unsupported semantic program configuration', 'unsupported')
    operations = [asdict(op) for op in program]
    return {'schema': 'objc3c.semantic-eligibility.v2', 'model_version': VERSION, 'generator_version': VERSION,
        'program_sha256': hashlib.sha256(json.dumps(operations, sort_keys=True).encode()).hexdigest(),
        'profile': {'target': 'x86_64-unknown-linux-gnu', 'int_bits': 32, 'pointer_bits': 64,
            'runtime': 'gnustep-2.2', 'language_revision': 3, 'language': 'objective-c++',
            'standard': standard, 'ownership': 'arc', 'transport': transport,
            'optimization': optimization, 'generated_sanitizers': 'address,undefined' if sanitized else None},
        'features': sorted({op.kind for op in program}),
        'bounds': {'operations': 64, 'allocations': 32, 'slots': 2, 'operand_min': -20, 'operand_max': 20,
            'signed_intermediate_abs_max': 64*22, 'event_max': 5063},
        'assumptions': ['initialized-active-arm-only.v1', 'explicit-full-expression-sequencing.v1',
            'bounded-signed-arithmetic.v1', 'logical-last-owner-destruction.v1',
            'weak-zeroing-before-snapshot.v1', 'no-observable-retain-count.v1',
            'no-moves-floats-shifts-or-concurrent-effects.v1', 'bounded-unrolled-termination.v1'],
        'relation': {'name': relation, 'preconditions': [RELATIONS[relation]] if relation else [],
            'scope': 'This exact generated witness; no arbitrary-input equivalence claim.'},
        'witness': {'argv': [], 'stdin': 'closed', 'external_inputs': [], 'operations': operations,
            'provider_bias': bias, 'fault_injection': mutant, 'expected_mismatch': expected_mismatch}}


def verify_eligibility(record, program, **configuration):
    if record.get('eligibility') != eligibility(program, **configuration):
        raise EligibilityFailure('semantic replay eligibility manifest is missing or incompatible')
    if 'expected' in record and record['expected'] != interpret(program, bias=configuration.get('bias', 0)):
        raise EligibilityFailure('semantic replay model observations are incompatible')


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
    optional, result, pairs, covered = set(), set(), set(), set()
    for program in programs:
        states = interpret(program)['states']
        previous = [0] * 8
        for state in states:
            optional.update((previous[index], state[index]) for index in (0, 2))
            result.update((previous[index], state[index]) for index in (4, 6))
            previous = state
        before = [0] * len(states[0])
        for op, state in zip(program, states):
            covered.add('operation:'+op.kind)
            if op.kind in ('some', 'none', 'copy_optional', 'alias_optional', 'self_optional'):
                offset = 2*op.slot
                covered.add(f'optional:{op.kind}:{before[offset]}>{state[offset]}')
            if op.kind in ('ok', 'err', 'step', 'copy_result'):
                offset = 4+2*op.slot
                covered.add(f'result:{op.kind}:{before[offset]}>{state[offset]}')
            if op.kind in ('new', 'copy_object', 'clear'):
                dead = sum(state[11::2])-sum(before[11::2])
                covered.add(owner_transition(op.kind, before[8:10], state[8:10], op.slot, dead))
            if op.kind in ('scope', 'partial_throw'):
                covered.add('cleanup:'+op.kind)
            before = state
        pairs.update((left.kind, right.kind) for left, right in zip(program, program[1:]))
    required = {(0, 0), (0, 1), (1, 0), (1, 1)}
    if optional != required or result != required:
        raise ValueError('semantic seed bank lost required active-arm transitions')
    domain = transition_domain()
    if not covered <= domain:
        raise ValueError('semantic transition escaped its declared supported domain')
    return {'scope': 'Bounded model transitions; compiler source coverage is measured separately.',
            'operations': sorted({op.kind for program in programs for op in program}),
            'optional_tag_transitions': sorted(optional), 'result_tag_transitions': sorted(result),
            'operation_pairs': sorted(pairs),
            'transition_ids': sorted(covered), 'transition_domain': sorted(domain),
            'transitions_covered': len(covered), 'transitions_supported': len(domain),
            'pair_ids': sorted(left+'>'+right for left, right in pairs),
            'pairs_covered': len(pairs), 'pairs_supported': len(OPERATIONS)**2}


def owner_transition(kind, before, after, slot, destroyed):
    def state(values):
        key = values[slot]
        return 'empty' if not key else 'shared' if values[1-slot] == key else 'sole'
    return f'ownership:{kind}:{state(before)}>{state(after)}:destroyed={destroyed}'


def transition_domain():
    """Enumerate the feasible finite state categories, independently of seeds."""
    domain = {'operation:'+name for name in OPERATIONS} | {'cleanup:scope', 'cleanup:partial_throw'}
    for family, operations in (('optional', {'some': (1,), 'none': (0,), 'copy_optional': (0, 1),
            'alias_optional': (1,), 'self_optional': ()}),
            ('result', {'ok': (1,), 'err': (0,), 'copy_result': (0, 1), 'step': ()})):
        for kind, outcomes in operations.items():
            domain.update(f'{family}:{kind}:{before}>{after}' for before in (0, 1)
                          for after in outcomes or (before,))
    for kind in ('new', 'copy_object', 'clear'):
        for values in itertools.product(range(3), repeat=2):
            for slot, other in itertools.product(range(2), repeat=2):
                after = list(values)
                after[slot] = 3 if kind == 'new' else values[other] if kind == 'copy_object' else 0
                destroyed = int(bool(values[slot]) and values[slot] not in after)
                domain.add(owner_transition(kind, values, after, slot, destroyed))
    return domain
