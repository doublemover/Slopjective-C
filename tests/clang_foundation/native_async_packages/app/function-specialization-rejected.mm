@import fixture.NativeAsync;
// A specialization is selected through its primary, not a separate overload.
auto invalid_specialization_address = &native_spec_add;

// Variable specializations also remain inside primary-template lookup.
auto invalid_variable_address = &native_spec_shared;
