@import fixture.NativeAsync;
int rejected_factory_templates() {
  using namespace NativeFactoryTemplateArguments;
  auto callable=read;
  auto storage=slot;
  auto record=Box;
  return 0;
}

void reject_native_contracts() {
  NativeDeclarationAttributes::checked(-1);
  NativeDeclarationAttributes::checked_template(-1);
  NativeDeclarationAttributes::provider_sentinel("missing sentinel",42);
  auto forbidden_address=&NativeDeclarationAttributes::provider_object_size;
}
