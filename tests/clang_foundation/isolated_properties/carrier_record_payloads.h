#ifndef OBJC3_COMPOSED_CPP_CARRIER_PAYLOADS_H
#define OBJC3_COMPOSED_CPP_CARRIER_PAYLOADS_H
#ifdef __cplusplus
static int carrier_record_live,carrier_record_created,carrier_record_destroyed;
static int carrier_record_copy_attempt,carrier_record_move_attempt;
static int carrier_record_fail_copy,carrier_record_fail_move;
static int carrier_record_temp_attempt,carrier_record_fail_temp;
struct CarrierRecordCopyParameter {
  ~CarrierRecordCopyParameter() noexcept(false) {
    if(++carrier_record_temp_attempt==carrier_record_fail_temp)throw 734;
  }
};
struct CarrierRecordLeaf {
  int value;
  explicit CarrierRecordLeaf(int value,int fail):value(value) {
    if(value==fail)throw 731;
    ++carrier_record_live;++carrier_record_created;
  }
  CarrierRecordLeaf(const CarrierRecordLeaf &other,CarrierRecordCopyParameter parameter={}):value(other.value) {
    if(++carrier_record_copy_attempt==carrier_record_fail_copy)throw 732;
    ++carrier_record_live;++carrier_record_created;
  }
  CarrierRecordLeaf(CarrierRecordLeaf &&other):value(other.value) {
    if(++carrier_record_move_attempt==carrier_record_fail_move)throw 733;
    other.value=-1;++carrier_record_live;++carrier_record_created;
  }
  ~CarrierRecordLeaf() noexcept {--carrier_record_live;++carrier_record_destroyed;assert(carrier_record_live>=0);}
};
using CarrierRecordMaybe=__objc3_optional(CarrierRecordLeaf);
static CarrierRecordMaybe carrier_record_element(int value,int fail) {
  return __objc3_some(CarrierRecordLeaf,CarrierRecordLeaf(value,fail));
}
struct CarrierRecordScalar {
  CarrierRecordMaybe first,second;
  explicit CarrierRecordScalar(int fail):first(carrier_record_element(1,fail)),second(carrier_record_element(2,fail)) {}
  CarrierRecordScalar(const CarrierRecordScalar &)=default;
  CarrierRecordScalar(CarrierRecordScalar &&)=default;
  ~CarrierRecordScalar() noexcept=default;
};
struct CarrierRecordArray {
  CarrierRecordMaybe values[2][2];
  explicit CarrierRecordArray(int fail):values{{carrier_record_element(1,fail),carrier_record_element(2,fail)},
      {carrier_record_element(3,fail),carrier_record_element(4,fail)}} {}
  CarrierRecordArray(const CarrierRecordArray &)=default;
  CarrierRecordArray(CarrierRecordArray &&)=default;
  ~CarrierRecordArray() noexcept=default;
};
using CarrierRecordOuter=__objc3_optional(CarrierRecordScalar);
using CarrierArrayOuter=__objc3_optional(CarrierRecordArray);
static CarrierRecordOuter carrier_record_outer(bool some,int fail) {
  if(some)return __objc3_some(CarrierRecordScalar,CarrierRecordScalar(fail));
  return __objc3_none(CarrierRecordScalar);
}
static CarrierArrayOuter carrier_array_outer(bool some,int fail) {
  if(some)return __objc3_some(CarrierRecordArray,CarrierRecordArray(fail));
  return __objc3_none(CarrierRecordArray);
}
static void carrier_record_assert_clean() {
  assert(!carrier_record_live && carrier_record_created==carrier_record_destroyed);
}
static void carrier_record_payload_suite() {
  for(int some=0;some!=2;++some) {
    {
      auto scalar=carrier_record_outer(some,0);auto array=carrier_array_outer(some,0);
      assert(__objc3_has_value(scalar)==some && __objc3_has_value(array)==some);
      auto copied=__objc3_copy(array);auto moved=__objc3_move(copied);
      assert(__objc3_has_value(moved)==some);
    }
    carrier_record_assert_clean();
  }
  for(int route=0;route!=4;++route)for(int fail=0;fail!=5;++fail) {
    bool caught=false;carrier_record_copy_attempt=carrier_record_move_attempt=0;
    try {
      if(route==0){CarrierRecordArray value(fail);assert(__objc3_has_value(value.values[1][1]));}
      else if(route==1){auto value=carrier_array_outer(true,fail);assert(__objc3_has_value(value));}
      else {
        CarrierRecordArray source(0);
        if(route==2) {
          carrier_record_fail_copy=fail;
          CarrierRecordArray destination(source);
          assert(__objc3_has_value(destination.values[1][1]));
        }else {
          carrier_record_fail_move=fail;
          CarrierRecordArray destination(static_cast<CarrierRecordArray &&>(source));
          assert(__objc3_has_value(destination.values[1][1]));
        }
      }
    }catch(int error){assert(error==(route<2?731:route==2?732:733));caught=true;}
    carrier_record_fail_copy=carrier_record_fail_move=0;
    assert(caught==bool(fail));carrier_record_assert_clean();
  }
  for(int fail=1;fail!=5;++fail) {
    bool caught=false;carrier_record_temp_attempt=0;
    try {
      CarrierRecordArray source(0);carrier_record_fail_temp=fail;
      CarrierRecordArray destination(source);
    }catch(int error){assert(error==734);caught=true;}
    carrier_record_fail_temp=0;assert(caught);carrier_record_assert_clean();
  }
}
#else
static void carrier_record_payload_suite() {}
#endif
#endif
