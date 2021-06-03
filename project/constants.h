#define Port 5555 

// possible menus
#define HomeScreen 0
#define SignUpOption 1
#define SignInOption 2
#define UserOption 3
#define AdminOption 4

// possible options
#define SignUp 5
#define SignIn 6
#define Create_User 7
#define Create_Joint 8
#define Create_Admin 9
#define SignIn_User 10
#define SignIn_Joint 11
#define SignIn_Admin 12
#define Deposit 13
#define Withdraw 14
#define Balance 15
#define Password 16
#define Details 17
#define Exit 18
#define Add_User 19
#define Modify_User 20
#define Delete_User 21
#define Search_User 22
#define EXIT 23
#define INVALID -1
#define Buf_size 1024

//struct user
struct user {
    char type[Buf_size];
    char password[Buf_size];
    char username[Buf_size];
};

//struct details
struct account {
    int balance;
};



