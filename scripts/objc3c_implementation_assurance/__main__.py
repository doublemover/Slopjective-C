"""Plan, execute and verify exact-revision implementation analysis."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import sys

from scripts.objc3c_assurance.__main__ import event_revisions
from scripts.objc3c_assurance.plan import sha
from scripts.objc3c_toolchain.core import ToolchainError

from .plan import from_checkout


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=('plan', 'foundation', 'run', 'verify'))
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--plan', type=Path, required=True)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--periodic-sha')
    parser.add_argument('--partition-index', type=int, default=0)
    parser.add_argument('--foundation-bundle', type=Path)
    parser.add_argument('--foundation-archive-sha256')
    parser.add_argument('--sanitizer-producer', type=Path)
    args = parser.parse_args(argv)
    repo = args.repo.resolve()
    revisions = (dict.fromkeys(('head', 'base', 'tested'), sha(args.periodic_sha))
                 if args.periodic_sha else event_revisions(repo))
    try:
        expected = from_checkout(repo, **revisions, periodic=bool(args.periodic_sha))
        if args.command == 'plan':
            args.plan.parent.mkdir(parents=True, exist_ok=True)
            args.plan.write_text(json.dumps(expected, indent=2)+'\n')
            if os.environ.get('GITHUB_OUTPUT'):
                with open(os.environ['GITHUB_OUTPUT'], 'a') as stream:
                    stream.write('analysis='+str(expected['required']).lower()+'\n')
                    stream.write('partitions='+json.dumps(list(range(expected['analysis_partitions'])))+'\n')
            print(json.dumps(expected))
            return 0
        plan = json.loads(args.plan.read_text())
        if plan != expected:
            raise ValueError('downloaded plan differs from the exact source revision')
        if not args.output:
            parser.error('--output is required for foundation, run and verify')
        if args.sanitizer_producer is not None and args.command != 'foundation':
            parser.error('--sanitizer-producer is only accepted by foundation')
        if args.foundation_archive_sha256 is not None and (args.command != 'run' or not args.foundation_bundle):
            parser.error('--foundation-archive-sha256 requires run with --foundation-bundle')
        if args.command == 'foundation':
            from .shared_foundation import produce
            produced = produce(repo, plan, args.output, sanitizer_producer=args.sanitizer_producer)
            report = {'status': produced['status'], 'identity': produced['identity'],
                      'archive_sha256': produced['archive']['sha256'],
                      'archive_bytes': produced['archive']['bytes']}
            if os.environ.get('GITHUB_OUTPUT'):
                with open(os.environ['GITHUB_OUTPUT'], 'a') as stream:
                    stream.write('archive_sha256='+report['archive_sha256']+'\n')
        elif args.command == 'run':
            from .runner import run
            report = run(repo, plan, args.output, partition_index=args.partition_index,
                         foundation_bundle=args.foundation_bundle,
                         foundation_archive_sha256=args.foundation_archive_sha256)
        else:
            from .evidence import aggregate
            report = aggregate(plan, args.output,
                outcomes=json.loads(os.environ['ASSURANCE_OUTCOMES']), tested=revisions['tested'])
        print(json.dumps(report, indent=2))
        return 0
    except (ValueError, KeyError, OSError, ToolchainError) as error:
        print('implementation-assurance: '+str(error), file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
