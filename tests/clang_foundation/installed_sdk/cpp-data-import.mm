@import CxxData;
int main(void) {
  {
    id object=record_object(90);
    @CxxData.CxxData value{40,__objc3_some(id,object),__objc3_some(id __weak,object),
                  {__objc3_some(id,record_object(91))}};
    object=(id)0;
    CxxData c=c_data_echo(value), cpp=@CxxData.cpp_data_echo(c);
    if (c.marker!=41 || cpp.marker!=42) return 1;
    if (data_overload(1)!=101 || @CxxData.data_overload(2L)!=202 || data_mode()!=DataMode::Copy) return 2;
    value.value=__objc3_none(id); c.value=__objc3_none(id);
    if (record_destroyed(90)) return 3;
    cpp.value=__objc3_none(id);
    if (record_destroyed(90)!=1) return 4;
    __objc3_match_stmt(value.weak) {
      case __objc3_case_some(let payload): { if (payload!=(id)0) return 5; }
      case __objc3_case_none: { return 6; }
    }
    value.values[0]=__objc3_none(id); c.values[0]=__objc3_none(id);
    if (record_destroyed(91)) return 7;
    cpp.values[0]=__objc3_none(id); if (record_destroyed(91)!=1) return 8;
  }
  return record_destroyed(90)==1 && record_destroyed(91)==1 ? 0 : 9;
}
