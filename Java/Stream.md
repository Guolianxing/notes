### Stream
#### 一. 方法引用
1. 对象名::实例方法
2. 类名::静态方法
3. 类名::实例方法名   
前两种相当于将`lambda`表达式的参数直接作为方法的参数，如`System.out::println`等价于`x -> System.out.println(x)`，`Math::pow`等价于`(x, y) -> Math.pow(x, y)`   
第三种，`lambda`表达式的第一个参数作为方法的调用者，剩下的参数作为方法的参数，如`String::compareTo`等价于`(x, y) -> x.compareTo(y)`
#### 二 流的几种创建方式
1. `Collection`接口的`stream`方法将任意一个集合转换成一个流
```java
List<String> list = new LinkedList<>();
Stream<String> stringStream = list.stream();
```
2. 静态方法`Stream.of`可以将一个数组或可变长参数转换成一个流
```java
String[] strings = new String[10];
Stream<String> stringStream = Stream.of(strings);
```
3. 静态方法`Arrays.stream(array)`或`Arrays.stream(array, from, to)`可以将一个数组转换成一个流（包括from，不包括to）
```java
String[] strings = new String[10];
Stream<String> stringStream = Arrays.stream(strings);
stringStream = Arrays.stream(strings, 2, 8);
```
4. `Stream.empty()`可以创建一个不包含任何元素的流
```java
Stream<String> stringStream = Stream.empty();
```
5. `Stream.generate`方法产生一个无限流
```java
Stream<String> stringStream = Stream.generate(() -> "a");
Stream<Double> doubleStream = Stream.generate(Math::random);
```
6. `Stream.iterate`产生一个无限流
```java
Stream<Integer> stream = Stream.iterate(0, x -> x + 1);
```
给定一个`x0`和函数`f(x)`，流的第一个元素是`x0`，第二个是`f(x0)`，第三个是`f(f(x0))`，以此类推

#### 三. 非终结操作
在流操作中分为终结操作和非终结操作，非终结操作不会触发数据的处理，只是创建一个新的流。    
`filter`     
`map`  
`flatMap`  
`limit`  
`skip`  
`concat`  
`distinct`  
`sorted`   
`peek`   
`parallel`  
1. `filter`接收一个`Predicate`，并产生一个流，其中包含当前流中所有满足断言条件的元素
```java
List<String> list = Arrays.asList("abc", "defg", "c", "asdf", "dafs", "ty");
list.stream().filter(e -> e.length() > 2).forEach(System.out::println);
```
2. `map`用于转换流，接收一个转换函数，产生一个流，它包含将转换函数应用于当前流中每个元素所产生的结果。
```java
Stream<String> stream = Stream.of("abc", "defg", "c", "asdf", "dafs", "ty");
Stream<Integer> integerStream = stream.map(e -> e.length());
```
3. `flat`将多个流摊平，合并成一个流，接收一个函数，这个函数对于当前流中的每个元素都产生一个流，然后将这些流合并成一个流
```java
Stream<String> stream = Stream.of("abc", "defg", "c", "asdf", "dafs", "ty");
Stream<String> stringStream = stream.flatMap(s -> Stream.of(s.split("")));
```
每个字符串会产生一个流，flatMap将这些流合并到一起 

4. `limit`，返回一个新流，其中包含当前流的前n个元素  
5. `skip`，跳过前n个元素  
6. `concat`，连接两个流
```java
Stream<String> stream1 = Stream.of("asdf", "dafs", "ty");
Stream<String> stream2 = Stream.of("abc", "defg", "c");
Stream<String> stream = Stream.concat(stream1, stream2);
```

7. `distinct`，返回一个流，包含当前流中的不同元素   
8. `sorted`，排序
```java
Stream<String> stream1 = Stream.of("asdf", "dafs", "ty", "abc", "defg", "c");
Stream<String> stream = stream1.sorted();
// 可以接收一个Comparator
stream = stream1.sorted(Comparator.comparing(String::length));
```

9. `peek`，产生一个流，与原来流中元素相同，但是在每次获取一个元素时，会调用一个函数（peek接收的函数）
```java
Stream<String> stream1 = Stream.of("asdf", "dafs", "ty", "abc", "defg", "c");
stream1.peek(System.out::println).toArray(String[]::new);
```
与`forEach`不同，`forEach`是终结操作，而`peek`不是，所以只有在最后调用终结操作时，才会为每个元素调用`peek`接收的函数  


10. `parallel`，产生一个并行流

#### 四. 终结操作
一个流只能有一个终结操作，当这个操作执行后，流就被使用“光”了，无法再被操作。所以这必定是流的最后一个操作。而且只有当这个操作开始的时候才开始真正地处理数据   
`max`  
`min`  
`findFirst`   
`findAny`   
`anyMatch`  
`allMatch`   
`noneMatch`  
`forEach`  
`forEachOrdered`   
`toArray`  
`collect`   
`reduce`  
`iterator`   
1. `max`，`min`使用给定的比较器找出最大最小元素
```java
Stream<String> stream1 = Stream.of("asdf", "dafs", "ty", "abc", "defg", "c");
Optional<String> max = stream1.max(Comparator.comparing(String::length));
```
2. `findFirst`，`findAny`找出这个流的第一个元素，找出这个流的任意一个元素
```java
Optional<String> max = stream1.findAny();
```
3. `anyMatch`，`allMatch`，`noneMatch`分别在这个流中的任意一个元素，所有元素，没有元素满足断言时返回`true`
```java
boolean flag = stream1.anyMatch(e -> e.length() > 10);
```
4. `forEach`，为流中的每个元素提供一个操作
5. `toArray`，获得由流中的元素构成的数组
```java
Stream<String> stream1 = Stream.of("asdf", "dafs", "ty", "abc", "defg", "c");
Object[] arr1 = stream1.toArray(); // t因为无法在运行时创建泛型数组
String[] arr2 = stream1.toArray(String[]::new);  // 返回给定类型的数组
```
6. `iterator`，产生一个用于获取当前流中各个元素的迭代器
7. `reduce`，从流中计算出某个值的通用机制
```java
Stream<Integer> stream = Stream.of(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
Optional<Integer> res = stream.reduce((a, b) -> a + b);
```
第一次计算将函数用于前两个元素，并将结果作为下一次计算的第一个参数，在第二次计算中和第三个元素一起计算，以此类推   
由于流可能是空的，所以返回一个`Optional`，避免无效结果，如果提供一个操作初始值，就可以不用返回`Optional`了，当流为空直接返回该初始值，同时这个初始值也参与计算
```java
Integer res = stream.reduce(0, Integer::sum);
```
对流中每个元素的某个属性进行计算
```java
Stream<String> stream1 = Stream.of("asdf", "dafs", "ty", "abc", "defg", "c");
// 字符串长度之和
Integer res = stream1.reduce(0,
        (total, word) -> total + word.length(), 
        (total1, total2) -> total1 + total2);
// s直接tream1.mapToInt(String::length).sum(); 会更方便些
```
#### 五. collect
接收一个`Collector`，用于收集流中的元素，可以使用`Collectors`类的工厂方法产生多种收集器
```java
Stream<String> stream1 = Stream.of("asdf", "dafs", "ty", "abc", "defg", "c");
List<String> list = stream1.collect(Collectors.toList());
```
1. `Collectors.toList()`，`Collectors.toSet()`将元素收集到一个列表或集合中
2. `Collectors.toCollection(Supplier<C> collectionFactory)`，收集到指定类型的集合类中，可以向其中传递一个集合类的构造器引用
```java
TreeSet<String> treeSet = stream1.collect(Collectors.toCollection(TreeSet::new));
```
3. `Collectors.joining()`连接流中字符串   
`Collectors.joining(CharSequence delimiter)`用给定字符串作为分隔符连接流中字符串
`Collectors.joining(CharSequence delimiter, CharSequence prefix,CharSequence suffix)`用给定字符串作为分隔符连接流中字符串，第一个字符串前可以有前缀，最后一个字符串后面可以有后缀
```java
String res = stream1.collect(Collectors.joining(", "));
```
4. `Collectors.summarizingInt(ToIntFunction<? super T> mapper)`可以产生一个`IntSummaryStatistics`对象的收集器，通过它可以获得将mapper应用于每个元素后所产生的结果的个数，总和，平均值，最大最小值
```java
IntSummaryStatistics summary = stream1.collect(Collectors.summarizingInt(String::length));
int max = summary.getMax(); // 在没有元素时产生Integer.MAX_VALUE
double ave = summary.getAverage();
long count = summary.getCount();
long sum = summary.getSum();
```
5. 收集到map中
```java
class User {
    private Integer id;
    private String name;

    public User(Integer id, String name) {
        this.id = id;
        this.name = name;
    }

    public Integer getId() {
        return id;
    }

    public String getName() {
        return name;
    }
}
```
已id为key，name为value
```java
Stream<User> stream = Stream.of(new User(1, "Tom"), new User(2, "Jack"), new User(3, "John"));
Map<Integer, String> map = stream.collect(Collectors.toMap(User::getId, User::getName));
```
已id为key，User对象自身为value
```java
Map<Integer, User> map = stream.collect(Collectors.toMap(User::getId, Function.identity()));
```
6. 群组和区分(map)与下游收集器 
```java
Stream<String> stream = Stream.of("asdf", "dafs", "ty", "abc", "defg", "c");
// 按照单词长度进行分组
Map<Integer, List<String>> map = stream.collect(Collectors.groupingBy(String::length)); // 为groupingBy提供一个分组函数
```
当分组函数是断言函数时，使用`partitioningBy`将元素分为key为true和false的两组会比使用`groupingBy`更高效     
`groupingBy`方法产生一个映射表，它的每个值都是一个列表，如果想要以某种方式来处理这些列表，就需要提供一个下游收集器  
```java
Map<Integer, Set<String>> map = stream.collect(Collectors.groupingBy(String::length, toSet())); // 以一个set来存放每个分组而不是默认的列表
```
```java
Map<Integer, Long> map = stream.collect(Collectors.groupingBy(String::length, counting())); // value为每个分组的元素个数
```
`counting()`，value为每个分组的元素数量   
`summingInt(ToIntFunction<? super T> mapper)`，value为将mapper应用到分组每个元素上之后产生的值的总和   
`maxBy(Comparator<? super T> comparator)`，value为每个分组中的最大元素   
`mapping(Function<? super T, ? extends U> mapper, Collector<? super U, A, R> downstream)`，产生一个收集器，先将mapper用于分组的每个元素，然后将mapper转换后的元素用downstream进行收集
```java
Stream<User> stream = Stream.of(new User(1, "Tom"), new User(2, "Jack"), new User(3, "John"));
Map<Integer, Optional<String>> map = stream.collect(Collectors.groupingBy(User::getId,
        mapping(User::getName, maxBy(String::compareTo))));
```
#### 六. 基本类型流
泛型无法应用于基本类型，所以直接对基本类型使用`Stream`会有装箱的开销   
提供了专门的类用来存储基本类型，比如`IntStream`，`LongStream`，`DoubleStream`等等   
与普通流一样可以直接使用静态方法`IntStream.of`，`Arrays.stream`，`IntStream.generate`，`IntStream.iterate`产生一个流   
对于`IntStream`和`LongStream`还可以使用`range`方法产生一个流  
```java
IntStream stream1 = IntStream.range(1, 10);  // [1, 10)
IntStream stream2 = IntStream.rangeClosed(1, 10); // [1, 10] 步长均为1
```
当有一个对象流时，可以通过`mapToInt`，`mapToLong`，`mapToDouble`将其转换为一个基本类型流
```java
Stream<String> stream = Stream.of("asdf", "dafs", "ty", "abc", "defg", "c");
stream.mapToInt(String::length);
```
`boxed`方法可以将一个基本类型流转换为其包装器类型的流
```java
Stream<Integer> stream = IntStream.range(1, 10).boxed();
```
`Random`对象的`ints`，`longs`，`doubles`方法可以返回有随机数构成的基本类型流
```java
Random random = new Random(47);
IntStream stream = random.ints();
```