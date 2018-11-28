### 对key的操作
> Redis key值是二进制安全的，这意味着可以用任何二进制序列作为key值，从形如”foo”的简单字符串到一个JPEG文件的内容都可以。空字符串也是有效key值。

* `set key value`  
* `setnx key value`：如果key已经存在则不执行插入操作  
* `mset k1 v1 [k2 v2 ...]`：一次性存储多个kv  
* `msetnx k1 v1 [k2 v2 ...]`：如果里面有一个key是已经存在的,那么不会执行任何的插入动作   
* `get key`   
* `getset key value`：为给定的key设置新的值，并返回旧的值，当key不存在时，返回nil   
* `mget key1 [key2...]`：获取多个给定的key的值，返回一个列表   
* `del key1 [key2...]`：删除给定的key   
* `exists key`：检查给定的key是否存在   
* `expire key seconds`：为给定key设置过期时间，单位：秒  
* `pexpire key millseconds`：为给定key设置过期时间，单位：毫秒  
* `keys pattern`：查找所有符合模式的key，如`keys *`查看所有key   
* `persist key`: 移除 key 的过期时间，key 将持久保持。    
* `ttl key`：以秒为单位，返回给定 key 的剩余生存时间(TTL, time to live)。  
* `pttl key`：以毫秒为单位返回 key 的剩余的过期时间。   
* `rename key newkey`：修改Key名称，若newkey已经存在，会被删除，然后将key改名为newkey
* `renamenx key newkey`：当新key不存在时，将Key改名为newkey   
* `type key`：返回key所存储值的类型  
* `dbsize`：返回当前数据库的key数量  
* `flushdb`：清空当前数据库中的所有key  
* `flushall`：清空所有数据库中的所有key   

set 命令还支持可选的NX选项和XX选项：
* 如果添加了NX选项,那么set操作的时候会判断key值是否存在,如果存在那么不会做覆盖操作.如果不存在,那么就会正常存入
* 如果添加了XX选项,那么set操作的时候会判断key值是否存在,如果存在,那么就执行操作进行覆盖,如果不存在,那么不会执行任何操作
如：
```shell
set name tom nx
set name tom xx
```
