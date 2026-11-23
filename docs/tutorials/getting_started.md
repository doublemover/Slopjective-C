# Getting started on Linux

[Install the published SDK](../releases/linux.md) for your architecture. The
preview supports x86-64 and AArch64 on the distributions in its
[support table](../releases/preview-support.md).

For a user installation, activate the SDK and check the compiler version:

```sh
. "$HOME/.local/objc3c/activate.sh"
objc3c --version
```

For a system installation, source `/opt/objc3c/activate.sh` instead. In an empty
working directory, create and run an Objective-C 3 program:

```sh
cat > hello.objc3 <<'SRC'
#include <stdio.h>

int main(void) {
  defer { puts("Done."); }
  puts("Hello from Objective-C 3.0.");
  return 0;
}
SRC
objc3c hello.objc3 -O2 -o hello
./hello
```

```text
Hello from Objective-C 3.0.
Done.
```

The `defer` block executes when `main` exits. The `.objc3` extension selects
Objective-C 3 with ARC and Blocks enabled.

Try the [log summary example](log_summary.md) next. It combines `defer` with
`Optional`, `Result`, and typed `throws`. The
[capability schema](../../schemas/objc3-language-capabilities.json) records
qualified features and modes. Public async/await and actors remain reserved.

For the compiler checkout and build, follow the
[maintainer workflow](../runbooks/objc3c_maintainer_workflows.md).
