# Native formatting, iterator traits and instantiated methods

Concurrency source 2.114 extends the standard-library publication span to
inherited formatter state and iterator-trait customizations. The interface
continues to require the checked native-header and standard-specialization
capabilities. Extension ABI 24 and the accepted 2.62 SDK remain unchanged.

An instantiated method with an `auto` return retains the identity of its mapped
template pattern in the same class specialization. Different deduction state in
the producer and consumer cannot create a second overload. Borrowed-result and
isolation signatures still have to agree. The standard-library formatting case
defends this relationship through the native sink iterator's reservation result.

Importing a virtual method body fills the existing in-class declaration. It maps
the defining function and its parameters before importing the body, preserves
the imported signature and transfers destructor deallocation information. The
record keeps one declaration per virtual slot. A focused AST test checks C++20/23
and full/minimal imports, native vtable construction, exact parameter references
and slot uniqueness. All fifteen AST infrastructure tests pass. The new test
reproduces duplicate slots and a vtable assertion with the previous importer.

Constant-expression import preserves reserved storage without inventing a cached
value and retains the immediate-invocation flag. Reading an unevaluated integer
slot previously manufactured a zero-width APInt during libc++ ARC code generation.
The AST test covers absent results under all storage kinds, immediate calls and
computed signed/unsigned 32-, 64- and 128-bit integers. The failing ARC formatting
consumer now compiles and both source-free native consumers execute correctly.

The maintained native witness publishes `std::formatter<Key, char>` derived from
the native integer formatter and `std::iterator_traits<Cursor>`. It removes the
provider source, imports into two consumers, checks formatting specifications,
forward-iterator concepts, reference/value traits, accumulation, distance and
advancement, then links and checks the common inline function address. The two
profiles are C++20 plain O0 and Objective-C++23 ARC O2 with ASan/UBSan. Both
libstdc++ and libc++ native profiles pass: four complete instrumented
provider/two-consumer link/execute configurations. All twelve affected frontend
groups and the focused ten-command source/PCH import controls also pass.

`objc3_standard_library_customization_provider` is an extended CTest entry.
Setting `OBJC3_TEST_LIBCXX_ROOT` also registers
`objc3_standard_library_libcxx_customizations`. Both native standard-library
witnesses share the same request, instrumentation, linker and execution helpers.
Broader source-free package/profile matrices and the coordinated SDK delivery
remain in consolidation.
