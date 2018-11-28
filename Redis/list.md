### list
> Redis列表是简单的字符串列表，按照插入顺序排序。你可以添加一个元素到列表的头部（左边）或者尾部（右边）。一个列表最多可以包含 2^(32 - 1) 个元素 (4294967295, 每个列表超过40亿个元素)。

* `lpush key value1 [value2...]`，`rpush key value1 [value2...]`：将一个或多个值push到列表左部/右部
* `lpushx key value`，`rpushx key value`：将一个值插入到一个已经存在的列表的左部/右部
* `lpop key`，`rpop key`：移出并获取列表最左/最有的元素
* `lindex key index`：通过索引获取列表中的元素
* `llen key`：获取列表长度
* `lrange key start stop`：获取列表指定范围内的元素（可以使用负数下标，以 -1 表示列表的最后一个元素， -2 表示列表的倒数第二个元素，以此类推）
   * `lrange key 0 -1`查看所有列表元素
* `lrem key count value`：根据参数 COUNT 的值，移除列表中与参数 VALUE 相等的元素
   1. count > 0 : 从表头开始向表尾搜索，移除与 VALUE 相等的元素，数量为 COUNT
   2. count < 0 : 从表尾开始向表头搜索，移除与 VALUE 相等的元素，数量为 COUNT 的绝对值
   3. count = 0 : 移除表中所有与 VALUE 相等的值
* `ltrim key start stop`：让列表只保留指定区间内的元素，不在指定区间之内的元素都将被删除
* `lset key index value`：通过索引设置列表元素的值
* `linsert key before|after pivot value`：将值 value 插入到列表 key 当中，位于值 pivot 之前或之后
