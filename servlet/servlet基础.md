### Servlet 基础
##### 一. 
&emsp;&emsp;一个Servlet程序就是一个在Web服务器端运行的特殊Java类，这个特殊的类必须实现javax.servlet.Servlet接口，Servlet接口定义了Servlet容器与Servlet程序之间的协议约定，且Servlet程序的运行离不开Servlet容器。Servlet API中提供了一个实现了Servlet的最简单的Servlet类，javax.servlet.GenericServlet，实现了Servlet程序的基本特征和功能。Servlet API中还提供了一个专门用于HTTP协议的Servlet类，javax.servlet.http.HttpServlet，它是GenericServlet的子类，在GenericServlet类的基础上进行了一些针对HTTP特点的扩充。

&emsp;&emsp;要编写一个Servlet类，只要这个类继承GenericServlet类或HttpServlet类就可以了，为了充分利用HTTP协议的功能，在一般情况下，应该继承HttpServlet类，而不是GenericServlet类。

#### 二. Servlet的注册
1. 使用web.xml注册
```xml
    <servlet>
        <servlet-name>TestServlet</servlet-name>
        <servlet-class>com.test.TestServlet</servlet-class>
    </servlet>
    <servlet-mapping>
        <servlet-name>TestServlet</servlet-name>
        <url-pattern>/test/testServlet</url-pattern>
    </servlet-mapping>
```
* <url-pattern> 子元素中的访问路径必须以`/`开头，`/`代表当前Web应用程序的根目录，且路径名可以是多级目录的形式。
* Servlet路径映射的优先级高于Web应用所映射的本地文件系统中的文件
* 同一个Servlet可以被映射到多个URL上，即多个<servlet-mapping>元素的<servlet-name>子元素的设置值可以是同一个Servlet的注册名。
* 在Servlet映射的URL中可以使用`*`通配符，但是只能有两种格式
    * `*.拓展名` 在`*`前面不能有目录分隔符`/`，例如: `*.do`表示匹配所有以`.do`结尾的URL，`/action/*.do`是无效的映射路径。
    * 以正斜杠`/`开头并以`/*`结尾，例如: `/*`匹配当前Web应用程序下所有URL，`action/*`匹配当前Web应用程序下的`/action`子路径下的所有URL。
    * 匹配原则：采用最具体匹配原则来查找请求的URL的映射， 其中`*.拓展名`的匹配优先级最低。

2. 使用@WebServlet注解  
Servlet3.0支持@WebServlet注解，将一个继承HttpServlet的类标注为可以处理用户请求的Servlet。

|常用属性名|描述|
|------|---|
|name|Servlet名称|
|value|Servlet的访问路径映射|
|urlPatterns|等价于value，且二者不能同时存在|
|loadOnStartup|指定加载顺序，默认为-1|
|initParams|指定一组初始化参数，对应<init-param>标签|
|asyncSupported|声明是否支持异步操作模式|

```java
@WebServlet(
        name = "HelloServlet", 
        urlPatterns = {"/hello1", "/hello2"}, 
        initParams = {@WebInitParam(name = "one", value = "first"),
                        @WebInitParam(name = "two", value = "second")},
        loadOnStartup = 1)
public class HelloServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

    }
}

```

#### 三. 缺省Servlet
&emsp;&emsp;如果某个Servlet的映射路径仅仅为一个正斜杠`/`，那么这个Servlet就成为当前Web应用程序的缺省Servlet，凡是在web.xml文件中找不到匹配的<servlet-mapping>元素的URL，它们的访问请求都将交给缺省Servlet处理，也就是说，缺省Servlet用于处理所有其他Servlet都不处理的访问请求。在tomcat的安装目录\conf\web.xml文件中注册了一个缺省Servlet：
```xml
    <servlet>
        <servlet-name>default</servlet-name>
        <servlet-class>org.apache.catalina.servlets.DefaultServlet</servlet-class>
        <init-param>
            <param-name>debug</param-name>
            <param-value>0</param-value>
        </init-param>
        <init-param>
            <param-name>listings</param-name>
            <param-value>false</param-value>
        </init-param>
        <load-on-startup>1</load-on-startup>
    </servlet>
    <servlet-mapping>
        <servlet-name>default</servlet-name>
        <url-pattern>/</url-pattern>
    </servlet-mapping>
```
&emsp;&emsp;当访问Tomcat服务器中的某个静态HTML文件和图片时，实际上是在访问这个缺省Servlet，而这个缺省Servlet的处理方式通常是把静态资源中的内容按字节原封不动地读出来，然后再按字节流原封不动地传递给客户端，并生成一些响应消息头字段，例如，根据静态资源的扩展名所映射的MIME类型生成Content-Type头字段，根据静态资源的大小生成Content-Length头字段。

#### 四. Servlet运行过程
Servlet容器按照下面的过程来调用一个Servlet程序：
1. 接收到访问某个Servlet的HTTP请求之后，Servlet容器首先检查是否已经装载并创建了该Servlet的实例对象，如果已经装载并创建，直接执行第四步，否则，执行第二步。
2. 装载并创建Servlet的一个实例对象。
3. 调用Servlet对象的init()方法，以便执行Servlet的一些初始化操作。
4. 创建一个用于封装HTTP请求消息的HttpServletRequest对象和一个代表HTTP响应消息的HttpServletResponse对象，然后调用Servlet的service()方法并将请求和响应对象作为参数传递进去。
5. 在Web程序被停止或重启之前，Servlet容器将卸载其中运行的Servlet。在卸载Servlet之前，容器将调用Servlet的destroy()方法，以便执行一些清理工作，如释放一些被改Servlet占用的资源。在卸载某个Servlet之后，该Servlet实例对象编程垃圾，等待JVM的垃圾收集器将其从内存中清除。

&emsp;&emsp;在<servlet>中可以嵌套一个<load-on-startup>元素，用于指定Servlet被装载的时机和顺序，如果该元素内容被设置为0或一个正整数，它指定该Servlet在Web应用程序启动时就被实例化和调用它的init()方法，且这个数字越小，Servlet被装载的时间也越早，相同数字的Servlet之间的装载顺序由Servlet容器自己决定。如果为负数，则由Servlet容器自己决定什么时候装载。该元素中数值的大小可用来解决Servlet之间的以来关系。

#### 四. ServletConfig接口
&emsp;&emsp;Servlet容器将代表Servlet容器和Servlet的配置参数信息一并封装到一个ServletConfig对象中，并在初始化Servlet实例对象的时候传递给该Servlet。ServletConfig接口则定义了ServletConfig对象需要对外提供的方法，以便在Servlet程序中可以调用这些方法来获取有关信息。
&emsp;&emsp;Servlet容器装载Servlet并创建该Servlet的一个实例对象后，会调用该实例对象的init(ServletConfig config)方法将ServletConfig对象传递给该Servlet，而Servlet接口中定义的getServletConfig()方法则返回init(ServletConfig config)方法传进来的这个ServletConfig对象的引用。在GenericServlet类中，为了简化代码，定义了一个ServletConfig类型的成员变量，用来存储init方法传进来的ServletConfig对象，同时，GenericServlet还实现了ServletConfig接口，在实现的ServletConfig接口方法的内部调用ServletConfig对象的同名方法：
```java
public String getServletName() {
    return this.config.getServletName();
}
```
&emsp;&emsp;GenericServlet内部还重载了一个没有入参的init方法，该方法内没有程序处理代码，并在Servlet接口中定义的init(ServletConfig config)中调用这个方法，省去了我们在编写自己的Servlet程序时重写init(ServletConfig config)中总要加一句super.init(config)的麻烦，所以，我们只需自己实现init()方法就可以了：
```java
public void init(ServletConfig config) throws ServletException {
    this.config = config;
    this.init();
}
public void init() throws ServletException{

}
```
ServletConfig中的方法：
* getInitParameterNames方法：在web.xml文件中可以为Servlet设置若干个初始化参数，此方法可以反悔一个Enumeration集合对象，该集合对象中包含在web.xml文件中为当前Servlet设置的所有初始化参数的名称，即<init-param>子元素中的<param-name>节点中的值。
* getInitParameter方法，返回<init-param>元素中<param-name>对应的<param-value>中的值，若指定参数不存在，返回NULL。
* getServletName方法，返回Servlet注册名称。
* getServletContext方法，返回该web应用的ServletContext对象的引用。

web.xml
```xml
    <servlet>
        <servlet-name>TestServlet</servlet-name>
        <servlet-class>com.test.TestServlet</servlet-class>
        <init-param>
            <param-name>first</param-name>
            <param-value>one</param-value>
        </init-param>
        <init-param>
            <param-name>second</param-name>
            <param-value>two</param-value>
        </init-param>
    </servlet>
    <servlet-mapping>
        <servlet-name>TestServlet</servlet-name>
        <url-pattern>/test/testServlet</url-pattern>
    </servlet-mapping>
```
```java
package com.test;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.IOException;
import java.util.Enumeration;

public class TestServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        ServletConfig config = getServletConfig();
        Enumeration<String> enumeration = config.getInitParameterNames();
        while(enumeration.hasMoreElements()) {
            String key = enumeration.nextElement();
            String value = getInitParameter(key);
            System.out.println(key + ": " + value);
        }
        System.out.println("ServletName: " + getServletName());
        ServletContext context = getServletContext();
        System.out.println(context.getRealPath("/"));
    }
}
```
#### 五. Servlet中的一些其他的方法
&emsp;&emsp;HttpServlet类继承GenericServlet类，GenericServlet实现了Servlet，ServletConfig，Serializable这三个接口。
1. init方法和destory方法，如上面所说，在初始化Servlet实例对象时和容器卸载Servlet之前分别调用这两个方法。
2. getServletInfo方法：用于返回Servlet的描述信息，Servlet容器要获得某个Servlet的描述信息时将调用这个方法。
3. service方法：是Servlet的核心方法，当针对某个Servlet的访问请求到达时，Servlet容器将调用该Servlet的service方法，Servlet接口中定义的该方法为：

```java
public void service(ServletRequest req, ServletResponse res) throws ServletException, IOException;
```
GenericServlet中没有此方法进行实现，HttpServlet中实现了这个方法，同时还重载了一个接受HttpServletRequset和HttpServletResponse类型参数的方法，在原方法中将入参转化为这两个类型，并调用重载的service方法，同时将类型转换后的请求和响应对象传递给此方法：

```java
protected void service(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
    ...
}

public void service(ServletRequest req, ServletResponse res) throws ServletException, IOException {
        HttpServletRequest request;
        HttpServletResponse response;
        try {
            request = (HttpServletRequest)req;
            response = (HttpServletResponse)res;
        } catch (ClassCastException var6) {
            throw new ServletException("non-HTTP request or response");
        }

        this.service(request, response);
    }
```
4. doXXX方法：在HttpServlet中定义了针对HTTP协议中的各种请求方式的doXXX方法，并由3中重载的service方法根据请求类型调用相对应的doXXX方法，比如，收到GET请求就调用doGet方法。注意，如果我们自己编写的继承HttpServlet的类中重写了service方法，且没有用super.service()来显式调用父类中的service方法，doXXX方法将不会被调用。

#### 六. ServletContext接口
&emsp;&emsp;每个web应用程序分别用一个ServletContext对象来表示，ServletContext接口定义了ServletContext对象需要对外提供的方法，Servlet程序通过这些方法与Servlet容器进行通信，Servlet容器为每个weba应用程序创建一个对应的ServletContext对象。
1. 获取ServletContext
    * ServletConfig.getServletContext()，由于GenericServlet实现了ServletConfig接口，所以可以在Servlet中调用getServletContext()方法。
    * Servlet3.0中为HttpServletRequest对象增加了getServletContext()方法。

2. 获取web应用程序的初始化参数   
为web应用设置初始化参数，需要在`<web-app>`中添加`<context-param>`元素，其余用法同ServletConfig对象获取Servlet参数：
```xml
    <context-param>
        <param-name>one</param-name>
        <param-value>first</param-value>
    </context-param>
    <context-param>
        <param-name>two</param-name>
        <param-value>first</param-value>
    </context-param>
```
```java
public class TestServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        ServletContext context = getServletContext();
        Enumeration<String> params = context.getInitParameterNames();
        while(params.hasMoreElements()) {
            String key = params.nextElement();
            String value = context.getInitParameter(key);
            System.out.println(key + ": " + value);
        }
    }
}
```
3. `application`域范围的属性  
一个web应用程序中的所有Servlet都共享一个ServletContext对象，所以，ServletContext对象被称为application对象，可以用来存储属性，并定义了四个方法用来添加，删除，访问属性：getAttributeNames，getAttribute, removeAttribute，setAttribute
4. 访问资源文件
ServletContext接口中定义了一些用于访问web应用程序内部资源文件的方法：
    * getResourcePaths方法，返回一个Set集合对象，该Set对象中包含某个资源目录中的所有子目录和文件的路径名称，每个路径名称都以相对于整个web应用程序的根目录形式表示，即都以`/`开头，其中每个目录路径的最后也都由`/`结尾。  
    如果根目录下有`/index.jsp`，`/WEB-INF/web.xml`，运行getResourcePaths("/)的结果是`[/index.jsp, /WEB-INF/]`。
    * getResource方法，返回映射到某个资源上的URL对象，入参必须是一个以`/`开头的资源路径名，`/`表示web根目录。
    * getResourceAsStream，返回连接到某个资源上的InputStream对象，这个方法实际上就是打开了getResource方法返回的URL对象的输入流。  
    与ClassLoader加载资源不同，ClassLoader只能加载类加载路径下的资源，即`/WEB-INF/classes/`，而ServletContext可以访问整个web应用下的资源文件。  
    在根目录下添加test.properties:
```properties
userName=tomcat
password=jetty
```
```java
public class TestServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        ServletContext context = getServletContext();
        try(InputStream input = context.getResourceAsStream("/test.properties")) {
            Properties properties = new Properties();
            properties.load(input);
            System.out.println("userName: " + properties.getProperty("userName"));
            System.out.println("password: " + properties.getProperty("password"));
        }
    }
}
```

 输出：
```text
userName: tomcat
password: jetty
```
5. 获取虚拟路径所映射的本地路径  
getRealPath方法，用于返回某个虚拟路径所映射的本地文件系统路径，如果入参是`/`或一个空字符串，则返回web应用程序的根目录所映射的本地文件系统路径。


