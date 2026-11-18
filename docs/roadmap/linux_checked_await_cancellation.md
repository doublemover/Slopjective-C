# Checked await cancellation

Source 2.243 / System 2.169 admits the public `try`, `try?` and `try!` prefix
spellings. The parser binds the operand at cast-expression precedence and uses
the existing typed propagation AST and canonical interface spellings. A C
declaration named `try` keeps ordinary lookup and parsing. Objective-C++ keeps
native try-blocks and function-try-blocks, including native rethrow.

`try? await` now handles native child cancellation and a parent cancellation
request observed at the await. The frame lowering leaves the await through its
normal cleanup edge before creating `None`. It does not attempt result copy-out
on cancellation. C++ task coroutines carry the optionalization policy in the
compiler-owned checked dependency adapter and use the distinguished cancellation
value for the failure branch. A known request skips starting another child.

The plain propagation path retains cancelled task completion. An exported C
completion handler receives the corresponding distinguished Objective-C error.
Full throwing-join and profile acceptance still require their remaining controls.

The 96 source/PCH commands cover C17, C23, C++20 and C++23, both ownership modes
and all four profiles. Eight source-free C client programs exercise 48 native
frame and 24 private C++ task cancellation cases. They check empty Optional
results, no result payload on cancelled propagation, one parent defer,
and exactly one destruction for each child that was constructed. The previous native frame lowering
fails the new optionalization control. Fresh patch application reproduces the
maintained parser, type adapter, Sema and code generation changes.
