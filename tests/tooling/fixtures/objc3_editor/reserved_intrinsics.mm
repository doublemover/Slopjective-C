#define throws(value) (value)
typedef __objc3_optional( int ) MaybeInt;
@protocol Probe
- (int)async:(int)async throws:(int)state;
@end
int async(int value){return throws(value);}
void exercise(void){__objc3_some( int,0); __objc3_none( int );__objc3_has_value(__objc3_none( int ));}

