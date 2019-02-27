### AQS共享锁与CountDownLatch
#### 一. 共享获取状态
```java
public final void acquireShared(int arg) {
    if (tryAcquireShared(arg) < 0)
        doAcquireShared(arg);
}
```
先调用`tryAcquireShared`方法尝试获取状态，如果获取失败(返回结果小于0)，调用`doAcquireShared`方法进队列等待获取状态，`tryAcquireShared`同样是需要在子类中实现的。   
```java
private void doAcquireShared(int arg) {
    final Node node = addWaiter(Node.SHARED);
    boolean failed = true;
    try {
        boolean interrupted = false;
        for (;;) {
            final Node p = node.predecessor();
            if (p == head) {
                int r = tryAcquireShared(arg);
                if (r >= 0) {
                    // 获取状态成功
                    setHeadAndPropagate(node, r);
                    p.next = null; // help GC
                    if (interrupted)
                        selfInterrupt();
                    failed = false;
                    return;
                }
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
```
这里逻辑大部分都和独占获取差不多，区别就是独占模式下，如果前继节点是头节点且获取状态成功后只是将此节点设为头节点，而共享模式除了设置头节点外，还要向后传播获取状态。
```java
private void setHeadAndPropagate(Node node, int propagate) {
    Node h = head; // Record old head for check below
    // 设置当前节点为头节点
    setHead(node); // head = node; node.thread = null; node.prev = null;

    // propagate > 0 还有状态可以获取
    // h == null 不知道是什么情况
    // h.waitStatus >= 0时是初始状态或取消状态，不需要向后唤醒
    if (propagate > 0 || h == null || h.waitStatus < 0) {
        Node s = node.next;
        // node的后继节点不存在
        // 或node为共享状态，共享锁向后传播时遇到独占节点就停止向后传播
        if (s == null || s.isShared())
            doReleaseShared();
    }
}
```
```java
// 唤醒头节点的后继节点并确保向后传播
// 头节点 SIGNAL -> 0 -> PROPAGATE
private void doReleaseShared() {
    /*
        * Ensure that a release propagates, even if there are other
        * in-progress acquires/releases.  This proceeds in the usual
        * way of trying to unparkSuccessor of head if it needs
        * signal. But if it does not, status is set to PROPAGATE to
        * ensure that upon release, propagation continues.
        * Additionally, we must loop in case a new node is added
        * while we are doing this. Also, unlike other uses of
        * unparkSuccessor, we need to know if CAS to reset status
        * fails, if so rechecking.
        */
    for (;;) {
        Node h = head;
        // 队列不空，头节点有后继节点
        if (h != null && h != tail) {
            int ws = h.waitStatus;
            if (ws == Node.SIGNAL) {
                // unparkSuccessor也会执行这个操作，
                // 如果在unparkSuccessor方法里执行，不确保CAS成功，
                // 而这里需要知道这个CAS是否成功(上面英文注释最后一句)
                // 可能有多个线程同时执行到这里，只有CAS成功的线程才能唤醒h的后继节点
                if (!compareAndSetWaitStatus(h, Node.SIGNAL, 0))
                    continue;            // loop to recheck cases
                unparkSuccessor(h);
            }
            else if (ws == 0 &&
                        !compareAndSetWaitStatus(h, 0, Node.PROPAGATE))
                continue;                // loop on failed CAS
        }
        // 头节点是否改变
        if (h == head)                   // loop if head changed
            break;
    }
}
```
头节点的ws变化为SIGNAL -> 0 -> PROPAGATE：首先，可能有多个线程去尝试唤醒头节点的后继节点，但之后CAS成功的线程才能去唤醒，最后将ws置为PROPAGATE，我猜测是为在`setHeadAndPropagate`方法中判断`h.waitStatus < 0`服务，确保能够去唤醒后继节点，那为什么不直接改为PROPAGATE呢？因为在`unparkSuccessor`方法中会将ws置0，即最后是SIGNAL->PROPAGATE->0->PROPAGATE，多了一次CAS。    
* 头节点改变：
    1. 假如在`unparkSuccessor(h)`中，头节点h唤醒了它的后继节点hn，在h的线程执行到`if (h == head)`之前，hn线程占有了头节点。
    2. `h == tail`，线程无法进入第一个if，但是在执行`if (h == head)`之前后面又加入一个节点并修改了头节点

> 这里好乱啊。。。
#### 二. 释放共享状态
```java
public final boolean releaseShared(int arg) {
    if (tryReleaseShared(arg)) {
        doReleaseShared();
        return true;
    }
    return false;
}
```
如果`tryReleaseShared`返回`true`就直接执行`doReleaseShared()`。
#### 三. CountDownLatch
`CountDownLatch`是典型的AQS共享模式的应用。   
在构造`CountDownLatch`对象时传给构造器一个`count`，每调用一次`countDown`方法`count`就减一，只要`count`不为0，调用`await()`方法的线程就会一直阻塞，直至`count`为0
```java
public CountDownLatch(int count) {
    if (count < 0) throw new IllegalArgumentException("count < 0");
    this.sync = new Sync(count);
}
```
`Sync`是`CountDownLatch`内部的一个AQS的实现类，对`CountDownLatch`设置的`count`会转为AQS的状态：
```java
private static final class Sync extends AbstractQueuedSynchronizer {
    private static final long serialVersionUID = 4982264981922014374L;

    Sync(int count) {
        setState(count);
    }

    int getCount() {
        return getState();
    }

    // 状态为0返回1，说明状态可以获取
    protected int tryAcquireShared(int acquires) {
        return (getState() == 0) ? 1 : -1;
    }

    protected boolean tryReleaseShared(int releases) {
        // Decrement count; signal when transition to zero
        for (;;) {
            int c = getState();
            // 状态已经为0，无法继续释放了
            if (c == 0)
                return false;
            int nextc = c-1;
            if (compareAndSetState(c, nextc))
                // 减一后如果为0，说明计数结束，可以去唤醒因调用await()而阻塞的线程了
                return nextc == 0;
        }
    }
}
```
`await()`方法：
```java
public void await() throws InterruptedException {
    sync.acquireSharedInterruptibly(1);
}
```
由上面`Sync`的代码可以看到，如果状态等于0的话，返回1，大于0，说明状态可以获取，此时调用`await()`方法的线程就可以获取状态，继续往下执行；如果状态不等于0，说明获取不到状态，需要入队列等待。
`countDown()`方法：
```java
public void countDown() {
    sync.releaseShared(1);
}
```
每次调用`countDown()`方法状态就减1，如果状态减为0就可以去唤醒队列中的线程了。

> 总结：await()方法获取状态，只有状态为0时才可以获取，countDown()方法释放状态，将计数减1，当减到0时就可以去唤醒阻塞的线程，这些线程唤醒后可以获取状态而继续执行