### AQS独占锁
#### 一. AbstractQueuedSynchronizer
juc包下的很多类，比如可重入锁，读写锁，闭锁等等，都是利用`AQS`来实现的，其在内部维护了一个FIFO的队列和一个状态变量，多个线程利用`CAS`去竞争获取状态，获取到状态的线程可以继续往下执行，当状态不可获取时，线程进队列，然后用`LockSupport`将线程`park`，等待队列中前面以获取到状态的线程唤醒它。    
`AQS`有独占和共享两种模式，需子类去实现具体逻辑。   
```java
private transient volatile Node head;

private transient volatile Node tail;

private volatile int state;
```
* `state`是同步状态
* `head`与`tail`是链表头部和尾部，头部节点并不存放任何线程引用，仅仅是代表一个链表头部。    
#### 二. 队列的节点数据结构
```java
static final class Node {
    /** Marker to indicate a node is waiting in shared mode */
    static final Node SHARED = new Node();
    /** Marker to indicate a node is waiting in exclusive mode */
    static final Node EXCLUSIVE = null;

    /** waitStatus value to indicate thread has cancelled */
    static final int CANCELLED =  1;
    /** waitStatus value to indicate successor's thread needs unparking */
    static final int SIGNAL    = -1;
    /** waitStatus value to indicate thread is waiting on condition */
    static final int CONDITION = -2;
    /**
        * waitStatus value to indicate the next acquireShared should
        * unconditionally propagate
        */
    static final int PROPAGATE = -3;

    volatile int waitStatus;

    volatile Node prev;

    volatile Node next;

    volatile Thread thread;

    Node nextWaiter;
}
```
* 两个`Node`型的常量`SHARED`和`EXCLUSIVE`分别表示这个节点是处于共享还是独占模式
* `prev`与`next`是双向链表的前继和后继指针，`thread`是该节点封装的线程
* 变量`waitStatus`表示该节点的等待状态，有以下几种：
    * `CANCELLED`：1，该线程由于中断或是超时而取消获取状态
    * `SIGNAL`：-1，该线程需要唤醒它的后继节点的线程
    * `CONDITION`：-2，该线程位于`condition`队列
    * `PROPAGATE`：-3，共享模式下`acquireShared`需要向后传播
    * 0：初始状态，表示当前线程正在队列中等待获取锁
* `nextWaiter`：`condition`队列中的下一个等待节点，否则用来表示是否是共享模式

#### 三. 独占获取状态
```java
public final void acquire(int arg) {
    if (!tryAcquire(arg) &&
        acquireQueued(addWaiter(Node.EXCLUSIVE), arg))
        selfInterrupt();
}
```
首先调用`tryAcquire`方法尝试获取状态，如果获取成功立即返回，如果获取失败，先调用`addWaiter`方法将该线程封装成节点，加入队列尾部，然后调用`acquireQueued`方法排队获取状态。    
`tryAcquire`方法需要自己去实现，下面是`ReentrantLock`中非公平锁的实现：
```java
protected final boolean tryAcquire(int acquires) {
    return nonfairTryAcquire(acquires);
}

final boolean nonfairTryAcquire(int acquires) {
    final Thread current = Thread.currentThread();
    int c = getState();
    if (c == 0) {
        // c==0说明锁还没有被获取，这时使用CAS尝试更改状态来获取锁
        if (compareAndSetState(0, acquires)) {
            // CAS获取成功，将独占线程设为当前线程，返回成功标识
            setExclusiveOwnerThread(current);
            return true;
        }
    }
    else if (current == getExclusiveOwnerThread()) {
        // 计数不为0且当前线程就为独占线程，说明是获取锁的线程重入的情况
        // 此时不会多个线程更改状态，所以直接更改状态而不用CAS
        // 返回成功标识
        int nextc = c + acquires;
        if (nextc < 0) // overflow
            throw new Error("Maximum lock count exceeded");
        setState(nextc);
        return true;
    }
    // 1. CAS获取状态失败
    // 2. 锁在获取之前已被其他线程独占
    return false;
}
```
`ReentrantLock`是可重入锁，其中用状态`state`来计数是实现可重入的关键。   
`addWaiter`方法将线程封装成节点，然后添加到队列尾部。   
```java
private Node addWaiter(Node mode) {
    Node node = new Node(Thread.currentThread(), mode);
    // Try the fast path of enq; backup to full enq on failure
    Node pred = tail;
    if (pred != null) {
        node.prev = pred;
        if (compareAndSetTail(pred, node)) {
            pred.next = node;
            return node;
        }
    }
    // 队列为空或CAS失败
    enq(node);
    return node;
}

private Node enq(final Node node) {
    for (;;) {
        Node t = tail;
        if (t == null) { // Must initialize
            // 队列为空，先添加个头节点
            if (compareAndSetHead(new Node()))
                tail = head;
        } else {
            node.prev = t;
            if (compareAndSetTail(t, node)) {
                t.next = node;
                return t;
            }
        }
    }
}    
```
如果队列不为空的话，先尝试用CAS快速将节点添加到链表尾部，如果失败或队列为空，再调用`enq`方法添加。   
在`enq`方法中，如果队列为空，会先添加一个头节点，然后再用`CAS`将节点添加到尾部。    
之后调用`acquireQueued`方法，在队列中获取状态。 
```java
final boolean acquireQueued(final Node node, int arg) {
    boolean failed = true;
    try {
        boolean interrupted = false;
        for (;;) {
            final Node p = node.predecessor();
            if (p == head && tryAcquire(arg)) {
                setHead(node);
                p.next = null; // help GC
                failed = false;
                return interrupted;
            }
            if (shouldParkAfterFailedAcquire(p, node) &&
                parkAndCheckInterrupt())
                interrupted = true;
        }
    } finally {
        if (failed)
            cancelAcquire(node);
    }
}

private void setHead(Node node) {
    head = node;
    node.thread = null;
    node.prev = null;
}
```
如果当前节点的前继节点是头节点，则尝试获取状态，如果获取成功，将当前节点设为头节点，返回是否被中断，否则如果获取状态失败，会调用`shouldParkAfterFailedAcquire`方法来决定获取失败后是否要`park`当前线程，如果要`park`，则调用`parkAndCheckInterrupt`方法来`park`线程，当从`park`中被唤醒后返回线程是否被中断。
```java
private static boolean shouldParkAfterFailedAcquire(Node pred, Node node) {
    int ws = pred.waitStatus;
    if (ws == Node.SIGNAL) // -1
        return true;
    if (ws > 0) {  // CANCELLED
        do {
            node.prev = pred = pred.prev;
        } while (pred.waitStatus > 0);
        pred.next = node;
    } else {  // 只能是0，CONDITION或PROPAGATE
        compareAndSetWaitStatus(pred, ws, Node.SIGNAL);
    }
    return false;
}
```
如果前继节点的`waitStatus`是`SIGNAL`，返回`true`，说明当前线程可以`park`，因为只有前继节点为`SIGNAL`当前线程才`park`后才会被前继节点`unpark`。   
如果前继节点被取消获取状态，则跳过前面所有取消的节点。   
如果是其他状态，都将状态改为`SIGNAL`，为了当前节点能够放心`park`。
```java
private final boolean parkAndCheckInterrupt() {
    LockSupport.park(this);
    return Thread.interrupted();
}
```
线程被`park`后，调用`unpark`或调用线程的`interrupt`方法都会使线程唤醒，所以这里调用`Thread.interrupted()`，返回是否被中断，然后在`selfInterrupt`方法里恢复中断状态。
#### 四. 独占释放状态
```java
public final boolean release(int arg) {
    if (tryRelease(arg)) {
        Node h = head;
        // 队列不为空
        // h.waitStatus==0 是初始化的空队列
        if (h != null && h.waitStatus != 0)
            // 唤醒后继节点
            unparkSuccessor(h);
        return true;
    }
    return false;
}
```
首先调用`tryRelease`方法尝试释放状态，这个方法需要子类去实现，释放成功后，去唤醒头节点的后继节点。
```java
private void unparkSuccessor(Node node) {
    int ws = node.waitStatus;
    if (ws < 0)
        // 如果等待状态小于0，就把它改为0，失败也没关系？？？？？？
        compareAndSetWaitStatus(node, ws, 0);

    Node s = node.next;
    // 如果s的后继节点为空，或被取消，重新找后继节点
    if (s == null || s.waitStatus > 0) {
        s = null;
        // node后面第一个waitStatus小于等于0的节点(等于0的话是尾节点)
        for (Node t = tail; t != null && t != node; t = t.prev)
            if (t.waitStatus <= 0)
                s = t;
    }
    // 唤醒线程
    if (s != null)
        LockSupport.unpark(s.thread);
}
```
下面这个是`ReentrantLock`实现的`tryRelease`：
```java
protected final boolean tryRelease(int releases) {
    int c = getState() - releases;
    if (Thread.currentThread() != getExclusiveOwnerThread())
        throw new IllegalMonitorStateException();
    boolean free = false;
    if (c == 0) {
        free = true;
        setExclusiveOwnerThread(null);
    }
    setState(c);
    return free;
}
```
如果释放线程不是独占线程会抛异常。  
如果当前状态-1 为0，说明该独占线程全部释放，独占线程置空，否则是重入后的解锁，只将状态-1.
#### 五. 可中断获取
```java
public final void acquireInterruptibly(int arg)
        throws InterruptedException {
    if (Thread.interrupted())
        throw new InterruptedException();
    if (!tryAcquire(arg))
        doAcquireInterruptibly(arg);
}
```
进入方法先检查是否被中断，如果是直接抛中断异常。然后如果`tryAcquire`获取失败，再进队列获取。
```java
private void doAcquireInterruptibly(int arg)
    throws InterruptedException {
    final Node node = addWaiter(Node.EXCLUSIVE);
    boolean failed = true;
    try {
        for (;;) {
            final Node p = node.predecessor();
            if (p == head && tryAcquire(arg)) {
                setHead(node);
                p.next = null; // help GC
                failed = false;
                return;
            }
            if (shouldParkAfterFailedAcquire(p, node) &&
                parkAndCheckInterrupt())
                throw new InterruptedException();
        }
    } finally {
        if (failed)
            cancelAcquire(node);
    }
}
```
这里的逻辑和上面的差不多，只是如果线程`park`后，是由于中断而唤醒的，会直接抛中断异常，然后再`finally`里取消获取。
#### 六. 超时获取（可中断）
```java
public final boolean tryAcquireNanos(int arg, long nanosTimeout)
        throws InterruptedException {
    if (Thread.interrupted())
        throw new InterruptedException();
    return tryAcquire(arg) ||
        doAcquireNanos(arg, nanosTimeout);
}
```
在给定时间内，如果获取成功返回`true`，反之返回`false`
```java
private boolean doAcquireNanos(int arg, long nanosTimeout)
    throws InterruptedException {
    long lastTime = System.nanoTime();
    final Node node = addWaiter(Node.EXCLUSIVE);
    boolean failed = true;
    try {
        for (;;) {
            final Node p = node.predecessor();
            if (p == head && tryAcquire(arg)) {
                setHead(node);
                p.next = null; // help GC
                failed = false;
                return true;
            }
            // 超时
            if (nanosTimeout <= 0)
                return false;
            if (shouldParkAfterFailedAcquire(p, node) &&
                nanosTimeout > spinForTimeoutThreshold)
                LockSupport.parkNanos(this, nanosTimeout);
            long now = System.nanoTime();
            // 计算剩余时间
            nanosTimeout -= now - lastTime;
            lastTime = now;
            if (Thread.interrupted())
                throw new InterruptedException();
        }
    } finally {
        if (failed)
            cancelAcquire(node);
    }
}
```
每次自旋，当剩余时间大于`spinForTimeoutThreshold`时，线程`park`，反之直接进行快速自旋，因为此时剩余时间已经非常小了，非常短的休眠时间无法做到十分精确。