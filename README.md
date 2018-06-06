IPmask
=======
A tool for ip netmask calculation.

Compile
----------
```
make
```

Example
-----------
```
~ ⇒ ipmask 192.168.1.4/24
Input:             192.168.1.4/24
Network:           192.168.1.0
Netmask:           255.255.255.0
Bitmask :          24
Broadcast:         192.168.1.255
Range:             192.168.1.0 - 192.168.1.255
Network(Binary):   11000000.10101000.00000001.00000000
Netmask(Binary):   11111111.11111111.11111111.00000000
Broadcast(Binary): 11000000.10101000.00000001.11111111
Available IP address: 256
~ ⇒ ipmask 172.16.30.3 255.255.240.0
Input:             172.16.30.3 255.255.240.0
Network:           172.16.16.0
Netmask:           255.255.240.0
Bitmask :          20
Broadcast:         172.16.31.255
Range:             172.16.16.0 - 172.16.31.255
Network(Binary):   10101100.00010000.00010000.00000000
Netmask(Binary):   11111111.11111111.11110000.00000000
Broadcast(Binary): 10101100.00010000.00011111.11111111
Available IP address: 4096
```