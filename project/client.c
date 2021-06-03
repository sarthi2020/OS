#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<string.h>
#include "constants.h"
// #define Port 5555

int display(int button);
void interface(int socket);
void SignUpInterface(int socket);
void SignInInterface(int socket);
void CreateAccount(int selection,int socket);
void SignInAccount(int selection,int socket);
void UserInterface(char* name, char* password,int selection,int socket);

int main(){
    struct sockaddr_in server,client;
    struct in_addr inadr;
    int sd,nsd;
    char buf[1024];
    
    sd = socket(AF_INET,SOCK_STREAM,0);
    if(sd == 0){
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_port = htons(Port);

    connect(sd,(void*)&client,sizeof(client));
    int selection = -1; 

    while(1){
 		interface(sd);
    }
    close(sd);
    return 0;
}

int display(int button){
	int selection;
	switch(button){
		case HomeScreen:
			printf("Hello user, how would like to continue?\n");
			printf("1 : Create account\n");
			printf("2 : Sign In\n");
			printf("3 : Exit\n");
			scanf("%d",&selection);
			switch(selection) {
				case 1:
					return SignUpOption;
				case 2:
					return SignInOption;
				case 3:
					return EXIT;
				default: 
					return INVALID;
			} 
		case SignUpOption:
			printf("Which account would like to create?\n");
			printf("1 : User account\n");
			printf("2 : Joint account\n");
			printf("3 : Admin account\n");
			scanf("%d",&selection);
			switch(selection) {
				case 1:
					return Create_User;
				case 2:
					return Create_Joint;
				case 3:
					return Create_Admin;
				default : 
					return INVALID ;
			} 
		case SignInOption:
			printf("How would you like to sign in?\n");
			printf("1 : User account\n");
			printf("2 : Joint account\n");
			printf("3 : Admin account\n");
			scanf("%d",&selection);
			switch(selection) {
				case 1:
					return SignIn_User;
				case 2:
					return SignIn_Joint;
				case 3:
					return SignIn_Admin;
				default : 
					return INVALID ;
			} 
		case UserOption:
			printf("What would you like to do?\n");
			printf("1 : Deposit\n");
			printf("2 : Withdraw\n");
			printf("3 : Check Balance\n");
			printf("4 : Change Password\n");
			printf("5 : View Account details\n");
			printf("6 : Exit\n");
			scanf("%d",&selection);
			switch(selection) {
				case 1:
					return Deposit;
				case 2:
					return Withdraw;
				case 3:
					return Balance;
				case 4:
					return Password;
				case 5:
					return Details;
				case 6:
					return Exit;
				default : 
					return INVALID ;
			} 
		case AdminOption:
			printf("What would you like to do?\n");
			printf("1 : Add User\n");
			printf("2 : Delete User\n");
			printf("3 : Modify User\n");
			printf("4 : View Account details\n");
			printf("5 : Exit\n");
			scanf("%d",&selection);
			switch(selection) {
				case 1:
					return Add_User;
				case 2:
					return Delete_User;
				case 3:
					return Modify_User;
				case 4:
					return Search_User;
				case 5:
					return Exit;
				default : 
					return INVALID ;
			}
		default:
			break; 

	}
}

void interface(int socket){
	int option = display(HomeScreen);
	if(option == SignUpOption)
		SignUpInterface(socket);
	else if(option == SignInOption)
		SignInInterface(socket);
	else if(option == EXIT){
		char str_selection[80];
		sprintf(str_selection,"%d",option);
		send(socket,str_selection,sizeof(str_selection),0);
		exit(EXIT_FAILURE);
	}
}

void SignUpInterface(int socket){
	int option = display(SignUpOption);
	if(option != -1)CreateAccount(option,socket);

}
void SignInInterface(int socket){
	int option = display(SignInOption);
	if(option != -1)SignInAccount(option,socket);
}

void CreateAccount(int selection,int socket){

	char* name = malloc(Buf_size * sizeof(char));
	char* password = malloc(Buf_size * sizeof(char));
	char str_selection[80];
	char return_msg[80];
	if(selection!=-1){
		printf("Enter your Account name\n");
		scanf("%s",name);
		printf("Enter password\n");
		scanf("%s",password);
			// printf("name %s password %s\n",name,password);
		sprintf(str_selection,"%d",selection);
		send(socket,str_selection,sizeof(str_selection),0);
		send(socket,name,Buf_size*sizeof(char),0);
		send(socket,password,Buf_size*sizeof(char),0);
	    recv(socket,return_msg,sizeof(return_msg),0);
	    printf("%s\n", return_msg);
	    if(!strcmp(return_msg,"Account created successfully\n"))
    		UserInterface(name,password,selection, socket);
    	}
    	else
    		printf("%s\n",return_msg);
}

void SignInAccount(int selection,int socket){

	char* name = malloc(Buf_size * sizeof(char));
	char* password = malloc(Buf_size * sizeof(char));

	char str_selection[80];
	char return_msg[80];

	if(selection!=-1){
		printf("Enter your Account name\n");
		scanf("%s",name);
		printf("Enter password\n");
		scanf("%s",password);
		// printf("name %s password %s\n",name,password);
		sprintf(str_selection,"%d",selection);
		send(socket,str_selection,sizeof(str_selection),0);
		send(socket,name,Buf_size*sizeof(char),0);
		send(socket,password,Buf_size*sizeof(char),0);
    	recv(socket,return_msg,sizeof(return_msg),0);
    	printf("%s\n", return_msg);
    	if(!strcmp(return_msg,"Login Successful\n"))
    		UserInterface(name,password,selection, socket);
	}
}

void UserInterface(char* name, char* password,int selection,int socket){
	char str_option[80];
	while(1){
		if((selection == SignIn_User) ||(selection == SignIn_Joint) ||(selection == Create_User) ||(selection == Create_Joint)){
			int option = display(UserOption);
			char* return_msg = malloc(Buf_size*sizeof(char));

			if(option == Deposit){
				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);
				send(socket,name,sizeof(name),0);
				int amount = 0;
				char amt_string[80];
				printf("How much amount do you want to deposit?\n");
				scanf("%d",&amount);

				sprintf(amt_string,"%d",amount);	
				send(socket, amt_string, sizeof(amt_string), 0);
				recv(socket,return_msg,Buf_size*sizeof(char),0);
				printf("%s\n\n", return_msg);
			}
			if(option == Withdraw){
				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);
				send(socket,name,sizeof(name),0);
				int amount = 0;
				char amt_string[80];
				printf("How much amount do you want to Withdraw?\n");
				scanf("%d",&amount);

				sprintf(amt_string,"%d",amount);	
				send(socket, amt_string, sizeof(amt_string), 0);
				recv(socket,return_msg,Buf_size*sizeof(char),0);
				printf("%s\n\n", return_msg);	
			}
			if(option == Balance){
				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);
				send(socket,name,sizeof(name),0);
				char return_msg[80];
				recv(socket,return_msg,sizeof(return_msg),0);
				printf("Balance in your account is: %s\n\n", return_msg);
			}

			if(option == Password){
				char password[80];

				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);
				send(socket,name,sizeof(name),0);
				
				printf("Type your new password:\n");
				scanf("%s",password);

				send(socket,password,sizeof(password),0);
				recv(socket,return_msg,Buf_size*sizeof(char),0);
				printf("%s\n\n", return_msg);
			}


			if(option == Details){
				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);
				send(socket,name,sizeof(name),0);
				recv(socket,return_msg,Buf_size*sizeof(char),0);
				printf("%s\n\n", return_msg);
			}
			if(option == Exit)
				break;
		}

		if((selection == SignIn_Admin) || (selection == Create_Admin)){
			int option = display(AdminOption);
			char* return_msg = malloc(Buf_size*sizeof(char));
			if(option == Add_User){
				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);
				
				char* name = malloc(Buf_size*sizeof(char));
				char* password = malloc(Buf_size*sizeof(char));
				int type;
				char str_selection[80];

				printf("Enter type of new account\n");
				printf("1: User Account\n");
				printf("2: Joint Account\n");
				scanf("%d",&type);
				if(type == 1) type = Create_User;
				if(type == 2) type = Create_Joint;

				printf("Enter Username of new account\n");
				scanf("%s",name);
				printf("Enter password\n");
				scanf("%s",password);
				sprintf(str_selection,"%d",type);
				send(socket,str_selection,sizeof(str_selection),0);
				send(socket,name,Buf_size*sizeof(char),0);
				send(socket,password,Buf_size*sizeof(char),0);

				recv(socket,return_msg,Buf_size*sizeof(char),0);
				printf("%s\n", return_msg);

			}
			if(option == Modify_User){
				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);

				char* name = malloc(Buf_size*sizeof(char));
				char* newname = malloc(Buf_size*sizeof(char));
				char* password = malloc(Buf_size*sizeof(char));

				printf("Enter old Username\n");
				scanf("%s",name);
				printf("Enter new Username\n");
				scanf("%s",newname);
				printf("Enter password\n");
				scanf("%s",password);
				send(socket,name,Buf_size*sizeof(char),0);
				send(socket,newname,Buf_size*sizeof(char),0);
				send(socket,password,Buf_size*sizeof(char),0);

				recv(socket,return_msg,Buf_size*sizeof(char),0);
				printf("%s\n", return_msg);

			}
			if(option == Delete_User){
				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);

				char* name = malloc(Buf_size*sizeof(char));

				printf("Enter name of the account to delete\n");
				scanf("%s",name);

				send(socket,name,Buf_size*sizeof(char),0);
				recv(socket,return_msg,Buf_size*sizeof(char),0);
				printf("%s\n", return_msg);

			}
			if(option == Search_User){
				sprintf(str_option,"%d",option);		
				send(socket,str_option,sizeof(str_option),0);
				char* name = malloc(Buf_size*sizeof(char));

				printf("Enter name of the account to find details of\n");
				scanf("%s",name);

				send(socket,name,Buf_size*sizeof(char),0);
				recv(socket,return_msg,Buf_size*sizeof(char),0);
				printf("%s\n", return_msg);

			}
			if(option == Exit)
				break;
		}
	}
}
