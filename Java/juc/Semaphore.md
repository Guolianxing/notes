### Semaphore
`Semaphore`信号量，能够限制对一个临界区域同时访问的线程的数量，`acquire`操作获取一个许可，`release`操作释放一个许可，只有当还有许可剩余时，线程才能够成功`acquire`到许可，并继续向下执行，否则会阻塞直到有许可被其他线程释放。`availablePermits`方法可以获取当前剩余许可数量：     

这段代码限制了同时最多3个线程执行`test`方法：  
```java
public class Main {

    private static Semaphore semaphore = new Semaphore(3, true);

    private static void test() {
        try {
            semaphore.acquire();

            System.out.println("test");
            Thread.sleep(3000);
        } catch (InterruptedException e) {


        } finally {
            semaphore.release();
        }

    }
    public static void main(String[] args) throws InterruptedException {
        for (int i = 0; i < 10; i++) {
            new Thread(() -> test()).start();
        }
    }
}
```

很明显`Semaphore`是基于`AQS`共享模式实现的，而且为`acquire`操作还提供了公平锁与非公平锁两种方式。
