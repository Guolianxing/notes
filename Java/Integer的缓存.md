### Integer的缓存
1. `Byte`、`Short`、`Integer`、`Long`、`Character`类都具有缓存机制，由相应的类中的一个静态内部类实现。
2. 缓存范围：`Byte`、`Short`、`Integer`、`Long`是[-128, 127]，`Character`是[0, 127]
3. 除了`Integer`可以指定缓存范围，其它类都不行，`Integer`类可以通过JVM参数`-XX:AutoBoxCacheMax=size`来指定缓存上界，如果指定值比127小，则取127，如果比127大，则取指定值与`Integer.MAX_VALUE - (-low) -1`的最小值(考虑到数组长度最大为Integer.MAX_VALUE，而且还要缓存128个负数)
4. `Boolean`也有缓存，只不过不是用数组，而是直接两个变量实现的：
    ```java
    public static final Boolean TRUE = new Boolean(true);

    public static final Boolean FALSE = new Boolean(false);
    ```
5. 当调用这些类的`valueOf`方法，或发生自动装箱时，缓存会生效，直接`new`不生效    
`Integer`中的缓存类：
```java
@HotSpotIntrinsicCandidate
public static Integer valueOf(int i) {
    if (i >= IntegerCache.low && i <= IntegerCache.high)
        return IntegerCache.cache[i + (-IntegerCache.low)];
    return new Integer(i);
}

private static class IntegerCache {
    static final int low = -128;
    static final int high;
    static final Integer cache[];

    static {
        // high value may be configured by property
        int h = 127;
        // 获取虚拟机参数并解析为整数
        String integerCacheHighPropValue =
            VM.getSavedProperty("java.lang.Integer.IntegerCache.high");
        if (integerCacheHighPropValue != null) {
            try {
                int i = parseInt(integerCacheHighPropValue);
                i = Math.max(i, 127);
                // Maximum array size is Integer.MAX_VALUE
                h = Math.min(i, Integer.MAX_VALUE - (-low) -1);
            } catch( NumberFormatException nfe) {
                // If the property cannot be parsed into an int, ignore it.
            }
        }
        high = h;

        cache = new Integer[(high - low) + 1];
        int j = low;
        for(int k = 0; k < cache.length; k++)
            cache[k] = new Integer(j++);

        // range [-128, 127] must be interned (JLS7 5.1.7)
        assert IntegerCache.high >= 127;
    }

    private IntegerCache() {}
}
```
```java
public static void main(String[] args) {
    Integer a = new Integer(3);
    Integer b = new Integer(3);
    System.out.println(a == b);   // false 直接new不使用缓存

    a = 3; b = 3;
    System.out.println(a == b);   // true 自动装箱，使用缓存

    a = 128; b = 128;
    System.out.println(a == b);   // false 超出缓存范围，无法使用缓存的对象，而是每次新建一个对象

    a = Integer.valueOf(127);
    b = Integer.valueOf(127);
    System.out.println(a == b);   // true valueOf方法，使用缓存

    a = Integer.valueOf(128);
    b = Integer.valueOf(128);
    System.out.println(a == b);   // false 超出缓存范围
}
```