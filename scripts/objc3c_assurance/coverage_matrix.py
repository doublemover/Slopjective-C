"""Build once, execute eight coverage partitions, and prove their exact union."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.build import build_foundation
from scripts.objc3c_toolchain.compiler_source_coverage import qualify as compiler_coverage
from scripts.objc3c_toolchain.core import build_plans, verify_development
from scripts.objc3c_toolchain.coverage_controls import qualify as coverage_controls
from scripts.objc3c_toolchain.semantic_build_costs import snapshot
from scripts.objc3c_toolchain.source_coverage import (
    qualify as first_party_coverage, validate_foundation_identity)
from .emitter_shards import emitter_partition
from .evidence import MAX_BYTES, SCHEMA, aggregate, artifact, junit, native_files, suite_results
from .plan import from_checkout, selected_suites, validate

PARTITIONS = 8
PROFILE = 'compiler-source-coverage'


def _require_checkout(repo: Path, plan: dict) -> None:
    if from_checkout(repo, **plan['revision'], periodic=plan['periodic']) != plan:
        raise ValueError('coverage matrix plan differs from the exact checked-out revision')


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def _profile_plan(repo: Path) -> dict:
    return build_plans(repo, host='linux-x86_64', instrumentation_profile=PROFILE)['configure']


def produce(repo: Path, plan: dict, output: Path, bundle: Path) -> dict:
    validate(plan)
    _require_checkout(repo, plan)
    output, bundle = output.resolve(), bundle.resolve()
    if (output.exists() or bundle.exists() or not output.is_relative_to(repo / 'out') or
            not bundle.is_relative_to(repo / 'out')):
        raise ValueError('coverage producer requires fresh owned outputs')
    output.mkdir(parents=True)
    merge_results = output / 'profile-merge-regressions.xml'
    subprocess.run([sys.executable, '-B', '-m', 'pytest', '-q',
        str(repo / 'tests/tooling/test_source_coverage_profile_merging.py'),
        '--junitxml='+str(merge_results)], cwd=repo, check=True)
    if len(junit(merge_results)) != 4:
        raise ValueError('native profile merge regression inventory is incomplete')

    evidence = output / 'foundations' / PROFILE
    evidence.mkdir(parents=True)
    try:
        foundation = build_foundation(repo, host='linux-x86_64', fetch=True, stage=True,
                                      instrumentation_profile=PROFILE, test_results=evidence)
    finally:
        configured = _profile_plan(repo)
        receipt = Path(configured['paths']['build']) / 'foundation-build-report.json'
        if receipt.is_file():
            saved = json.loads(receipt.read_text())
            write_json(evidence / 'foundation.json', saved)
            for index, phase in enumerate(saved.get('phases', [])):
                source = Path(phase['log'])
                if source.is_file():
                    shutil.copyfile(source, evidence / ('phase-'+str(index)+'.log'))
    configured = _profile_plan(repo)
    if foundation.get('status') != 'passed':
        raise ValueError('coverage profile foundation did not pass')
    controls = coverage_controls(repo, output / 'controls')
    compiler = compiler_coverage(repo, output / 'compiler', jobs=2)
    if controls.get('status') != 'PASS' or compiler.get('status') != 'PASS':
        raise ValueError('coverage producer controls or compiler denominator did not pass')

    stage = Path(configured['paths']['stage']).resolve(strict=True)
    build = Path(configured['paths']['build']).resolve(strict=True)
    source_root = Path(configured['paths']['source']).resolve(strict=True)
    build_receipt = build / 'foundation-build-report.json'
    portable_inputs = [
        build / 'bin/llvm-tblgen',
        source_root / 'clang/tools/clang-scan-deps/ClangScanDeps.cpp',
        source_root / 'clang/tools/clang-scan-deps/Opts.td',
        source_root / 'llvm/include/llvm/Option/OptParser.td',
    ]
    for path in portable_inputs:
        path.resolve(strict=True)
        if not path.is_file() or path.is_symlink():
            raise ValueError('coverage profile portable build input is not a regular file')
    relative_stage = stage.relative_to(repo)
    relative_receipt = build_receipt.relative_to(repo)
    relative_inputs = [path.relative_to(repo) for path in portable_inputs]
    bundle.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(['tar', '-I', 'zstd -T2 -3', '-cf', str(bundle), '-C', str(repo),
                    str(relative_stage), str(relative_receipt),
                    *map(str, relative_inputs)], check=True)
    source = verify_development(repo)
    foundation_identity = {
        'schema': 'objc3c.portable-foundation-identity.v1',
        'profile': PROFILE,
        'source_build_id': configured['source_build_id'],
        'build_config_digest': configured['build_config_digest'],
        'upstream_commit': source['upstream_commit'],
        'patch_digest': source['patch_digest'],
        'paths': configured['paths'],
        'instrumentation': configured['instrumentation'],
    }
    report = {'schema': 'objc3c.coverage-profile-producer.v1', 'status': 'PASS',
              'revision': plan['revision'], 'plan_sha256': plan['sha256'],
              'source_build_id': configured['source_build_id'],
              'build_config_digest': configured['build_config_digest'],
              'foundation_plan': configured,
              'foundation_identity': foundation_identity,
              'profile': PROFILE, 'partitions': PARTITIONS,
              'bundle': {'path': str(bundle), 'sha256': digest(bundle),
                         'bytes': bundle.stat().st_size,
                         'members': [str(relative_stage), str(relative_receipt),
                                     *map(str, relative_inputs)]},
              'foundation_source_identity': foundation['source_identity'],
              'compiler_suites': len(compiler['suites']),
              'controls': len(controls['controls']), 'merge_regressions': 4}
    write_json(output / 'producer.json', report)
    return report


def restore(repo: Path, plan: dict, producer: Path, bundle: Path) -> dict:
    validate(plan)
    _require_checkout(repo, plan)
    producer = producer.resolve(strict=True)
    bundle = bundle.resolve(strict=True)
    report = json.loads(producer.read_text())
    configured = report.get('foundation_plan', {})
    identity = report.get('foundation_identity')
    if (report.get('schema') != 'objc3c.coverage-profile-producer.v1' or
            report.get('status') != 'PASS' or report.get('revision') != plan['revision'] or
            report.get('plan_sha256') != plan['sha256'] or report.get('profile') != PROFILE or
            report.get('partitions') != PARTITIONS or
            report.get('source_build_id') != configured.get('source_build_id') or
            report.get('build_config_digest') != configured.get('build_config_digest') or
            report.get('bundle', {}).get('sha256') != digest(bundle) or
            report['bundle'].get('bytes') != bundle.stat().st_size):
        raise ValueError('coverage profile bundle is stale, substituted or incomplete')
    validate_foundation_identity(repo, configured, identity)
    stage = Path(configured['paths']['stage'])
    receipt = Path(configured['paths']['build']) / 'foundation-build-report.json'
    if stage.exists() or receipt.exists():
        raise ValueError('coverage profile restore refuses pre-existing build products')
    members = subprocess.check_output(['tar', '-I', 'zstd', '-tf', str(bundle)], text=True).splitlines()
    allowed = tuple(member.rstrip('/') for member in report['bundle']['members'])
    if (not members or any(name.startswith(('/', '../')) or '/..' in name or
            not any(name.rstrip('/') == prefix or name.startswith(prefix+'/') for prefix in allowed)
            for name in members)):
        raise ValueError('coverage profile bundle contains an escaped or unowned member')
    subprocess.run(['tar', '-I', 'zstd', '-xf', str(bundle), '-C', str(repo)], check=True)
    completed = json.loads(receipt.read_text())
    if (completed.get('status') != 'passed' or
            completed.get('source_identity') != configured['source_build_id']):
        raise ValueError('restored coverage profile receipt differs from the exact build')
    return report


def run_partition(repo: Path, plan: dict, producer: Path, bundle: Path,
                  output: Path, index: int) -> dict:
    profile = restore(repo, plan, producer, bundle)
    result = first_party_coverage(repo, output, jobs=2, partition_index=index,
                                  partition_count=PARTITIONS,
                                  foundation_plan=profile['foundation_plan'],
                                  foundation_identity=profile['foundation_identity'])
    result['profile_bundle'] = {'sha256': profile['bundle']['sha256'],
                                'source_build_id': profile['source_build_id'],
                                'build_config_digest': profile['build_config_digest']}
    write_json(output / 'qualification.json', result)
    return result


def _partition_root(root: Path, index: int) -> Path:
    direct = root / str(index)
    if (direct / 'qualification.json').is_file():
        return direct
    matches = [path.parent for path in root.rglob('qualification.json')
               if path.parent.name in (str(index), 'partition-'+str(index))]
    if len(matches) != 1:
        raise ValueError('coverage partition evidence is missing or duplicated: '+str(index))
    return matches[0]


def aggregate_partitions(repo: Path, plan: dict, producer_root: Path,
                         partitions_root: Path, output: Path) -> dict:
    validate(plan)
    _require_checkout(repo, plan)
    producer_root = producer_root.resolve(strict=True)
    partitions_root = partitions_root.resolve(strict=True)
    output = output.resolve()
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('coverage aggregate requires a fresh owned output')
    producer = json.loads((producer_root / 'producer.json').read_text())
    if (producer.get('status') != 'PASS' or producer.get('revision') != plan['revision'] or
            producer.get('plan_sha256') != plan['sha256'] or producer.get('partitions') != PARTITIONS):
        raise ValueError('coverage producer evidence is stale or incomplete')

    suite_root = output / 'source-coverage'
    suite_root.mkdir(parents=True)
    for name in ('profile-merge-regressions.xml', 'controls', 'compiler', 'foundations'):
        source = producer_root / name
        target = suite_root / name
        if source.is_dir(): shutil.copytree(source, target)
        else: shutil.copyfile(source, target)
    first_party = suite_root / 'first-party'
    (first_party / 'suites').mkdir(parents=True)
    reports = []
    suites = {}
    for index in range(PARTITIONS):
        root = _partition_root(partitions_root, index)
        report = json.loads((root / 'qualification.json').read_text())
        partition = report.get('partition', {})
        if (report.get('status') != 'PASS' or report.get('revision') != plan['revision']['tested'] or
                report.get('source_build_id') != producer['source_build_id'] or
                report.get('build_config_digest') != producer['build_config_digest'] or
                report.get('profile_bundle') != {
                    'sha256': producer['bundle']['sha256'],
                    'source_build_id': producer['source_build_id'],
                    'build_config_digest': producer['build_config_digest']} or
                partition.get('index') != index or partition.get('count') != PARTITIONS):
            raise ValueError('coverage partition is stale, failed or substituted: '+str(index))
        reports.append(report)
        for row in report.get('suites', []):
            key = (row.get('component'), row.get('name'))
            if key in suites:
                raise ValueError('coverage partitions overlap: '+repr(key))
            source = root / 'suites' / (key[0]+'-'+key[1])
            if not source.is_dir():
                raise ValueError('coverage partition omitted suite evidence: '+repr(key))
            shutil.copytree(source, first_party / 'suites' / source.name)
            suites[key] = row
    policy = reports[0]['policy']
    catalogue = [(component, name) for component in ('frontend', 'runtime')
                 for name in policy[component+'_tests']]
    if (set(suites) != set(catalogue) or len(suites) != len(catalogue) or
            any(report.get('policy') != policy or
                report.get('partition', {}).get('catalogue_sha256') !=
                reports[0]['partition']['catalogue_sha256'] for report in reports)):
        raise ValueError('coverage partitions do not form the exact reviewed catalogue')
    ordered = [suites[key] for key in catalogue]
    merged = {key: value for key, value in reports[0].items()
              if key not in ('commands', 'suites', 'objects', 'inputs', 'partition',
                             'temporary_build_root', 'seconds', 'emitter_partition')}
    merged.update(status='PASS', suites=ordered,
                  commands=[row for report in reports for row in report.get('commands', [])],
                  objects={str(index): report.get('objects', {}) for index, report in enumerate(reports)},
                  inputs={str(index): report.get('inputs', {}) for index, report in enumerate(reports)},
                  partitions=[report['partition'] for report in reports],
                  temporary_build_reclaimed=all(report.get('temporary_build_reclaimed') for report in reports),
                  seconds=max(float(report.get('seconds', 0)) for report in reports),
                  profile_bundle=reports[0]['profile_bundle'])
    for component in ('frontend', 'runtime'):
        key = component+'-unselected-tests'
        merged['exclusions'][key] = sorted(set.intersection(
            *(set(report['exclusions'][key]) for report in reports)))
    merged['emitter_partition'] = emitter_partition(first_party, ordered)
    write_json(first_party / 'qualification.json', merged)

    suite = selected_suites(plan, 'source-coverage')
    if [row['id'] for row in suite] != ['source-coverage']:
        raise ValueError('coverage aggregate differs from the canonical source-coverage shard')
    executions = suite_results(output, 'source-coverage')
    files = native_files(suite_root)
    row = {'id': 'source-coverage', 'status': 'PASS', 'executions': executions,
           'artifacts': [artifact(output, path) for path in files]}
    foundation = suite_root / 'foundations' / PROFILE / 'foundation.json'
    costs = snapshot(foundations=[foundation])
    write_json(output / 'operational-costs.json', costs)
    result = {'schema': SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'shard': 'source-coverage',
              'suites': [row], 'commands': [],
              'cache': {'restored_build_products': True,
                        'reason': 'exact same-run coverage profile bundle; archive hash and source-build ID required'},
              'coverage_partitions': PARTITIONS}
    result['operational_costs'] = artifact(output, output / 'operational-costs.json')
    total = sum(saved['bytes'] for saved in row['artifacts']) + result['operational_costs']['bytes']
    if total > MAX_BYTES:
        raise ValueError('merged coverage evidence exceeds the native artifact budget')
    write_json(output / 'result.json', result)
    aggregate(plan, output, outcomes={'plan': 'success', 'native': 'success'},
              tested=plan['revision']['tested'], shard='source-coverage')
    return result


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    common = argparse.ArgumentParser(add_help=False)
    common.add_argument('--repo', type=Path, default=Path.cwd())
    common.add_argument('--plan', type=Path, required=True)
    producer = commands.add_parser('produce', parents=[common])
    producer.add_argument('--output', type=Path, required=True)
    producer.add_argument('--bundle', type=Path, required=True)
    partition = commands.add_parser('partition', parents=[common])
    partition.add_argument('--producer', type=Path, required=True)
    partition.add_argument('--bundle', type=Path, required=True)
    partition.add_argument('--output', type=Path, required=True)
    partition.add_argument('--index', type=int, choices=range(PARTITIONS), required=True)
    merged = commands.add_parser('aggregate', parents=[common])
    merged.add_argument('--producer-root', type=Path, required=True)
    merged.add_argument('--partitions-root', type=Path, required=True)
    merged.add_argument('--output', type=Path, required=True)
    args = parser.parse_args(argv)
    repo = args.repo.resolve(strict=True)
    plan = json.loads(args.plan.read_text())
    if args.command == 'produce':
        result = produce(repo, plan, args.output, args.bundle)
    elif args.command == 'partition':
        result = run_partition(repo, plan, args.producer, args.bundle, args.output, args.index)
    else:
        result = aggregate_partitions(repo, plan, args.producer_root,
                                      args.partitions_root, args.output)
    print(json.dumps({'status': result['status']}, indent=2))


if __name__ == '__main__':
    main()
