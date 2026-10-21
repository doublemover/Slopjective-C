@import CxxData;
int main(void) {
  {
    id object=record_object(90);
    @CxxData.CxxData value{40,Some(id,object),Some(id __weak,object),
                  {Some(id,record_object(91))}};
    object=(id)0;
    CxxData c=c_data_echo(value), cpp=@CxxData.cpp_data_echo(c);
    if (c.marker!=41 || cpp.marker!=42) return 1;
    if (data_overload(1)!=101 || @CxxData.data_overload(2L)!=202 || data_mode()!=DataMode::Copy) return 2;
    value.value=None(id); c.value=None(id);
    if (record_destroyed(90)) return 3;
    cpp.value=None(id);
    if (record_destroyed(90)!=1) return 4;
    match(value.weak) {
      case .Some(let payload): { if (payload!=(id)0) return 5; }
      case .None: { return 6; }
    }
    value.values[0]=None(id); c.values[0]=None(id);
    if (record_destroyed(91)) return 7;
    cpp.values[0]=None(id); if (record_destroyed(91)!=1) return 8;
  }
  return record_destroyed(90)==1 && record_destroyed(91)==1 ? 0 : 9;
}
