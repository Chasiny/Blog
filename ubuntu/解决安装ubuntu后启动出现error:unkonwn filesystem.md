
# 解决安装ubuntu后启动出现error:unkonwn filesystem
这种情况一般是由于装Ubuntu后覆盖了原来的引导文件
```
error: unknown filesystem.
Entering rescue mode...
grub rescue>
```
这是因为重装后引导出错
首先需要确定ubuntu安装在那个分区
使用ls命令列出当前所有分区
这是我的电脑的分区
```
(hd0)(hd0,msdos7)(hd0,msdos6)(hd0,msdos5)(hd0,msdos4)(hd0,msdos3)(hd0,msdos2)(hd0,msdos1)(hd1)(hd0,msdos1)(hd2)(hd0,msdos4)
```
使用set命令查看当前设置的启动属性
```
cmdpath=(hd0)
prefix=(hd0,msdos7)/boot/grub  // 如果出现找不到/boot/grub，可以设置/grub
root=hd0,msdos7
```
然后我们需要试着找出ubuntu的安装位置   
可以根据猜想试着检测
例如：先假设ubuntu安装在(hd0,msdos5)，那么我们需要执行以下命令
```
set root=hd0,msdos5
set prefix=(hd0,msdos5)/grub
```
然后执行
```
insmod normal
normal
```
如果没有出现其他错误的话说明你试的位置是正确的   
如果出现的话重新设置root跟prefix   

如果设置正确的话会进入ubuntu界面，但为了每次开机都能正常进入界面，我们需要设置grub安装位置
```
sudo update-grub
sudo grub-install /dev/sda
```

一般而言，sudo grub-install /dev/sda中设置的是sda，但是如果你的启动盘是sdb(多个硬盘)的话，就要设置sdb