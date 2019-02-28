### AQS与Condition
> 使用了Condition对象后，除了AQS内部维护的一个同步队列之外，还有一个在Condition对象中的等待队列，调用`await`方法而等待的线程处于等待队列中，被`signal`的线程会从等待队列移入同步队列，等待获取锁。
#### 一. Condition接口
`Condition`是一个接口，它的实现类是在AQS中定义的一个内部类`ConditionObject`
```java
public interface Condition {

    void await() throws InterruptedException;

    void awaitUninterruptibly();

    long awaitNanos(long nanosTimeout) throws InterruptedException;

    boolean await(long time, TimeUnit unit) throws InterruptedException;

    boolean awaitUntil(Date deadline) throws InterruptedException;

    void signal();
    
    void signalAll();
}
```
`ConditionObject`内部维护了一个等待队列，是一个单向链表，`firstWaiter`是队头，`lastWaiter`是队尾。
#### 二. await()
```java
public final void await() throws InterruptedException {
    if (Thread.interrupted())
        throw new InterruptedException();
    Node node = addConditionWaiter();
    // 记录锁释放之前的状态
    int savedState = fullyRelease(node);
    int interruptMode = 0;
    // 当节点不在同步队列中，即在等待队列中时，在这里循环
    // 知道节点进入同步队列竞争锁
    while (!isOnSyncQueue(node)) {
        // 如果节点还在等待队列中，park
        LockSupport.park(this);
        // 线程被中断后立即跳出循环
        if ((interruptMode = checkInterruptWhileWaiting(node)) != 0)
            break;
    }
    if (acquireQueued(node, savedState) && interruptMode != THROW_IE)
        interruptMode = REINTERRUPT;
    if (node.nextWaiter != null) // clean up if cancelled
        unlinkCancelledWaiters();
    if (interruptMode != 0)
        reportInterruptAfterWait(interruptMode);
}
```
先调用`addConditionWaiter()`方法将当前线程加入`Condition`队列：
```java
private Node addConditionWaiter() {
    Node t = lastWaiter;
    // 清除等待队列中已取消的节点
    if (t != null && t.waitStatus != Node.CONDITION) {
        unlinkCancelledWaiters();
        t = lastWaiter;
    }
    // 入队列
    Node node = new Node(Thread.currentThread(), Node.CONDITION);
    if (t == null)
        firstWaiter = node;
    else
        t.nextWaiter = node;
    lastWaiter = node;
    return node;
}
```
然后调用`fullyRelease`方法将锁释放，以便让其他线程能够获取锁，然后满足条件时来唤醒此线程：
```java
final int fullyRelease(Node node) {
    boolean failed = true;
    try {
        int savedState = getState();
        if (release(savedState)) {
            failed = false;
            // 返回锁释放之前的状态
            return savedState;
        } else {
            // 释放失败说明该线程没有获取锁就释放锁，即没获取锁就调用await
            throw new IllegalMonitorStateException();
        }
    } finally {
        if (failed) // 释放失败，节点设为取消
            node.waitStatus = Node.CANCELLED;
    }
}
```
然后调用`isOnSyncQueue`方法不断检查节点是否在同步队列中，如果不在，那就是在等待队列中，这时应一直在这里循环挂起，直到节点进入同步队列竞争锁。
```java
// 如果节点在同步队列中，返回true，在等待队列中返回false
final boolean isOnSyncQueue(Node node) {
    // 节点的ws为CONDITION，肯定是在等待队列中
    // 由于node节点是在await方法中新建的，所以next，prev最初都为null
    // 而另一个线程调用signal方法唤醒此线程时，会将此node放入同步队列，
    // 此时node一定会有前继节点(enq方法)，所以prev == null一定是在等待队列中
    // 但如果prev != null，并不能说明节点就是在同步队列中(下面注释)
    if (node.waitStatus == Node.CONDITION || node.prev == null)
        return false;
    // next == null不能说明是在等待队列中，因为可能是同步队列中的尾节点
    // 所以，反过来，如果next != null，那一定是在同步队列中
    if (node.next != null) // If has successor, it must be on queue
        return true;

    // node.prev != null时，节点也有可能不在同步队列中，因为在enq方法中，
    // 是先node.prev = t，然后用CAS更改尾指针tail，如果这个CAS成功了，才能算是node进入同步队列
    // 但如果失败了，就不在同步队列中，这个方法返回true后，是要继续竞争锁的，但如果这时候节点还没完全入队，是不可以的
    // 由于节点是插入到队列尾部的，所以从尾部向前遍历查找会效率高些
    return findNodeFromTail(node);
}

private boolean findNodeFromTail(Node node) {
    Node t = tail;
    for (;;) {
        if (t == node)
            return true;
        if (t == null)
            return false;
        t = t.prev;
    }
}
```
检查线程在等待的时候是否被中断，如果没有被中断返回0；        
如果被中断，如果是在被signal之前中断的，返回`THROW_IE`，如果之后中断的，返回`REINTERRUPT`    
`REINTERRUPT`：从`wait`退出后应恢复中断；    
`THROW_IE`：从`wait`退出后应抛出`InterruptedException`
```java
private int checkInterruptWhileWaiting(Node node) {
    return Thread.interrupted() ?
        (transferAfterCancelledWait(node) ? THROW_IE : REINTERRUPT) :
        0;
}

// 如果线程在被signal之前取消，返回true
final boolean transferAfterCancelledWait(Node node) {
    // 这个CAS只有可能在两个地方出现，一个是这里，另一个是signal此线程的时候transferForSignal方法内
    // 如果这里的CAS成功了，说明此线程还没有被signal，将节点加入同步队列，返回true(THROW_IE)
    if (compareAndSetWaitStatus(node, Node.CONDITION, 0)) {
        enq(node);
        return true;
    }

    // 此节点被signal后，还未完全进入同步队列，让出cpu时间
    while (!isOnSyncQueue(node))
        Thread.yield();
    // 确保节点已经进入同步队列，返回false(REINTERRUPT)
    return false;
}        
```
到这里，在`await()`方法中，`while()`循环部分已经执行完了，不管是被`signal`还是线程被中断而退出循环，执行到这里，此线程都在同步队列中，等待获取状态(对`acquireQueued`方法的调用)，`acquireQueued`方法会返回在同步队列中等待时线程是否被中断，如果在等待获取状态时被中断，而且之前被中断而得到的`interruptMode`不为`THROW_IE`时(不需要抛中断异常)，将`interruptMode`设为`REINTERRUPT`，说明待会要恢复中断状态。   
在方法的最后会有对中断的统一处理
```java
if (interruptMode != 0)
    reportInterruptAfterWait(interruptMode);

private void reportInterruptAfterWait(int interruptMode)
    throws InterruptedException {
    if (interruptMode == THROW_IE)
        throw new InterruptedException();
    else if (interruptMode == REINTERRUPT)
        selfInterrupt();
}
```
在处理中断之前，还有这么一段代码来清除已经取消节点：
```java
if (node.nextWaiter != null) // clean up if cancelled
    unlinkCancelledWaiters();

// 遍历一遍链表，删除ws不为CONDITION的节点
private void unlinkCancelledWaiters() {
    Node t = firstWaiter;
    Node trail = null;
    while (t != null) {
        Node next = t.nextWaiter;
        if (t.waitStatus != Node.CONDITION) {
            t.nextWaiter = null;
            if (trail == null) // 删的是头节点
                firstWaiter = next;
            else
                // 前一个CONDITION节点的next指向下一个节点
                trail.nextWaiter = next;
            if (next == null)
                lastWaiter = trail;
        }
        else
            // CONDITION的节点不需要删除
            trail = t;
        t = next;
    }
}    
```
#### 三. signal()  
```java
public final void signal() {
    // 如果当前线程没上锁，抛异常
    if (!isHeldExclusively())
        throw new IllegalMonitorStateException();
    Node first = firstWaiter;
    if (first != null)
        doSignal(first);
}
```
将等待队列的队头节点放入同步队列去等待获取锁
```java
private void doSignal(Node first) {
    do {
        // 从等待队列中删除头节点并将其移动到同步队列，成功则结束
        if ( (firstWaiter = first.nextWaiter) == null)
            lastWaiter = null;
        first.nextWaiter = null;
    } while (!transferForSignal(first) &&
                (first = firstWaiter) != null);
}
```
```java
// 将节点从等待队列放入同步队列中
final boolean transferForSignal(Node node) {
    // 如果这个CAS失败了，说明线程被中断了
    if (!compareAndSetWaitStatus(node, Node.CONDITION, 0))
        return false;

    // 上面的CAS成功，节点入同步队列，并返回在它同步队列中的前继节点
    Node p = enq(node);
    int ws = p.waitStatus;
    // 如果它的前继节点被取消了，唤醒node中的线程
    // 否则如果CAS设置尾SIGNAL失败，也唤醒node中的线程，设置成功的话则不唤醒，由同步队列自己去解决
    // 总之，这里保证了线程会被unpark
    // 由这里唤醒后会进入acquireQueued方法
    if (ws > 0 || !compareAndSetWaitStatus(p, ws, Node.SIGNAL))
        LockSupport.unpark(node.thread);
    return true;
}
```