#!/bin/sh
set -eu
export LC_ALL=C
: "${OBJC3C_SDK_ROOT:?activate the installed Linux SDK first}"
example=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
scratch=$(mktemp -d)
trap 'rm -rf -- "$scratch"' EXIT

sh "$example/run.sh" "$OBJC3C_SDK_ROOT" "$scratch/quotes" > "$scratch/output.log"
cmp - "$scratch/output.log" <<'EXPECTED'
Books (1250 g, standard): $8.00, 5 business days
Glassware (750 g, express): $10.50, 2 business days
Empty parcel (0 g, standard): rejected, weight must be positive
Oversized carton (22000 g, express): rejected, weight exceeds 20 kg
EXPECTED
test ! -e "$scratch/quotes/producer"
python3 - "$scratch/quotes/consumer-build/consumer-report.json" <<'PY'
import json
from pathlib import Path
import sys

report = json.loads(Path(sys.argv[1]).read_bytes())
if report.get('status') != 'PASS' or report.get('provider_compilations') != 0:
    raise SystemExit('parcel quote consumer did not use only the installed provider')
PY
cat "$scratch/output.log"
