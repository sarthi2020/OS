#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<string.h>
#include<pthread.h>
#include "constants.h"
// #define Port 5555

void* threadhandler(void *sd);
void action(int selection,int socket);

int deposit(char* username, int amt);
int withdraw(char* username, int amt);
int balance(char* username);
int change_password(char* username,char* pwd);
char* get_details(char* username);

char* modify_User(char* username, char* newname, char* password);
char* delete_User(char* username);

int main(){

    struct sockaddr_in server,client;
    unsigned int sd,nsd,size; //server file descriptor, size, nsd
    char buf[1024]; //buffer 
    int opt = 1;
    // int selection;
    // char str_selection[80];
    // int signIn = 0;

    sd = socket(AF_INET,SOCK_STREAM,0);  //  or AF_INET for internet, TCP 
    if(sd == 0){
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }
 
    if(setsockopt(sd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT, &opt,sizeof(opt))){
	    perror("setsockopt");
	    exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port = htons(Port); //big endian (host to socket)
    
    if(bind(sd,(struct sockaddr *)&server,sizeof(server)) <0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if(listen(sd,5) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }   
    
    printf("This is server side\n");
    while(1){
    if((nsd = accept(sd,(struct sockaddr *)NULL,NULL)) <0){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        pthread_t thread_id;
        if(pthread_create( &thread_id, NULL, threadhandler, (void*) &nsd) < 0){
            perror("could not create thread");
            return 1;
        }
    }
    close(nsd);
    close(sd);
}

void* threadhandler(void *sd){
    int selection;
    char str_selection[80];
    unsigned int nsd;
        nsd = *(int*)sd;
    while(1){   
        printf("Listening\n");
        recv(nsd,str_selection,sizeof(str_selection),0);
        printf("Message from client: %s\n",str_selection);
        selection = atoi(str_selection);
        if(selection == EXIT)
            exit(EXIT_FAILURE);
        action(selection,nsd);
    }
}
void action(int selection,int socket){

    if((selection == Create_User) ||(selection == Create_Joint) ||(selection == Create_Admin)){
        char* name = malloc(Buf_size*sizeof(char));
        char* password = malloc(Buf_size*sizeof(char));
        
        recv(socket,name,Buf_size*sizeof(char),0);
        recv(socket,password,Buf_size*sizeof(char),0);
        printf("name %s password %s\n",name, password);
        
        char filename[Buf_size];
        strcpy(filename,name);
        char extension[5] = ".txt";
        int fd;
        strncat(filename,extension,sizeof(extension));
        
        if(open(filename,O_WRONLY) != -1){
            send(socket,"Username exists\n",sizeof("Username exists\n"),0);
        }
        else{

            fd = open(filename,O_WRONLY|O_CREAT, 0644);

            struct user u;
            strcpy(u.username,name);
            strcpy(u.password,password);
            if(selection == Create_User)
                strcpy(u.type,"UserAccount");
            else if(selection == Create_Joint)
                strcpy(u.type,"JointAccount");
            else if(selection == Create_Admin)
                strcpy(u.type,"AdminAccount");

            write(fd,&u,sizeof(struct user));
            struct account acc;
            acc.balance = 0;
            write(fd,&acc,sizeof(struct account));
            send(socket,"Account created successfully\n",sizeof("Account created successfully\n"),0);
        }
    }
    if((selection == SignIn_User) ||(selection == SignIn_Joint) || (selection == SignIn_Admin)){
        char* name = malloc(Buf_size*sizeof(char));
        char* password = malloc(Buf_size*sizeof(char));

        recv(socket,name,Buf_size*sizeof(char),0);
        recv(socket,password,Buf_size*sizeof(char),0);

        printf("name %s password %s\n",name, password);
        
        char filename[Buf_size];
        strcpy(filename,name);
        char extension[5] = ".txt";
        int fd;
        strncat(filename,extension,sizeof(extension));

        struct user u;        
        fd = open(filename,O_RDWR);
        if(fd == -1)
            send(socket,"User doesn't exist\n",sizeof("User doesn't exist\n"),0);
        else{
            read(fd,&u,sizeof(struct user));
            int flag = strcmp(password,u.password) 
                || ((selection == SignIn_User) && strcmp(u.type,"UserAccount")) || 
                ((selection == SignIn_Joint) && strcmp(u.type,"JointAccount")) || 
                ((selection == SignIn_Admin) && strcmp(u.type,"AdminAccount"));
            if(flag)
                send(socket,"User password or type doesn't match\n",sizeof("User password or type doesn't match\n"),0);
            else
                send(socket,"Login Successful\n",sizeof("Login Successful\n"),0);
        }
        close(fd);
    }
    if((selection == Deposit) || (selection == Withdraw) || (selection == Balance) || (selection == Password) || (selection == Details)){
            char* name = malloc(Buf_size*sizeof(char));
                        
            recv(socket,name,sizeof(name),0);
            char* str_acc = malloc(Buf_size*sizeof(char));

            if(selection == Deposit){
                int amount = 0;
                char str_amount[80];
                recv(socket,str_amount,sizeof(str_amount),0); 
                amount = atoi(str_amount);
                deposit(name,amount);
                str_acc = "Amount added successfully\n";
                send(socket,str_acc,Buf_size*sizeof(char),0);
            }

            if(selection == Withdraw){
                int amount = 0;
                char str_amount[80];
                recv(socket,str_amount,sizeof(str_amount),0);
                amount = atoi(str_amount);
                withdraw(name,amount);
                str_acc = "Amount withdraw successful\n";
                send(socket,str_acc,Buf_size*sizeof(char),0);
            }

            if(selection == Balance){
                int amount = 0;
                amount = balance(name);
                sprintf(str_acc,"%d",amount);        
                send(socket,str_acc,sizeof(str_acc),0);
            }

            if(selection == Password){
                char* password = malloc(Buf_size*sizeof(char));
                recv(socket,password,sizeof(password),0);
                change_password(name,password);
                str_acc = "Password change successful\n";
                send(socket,str_acc,Buf_size*sizeof(char),0);
            }

            if(selection == Details){
                int amount = 0;
                str_acc = get_details(name);
                send(socket,str_acc,Buf_size*sizeof(char),0);
            }

        }
    if((selection == Add_User) || (selection == Modify_User) || (selection == Delete_User) || (selection == Search_User)){                        
        char* str_acc = malloc(Buf_size*sizeof(char));

        if(selection == Add_User){
            char* name = malloc(Buf_size*sizeof(char));
            char* password = malloc(Buf_size*sizeof(char));
            char str_selection[80];

            recv(socket,str_selection,sizeof(str_selection),0);
            int type = atoi(str_selection);

            recv(socket,name,Buf_size*sizeof(char),0);
            recv(socket,password,Buf_size*sizeof(char),0);

            printf("name %s password %s\n",name, password);
        
            char filename[Buf_size];
            strcpy(filename,name);
            char extension[5] = ".txt";
            int fd;
            strncat(filename,extension,sizeof(extension));
        
            if(open(filename,O_WRONLY) != -1){
                str_acc = "Username exists\n";
                send(socket,str_acc,Buf_size*sizeof(char),0);
            }
            else{
                fd = open(filename,O_WRONLY|O_CREAT, 0644);

                struct user u;
                strcpy(u.username,name);
                strcpy(u.password,password);
                if(type == Create_User)
                    strcpy(u.type,"UserAccount");
                else if(type == Create_Joint)
                    strcpy(u.type,"JointAccount");

                write(fd,&u,sizeof(struct user));
                struct account acc;
                acc.balance = 0;
                write(fd,&acc,sizeof(struct account));
                close(fd);
                str_acc = "Account created successfully\n";
                send(socket,str_acc,Buf_size*sizeof(char),0);
            }
        }
        if(selection == Modify_User){
            char* name = malloc(Buf_size*sizeof(char));
            char* newname = malloc(Buf_size*sizeof(char));
            char* password = malloc(Buf_size*sizeof(char));

            recv(socket,name,Buf_size*sizeof(char),0);
            recv(socket,newname,Buf_size*sizeof(char),0);
            recv(socket,password,Buf_size*sizeof(char),0);

            str_acc = modify_User(name, newname, password);
            send(socket,str_acc,Buf_size*sizeof(char),0);

        }

        if(selection == Delete_User){
            char* name = malloc(Buf_size*sizeof(char));
            recv(socket,name,Buf_size*sizeof(char),0);

            str_acc = delete_User(name);
            send(socket,str_acc,Buf_size*sizeof(char),0);
        }

        if(selection == Search_User){
            char* name = malloc(Buf_size*sizeof(char));
            recv(socket,name,Buf_size*sizeof(char),0);
            
            str_acc = get_details(name);
            send(socket,str_acc,Buf_size*sizeof(char),0);
        }
    }
}

int deposit(char* username, int amt){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();
    
    char filename[Buf_size];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDWR,0744);

    struct account acc;
    fcntl(fd, F_SETLKW, &lock);
    lseek(fd,sizeof(struct user),SEEK_SET);
    read(fd,&acc,sizeof(struct account));
    
    acc.balance += amt;
    lseek(fd,sizeof(struct user),SEEK_SET);
    write(fd,&acc,sizeof(struct account));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 0;
}

int withdraw(char* username, int amt){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();

    char filename[Buf_size];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDWR,0744);

    struct account acc;
    fcntl(fd, F_SETLKW, &lock);
    // start of critical section
    lseek(fd,sizeof(struct user),SEEK_SET);
    read(fd,&acc,sizeof(struct account));

    acc.balance -= amt;

    if(acc.balance < 0) acc.balance = 0;
    lseek(fd,sizeof(struct user),SEEK_SET);
    write(fd,&acc,sizeof(struct account));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 0;
}

int balance(char* username){
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();

    char filename[Buf_size];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDONLY,0744);

    struct account acc;
    fcntl(fd, F_SETLKW, &lock);
    lseek(fd,sizeof(struct user),SEEK_SET);
    read(fd,&acc,sizeof(struct account));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return acc.balance;
}

int change_password(char* username, char* pwd){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user);
    lock.l_pid = getpid();

    char filename[Buf_size];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDWR,0744);

    struct user u;
    lseek(fd,0,SEEK_SET);
    fcntl(fd, F_SETLKW, &lock);
    read(fd,&u,sizeof(struct user));

    strcpy(u.password,pwd);
    lseek(fd,0,SEEK_SET);
    write(fd,&u,sizeof(struct user));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 0;
}

char* get_details(char* username){
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[Buf_size];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDWR,0744);
    char* return_string = (char*)malloc(Buf_size * sizeof(char));
    
    if(fd == -1)
        sprintf(return_string,"Username doesnt exists\n");
    else{
        struct account acc;
        struct user u;
        fcntl(fd, F_SETLKW, &lock);
        lseek(fd,0,SEEK_SET);
        read(fd,&u,sizeof(struct user));
        read(fd,&acc,sizeof(struct account));
        
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);
        close(fd);
        sprintf(return_string,"username: %s\npassword: %s\ntype: %s\nbalance: %d\n",u.username,u.password,u.type,acc.balance);
    }
    return return_string;
}


char* modify_User(char* username,char* newname,char* password){
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[Buf_size];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    
    char* return_string = (char*)malloc(Buf_size * sizeof(char));

    char type[1024];
    int balance;

    fd = open(filename,O_RDONLY);
    if(fd != -1){
        struct user u;
        struct account acc;
        if(fcntl(fd, F_SETLKW, &lock)==-1) {perror("fcntl");}
    // start of critical section
        lseek(fd,0,SEEK_SET);
        read(fd,&u,sizeof(struct user));
        read(fd,&acc,sizeof(struct account));
        // strcpy(u.username,name);
        // strcpy(u.password,password);
        strcpy(type,u.type);

        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);
        
        close(fd);
        remove(filename);

        char filename[Buf_size];
        strcpy(filename,newname);
        char extension[5] = ".txt";
        int fd1;
        strncat(filename,extension,sizeof(extension));

        fd1 = open(filename,O_WRONLY|O_CREAT, 0644);
        struct user u1;
        strcpy(u1.username,newname);
        strcpy(u1.password,password);
        strcpy(u1.type,type);
        printf("%s ds %s we%s cdf %d\n",u1.username,u1.password,u1.type,acc.balance );
        write(fd1,&u1,sizeof(struct user));
        write(fd1,&acc,sizeof(struct account));
        close(fd1);
        sprintf(return_string,"Update successful\n");
    }
    else
        sprintf(return_string,"Username doesnt exist\n");
    return return_string;
}

char* delete_User(char* username){

    char filename[Buf_size];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
            
    char* return_string = (char*)malloc(Buf_size * sizeof(char));

    if(remove(filename) == 0){
        sprintf(return_string,"Account deleted successfully\n");
    }
    else{
        sprintf(return_string,"Unable to delete account\n");
    }
    return return_string;
}