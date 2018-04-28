# netstat命令详解

netstat可以列出系统上所有的网络套接字的连接情况，包括tcp，udp，unix套接字，也可以列出处于监听状态的套接字。

## 使用
### netstat：直接列出当前所有的连接

```bash
Active Internet connections (w/o servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State      
tcp        0      0 202.192.81.9:37128      203.208.41.65:https     ESTABLISHED
tcp        0      1 202.192.81.9:44206      tl-in-f101.1e100.:https SYN_SENT   
tcp        0      1 202.192.81.9:44204      tl-in-f101.1e100.:https SYN_SENT   
tcp        1      0 localhost:socks         localhost:44368         CLOSE_WAIT 
tcp        1      0 localhost:socks         localhost:44372         CLOSE_WAIT 
tcp        0      0 202.192.81.9:55556      .:https                 ESTABLISHED
tcp        0      0 202.192.81.9:35048      151.101.42.49:https     ESTABLISHED
tcp6       0      0 2001:da8:2019:a02:59038 lga34s14-in-x04.1:https ESTABLISHED
tcp6       0      0 2001:da8:2019:a02:59046 lga34s14-in-x04.1:https ESTABLISHED
tcp6       0      0 2001:da8:2019:a02:59036 lga34s14-in-x04.1:https ESTABLISHED
tcp6       0      0 2001:da8:2019:a02:49492 bom07s11-in-x03.1e:http ESTABLISHED
tcp6       0      0 2001:da8:2019:a02:59040 lga34s14-in-x04.1:https ESTABLISHED
tcp6       0      0 2001:da8:2019:a02:33384 qb-in-xbc.1e100.ne:5228 ESTABLISHED
Active UNIX domain sockets (w/o servers)
Proto RefCnt Flags       Type       State         I-Node   Path
unix  2      [ ]         DGRAM                    877      /run/user/1000/systemd/notify
unix  2      [ ]         DGRAM                    586      /run/wpa_supplicant/wlp3s0
unix  3      [ ]         DGRAM                    2373     /run/systemd/notify
unix  7      [ ]         DGRAM                    2390     /run/systemd/journal/socket
unix  22     [ ]         DGRAM                    14405    /run/systemd/journal/dev-log
unix  2      [ ]         DGRAM                    16833    /run/systemd/journal/syslog
```

## 参数表
-a 显示所有连接中的套接字  
-A<网络类型>或--<网络类型>   列出该网络类型连线中的相关地址。
-c 持续列出网络状态  
-e 显示网络其他相关信息  
-g 显示多重广播功能群组组员名单  
-i 显示网卡表  
-l 显示处于LISTEN的Socket  
-n 直接使用IP地址，而不通过域名服务器  
-N 显示网络硬件外围设备的符号连接名称  
-o 显示计时器  
-p 显示正在使用Socket的程序PID和程序名称  
-r 显示路由表
-s 显示网络统计表
-t 显示TCP传输协议的连线状况
-u 显示UDP传输协议的连线状况
-w 显示RAW传输协议的连线状况。
-x 此参数的效果和指定"-A unix"参数相同
--ip 此参数的效果和指定"-A inet"参数相同

### netstat -g
```bash
IPv6/IPv4 Group Memberships
Interface       RefCnt Group
--------------- ------ ---------------------
lo              1      all-systems.mcast.net
enp4s0f1        2      224.0.0.251
enp4s0f1        1      all-systems.mcast.net
wlp3s0          2      224.0.0.251
wlp3s0          1      all-systems.mcast.net
lo              1      ip6-allnodes
lo              1      ff01::1
enp4s0f1        1      ff02::1:ff7e:ef0a
enp4s0f1        1      ff02::1:ffeb:d813
enp4s0f1        2      ff02::fb
enp4s0f1        1      ff02::1:ffc7:4e70
enp4s0f1        1      ip6-allnodes
enp4s0f1        1      ff01::1
wlp3s0          2      ff02::fb
wlp3s0          1      ff02::1:ff05:6332
wlp3s0          1      ip6-allnodes
wlp3s0          1      ff01::1
```

### netstat -i
```bash
Kernel Interface table
Iface   MTU Met   RX-OK RX-ERR RX-DRP RX-OVR    TX-OK TX-ERR TX-DRP TX-OVR Flg
enp4s0f1   1500 0     40664      0      0 0         28005      0      0      0 BMRU
lo        65536 0      3816      0      0 0          3816      0      0      0 LRU
wlp3s0     1500 0      1026      0      0 0           916      0      0      0 BMRU
```

### netstat -r
```bash
Kernel IP routing table
Destination     Gateway         Genmask         Flags   MSS Window  irtt Iface
default         202.192.81.254  0.0.0.0         UG        0 0          0 enp4s0f1
default         192.168.1.1     0.0.0.0         UG        0 0          0 wlp3s0
link-local      *               255.255.0.0     U         0 0          0 enp4s0f1
192.168.1.0     *               255.255.255.0   U         0 0          0 wlp3s0
202.192.18.197  202.192.81.254  255.255.255.255 UGH       0 0          0 enp4s0f1
202.192.81.0    *               255.255.255.0   U         0 0          0 enp4s0f1
```