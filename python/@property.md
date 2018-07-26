### @property
直接访问一个对象的属性时，会从对象的`__dict__`里面搜索他
```py
class Student(object):
    def __init__(self, name, age):
        self.name = name
        self.age = age


s = Student('tom', 18)
print(s.__dict__)
```
```text
{'name': 'tom', 'age': 18}
```

使用getter与setter方法，可以隐藏属性，并且定义自己的对参数的验证规则
```py
class Student(object):
    def __init__(self, name, age):
        self.name = name
        self.age = age

    @property
    def name(self):
        print("get name")
        return self.__name

    @name.setter
    def name(self, name):
        print("set name")
        if len(name) < 3:
            raise ValueError('len < 3')
        self.__name = name

    @property
    def age(self):
        print("get age")
        return self.__age

    @age.setter
    def age(self, age):
        print("set age")
        if age < 18:
            raise ValueError('18 banned')
        self.__age = age


s = Student('tom', 18)
s.age = 19
```
```text
set name
set age
set age
```
比如对于`name`属性，无论你内部是`name`，`_name`还是`__name`，对外都是通过`name`来访问的，可以看到，当访问`name`属性时，将通过调用相应的getter，setter方法，而不是直接从`__dict__`里面搜索     
当参数不合法时可以进行验证：
```py
s = Student('tom', 15)
```
```text
set age
  File "C:/pyproject/pytest/test.py", line 31, in <module>
    s = Student('tom', 15)
  File "C:/pyproject/pytest/test.py", line 4, in __init__
    self.age = age
  File "C:/pyproject/pytest/test.py", line 27, in age
    raise ValueError('18 banned')
ValueError: 18 banned
```