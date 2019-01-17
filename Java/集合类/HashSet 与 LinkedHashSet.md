### HashSet
`HashSet`是利用`HashMap`来实现的，只使用了`Entry`中的`key`而已，以一个静态的对象`PRESENT`作为所有`key`对应的`value`。     
`LinkedHashSet`继承自`HashSet`，`HashSet`有一个带有`dummy`参数的构造器：
```java
HashSet(int initialCapacity, float loadFactor, boolean dummy) {
    map = new LinkedHashMap<>(initialCapacity, loadFactor);
}
```
`dummy`并没什么意义，`LinkedHashSet`所有的构造器都会调用`HashSet`中的这个构造器，从而将底层变为用`LinkedHashMap`实现。