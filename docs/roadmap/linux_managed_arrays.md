# Linux managed carrier arrays

Extension ABI 9 adds fixed-size local arrays of managed Optional/Result values
on Linux x86-64 with ARC and GNUstep 2.2. Multidimensional arrays, omitted
zero-initialized elements, const arrays, nested carriers, Result arm changes and
copies or moves of individual elements use the same active-arm ownership
operations as standalone values. Destruction runs in reverse element order.
A failing element initializer destroys only the completed prefix, including
nested dimensions. Native exceptions and typed-error propagation follow those
cleanup paths.

Initialization now owns a completed carrier before surrounding initializer
temporaries are destroyed. If a C++ temporary destructor throws after the payload
and tag have been initialized, the completed value is destroyed exactly once.
The same rule covers direct C++ payloads, array initialization and return
construction. Failed return storage is cleared only after normal initialization
and lexical cleanups finish. C jumps and switch labels cannot bypass managed
initialization.

Block payloads now preserve native strong, zeroing weak and explicit unsafe
ownership. Weak carrier copies and moves use native weak registration
operations; unsafe carriers retain no ownership. Runtime encodings distinguish
all three conventions, including nested values and closure helper identities.
Global Blocks retain their native permanent lifetime. Heap Block captures can
hold weak carriers after their factory returns and observe zeroing when the
last strong owner releases the captured Block.

An array declaration requires an initializer and constant dimensions. Variable
length arrays, `__block` arrays, enclosing-record array subobjects and whole-array
capture or copying require further lifetime operations. Their admission remains
separate from local array construction and element operations. The public
Optional/Result and synchronous throws feature predicates remain reserved.

The installed `qualify-array-carriers.py` suite exercises separate providers and
consumers in C17 and C++20 at O0, O2 and ThinLTO, plus PCH and semantic imports.
It checks normal and partial reverse destruction, strong and weak elements,
nested and multidimensional values, initializer temporary failures, native
exceptions, typed errors, weak/unsafe Block carriers, heap captures and metadata.
Six forced failures and six rejected storage cases supplement nine successful
executions. Every successful executable receives DWARF verification.

```sh
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-array-carriers.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./array-carrier-evidence
```

The next storage milestone applies these operations to enclosing records and
their implicit copy, assignment and destruction, then Objective-C ivars and
properties and reference or `__block` captures. Public syntax and feature
promotion follow a bounded, fully qualified synchronous contract.
