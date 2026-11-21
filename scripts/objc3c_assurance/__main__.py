"""Plan, execute or verify exact-revision native Linux assurance."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import sys

from .plan import SHARDS, execution_shards, from_checkout, sha


def event_revisions(repo: Path) -> dict:
    event = json.loads(Path(os.environ['GITHUB_EVENT_PATH']).read_text())
    tested = sha(os.environ['GITHUB_SHA'])
    if 'pull_request' in event:
        return dict(head=sha(event['pull_request']['head']['sha']),
                    base=sha(event['pull_request']['base']['sha']), tested=tested)
    before = event.get('before')
    # An initial push has no baseline. Comparing the commit to itself deliberately
    # produces an empty comparison, which selects the complete native surface.
    return dict(head=tested, base=sha(before) if before and before != '0'*40 else tested, tested=tested)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=('plan', 'run', 'verify', 'publish'))
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--plan', type=Path, required=True)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--periodic-sha')
    parser.add_argument('--shard', choices=SHARDS)
    parser.add_argument('--sharded', action='store_true')
    parser.add_argument('--destination', type=Path)
    args = parser.parse_args(argv)
    if args.sharded and args.command != 'verify':
        parser.error('--sharded is only valid for verification')
    if args.shard and args.command not in ('run', 'publish'):
        parser.error('--shard is only valid for execution or publication')
    if args.destination and args.command != 'publish':
        parser.error('--destination is only valid for publication')
    repo = args.repo.resolve()
    revisions = (dict.fromkeys(('head', 'base', 'tested'), sha(args.periodic_sha))
                 if args.periodic_sha else event_revisions(repo))
    try:
        expected = from_checkout(repo, **revisions, periodic=bool(args.periodic_sha))
        if args.command == 'plan':
            args.plan.parent.mkdir(parents=True, exist_ok=True)
            args.plan.write_text(json.dumps(expected, indent=2) + '\n')
            if os.environ.get('GITHUB_OUTPUT'):
                with open(os.environ['GITHUB_OUTPUT'], 'a') as stream:
                    stream.write('native=' + str(bool(expected['suites'])).lower() + '\n')
                    stream.write('shards=' + json.dumps(execution_shards(expected)) + '\n')
            print(json.dumps(expected))
            return 0
        plan = json.loads(args.plan.read_text())
        if plan != expected:
            raise ValueError('downloaded plan differs from current revision and changed paths')
        if not args.output:
            parser.error('--output is required for run and verify')
        if args.command == 'run':
            from .runner import run
            report = run(repo, plan, args.output, shard=args.shard)
        elif args.command == 'publish':
            if not args.destination:
                parser.error('--destination is required for publication')
            from .evidence import publish
            report = publish(plan, args.output, args.destination, shard=args.shard)
        else:
            from .evidence import aggregate, aggregate_shards
            verify = aggregate_shards if args.sharded else aggregate
            report = verify(plan, args.output, outcomes=json.loads(os.environ['ASSURANCE_OUTCOMES']),
                               tested=revisions['tested'])
        print(json.dumps(report, indent=2))
        return 0
    except (ValueError, KeyError, OSError) as error:
        print('native-assurance: ' + str(error), file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
