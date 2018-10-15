### Spring data jpa使用utf8mb4建表

```java
package com.test.config;

import org.hibernate.dialect.MySQL57Dialect;

public class MySQL5InnoDBDialectUtf8mb4 extends MySQL57Dialect {

    @Override
    public String getTableTypeString() {
        return "ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE utf8mb4_general_ci";
    }
}
```
添加配置:
```text
spring.jpa.properties.hibernate.dialect=com.test.config.MySQL5InnoDBDialectUtf8mb4
```
> 在网上的例子都是去继承`MySQL5InnoDBDialect`这个类，但是这个类被标记为过时，因为从MySQL5.5开始就默认使用InnoDB引擎，所以Hibernate作者认为Dialect分为两类就没有什么必要了。