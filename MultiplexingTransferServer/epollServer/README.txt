使用epoll实现一个多路转接的服务器,将客户端发送过来的数据进行打印(使用的是epoll的LT模式)

int epoll_create(int size);
创建epoll模型,返回文件描述符，epoll_create函数的参数必须是一个大于0的值

int epoll_ctl(int epfd,int op,int fd,struct epoll_event* event);
epoll_ctl函数用于向指定的epoll模型中注册事件
第1个参数int epfd:指定的epoll模型
第2个参数op：
EPOLL_CTL_ADD:注册新的文件描述符到epoll模型，就是向红黑树中添加节点
EPOLL_CTL_MOD:修改已经注册好的文件描述符的监听事件
EPOLL_CTL_DEL:从epoll模型中删除指定的事件，本质是删除红黑树中的节点
第三个参数fd:要监听的文件描述符
第四个参数:要监视的事件
typedef union epoll_data{
    void* ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;
struct epoll_event {
    uint32_t events;//要监视的事件
    epoll_data_t data;
};
events：
EPOLLIN:读事件
EPOLLOUT:写事件
EPOLLERR:错误事件
EPOLLET:将文件描述符设置为边缘触发(edge triggered)
EPOLLONESHOT:对该文件描述符只监听一次，监听完毕之后就从红黑树中删除

int epoll_wait(int epfd,struct epoll_event* events,int maxevents,int timeout);
events数组用来保存已经就绪的事件，maxevents为数组的长度，不能超过epoll_create函数的参数size
timeout设置为0表示非阻塞，设置为-1表示阻塞
epoll_wait失败：
EBADF:第一个参数epfd不是一个有效的epoll模型
EFAULT:传入的events数组无效
EINTR:epoll_wait函数的调用被信号打断

epoll工作原理：
epoll模型：
调用epoll_create会创建eventpoll结构，eventpoll结构中包含红黑树的根节点和就绪队列的头节点（就绪队列通过链表实现）
红黑树：每一个被监视的文件描述符都会被封装成红黑树的一个节点，key值为文件描述符，如果文件描述符的events中含有EPOLLONESHOT，那么当这个文件描述符上有数据时，除了被添加到就绪队列，还会从红黑树中删除

就绪队列：所有有事件就绪的文件描述符会被封装成就绪队列中的一个节点，上层判断是否有文件描述符就绪时，只需要判断就绪队列是否为空即可

回调机制：当红黑树中的节点对应的文件描述符有事件时，会触发回调机制，生成对应的节点插入到就绪队列中，上层调用epoll_wait就是从就绪队列中获取节点

epoll的线程安全问题：
epoll本身是线程安全的，epoll模型中的就绪队列会同时存在红黑树向其中插入数据和上层读取数据的情况，在epoll模型的eventpoll结构中，有互斥锁对就绪队列进行保护，内核向就绪队列添加数据，用户读取就绪队列的数据是线程安全的

如何查看一个进程打开了哪些文件？
ls /proc/进程pid号/fd/

epoll的优点：
1.epoll可以同时监视多个文件描述符上的事件是否就绪，这也是所有多路转接的优点
2.epoll相对于select和poll将设置监听和返回进行了分离，注册事件使用epoll_ctl,返回使用epoll_wait
3.epoll在进行注册事件时更加轻量，相比于select和poll，无需每一次都将大量数据从用户拷贝到内核进行注册，poll虽然在使用上相比于select方便，但是每一次调用还是要将数据从用户拷贝到内核进行注册
4.epoll注册事件时只需要调用epoll_ctl向红黑树插入节点即可
5.epoll在判断底层是否有事件就绪时，不是采用select和poll进行轮训检测，而是直接判断就绪队列是否为空即可
6.epoll事件就绪时，直接调用回调函数将事件添加到就绪队列即可
7.epoll的就绪队列通过eventpoll结构中的互斥锁保证了线程安全
8.epoll_wait函数返回时，得到的一定是已经就绪的文件描述符，相比于select和poll提高了效率