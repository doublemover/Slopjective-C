@import RecordCarriers;
int main(void) {
  {
    struct RecordCall original=record_make(50), copy=record_echo(original);
    original.value=__objc3_none(id);
    if (record_destroyed(50)) return 1;
    copy.value=__objc3_none(id);
    if (record_destroyed(50)!=1 || copy.marker!=42) return 2;
  }
  struct RecordPair values[2] = {
    {0,__objc3_some(id,record_object(51)),__objc3_none(id __weak),{}},
    {1,__objc3_some(id,record_object(52)),__objc3_none(id __weak),{}}
  };
  return !__objc3_has_value(values[1].first);
}
