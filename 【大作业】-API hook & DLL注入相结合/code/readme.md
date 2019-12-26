# 代码list
* project1中有3个项目
    * hook项目是生成hook.dll文件的项目，包含fake_writefile的函数定义等内容
    * dllhook是将hook.dll注入目标notepad.exe的项目
    * project1是IAT hook的示例代码，我写的hook项目就是在这个基础上进行的修改（project1在此实验中没用到，只用了前两个项目）