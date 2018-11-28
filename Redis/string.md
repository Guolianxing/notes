### string
* `getrange key start end`：返回key中字符串值的子串，下标范围是`[start, end]`  
* `setrange key offset value`：用 value 参数覆写给定 key 所储存的字符串值，从偏移量 offset 开始  
* `strlen key`：返回指定key的value的长度   
* `incr key`：   
    1. 将key中存储的值加1
    2. 如果 key 不存在，那么 key 的值会先被初始化为 0 ，然后再执行加1操作。
    3. 如果值包含错误的类型，或字符串类型的值不能表示为数字，那么返回一个错误。
    4. 本操作的值限制在 64 位(bit)有符号数字表示之内。
* `incrby key increment`：将 key 所储存的值加上给定的增量值  
* `incrbyfloat key increment`：将 key 所储存的值加上给定的浮点增量值  
* `decr key`：将 key 中储存的数字值减一
* `decrby key decrement`：key 所储存的值减去给定的减量值  
* `append key value`：
    1. 如果 key 已经存在并且是一个字符串， APPEND 命令将 value 追加到 key 原来的值的末尾
    2. 如果 key 不存在， APPEND 就简单地将给定 key 设为 value ，就像执行 SET key value 一样