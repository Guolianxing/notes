### jdk序列化
#### 一. Serializable接口
Java的序列化将那些实现了`Serializable`接口的对象转换成一个字节序列，并能后在以后将这个字节序列完全恢复为原来的对象。
```java
import java.io.Serializable;

public class User implements Serializable {

    private Long userId;
    private String username;

    public User() {
        System.out.println("User()");
    }

    public User(Long userId, String username) {
        System.out.println("User(Long, String)");
        this.userId = userId;
        this.username = username;
    }

    @Override
    public String toString() {
        return "User{" + "userId=" + userId + ", username='" + username + '\'' + '}';
    }

    public static void main(String[] args) throws IOException, ClassNotFoundException {
        String fileName = "data.txt";
        User user = new User(2L, "John");
        System.out.println(user);
        ObjectOutputStream outputStream = new ObjectOutputStream(new FileOutputStream(fileName));
        outputStream.writeObject(user);
        outputStream.close();

        ObjectInputStream inputStream = new ObjectInputStream(new FileInputStream(fileName));
        User user2 = (User) inputStream.readObject();
        inputStream.close();
        System.out.println(user2);
    }    
}
```
输出：
```text
User(Long, String)
User{userId=2, username='John'}
User{userId=2, username='John'}
```
* `Serializable`接口是一个标记接口，没有任何方法。
* 反序列化的过程中，不会调用任何构造器，包括默认构造器，整个对象都是根据从`InputStream`中获取到的数据恢复的。
* `transient`关键字修饰的成员变量不会被序列化。
* 序列化保存的是对象的数据，所以静态变量不会被序列化。
* 父类没有实现`Serializable`接口，而子类实现了，那么父类中的数据不会被序列化；此时，在反序列化时，父类的默认构造器会被调用，所以父类必须要有一个默认构造器，否则会报错。
* 反序列化时，必须要有这个类，否则会`ClassNotFoundException`。
* 序列化和反序列的类的`serialVersionUID`必须一致才会反序列化成功。
* 可以自己在类中实现`writeObject`和`readObject`方法，此时默认不保存任何字段，需要自己在这两个方法中实现相应的序列化和反序列化逻辑，比如决定保存哪些字段，或者做一些加密解密的处理，在序列化和反序列化时会自动调用这个两个方法。方法的签名必须是如下：
    ```java
    private void writeObject(ObjectOutputStream stream);
    private void readObject(ObjectInputStream stream);
    ```

#### 二. Externalizable接口
```java
package java.io;

import java.io.ObjectOutput;
import java.io.ObjectInput;

public interface Externalizable extends java.io.Serializable {

    void writeExternal(ObjectOutput out) throws IOException;

    void readExternal(ObjectInput in) throws IOException, ClassNotFoundException;
}
```
```java
import java.io.*;

public class User implements Externalizable {
    private Long userId;
    private String username;

    public User() {
        System.out.println("User()");
    }

    public User(Long userId, String username) {
        System.out.println("User(Long, String)");
        this.userId = userId;
        this.username = username;
    }

    @Override
    public String toString() {
        return "User{" + "userId=" + userId + ", username='" + username + '\'' + '}';
    }


    @Override
    public void writeExternal(ObjectOutput out) throws IOException {
        System.out.println("write");
        out.writeObject(userId);
        out.writeObject(username);
    }

    @Override
    public void readExternal(ObjectInput in) throws IOException, ClassNotFoundException {
        System.out.println("read");
        userId = (Long) in.readObject();
        username = (String) in.readObject();
    }

    public static void main(String[] args) throws IOException, ClassNotFoundException {
        String fileName = "data.txt";
        User user = new User(2L, "John");
        System.out.println(user);
        ObjectOutputStream outputStream = new ObjectOutputStream(new FileOutputStream(fileName));
        outputStream.writeObject(user);
        outputStream.close();

        ObjectInputStream inputStream = new ObjectInputStream(new FileInputStream(fileName));
        User user2 = (User) inputStream.readObject();
        inputStream.close();
        System.out.println(user2);
    }
}
```
输出：
```text
User(Long, String)
User{userId=2, username='John'}
write
User()
read
User{userId=2, username='John'}
```
* `Externalizable`默认不保存任何字段，所以`transient`关键字只能和`Serializable`一起使用。
* `Externalizable`接口继承了`Serializable`接口，并添加了`writeExternal`和`readExternal`这两个方法，用于自定义自己的序列化和反序列化逻辑，和上面的实现了`Serializable`的类中添加自己的`writeObject`和`readObject`方法差不多，但是，*Externalizable在反序列化的时候，会先调用默认构造器，然后调用readRxternal方法，而Serializable不会调用任何构造器*

---
相关链接：  
[Java序列化协议](https://docs.oracle.com/javase/8/docs/platform/serialization/spec/protocol.html)    
[Java序列化的安全漏洞](https://blog.chaitin.cn/2015-11-11_java_unserialize_rce/)