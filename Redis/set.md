### set
> Redis 的 Set 是 String 类型的无序集合。集合成员是唯一的，这就意味着集合中不能出现重复的数据。Redis 中集合是通过哈希表实现的，所以添加，删除，查找的复杂度都是 O(1)。集合中最大的成员数为 232 - 1 (4294967295, 每个集合可存储40多亿个成员)。

* `sadd key value1 [value2...]`：向集合添加一个或多个成员
* `srem key value1 [value2...]`：从集合中移除一个或多个成员
* `smembers key`：返回给定集合中的所有成员
* `scard key`：获取集合的成员数
* `spop key`：移除并返回集合中的一个随机元素
* `smove source destination value`：将value从source移动到destination
* `sismember key value`：判断给定元素是否是集合 key 的成员
* `sdiff key1 [key2...]`：返回给定所有集合的差集
* `sdiffstore destination key1 [key2...]`：返回给定所有集合的差集并存储在 destination 中
* `sinter key1 [key2...]`：返回给定所有集合的交集
* `sinterstore key1 [key2...]`
* `sunion key1 [key2...]`：返回所有给定集合的并集
* `sunionstore destination key1 [key2...]`