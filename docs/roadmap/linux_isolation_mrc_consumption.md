# Manual ownership at isolated and lazy native entries

This source span extends awaited isolation to MRC consumed object and Block
parameters, consuming instance receivers, and synchronous init-family methods.
It also supplies the ownership stages required by consumed MRC native-frame
parameters and receivers. C++ task-ABI MRC consumption, CF ownership and async
initializers remain open. Extension ABI 22 and the runtime frame layout are
unchanged by these lowering changes.

An MRC caller supplies one existing +1 for each consumed operand. The generated
ramp adopts that claim with normal/EH cleanup before frame allocation or copying.
Each lazy frame acquires its own lifetime pin. Ramp exit balances the original
claim; cancellation or destruction before body entry releases only the frame pin.
After cancellation and executor-entry checks, the manual body receives a distinct
+1 that it balances according to its ordinary consumed-parameter contract. A
synchronous target receives this claim when invoked by its entry thunk. A native
async target receives it when its own lazy body begins. Neither cancellation nor
failed allocation invokes a manual body to dispose an unentered claim.

ARC native-frame consuming receivers retain the normal adopting parameter ABI.
Their lazy copy uses a strong local slot and destroys its current contents, so
reassigning consumed self neither leaks the replacement nor releases the original
snapshot twice. Nonconsuming receivers keep their independent precise pin.

Synchronous initializer sends preserve the actual send's `instancetype`, rather
than the declaration's erased `id`. Receiver and result ownership remain separate:
an initializer can return its receiver, null, or a different retained object.
The result stages its own owner before receiver cleanup. Under MRC, an await
result keeps the established +0 convention; a caller retaining it does so within
the full-expression that owns the temporary result.

`native-isolation-consumption.m` compiles the common source and PCH bodies under
C17/C++23 and ARC/MRC. The frontend suite checks concrete initializer result types
and consuming signatures. The maintained runtime driver always compiles
`mrc-consuming.m` without ARC and `consuming.m` with ARC, independently of client
and provider modes. Exact entry, argument and destruction counts defend success,
cancellation before entry, cancellation between lazy frames, and allocation
failure in either frame. Block and combined receiver/argument transfers exercise
independent owners. The initializer cases cover self, nil and replacement
results, and an ARC receiver case reassigns self.

These source witnesses do not establish SDK acceptance. The completion ledger
records current execution evidence; final source/PCH/package and instrumented
matrices accompany the coordinated SDK delivery.
