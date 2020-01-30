### lambda中的return

现在有一个函数`f`，接受一个函数`action`：
```kotlin
fun f(action: (x) -> Unit) {

}
```
1. `f`非内联
    * `action`为`lambda`表达式，则`action`中只支持带标签的`return`，从`lambda`表达式中返回
    * `action`为匿名函数，支持`return`，表示从匿名函数中返回到`f`
    * 上面两个都为`局部返回`
2. `f`内联
    * `action`为`lambda`表达式
        * 带标签`return`，同非内联为局部返回
        * 直接`return`，则是从调用`f`的函数返回
    * `action`为匿名函数同非内联为局部返回

```kotlin
fun <T> List<T>.each(process: (T) -> Unit): Unit {
    for (x in this) {
        process(x)
    }
}

inline fun <T> List<T>.inlineEach(process: (T) -> Unit): Unit {
    for (x in this) {
        process(x)
    }
}

fun test1(arr: List<Int>) {
    arr.each {
        if (it == 3) {
            return@each
        }
        println(it)
    }
}

fun test2(arr: List<Int>) {
    arr.each(fun(x: Int) {
        if (x == 3) {
            return
        }
        println(x)
    })
}

fun test3(arr: List<Int>) {
    arr.inlineEach {
        if (it == 3) {
            return@inlineEach
        }
        println(it)
    }
}

fun test4(arr: List<Int>) {
    arr.inlineEach {
        if (it == 3) {
            return
        }
        println(it)
    }
}

fun test5(arr: List<Int>) {
    arr.inlineEach(fun(x: Int) {
        if (x == 3) {
            return
        }
        println(x)
    })
}

fun main() {
    val arr = listOf(1, 2, 3, 4, 5)
    println("test1")
    test1(arr)
    println("test2")
    test2(arr)
    println("test3")
    test3(arr)
    println("test4")
    test4(arr)
    println("test5")
    test5(arr)
}
```
结果：
```txt
test1
1
2
4
5
test2
1
2
4
5
test3
1
2
4
5
test4
1
2
test5
1
2
4
5
```