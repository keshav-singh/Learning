////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//							ipc

system v ipc :-
1) Message queue
2) Shared memory
3) Semaphore
 
why ipc :-
1) data transfer :- pipe, msg-queue
2) sharing data :- shared memory
3) event notification :- signal
4) resource sharing :- semaphore(synchronisation)

point :-
1) pipe and message queue both are not fit for the process more than two.
2) mq are more efficient than the pipe because in mq every message is different in that.
3) dividing of message is not possible in mq but it is possible in pipe.

///////////////////////////////////////
//pipe:-

pipe:-pipe as a special file that can store a limited amount of data in a first in, first out (FIFO) manner.The amount of data that it can store is 65536 bytes in our system.It can take a message in a break form means a 20 byte message can read in split of 4 message in which each having a 5 bytes of data.

difference between pipe and file :-
1) pipe is a data structure in kernel whereas file is in hard disk.
2) pipe is just used for transferring the data not to stored.
3) random access is not possible in pipe by using lseek, it follow the fifo process. 


Pipes can be divided into two categories: 
1) unnamed pipes :- Unnamed pipes can be used only with related processes (e.g., parent/child or child/child) and exist only for as long as the processes using them exist.
2) named pipes :-Named pipes actually exist as directory entries. As such, they have file access permissions and can be used with unrelated processes.


difference between named and unnamed pipe :-
1) unnamed pipe can be used by only related process whereas named can be used by any two process in the system.
2) unnamed will generate SIGPIPE signal when no reader is there but named will not generate SIGPIPE signal.
3) For opening the file unnamed pipe used pipe system call whereas named pipe used open system call.
4) Named pipe is permanent in memory untill the system is on whereas unnamed pipe is transient in memory.
5) No permission in case of unnamed pipe whereas we can give in named pipe.


drawback:-
1) The communication channel provided by a pipe is a byte stream: there is no concept of message boundaries.so, if two message are coming from two process, we can't differentiate them according to their process.Multiple readers/writers can not be useful.
2) we can't change the size of the pipe.


we can creat unnamed pipe by two system call:-
1) pipe(int fd[2])
2) int pipe2(int fd[2], int flags);
pipe and pipe2 are same. pipe2 only save the extra call to fcntl to achieve the same result. If we are open any file in the O_NONBLOCK state then it will pass from the blocking call by setting the error EAGAIN.

flags:-
1) O_NONBLOCK
2) O_CLOEXEC 

ERRORS :-
1) EFAULT pipefd is not valid.
2) EINVAL (pipe2()) Invalid value in flags.
3) EMFILE Too many file descriptors are in use by the process.
4) ENFILE The system limit on the total number of open files has been reached.
////////////////////////////////////////
// unnamed pipe :-
function used for unnamed pipe are:-
1) pipe, pipe2
2) write(int fd, void *buf, int n_byte);
3) read(int fd, void *buf, int n_byte);
4) close(int fd) 	// for closing the file descriptor of file.


1) if we close the read end after that if we tried to write into file then we will get the SIGPIPE signal but if we close the write end and we tried to read from pipe then we will get the 0 return value and nothing operation will done by the read because it will find the EOF in the file.

programme:-
int main()
{
    int fd[2];
    char buff[20];

    pipe(fd);
    if (fork()) {
        read(fd[0], buff, 20);
        printf("%s\n", buff);
    }
    else {
        write(fd[1], "hello", 20);
    }
}


FILE *popen ( char *command, char *type);
This standard library function creates a half-duplex pipeline by calling pipe() internally. It then forks a child process, execs the Bourne shell, and executes the "command" argument within the shell. Direction of data flow is determined by the second argument, "type". It can be "r" or "w", for "read" or "write". It cannot be both! Under Linux, the pipe will be opened up in the mode specified by the first character of the "type" argument. So, if you try to pass "rw", it will only open it up in "read" mode.

int main(void)
{
        FILE *pipein_fp, *pipeout_fp;
        char readbuf[80];

        /* Create one way pipe line with call to popen() */
        if (( pipein_fp = popen("ls", "r")) == NULL)
        {
                perror("popen");
                exit(1);
        }

        /* Create one way pipe line with call to popen() */
        if (( pipeout_fp = popen("sort", "w")) == NULL)
        {
                perror("popen");
                exit(1);
        }

        /* Processing loop */
        while(fgets(readbuf, 80, pipein_fp))
                fputs(readbuf, pipeout_fp);

        /* Close the pipes */
        pclose(pipein_fp);
        pclose(pipeout_fp);

        return(0);
}

//////////////////////////////////////////////
//  named pipe
the data written to the FIFO is passed to and stored by the kernel and is not directly written to the file system.At the shell level the command used to make a named pipe is mknod.
if we are trying to write into named pipe but the read end is not there, then it will simply pass through the write without generating SIGPIPE signal.

The syntax for the mknod command to make a named pipe is :-
1) through command line :-
	mknod file_name p   // by giving p it is creating a fifo file.
	mkfifo file_name

2) through function :-
	int mknod(const char *pathname, mode_t mode, dev_t dev);	
	int mkfifo(const char *pathname, mode_t mode);

	The system call to generate a FIFO in a program has the same name as the system command equivalent i.e The dev argument for mknod is used only when a character or block special file is specified. For character and block special files, the dev argument is used to assign the major and minor number of the device.for creating a fifo file we can use dev as a 0 but the mode should be S_IFIFO.
The mkfifo library function uses the mknod system call to generate the FIFO.we can use 0777 at the place of mode in mkfifo.


FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);
it is used for open and close the pipe special file.

The server process is responsible for creating the public FIFO. Once created, the public FIFO is opened for both reading and writing. This may appear to be a little odd, as the server process only needs to read from the public FIFO.
int main(void)
{
    int fd;
    char buff[20];
    int fd1;
    int fd2;

    mkfifo("./raj", 0777);
    fd1 = open("./raj", O_RDWR);
    fd2 = open("./raj", O_RDWR);
    write(fd1, "hello raj", 20);
    read(fd2, buff, 20);
    printf("%s\n", buff);
    close(fd1);
    close(fd2);
}



One thing must be remember that at the reading time the writing process must be running at the same time otherwise it will not work. The size of the pipe either named or unnamed will be same i.e 65536. 

programme :-
//  sender
int main(void)
{
    int fd;
    char buff[20];
    int fd1;

    mkfifo("./raj", 0777);
    fd1 = open("./raj", O_RDWR);
    write(fd1, "hello raj", 20);
    while(1);   // it must be here otherwise no reading operation will be done.
    close(fd1);
}

// receiver
int main(void)
{
    int fd;
    char buff[20];
    int fd1;

    mkfifo("./raj", 0777);
    fd1 = open("./raj", O_RDWR);
    read(fd1, buff, 20);
    printf("%s\n", buff);
    close(fd1);
}

//////////////////////////////////

// message queue

Only two person can communicate through the message queue, more can be possible but not directly. The communication is done by the data file which we have to be mounted in the kernel by using the mount command. 
	 mount -t mqueue /dev/mqueue

	In msg queue default msg is 10 and it can be change from 1-32768. Default value of msg size is 8192 and it can be change upto 2 giga. Max queue in the application can be 256 but it can be upto 2 giga. 
	
system call used:-
1) mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
	
2) set the attribute in the mq_attr structure and pass that into the mq_open.
3) we can use two function for sending :-
	1) int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);
	2) int mq_timedsend(mqd_t mqdes, const char *msg_ptr,size_t msg_len, unsigned msg_prio,const struct timespec *abs_timeout);
4) at receiver side it will be same.
5) mq_unlink 
	it is used for removing the msg-queue.

// sender
int main(void)
{
    mqd_t m1;
    int status;
    struct mq_attr *attr;
    struct timespec *time_ptr;

    attr = (struct mq_attr*)malloc(sizeof(struct mq_attr));
    attr -> mq_maxmsg = 1;
    attr -> mq_msgsize = 6;

    time_ptr = (struct timespec*)malloc(sizeof(struct timespec));
    time_ptr -> tv_sec = time(NULL) + 5;

    m1 = mq_open("/msg_queue1", O_RDWR | O_CREAT, 0777, attr);
    perror("mq_open");

//  status = mq_send(m1, "hello", 6, 1);
//  status = mq_send(m1, "hello", 6, 1);
    status = mq_timedsend(m1, "hello", 6, 1, time_ptr);
    status = mq_timedsend(m1, "hello", 6, 1, time_ptr);
//  perror("mq_send");
    perror("mq_timedsend");
    return 0;
}


//  receiver
int main(void)
{
    mqd_t m1;
    int status;
    int prio;
    char buff[20] = {'\0'};
    struct mq_attr *attr;
    struct sigevent sev;

    m1 = mq_open("/msg_queue1", O_RDWR);
    perror("mq_open");

    attr = (struct mq_attr*)malloc(sizeof(struct mq_attr));
    attr -> mq_maxmsg = 1;
    attr -> mq_msgsize = 15;
    mq_setattr(m1, attr, attr);
    perror("mq_setattr");

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = fun;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &m1;
    mq_notify(m1, &sev);
    perror("mq_notify");

    status = mq_receive(m1, buff, 192, &prio);
    perror("mq_receive");
    printf("%s\n", buff);
    printf("%d\n", status);

    pause();
    mq_unlink("/msg_queue1");
    perror("mq_unlink");
    return 0;
}

	Here we can receive the data after terminating the sender. mq_overview for info. mq_open() is used to ceate the msg. queue. In this we can change the size of queue and no of max msg in queue by using the mq_setattr. mq_unlink is used to completely remove the file otherwise it will be in the system untill the system not get to shut down. in sender look at how to set the time by using time(NULL) it is called current time. the default value of msg in queue is 10 and it can be vary upto 1-32768. same as size of msg is 8192 bytes and it can vary upto 128-1048576. 

	in this mq_send will write the data in the file and closed the sender, Now we opened the receiving end for reading, at that time we can read that data but this facility is not possible in the pipe. In pipe read end must be opened before writing.

This ipc is good for tranferring small message but if the message size is large then it is not good because in this case it copy from user to kernel then again from kernel to user. In that case we can use shared memory.

	mq_send is block the process if the queue is full or mq_receive block if queue is empty.we can use the mq_notify which tells when the message is comes into a empty message queue.it gives the notification to the calling process and execute the thread or isr whichever we are using. when it get the notification it can also generate the signal if we set that.

	By using mq_notify we can execute a particular function when the message will come.

	The mount command mounts a storage device or filesystem, making it accessible and attaching it to an existing directory structure.

bug :-
1) Default value of max msg in queue is 10 and size of each message is 8192 butes.but if we are giving less than that it is accepting as shown in above programme.but it is not working for the value if we give more than that like if we are giving the no of message in queue 50, at that time it will fail.


advantage over pipe:-
1) notification alert is there.
2) priority can be set for the message.
3) no of message in queue or size of message can be change.
4) Message are not mixing with each other.

///////////////////////////////
// shared memory
	
shared memory means we assign a page in the physical memory and we are attaching that page with our process virtual address. In the shared memory kernel will maintain only the page or file information because it is physical page. It have no involvement of kernel, everything is done in the user space. That's why it is faster than the other ipc.Initially when the ipc is created it is 0 in size. By ftruncate we increase the size. once you write in the shared memory u can retrieve the data many time as u want. mmap is used to create a new virtual address space.in the argument we generally pass NULL for portability but if we pass some addr then it start the mapping from that particular address.mapping means entry in the "PT(page table) TABLE" which is virual address will point to that particular physical address.

   we can't say that there is not a single percent involvement of kernel. there is some involvement of kernel by maintaining file descriptor. In ftruncate the size must be multiple of page size.		

	mmap map the shared memory object to virtual address space.shared memory object is placed in the vfs called /dev/shm.on failing mmap did not return NULL, it return a pointer which is pointed to -1 i.e a macro called MAP_FAILED.
shm_unlink function is also there for removing the shared memory object.

	"df -h" is used to see all the mounted file in the system. 
function used :-
sender/-
1) int shm_open(const char *name, int oflag, mode_t mode);
2) int ftruncate(int fd, off_t length); // for increasing the size of shm.
3) void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
4) char *fgets(char *s, int size, FILE *stream);

receiver/-
1) int shm_open(const char *name, int oflag, mode_t mode);
2) void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
3) int fstat(int fd, struct stat *buf); // fstat is used for taking the status of file.
4) ssize_t write(int fd, const void *buf, size_t count);

programme :-
sender/-
int main(void)
{
    int fd;
    char *ptr = NULL;

    fd = shm_open("/my_shm", O_RDWR | O_CREAT | O_TRUNC, 0777);
    ftruncate(fd, 4096);

    if((ptr = mmap(NULL, 4096, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)    
        perror("mmap");

    close(fd);
    fgets(ptr, 1024, stdin);
}


receiver/-
int main(void)
{
    int fd;
    char *ptr = NULL;
    struct stat v;
    fd = shm_open("/my_shm", O_RDWR, 0777);

    if((ptr = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)    
        perror("mmap");

    fstat(fd,&v);
    write(1, ptr, v.st_size);
    exit(0);
}

///////////////////////////
// semaphore
	
POSIX  semaphores  allow  processes  and threads to synchronize their actions. suppose both process want to write in the shared memory. one process is writing and then switchin happen to other process. At that time the output will be undefined. so, for maintaining the data integrity in the shared memory we used the semaphore.occuring a semaphore means the value will be zero.value of semaphore can't be less than zero.sem_wait will block the process if the value of semaphoe is 0. named of semaphore must be start with the /.
   the name of semaphore will be in same file /dev/shm but with the prefix (sem.).

"important";-in the unnamed semaphore we are mapping the semaphore with the physical memory so at the time of changing the value of semaphore, it will change at the place of physical memory. so, no separate copy will be created.


types of semaphore :-
1) named
2) unnamed	


// internal and external fragmentation
when memory assigned to the process is more than the required that is internal fragmentation. 
when the memory is available for the process but that is not in the continuous form which is required for the process to run, that is external fragmentation.


function used in name semaphore :-
1) sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
2) int sem_wait(sem_t *sem);
3) int sem_post(sem_t *sem);
4) int sem_close(sem_t *sem);
5) int sem_unlink(const char *name);

function used in unnamed semaphore :-
1) int sem_init(sem_t *sem, int pshared, unsigned int value);
2) int sem_wait(sem_t *sem);
3) int sem_post(sem_t *sem);
4) int sem_destroy(sem_t *sem);
// in unnamed semaphore connect the semaphore with the shared memory and use sem_wait and sem_post.



// named semaphore :-
programme:-

pthread_t t1;                                                                   
pthread_t t2;                                                                   
char *ptr = NULL;                                                               
sem_t *my_sem;                                                                  
                                                                                
void* fun1(void *ptr2)                                                          
{                                                                               
    sem_wait(my_sem);                                                           
    sleep(1);                                                                   
    printf("write in child1 process\n");                                        
    fgets(ptr, 100, stdin);                                                     
    sleep(1);                                                                   
    printf("write in child1 process\n");                                        
    fgets(ptr, 100, stdin);                                                     
    sem_post(my_sem);                                                           
}
              
void* fun2(void *ptr2)                                                          
{                                                                               
    sem_wait(my_sem);                                                           
    sleep(1);                                                                   
    printf("write in child process\n");                                         
    fgets(ptr, 100, stdin);                                                     
    sleep(1);                                                                   
    printf("write in child process\n");                                         
    fgets(ptr, 100, stdin);                                                     
    sem_post(my_sem);                                                           
}                                                                               
                                                                                                                                                                
int main(void)                                                                  
{                                                                               
    int fd;                                                                     
    void *ptr1;                                                                 
    void *ptr2;                                                                 
                                                                                
    shm_unlink("/my_shm");                                                      
    fd = shm_open("/my_shm", O_RDWR | O_CREAT | O_TRUNC, 0777);                 
    my_sem = sem_open("/semaphore", O_CREAT, 0777, 1);                          
                                                                                
    ftruncate(fd, 4096);                                                         
    if((ptr = mmap(NULL, 4096, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)   
        perror("mmap");                                                         
                                                                                
    printf("start\n");                                                          
    pthread_create(&t1, NULL, fun1, NULL);                                      
    pthread_create(&t2, NULL, fun2, NULL);                                      
    pthread_join(t1, &ptr1);                                                    
    pthread_join(t2, &ptr2);                                                    
    printf("end\n");                                                            
}                           


//unnamed semaphore

int main(void)
{
    int fd;
    char *ptr = NULL;
    sem_t *my_sema;

    sem_destroy(my_sema);
    fd = shm_open("/my_shm", O_RDWR | O_CREAT | O_TRUNC, 0777);
    ftruncate(fd, 100);

    if((ptr = mmap(NULL, 100, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)    
        perror("mmap");

    my_sema = (sem_t*)ptr;
    sem_init(my_sema, 1, 1); //second arg decide whether this will be shared b/t the thread or the process,if non-zero means process.

    if (fork() == 0) {
        sem_wait(my_sema);
        perror("sem_wait");
        printf("in child\n");
        sleep(1);
        printf("in child\n");
        sleep(1);
        printf("in child\n");
        sem_post(my_sema);
    }
    else {
        sem_wait(my_sema);
        perror("sem_wait");
        printf("in parent\n");
        sleep(1);
        printf("in parent\n");
        sleep(1);
        printf("in parent\n");
        sem_post(my_sema);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//						mutex

point :-
1) we can use mutex only for same process, if we are going to use that in the fork process then at the time of changing the mutex value it will creat a separate copy for the process.

pthread_mutex_t fastmutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t recmutex  =  PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

pthread_mutex_t errchkmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

The mutex kind, which is either "fast",  "recursive",  or  "error checking". The kind of a mutex determines whether it can be locked again by a thread that already owns it. The default kind is "fast". 

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr); 
pthread_mutex_init  initializes  the mutex object pointed to by mutex according to the mutex attributes specified in mutexattr. If mutexattr is NULL, default attributes are used instead.


int pthread_mutex_lock(pthread_mutex_t *mutex);

int pthread_mutex_trylock(pthread_mutex_t *mutex);
pthread_mutex_trylock behaves identically to pthread_mutex_lock, except that it does not block the calling thread  if the mutex is already locked by another thread (or by the calling thread in the case of a ``fast''  mutex).  Instead,pthread_mutex_trylock  returns  immediately with the error code EBUSY.

int pthread_mutex_unlock(pthread_mutex_t *mutex);
 
int pthread_mutex_destroy(pthread_mutex_t *mutex);

2) Always compile the pthread file with the argument -lpthread.

//programme :-

pthread_t t1;
pthread_t t2;
pthread_mutex_t m1;
void* fun1(void *ptr)
{
    pthread_mutex_lock(&m1);
    printf("in 1\n");
    sleep(1);
    printf("in 1\n");
    sleep(1);
    printf("in 1\n");
    sleep(1);
    pthread_mutex_unlock(&m1);
}

void* fun2(void *ptr)
{                                            
    pthread_mutex_lock(&m1);                                
    printf("in 2\n");                       
    sleep(1);         
    printf("in 2\n");                         
    sleep(1);                           
    printf("in 2\n");        
    sleep(1);                               
    pthread_mutex_unlock(&m1);
}                                                                      
                                                                     
int main()                                                    
{                                         
    pthread_create(&t1, NULL, fun1, NULL);                  
    pthread_create(&t1, NULL, fun2, NULL);        
    pthread_join(t1);                                            
    pthread_join(t2);     
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						read and write

write:-
If a write() is interrupted by a signal handler before any bytes are written, then the call fails with the error EINTR;if it is interrupted after at least one byte has been written, the call succeeds, and returns the number of bytes written.

read :-ssize_t read(int fd, void *buf, size_t count);
If count is greater than SSIZE_MAX, the result is unspecified.
EINTR:-  The call was interrupted by a signal before any data was read.
EISDIR:- fd refers to a directory.
EFAULT:- Bad address or buf references an illegal address.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      	  Coredumps on Unix

A coredump is a special file which represents the memory image of a process. Many operating systems have the capability of saving a core dump when the application crashes. The core dump is an important part of diagnosing the cause of the crash, since the data which the application was accessing at the time is in the core dump, along with information about which part of the application was running at the time of the crash.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//						fork

Fork :- It is required only when we want to run the parallel activity. It is required in asynchronous programming. Best example where we should used fork is iterative server. Concurrent server is better than iterative server.
 
In child everything is copied from the parent but things which is not copied to parent :-

1) The child does not inherit its parent's memory locks (mlock(2), mlockall(2)).
2) Process resource utilizations (getrusage(2)) and CPU time counters (times(2)) are reset to zero in the child.
3) The child's set of pending signals is initially empty (sigpending(2)).
4) The child does not inherit semaphore adjustments from its parent (semop(2)).
5) The child does not inherit record locks from its parent (fcntl(2)).
6) The child does not inherit timers from its parent (setitimer(2), alarm(2), timer_create(2)).
 	alarm, signal, semaphore, memory lock, fcntl, cpu usage resources.

When the task_struct is created then we can say that process is created. Thread is also a light weight process.

ps -ef | grep 'defunct'


we can change the priority of the process by using the macro "set_task_state(struct task_struct*, int state_value)".


///////////////////////////

// exec

       int execl(const char *path, const char *arg, ...);
       int execlp(const char *file, const char *arg, ...);
       int execle(const char *path, const char *arg, ..., char * const envp[]);
       int execv(const char *path, char *const argv[]);
       int execvp(const char *file, char *const argv[]);
       int execvpe(const char *file, char *const argv[], char *const envp[]);

The exec() family of functions replaces the current process image with a new process image.  The functions described are front-ends for execve(2). whenever it is going to replace the image at that time first it is checking that how many process is using that page. If more than one process is using then it just take the new copy and run the newly programme

// programme for the ls | wc
int main()                                                                      
{                                                                               
    int fd[2];                                                                  
                                                                                
    pipe(fd);                                                                   
    if (fork()) {                                                               
        close(0);                                                               
        dup(fd[0]);                                                             
        close(fd[1]);                                                           
        execlp("wc", "wc", NULL);                                               
                                                                                
    }                                                                           
    else {                                                                      
        close(1);                                                               
        dup(fd[1]);                                                             
        close(fd[0]);                                                           
        execlp("ls", "ls", NULL);                                               
    }                                                                           
}           

//////////////////////////////////

// clone

Clone is a system call which is used to create fork, vfork, thread etc. It contain the variable argument list. It contain the stack which must be downgrowing. 

flag for
1) fork :- 
2) vfork :- CLONE_VM, CLONE_VFORK	
3) thread :- CLONE_VM, CLONE_THREAD, CLONE_FILES, CLONE_FS
                                                                    
/////////////////////////////////

// dup

int dup(int oldfd);
int dup2(int oldfd, int newfd);
int dup3(int oldfd, int newfd, int flags); // flags is FD_CLOEXEC,  O_CLOEXEC

After  a  successful return from one of these system calls, the old and new file descriptors may be used interchangeably. They refer
to the same open file description (see open(2)) and thus share file offset and file status flags; for example, if the file offset  is
modified by using lseek(2) on one of the descriptors, the offset is also changed for the other.

it going to fail when there is bad file descriptor or so many files are alresdy open.

both the dup and dup2 calls can be implemented with the fcntl system call (when passed the proper flag values).

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//							point :-
1) #include <errno.h>
   printing the error no and the message related to error no.
	printf("%s\n", strerror(errno)); 
	printf("%d\n", errno);

2) we can open maximum 1024 files from which 3 files are already open by system for every application.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//							kernel

kernel is the core of operating system which provides the various services to the process. Like file mgmt, memory mgmt, n/w mgmt.
	
execution environment is that which fascilitate the environment for the execution of an application. it provide the memory, create the task_struct for the process etc.

types of execution env :-
  1) os based :- it fascilitate the programmer to write,build and execute an application on an up and running kernel. e.g:-linux(desktop environment). Application can avail the service of kernel by using the system call mechanism. Here kernel is built separately and app is built separately.
	advantage:- i) As an application programmer we have no need to know about the kernel function because system call interface is there.
		   ii) it give the multi-tasking process.
     disadvantage:- i) it is slower as compare to kernel based.

2) kernel based:-Here kernel as well as application built together. It will be one executable. e.g:-pacemaker. These type of environment contain the fixed application.
	advantage:- i) it is faster.
     disadvantage:- i) we have to know about the kernel function.

//////////////////////////////

types of kernel :-
  1) monolithic kernel:- In this all the subsystem(file system, task mgmt, memory mgmt, interrupt, IPC) are bounded together to build one executable. e.g:- "Traditional unix"

	advantage :- it is faster as compare to other because for communication between two process no IPC is required.
	disadvantage :- Removal of unnecessary module is very much typical because of dependency of one module to other module.


2) micro kernel:- Here base kernel & ststem level task are there.Base kernel comprises of task management, IPC, interuppts. Memory mgmt, VFS all are in system level. so, if we don't want to add any subsystem then easily we can do that. e.g:- "unix"

	base kernel :- task mgmt, IPC, signal handling, interrupts.
	system level task :- VFS, memory mgmt.
	
	advantage :- it is highly scalable means if you don't want any system level task then you can easily remove.
	disadvantage :- For communication we required IPC, so it is slow as compare to the monolithic.


3) modular monolithic kernel:- By principle it is monolithic but it gain the theoritical advantage of micro kernel. it add the module at run time. It make the one executable file. e.g:-"linux"
 	
	advantage :- i) Because only one executable. so, no ipc is required. so, it is faster.
		    ii) We can insert the module as per the requirement.

/////////////////////////////////

// configuration of kernel
1) make O=build menuconfig
2) make O=build bzimage
3) make O=build modules
4) make O=build modules-install
5) make O=build install
6) update grub
7) reboot

// to uninstall the kernel module
1) make O=build modules-uninstall
2) make O=build uninstall

/////////////////////////////////

modes of execution:-
  1) usermode - 3GB
  2) kernel mode - 1GB

Mainly there are 4 mode.

VAS is divided into two address space i.e user and kernel. Two bits in the CS register define the mode of the execution. 00-kernel, 11-user. Kernel virtual address space is belong from 0xc0000000-0xffffffff, user address space is lower than 3g.In kernel mode we can access the whole 4g space but it is not possible in user mode. There are some register which can be used only in the kernel mode. They did it for the security.

//////////////////////////////////

// major and minor number :-

	Traditionally, the major number identifies the driver associated with the device. When there is more than one device of same type then the minor number is used by the deriver to determine exactly which device is being referred to.

	First 12 bit define the major no and remaining 20 bit define the minor no. The major number selects which device driver is being called to perform the input/output operation. The minor number define the actually which devices is going is to be used. If ther are single device in the system at that time minor number is not useful but like if there are two port of usb are using at that time minor number play the vital role.

///////////////////////////////////

// invoking a system call :-

system call means a function whose definition is defined in the kernel space. sysenter is used to generate a interrupt or the event. system call is maintain as array of function pointer. Here kernel shared the array index with the libc. Internally that index number will pass to the syscall. Every process having some register for the system call. In intel these are "eax, ebx, ecx, ebp, esi". eax is used for passing the array index and rest are using for passing the function parameter.

	main -> open -> Sysenter ------------------> int_handler -> __sys_open -> myopen

An application can invoke a system call by two ways :-
1) By executing the assembly instruction $0x80.
2) By executing the sysenter assembly language instruction.

an application can exit from a system call by two ways :-
1) By executing the iret assembly language instruction.
2) By executing the sysexit assembly language instruction.

TSS(task state segment) register is there for storing the kernel stack address. When it switch from the user to kernel it store all the register in the kernel stack by taking the address from the TSS register for returning back to the application from the kernel mode.


// device deriver

Deriver's just create the instance of the structure which contain the array of function pointer, it fill all the requirement by initializing the required capability and register it.

type of devices :-
1) block
2) character
3) network

cat /proc/devices = To show the major no allocated to each devices.

Deriver create the file, assign the major number to file for the application to access the device. Deriver place the major number in the "i_rdev" variable in the inode.

major number is used to identify the device in the system. when there are more than one device for the same deriver, at that time minor number is used. In i_rdev most significant 12 bit containing the major number and last 20 bit contain the minor number. for servicing anything in the kernel mode we have to pass the inode in the function. From inode it take the major and minor number.

through makefile we are linking the relocatable object file to the base kernel. kernel module are relocatable object file.

file_operations 	= registration form (filling the capability)
module_init 		= for giving the name to the first function.
module_exit		= for giving the name to the last function(cleanup function).
alloc_chrdev_region 	= request for major number and minor number (for registering a character device)
register_chrdev_region	= if already have the mojor number(it just giving the major number to the kernel)
cdev_add 		= to register the capability (it is the descriptor of the character device)
class_create,device_create = to create the file abstraction for the device
cdev_alloc		= to allocate the memory for the cdev_add
module_param		= to give the name at compile time 
minor number can we start as per our choice.

base kernel give a permission to the deriver that he can give any name to the first function. cleanup function do the complimentary action which is done by the init function. just like cdev_add to add the capability in init and cdev_del to delete in exit.

How to run the device code :-
make 
insmod 	devname=keshav
rmmod keshav

"lsmod" show all the kernel module in the system
dmesg is used to print the kernel message.
insmod
rmmod

export_module and module descriptor are the enteries in the symbol table.
modeprobe is used for adding and remove the modules from the linux kernel.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//						memory management

PGD(page global directory) it contain the virtual base address of kernel. Corresponding physical address is stored in the CR3 register.	register always stored the physical address.
UNIX is Uniplexed Information Computing System (UNICS), later known as UNIX.

 file is useful only upto its content is not copied into the memory after that it is not useful. when we give ./a.out, kernel start the reading of programme header. it will check the size of code and data, after checking it will directly copy the content of file in the memory page by page. for loading each section it divide the offset with the allign and that will be the offset in the file, according to that offset it copy the total pages. if u are closely see in the programme header u find that it have to copy 296 byte from the offset 3848. so if we are adding this we get 4144, which is out of the range of page. so, it will allocate two page memory at that time.
	symmetery in the address bring down the complexicity of the translation of virtual address to the physical address.

//////////////////////////////////////

// FCOM :- fixed constant offset mapped region

	At the time of initialization or boot initialization, kernel had built "master kernel page table". In that VA of 896 MB above from 0xc0000000 is directly mapped with the lower region of PA. 896 MB which present in the VA is FCOM region. After that there is DAMR region. 896 MB in the physical region can be used by the kernel as well as user,but when the user want to use that region at that time kernel have to do the dynamic mapping of that region. If the user memory is mapped in that region then that memory can be accesed by both kernel as well as user. That's why we called that dual mapping region. Be remember for kernel it is only mapping no page allocation but for the user page allocation is also done.

	VA = PA + 0xc0000000

	There is a physical memory which is mapped with kernel virtual address(i.e 896 MB). Only mapping is done not the allocation. we are doing this to save the time because every application required the kernel services and every time mapped the kernel region and storing the house keeping information will be a waste of time and memory, this time will be taken by each application.

///////////////////////////////////////

// DAMR :- Dynamic arbitrarly mappable region

	kernel module always required continuous memory. If it is not available in the FCOM region then it went for the DAMR region. The memory or page table assign for the DAMR is similar to the page table assign for the user application. So, kernel know how to deal with that region. 
	Be remember in that whole upper or lower case we have only 1 Gb of ram that is mapped with the lower memory of physical address, other peripheral is also there like I/O device etc.

///////////////////////////////////////

	Linker assign the virtual address for code and data but for heap and stack - kernel assign the virtual address. For code and data kernel directly take the virual address from the programme header. By the vm_file kernel comes to know that from where it have to take the virtual adderss. that's why for code and data vm_files can't be NULL in vm_area_struct.

	Kernel allocate a page for the process, it just add a node in the task_struct list. Kernel allocate a 132 kb of memory for heap and stack.kernel maintain a descriptor for the every resources. 

For every address kernel did the two type of validation :- 
1) Address legality validation 
2) Access legality validation

For arranging the large structure of vm_area_struct kernel used the red-black tree data structure.

cat /proc/pid/maps - it gives the information about the valid address region for a process.

mmap contain the base address of the first vm_area_struct. vm_flags contains the access permission of the page.

The page base address must be same in the VA as well as FO(file offset). That,s why we go for the last 3 nibbles, that must be 0. Kernel doing it for the symmetry.

.got is the part of the GNU_RELRO and .got.plt is the part of the .data section. paging can not avoid internal fragmentation.

code and GNU_RELRO are stored in the same page. According to allign(1000), file offset(0000) and size of data(1144). by calculating it come to know it required two pages.
																																														
symmetry bring down the complexicity from VA to PA.

mem_map contains the base address of the array of struct page table. struct page contain the all information about the page.

PA - unsigned int because can't be derefrenced.
VA - int* because it can be derefrenced.

The GNU.RELRO contain the information about the dynamic linking function.

Translation table :-
	total pages in ram = 4 Gb(ram size) / 4 Kb(page size) = 1 Mb pages

////////////////////////////////

1) 0th level translation table :-  In this translation table we directly put the VA address and corresponding PA. In best case where only 5 enteries are there it required only 4k and in worst case it required 8 Mb. But by putting like this searching is very tough. 

////////////////////////////////

2) 1st level translation table(20 - 12) :- In this we save the page according to their virtual address space. In this there is no need of storing virtual address in the table.
	e.g:- suppose we want to know the physical address of 0x08048524. Then we simply right shifted the address by 12 times we get 0x08048 we will go to this index in the translation table and we will pick the corresponding entry in the list. That will be the PA.
	In this searching is easy but so much memory is wasted. For each process either it is best or worst case 4 Mb is required.  

///////////////////////////////

3) 2nd level translation table(12-8-12) :- suppose i want to get the PA of the instruction 0x080 48 524. Then first of all i will go to the index 0x080 there i will get the next page which also contain the 256 entry of the next physical page. Then i will go to the 48 entry. That will contain the actual page. At that page i will mask last 12 bit and add 524. It will be my PA of corresponding VA. 
	Drawback is at 2nd phase there i can store 1024 entry but i am storing only 256 entry(memory = 256*4 = 1024 = 1k). so, 3k memory is directly wasted. 

  In best case it required 
	4096 entry * 4 = 4k (for 1st phase)
	 	       = 3k (for 2nd phase)
	4k + 3k = 7k

  In worst case
	4096 entry * 4 = 4k (for 1st phase)
	4k * 4k	       = 16Mb
	16Mb + 4k      = 16Mb

////////////////////////////////////

4) 2nd level translation table(10-10-12) :- suppose i want to get the PA of the instruction 0x080 48 524. Then first of all i will go to the index 0x020 there i will get the next page which also contain the 1024 entry of the next physical page. Then i will go to the 48 entry. That will contain the actual page. At that page i will mask last 12 bit and add 524. It will be my PA of corresponding VA. 
	
  In best case it required 
	4096 entry * 4 = 16384 byte = 4k (for 1st phase)
	 	       = 3k (for 2nd phase)
	4k + 3k = 7k

  In worst case
	4096 entry * 4 = 16384 byte = 4k (for 1st phase) = 1 page
	1024 * 4k      = 1k * 4k = 4Mb
	4Mb + 4k       = 4Mb

////////////////////////////////////////

At every phase the physical address contain some information about the page. last three nibble of page give that information. i.e:-

#define _PAGE_BIT_WT        0  /* W-bit   : write-through */
#define _PAGE_BIT_DIRTY     1  /* D-bit   : page changed */
#define _PAGE_BIT_SZ0       2  /* SZ0-bit : Size of page */
#define _PAGE_BIT_SZ1       3  /* SZ1-bit : Size of page */
#define _PAGE_BIT_EXECUTE   4  /* X-bit   : execute access allowed */
#define _PAGE_BIT_RW        5  /* AP0-bit : write access allowed */
#define _PAGE_BIT_USER      6  /* AP1-bit : user space access allowed */
#define _PAGE_BIT_BUFFER    7  /* B-bit   : bufferable */
#define _PAGE_BIT_GLOBAL    8  /* G-bit   : global (ignore ASID) */
#define _PAGE_BIT_CACHABLE  9  /* C-bit   : cachable */
#define _PAGE_BIT_PRESENT   10
#define _PAGE_BIT_ACCESSED  11 /* software: page was accessed */

Only virtual address is divided as user space and kernel space. Physical address space is not divided.

Q1) can we run a application which is more than 4gb ?

symbol table contain the symbol name and to which section it belong to, binding scope(whether local or global or static), value and offset within the respctive section which it belong to.

relocation table contain the offset within the text section or we can say the address of the instruction within the text section. it also give the symbol information.

"why we need virtual address":-
1) compiler don't know the physical address which is free.
2) any one process can go into the other process region and modify the data.
3) if there is a requirement of allocating the continuous memory like a[5000], then the kernel have to allocate a continuous chunk of memory.

programmme counter contain the virtual address. for converting VA to PA we used some h/w i.e called MMU. It contain the TLB(translation lookaside buffer)

hot pluggable :- which can be inserted at the working time. e.g:- pen-drive

we are using the kmap only when the PA is greater than the 896 MB. High mem is printing the starting region of DAMR.

when there is 512 mb ram. then whole ram is taken by the FCOM only. But still there will be DAMR region. Because suppose if we want to insert any kernel module(code and data). So, that must want the continuous memory but may or may not it will get. so, for removing that problem it will allocate that memory in the non-continuous way but it will give the virtual address continuously. The mapping is same as the mapping of the application.

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//						Task management

process :- A programme which is active in the system is k/n as process.

Through file command we can come to know whether the file is executable or not.

ps -t /dev/pts/2 -o pid,ppid,args,wchan    = This command is used to print the particular things of the process. Here 2 is the terminal name.

cat /proc/1205/status  =  It give the all attribute of the process.

synchronous means one by one. Other process will execute only when the current task will completed.

we can use the fork only when it is required. we can give the example like :-
	D = A + B * C

1 ADDER 
1 MULTIPLIER

Here no use of fork. because anyhow we have to synchronous way. but if we have the expression like :- 

	E = (A + B) * (C + D)

Here we can use fork because we have two expression for addition. So, different process can do that and finally that will merge and give a single result.

Fork is creating a duplicate of task_struct and copy the whole thing from the parent and update the pointer. Child does not inherit the alarm, cpu time usage from the parent.

When the child is going to exit it remove all the code and data pages but it will not remove the task_struct. That is removed by the parent only.

code,address space and data will be shared in the vfork(). So, anything which we are writing that will be writing in the  parent only untill the exec. vfork not sharing the file management. parent will be blocked.

///////////////////////////////////////////

// vfork

A process which is present in the system. programs that know they're finished and just want to run another program don't need to fork(),exec() and then wait()/waitpid() for the child. They can just load the child directly into their current process space with exec().

Linux also has a vfork(), even more optimised, which shares just about everything between the two processes. Because of that, there are certain restrictions in what the child can do, and the parent halts until the child calls exec() or _exit().

Bash is using the vfork and then exec. Vfork is create a child and waiting for the termination of the child. After termination of child it will not free the memory of task_struct, For free that memory we must have to use the wait system call. In vfork the parent is blocked untill the child does not terminate or it does not using the exec family function. 

When we created the child using vfork at that time parent will go to the task_unintrupptible state(in this state we can't give any signal to the parent like a device driver code). When the child is going to terminate or using exec family call then child pass SIGCHLD to the parent and parent will start execution.
The following diagram illustrates the typical fork/exec operation where the bash shell is used to list a directory with the ls command :

+--------+
| pid=7  |
| ppid=4 |
| bash   |
+--------+
    |
    | calls fork
    V
+--------+             +--------+
| pid=7  |    forks    | pid=22 |
| ppid=4 | ----------> | ppid=7 |
| bash   |             | bash   |
+--------+             +--------+
    |                      |
    | waits for pid 22     | calls exec to run ls
    |                      V
    |                  +--------+
    |                  | pid=22 |
    |                  | ppid=7 |
    |                  | ls     |
    V                  +--------+
+--------+                 |
| pid=7  |                 | exits
| ppid=4 | <---------------+
| bash   |
+--------+
    |
    | continues
    V

///////////////////////////////////////

//  Question :-

Q1 ) write a shell programme using fork and exec. Ensure that there will not be any zoombie process.

Q2 ) use exec in vfork().

Q3) WAP to create a child process using vfork and do the following :-
	a) verify the child is sharing the address space of the parent.
	b) verify the child is sharing the code/data frame of parent.
	c) verify the child is sharing the stack region of parent.
	d) verify the child is sharing the same stack frame of the parent's function which spawned a child using vfork.
	e) verify the child doesn't share the VFS data structure of it's parent process.

Q4) Write a shell programme using vfork and exec. Ensure that there will not retain any zoombie process.

	Experiment on copy on write
Q1) WAP to create a child process and print the code pages frame address in the parent and child context.

Q2) WAP to verify that parent and child will be pointing to the same data page and also verify the following:-
	a) On parent updating the data page, Parent will get a copy of data page.
	b) On child updating the data page, child will get a copy of data page.

/////////////////////////////////

// Thread

	Thread is a light weight process. We are using thread instead of using fork because when we are using thread then we are using at the same code, data and heap section. So, the directly communication is possible without any IPC mechanism. IPC mechanism is more costly in the system. So, we are using thread while in the fork for the communication we have to use the IPC.
	The main use of thread is used by the process in a way that when scheduler find the same tgid of the current running process and the upcoming then scheduler does not flush the TLB(translation lookup table) from the MMU and also the cache content.

	Thread second argument containing the attribute which is used to set the priority, scheduling policy of the thread. In case of thread having execution entity as shared semantics occured whereas in the process encapsulated semantics is occured. 
  ps -eLf  	=   This command is used for seeing the thread in the system.

In thread pid    = syscall(SYS_gettid)
	  tgid   = getpid()

threads share all segments except the stack.

In pthread_create the second argument is attribute which is used to set the policy, priority and other field of the thread.

	pthread_create
	pthread_join
	pthread_detach
	pthread_self
	pthread_exit

once the thread detach, then it can't be join to any thread.
 If we are using the pthread_exit in the main thread then it will wait to terminate the whole sub-thread. by using pthread_exit the main thread will become zoombie and it wait only for the termination of the sub-thread.

    we are prefering thread somewhere on process because :-
1) Easy to create for the kernel.
2) For communication no requirement of IPC.
3) No need of flushing the TLB if the same tgid is there of next process. 

Q1) creat a alarm process which should be asynchronous.

/////////////////////////////////////////

// clone

 	It is used to create the process as well as the thread. Task may be a thread or may be a process. Clone is not portable while pthread is portable that's why not used clone.

Q1) WAP to create thread and process using clone system call.

/////////////////////////////////////////
// cache

1) Buffer Cache :- The buffer cache contains data buffers that are used by the block device drivers.

2) Page Cache :- It is used to cache the logical contents of a file a page at a time and is accessed via the file and offset within the file. 

3) Swap Cache :- Only modified (or dirty) pages are saved in the swap file.

4) Hardware Caches :- One commonly implemented hardware cache is in the processor; a cache of Page Table Entries. In this case, the processor does not always read the page table directly but instead caches translations for pages as it needs them. These are the Translation Look-aside Buffers and contain cached copies of the page table entries from one or more processes in the system.

/////////////////////////////////////////

// copy on write

	When two process sharing the same page then count is two and in the page last 12 bit which are using for taking the information of the page that will be used for setting the page as a read only. Whenever anyone try to modified the page, at that time a exception is occured and the kernel will come to know about that exception. Now, kernel go to the vm_area_struct for getting the actual information about the page. There he find that it is r/w page. Then kernel go for a count, if the count more than 1, then kernel allocate a one more page to that procecss. Flags are stored in the vm_flags field of the vm_area_struct descriptor.

// important link :-

http://www.tldp.org/LDP/tlk/mm/memory.html     // memory mgmt

https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html      // for optimisation

https://www.cs.usfca.edu/~galles/visualization/AVLtree.html   // for avl tree visualisation

// when there is a wrong address then the os will terminate the process.

// there is always a fixed address of the global variable but not for the stack.

1) what is address space ?

2) Is processor cache present in the PT table.

3) If the page has been swapped then how the process to which the page is belong come to know that the page is not present in the memory ?

4) How the kernel know about the free memory in the ram.

5) When the structure vm_area_struct is used ?





"mm_struct" contain the start_code, end_code, start_data, end_data.

"mm_users" contain that how many light weight process is using the current process. "mm_count" is counter of memory descriptor used by the kernel process or thread. For clearing the memory of the mm_struct, both of the count must bre zero. We can use the mm_user instead of using the mm_count.

The "map_count" field of the memory descriptor contains the number of regions owned by the process. By default, a process may own up to 65,536 different memory regions. however, the system administrator may change this limit by writing in the "/proc/sys/vm/max_map_count" file.

red-black tree with n internal nodes has a height of at most 2 x log(n + 1). The head of the red-black tree is referenced by the "mm_rb" field of the memory descriptor.

when the exception occured at that time first of all "do_page_fault" occured. When the exception occurs, the CPU control unit stores that value in the "cr2" control register. The argument of the function is :-
	1) A variable which contain the base address of the structure which contain the value of all the register.
	2) A 3 bit error-code in which-
		0 bit:-
			clear - page not present
			set   - invalid address
		1 bit:-
			clear - Due to write access
			set   - Due to read and execute access
		2 bit:-
			clear - Process in kernel mode
			set   - Process in user mode

when it happened then first it check the reason of the page fault, it come to know that it is due to the writing in the page which is write protected. It called the function having name do_wp_page() wp stands for write protected. It check the _count value, through that it come to know whether it has to allocate a new page or not.
				



size of task_struct is 7680 byte.


 
///////////////////////////////////////////

Following are some typical situations in which a process gets new memory regions:-

1) when the user run command
2) when using the exec function
3) when the user open a file
4) when it cover the whole stack memory in that case it have to expand the stack size.
5) when user using the ipc.
6) At malloc time.

///////////////////////////////////////////
// Interrupts :-

An interrupt is usually defined as an event that alters the sequence of instructions executed by a processor.
interrupt are of two type :-
1) synchronous(exception) :- It is generated by the language. It is caused either by programming errors or by anomalous conditions that must be handled by the kernel. The kernel handles the exception by delivering to the current process one of the signals.

2) asynchronous :- It is generated by the H/W.

Both are finally merged into the signal. 

interrupt are of two type :-
1) maskable
2) non-maskable :- Only a few critical events (such as hardware failures) give rise to nonmaskable interrupts. Nonmaskable interrupts are always recognized by the CPU.

exception are of three type :-
1) Faults
2) Traps
3) Aborts

///////////////////////////////////////////

// memory mapping

	Whenever a command is executed, the file containing it is opened and its contents are mapped into the processes virtual memory. This is done by modifying the data structures describing this processes memory map and is known as memory mapping. However, only the first part of the image is actually brought into physical memory. The rest of the image is left on disk. As the image executes, it generates page faults and Linux uses the processes memory map in order to determine which parts of the image to bring into memory for execution.

///////////////////////////////////////////

// swapping

	Linux uses a Least Recently Used (LRU) page aging technique to fairly choose pages which might be removed from the system. This scheme involves every page in the system having an age which changes as the page is accessed. The more that a page is accessed, the younger it is; the less that it is accessed the older and more stale it becomes. Old pages are good candidates for swapping.

///////////////////////////////////////////

// Physical and Virtual Addressing Modes

It does not make much sense for the operating system itself to run in virtual memory. This would be a nightmare situation where the operating system must maintain page tables for itself. Most multi-purpose processors support the notion of a physical address mode as well as a virtual address mode. Physical addressing mode requires no page tables and the processor does not attempt to perform any address translations in this mode. The Linux kernel is linked to run in physical address space.

///////////////////////////////////////////

// state of task

Process execution :- process executing means it is taking the time of processor.

	To manage processes, the kernel must have a clear picture of what each process is doing. It must know, for instance, the process's priority, whether it is running on a CPU or blocked on an event, what address space has been assigned to it, which files it is allowed to address, and so on. This is the role of the process descriptor a task_struct type structure whose fields contain all the information
related to a single process.

the state field of the process descriptor describes what is currently happening to the process. It consists of an array of flags, each of which describes a possible process state. In the current Linux version, these states are mutually exclusive, and hence exactly one flag of state always is set, the remaining flags are cleared.

 The following are the possible process states:-
1) TASK_RUNNING
2) TASK_INTERRUPTIBLE :- sleep, read, select (we can send any signal)
3) TASK_UNINTERRUPTIBLE :- device installation
4) TASK_STOPPED :- By the signal
5) TASK_TRACED :- GDB
6) EXIT_ZOMBIE
7) EXIT_DEAD	

We are using TASK_UNINTERRUPTIBLE state for device deriver because if we left the device driver code in between then it might be given the unexpected output.
	Scheduler is a kernel function which is executed at various point of kernel.

cat /proc/12019/status :- To know the status of particular process.
cat /proc/12019/maps :- To show the memory allocated for each section.

Q1) Print the state of the process in the debug times.
 
////////////////////////////////////////////

// priority and scheduling

	A scheduler is a kernel function which will be executed at various points in the kernel.
POSIX has 140 priority.

	types of priority :-
1) Real time priority(0-99)
2) Non real time priority(100-139)

In POSIX 0 having lower priority while in the linux 0 having maximum priority.

Real time threads follow the sched policy :-
	sched-FIFO
	sched-RR(round robin)

Non real time follow the policy :
	CFS - completely fair scheduling.

For real time thread :-
When the threads in a ready queue then it maintain a one queue according to the priority. The main disadvantage of this is as long as there is a higher prioriry task, the lower priority task will not get the chance to be executed. Real time priority thread are mostly used in the embedded system. 


For normal thread :-
Here no priority concept is there. Directly it arrange all the task in the rb-tree. The left most node is going to executed according to their time.

///////////////////////////////////////

// Mutex and condition variable

 A  mutex  is a MUTual EXclusion device, and is useful for protecting shared data structures from concurrent modifications, and implementing critical sections and monitors.

Types :-
1) pthread_mutex_t fastmutex 
2) pthread_mutex_t recmutex
3) pthread_mutex_t errchkmutex 

Initializer :-
1) PTHREAD_MUTEX_INITIALIZER
2) PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
3) PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP

 The default initializer is "fast".

///////////////////////

  int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);

       pthread_mutex_init initializes the mutex object pointed to by mutex according to the mutex attributes  specified  in  mutexattr.   If
mutexattr is NULL, default attributes are used instead.

///////////////////////

   int pthread_mutex_trylock(pthread_mutex_t *mutex);

       pthread_mutex_trylock behaves identically to pthread_mutex_lock, except that it does not block the calling thread  if  the  mutex  is
already  locked  by another thread (or by the calling thread in the case of a "fast" mutex). Instead, pthread_mutex_trylock returns immediately with the error code EBUSY.

/////////////////////////////

   int pthread_mutex_unlock(pthread_mutex_t *mutex);

       If  the  mutex  is  already locked by the calling thread, the behavior of pthread_mutex_lock depends on the kind of the mutex. If the
mutex is of the ``fast'' kind, the calling thread is suspended until the mutex is unlocked,  thus  effectively  causing  the  calling thread  to  deadlock.  If  the  mutex  is  of the ``error checking'' kind, pthread_mutex_lock returns immediately with the error code EDEADLK.  If the mutex is of the ``recursive'' kind, pthread_mutex_lock succeeds and returns immediately,  recording  the  number  of times  the calling thread has locked the mutex. An equal number of pthread_mutex_unlock operations must be performed before the mutex returns to the unlocked state.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//						SIGNAL

Signal inform the process about the occurence of asynchronous events. A signal is a very short message that may be sent to a process or a group of processes. The only information given to the process is usually a number identifying the signal; there is no room in standard signals for arguments, a message, or other accompanying information.

	In real-time signals, there can be several pending signals of the same type.

When any process send the signal, in that it just set the corresponding bit in the variable of signal_struct. when the process comes for the execution then first it check for the signal value, checking of the variable value is done in every millisecond. If any bit is set in the variable then it go for checking the variable value of sigset_t blocked value. If the particular bit is set then the process will serve the particular signal else not.

//point :-

posix having 64 signals.
	1-31  -> non-real-time
	32-64 -> real-time

tkill and tgkill both having the same operation but there is one difference that is in tkill we have only two argument in which first one is PID and the signal number. But suppose there are multithread application is going on, may there will be a scenario that the two thread getting the same PID, to resolve this problem used tgkill because in that there is one more parameter is there i.e tgid. Now there will not be any contradiction because two process can't be having the same tgid.

	When we blocked any signal then one variable is there name is "blocked", the particular bit of the variable is set. 

3) do_signal() is used for checking that any signal is came or not. Handling of signal is done only when the process is scheduled.

4) in the signal function if we put 0 or 1 at the side of function pointer then it will treat as 0 for exit or 1 for ignore.

5) psig function is used for handling the signal.
 
6) Signals serve two main purposes:-
	1) To make a process aware that a specific event has occurred.
	2) To cause a process to execute a signal handler function included in its code.

7) default action of signal :-
	1) term
	2) ign
	3) core
	4) stop
	5) cont

8) We can't send the signal to process 0(swapper) and process 1(init).

9) we can send to signal to other process only when :-
	1) if the sending process have the super user permission.
	2) if your real id will match with the receiving process.

10) sigsuspend signal always return -1.
	sigsuspend and pause both having the same task but the difference is in sigsuspend we can make the process to not wait for the particular process but that is not possible in pause.

//////////////////////////////////////

# Signal     Default-action  Comment                                     POSIX

1 SIGHUP     Terminate       Hang up controlling terminal or process     Yes
2 SIGINT     Terminate       Interrupt from keyboard                     Yes          
3 SIGQUIT    Dump            Quit from keyboard                          Yes
4 SIGILL     Dump            Illegal instruction                         Yes
5 SIGTRAP    Dump            Breakpoint for debugging                    No
6 SIGABRT    Dump            Abnormal termination                        Yes
6 SIGIOT     Dump            Equivalent to SIGABRT                       No
7 SIGBUS     Dump            Bus error                                   No
8 SIGFPE     Dump            Floating-point exception                    Yes
9 SIGKILL    Terminate       Forced-process termination                  Yes
10 SIGUSR1   Terminate       Available to processes Yes
11 SIGSEGV   Dump            Invalid memory reference Yes
12 SIGUSR2   Terminate       Available to processes Yes
13 SIGPIPE   Terminate       Write to pipe with no readers Yes
14 SIGALRM   Terminate       Real-timerclock Yes
15 SIGTERM   Terminate       Process termination Yes
16 SIGSTKFLT Terminate       Coprocessor stack error No                  
17 SIGCHLD   Ignore          Child process stopped or terminated,         Yes
                             or got signal if traced 
18 SIGCONT   Continue        Resume execution, if stopped                 Yes
19 SIGSTOP   Stop            Stop process execution                       Yes
20 SIGTSTP   Stop            Stop process issued from tty                 Yes
21 SIGTTIN   Stop            Background process requires input            Yes
22 SIGTTOU   Stop            Background process requires output           Yes
23 SIGURG    Ignore          Urgent condition on socket                   No
24 SIGXCPU   Dump            CPU time limit exceeded                      No
25 SIGXFSZ   Dump            File size limit exceeded                     No
26 SIGVTALRM Terminate       Virtual timer clock                          No
27 SIGPROF   Terminate       Profile timer clock                          No
28 SIGWINCH  Ignore          Window resizing                              No
29 SIGIO     Terminate       I/O now possible                             No
29 SIGPOLL   Terminate       Equivalent to SIGIO                          No
30 SIGPWR    Terminate       Power supply failure                         No
31 SIGSYS    Dump            Bad system call                              No
31 SIGUNUSED Dump            Equivalent to SIGSYS                         No




A number of system calls allow programmers to send signals and determine how their processes respond to the signals they receive.

System call           Description

kill()                Send a signal to a thread group
tkill()               Send a signal to a process
tgkill()              Send a signal to a process in a specific thread group
sigaction()           Change the action associated with a signal
signal()              Similar to sigaction( )
sigpending()          Check whether there are pending signals
sigprocmask()         Modify the set of blocked signals
sigsuspend()          Wait for a signal


Each signal generated can be delivered once, at most. Signals are consumable resources, once they have been delivered, all process descriptor information that refers to their previous existence is canceled.

Signals that have been generated but not yet delivered are called pending signals . At any time, only one pending signal of a given type may exist for a process, additional pending signals of the same type to the same process are not queued but simply discarded.

The SIGKILL and SIGSTOP signals cannot be ignored, caught, or blocked, and their default actions must always be executed. Therefore, SIGKILL and SIGSTOP allow a user with appropriate privileges to terminate and to stop, respectively.

There are two exceptions: it is not possible to send a signal to process 0 (swapper), and signals sent to process 1 (init) are always discarded unless they are caught. Therefore, process 0 never dies, while process 1 dies only when the init program terminates.

Each signal sent to a multithreaded application will be delivered to just one thread, which is arbitrarily chosen by the kernel among the threads that are not blocking that signal. If a fatal signal is sent to a multithreaded application, the kernel will kill all threads of the application not just the thread to which the signal has been delivered.

The blocked field stores the signals currently masked out by the process. It is a sigset_t array of bits, one for each signal type:

Signal is a mechanism provide by the kernel to interact among the process. Every signal is delivered by the kernel only, any process can't send any signal to the other process. pause is wait for any signal while sigsuspend is wait for the particular signal.

In sigaction third argument is not required. signal is just giving the signal number whereas sigaction is giving the lot of information.
types :-
1) real time signal 
2) non-real time signal 

SIGFPE is raise by the h/w.
SIGSEGV is raise by the processor.

signal can be generated by the :-
1) process to process
2) kernel to process

There is one field in the task_struct that is TIF_SIGPENDING which is used for checking the signal. Whether any signal is coming or not. One function name do_signal() is used most probably after every instruction to check whether any signal is coming or not.

	When there is any invokation of the system call. then there is a exception is occuring. That exception is handled by the processor. Processor take the "GDTR(global descriptor table register)" register value and reach to the "GDT" table, From GDT table it take the value of "TSS(task stack segment)" value, TSS is again table. From that table it take the value of kernel stack pointer value and store all the user register value in the kernel stack. All these things is done by the processor. The kernel stack value is also present in the task_struct but it can't get from that side because h/w(processor) don't know about the task_struct, it only know about the register.

	At the time of creation of task_struct, the kernel stack is also created and after scheduling to that process the value of kernel stack address is updated in the TSS table. For every process minimum 1,2,4 according to the code the page is allocated for the kernel stack. 

	When the kernel have to run the user ISR, at that time kernel store all of his register at the user stack and excute the user ISR and get back to the kernel.

When we blocked any signal and we are passing the signal to that process at that time it set the bit of that signal and after that it check the bit in the sigset_t blocked and check the corresponding bit. It find set so it will not execute that signal but bit is set in task_struct, so whenever the signal is going to unblock state then it will be served.
	But in case of ignoring the signal the bit in the task_struct is not going to set. so, it will never be served.


// Programme for blocking/unblocking a signal :-

int main(int argc,  char *argv[])                                               
{                                                                               
    int i;                                                                      
    sigset_t intmask;                                                           
    sigset_t oldset;                                                            
                                                                                
    if (sigemptyset(&intmask) == -1) {                                          
        perror("Failed to initialize the signal mask");                         
        return 1;                                                               
    }                                                                           
                                                                                
    sigaddset(&intmask, SIGINT);                                                
    sigaddset(&intmask, SIGQUIT);                                               
    sigprocmask(SIG_BLOCK, &intmask, &oldset);                                  
                                                                                
    sleep(5);                                                               
    printf("time complete\n");                                                  

    sigprocmask(SIG_UNBLOCK, &intmask, &oldset);                                
    while (1);                                                                  
    return 1;                                                                   
}                                                                               


Q1) Use sigaction and print the sending process id in the handler.

Q3) Print the entry in the struct sigpending.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						virtual file system
	
	File is a logic continuity of data. From layman(user) view hard disk is like folder or file system hierachy. 

App
vfs
ext2
block device deriver
hard disk controller

pendrive having VFAT file system. in pen drive some space is using for storing the file system which it will use at the execution time. 
	
Hard disk contain the data and some info about the file system. linux support multiple file system due to VFS. file system see the hard disk as a seq of block like 1,2,3,4...  Driver see the hard disk as a cylinder, track and sector. When it is writing it will interact with the HD controller. HD controller is active device while HD is passive device.


The Virtual Filesystem is a kernel software layer that handles all system calls related to a standard Unix filesystem.Its main strength is providing a common interface to several kinds of filesystems. like VFAT, ext2 etc.

	The size of block depend on the partition. If the partition is less then the block size will be less, or vice versa.

main componnenrt of vfs :-
1) super block :- every file system contain the aggregate info about the partition. it maintain per file system.

2) inode :- every file having its file descriptor.That file descriptor is called inode. VFS maintain the inode in the inode struct

3) dentry :- It maintain per directory entry. Any folder entry like name of file, directory which are present in the folder is present in the dentry.

4) struct file :- when an app is engage with the file. Then kernel open a session for the file through which an application can communicate with file. struct file is a session descriptor.

5) struct addrss_space :- it hold the data of file in the form of radix tree. it also have  a page cache.


	kernel is maintaining a d-cache which hold the info of d-entry. i_data is a instance which hold the address space of the file. page_tree is used for storing the base address of the radix tree. every radix tree contain the slot and every slot contain 64 page frame which contain the actual data.

	Block device is the h/w. 
	application 	  = partition(folder)
	VFS				  = block
	hard disk deriver = cylinder, track

every resource in the kernel having the file descriptor. inode is the file descriptor. inode contain the file type, access permission, owner, directory entry, gp id, file time, size, no of block and pointer to the data block. It have 12 direct pointer to data block, 1 no of 1,2,3 level of indirection.

	The 1 level of indirection contain the pointer to 256 page, the 2 level of indirection contain the pointer to 256*256 page, the 3 level of indirection contain the pointer to 256*256*256 page. 
  
VFS divide the partition in the block.

inode block bitmap :- every bit represent the no of inode in the block group.
data block bitmap  :- same it represent the data block in the block group. 
super block 	   :- it contain the main file info. It present in every block. It contain the meta info of each block group.

formatting means just remove all the structure.

directory is also representing through the inode. if the inode contain the directory info then it finish only in the direct pointer. Directory entry contain the member like inode, rec_len, name_len, file_type, name. Here name having 255 bytes size.

	when we give any path for the file then it will reach to that through the root. That's why root is mandatory for the system. Root information is stored in the super block.

	when we are creating a soft link in that case it may be possible that inode is present in the one block and data is present in the other block group.
	Every bit in the bitmap is the index in inode table. every bit in the inode bitmap represent that whether that inode is free or not.

block group descriptor :- block group descriptor contain the information of the particular block. like :- block no of data block bitmap,block no of inode block bitmap, block no of first inode table, no of free block in the group, no of free inode in the group, no of directory in the group.

super block group :- it contain the aggregate information about the complete partition, no of free inode in the partition, no of free data block in partition, no of first useful block, file system size, no of block per group, no of inode per group, it contain the inode of root. it contain the size of the on disk inode structure. 

////////////////////////////

scenario 1 :-
	VFS inode, dentry & the pages in the page cache are present ?

////////////////////////////

scenario 2 :-
	VFS inode, dentry are present and page frames related to the file in the page cache are not present. In memory copy of the disk inode is present.

	EXT2 invoke the struct file, which having the one member f_mapping which point to the address space. b_read function is used to read the content from the hard disk according to their data block no. 1024 block are consuming by the inode table.

	here i have the VFS inode through that i went to the /dev/sda, it contain the same structure of vfs in that i went to that particular inode suppose 5000. so, it will be at the 632 block. in the radix tree each entry contain the one page. so 632 will be at the 158 array subscript. we went to that page and find the disk inode in that. because getting a file from the disk we must know about the disk inode.

	one buffer descriptor is there which contain the information about the each block which are in the ram. page cache contain the block and the buffer descriptor. 
Q 1) why 16 Gb is not possible ?
Q 2) how to open a file /home/trainee/file.c ? 

why we are going to data block everytime?

we have more no of inode as compare to the data block.
what will happen if we are creating the more no of soft link.
what will happen after getting the disk inode from the /dev/sda

//////////////////////////////

scenario 3 :-
	in section 3 we don't have the page in the /dev/sda. Now we will call the b_read function four times. that will update the table in the page cache.

///////////////////////////////

scenario 4 :- 
	File is not present in the disk. Hence, need to be created. First it will search and then it will go for checking the bit in the inode table and assign the corresponding inode in the table.

//////////////////////////////////////////////////////////////////////////////////////////////////

// Descriptors :-
 
 virtual address---------------	= vm_area_struct
 session descriptor------------	= struct file
 file descriptor---------------	= inode
 page frame--------------------	= struct page
 process descriptor------------	= task_struct
 memory descriptor-------------	= mm_struct
 signal descriptor-------------	= signal_struct
 signal handler descriptor-----	= sighand_struct
mem-map contains the base address of the array of struct page table.

//////////////////////////////////////////////////////////////////////////////////////////////////

//					swap in and swap out

The process of writing pages out to disk to free memory is called swapping-out. If later a page fault is raised because the page is on disk, in the swap area rather than in memory, then the kernel will read back in the page from the disk and satisfy the page fault. This is swapping-in.

	Thrashing reduce the speed of the system. 

vmstat 1 

//////////////////////////////////////////////////////////////////////////////////////////////////

sbrk() increments the program's data space by increment bytes.  Calling sbrk() with an increment of 0 can be used to find the current location of the program break.
mmap is used to map files or devices into memory.

nice command is used to giving the priority to the process.
nice matho-primes 0 9999999999 > /dev/null &
matho-primes 0 9999999999 > /dev/null &
// when the system getting hanged again and again then go to the "/var/log" and remove all file. For removing the file we can use rm -f *

1) use of kmap function ?
2) best case in 2nd level translation.
3) given by pthread_self ?
4) create the child and thread by using clone system call ?


interrupts :-
1) I/O interrupts
2) Timer interrupts
3) Interprocessor interrupts


use "critical section" word when explaining the semaphore and mutex.

a mutex is locking mechanism used to synchronize access to a resource. 
Semaphore is signaling mechanism (“I am done, you can carry on” kind of signal).

mutex can be used only in between the thread not in between the process. whenever we are going to change the value of the mutex in the child or parent it will give the separate copy.

The best way to avoid this problem is to make sure that whenever threads lock multiple mutexes, they do so in the same order. When locks are always taken in a prescribed order, deadlock should not occur. This technique is known as lock hierarchies.

//cache
. "Cached" is similar like "Buffers",


// priority inversion & priority inheritance
suppose there are two process p1 having the priority 10 and p2 having the priority 20. suppose higher priority having higher precedence. let p1 acquire the lock and p2 is waiting for the lock. In that case it will be priority inversion because when the process come which having the priority less than the p2 but greater than the p1, they will execute. 
	For removing that problem we used priority inheritance, in that the process which having the lowest priority will get the priority of the another process.


 a raw socket is an internet socket that allows direct sending and receiving of Internet Protocol packets without any protocol-specific transport layer formatting.


// Fragmentation 
	Fragmentation is something that primarily affects larger files that will not fit completely into the free space on a hard drive because of a lack of sufficiently large, contiguous space; thus, a file will reside in different segments.

types :-
1) External fragmentation :- It occurs when there is enough memory space to satisfy a request, but the spaces are not contiguous.

2) internal fragmentation :- The unused space remaining in between the page is called internal fragmentation.


// What is Paging?
	Paging is a memory management scheme that permits the physical address space of a 	process to be non-contiguous. So, external fragmentation is avoided by using paging.


// Why page size is 4k?
	Page size depends on architecture. But why intel kept it as 4k because if 2k page size was there then translation table size which converts page base address to page frame address would have been increased. If it was 8k size then internal fragmentation would have been more. So, to use the RAM effectively they went for 4k page size. 


///////////////////////////////////////////////////////////////////////////////////////////
	
A pipe is a form of redirection (transfer of standard output to some other destination) that is used in Linux and other Unix-like operating systems to send the output of one command/program/process to another command/program/process for further processing. The Unix/Linux systems allow stdout of a command to be connected to stdin of another command. You can make it do so by using the pipe character ‘|’.

///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
