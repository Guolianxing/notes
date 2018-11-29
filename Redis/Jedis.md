### Jedis
```xml
<dependency>
    <groupId>redis.clients</groupId>
    <artifactId>jedis</artifactId>
    <version>2.9.0</version>
</dependency>
```
```java
package redis;

import redis.clients.jedis.Jedis;
import redis.clients.jedis.JedisPool;
import redis.clients.jedis.JedisPoolConfig;

public class JedisUtil {

    private static final String redis_host = "host";
    private static final String redis_password = "password";
    private static JedisPool jedisPool;

    static {
        JedisPoolConfig config = new JedisPoolConfig();
        // 设置最大连接数量
        config.setMaxTotal(50);
        jedisPool = new JedisPool(config, redis_host);
    }

    public static Jedis getJedis() {
        Jedis jedis = jedisPool.getResource();
        jedis.auth(redis_password);
        return jedis;
    }


    public static void main(String[] args) {
        try (Jedis jedis = getJedis()) {
            System.out.println(jedis.ping());
            jedis.set("name", "tom");
            String name = jedis.get("name");
            System.out.println(name);
        }
    }
}
```