### logback配置    
可在控制台实现与spring boot默认相同的配色以及日志按日期分割    
参考[https://blog.csdn.net/u012693119/article/details/79716306?utm_source=blogxgwz0](https://blog.csdn.net/u012693119/article/details/79716306?utm_source=blogxgwz0)
1. maven依赖
```xml
<!-- https://mvnrepository.com/artifact/ch.qos.logback/logback-classic -->
<dependency>
    <groupId>ch.qos.logback</groupId>
    <artifactId>logback-classic</artifactId>
    <version>1.2.3</version>
</dependency>
<!-- https://mvnrepository.com/artifact/org.slf4j/slf4j-api -->
<dependency>
    <groupId>org.slf4j</groupId>
    <artifactId>slf4j-api</artifactId>
    <version>1.7.25</version>
</dependency>
```
> slf4j译为简单日志门面，是日志框架的抽象。而log4j和logback是众多日志框架中的几种。所以，slf4j与logback的关系就像jpa和hibernate的关系，在写代码时可以调用slf4j的接口打日志，但底层配置需要配置其他日志框架
2. 颜色高亮转换器和获取进程id的转换器
HighlightingConverter
```java
package com.test.config;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.pattern.color.ANSIConstants;
import ch.qos.logback.core.pattern.color.ForegroundCompositeConverterBase;

public class HighlightingConverter extends ForegroundCompositeConverterBase<ILoggingEvent> {
    protected String getForegroundColorCode(ILoggingEvent event) {
        switch (event.getLevel().toInt()) {
            case Level.ERROR_INT:
                return ANSIConstants.RED_FG;
            case Level.WARN_INT:
                return ANSIConstants.YELLOW_FG;
            case Level.INFO_INT:
                return ANSIConstants.GREEN_FG;
            case Level.DEBUG_INT:
                return ANSIConstants.MAGENTA_FG;
            default:
                return null;
        }
    }
}
```
ProcessIdClassicConverter
```java
package com.test.config;

import ch.qos.logback.classic.pattern.ClassicConverter;
import ch.qos.logback.classic.spi.ILoggingEvent;

import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;

public class ProcessIdClassicConverter extends ClassicConverter {
    @Override
    public String convert(ILoggingEvent iLoggingEvent) {
        RuntimeMXBean runtime = ManagementFactory.getRuntimeMXBean();
        String name = runtime.getName();
        return name.substring(0, name.indexOf("@"));
    }
}
```
3. logback.xml
```xml
<?xml version="1.0" encoding="UTF-8" ?>
<configuration>
    <conversionRule conversionWord="pid" converterClass="com.test.config.ProcessIdClassicConverter"/>
    <conversionRule conversionWord="highlight" converterClass="com.test.config.HighlightingConverter"/>
    <conversionRule conversionWord="magenta" converterClass="ch.qos.logback.core.pattern.color.MagentaCompositeConverter"/>
    <conversionRule conversionWord="cyan" converterClass="ch.qos.logback.core.pattern.color.CyanCompositeConverter"/>
    <property name="CONSOLE_LOG_PATTERN" value="%d{yyyy-MM-dd HH:mm:ss.SSS} %highlight(%-5level) %magenta(%pid)  --- [%15.15thread] %cyan(%-40.40logger{39}) : %msg%n"/>
    <property name="FILE_LOG_PATTERN" value="%d{yyyy-MM-dd HH:mm:ss.SSS} %-5level %pid  --- [%15.15thread] %-40.40logger{39} : %msg%n"/>
    <property name="LOG_PATH" value="logs"/>
    <appender name="CONSOLE_LOG" class="ch.qos.logback.core.ConsoleAppender">
        <encoder>
            <pattern>${CONSOLE_LOG_PATTERN}</pattern>
            <charset>UTF-8</charset>
        </encoder>
    </appender>
    <appender name="FILE_LOG" class="ch.qos.logback.core.rolling.RollingFileAppender">
        <encoder>
            <pattern>${FILE_LOG_PATTERN}</pattern>
            <charset>UTF-8</charset>
        </encoder>
        <rollingPolicy class="ch.qos.logback.core.rolling.TimeBasedRollingPolicy">
            <fileNamePattern>${LOG_PATH}/%d{yyyy-MM-dd}.log</fileNamePattern>
        </rollingPolicy>
    </appender>
    <root level="INFO">
        <appender-ref ref="CONSOLE_LOG"/>
        <appender-ref ref="FILE_LOG"/>
    </root>
</configuration>
```