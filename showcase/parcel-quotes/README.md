# Parcel quotes from a source-free SDK package

`Parcel.Quotes` returns an Objective-C 3 `Result` with a native C++ price record
or a weight error enum. The Objective-C++ consumer passes a parcel batch through
`std::span` and formats approved quotes and rejected shipments using C++20.

## Build and run

From the repository root, use a verified Linux preview SDK and a fresh output
directory. The [SDK setup guide](../../docs/tutorials/getting_started.md) sets
`OBJC3C_SDK_ROOT`.

```sh
sh showcase/parcel-quotes/run.sh "$OBJC3C_SDK_ROOT" ./parcel-quote-output
```

```text
Books (1250 g, standard): $8.00, 5 business days
Glassware (750 g, express): $10.50, 2 business days
Empty parcel (0 g, standard): rejected, weight must be positive
Oversized carton (22000 g, express): rejected, weight exceeds 20 kg
```

The runner builds the [producer workspace](producer/workspace.json) with
`objc3c-build`. It publishes and installs the provider object and both files
of the semantic interface with `objc3c-package`. The consumer uses the
installed package's content SHA-256 and its semantic fingerprint when
importing `Parcel.Quotes`.

The runner removes its staged producer sources and build outputs before
`objc3c-package consume` compiles the [separate client](consumer/main.mm).
`consumer-build/consumer-report.json` records the package and SDK identities,
the compiler and linker commands, and `provider_compilations: 0`.

Run the application again with
`./parcel-quote-output/consumer-build/application.bin`. To quote a different
batch, edit `consumer/main.mm` and run the example with a new output directory.
[Immutable package commands](../../docs/roadmap/linux_immutable_packages.md)
