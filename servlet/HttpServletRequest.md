### HttpServletRequest
> HttpServletRequest是专用于HTTP协议的ServletRequest子接口，用于封装HTTP请求消息，增加了获取HTTP协议专有的头信息的方法，支持cookie和session跟踪，以及获取HTTP请求消息参数的功能。
#### 一. 获取请求行相关信息
HTTP请求消息的请求行可以分为三部分：请求方式、资源路径和HTTP协议版本，HttpServletRequest接口中定义了若干获取请求行中各个部分的方法。
1. `getMethod`方法：返回HTTP请求消息中的请求方式，如`GET`，`post`等。
2. `getRequestURI`方法：返回请求行中的资源名部分，也就是位于`URL的主机和端口之后，参数部分之前的那部分`，如：   
对于`http://localhost:8080/test/one?userName=tom&isDelete=0`，该方法返回`/test/one`。
3. `getQueryString`方法：返回请求行中的参数部分，对上面的请求返回`userName=tom&isDelete=0`，如果资源路径后面没有参数部分，返回`null`。
4. `getProtocol`方法：返回请求行中的协议名和版本。
5. `getContextPath`方法：返回请求URL所属的web应用程序的路径，这个路径以`/`开头，表示相对于整个web站点的根目录，路径结尾不含`/`，如果请求URL属于web站点的根目录返回空字符串。
6. `getPathInfo`方法：返回请求URL中的额外路径信息，即请求URL中位于Servlet的路径之后和查询参数之前的内容，以`/`开头，如果请求URL中没有额外信息，返回null。
7. `getPathTranslated`方法：返回URL中额外路径信息对应的资源的真实路径。
8. `getServletPath`方法：返回Servlet所映射的路径。
```java
@WebServlet(name = "testServlet", urlPatterns = "/user/*")
public class TestServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        System.out.println("getMethod: " + request.getMethod());
        System.out.println("getRequestURI: " + request.getRequestURI());
        System.out.println("getQueryString: " + request.getQueryString());
        System.out.println("getProtocol: " + request.getProtocol());
        System.out.println("getContextPath: " + request.getContextPath());
        System.out.println("getPathInfo: " + request.getPathInfo());
        System.out.println("getPathTranslated: " + request.getPathTranslated());
        System.out.println("getServletPath: " + request.getServletPath());
    }
}
```
```java
@WebServlet(name = "servletOne", urlPatterns = "/test/one")
public class ServletOne extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {

    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        System.out.println(request.getPathTranslated());
        System.out.println("in servletOne");
        User user = new User("tomcat", "jetty");
        HttpSession session = request.getSession();
        System.out.println("session in servletOne: " + session.getId());
        session.setAttribute("user", user);
    }
}
```
在浏览器直接访问`http://localhost:8080/user/test/one?username=tomcat&password=jetty`，控制台输出：
```txt
getMethod: GET
getRequestURI: /user/test/one
getQueryString: username=tomcat&password=jetty
getProtocol: HTTP/1.1
getContextPath: 
getPathInfo: /test/one
getPathTranslated: C:\Project\myweb\out\artifacts\myweb_war_exploded\test\one
getServletPath: /user
```
可以看到这里我们让请求URL中的额外路径信息指向另一个Servlet，然后通过`getPathTranslated`方法得到了这个Servlet在磁盘上的绝对路径。
#### 二. 获取网络连接信息
1. `getRemoteAddr`方法：返回发出请求的客户机的IP地址。
2. `getRemoteHost`方法：返回发出请求的客户机的完整主机名，如果解析不出来就返回客户机IP。
3. `getRemotePort`方法：返回发出请求的客户机所使用的网络接口的端口号。
4. `getLocalAddr`方法：返回web服务器上接收当前请求的网络接口的IP地址。
5. `getLocalName`方法：返回web服务器上接收当前请求的网络接口的IP地址对应的主机名。
6. `getLocalPort`方法：返回web服务器上接收当前请求的网络接口的端口号。
7. `getServerName`方法：返回当前请求所指向的主机名，如果HTTP请求消息的Host头字段中包含有主机名部分，那么返回值即为该主机名部分。
8. `getServerPort`方法：返回当前请求所连接的服务器端口号，如果HTTP请求消息的Host头字段中包含有端口号部分，那么返回值即为该端口号部分。
9. `getScheme`方法：返回请求的协议名。
10. `getRequestURL`方法：返回客户端发出请求时的完整URL，包括协议、服务器名、端口号。资源路径等信息，`但不包括后面的查询参数部分`。
#### 三. 获取请求头信息
1. `getHeader`方法：返回一个指定名称的头字段的值 `public String getHeader(String name)`，如果不包含此字段则返回null。
2. `getHeaders`方法：返回一个Enumeration集合对象，该集合对象由请求消息中出现的某个指定名称的所有头字段值组成。
3. `getHeaderNames`方法：返回一个包含请求消息中所有头字段名的Enumeration集合对象。
4. `getIntHeader`方法：获取一个指定的头字段值，并将其转换为int类型后返回，如果不存在此字段则返回-1，如果头字段值不能转化为int类型，则抛出`NumberFormatException`异常。
5. `getDateHeader`方法：用于获取一个指定名称的头字段的值并将其按GMT时间格式转换成一个代表日期的long类型值返回，不存在此头字段返回-1，类型转换错误抛出`NumberFormatException`异常。
6. `getContentType`方法：直接返回`Content-Type`头字段值。
7. `getContentLength`方法：直接返回`Content-Length`头字段值，结果为int类型。
8. `getCharacterEncoding`方法：用于获取请求消息的实体部分的字符集编码，通常从`Content-Type`头字段中进行提取。