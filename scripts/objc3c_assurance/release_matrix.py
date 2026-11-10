"""Run and merge exact-SDK native release partitions."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil

from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.semantic_build_costs import snapshot
from .evidence import (MAX_BYTES, SCHEMA, aggregate, artifact, native_files,
                       suite_results)
from .plan import foundation_receipts, from_checkout, selected_suites, validate
from .runner import run


# Packaging owns the exact archive and keeps the matching upstream build tree.
# Every remaining group consumes an independently installed copy of that archive.
GROUPS = {
    'product-producer': ('package', 'upstream'),
    'foundation': ('foundation',),
    'runtime': ('runtime',),
    'frontend': ('frontend',),
    'installed-sanitizers': ('installed', 'sanitizers'),
    'swift': ('swift-consumer',),
    'semantic': ('semantic-workloads',),
    'asan-products': ('sanitizer-products', 'runtime-schedules'),
}
CONSUMER_GROUPS = tuple(name for name in GROUPS if name != 'product-producer')


def _require_checkout(repo: Path, plan: dict) -> None:
    if from_checkout(repo, **plan['revision'], periodic=plan['periodic']) != plan:
        raise ValueError('release matrix plan differs from the exact checked-out revision')


def run_group(repo: Path, plan: dict, output: Path, group: str, *,
              product_sdk: Path | None = None,
              package_release: Path | None = None) -> dict:
    validate(plan)
    _require_checkout(repo, plan)
    suites = GROUPS.get(group)
    if suites is None:
        raise ValueError('unknown release matrix group')
    if group == 'product-producer':
        if product_sdk is not None or package_release is not None:
            raise ValueError('the product producer cannot consume a prior product')
    elif product_sdk is None or package_release is None:
        raise ValueError('release consumers require the exact installed product and receipt')
    result = run(repo, plan, output, suite_ids=suites, product_sdk=product_sdk,
                 package_release=package_release)
    result['release_matrix_group'] = group
    write_json(output / 'result.json', result)
    return result


def _partial(root: Path, group: str) -> Path:
    direct = root / group
    if (direct / 'result.json').is_file():
        return direct
    matches = [path.parent for path in root.rglob('result.json')
               if path.parent.name == group]
    if len(matches) != 1:
        raise ValueError('release matrix group evidence is missing or duplicated: '+group)
    return matches[0]


def aggregate_core(repo: Path, plan: dict, partials: Path, output: Path) -> dict:
    validate(plan)
    _require_checkout(repo, plan)
    partials = partials.resolve(strict=True)
    output = output.resolve()
    if output.exists() or not output.is_relative_to(repo.resolve() / 'out'):
        raise ValueError('release matrix aggregate requires a fresh owned output')
    expected_suites = selected_suites(plan, 'core')
    expected = [row['id'] for row in expected_suites]
    assigned = [suite for group in GROUPS.values() for suite in group]
    if len(assigned) != len(set(assigned)) or set(assigned) != set(expected):
        raise ValueError('release matrix groups overlap or differ from the canonical core shard')

    output.mkdir(parents=True)
    records = {}
    commands = []
    products = []
    elapsed = 0.0
    peak = 0
    for group, suites in GROUPS.items():
        root = _partial(partials, group)
        receipt = json.loads((root / 'result.json').read_text())
        if (receipt.get('schema') != SCHEMA or receipt.get('status') != 'PASS' or
                receipt.get('revision') != plan['revision'] or
                receipt.get('plan_sha256') != plan['sha256'] or
                receipt.get('suite_partition') != list(suites) or
                receipt.get('release_matrix_group') != group):
            raise ValueError('release matrix group is failed, stale or substituted: '+group)
        rows = receipt.get('suites', [])
        if len(rows) != len(suites) or [row.get('id') for row in rows] != list(suites):
            raise ValueError('release matrix group suite inventory changed: '+group)
        products.append(receipt.get('product'))
        commands.extend(receipt.get('commands', []))
        elapsed += float(receipt.get('elapsed_seconds', 0))
        peak = max(peak, int(receipt.get('peak_child_rss_kib', 0)))
        for row in rows:
            name = row['id']
            if row.get('status') != 'PASS' or suite_results(root, name) != row.get('executions'):
                raise ValueError('release matrix suite did not retain canonical passing evidence: '+name)
            expected_files = {path.relative_to(root).as_posix() for path in native_files(root / name)}
            if {saved['path'] for saved in row.get('artifacts', [])} != expected_files:
                raise ValueError('release matrix suite artifact inventory is incomplete: '+name)
            for saved in row['artifacts']:
                source = root / saved['path']
                if artifact(root, source) != saved:
                    raise ValueError('release matrix suite artifact changed: '+name)
                target = output / saved['path']
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(source, target)
                if artifact(output, target) != saved:
                    raise ValueError('release matrix suite artifact changed while merging: '+name)
            records[name] = {**row, 'artifacts': [artifact(output, output / saved['path'])
                                                   for saved in row['artifacts']]}
    if not products or any(product != products[0] for product in products):
        raise ValueError('release matrix consumers used different compiler product identities')

    ordered = [records[name] for name in expected]
    costs = snapshot(foundations=foundation_receipts(output, expected_suites),
                     installations=[output / 'package/installation-cost.json'])
    write_json(output / 'operational-costs.json', costs)
    report = {'schema': SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'shard': 'core', 'suites': ordered,
              'commands': commands, 'product': products[0],
              'cache': {'restored_build_products': True,
                        'reason': 'exact same-run SDK archive producer; source-build ID, seal and archive hash required'},
              'release_matrix_groups': {name: list(suites) for name, suites in GROUPS.items()},
              'elapsed_seconds_sum': round(elapsed, 6),
              'peak_child_rss_kib_max': peak}
    report['operational_costs'] = artifact(output, output / 'operational-costs.json')
    total = sum(saved['bytes'] for row in ordered for saved in row['artifacts'])
    total += report['operational_costs']['bytes']
    if total > MAX_BYTES:
        raise ValueError('merged core evidence exceeds the native artifact budget')
    write_json(output / 'result.json', report)
    aggregate(plan, output, outcomes={'plan': 'success', 'native': 'success'},
              tested=plan['revision']['tested'], shard='core')
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    run_parser = commands.add_parser('run')
    run_parser.add_argument('--repo', type=Path, default=Path.cwd())
    run_parser.add_argument('--plan', type=Path, required=True)
    run_parser.add_argument('--output', type=Path, required=True)
    run_parser.add_argument('--group', choices=tuple(GROUPS), required=True)
    run_parser.add_argument('--product-sdk', type=Path)
    run_parser.add_argument('--package-release', type=Path)
    aggregate_parser = commands.add_parser('aggregate')
    aggregate_parser.add_argument('--repo', type=Path, default=Path.cwd())
    aggregate_parser.add_argument('--plan', type=Path, required=True)
    aggregate_parser.add_argument('--partials', type=Path, required=True)
    aggregate_parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args(argv)
    repo = args.repo.resolve(strict=True)
    plan = json.loads(args.plan.read_text())
    if args.command == 'run':
        result = run_group(repo, plan, args.output, args.group,
                           product_sdk=args.product_sdk,
                           package_release=args.package_release)
    else:
        result = aggregate_core(repo, plan, args.partials, args.output)
    print(json.dumps({'status': result['status'], 'suites': [row['id'] for row in result['suites']]},
                     indent=2))


if __name__ == '__main__':
    main()
