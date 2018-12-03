### Spring boot中配置redis cache
1. pom.xml
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-cache</artifactId>
</dependency>
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-data-redis</artifactId>
</dependency>
<dependency>
    <groupId>org.apache.commons</groupId>
    <artifactId>commons-pool2</artifactId>
    <version>2.6.0</version>
</dependency>
<dependency>
    <groupId>com.esotericsoftware</groupId>
    <artifactId>kryo</artifactId>
    <version>4.0.2</version>
</dependency>
```
2. application.properties（正常配置redis就行）
```shell
spring.redis.database=0
spring.redis.host=
spring.redis.port=6379
spring.redis.password=
spring.redis.lettuce.pool.max-active=50
spring.redis.lettuce.pool.max-wait=-1
spring.redis.lettuce.pool.max-idle=20
spring.redis.lettuce.pool.min-idle=10
spring.redis.timeout=1000ms
```
3. 用redis做缓存时对可读性没什么要求，而存储空间和速度是比较重要的，可以使用`Kyro`进行序列化：
```java
package com.example.config;

import com.esotericsoftware.kryo.Kryo;
import com.esotericsoftware.kryo.io.Input;
import com.esotericsoftware.kryo.io.Output;
import org.springframework.data.redis.serializer.RedisSerializer;
import org.springframework.data.redis.serializer.SerializationException;

import java.io.ByteArrayOutputStream;

public class KryoRedisSerializer<T> implements RedisSerializer<T> {

    private static final byte[] EMPTY_BYTE_ARRAY = new byte[0];

    private static final ThreadLocal<Kryo> kryos = ThreadLocal.withInitial(Kryo::new);

    private Class<T> clazz;

    public KryoRedisSerializer(Class<T> clazz) {
        super();
        this.clazz = clazz;
    }

    @Override
    public byte[] serialize(T t) throws SerializationException {
        if (t == null) {
            return EMPTY_BYTE_ARRAY;
        }

        Kryo kryo = kryos.get();
        kryo.setReferences(false);
        kryo.register(clazz);

        try (ByteArrayOutputStream baos = new ByteArrayOutputStream();
             Output output = new Output(baos)) {
            kryo.writeClassAndObject(output, t);
            output.flush();
            return baos.toByteArray();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return EMPTY_BYTE_ARRAY;
    }

    @Override
    @SuppressWarnings("unchecked")
    public T deserialize(byte[] bytes) throws SerializationException {
        if (bytes == null || bytes.length <= 0) {
            return null;
        }

        Kryo kryo = kryos.get();
        kryo.setReferences(false);
        kryo.register(clazz);

        try (Input input = new Input(bytes)) {
            return (T) kryo.readClassAndObject(input);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }
}
```
4. 配置缓存管理器：
```java
package com.example.config;

import org.springframework.context.annotation.*;
import org.springframework.data.redis.cache.RedisCacheConfiguration;
import org.springframework.data.redis.cache.RedisCacheManager;
import org.springframework.data.redis.connection.RedisConnectionFactory;
import org.springframework.data.redis.serializer.*;

import java.time.Duration;
import java.util.*;

@Configuration
public class CacheConfig {
    @Bean
    public RedisCacheManager redisCacheManager(RedisConnectionFactory factory) {
        RedisCacheConfiguration configuration = RedisCacheConfiguration.defaultCacheConfig()
                .serializeKeysWith(RedisSerializationContext.SerializationPair.fromSerializer(new StringRedisSerializer()))
                .serializeValuesWith(RedisSerializationContext.SerializationPair.fromSerializer(new KryoRedisSerializer<>(Object.class)));
        Set<String> cacheNames = new HashSet<>();
        cacheNames.add("cache1");
        cacheNames.add("cache2");
        Map<String, RedisCacheConfiguration> configurationMap = new HashMap<>();
        configurationMap.put("cache1", configuration.entryTtl(Duration.ofHours(1L)).prefixKeysWith("cache1:"));
        configurationMap.put("cache2", configuration.entryTtl(Duration.ofHours(2L)).prefixKeysWith("cache2"));

        RedisCacheManager cacheManager = RedisCacheManager.builder(factory)
                .initialCacheNames(cacheNames)
                .withInitialCacheConfigurations(configurationMap)
                .transactionAware()
                .build();
        return cacheManager;
    }
}
```
5. 启动类上开启缓存注解：`@EnableCaching`