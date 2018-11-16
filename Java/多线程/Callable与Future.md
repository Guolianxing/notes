### Callable与Future
`Runnable`封装一个异步任务，没有参数也没有返回值，而`Callable`接口封装的任务有返回值，返回值类型为泛型的类型。
```java
package java.util.concurrent;

@FunctionalInterface
public interface Callable<V> {
    V call() throws Exception;
}
```
`Future`保存异步任务计算的结果
```java
package java.util.concurrent;

public interface Future<V> {

    // 如果计算没开始，则它被取消不再开始，如果计算正在运行，如果参数为true则被中断
    boolean cancel(boolean mayInterruptIfRunning);

    // 在任务完成之前是否被取消
    boolean isCancelled();

    // 如果任务结束，无论是正常结束、中途取消或发生异常，都返回true
    boolean isDone();

    // 如果运行该任务的线程被中断，以下两个方法都会抛出InterruptedException

    // 如果异步任务已经完成则立即返回计算结果，否则阻塞至任务完成
    V get() throws InterruptedException, ExecutionException;

    // 如果任务完成之前超时，抛出TimeoutException
    V get(long timeout, TimeUnit unit) throws InterruptedException, ExecutionException, TimeoutException;
}
```
`FutureTask`可以将一个`Callable`转换成一个`Future`和`Runnable`，因为这个类实现了`RunnableFuture`接口，而`RunnableFuture`接口同时继承自`Runnable`与`Future`接口。
```java
public static void main(String[] args) throws Exception {
    FutureTask<Integer> task = new FutureTask<>(() -> {
        TimeUnit.SECONDS.sleep(3);
        System.out.println("done");
        return 3;
    });
    new Thread(task).start();
    Integer res = task.get(1, TimeUnit.SECONDS);
    System.out.println(res);
}
```
