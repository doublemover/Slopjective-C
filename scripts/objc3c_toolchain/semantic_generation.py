"""Measure uniform versus pair-balanced generation without changing the oracle."""
from __future__ import annotations

import argparse
from collections import Counter
import json
from pathlib import Path
import random
import resource
import time

from .binary_packages import digest
from .semantic_model import VERSION, OPERATIONS, Operation, coverage, generate
from .semantic_oracles import Campaign


def program(seed, strategy):
    if strategy == 'uniform':
        return generate(seed)
    if strategy != 'pair-balanced':
        raise ValueError('unknown generation strategy')
    # Selection uses only operation-pair frequency. It never queries model
    # observations, compiler output or a target-coverage oracle for expectations.
    result, rng = generate(seed)[:15], random.Random(seed)
    pairs = Counter((left.kind, right.kind) for left, right in zip(result, result[1:]))
    while len(result) < 24:
        least = min(pairs[result[-1].kind, kind] for kind in OPERATIONS)
        kind = rng.choice([kind for kind in OPERATIONS if pairs[result[-1].kind, kind] == least])
        pairs[result[-1].kind, kind] += 1
        result.append(Operation(kind, rng.randrange(2), rng.randint(-20, 20), rng.randrange(2)))
    return result


def cpu_seconds():
    return sum(usage.ru_utime+usage.ru_stime for usage in
               (resource.getrusage(resource.RUSAGE_SELF), resource.getrusage(resource.RUSAGE_CHILDREN)))


def measure(repo, output, baseline, *, facade, services, runtime, seeds=(7, 19, 41), budget=180):
    repo, output, baseline = repo.resolve(strict=True), output.resolve(), baseline.resolve(strict=True)
    prior = json.loads(baseline.read_text())
    if (prior.get('status') != 'PASS' or prior.get('model_version') != VERSION or
            not prior.get('controls') or any(digest(Path(path)) != value for path, value in prior['inputs'].items())):
        raise ValueError('generation measurement requires a current qualified model/tool/runtime baseline')
    if (output.exists() or not output.is_relative_to(repo / 'out') or
            type(budget) is not int or not 10 <= budget <= 600 or
            len(seeds) < 3 or len(seeds) > 8 or len(set(seeds)) != len(seeds) or
            any(type(seed) is not int or not 0 <= seed < 2**32 for seed in seeds)):
        raise ValueError('generation measurement needs fresh owned output, 3..8 seeds and a bounded budget')
    output.mkdir(parents=True)
    campaign = Campaign(repo, output, facade, services, runtime)
    if any(prior['inputs'].get(path) != value for path, value in campaign.inputs.items()):
        raise ValueError('generation measurement differs from its qualified baseline')
    campaign.inputs[str(Path(__file__))] = digest(Path(__file__))
    campaign.report.update(subject='non-promoting-generation-strategy-measurement',
        baseline={'path': str(baseline), 'sha256': digest(baseline)}, seeds=list(seeds),
        strategy_measurements={}, adaptive_generation_enabled=False,
        compiler_source_coverage={'status': 'not-measured', 'owner_issue': 8290},
        comparison_limits='Single bounded paired sample; process/compiler warmth and measurement noise may affect speed. No adaptive promotion without target coverage and repeated measured benefit.')
    try:
        for strategy in ('uniform', 'pair-balanced'):
            started, cpu = time.monotonic(), cpu_seconds()
            programs = [program(seed, strategy) for seed in seeds]
            for seed, witness in zip(seeds, programs):
                remaining = min(budget-(time.monotonic()-started), budget-(cpu_seconds()-cpu))
                if remaining <= 0:
                    raise ValueError('generation strategy exceeded its fixed wall/CPU budget')
                campaign.executor.timeout = min(180, remaining)
                campaign.case(strategy+'-'+str(seed), witness)
            seconds, used_cpu = time.monotonic()-started, cpu_seconds()-cpu
            if seconds > budget or used_cpu > budget:
                raise ValueError('generation strategy exceeded its fixed wall/CPU budget')
            campaign.report['strategy_measurements'][strategy] = {
                'programs': len(programs), 'operations_per_program': 24,
                'wall_budget_seconds': budget, 'cpu_budget_seconds': budget,
                'wall_seconds': round(seconds, 6), 'cpu_seconds': round(used_cpu, 6),
                'eligible_programs_per_second': len(programs)/seconds,
                'semantic_coverage': coverage(programs), 'reproducing_mismatch_classes': []}
            campaign.executor.save()
        if any(digest(Path(path)) != value for path, value in campaign.inputs.items()):
            raise ValueError('generation measurement inputs changed')
        campaign.report['status'] = 'PASS'
    except (OSError, ValueError) as error:
        campaign.report.update(status='FAIL', error=str(error))
        raise
    finally:
        campaign.executor.save()
    return campaign.report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    for name in ('output', 'baseline', 'facade', 'services', 'runtime'):
        parser.add_argument('--'+name, type=Path, required=True)
    parser.add_argument('--budget', type=int, default=180)
    args = parser.parse_args()
    result = measure(**vars(args))
    print(json.dumps({'status': result['status'], 'adaptive_generation_enabled': False}))


if __name__ == '__main__':
    main()
