"""Expected versus executed native IR stages, independent of command counts."""
from __future__ import annotations

from pathlib import Path
import re


def stages(case: dict, optimization: str) -> list[str]:
    name = case['name']+'-'+optimization
    result = []
    if case['transport'] in ('pch', 'interface'):
        result.append(name+'-'+case['transport'])
    for index, source in enumerate(case['sources']):
        unit = name+'-'+str(index)
        imported = case['transport'] == 'interface' and source.endswith('import.m')
        if not imported:
            result.append(unit+'-cc1')
        result += [unit+'-'+stage for stage in ('frontend-ir', 'input-verifier', 'baseline-dis',
            'optimizer', 'optimized-dis', 'backend', 'production-ir', 'production-verifier',
            'production-object', 'production-dwarf', 'split-dwarf')]
        if case.get('thinlto') and optimization == 'O2':
            result += [unit+'-thinlto', unit+'-thin-prelink-verify']
    routes = ['production', 'split']+(['thinlto'] if case.get('thinlto') and optimization == 'O2' else [])
    for route in routes:
        result += [name+'-'+route+'-'+stage for stage in ('link', 'loader', 'oracle', 'dwarf', 'line-tables')]
    return result


def verify(case: dict, optimization: str, row: dict, commands: list[dict]) -> dict:
    expected = stages(case, optimization)
    by_name = {command['name']: command for command in commands}
    if len(by_name) != len(commands):
        raise ValueError('duplicate native IR execution stage')
    missing = [name for name in expected if by_name.get(name, {}).get('status') != 'PASS']
    routes = ['production', 'split']+(['thinlto'] if case.get('thinlto') and optimization == 'O2' else [])
    if (missing or [unit['source'] for unit in row['units']] != case['sources'] or
            any(unit['status'] != 'PASS' for unit in row['units']) or
            [execution['route'] for execution in row['executions']] != routes or
            any(execution['status'] != 'PASS' for execution in row['executions'])):
        raise ValueError('incomplete native IR stage/unit/oracle inventory: '+', '.join(missing))
    return {'status': 'PASS', 'expected_stages': expected,
            'executed_stages': [name for name in expected if by_name[name]['status'] == 'PASS'],
            'expected_routes': routes, 'executed_routes': [execution['route'] for execution in row['executions']]}


def thinlto_modules(work: Path, inputs: list[Path]) -> list[Path]:
    """Retain actual per-module ThinLTO backend IR, not only the link index.

    LLD writes combined/index outputs beside the executable and each ThinLTO
    backend's six stages beside its own input. A nonrecursive glob misses them.
    'preopt.bc' is not evidence that an 'opt.bc' stage executed.
    """
    modules = sorted(path for path in work.rglob('*.bc')
                     if re.search(r'\.\d+\.(?:preopt|promote|internalize|import|opt|precodegen)\.bc$', path.name))
    for source in inputs:
        for stage in ('preopt', 'import', 'opt', 'precodegen'):
            expected = list(source.parent.glob(source.name+'.*.'+stage+'.bc'))
            if len(expected) != 1 or expected[0] not in modules or expected[0].stat().st_size == 0:
                raise ValueError('ThinLTO missing actual per-module '+stage+' IR: '+str(source))
    return modules
