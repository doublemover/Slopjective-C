# Summarize a log file

This command-line application counts `INFO`, `WARN`, and `ERROR` records in a
text file. Blank lines are skipped. An unrecognized level produces a diagnostic
with the filename and line number.

Activate the SDK as shown in [Getting started](getting_started.md). Download
[log-summary.objc3](../../showcase/log-summary/library/log-summary.objc3),
[sample.log](../../showcase/log-summary/sample.log), and
[invalid.log](../../showcase/log-summary/invalid.log) to an empty directory.
Then compile and run:

```sh
objc3c log-summary.objc3 -O2 -Wall -Wextra -Werror -o log-summary
./log-summary sample.log
```

```text
INFO 3
WARN 1
ERROR 1
```

```sh
./log-summary invalid.log
```

The second invocation exits with status 1 and prints
`invalid.log:2: expected INFO, WARN, or ERROR` to standard error.

- `level_start` returns `Optional(size_t)`. A blank line has no token.
- `parse_level` returns `Result(Level, LogFailure)`. The parser leaves the
  choice of diagnostic and recovery to its caller.
- `checked_level` converts a rejected level into a typed `id<Error>`. The
  caller uses `tryValue` to propagate it into the local handler which supplies
  the filename and line number.
- `defer` frees the line buffer and closes the file when the function returns,
  including the early return from the error handler.

## Split the application into a library and a caller

Use the [full example directory](../../showcase/log-summary/) from the repository
or copy it from an SDK that includes `share/objc3c/examples/log-summary`. Run
from the copy so the application can open `sample.log` by its relative path.

```sh
objc3c-build --workspace workspace.json --output-dir ./build --optimization O2
./build/application.bin sample.log
```

The workspace exports `summarize_log` from `example.LogSummary`. The application
imports it through the package identity in `workspace.json`. The module provider
includes the same implementation and disables its standalone `main`.
Inspect `build/build-report.json` for the resolved package and compilation stages.

Run `sh verify.sh` in the full example directory to check good, missing, and
malformed inputs at O0 and O2 and run the package build. For other workflow
examples, see the [showcase catalog](../../showcase/README.md).
