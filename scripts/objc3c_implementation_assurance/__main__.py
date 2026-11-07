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
    parser.add_argument('command', choices=('plan', 'run', 'verify'))
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--plan', type=Path, required=True)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--periodic-sha')
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
            print(json.dumps(expected))
            return 0
        plan = json.loads(args.plan.read_text())
        if plan != expected:
            raise ValueError('downloaded plan differs from the exact source revision')
        if not args.output:
            parser.error('--output is required for run and verify')
        if args.command == 'run':
            from .runner import run
            report = run(repo, plan, args.output)
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
