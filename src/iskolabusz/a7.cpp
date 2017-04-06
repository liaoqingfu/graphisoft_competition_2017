#import <tgmath.h>

main() {
    int n;
    scanf("%d",&n);
    printf("%d", n > 1 ?  2*n - (2<<ilogb(n-1))  : 1);
}
