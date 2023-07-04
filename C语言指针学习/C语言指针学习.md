## 1.指针的类型
    32位机器上指针类型大小都是四字节

## 2.算术运算：例如int* p; 
    此时p+1：表示向后移动int大小的长度

## 3.void指针：void*
    可以指向任意类型的数据
```C
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i = 2;
    int *p = &i;
    int ** pp = &p;
    void *q = pp;
    printf("%d" , **((int**)q)); // 2
    return 0;
}
```
## 4.数组作为参数传递
    注意：需要传递类型以及大小
    下面的函数参数传递类型int[]，大小为size
```C
void fun(int A[], int size);
```
    补充：
    1.sizeof关键字是编译时解析，在编译时就已经确定下来数组大小了，所以使用sizeof直接获得即可。
    2.对于函数入参不可以先传数组再在函数中使用sizeof，因为函数入参只保存类型不保存长度，并且入参数组长度可能会变的。

## 5.字符串与指针
### 5.1 字符串初始化形式
    注意：
        1.strlen函数不包括'\0'，计算的是'\0'之前的字符串长度
        2.sizeof关键字：
            在无长度初始化场景下为从开始到包含第一个'\0'的长度
            在有长度的场景下为设置的长度
        3.如果char数组长度设置小于字符串长度会出现乱码
        4.长度符合要求的场景下隐式初始化会自动加'\0'，显示初始化需要手动加'\0'，加上'\0'防止乱码

* 按位初始化
```C
#include <stdio.h>
#include <string.h>

int main()
{
    char c[10];
    c[0] = 'J';
    c[1] = 'O';
    c[2] = 'H';
    c[3] = 'N';
    c[4] = '\0';
    printf("%s\n" , c); // JOHN
    printf("len = %d\n" , strlen(c)); // len = 4
    printf("size = %d\n" , sizeof(c)); // size = 10
    return 0;
}
```

* 有长度隐式初始化
```C
#include <stdio.h>
#include <string.h>

int main()
{
    char c[20] = "JOHN";
    printf("%s\n" , c); // JOHN
    printf("len = %d\n" , strlen(c)); // len = 4
    printf("size = %d\n" , sizeof(c)); // size = 20
    return 0;
}
```

* 无长度隐式初始化
```C
#include <stdio.h>
#include <string.h>

int main()
{
    char c[] = "JOHN";
    printf("%s\n" , c); // JOHN
    printf("len = %d\n" , strlen(c)); // len = 4
    printf("size = %d\n" , sizeof(c)); // size = 5
    return 0;
}
```

* 有长度显示初始化
```C
#include <stdio.h>
#include <string.h>

int main()
{
    char c[6] = {'J','O','H','N','\0'};
    printf("%s\n" , c); // JOHN
    printf("len = %d\n" , strlen(c)); // len = 4
    printf("size = %d\n" , sizeof(c)); // size = 6
    return 0;
}
```
### 5.2 字符指针
    字符指针不要带长度也可以正确执行，会在遍历中判断到'\0'结束
```C
#include <stdio.h>
void print(char* c) 
{
    while(*c != '\0')
    {
        printf("%c", *c);
        c++;
    }
    printf("\n");
}

int main()
{
    char c[6] = "Hello";
    print(c); // Hello
    return 0;
}
```

## 6.指针与多维数组
### 6.1 二维数组
* 案例：如果一个二维数组int B[2][3] 起始地址从400开始，以字节为单位
    * 地址[400, 403]：B[0][0]
    * 地址[404, 407]：B[0][1]
    * 地址[408, 411]：B[0][2]
    * 地址[412, 415]：B[1][0]
    * 地址[416, 419]：B[1][1]
    * 地址[420, 423]：B[1][2]
    
```C
int B[2][3]; // B[0]和B[1]是包含三个int类型的一维数组
int (*p)[3] = B; // 指针p指向int[3]类型的指针
print B or &B[0] //400
print *B or B[0] or &B[0][0] //400
print B + 1 or &B[1] //412
print *(B + 1) or B[1] or &B[1][0] //412
print *(B + 1) + 2 or B[1] + 2 or &B[1][2] //420
print *(*B + 1) or B[0][1] //是地址[404, 407]存的值
```
* 总结：B[i][j] = * (B[i] + j) = * (*(B + i) + j)

### 6.2 三维数组
* 案例：如果一个二维数组int C[3][2][2] 起始地址从800开始，以字节为单位
    * 地址[800, 807]：C[0][0]
    * 地址[808, 815]：C[0][1]
    * 地址[816, 823]：C[1][0]
    * 地址[824, 831]：C[1][1]
    * 地址[832, 839]：C[2][0]
    * 地址[840, 847]：C[2][1]

```C
int C[3][2][2]; 
int (*p)[2][2] = C; // 指针p指向包含int[2][2]类型的指针
print C or &C[0] //800
print *C or C[0] or &C[0][0] //800
print *(C[0][1] + 1) or C[0][1][1] // 地址[820, 823]存的值
print *(C[1] + 1) or C[1][1] or &C[1][1][0] //824
```
* 总结：C[i][j][k] = * (C[i][j] + k) = * (* (C[i] + j) + k) = * (* (* (C + i) + j) + k)

### 6.3 多维数组函数调用
* 注意：
    * int C[3][2][2]作为函数入参形参不是int*** p类型，而是int (*p)[2][2]类型
    * 其中指针只可以省略第一维，因为p的含义是指向int[2][2]类型的指针
```C
#include <stdio.h>

// 不是void fun(int*** p){}
void fun(int *A[2][2]) {}
int main()
{
    int C[3][2][2] = {0};
    fun(C);
    return 0;
}
```

## 7.指针与动态内存
### 7.1 多维指针的创建和释放
```C
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char ***a, i, j;
    int m = 2, n = 3, p = 4;
    a = (char ***)malloc(m * sizeof(char**));
    for (i = 0; i < m; ++i)
        a[i] = (char **)malloc(n * sizeof(char*));
    for (i = 0; i < m; ++i)
        for (j = 0; j < n; ++j)
            a[i][j] = (char *)malloc(p * sizeof(char));

    for (i = 0; i < m; ++i)
        for (j = 0; j < n; ++j)
            free((void *)a[i][j]);
    for (i = 0; i < m; ++i)
        free((void *)a[i]);
    free((void *)a);
    return 0;
}
```

### 7.2 多维数组和指针混合使用创建和释放
```C
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int m = 2, n = 3, p = 4;
    char(*a)[n][p] = (char(*)[3][4])malloc(m * n * p * sizeof(char));

    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < p; ++k)
                a[i][j][k] = i * 100 + j * 10 + k;
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < p; ++k)
                printf("%d, ", a[i][j][k]);

    for (int i = 0; i < m; ++i)
        free((void *)a[i]);
    free((void *)a);
    return 0;
}
```

### 7.3 C++中使用一维
```C++
int *p;
p = new int;
delete p;
p = new int[20];
delete[] p;
```

### 7.4 C++中使用多维
```C++
#include <iostream>

using namespace std;

int main()
{
    int n = 2, m = 3;
    int **dp = new int *[n]; //该指针数组的指针来访问 n 个指针
    for (int i = 0; i < n; ++i) // 循环分配
        dp[i] = new int[m];

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            dp[i][j] = i * 10 + j;

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            cout << dp[i][j] << endl;

    // 首先释放低一级的动态数组
    for (int i = 0; i < n; ++i)
    {
        delete[] dp[i];
    }
    // 然后释放高一级的动态数组
    delete[] dp;
    return 0;
}
```

## 8.函数指针
```C
#include <stdio.h>
#include <stdlib.h>

void print(char* name) 
{
    printf("name = %s\n", name);
}
int add(int a, int b)
{
    return a + b;
}

int main()
{
    void (*ptr1)(char*);
    ptr1 = print;
    ptr1("hello");  // name = hello
    int (*ptr2)(int, int);
    ptr2 = add;
    int c = ptr2(3, 4);
    printf("c = %d\n", c);  // c = 7;
    return 0;
}
```