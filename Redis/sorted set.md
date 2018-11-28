### sorted set
> Redis 有序集合和集合一样也是string类型元素的集合,且不允许重复的成员。不同的是每个元素都会关联一个double类型的分数。redis正是通过分数来为集合中的成员进行从小到大的排序。有序集合的成员是唯一的,但分数(score)却可以重复。集合是通过哈希表实现的，所以添加，删除，查找的复杂度都是O(1)。 集合中最大的成员数为 2^(32 - 1) (4294967295, 每个集合可存储40多亿个成员)。

* `zadd key score1 value1 [score2 value2 ...]`：向有序集合添加一个或多个成员，或者更新已存在成员的分数
* `zcard key`：获取有序集合的成员数
* `zcount key min max`：计算在有序集合中指定区间分数的成员数
* `zincrby key increment value`：有序集合中对指定成员的分数加上增量 increment
* `zinterstore destination numkeys key1 [key2...]`：计算给定的一个或多个有序集的交集并将结果集存储在新的有序集合 key 中
* `zlexcount key min max`：有序集合中成员名称 min 和 max 之间的成员数量; Integer类型
    1. 有序集合中成员名称 min 和 max 之间的成员数量; Integer类型
    2. 可以使用 - 和 + 表示得分最小值和最大值
    3. min 和 max 不能反, max 放前面 min放后面会导致返回结果为0
    4. 计算成员之间的成员数量时,参数 min 和 max 的位置也计算在内
    5. min 和 max 参数的含义与 zrangebylex 命令中所描述的相同
* `zrange key start stop [withscores]`：通过索引区间返回有序集合成指定区间内的成员
* `zrangebylex key min max [limit offset count]`：通过字典区间返回有序集合的成员
* `zrangebyscore key min max`：通过分数返回有序集合指定区间内的成员
* `zrank key value`：返回有序集合中指定成员的索引
* `zrem key value1 [value2...]`：移除有序集合中的一个或多个成员
* `zremrangebylex key min max`：移除有序集合中给定的字典区间的所有成员
* `zremrangebyrank key start stop`：移除有序集合中给定的排名区间的所有成员
* `zremrangebyscore key min max`：移除有序集合中给定的分数区间的所有成员
* `zrevrange key start stop`：返回有序集中指定区间内的成员，通过索引，分数从高到底
* `zrevrangebyscore key max min`：返回有序集中指定分数区间内的成员，分数从高到低排序
* `zrevrank key value`：返回有序集合中指定成员的排名，有序集成员按分数值递减(从大到小)排序
* `zscore key value`：返回有序集中，成员的分数值
* `zunionstore destination numkeys key1 [key2...]`：计算给定的一个或多个有序集的并集，并存储在新的 key 中