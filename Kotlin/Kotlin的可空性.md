### Kotlin的可空性

* Kotlin中的所有引用不能为null，否则会编译错误
* 如果引用允许为null，则应该声明时在类型后面加`?`
```kotlin
var str: String? = null

fun strLen(str: String?): Int = if (str != null) str.length else 0
```
* 如果一个引用允许为null，在其上调用方法时应该做判断，比如上面的`strLen`函数，否则如果直接调会编译错误(不一定非要判断，kotlin会根据上下文推断，只要保证调用时引用非空就可以，比如赋给一个非空值)

* 安全调用运算符 `?.`，把一次null检查和一次方法调用合并成一个操作，如果非空则调用方法，否则返回null，支持一个表达式中多个安全调用(a?.b?.c?.d)，下面这两种写法是等价的：
```kotlin
fun toUpper(str: String?): String? {
    return str?.toUpperCase()
}

fun toUpper(str: String?): String? = str?.toUpperCase()
```

* Elvis运算符 `?:`，`a ?: b` 等价于 `if (a != null) a else b`，经常和安全调用运算符一起使用：
```kotlin
fun strLen(str: String?): Int = if (str != null) str.length else 0

fun strLen(str: String?): Int = str?.length ?: 0

fun strLen(str: String?): Int = str?.length ?: throw IllegalArgumentException("str is null!")
```

* 安全类型转换`as?`，`a as? Type`等价于`if (a is Type) a as Type else null`，避免类型不正确时直接`as`转换的`ClassCastException`

* 非空断言`!!`，`a!!`等价于`if (a != null) a else throw NullPointException()`

* `let`函数，`let`函数接受一个lambda表达式作为入参，结合安全调用运算符，如果调用`let`函数的表达式的结果不为空，就会将这个结果作为lambda表达式的参数去执行，否则为空，什么也不会发生
```kotlin
// 一个接受非空参数的函数
fun out(str: String) {
    println(str)
}

fun main() {
    var str: String? = "abc"
    // 下面这两种调用方式等价
    if (str != null) {
        out(str)
    }
    
    str?.let{ out(it) }
}
```