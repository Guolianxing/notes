### Java8 Time api的常用方法
> 时间戳是指格林威治时间1970年01月01日00时00分00秒(北京时间1970年01月01日08时00分00秒)起至现在的总秒数。
#### 一. 主要的几个类
* Instant 时间戳
* LocalDate 日期
* LocalTime 时间
* LocalDateTime 日期时间
* Duration 两个时间之间的间隔
* Period 两个日期之间的间隔
> 这些类都是不可变的，线程安全的，在这些类的对象上做的修改都会生成一个新的对象。
#### 二. 常用的方法
* Instant与Duration，Period类与之类似
```java
public static void test() {
    Instant start = Instant.now();
    try {
        TimeUnit.SECONDS.sleep(3);
    } catch (InterruptedException e) {
        e.printStackTrace();
    }
    Instant end = Instant.now();
    Duration dif = Duration.between(start, end);
    System.out.println(dif.getSeconds());   // 3000
}
```
* 由秒(时间戳)构建时间对象，并格式化为`yyyy-MM-dd HH:mm:ss`格式字符串
```java
public static String dateTimeToStr(long seconds) {
    LocalDateTime dateTime = LocalDateTime.ofEpochSecond(seconds, 0, ZoneOffset.ofHours(8));
    return dateTime.withNano(0).format(DateTimeFormatter.ISO_LOCAL_DATE_TIME).replace('T', ' ');  // 2018-09-29 22:29:44
}
```
```java
public static String dateTimeToStr(long seconds) {
    Instant instant = Instant.ofEpochSecond(seconds);
    LocalDateTime dateTime = LocalDateTime.ofInstant(instant, ZoneOffset.ofHours(8));
    return dateTime.format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
}
```
* 由毫秒构建
```java
public static String dateTimeToStr(long mill) {
    Instant instant = Instant.ofEpochMilli(mill);
    LocalDateTime dateTime = LocalDateTime.ofInstant(instant, ZoneOffset.ofHours(8));
    return dateTime.withNano(0).format(DateTimeFormatter.ISO_LOCAL_DATE_TIME).replace('T', ' ');
}
```
* LocalDateTime对象可以由LocalDate和LocalTime对象合并得到
```java
    LocalDate date = LocalDate.of(2015, Month.AUGUST, 23);
    LocalTime time = LocalTime.of(14, 35, 21);
    LocalDateTime dateTime = LocalDateTime.of(date, time);
```
* 由字符串解析时间类
```java
public static LocalDateTime parseDateTime(String timeStr) { // 2015-01-12 13:43:13
    LocalDateTime dateTime = LocalDateTime.parse(timeStr, DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
    return dateTime;
}
```