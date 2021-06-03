OS-mini-project

Overview

The mini project aims to develop a banking system that is user-friendly and has following feaatures:-

Has a login system to ensure the data of user is protected and is prevented from unauthorized access.
Supports 3 types of users - normal, joint, admin user
Supports basic banking functionanility ie authentication, deposit and withdrawal money, check account balance and modify account details.
Admin users have priviledge to add, delete, modify and view other accounts.
Functionality Provided

Create Account

Different options are provided to create account ie normal, joint and admin. One can't create an account if the username exists. Account is created with 0 balance.

Sign In

Username, password and type of account are required to signIn. If either of them is incorrect then signIn fails

Deposit

User can define the amount and deposit in his/her account.

Withdraw

User can define the amount and deposit in his/her account

Balance

User can check his/her balance.

Change Password

User can change password which is required at the time of login.

View Details

Will return details about user ie username, password, account type and balance.

Exit

User can sign out.

Admin Priviledges

Add User

Admin can create user or joint account.

Delete User

Admin can delete account.

Modify User

Admin can modify username or passwords of existing accounts.

Get User Details

Used to get details about other accounts.

File Structure

constants.h

Holds constant values are specified here.

client.c

UI for the client is implemented here. Based on the way user interacts the data is collected and shared with server.

server.c

The server designed is a concurrent server. This is the backend of our banking app. For every client, the server creates a new thread in order to service it further.

Overview of Working

The server creates a new file for every user who signs up. The name of this file is username.txt. If a file with a given username already exists, then that user cannot sign up. Every file contains first a struct which has user details - username, password and type - followed by a struct which has account details - the balance amount. All queries are answered by opening the corresponding file, and reading from it. In order to delete a user, the corresponding file is deleted.

Commands to compile and run

To Compile

To compile server file: ./CreateServer / gcc server.c -lpthread -o server To compile client file: ./CreateClient / gcc client.c -lpthread -o client

To Execute

To execute server : ./server To execute client : ./client

Commands to clear

./Clean
