### Spring boot中解决跨域的几种方式（CORS)
1. 使用过滤器`CorsFilter`
```java
@Configuration
public class CorsConfig {

    @Bean
    public CorsFilter corsFilter() {
        // 1.添加CORS配置信息
        CorsConfiguration corsConfiguration = new CorsConfiguration();
        // 放行哪些原始域
        corsConfiguration.addAllowedOrigin("*");
        // 是否发送Cookie信息
        corsConfiguration.setAllowCredentials(true);
        // 放行哪些原始域(请求方式)
        corsConfiguration.addAllowedMethod("*");
        // 放行哪些原始域(头部信息)
        corsConfiguration.addAllowedHeader("*");
        // 暴露哪些头部信息（因为跨域访问默认不能获取全部头部信息）
//        corsConfiguration.addExposedHeader("*");  不能直接用*
        // 2.添加映射路径
        UrlBasedCorsConfigurationSource configurationSource = new UrlBasedCorsConfigurationSource();
        configurationSource.registerCorsConfiguration("/**", corsConfiguration);
        // 3.返回新的CorsFilter.
        return new CorsFilter(configurationSource);
    }
}
```
2. 重写`WebMvcConfigurer`
```java
@Configuration
public class MvcConfig implements WebMvcConfigurer {

    @Override
    public void addCorsMappings(CorsRegistry registry) {
        registry.addMapping("/**")
                .allowedOrigins("*")
                .allowCredentials(true)
                .allowedMethods("GET", "POST")
                .allowedHeaders("*");
    }
}
```
3. `@CrossOrigin`注解，可以用于controller上或`@RequestMapping`的方法上 
```java
@RestController
@RequestMapping("/api")
@CrossOrigin(origins = "*", maxAge = 3600)
public class TestController {

}
```