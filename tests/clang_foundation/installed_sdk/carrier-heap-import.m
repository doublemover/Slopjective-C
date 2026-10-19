#ifdef __cplusplus
@import HeapStorageCPP;
#else
@import HeapStorageC;
#endif
int main(void) {
  StorageBox *box=[StorageBox new];
  box.strong=Some(id,record_object(50));
  RecordStrong copy=box.strong; box.strong=None(id);
  if(record_destroyed(50)) return 1;
  copy=None(id); if(record_destroyed(50)!=1) return 2;
  StorageState state=heap_capture_shared_record(51);
  if(!state(0) || record_destroyed(51)) return 3;
  if(state(1) || record_destroyed(51)!=1) return 4;
  struct RecordPair original={42,Some(id,record_object(52)),None(id __weak),{}};
  box.pair=original;
  struct RecordPair echoed=[box echo:original];
  if(echoed.marker!=43) return 5;
  HeapRecordCall block=heap_record_block();
  struct RecordPair mapped=heap_invoke_block(block,original);
  return mapped.marker!=43;
}
