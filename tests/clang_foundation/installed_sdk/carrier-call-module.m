// A public declaration surface for the separately compiled provider.
typedef Optional(id) Strong;
typedef Optional(id __weak) Weak;
id call_object(int key) __attribute__((ns_returns_retained));
int call_destroyed(int key);
Strong call_make(int key);
Strong call_identity(Strong value);
Weak call_weak(Weak value);
