#ifndef OBJC3_COMPOSED_CPP_CARRIER_ALLOCATIONS_H
#define OBJC3_COMPOSED_CPP_CARRIER_ALLOCATIONS_H
#ifdef __cplusplus
static int carrier_dynamic_attempts,carrier_dynamic_fail;
struct CarrierDynamicRecord : CarrierRecordArray {
  CarrierDynamicRecord():CarrierRecordArray(0) {
    if(++carrier_dynamic_attempts==carrier_dynamic_fail)throw 735;
  }
  ~CarrierDynamicRecord() noexcept=default;
};
static void carrier_composed_allocation_suite() {
  for(int count:{0,1,3})for(int fail=0;fail<=count;++fail) {
    bool caught=false;carrier_dynamic_attempts=0;carrier_dynamic_fail=fail;
    try {
      auto *values=new CarrierDynamicRecord[count];
      assert(carrier_record_live==count*4);
      if(count)assert(__objc3_has_value(values[count-1].values[1][1]));
      delete[] values;
    }catch(int error){assert(error==735);caught=true;}
    assert(caught==bool(fail));carrier_record_assert_clean();
  }
  carrier_dynamic_fail=0;
}
#else
static void carrier_composed_allocation_suite() {}
#endif
#endif
