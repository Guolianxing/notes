### 指针，引用和const

#### 一. const的引用

* 对常量的引用，不能用作修改它所绑定的对象：
```C++
const int a = 3;
const int &r = a;
r = 4;    // 错误，不能修改它所绑定的对象
```
* 若所绑定的对象非const，则无法阻止其他方式改变对象值：
```C++
int a = 3;
const int &r = a;
cout << r << endl;
a = 4;
cout << r << endl;
r = 4;    // 错误，不能通过该引用修改它所绑定的对象
```
* 引用的类型必须与其所引用的对象类型一致，但是在初始化常量引用时允许用任意表达式作为初始值，只要该表达式的结果能转换成引用的类型那个即可
```C++
double d = 3.14;
const int &r = d;
```
编译器把上述代码编程了如下形式：
```C++
const int temp = d;
const int &r = temp;
```

#### 二. 指向常量的指针
要想存放常量对象的地址，只能使用指向常量的指针：
```C++
const int a = 3;
const int *p = &a;
int *p_1 = &a;    // 错误：变量a是const的，指针要const才行
```
指向常量的指针，无法用该指针修改指向对象的值，但无法阻止其他方式修改该指针指向对象的值：
```C++
int a = 3;
const int *p = &a;
cout << *p << endl;
a = 4;
cout << *p << endl;
```
但指针自身的值是可以变的：
```C++
int a = 3;
int b = 4;
const int *p = &a;
p = &b;
cout << *p << endl;
```

#### 三. const指针
不变的是指针自身，而不是指向的对象：
```C++
int a = 3;
int *const p = &a;
const int *const p_1 = &a;
cout << a << endl;
*p = 2;
cout << a << endl;
int b = 3;
p = &b;     // 错误：常量指针自身不可变
```
常量指针自身不可变，但是可以用该指针来改变指向的对象的值。

#### 四. 顶层const，底层const
顶层const表示指针本身是个常量    
底层const表示指针所指的对象是一个常量     
更一般的，顶层const可以表示任意的对象是常量，对任何数据类型都适用，比较特殊的是，指针既可以是顶层const，也可以是底层const   
```C++
int i = 0;
int *const p1 = &i;   // 不能改变p1的值，顶层
const int ci = 42;    // 不能改变ci的值，顶层
const int *p2 = &ci;  // 不能用p2改变ci，但可以改变p2，底层
const int *const p3 = p2; //右边顶层，左边底层
const int &r = ci;    // 用于声明引用的都是底层const
```
> 用于声明引用的都是底层const：引用不是一个对象，所以我们没法让引用本身恒定不变，更谈不上常量       

当执行对象拷贝的时候，顶层const不受什么影响，但底层const的限制却不能忽视，拷入和拷出的对象必须具有相同的底层const资格，或者两个对象的数据类型必须能够转换，一般来说，非常量可以转换成常量，反之则不行。
```C++
const int a = 4;
const int *p = &a;
const int *p_1 = p;   // 正确
int *p_2 = p;    // 错误，p_2不具有底层const    
```
如果`int *p_2 = p`是合法的，则可以通过`p_2`改变a的值，但a是const的，无法改变，所以不合法。


