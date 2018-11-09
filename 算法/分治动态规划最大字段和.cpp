#include <iostream>
#define M 100
using namespace std;

int a[M] = {-1, 11, -4, 13, -5, -2};
int n = 6;

int maxSubSum(int a[], int left, int right) {
    if (left == right) {
        return a[left] > 0 ? a[left] : 0;
    } else {
        int mid = (left + right) / 2;
        int leftSum = maxSubSum(a, left, mid);
        int rightSUm = maxSubSum(a, mid  + 1, right);
        int s1 = 0, lefts = 0;
        for (int i = mid; i >= left; i--) {
            lefts += a[i];
            if (lefts > s1) {
                s1 = lefts;
            }
        } 
        int s2 = 0, rights = 0;
        for (int i = mid + 1; i <= right; i++) {
            rights += a[i];
            if (rights > s2) {
                s2 = rights;
            }
        }
        int sum = s1 + s2;
        if (sum < leftSum) sum = leftSum;
        if (sum < rightSUm) sum = rightSUm;
        return sum;
    }
}

int maxSubSum(int a[], int n) {
    int b = 0, max = 0;
    for (int i = 0; i < n; i++) {
        if (b > 0) {
            b += a[i];
        } else {
            b = a[i];
        }
        if (b > max) {
            max = b;
        }
    }
    return max;
}


int main() {
    int sum = maxSubSum(a, 0, 5);
    cout << sum << endl;
    sum = maxSubSum(a, 6);
    cout << sum << endl;
    return 0;
}
