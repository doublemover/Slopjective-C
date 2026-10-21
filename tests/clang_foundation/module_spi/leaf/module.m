#define SPI(group) __attribute__((annotate("objc3.spi:" #group)))
int spi_hidden(void) { return 1; }
int spi_public(void) { return 20; }
SPI(Testing) int spi_secret(void) { return 22 + spi_hidden() - spi_hidden(); }
int spi_global SPI(Testing) = 22;
struct SPI(Testing) SPIValue { int value; };
typedef int SPIAlias SPI(Testing);
enum SPI(Testing) SPIChoice { spi_choice = 22 };
SPI(Other) int spi_other(void) { return -1; }
#ifdef __OBJC__
SPI(Testing) __attribute__((objc_root_class)) @interface SPIRoot
- (int)onlySecret;
@property(readonly) int secretProperty;
@end
@implementation SPIRoot
- (int)onlySecret { return 22; }
- (int)secretProperty { return 22; }
@end
SPI(Testing) @protocol SPIProtocol @end
#endif
