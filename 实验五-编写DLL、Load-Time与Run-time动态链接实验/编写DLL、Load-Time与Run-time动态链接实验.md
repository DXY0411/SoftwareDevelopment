# 编写DLL & Load-Time/Run-time动态链接实验
## 手动编译链接基础实验：
* 在VS中新建一个项目，添加两个.c文件，a.c和b.c，内容如下图    
  ![](images\ab.png)
* 手动编译链接文件
  * 编译用```cl.exe /c xxx.c```指令，其中/c代表只编译不链接
  * 可以发现编译成功，生成了a.obj和b.obj两个文件
  * 将两个obj文件进行链接，用```link a.obj b.obj /out:haha.exe```指令，将链接得到的exe文件输出为haha.exe。
  * 观察到成功链接，生成了可执行文件。    
    ![](images\ab-c-link.png)     
    ![](images\haha.png)    
  * 执行起来也没有报错，手动编译链接成功。  
  * 现在将b.c内容进行更改，调用一个MessageBox系统函数，如下图     
    ![](images\b-change.png) 
  * 重新编译b.c生成新的b.obj文件（覆盖了之前的文件）     
    ![](images\bobjnew.png)   
  * 将a.obj和新的b.obj进行链接，设置生成的exe文件名为hehe.exe     
    发现此时链接会报错，显示有一个无法解析的外部命令，即我们调用的MessageBox函数。      
    ![](images\new-link.png)   
  * 去查找[MessageBox function的官方函数文献](https://docs.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-messagebox)，发现要想调用这个函数需要User32模块     
    ![](images\MB-Re.png)    
  * 所以在链接的时候要手动添加User32.lib文件    
    ![](images\hehe.png)     
  * 观察到这次链接成功了，没有报错，且运行hehe.exe也成功了。   
  * 对之前haha.exe和hehe.exe用dumpbin进行导入表的比较   
    发现hehe.exe确实多导入了User32.dll模块，这个模块中有我们调用的MessageBox函数相关信息   
    ![](images\moreMB.png)      


************
## 进阶实验
### 【要求一】
* 会编写dll。把.c文件编译为obj文件，把obj文件和lib文件链接为新的dll和lib文件。
* 注意使用def文件定义导出函数。
#### 实验过程：
* 在VS中新建一个项目base，添加两个文件，base.c和exp.def，内容如下图    
  * 其中def文件用于指定动态链接库文件名、定义导出哪个函数。  
    * 此处指定之后生成的dll文件名为baselib.dll    
  且只导出base.c中写出的lib_function函数   
    * 该函数调用了MessageBoxA函数（所以在链接时需要 User32.lib文件）  

  ![](images\base.png)  
* 把base.c文件编译为base.obj文件，使用def文件定义导出函数，把base.obj文件和User32.lib文件链接为新的baselib.dll和base.lib文件。  
  ![](images\basedll.png)   
* 以上是手动生成的方式
* 若用VS生成dll文件和lib文件，则需要进行以下设置  
  ![](images\vsset.png)  
* 设置完之后右键项目，点击“生成”，则可把obj文件和lib文件链接为新的dll和lib文件。 
  * 用vs这样生成有一点小问题，就是生成的dll文件名没有按照我们想要的baselib.dll生成，而是按照项目名称自动生成的base.dll(不过并无大碍) 
  
  ![](images\vsok.png)  


### 【要求二】
* 编写一个exe，调用第一步生成的dll文件中的导出函数。
* 方法是
  * link时，将第一步生成的lib文件作为输入文件。
  * 保证dll文件和exe文件在同一个目录，或者dll文件在系统目录。
#### 实验过程：
* 在VS中新建一个项目app，添加app.c文件，内容如下图    
  app.c中调用了第一步生成的baselib.dll文件中的导出函数lib_function  
  ![](images\app.png)   
* 手动操作：
  * 先将第一布生成的baselib.dll文件拷贝到当前目录
    * 否则生成了app.exe后直接运行会报错  
      ![](images\baselibno.png)
  * 然后将app.c进行编译，得到app.obj文件  
  * 再通过```link app.obj ..\base\base.lib /out:app.exe```指令，将第一部生成的base.lib文件作为输入文件，链接生成app.exe文件，发现可以成功运行。   
  ![](images\appok.png)  
  * 此时用dumpbin查看app.exe的导入表  
    可以发现确实导入了baselib.dll模块，其中包含我们的函数lib_function    
    ![](images\dumpapp.png) 
* 若在VS中实现：
  * 首先应在base项目中添加头文件，并在app.c中include该头文件    
    ![](images\head.png)
  * 然后配置链接器输入的附加依赖项，添加上base.lib文件  
  * 并且需要将base.lib文件拷贝到当前目录   
  ![](images\vsset2.png)  
  * 这样便可成功运行    
  ![](images\vsok2.png)

*********************
### 小结：以上实现的这种调用方式称为load time动态链接，特点是：
  * 在link 生成exe时，需明确输入lib文件。
  * exe文件导入表中会出先需要调用的dll文件名及函数名

*********************
### 还有一种调用方式称为 run time动态链接，接下来进行学习、实验
### 要求：
* 参考[链接](https://docs.microsoft.com/zh-cn/windows/win32/dlls/using-run-time-dynamic-linking)，使用run time的方式，调用dll的导出函数。
* 系统API和第一步自行生成的dll，都要能成功调用。
### 基础理解：
* 两种方式的比较：
  * 装入时动态链接(Load-Time Dynamic Linking)
    * 用户源程序经编译后所得到的目标模块,是在装入内存时,边装入边链接的.即在装入一个目标模块时,若发生一个外部模块调用,将引起装入程序去找出相应的外部目标模块,并将它装入内存,还要修改目标模块中的相对地址。
    * 装入时动态链接方式有以下优点：
      * 便与软件版本的修改和更新
      * 便于实现目标模块共享：若采用装入时动态链接方式，OS能够将一个目标模块链接到几个应用模块，即实现多个应用程序对该模块的共享；然而，采用静态链接方式时每个应用模块都必须含有该目标模块的拷贝，否则无法实现共享。
    * 装入时动态链接方式有以下缺点：
      * 虽然可将装入模块装入到内存的任何地方，但装入模块的结构是静态的，它主要表现在两个方面：一是在进程的整个执行期间，装入模块不改变；再者是每次运行时的装入模块都是相同的。实际上，在许多情况下，每次要运行的模块可能是不相同的，但由于事先无法知道本次要运行哪些模块，故只能是将所有可能要运行到的模块，在装入时全部链接在一起，是每次执行时的装入模块是相同的。显然这是低效的。因为这样，在装入模块的运行过程中，往往会有某些目标模块根本就不运行。比较典型的例子是错误处理模块，如果程序在整个运行过程中，都不出现错误，便不会用到该模块。
  * 运行时动态链接（Run –Time Dynamic Linking）
    * 这种链接方式，可将某些目标模块的链接，推迟到执行时才进行。即在执行过程中，若发现一个被调用模块尚未装入内存时，才由OS去找到该模块，将它装入内存，并把它连接到调用者模块上。
    * 因为是在运行时进行动态链接，所以不必将模块与DLL的导入库链接。
    * 使用运行时动态链接即使DLL不可用的话，进程也能继续运行，然后进程可以通过一个变更方法达到最终目的。例如，如果进程不能找到一个DLL，它可以尝试另外一个或者向用户提示错误。如果用户可以提供丢失的DLL的全路径，进程就可以不管是否在正常的搜索路径中而使用该路径信息。（然而如果是载入时链接的话，系统就会终止，上一个实验也证明了这一点）

### 实验过程：
* 代码逻辑：     
  ![](images\runtime.png)         
  * 使用LoadLibrary函数获取指定DLL的句柄。如果LoadLibrary成功，则程序将使用GetProcAddress函数中返回的句柄来获取DLL的某一指定函数的地址。调用DLL函数后，程序将调用FreeLibrary函数以卸载DLL。
  * 当应用程序调用LoadLibrary函数时，系统会尝试定位DLL（方式和载入时动态链接的定位过程一致：①应用程序载入目录 ②当前目录 ③系统目录 等）。如果找到，系统就把DLL模块映射到进程的虚地址空间中，并增加引用计数。
  * 如果调用LoadLibrary时指定的DLL其代码已经映射到调用进程的虚地址空间，函数就会仅返回DLL的句柄并增加DLL引用计数。
  * 如果DLL不可用，运行时动态链接示例可以响应该错误。
* 当dll不可用时，程序运行如下，可见不会像Load-Time链接一样弹框终止运行，而是正常报错。      
  ![](images\runtimeerror.png)  
 
#### （1）修改代码，调用系统User32.dll中的函数MessageBoxA        
  ![](images\runapi.png)     
  * 修改LoadLibrary参数、GetProcAddress参数    
  * 传入MessageBoxA函数的参数设为(0, "msg", 0, 0)
  * 程序会自己在系统目录找到User32.dll  
  * 直接运行，结果和直接调用MessageBoxA(0, "msg", 0, 0)是一致的，说明成功进行了运行时链接！      
    ![](images\runresult2.png)      

#### （2）修改代码，调用之前实验生成的baselib.dll中的函数lib_function      
  ![](images\runfunc.png)        
  ![](images\rundll.png)      
  * 修改LoadLibrary参数、GetProcAddress参数      
  * 传入lib_function函数的参数设为"dxy"     
  * 将baselib.dll copy到当前目录下       
    ![](images\runbox.png)  
  * 成功运行，结果和直接调用lib_function("dxy")是一致的，说明成功进行了运行时链接！      
    ![](images\runresult.png)   

### 小结
* 通过实验更深入的认识了Load-Time和Run-time 这两种动态链接方式。（具体比较优缺点在上文中已经写过啦！）

### 参考资料
* 载入时动态链接及运行时动态链接 https://blog.csdn.net/houyichaochao/article/details/80857893
* 静态链接、装入时动态链接和运行时动态链接 https://blog.csdn.net/nankaixinren/article/details/9207073
* Using Run-Time Dynamic Linking https://docs.microsoft.com/zh-cn/windows/win32/dlls/using-run-time-dynamic-linking
