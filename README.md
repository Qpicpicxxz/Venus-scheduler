# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband

### 超级阉割版
1. pico测试只支持中断触发，此版本检测到irq=0x1000的中断即认为是block1发生的，未考虑DMA中断
2. 服务器python会报错，按理来说是这个属性python3.2之后就支持了，因此服务器上跑不起来python脚本，此分支里预存了模拟的三个task_bin.c
```
Traceback (most recent call last):
  File "read_bin.py", line 23, in <module>
    new_value = int.from_bytes(sub_array, byteorder='big')
AttributeError: type object 'int' has no attribute 'from_bytes'
```

