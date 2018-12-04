### Spring boot读取配置文件
1. `@ConfigurationProperties`注解   
pom.xml
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-configuration-processor</artifactId>
    <optional>true</optional>
</dependency>
```
配置类的Bean，`@PropertySource`注解指明要绑定的配置文件的位置，不指定的话就是默认的`application.properties`，*即使指定了，如果在application.properties有相同的配置项的话，会被application.properties中的配置给覆盖掉*，`@ConfigurationProperties`指明配置项名称的前缀，该注解同时也支持`JSR-303`的注解校验配置的值是否合法，如下所示，age配置为非正数会在启动时报错:
```java
package com.example.config;

import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;
import org.springframework.stereotype.Component;

import java.util.List;
import java.util.Map;

@Component
@PropertySource(value = {"classpath:user.properties"})
@ConfigurationProperties(prefix = "user")
@Validated
public class UserConfig {
    private String name;
    @Positive
    private Integer age;
    private Map<String, Integer> maps;
    private List<String> lists;
    private Address address;

    ...getter and setter...

    public static class Address {
        private String province;
        private String city;

        ...getter and setter...
    }
}

```
user.properties
```shell
user.name=tom
user.age=24
user.maps.one=1
user.maps.two=2
user.lists=1,2,3
user.address.province=LN
user.address.city=DL
```
spring boot在启动时会扫描以下位置的application.properties或yml文件作为默认的配置文件：   
* `file:./config/`
* `file:./`
* `classpath:/config/`
* `classpath:/`
以上是按照优先级从高到低的顺序，虽然有优先级，但是所有位置的配置文件都会被加载，且都会生效，只不过对于相同的配置项是高优先级覆盖低优先级  

2. `@ImportResource`注解用于引入一个spring配置文件，比如在`resources`目录下新建一个配置bean的xml文件，默认并不会被spring boot加载，需要在启动类上或其他bean上用该注解把文件引入进来：   
bean.xml
```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:p="http://www.springframework.org/schema/p"
       xsi:schemaLocation="http://www.springframework.org/schema/beans http://www.springframework.org/schema/beans/spring-beans.xsd">

    <bean id="user" class="com.example.config.User" p:username="tom" p:age="12"/>

</beans>
```
```java
@ImportResource(locations = {"classpath:bean.xml"})
```
3. `@Import`注解用于将一个普通类声明为bean，如：`@Import(value = {User.class})`