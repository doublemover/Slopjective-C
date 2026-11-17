"""Compare uniform and feedback generation against real patch-owned source coverage."""
from __future__ import annotations

from .linux_host import native_linux_host

import argparse
from dataclasses import asdict
import gzip
import json
import os
from pathlib import Path
import random
import re
import shutil
import statistics
import tempfile
import time

from .binary_packages import digest, installed_shared_library, write_json
from .compiler_source_coverage import (
    EXPORT_FLAGS, PROFILE, compile_inventory, compiler_raw_profiles, patched_inventory,
    require_binary_ids,
)
from .core import build_plans
from .llvm_tools import identity
from .semantic_generation import cpu_seconds
from .semantic_model import VERSION, OPERATIONS, Operation, coverage, eligibility, generate
from .semantic_oracles import Campaign
from .source_coverage import binary_ids, elf_build_id

STRATEGIES = ('uniform', 'source-feedback')
GENERATOR_VERSION = 'source-feedback.v1'
MAX_POINTS = 500_000


def indexed(path: Path, document) -> dict:
    payload = json.dumps(document, sort_keys=True, separators=(',', ':')).encode()
    path.write_bytes(gzip.compress(payload, compresslevel=6, mtime=0))
    return {'path': str(path), 'sha256': digest(path), 'bytes': path.stat().st_size}


def source_points(text: str, source: Path, eligible: dict) -> tuple[set, set]:
    """Count source entry points, separately from fuzzer edges and model states."""
    if len(text.encode()) > 256 * 1024**2:
        raise ValueError('source-feedback export exceeded its bound')
    document = json.loads(text)
    if document.get('type') != 'llvm.coverage.json.export' or not document.get('data'):
        raise ValueError('missing actual compiler source-feedback export')
    mapped, covered, seen = set(), set(), set()
    for section in document['data']:
        for row in section['files']:
            path = Path(row['filename']).resolve()
            if not path.is_relative_to(source):
                continue
            name = path.relative_to(source).as_posix()
            if name not in eligible:
                continue
            if name in seen:
                raise ValueError('duplicate source-feedback mapping owner')
            seen.add(name)
            for segment in row.get('segments', []):
                if (len(segment) < 5 or any(type(segment[i]) is not int for i in (0, 1, 2)) or
                        segment[0] <= 0 or segment[1] <= 0 or segment[2] < 0 or
                        any(type(segment[i]) is not bool for i in (3, 4)) or
                        (len(segment) > 5 and type(segment[5]) is not bool)):
                    raise ValueError('invalid compiler source-feedback segment')
                if not segment[3] or not segment[4] or (len(segment) > 5 and segment[5]):
                    continue
                point = (name, segment[0], segment[1])
                mapped.add(point)
                if segment[2]:
                    covered.add(point)
                if len(mapped) > MAX_POINTS:
                    raise ValueError('source-feedback mapping exceeded its point bound')
    if not mapped or not covered:
        raise ValueError('source-feedback case has no executed patch-owned source entry points')
    return mapped, covered


class Generator:
    """Select parents using observed target coverage; expectations stay in the model."""
    def __init__(self, seed: int):
        self.seed, self.random = seed, random.Random(seed)
        self.archive, self.covered = [], set()

    def propose(self) -> list[Operation]:
        if not self.archive:
            return generate(self.seed)
        best = max(gain for _, gain in self.archive)
        for _ in range(16):
            parent = self.random.choice([program for program, gain in self.archive if gain == best])
            result = list(parent)
            # The reviewed prefix preserves the complete operation/transition bank.
            for index in self.random.sample(range(15, 24), self.random.randint(1, 4)):
                result[index] = Operation(self.random.choice(OPERATIONS), self.random.randrange(2),
                    self.random.randint(-20, 20), self.random.randrange(2))
            eligibility(result)
            if all(tuple(result) != prior for prior, _ in self.archive):
                return result
        raise ValueError('bounded source-feedback mutator could not produce a new witness')

    def observe(self, program: list[Operation], points: set) -> int:
        eligibility(program)
        gain = len(points - self.covered)
        self.covered.update(points)
        # The bootstrap program establishes a baseline, not a permanent reward
        # larger than every later discovery combined.
        self.archive.append((tuple(program), gain if self.archive else 0))
        # Bounded elitism retains discovered parents without an unbounded corpus.
        self.archive = sorted(self.archive, key=lambda row: row[1], reverse=True)[:8]
        return gain


def admit_baseline(repo: Path, prior: dict, services: Path, runtime: Path) -> dict:
    normal = build_plans(repo, host=native_linux_host(), instrumentation_profile='compiler-asan-ubsan')['configure']
    expected = {name: 'cleanup-order' if name in ('defer', 'destructor') else 'value-or-lifetime'
                for name in ('tag', 'defer', 'destructor', 'provider', 'error')}
    controls = {row.get('name'): row for row in prior.get('controls', [])}
    if (prior.get('schema') != 'objc3c.generated-semantic-oracles.v2' or
            prior.get('status') != 'PASS' or prior.get('model_version') != VERSION or
            len(controls) != len(prior.get('controls', [])) or
            not prior.get('expected_cases') or
            len(prior['expected_cases']) != len(set(prior['expected_cases'])) or
            {row.get('name') for row in prior.get('cases', [])} != set(prior['expected_cases']) or
            len(prior.get('cases', [])) != len(prior['expected_cases']) or
            any(row.get('status') != 'PASS' or row.get('mismatch') is not None for row in prior['cases']) or
            prior.get('source_build_id') != normal['source_build_id'] or
            prior.get('build_config_digest') != normal['build_config_digest'] or
            prior.get('source_tree') != normal['source_tree'] or
            any(controls.get('control-'+name, {}).get('mismatch') != mismatch or
                controls.get('control-'+name, {}).get('status') != 'PASS'
                for name, mismatch in expected.items())):
        raise ValueError('source-feedback experiment requires the current independent-oracle baseline and controls')
    for path in (services, runtime, *(Path(__file__).with_name(name) for name in
            ('semantic_model.py', 'semantic_sources.py', 'semantic_oracles.py'))):
        if prior.get('inputs', {}).get(str(path)) != digest(path):
            raise ValueError('source-feedback baseline model/runtime input changed: '+str(path))
    return normal


class Feedback:
    def __init__(self, campaign: Campaign, profile_root: Path):
        self.campaign, self.root = campaign, profile_root
        self.plan = campaign.foundation
        self.source = Path(self.plan['paths']['source']).resolve(strict=True)
        self.build = Path(self.plan['paths']['build']).resolve(strict=True)
        self.eligible, self.changed = patched_inventory(campaign.repo, self.source)
        self.objects = compile_inventory(self.build, self.source, self.eligible)
        self.identities = {}
        stage = Path(self.plan['paths']['stage'])
        self.owner = installed_shared_library(stage / 'lib', 'libclang-cpp.so')
        paths = [*campaign.tools.values(), self.owner, installed_shared_library(stage / 'lib', 'libLLVM.so')]
        for index, path in enumerate(dict.fromkeys(path.resolve(strict=True) for path in paths)):
            self.identities[str(path)] = elf_build_id(campaign.run('feedback-notes-'+str(index),
                [campaign.tools['llvm-readobj'], '--notes', path]))
        self.owner_id = self.identities[str(self.owner.resolve(strict=True))]
        self.mapping = None
        campaign.inputs.update({str(self.source / name): value for name, value in self.eligible.items()})
        campaign.inputs.update({row['object']: row['sha256'] for row in self.objects})
        campaign.report['compiler_source_coverage'] = {
            'owner': 'shared-clang-cpp-patch-owned-whole-file-source-entry-points',
            'profile': PROFILE, 'eligible': self.eligible, 'objects': self.objects,
            'owner_build_id': self.owner_id, 'allowed_profile_ids': sorted(set(self.identities.values())),
            'scope': 'LLVM source entry points in patch-owned Clang files; not fuzzer edges, model states or patch-hunk percentages.'}

    def begin(self, name):
        directory = self.root / name
        directory.mkdir()
        self.campaign.executor.environment['LLVM_PROFILE_FILE'] = str(directory / '%b-%8m.profraw')
        return directory

    def collect(self, name: str, directory: Path) -> tuple[set, dict]:
        self.campaign.executor.environment['LLVM_PROFILE_FILE'] = os.devnull
        try:
            profiles = compiler_raw_profiles(directory)
            selected, observed = [], set()
            for path in profiles:
                matched = re.fullmatch(r'([0-9a-f]{40})-.+\.profraw', path.name)
                if not matched:
                    raise ValueError('source-feedback profile omitted its binary identity')
                observed.add(matched[1])
                if matched[1] == self.owner_id:
                    selected.append(path)
            require_binary_ids(observed, required={self.owner_id}, allowed=set(self.identities.values()), kind='source-feedback raw profiles')
            merged = directory / 'compiler.profdata'
            self.campaign.run(name+'-merge', [self.campaign.tools['llvm-profdata'], 'merge', '--sparse', *selected, '-o', merged])
            observed = binary_ids(self.campaign.run(name+'-profile-ids',
                [self.campaign.tools['llvm-profdata'], 'show', '--binary-ids', merged]))
            require_binary_ids(observed, required={self.owner_id}, allowed={self.owner_id}, kind='source-feedback indexed profile')
            text = self.campaign.run(name+'-export', [self.campaign.tools['llvm-cov'], 'export', self.owner,
                *EXPORT_FLAGS, '--instr-profile='+str(merged), '--compilation-dir='+str(self.build),
                *(self.source / entry for entry in self.eligible)])
            mapped, covered = source_points(text, self.source, self.eligible)
            if self.mapping is None:
                self.mapping = tuple(sorted(mapped))
                self.indices = {point: index for index, point in enumerate(self.mapping)}
                self.campaign.report['compiler_source_coverage']['mapping'] = indexed(
                    self.campaign.output / 'source-mapping.json.gz', self.mapping)
            elif mapped != set(self.mapping):
                raise ValueError('source-feedback compiler mapping changed between paired cases')
            hits = indexed(self.campaign.output / (name+'-coverage.json.gz'),
                           sorted(self.indices[point] for point in covered))
            export = next(row for row in self.campaign.report['commands'] if row['name'] == name+'-export')
            raw_export = {'sha256': export['log_sha256'], 'bytes': len(text.encode()), 'reclaimed': True}
            write_json(Path(export['log']), {'indexed_source_coverage': hits, 'raw_export': raw_export})
            export['log_sha256'] = digest(Path(export['log']))
            return covered, {'covered_points': len(covered), 'mapping_points': len(mapped),
                'indexed_hits': hits, 'raw_export': raw_export, 'profile_sha256': digest(merged),
                'profile_pattern': str(directory / '%b-%8m.profraw'), 'temporary_profiles_reclaimed': True,
                'raw_profiles': {'count': len(profiles), 'bytes': sum(path.stat().st_size for path in profiles),
                                 'sha256': {path.name: digest(path) for path in profiles}}}
        finally:
            shutil.rmtree(directory)


class Window:
    def __init__(self, campaign, seconds):
        self.campaign, self.seconds = campaign, seconds
        self.started, self.cpu = time.monotonic(), cpu_seconds()
        self.original = campaign.run

    def remaining(self):
        remaining = min(self.seconds-(time.monotonic()-self.started), self.seconds-(cpu_seconds()-self.cpu))
        if remaining <= 0:
            raise ValueError('source-feedback strategy exhausted its fixed wall/CPU budget')
        return remaining

    def run(self, name, argv, **kwargs):
        kwargs['timeout'] = min(kwargs.get('timeout') or self.campaign.executor.timeout, self.remaining())
        return self.original(name, argv, **kwargs)

    def metrics(self):
        self.remaining()
        return {'wall_budget_seconds': self.seconds, 'cpu_budget_seconds': self.seconds,
                'wall_seconds': time.monotonic()-self.started, 'cpu_seconds': cpu_seconds()-self.cpu}


def paired_summary(rows: list[dict], seeds) -> dict:
    indexed_rows = {(row['seed'], row['strategy']): row for row in rows}
    if (len(rows) != 2*len(seeds) or len(indexed_rows) != len(rows) or
            set(indexed_rows) != {(seed, strategy) for seed in seeds for strategy in STRATEGIES}):
        raise ValueError('source-feedback paired comparison is incomplete or duplicated')
    pairs = []
    for seed in seeds:
        uniform, adaptive = (indexed_rows[seed, strategy] for strategy in STRATEGIES)
        pairs.append({'seed': seed,
            'covered_point_difference': adaptive['covered_points']-uniform['covered_points'],
            'covered_point_ratio': adaptive['covered_points']/uniform['covered_points'],
            'eligible_program_rate_ratio': adaptive['eligible_programs_per_second']/uniform['eligible_programs_per_second'],
            'cpu_seconds_ratio': adaptive['cpu_seconds']/uniform['cpu_seconds']})
    return {'pairs': pairs,
        'median_covered_point_ratio': statistics.median(row['covered_point_ratio'] for row in pairs),
        'median_program_rate_ratio': statistics.median(row['eligible_program_rate_ratio'] for row in pairs),
        'default_strategy': 'uniform', 'promotion': 'not-authorized-by-this-development-sample'}


def measure(repo: Path, output: Path, baseline: Path, *, services: Path, runtime: Path,
            seeds=(7, 19, 41), rounds=6, budget=180):
    repo, output, baseline = repo.resolve(strict=True), output.resolve(), baseline.resolve(strict=True)
    services, runtime = services.resolve(strict=True), runtime.resolve(strict=True)
    if (output.exists() or not output.is_relative_to(repo / 'out') or
            type(rounds) is not int or not 3 <= rounds <= 16 or type(budget) is not int or not 30 <= budget <= 600 or
            not 3 <= len(seeds) <= 8 or len(set(seeds)) != len(seeds) or
            any(type(seed) is not int or not 0 <= seed < 2**32 for seed in seeds)):
        raise ValueError('source-feedback comparison requires fresh owned output, 3..8 seeds and bounded rounds/time')
    prior = json.loads(baseline.read_text())
    normal = admit_baseline(repo, prior, services, runtime)
    output.mkdir(parents=True)
    campaign, profile_root = None, None
    report = {'schema': 'objc3c.semantic-source-feedback.v1', 'status': 'RUNNING',
              'subject': 'non-promoting-paired-source-feedback-measurement'}
    try:
        campaign = Campaign(repo, output, None, services, runtime, compiler_profile=PROFILE)
        if campaign.foundation['source_tree'] != normal['source_tree']:
            raise ValueError('paired source-feedback profiles refer to different compiler sources')
        report = campaign.report
        report.update(schema='objc3c.semantic-source-feedback.v1', generator_version=GENERATOR_VERSION,
            subject='non-promoting-paired-source-feedback-measurement', seeds=list(seeds), rounds=rounds,
            baseline={'path': str(baseline), 'sha256': digest(baseline)}, measurements=[],
            adaptive_generation_enabled=False,
            comparison_limits='Bounded paired development sample with counterbalanced order; default generation is unchanged. No promotion from this sample alone.')
        for name in ('semantic_source_feedback.py', 'compiler_source_coverage.py', 'semantic_generation.py'):
            path = Path(__file__).with_name(name);campaign.inputs[str(path)] = digest(path)
        campaign.inputs[str(baseline)] = digest(baseline)
        profile_root = Path(tempfile.mkdtemp(prefix='objc3-generation-coverage-', dir='/tmp'))
        if shutil.disk_usage(profile_root).free < 3 * 1024**3:
            raise ValueError('source-feedback comparison needs 3 GiB of temporary profile storage')
        report['temporary_profile_root'] = str(profile_root)
        feedback = Feedback(campaign, profile_root)
        # Prove that coverage-profile execution still detects the same independent
        # wrong-code controls before considering feedback as a generation signal.
        for mutant in ('tag', 'defer', 'destructor', 'provider', 'error'):
            campaign.case('coverage-control-'+mutant, generate(seeds[0]), mutant=mutant,
                expected_mismatch='cleanup-order' if mutant in ('defer', 'destructor') else 'value-or-lifetime')
        for ordinal, seed in enumerate(seeds):
            campaign.case('warmup-'+str(seed), generate(seed))
            for strategy in STRATEGIES if ordinal % 2 == 0 else tuple(reversed(STRATEGIES)):
                generator, witnesses, union, trajectory = Generator(seed), [], set(), []
                window = Window(campaign, budget);campaign.run = window.run
                try:
                    for turn in range(rounds):
                        witness = generator.propose() if strategy == 'source-feedback' else generate((seed+turn*0x9e3779b9) % 2**32)
                        name = f'seed-{seed}-{strategy}-{turn}'
                        directory = feedback.begin(name)
                        campaign.checked_case(name, witness)
                        points, receipt = feedback.collect(name, directory)
                        gain = len(points-union);union.update(points);witnesses.append(witness)
                        if strategy == 'source-feedback':
                            if generator.observe(witness, points) != gain:
                                raise ValueError('source-feedback generator and measured coverage disagree')
                        trajectory.append({'name': name, 'program_sha256': identity([asdict(op) for op in witness]),
                            'new_points': gain, 'union_points': len(union), **receipt})
                        window.remaining();campaign.executor.save()
                    metrics = window.metrics()
                finally:
                    campaign.run = window.original
                    campaign.executor.environment['LLVM_PROFILE_FILE'] = os.devnull
                report['measurements'].append({'seed': seed, 'strategy': strategy, 'execution_order': len(report['measurements']),
                    'programs': len(witnesses), 'operations_per_program': 24, 'covered_points': len(union),
                    'eligible_programs_per_second': len(witnesses)/metrics['wall_seconds'],
                    'semantic_coverage': coverage(witnesses), 'trajectory': trajectory,
                    'reproducing_mismatch_classes': [], **metrics})
                campaign.executor.save()
        if any(digest(Path(path)) != value for path, value in campaign.inputs.items()):
            raise ValueError('source-feedback compiler/model/runtime inputs changed during measurement')
        report['comparison'] = paired_summary(report['measurements'], seeds)
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error));raise
    finally:
        try:
            if profile_root is not None and profile_root.exists():
                shutil.rmtree(profile_root)
        except BaseException as error:
            report.update(status='FAIL', cleanup_error=str(error))
            raise
        finally:
            report['temporary_profiles_reclaimed'] = profile_root is None or not profile_root.exists()
            if campaign is not None:
                campaign.executor.save()
            else:
                write_json(output / 'qualification.json', report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    for name in ('output', 'baseline', 'services', 'runtime'):
        parser.add_argument('--'+name, type=Path, required=True)
    parser.add_argument('--seeds', type=int, nargs='+', default=[7, 19, 41])
    parser.add_argument('--rounds', type=int, default=6)
    parser.add_argument('--budget', type=int, default=180)
    report = measure(**vars(parser.parse_args()))
    print(json.dumps({'status': report['status'], 'paired_samples': len(report['measurements'])//2,
                      'adaptive_generation_enabled': report['adaptive_generation_enabled']}))


if __name__ == '__main__':
    main()
