### centos 7下基本环境配置  
#### 一. jdk配置
1. 新建一个目录
```shell
mkdir /usr/java
```
2. 上传jdk压缩包到 /usr  
3. 解压jdk压缩包到上面新建的目录
```shell
cd /usr
tar xzf jdk-8u161-linux-x64.tar.gz -C /usr/java
```
4. 设置环境变量
编辑`/etc/profile`，添加以下内容：
```shell
# set java environment
export JAVA_HOME=/usr/java/jdk1.8.0_161
export CLASSPATH=$JAVA_HOME/lib/tools.jar:$JAVA_HOME/lib/dt.jar:$JAVA_HOME/lib
export PATH=$JAVA_HOME/bin:$PATH
```
加载环境变量：
```shell
source /etc/profile
```
查看java版本信息：
```shell
java -version
```
出现版本信息说明配置成功：
```shell
java version "1.8.0_161"
Java(TM) SE Runtime Environment (build 1.8.0_161-b12)
Java HotSpot(TM) 64-Bit Server VM (build 25.161-b12, mixed mode)
```
