"""One disjoint product-owner catalogue for local and distributed qualification."""
from __future__ import annotations

OWNERS = ('structured', 'native-async', 'coroutines', 'c-frame-bridge',
    *(kind+'-'+sanitizer+'-'+optimization for sanitizer in ('address-undefined', 'thread')
      for optimization in ('O0', 'O2') for kind in ('features', 'factories')),
    'installed-ownership')
PARTITIONS = (
    ('structured',), ('native-async',), ('coroutines',),
    ('c-frame-bridge', 'installed-ownership'),
    ('features-address-undefined-O0',),
    ('features-address-undefined-O2',),
    ('factories-address-undefined-O0', 'factories-thread-O0'),
    ('factories-address-undefined-O2', 'factories-thread-O2'),
    ('features-thread-O0',),
    ('features-thread-O2',),
)
GROUPS = tuple('asan-products-partition-'+str(index) for index in range(len(PARTITIONS)))


def select(index: int | None = None) -> tuple[str, ...]:
    assigned = [name for part in PARTITIONS for name in part]
    if len(assigned) != len(set(assigned)) or set(assigned) != set(OWNERS):
        raise ValueError('sanitizer product partitions overlap or omit catalogue owners')
    if index is None:
        return OWNERS
    if type(index) is not int or not 0 <= index < len(PARTITIONS):
        raise ValueError(f'sanitizer product partition must be an index in 0..{len(PARTITIONS) - 1}')
    return PARTITIONS[index]


def profiles(owners) -> tuple[tuple[str, str], ...]:
    required = set()
    for owner in owners:
        if owner not in OWNERS:
            raise ValueError('unknown sanitizer product owner: '+owner)
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

SOURCE_FILES = tuple('scripts/objc3c_toolchain/'+name+'.py' for name in (
    'sanitizer_products', 'run_linux_feature_batch', 'run_linux_factory_batch',
    'qualify_native_async_sanitizers', 'qualify_task_coroutine_sanitizers',
    'qualify_structured_sanitizers', 'qualify_async_c_abi_sanitizers',
    'product_evidence', 'sanitizer_policy', 'sanitizer_partitions',
    'runtime_instrumentation', 'qualify_async_c_abi', 'sdk_example')) + (
    'scripts/qualify_linux_ownership_sanitizers.py',)
