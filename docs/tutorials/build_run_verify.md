# Build, run and verify

Complete [Getting started](getting_started.md) and keep the SDK activated.
In the same working directory, compile your first program again with debug
information and a Makefile dependency record:

```sh
objc3c hello.objc3 -O0 -g -MMD -MF hello.d -o hello
./hello
cat hello.d
```

The program prints the same two lines. `hello.d` lists the source and any
project headers used to build `hello`. Use `objc3c --help` for driver options.
Use `-fsyntax-only` to check the source without linking or `-c` to stop at an
object file.

If the compiler command is missing, source the installation's `activate.sh`
in the current shell. If the installer reports a missing prerequisite,
see the provisioning instructions in the [release guide](../releases/linux.md).

Continue with the [log summary example](log_summary.md) or the
[installed module and package walkthrough](guided_walkthrough.md).
