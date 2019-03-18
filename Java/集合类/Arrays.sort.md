### Arrays.sort
#### 一. int[]排序
* 数组元素个数小于287
    * 数组元素个数小于47
        * 排序区间是最左边的，传统的插入排序
        * 排序区间不是最左边的，双插入排序
    * 数组元素个数大于等于47
        * 选5个基准值
        * 如果5个基准值各不相同
            * 双轴快排
                * 递归排序左部分和右部分
                * 数组中间部分长度太大的话，先缩小一下范围
                * 递归排序中间部分
        * 5个基准值中有重复的
            * 单轴快排
* 数组元素个数大于等于287
    * 判断数组无序程度
        * 如果有序子序列个数大于等于67，此时可以认为数组基本无序，直接用上面的快排
        * 否则归并排序
```java
// Arrays
public static void sort(int[] a) {
    DualPivotQuicksort.sort(a, 0, a.length - 1, null, 0, 0);
}
```
这个方法进来后，先进行了一个判断，判断要排序的元素个数是否小于一个阈值(287)，如果小于，则特殊处理
```java
static void sort(int[] a, int left, int right, int[] work, int workBase, int workLen) {
    // Use Quicksort on small arrays
    if (right - left < QUICKSORT_THRESHOLD) { // 286
        sort(a, left, right, true);
        return;
    }

    ...

}
```
上面的`sort`方法进来之后，又做了一次判断，判断排序元素的个数是否小于47，如果小于47并且这段数组是最左边的就直接采用插入排序，在元素数量比较小的时候，插入排序会有更好的效率。
```java
private static void sort(int[] a, int left, int right, boolean leftmost) {
    int length = right - left + 1;

    // Use insertion sort on tiny arrays
    if (length < INSERTION_SORT_THRESHOLD) {  // 47
        if (leftmost) { // 这段数组是最左边的
            // 插入排序
            for (int i = left, j = i; i < right; j = ++i) {
                int ai = a[i + 1];
                while (ai < a[j]) {
                    a[j + 1] = a[j];
                    if (j-- == left) {
                        break;
                    }
                }
                a[j + 1] = ai;
            }
        } else {        // 这段数组不是最左边的，采用双插入排序，这个排序方法在左边有可能数组越界
            // 跳过最长的上升序列
            do {
                if (left >= right) {
                    return;
                }
            } while (a[++left] >= a[left - 1]);
            // 双插入排序，每次迭代的过程完成两个元素的插入
            for (int k = left; ++left <= right; k = ++left) {
                int a1 = a[k], a2 = a[left];
                // 保证a1 >= a2
                if (a1 < a2) {                  // 一次迭代完成两个元素的插入排序
                    a2 = a1; a1 = a[left];      // 0 1 2 3 4 5   下标值
                }                               // 4 7 8 9 6 5   6和5为a1和a2 
                // 找到a1的插入位置              // 4 7 6 7 8 9   找到6的位置，此时k = 1
                while (a1 < a[--k]) {           // 4 5 6 7 8 9   找到5的位置
                    a[k + 2] = a[k];
                }
                a[++k + 1] = a1;
                // 由于a2 <= a1，所以可以接着刚才的k继续向前遍历
                while (a2 < a[--k]) {
                    a[k + 1] = a[k];
                }
                a[k + 1] = a2;
            }
            int last = a[right];
            // 由于除去前面有序部分后面的元素个数为单数，还剩一个元素没插入
            while (last < a[--right]) {
                a[right + 1] = a[right];
            }
            a[right + 1] = last;
        }
        return;
    }

    // 长度/7 的近似值
    int seventh = (length >> 3) + (length >> 6) + 1;

    // 以seventh为间隔，在数组中找了5个基准值
    int e3 = (left + right) >>> 1; // The midpoint
    int e2 = e3 - seventh;
    int e1 = e2 - seventh;
    int e4 = e3 + seventh;
    int e5 = e4 + seventh;

    // 下面这4个if是用插入排序的方式对这5个基准值进行排序，只有5个元素，所以没有写成循环的形式
    if (a[e2] < a[e1]) { int t = a[e2]; a[e2] = a[e1]; a[e1] = t; }

    if (a[e3] < a[e2]) { int t = a[e3]; a[e3] = a[e2]; a[e2] = t;
        if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
    }
    if (a[e4] < a[e3]) { int t = a[e4]; a[e4] = a[e3]; a[e3] = t;
        if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
            if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
        }
    }
    if (a[e5] < a[e4]) { int t = a[e5]; a[e5] = a[e4]; a[e4] = t;
        if (t < a[e3]) { a[e4] = a[e3]; a[e3] = t;
            if (t < a[e2]) { a[e3] = a[e2]; a[e2] = t;
                if (t < a[e1]) { a[e2] = a[e1]; a[e1] = t; }
            }
        }
    }

    // Pointers
    int less  = left;  // The index of the first element of center part
    int great = right; // The index before the first element of right part

    // 如果这5个基准值都不相等
    if (a[e1] != a[e2] && a[e2] != a[e3] && a[e3] != a[e4] && a[e4] != a[e5]) {
        // 选择第2个和第4个作为双轴快排的两个基准值，这样数组分成了三部分
        int pivot1 = a[e2];
        int pivot2 = a[e4];
        // 开始快排
        // 将数组最左边的值放在第一个基准值的位置上，最右边的值放在第二个基准值的位置上
        a[e2] = a[left];
        a[e4] = a[right];

        while (a[++less] < pivot1); // 最左部分找到第一个不小于pivot1的值
        while (a[--great] > pivot2); // 最右部分找到第一个不大于pivot2的值

        /*
            * Partitioning:
            *
            *   left part           center part                   right part
            * +--------------------------------------------------------------+
            * |  < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2  |
            * +--------------------------------------------------------------+
            *               ^                          ^       ^
            *               |                          |       |
            *              less                        k     great
            *
            * Invariants:
            *
            *              all in (left, less)   < pivot1
            *    pivot1 <= all in [less, k)     <= pivot2
            *              all in (great, right) > pivot2
            *
            * Pointer k is the first index of ?-part.
            */
        // 这几个变量区间范围内的数组元素总会保持上面注释中的大小关系    
        outer:
        for (int k = less - 1; ++k <= great; ) {
            // 从less开始向后遍历，此时less是从左边开始第一个不小于prvot1的位置
            int ak = a[k];
            if (ak < pivot1) {
                // ak < pivot1时，ak需要被移动到左侧左侧部分
                // 交换a[less]和a[k]，并将less++
                // 交换后ak就被移到了左侧部分
                a[k] = a[less];
                a[less] = ak;
                ++less;
            } else if (ak > pivot2) { // Move a[k] to right part
                // ak > pivot2时，ak需要被移动到右侧部分
                while (a[great] > pivot2) {
                    if (great-- == k) {
                        break outer;
                    }
                }
                // a[great]需要移动到左侧
                if (a[great] < pivot1) { // a[great] <= pivot2
                    // 这部分操作相当于[less, k)区间最左侧的元素移到了区间右侧
                    // 然后less++，结果就是这个区间的元素整体右移
                    a[k] = a[less];
                    a[less] = a[great]; // a[great]移到左侧
                    ++less;  
                } else { // pivot1 <= a[great] <= pivot2
                    a[k] = a[great]; // a[great]加入[less, k)区间
                }
                // ak移动到右侧部分
                a[great] = ak;
                --great;
            }
        }

        // 将基准值放入对应的位置
        a[left]  = a[less  - 1]; a[less  - 1] = pivot1;
        a[right] = a[great + 1]; a[great + 1] = pivot2;
        // 现在数组分成三个区间
        // [left, less - 2] < pivot1
        // [less， great]   >= pivot1 && <= pivot2
        // [great + 2, right] > pivot2
        // 对最左和最右部分递归排序
        sort(a, left, less - 2, leftmost);
        sort(a, great + 2, right, false);

        // 如果中间部分范围太长，就缩小一下，将与基准值相等的全放到区间两边
        // 这样可以只排序中间 > pivot1 && < pivot2的
        // 缩小了[less, great]范围
        if (less < e1 && e5 < great) {
            // 跳过和两边的基准值相等的元素
            while (a[less] == pivot1) {
                ++less;
            }
            while (a[great] == pivot2) {
                --great;
            }

            /*
                * Partitioning:
                *
                *   left part         center part                  right part
                * +----------------------------------------------------------+
                * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
                * +----------------------------------------------------------+
                *              ^                        ^       ^
                *              |                        |       |
                *             less                      k     great
                *
                * Invariants:
                *
                *              all in (*,  less) == pivot1
                *     pivot1 < all in [less,  k)  < pivot2
                *              all in (great, *) == pivot2
                *
                * Pointer k is the first index of ?-part.
                */
            // 类似于上面的代码，只不过这个是将与两个基准值相等的元素全放到区间两端
            outer:
            for (int k = less - 1; ++k <= great; ) {
                int ak = a[k];
                if (ak == pivot1) { // Move a[k] to left part
                    a[k] = a[less];
                    a[less] = ak;
                    ++less;
                } else if (ak == pivot2) { // Move a[k] to right part
                    while (a[great] == pivot2) {
                        if (great-- == k) {
                            break outer;
                        }
                    }
                    if (a[great] == pivot1) { // a[great] < pivot2
                        a[k] = a[less];
                        a[less] = pivot1;
                        ++less;
                    } else { // pivot1 < a[great] < pivot2
                        a[k] = a[great];
                    }
                    a[great] = ak;
                    --great;
                }
            }
        }

        // 递归对中间部分排序
        sort(a, less, great, false);

    } else { // Partitioning with one pivot
    // 如果这5个基准值有重复的，就假定当前数组中有许多重复的元素，退化到传统单轴快排
        // 用数组中间位置的值作为基准值
        int pivot = a[e3];

        /*
            * Partitioning degenerates to the traditional 3-way
            * (or "Dutch National Flag") schema:
            *
            *   left part    center part              right part
            * +-------------------------------------------------+
            * |  < pivot  |   == pivot   |     ?    |  > pivot  |
            * +-------------------------------------------------+
            *              ^              ^        ^
            *              |              |        |
            *             less            k      great
            *
            * Invariants:
            *
            *   all in (left, less)   < pivot
            *   all in [less, k)     == pivot
            *   all in (great, right) > pivot
            *
            * Pointer k is the first index of ?-part.
            */
        // 下面的循环将将数组分成三部分
        // [left, less - 1] < pivot
        // [less, great] == pivot  (因为此时假定数组中有很多重复的元素)
        // [great + 1, right] > pivot
        for (int k = less; k <= great; ++k) {
            if (a[k] == pivot) {
                continue;
            }
            int ak = a[k];
            if (ak < pivot) { // Move a[k] to left part
                a[k] = a[less];
                a[less] = ak;
                ++less;
            } else { // a[k] > pivot - Move a[k] to right part
                while (a[great] > pivot) {
                    --great;
                }
                if (a[great] < pivot) { // a[great] <= pivot
                    a[k] = a[less];
                    a[less] = a[great];
                    ++less;
                } else { // a[great] == pivot
                    a[k] = pivot;
                }
                a[great] = ak;
                --great;
            }
        }
        // 只需对左右两部分递归排序
        sort(a, left, less - 1, leftmost);
        sort(a, great + 1, right, false);
    }
}
```
如果数组长度大于等于287，进行归并排序
```java
// 评估数组无序程度
// 数组run存放的是每个递增或递减的子序列的开始下标
int[] run = new int[MAX_RUN_COUNT + 1];
int count = 0; run[0] = left;

// Check if the array is nearly sorted
for (int k = left; k < right; run[count] = k) {
    // Equal items in the beginning of the sequence
    while (k < right && a[k] == a[k + 1])
        k++;
    if (k == right) break;  // Sequence finishes with equal items
    if (a[k] < a[k + 1]) { // 递增
        while (++k <= right && a[k - 1] <= a[k]);
    } else if (a[k] > a[k + 1]) { // 递减
        while (++k <= right && a[k - 1] >= a[k]);
        // 递减序列转换成递增序列
        for (int lo = run[count] - 1, hi = k; ++lo < --hi; ) {
            int t = a[lo]; a[lo] = a[hi]; a[hi] = t;
        }
    }
    // 一个递减序列转换成递增序列后是否可以合并到前一个递增序列
    if (run[count] > left && a[run[count]] >= a[run[count] - 1]) {
        count--;
    }
    // 子序列个数达到一定阈值就可以认为数组基本是无序的，直接采用上面改进过的快排排序
    if (++count == MAX_RUN_COUNT) {
        sort(a, left, right, true);
        return;
    }
}

// These invariants should hold true:
//    run[0] = 0
//    run[<last>] = right + 1; (terminator)

if (count == 0) {
    // 数组元素都相等
    return;
} else if (count == 1 && run[count] > right) {
    // 只有一个序列，数组已经有序
    return;
}
// 正常情况下，run[count] == right + 1，但也有例外
// 最后一段元素都是相等的，或者最后一个序列只有一个元素
// 此时，要将right+1添加到数组尾部，作为哨兵
right++;
if (run[count] < right) {
    run[++count] = right;
}

// 需要奇数轮merge时，odd = 0
// 需要偶数轮merge时，odd = 1
byte odd = 0;
for (int n = 1; (n <<= 1) < count; odd ^= 1);

// 创建临时数组b用于merge操作
int[] b;                 // temp array; alternates with a
int ao, bo;              // array offsets from 'left'
int blen = right - left; // space needed for b
if (work == null || workLen < blen || workBase + blen > work.length) {
    work = new int[blen];
    workBase = 0;
}
// 如果需要奇数轮，令b指向需要排序的原数组，a指向work
if (odd == 0) {
    System.arraycopy(a, left, work, workBase, blen);
    b = a;
    bo = 0;
    a = work;
    ao = workBase - left;
} else { // 偶数轮的话，a不动，还是指向需要排序的原数组，b指向work
    b = work;
    ao = 0;
    bo = workBase - left;
}
// 上面这个对a，b指向的处理是因为在下面每一轮merge结束后，会交换a，b的指向
// 要确保最后一轮merge中，元素被赋值的数组是需要排序的原数组，即需要b指向原数组
// Merging
for (int last; count > 1; count = last) {
    // 在循环中将两个相邻的递增子序列合并
    for (int k = (last = 0) + 2; k <= count; k += 2) { 
        // 两个子序列下标范围是[run[k - 2], run[k - 1])，[run[k - 1], run[k])
        int hi = run[k], mi = run[k - 1];              
        // 下面这个for合并了一对相邻的子序列
        for (int i = run[k - 2], p = i, q = mi; i < hi; ++i) { // [ , mi), [mi, hi)
            // p 第一个序列中的指针，q 第二个序列中的指针         //   p         q
            if (q >= hi || p < mi && a[p + ao] <= a[q + ao]) { // 左边的序列遍历完了 或 左边序列的元素比右边的小
                b[i + bo] = a[p++ + ao];                       // 取左边的元素  
            } else { // 否则取右边的元素
                b[i + bo] = a[q++ + ao];
            }
        }
        // 两个序列合并成一个有序序列后，更新序列起始下标
        run[++last] = hi;
    }
    // 如果子序列个数是奇数，则上面的合并完后还剩一个，直接copy到b中
    if ((count & 1) != 0) {
        for (int i = right, lo = run[count - 1]; --i >= lo;
            b[i + bo] = a[i + ao]
        );
        run[++last] = right;
    }
    // 交换数组a, b的指向
    int[] t = a; a = b; b = t;
    int o = ao; ao = bo; bo = o;
}
```
#### 二. Object[]排序
对于对象数组的排序，由于要保持排序的稳定性，所以与对基本类型数组的排序采用了不同的策略
* 数组元素个数小于32，采用二分插入排序
* 数组元素个数大于等于32，先分成一个个子序列，对这些子序列内部采用二分插入排序，然后对这些有序子序列进行归并      
插入排序与归并排序都是稳定的排序方式
```java
// Arrays
public static void sort(Object[] a) {
    if (LegacyMergeSort.userRequested)
        legacyMergeSort(a);
    else
        ComparableTimSort.sort(a, 0, a.length, null, 0, 0);
}
```
数组长度小于32时，采用"mini-TimSort"，二分插入排序
```java
// ComparableTimSort
static void sort(Object[] a, int lo, int hi, Object[] work, int workBase, int workLen) {
    assert a != null && lo >= 0 && lo <= hi && hi <= a.length;

    int nRemaining  = hi - lo;
    if (nRemaining < 2)
        return;  // 数组长度为0或1已经有序了

    // If array is small, do a "mini-TimSort" with no merges
    if (nRemaining < MIN_MERGE) {
        // 让数组以递增序列开头，并返回递增序列长度
        int initRunLen = countRunAndMakeAscending(a, lo, hi);
        binarySort(a, lo, hi, lo + initRunLen);
        return;
    }

    ...
}
```
保证排序区间是以递增序列开始的，并且返回这个递增序列的长度
```java
@SuppressWarnings({"unchecked", "rawtypes"})
private static int countRunAndMakeAscending(Object[] a, int lo, int hi) {
    assert lo < hi;
    int runHi = lo + 1;
    if (runHi == hi) // 只有一个元素
        return 1;

    // 如果是递减序列，反转成递增序列
    if (((Comparable) a[runHi++]).compareTo(a[lo]) < 0) { // Descending
        while (runHi < hi && ((Comparable) a[runHi]).compareTo(a[runHi - 1]) < 0)
            runHi++;
        reverseRange(a, lo, runHi);
    } else {                              // Ascending
        while (runHi < hi && ((Comparable) a[runHi]).compareTo(a[runHi - 1]) >= 0)
            runHi++;
    }
    // 返回序列长度
    return runHi - lo;
}

private static void reverseRange(Object[] a, int lo, int hi) {
    hi--;
    while (lo < hi) {
        Object t = a[lo];
        a[lo++] = a[hi];
        a[hi--] = t;
    }
}
```
```java
// +---------------------+
// |  递增序列   |  乱序  |
// +---------------------+
// lo            start    hi
// 二分插入排序
@SuppressWarnings({"fallthrough", "rawtypes", "unchecked"})
private static void binarySort(Object[] a, int lo, int hi, int start) {
    assert lo <= start && start <= hi;
    if (start == lo)
        start++;
    // [left, start) 范围内是已排好序的元素
    // [start, hi) 范围内是待排序元素
    // 从start开始，对每个待待排序元素用二分查找在前面已排好序的部分寻找插入位置
    for ( ; start < hi; start++) {
        Comparable pivot = (Comparable) a[start];
        int left = lo;
        int right = start;
        assert left <= right;
        // 二分查找插入位置
        while (left < right) {
            int mid = (left + right) >>> 1;
            if (pivot.compareTo(a[mid]) < 0)
                right = mid;
            else
                left = mid + 1;
        }
        assert left == right;
        // 插入位置为left，计算left 与 start之间元素数量
        int n = start - left;  // The number of elements to move
        // 下标范围为[left, start)的元素需向后移动一位
        // 如果元素数量为2或1，直接移动，而不是调用数组拷贝，数量更多的话就直接调用拷贝方法
        switch (n) {
            case 2:  a[left + 2] = a[left + 1];
            case 1:  a[left + 1] = a[left];
                        break;
            default: System.arraycopy(a, left, a, left + 1, n);
        }
        // 将待插入元素放入应该插入的位置
        a[left] = pivot;
    }
}
```
如果数组元素数量大于等于32，采用归并排序，基本思路是将数组分成一个个小的部分，对这些小的部分进行二分插入排序，让它们形成一个个有序子序列，然后对这些子序列进行merge
```java
ComparableTimSort ts = new ComparableTimSort(a, work, workBase, workLen);
// minRun就是每一个子序列的长度，是一个[0, 31]范围的整数
int minRun = minRunLength(nRemaining);
do {
    // 对子序列二分插入排序
    int runLen = countRunAndMakeAscending(a, lo, hi);
    if (runLen < minRun) {
        int force = nRemaining <= minRun ? nRemaining : minRun;
        binarySort(a, lo, lo + force, lo + runLen);
        runLen = force;
    }

    // 将这个子序列的起始位置与长度记录下来
    ts.pushRun(lo, runLen);
    // 如果满足条件会合并
    ts.mergeCollapse();

    // 更新子序列起点 与 剩余元素个数
    lo += runLen;
    nRemaining -= runLen;
} while (nRemaining != 0);

assert lo == hi;
// 合并剩下的有序子序列
ts.mergeForceCollapse();
assert ts.stackSize == 1;
```