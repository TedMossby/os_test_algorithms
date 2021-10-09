# os_test_algorithms
练习os调度算法

简单描述一下结构

从外存中拿入进程放入内存的ready链表中        Action1 

从ready链表中拿出第一个进程放入running中执行  Action2

在RR算法和可抢占形式的调度算法中还会把running中的进程重新放回ready队列中 Action3

执行完毕放入finish队列中 Action4

Action1 Action2 Action3 Action3 调用dispatcher类和manage_process_inf类中的相应方法

添加的调度算法写入dispatcher类中







![IMG_20211008_212619](https://user-images.githubusercontent.com/71285704/136651843-92798b55-bb99-40ff-b236-7eea91e91aad.jpg)
