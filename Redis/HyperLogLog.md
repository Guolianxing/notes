### HyperLogLog
* HyperLogLog 可以接受多个元素作为输入，并给出输入元素的基数估算值，即集合中不同元素的数量
* 算法给出的基数并不是精确的，可能会比实际稍微多一些或者稍微少一些，但会控制在合理的范围之内。
* 每个 HyperLogLog 键只需要花费 12 KB 内存，就可以计算接近 2^64 个不同元素的基 数。这和计算基数时，元素越多耗费内存就越多的集合形成鲜明对比。
* HyperLogLog 只会根据输入元素来计算基数，而不会储存输入元素本身，所以 HyperLogLog 不能像集合那样，返回输入的各个元素
* `pfadd key value1 [value2...]`：添加指定元素到 HyperLogLog 中
* `pfcount key1 [key2...]`：返回给定 HyperLogLog 的基数值，如果多个 HyperLogLog 则返回基数估值之和。
* `pfmerge destkey sourcekey1 [sourcekey2...]`：将多个 HyperLogLog 合并为一个 HyperLogLog