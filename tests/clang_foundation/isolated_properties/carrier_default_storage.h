#ifndef OBJC3_CPP_CARRIER_DEFAULT_STORAGE_H
#define OBJC3_CPP_CARRIER_DEFAULT_STORAGE_H
#ifdef __cplusplus
struct CarrierDefaultFields {
  CarrierCallOptional value{};
  CarrierCallOptional matrix[2][2]{};
  CarrierDefaultFields()=default;
  CarrierDefaultFields(const CarrierDefaultFields &)=default;
  CarrierDefaultFields(CarrierDefaultFields &&)=default;
  ~CarrierDefaultFields() noexcept=default;
};
static int carrier_default_bound_evaluations;
static int carrier_default_bound(int count) {++carrier_default_bound_evaluations;return count;}
static void carrier_default_storage_suite(PropertyMain *object) {
 @autoreleasepool {
  checked_property_error=object;
  carrier_call_stage=8000;
  {
    CarrierCallOptional empty{};
    CarrierCallOptional other=CarrierCallOptional();
    assert(!carrier_call_live);
    CarrierDefaultFields fields,copy(fields),moved(static_cast<CarrierDefaultFields &&>(copy));
    assert(!carrier_call_live);
    assert(!__objc3_has_value(empty) && !__objc3_has_value(other));
    assert(!__objc3_has_value(fields.value) && !__objc3_has_value(moved.matrix[1][1]));
    CarrierCallOptional matrix[2][2]={{carrier_call_optional(true)}};
    assert(carrier_call_live==1);
    assert(__objc3_has_value(matrix[0][0]) && !__objc3_has_value(matrix[0][1]) &&
        !__objc3_has_value(matrix[1][0]) && !__objc3_has_value(matrix[1][1]));
  }
  carrier_call_assert_clean();
  for(int count:{0,1,3})for(int route=0;route!=3;++route) {
    carrier_call_stage=8100+count*10+route;
    int before=carrier_default_bound_evaluations;
    if(route==2) {
      auto *values=new CarrierCallOptional[carrier_default_bound(count)][2]{};
      for(int i=0;i<count;++i)for(int j=0;j<2;++j)assert(!__objc3_has_value(values[i][j]));
      delete[] values;
    }else {
      auto *values=route==0?new CarrierCallOptional[carrier_default_bound(count)]():
          new CarrierCallOptional[carrier_default_bound(count)]{};
      for(int i=0;i<count;++i)assert(!__objc3_has_value(values[i]));
      delete[] values;
    }
    assert(carrier_default_bound_evaluations==before+1);
    carrier_call_assert_clean();
  }
  for(int count:{2,3,5})for(int route=0;route!=3;++route)for(int fail=0;fail!=2;++fail) {
    carrier_call_stage=9000+count*100+route*10+fail;
    bool caught=false;int before=carrier_default_bound_evaluations;
    try {
      if(route==0) {
        auto *values=new CarrierCallOptional[carrier_default_bound(count)]{
            carrier_call_optional(true),carrier_call_maybe_fault(fail)};
        assert(__objc3_has_value(values[0]) && __objc3_has_value(values[1]));
        for(int i=2;i<count;++i)assert(!__objc3_has_value(values[i]));
        delete[] values;
      }else if(route==1) {
        auto *values=new CarrierCallOptional[carrier_default_bound(count)][2]{
            {carrier_call_optional(true),__objc3_none(CarrierCallOwner)},
            {carrier_call_maybe_fault(fail)}};
        assert(__objc3_has_value(values[0][0]) && !__objc3_has_value(values[0][1]) &&
            __objc3_has_value(values[1][0]) && !__objc3_has_value(values[1][1]));
        for(int i=2;i<count;++i)for(int j=0;j<2;++j)assert(!__objc3_has_value(values[i][j]));
        delete[] values;
      }else {
        auto *values=new CarrierCallOptional[carrier_default_bound(count)](
            carrier_call_optional(true),carrier_call_maybe_fault(fail));
        assert(__objc3_has_value(values[0]) && __objc3_has_value(values[1]));
        for(int i=2;i<count;++i)assert(!__objc3_has_value(values[i]));
        delete[] values;
      }
    }catch(int error){assert(fail && error==726);caught=true;}
    assert(caught==bool(fail) && carrier_default_bound_evaluations==before+1);
    carrier_call_assert_clean();
  }
  {
    carrier_call_stage=10000;
    alignas(CarrierCallOptional) unsigned char memory[sizeof(CarrierCallOptional)*3];
    auto *value=new (memory) CarrierCallOptional{};
    assert(!__objc3_has_value(*value));value->~CarrierCallOptional();
    auto *values=new (memory) CarrierCallOptional[3]{carrier_call_optional(true)};
    assert(__objc3_has_value(values[0]) && !__objc3_has_value(values[2]));
    for(int i=3;i!=0;--i)values[i-1].~CarrierCallOptional();
  }
  carrier_call_assert_clean();
  for(int count:{-1,0,1}) {
    carrier_call_stage=11000+count;
    int entries=carrier_call_entries,before=carrier_default_bound_evaluations;bool caught=false;
    try {
      auto *values=new CarrierCallOptional[carrier_default_bound(count)]{
          carrier_call_optional(true),carrier_call_optional(true)};
      delete[] values;assert(!"short allocation bound accepted");
    }catch(const std::bad_array_new_length &){caught=true;}
    assert(caught && carrier_call_entries==entries && carrier_default_bound_evaluations==before+1);
    carrier_call_assert_clean();
  }
  for(int count:{-1,0,1}) {
    int entries=carrier_call_entries,before=carrier_default_bound_evaluations;
    auto *values=new (std::nothrow) CarrierCallOptional[carrier_default_bound(count)]{
        carrier_call_optional(true),carrier_call_optional(true)};
    assert(!values && carrier_call_entries==entries && carrier_default_bound_evaluations==before+1);
    carrier_call_assert_clean();
  }
  for(int route=0;route!=2;++route)for(int position=0;position!=4;++position)for(int mode=2;mode!=6;++mode) {
    carrier_call_stage=12000+route*1000+position*10+mode;
    bool typed=false,foreign=false;
    try {
      __objc3_do {
        auto element=[&](int index) __objc3_throws(id<Error>) {
          return __objc3_try(carrier_call_checked_result(index==position?mode:0));
        };
        if(route==0) {
          int count=5;
          auto *values=new CarrierCallOptional[count]{__objc3_try(element(0)),__objc3_try(element(1)),
              __objc3_try(element(2)),__objc3_try(element(3))};
          delete[] values;
        }else {
          int count=3;
          auto *values=new CarrierCallOptional[count][2]{{__objc3_try(element(0)),__objc3_try(element(1))},
              {__objc3_try(element(2)),__objc3_try(element(3))}};
          delete[] values;
        }
        assert(!"checked initializer fault returned an allocation");
      }__objc3_catch(id<Error> error){assert(mode<5 && error==checked_property_error);typed=true;}
    }catch(int error){assert(mode==5 && error==727);foreign=true;}
    assert(typed==(mode<5) && foreign==(mode==5));carrier_call_assert_clean();
  }
  checked_property_error=(id)0;
 }

}
#else
static void carrier_default_storage_suite(PropertyMain *object) {(void)object;}
#endif
#endif
