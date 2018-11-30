### Spring中配置Jpa与事务
pom.xml
```xml
<dependency>
    <groupId>org.springframework</groupId>
    <artifactId>spring-orm</artifactId>
    <version>4.3.18.RELEASE</version>
</dependency>
<dependency>
    <groupId>org.springframework.data</groupId>
    <artifactId>spring-data-jpa</artifactId>
    <version>1.10.5.RELEASE</version>
</dependency>
<dependency>
    <groupId>com.zaxxer</groupId>
    <artifactId>HikariCP</artifactId>
    <version>3.2.0</version>
</dependency>
<dependency>
    <groupId>mysql</groupId>
    <artifactId>mysql-connector-java</artifactId>
    <version>8.0.13</version>
</dependency>
<dependency>
    <groupId>org.hibernate</groupId>
    <artifactId>hibernate-core</artifactId>
    <version>5.2.10.Final</version>
</dependency>
<dependency>
    <groupId>org.hibernate</groupId>
    <artifactId>hibernate-entitymanager</artifactId>
    <version>5.2.10.Final</version>
</dependency>
```
jdbc.properties
```shell
dataSource.user=root
dataSource.password=g123456
jdbcUrl=jdbc:mysql://localhost:3306/hikaricptest?useUnicode=true&characterEncoding=utf8&serverTimezone=UTC
dataSource.cachePrepStmts=true
dataSource.prepStmtCacheSize=250
dataSource.prepStmtCacheSqlLimit=2048
dataSource.useServerPrepStmts=true
dataSource.useLocalSessionState=true
dataSource.rewriteBatchedStatements=true
dataSource.cacheResultSetMetadata=true
dataSource.cacheServerConfiguration=true
dataSource.elideSetAutoCommits=true
dataSource.maintainTimeStats=false
dataSource.minimumIdle=10
dataSource.maximumPoolSize=30
driverClassName=com.mysql.cj.jdbc.Driver
```
application.xml
```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:aop="http://www.springframework.org/schema/aop"
       xmlns:context="http://www.springframework.org/schema/context" xmlns:p="http://www.springframework.org/schema/p"
       xmlns:tx="http://www.springframework.org/schema/tx" xmlns:jpa="http://www.springframework.org/schema/data/jpa"
       xsi:schemaLocation="http://www.springframework.org/schema/beans http://www.springframework.org/schema/beans/spring-beans.xsd http://www.springframework.org/schema/aop http://www.springframework.org/schema/aop/spring-aop.xsd http://www.springframework.org/schema/context http://www.springframework.org/schema/context/spring-context.xsd http://www.springframework.org/schema/cache http://www.springframework.org/schema/cache/spring-cache.xsd http://www.springframework.org/schema/tx http://www.springframework.org/schema/tx/spring-tx.xsd http://www.springframework.org/schema/data/jpa http://www.springframework.org/schema/data/jpa/spring-jpa.xsd">

    <!-- 开启aop自动代理-->
    <aop:aspectj-autoproxy/>
    <!-- 扫描所有注解定义的bean-->
    <context:component-scan base-package="com.test.*"/>

    <!--配置数据源-->
    <bean id="hiKariConfig" class="com.zaxxer.hikari.HikariConfig">
        <constructor-arg value="/jdbc.properties"/>
    </bean>
    <bean id="dataSource" class="com.zaxxer.hikari.HikariDataSource">
        <constructor-arg ref="hiKariConfig"/>
    </bean>

    <!--Hibernate对Jpa的实现-->
    <bean id="hibernateJpaVendorAdapter"
          class="org.springframework.orm.jpa.vendor.HibernateJpaVendorAdapter">
        <property name="databasePlatform" value="org.hibernate.dialect.MySQL57Dialect"/>
    </bean>

    <!--实体管理器工厂-->
    <bean id="entityManagerFactory"
          class="org.springframework.orm.jpa.LocalContainerEntityManagerFactoryBean">
        <!--指定数据源-->
        <property name="dataSource" ref="dataSource"/>
        <!--指定特定的Jpa实现-->
        <property name="jpaVendorAdapter" ref="hibernateJpaVendorAdapter"/>
        <!--配置实体类包路径-->
        <property name="packagesToScan" value="com.test.entity"/>
        <property name="jpaProperties">
            <props>
                <prop key="hibernate.ejb.naming_strategy">org.hibernate.cfg.ImprovedNamingStrategy</prop>
                <prop key="hibernate.show_sql">false</prop>
                <prop key="hibernate.format_sql">false</prop>
                <prop key="hibernate.dialect">org.hibernate.dialect.MySQL57Dialect</prop>
            </props>
        </property>
    </bean>

    <!--启用扫描并自动创建代理的功能-->
    <jpa:repositories base-package="com.test.repository"
                      entity-manager-factory-ref="entityManagerFactory"
                      transaction-manager-ref="transactionManager"/>

    <!--Jpa事务管理器-->
    <bean id="transactionManager"
          class="org.springframework.orm.jpa.JpaTransactionManager"
          p:entityManagerFactory-ref="entityManagerFactory"/>

    <!--基于注解的事务，会自动使用名为transactionManager的事务管理器-->
    <tx:annotation-driven/>

</beans>
```
Jpa事务管理器配置思路：要先提供一个数据源，然后配置一个`EntityManagerFactory`，最后配置`JpaTransactionManager`