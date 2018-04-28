# git

## 集中式VS分布式

### 集中式(cvs,svn)
集中式版本控制系统的版本库是放在中央服务器的，当要工作的时候，需要从中央服务器拉取最新版本，然后工作完再将工作的内容提交到中央服务器。
其最大的毛病就是需要联网才能工作，而且一旦中央服务器的版本库出了问题，所有人就无法工作了。

![集中式版本控制系统](../../image/others/git-1.jpeg)

### 分布式(git)
分布式版本控制系统没有中央服务器这个说法，每个人的电脑都是一个完整的版本库，工作的时候也不需要联网，因为版本库就在你自己的电脑上。当两个人都修改了文件时，只需要把自己修改的内容推送给对方就可以了，就可以互相看到对方的修改

![分布式版本控制系统](../../image/others/git-2.jpeg)

## 安装git
[下载git](https://git-scm.com/downloads)  
Debian或Ubuntu安装
```bash
sudo apt-get install git
```
安装后还需要设置邮箱跟名字
```bash
git config --global user.name "Your Name"
git config --global user.email "email@example.com"
```

## 创建版本库
* 创建新的目录
* 进入目录并执行命令`git init`，
使用`ls -a`查看目录结构

## 版本回退


---
参考  
* [git教程](https://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000)
* [Pro Git](https://git-scm.com/book/zh/v1)