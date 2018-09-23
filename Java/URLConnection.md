### URLConnection
利用URLConnection发送http请求的几个步骤：  
1. 构建URL对象： `URL url = new URL(urlStr);`
2. 从URL对象中获取URLConnection：`URLConnection connection = url.openConnection();`
3. 调用URLConnection对象的setRequestProperty方法或一些具体的方法设置请求头属性：`connection.setRequestProperty(key, value);`
4. 设置请求参数：
    * 如果是GET请求，则参数应该URL编码后跟在构建URL对象的字符串后面：`http://abc.com/query/?name=tomcat&age=12`。
    * 如果是POST请求，则应先调用URLConnection对象的setDoOutput(true)，然后从URLConnection对象获取输出流，将URL编码后的参数写进输出流。
5. 调用URLConnection对象的connect方法连接远程资源。
6. 访问响应：
    * 通过getHeaderField方法或一些具体的方法访问响应头
    * 从URLConnection对象中获取输入流来访问响应实体。

```java
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.util.Map;

public class HttpUtil {
    public static String doPost(String url, Map<String, String> params, Map<String, String> headers) throws IOException {
        URLConnection connection = new URL(url).openConnection();
        if (headers != null) {
            for (Map.Entry<String, String> entry : headers.entrySet()) {
                connection.setRequestProperty(entry.getKey(), entry.getValue());
            }
        }
        if (params != null) {
            connection.setDoOutput(true);
            try (PrintWriter writer = new PrintWriter(connection.getOutputStream())) {
                boolean first = true;
                for (Map.Entry<String, String> entry : params.entrySet()) {
                    if (first) {
                        first = false;
                    } else {
                        writer.write("&");
                    }
                    writer.write(entry.getKey());
                    writer.write("=");
                    writer.write(URLEncoder.encode(entry.getValue(), StandardCharsets.UTF_8.name()));
                }
            }
        }
        connection.connect();
        String encode = connection.getContentEncoding();
        if (encode == null) {
            encode = StandardCharsets.UTF_8.name();
        }
        return getResponse(connection, encode);
    }

    public static String doGet(String url, Map<String, String> params, Map<String, String> headers) throws IOException {
        if (params != null) {
            StringBuilder builder = new StringBuilder(url);
            builder.append("?");
            boolean first = true;
            for (Map.Entry<String, String> entry : params.entrySet()) {
                if (first) {
                    first = false;
                } else {
                    builder.append("&");
                }
                builder.append(entry.getKey()).append("=").append(URLEncoder.encode(entry.getValue(), StandardCharsets.UTF_8.name()));
            }
            url = builder.toString();
        }
        System.out.println(url);
        URLConnection connection = new URL(url).openConnection();
        if (headers != null) {
            for (Map.Entry<String, String> entry : headers.entrySet()) {
                connection.setRequestProperty(entry.getKey(), entry.getValue());
            }
        }
        connection.connect();
        String encode = connection.getContentEncoding();
        if (encode == null) {
            encode = StandardCharsets.UTF_8.name();
        }
        return getResponse(connection, encode);
    }

    private static String getResponse(URLConnection connection, String encode) throws IOException {
        String res = null;
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream(), encode))) {
            StringBuilder builder = new StringBuilder();
            String str = null;
            while ((str = reader.readLine()) != null) {
                builder.append(str);
            }
            res = builder.toString();
        }
        return res;
    }
}
```