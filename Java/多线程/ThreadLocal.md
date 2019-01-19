### ThreadLocal
线程本地变量,通过为每个线程提供一个变量的副本，从而实现线程安全的访问。   
`ThreadLocal`里有一个叫`ThreadLocalMap`的内部类，在每个线程对象中都有一个`ThreadLocalMap`对象，当向一个`ThreadLocal`对象中存放值的时候，会从`Thread`对象中获取这个map，如果不存在就创建一个，然后以这个`ThreadLocal`对象为key，将value存进这个map中，从`ThreadLocal`中获取对象也是同样是从这个map中取。
```java
public void set(T value) {
    Thread t = Thread.currentThread();
    ThreadLocalMap map = getMap(t);
    if (map != null) {
        map.set(this, value);
    } else {
        createMap(t, value);
    }
}

void createMap(Thread t, T firstValue) {
    t.threadLocals = new ThreadLocalMap(this, firstValue);
}
```
所以`ThreadLocalMap`是`ThreadLocal`的核心，与`HashMap`不同，在hash发生冲突时，这里采用的是线性探测法。

---   
存放键值对的类：   
这里使用弱引用来指向`ThreadLocal`对象是为了当没有强引用指向`ThreadLocal`对象时，能尽早被回收，如果这里用强引用，那么`ThreadLocalMap`里总会有一个强引用指向`ThreadLocal`对象，而`ThreadLocalMap`对象被`Thead`对象强引用，这样`ThreadLocal`对象就和线程的生命周期绑定到一起。采用弱引用后，`ThreadLocal`对象没用时能被及时回收，但对应的`Entry`对象却不会，所以`ThreadLocalMap`中有相应的清理机制，只要`Entry.get() == null`，这个`Entry`就可以被清理掉了，在get set remove操作中都会触发不同程度的清理行为。
```java
static class Entry extends WeakReference<ThreadLocal<?>> {
    /** The value associated with this ThreadLocal. */
    Object value;

    Entry(ThreadLocal<?> k, Object v) {
        super(k);
        value = v;
    }
}
```
Fields:
```java
// 数组初始容量，必须是2的次方数
private static final int INITIAL_CAPACITY = 16;

// 存放键值对的数组，必要时会扩容，长度必须是2的次方数
private Entry[] table;

// 存放键值对数量
private int size = 0;

// 触发下一次扩容时键值对的数量
private int threshold; // Default to 0
```
Constructors:
```java
// 第一次向一个空ThreadLocal里放值的时候会用这个构造器来创建
ThreadLocalMap(ThreadLocal<?> firstKey, Object firstValue) {
    // 以初始容量创建一个数组
    table = new Entry[INITIAL_CAPACITY];
    // 采用和HashMap相同的方法计算hash % INITIAL_CAPACITY，所以数组长度必须是2的次方数
    int i = firstKey.threadLocalHashCode & (INITIAL_CAPACITY - 1);
    table[i] = new Entry(firstKey, firstValue);
    size = 1;
    // 计算键值对容量上限
    setThreshold(INITIAL_CAPACITY);
}
// 可以看成是负载因子为 2 / 3
private void setThreshold(int len) {
    threshold = len * 2 / 3;
}
```
每个`ThreadLocal`对象的`hashcode`会比上一个创建的`hashcode`多`0x61c88647`，据说这个数和斐波那契数列有关，可以使得hashcode对2的次方数取模的结果分布很均匀，在发生冲突时，可以很快地找到下一个可用的地址。  
```java
private final int threadLocalHashCode = nextHashCode();

private static AtomicInteger nextHashCode =
    new AtomicInteger();

private static final int HASH_INCREMENT = 0x61c88647;

private static int nextHashCode() {
    return nextHashCode.getAndAdd(HASH_INCREMENT);
}
```
Fields:   
`getEntry`方法，根据key找到键值对：
```java
private Entry getEntry(ThreadLocal<?> key) {
    // hash % len
    int i = key.threadLocalHashCode & (table.length - 1);
    Entry e = table[i];
    // 地址没冲突
    if (e != null && e.get() == key)
        return e;
    else
        // 地址发生冲突
        return getEntryAfterMiss(key, i, e);
}

private Entry getEntryAfterMiss(ThreadLocal<?> key, int i, Entry e) {
    Entry[] tab = table;
    int len = tab.length;
    // 线性地址探测
    while (e != null) {
        ThreadLocal<?> k = e.get();
        if (k == key)
            // 找到目标，返回
            return e;
        if (k == null)
            // 该Entry中存的ThreadLocal对象为null
            // 说明该Entry中的ThreadLocal对象被回收了，此时需要清理
            expungeStaleEntry(i);
        else
            // 线性探测的下一个下标 (i + 1) % len
            i = nextIndex(i, len);
        e = tab[i];
    }
    return null;
}

private static int nextIndex(int i, int len) {
    return ((i + 1 < len) ? i + 1 : 0);
}
// 清理失效的弱引用，从staleSlot开始清理一段连续的区域
private int expungeStaleEntry(int staleSlot) {
    Entry[] tab = table;
    int len = tab.length;

    // 该Entry的value置为null，数组此位置置为null
    tab[staleSlot].value = null;
    tab[staleSlot] = null;
    size--;

    // Rehash until we encounter null
    Entry e;
    int i;
    for (i = nextIndex(staleSlot, len);
            (e = tab[i]) != null;
            i = nextIndex(i, len)) {
        ThreadLocal<?> k = e.get();
        if (k == null) {
            // 弱引用失效，需要清理
            e.value = null;
            tab[i] = null;
            size--;
        } else {
            // 如果该引用未失效，则需要重新计算该Entry的位置
            // 否则，如果该Entry前面的位置有被清理过的
            // 会造成具有相同hash的Entry在数组中的下标不连续，中间出现null值
            // 而在查找的时候，就是以出现null值作为线性探测的结束标志，可能造成明明存在，但就是找不到
            int h = k.threadLocalHashCode & (len - 1);
            if (h != i) {
                while (tab[h] != null)
                    h = nextIndex(h, len);
                tab[h] = e;
            }
        }
    }
    return i;
}
```
`set`方法：  
```java
private void set(ThreadLocal<?> key, Object value) {

    Entry[] tab = table;
    int len = tab.length;
    int i = key.threadLocalHashCode & (len-1);
    // 线性探测
    for (Entry e = tab[i];
            e != null;
            e = tab[i = nextIndex(i, len)]) {
        ThreadLocal<?> k = e.get();
        // 找到对应的Entry
        if (k == key) {
            e.value = value;
            return;
        }
        // 此位置的Entry失效，可以替换此位置的Entry，将key和value放到到这个位置
        if (k == null) {
            replaceStaleEntry(key, value, i);
            return;
        }
    }
    // 到这里说明这个key是新添加的，之前不存在
    tab[i] = new Entry(key, value);
    int sz = ++size;
    // 清理之后，如果没有元素被移除，且键值对容量达到上限，扩容
    if (!cleanSomeSlots(i, sz) && sz >= threshold)
        rehash();
}

// 将key value 放在staleSlot处
private void replaceStaleEntry(ThreadLocal<?> key, Object value,
                                int staleSlot) {
    Entry[] tab = table;
    int len = tab.length;
    Entry e;

    // 从staleSlot开始向前查找，找到一片连续的不为null的区间中最左边的失效的弱引用的下标slotToExpunge
    int slotToExpunge = staleSlot;
    for (int i = prevIndex(staleSlot, len);
            (e = tab[i]) != null;
            i = prevIndex(i, len))
        if (e.get() == null)
            slotToExpunge = i;

    // 从staleSlot开始向后查找
    for (int i = nextIndex(staleSlot, len);
            (e = tab[i]) != null;
            i = nextIndex(i, len)) {
        ThreadLocal<?> k = e.get();
        // 在staleSlot后面找到key，将value放在此处
        // 然后交换staleSlot和此处的Entry
        // 结果是value被放到staleSlot出，i处变为失效引用
        if (k == key) {
            e.value = value;

            tab[i] = tab[staleSlot];
            tab[staleSlot] = e;

            // 如果staleSlot左边没有失效引用，将清理开始位置设为当前下标
            if (slotToExpunge == staleSlot)
                slotToExpunge = i;
            cleanSomeSlots(expungeStaleEntry(slotToExpunge), len);
            return;
        }

        // 如果staleSlot左边没有失效引用，将清理开始位置设为当前下标
        if (k == null && slotToExpunge == staleSlot)
            slotToExpunge = i;
    }

    // 在staleSlot后面没有找到key，说明是新插入的，直接放在这里
    tab[staleSlot].value = null;
    tab[staleSlot] = new Entry(key, value);

    // slotToExpunge != staleSlot 说明在staleSlot左边或右边有失效的弱引用，需要清理
    if (slotToExpunge != staleSlot)
        cleanSomeSlots(expungeStaleEntry(slotToExpunge), len);
}
// Heuristically scan 启发式扫描
// 传入的i处要么为null，要么是一个有效的Entry，所以从i的下一个位置开始扫描
// n用于控制扫描次数，如果log n 此扫描没有找到失效的引用，扫描就结束了。
private boolean cleanSomeSlots(int i, int n) {
    boolean removed = false;
    Entry[] tab = table;
    int len = tab.length;
    do {
        i = nextIndex(i, len);
        Entry e = tab[i];
        // 每次找到失效的弱引用，开始一段连续空间的清理
        if (e != null && e.get() == null) {
            n = len;
            removed = true;
            i = expungeStaleEntry(i);
        }
    } while ( (n >>>= 1) != 0);
    return removed;
}


private void rehash() {
    // 整个数组清理一遍
    expungeStaleEntries();

    // 因为做了一次清理，所以size很可能会变小
    // 此时size只要达到 len / 2就扩容( 3/4 * 2/3)
    if (size >= threshold - threshold / 4)
        resize();
}

private void expungeStaleEntries() {
    Entry[] tab = table;
    int len = tab.length;
    for (int j = 0; j < len; j++) {
        Entry e = tab[j];
        if (e != null && e.get() == null)
            expungeStaleEntry(j);
    }
}

private void resize() {
    Entry[] oldTab = table;
    int oldLen = oldTab.length;
    // 数组长度变为原来2倍
    int newLen = oldLen * 2;
    Entry[] newTab = new Entry[newLen];
    int count = 0;
    // 把旧数组中的Entry重新用线性探测寻找位置，放入新数组
    for (Entry e : oldTab) {
        if (e != null) {
            ThreadLocal<?> k = e.get();
            if (k == null) {
                e.value = null; // Help the GC
            } else {
                int h = k.threadLocalHashCode & (newLen - 1);
                while (newTab[h] != null)
                    h = nextIndex(h, newLen);
                newTab[h] = e;
                count++;
            }
        }
    }

    setThreshold(newLen);
    size = count;
    table = newTab;
}
```
`remove`方法：  
```java
private void remove(ThreadLocal<?> key) {
    Entry[] tab = table;
    int len = tab.length;
    int i = key.threadLocalHashCode & (len-1);
    for (Entry e = tab[i];
            e != null;
            e = tab[i = nextIndex(i, len)]) {
        // 线性探测找到后，切断弱引用，从当前下标开始清理一段
        if (e.get() == key) {
            e.clear();
            expungeStaleEntry(i);
            return;
        }
    }
}
```