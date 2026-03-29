__attribute__((objc_root_class))
@interface BenchWidget {
    int _count;
}
+ (id)new;
@property (nonatomic, assign) int count;
- (int)score;
@end

@implementation BenchWidget
@synthesize count = _count;
+ (id)new {
    return (id)0;
}
- (int)score {
    int total = self.count;
    for (int index = 0; index < 7; index++) {
        total += index;
    }
    return total;
}
@end

int main(void) {
    BenchWidget *widget = [BenchWidget new];
    (void)widget;
    return 7 % 31;
}
