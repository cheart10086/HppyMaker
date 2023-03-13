#include<stdio.h>

const int MAXN = 10000 ;



int main() {
    int n, x, y;
    int a[MAXN], b[MAXN], g[MAXN], k[MAXN];
    scanf("%d", &n);
    for(int i = 0; i < n; i++) {
        scanf("%d%d%d%d", &a[i], &b[i], &g[i], &k[i]);//输入
    }
    scanf("%d%d", &x, &y);
    
    int ans = -1;
    for(int i = 0; i < n; i++) {
        if(x >= a[i] && y >= b[i] && x <= a[i] + g[i] && y <= b[i] + k[i]) {
            ans = i + 1;
        }
    }
    
    printf("%d\n", ans);
    
    return 0;
}