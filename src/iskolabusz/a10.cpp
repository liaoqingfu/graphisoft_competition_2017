#import <uv.h>

main() {
    int n,v=scanf("%d",&n);
    while (v < n) v *= 2;
    printf("%d", 2 * n - v);
}
