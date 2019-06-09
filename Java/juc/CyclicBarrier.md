### CyclicBarrier

#### 一. 
`CyclicBarrier`允许一组线程相互等待直到所有线程都到达一个公共的屏障点，作用类似于`CountDownLatch`，但是和`CountDownLatch`相比，它在所有程序到达屏障点后可以被重复使用。
可以为`CyclicBarrier`提供一个任务，当所有线程到达屏障点后(`tripped`)、屏障释放让线程继续往下执行之前，这个任务可以被触发执行，比如各个线程执行一个计算任务，都完成计算后，由这个任务对各个线程的计算结果进行汇总处理。
```java
public static void main(String[] args) {
    CyclicBarrier cyclicBarrier = new CyclicBarrier(3, () -> System.out.println("=========done========="));

    Runnable runnable = () -> {
        System.out.println("Thread " + Thread.currentThread().getName() + " step 1");
        try {
            cyclicBarrier.await();
            System.out.println("Thread " + Thread.currentThread().getName() + " step 2");
            cyclicBarrier.await();
            System.out.println("Thread " + Thread.currentThread().getName() + " step 3");
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (BrokenBarrierException e) {
            e.printStackTrace();
        }
    };

    Thread t1 = new Thread(runnable);
    t1.setName("t1");
    Thread t2 = new Thread(runnable);
    t2.setName("t2");
    Thread t3 = new Thread(runnable);
    t3.setName("t3");

    t1.start();
    t2.start();
    t3.start();
}
```
输出结果：
```text
Thread t2 step 1
Thread t1 step 1
Thread t3 step 1
=========done=========
Thread t1 step 2
Thread t2 step 2
Thread t3 step 2
=========done=========
Thread t1 step 3
Thread t3 step 3
Thread t2 step 3
```

#### 二. 源码分析
1. 主要成员变量
```java
private final ReentrantLock lock = new ReentrantLock();
private final Condition trip = lock.newCondition();
// 参与的线程的数量
private final int parties;
// 当所有线程到达屏障点时触发的任务
private final Runnable barrierCommand;

private Generation generation = new Generation();
// 记录还有多少个线程没到屏障点
private int count;
```
对`CyclicBarrier`的每一次循环使用，都用一个`Generation`实例来表示，其内部也记录了此次循环`barrier`是否被`break`，当`barrier`被`tripped`或`reset`时，会生成一个新的实例
```java
private static class Generation {
    Generation() {}                 // prevent access constructor creation
    boolean broken;                 // initially false
}
```
2. await
```java
public int await() throws InterruptedException, BrokenBarrierException {
    try {
        return dowait(false, 0L);
    } catch (TimeoutException toe) {
        throw new Error(toe); // cannot happen
    }
}
```
`await`方法的主要逻辑都是在`doWait`中实现的：
```java
private int dowait(boolean timed, long nanos) throws InterruptedException, BrokenBarrierException, TimeoutException {
    final ReentrantLock lock = this.lock;
    lock.lock();
    try {
        final Generation g = generation;

        if (g.broken) // 如果barrier的此次使用已经被break，抛出异常
            throw new BrokenBarrierException();

        if (Thread.interrupted()) { // 如果线程被中断，先将barrier给break掉，然后抛出中断异常
            breakBarrier();
            throw new InterruptedException();
        }
        // count最开始等于parties，每await一次就减一，直至0
        // 这个index表示了此线程是第几个到达屏障点的，parties-1为第一个，0为最后一个
        int index = --count;
        if (index == 0) {  // 剩余线程数量为0，可以继续往下执行了，tripped
            boolean ranAction = false;
            try {
                final Runnable command = barrierCommand;
                if (command != null)
                    command.run();
                ranAction = true; // tripped时触发的任务成功执行完，barrier进入下一次使用
                nextGeneration(); // 此时会创建一个新的Generation实例
                return 0;
            } finally {
                if (!ranAction)  // 如果这个任务执行失败，将barrier给break掉
                    breakBarrier();
            }
        }
        // 执行到这里说明，还有线程没到屏障点，此线程需要await
        // loop until tripped, broken, interrupted, or timed out
        for (;;) {
            try { // 【1】
                if (!timed) // 非计时等待，直接await
                    trip.await();
                else if (nanos > 0L) // 计时等待
                    nanos = trip.awaitNanos(nanos);
            } catch (InterruptedException ie) {
                if (g == generation && ! g.broken) {
                    breakBarrier(); // 线程在await时被中断，break
                    throw ie;
                } else {
                    Thread.currentThread().interrupt();
                }
            }
            // barrier已经break
            if (g.broken)
                throw new BrokenBarrierException();
            // 正常结束，返回本线程到达屏障点的顺序
            if (g != generation)
                return index;
            //超时，break掉barrier，则后续线程都会抛出BrokenBarrierException
            if (timed && nanos <= 0L) {
                breakBarrier();
                throw new TimeoutException();
            }
        }
    } finally {
        lock.unlock();
    }
}
```
最后一个到达屏障点的线程，会去执行`barrierCommand`，成功执行后，就会调用`nextGeneration()`方法，让`barrier`进入下一次循环使用，而且在这个方法里也会调用`signalAll`，将所有在【1】处`await`的线程唤醒：
```java
private void nextGeneration() {
    // signal completion of last generation
    trip.signalAll();
    // set up next generation
    count = parties;
    generation = new Generation();
}
```

总结：调用`await`方法的线程会在一个`Condition`对象上`await`，最后一个到达屏障点的线程会调用`signalAll`方法将它们都唤醒，并重置`barrier`的参数以便重复使用。  

3. 由源码可以看到，调用`await`方法的线程，在以下情况下会结束等待状态：
    * 所有线程到达屏障点，并且`barrierCommand`成功执行
    * 当前线程被中断，则抛出`InterruptedException`异常
    * 当前线程等待超时，则抛出`TimeoutException`异常
    * 其他等待的线程被中断，则当前线程抛出`BrokenBarrierException`异常
    * 其他等待的线程超时，则当前线程抛出`BrokenBarrierException`异常
    * 其他线程调用`CyclicBarrier.reset()`方法，则当前线程抛出`BrokenBarrierException`异常