### hash
> Redis hash 是一个string类型的field和value的映射表，hash特别适合用于存储对象。Redis 中每个 hash 可以存储 2^(32 - 1) 键值对（40多亿）。

* `hset key field value`：设置hash的fiel为指定值,如果key不存在,则先创建
* `hsetnx key field value`：只有在字段 field 不存在时，设置哈希表字段的值
* `hmset key field1 value1 [field2 value2 ...]`：同时对指定的key设置多个值
* `hget key field`：获取指定的hash指定field
* `hmget key field1 [field1 ...]`：获取指定的hash的多个field
* `hgetall key`：获取指定hash的所有field
* `hdel key field1 [field2 ..]`：删除一个或多个哈希表字段
* `hexists key field`：查看哈希表 key 中，指定的字段是否存在。  
* `hkeys key`：获取hash中的所有字段
* `hlen key`：获取hash中字段数量
* `hvals key`：获取哈希表中所有值
* `hincrby key field increment`：为哈希表 key 中的指定字段的整数值加上增量 increment 
* `hincrbyfloat key field increment`：为哈希表 key 中的指定字段的浮点数值加上增量 increment 
