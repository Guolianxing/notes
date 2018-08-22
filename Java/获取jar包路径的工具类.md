### 获取jar包路径的工具类
```java
import java.io.File;

public class JarTool {

    // 获取jar包完整的绝对路径（包括文件名）
    public static String getJarPath() {
        File file = getFile();
        if (file == null)
            return null;
        return file.getAbsolutePath();
    }

    // 获取jar包所在完整路径
    public static String getJarDir() {
        File file = getFile();
        if (file == null)
            return null;
        return getFile().getParent();
    }

    // 获取jar包名
    public static String getJarName() {
        File file = getFile();
        if (file == null)
            return null;
        return getFile().getName();
    }

    private static File getFile() {
        String path = JarTool.class.getProtectionDomain().getCodeSource().getLocation().getFile();
        try {
            path = java.net.URLDecoder.decode(path, "UTF-8");
        } catch (java.io.UnsupportedEncodingException e) {
            return null;
        }
        return new File(path);
    }

}
```
写个测试类，打jar包测试一下：
```java
public class Main {
    public static void main(String[] args) {
        System.out.println("Jar path: " + JarTool.getJarPath());
        System.out.println("Jar dir: " + JarTool.getJarDir());
        System.out.println("Jar name: " + JarTool.getJarName());
    }
}
```
输出：
```shell
Jar path: C:\Project\demo\out\artifacts\demo_jar\demo.jar
Jar dir: C:\Project\demo\out\artifacts\demo_jar
Jar name: demo.jar
```