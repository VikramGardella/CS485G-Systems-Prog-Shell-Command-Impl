#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fstream>

using namespace std;

bool qt = false; //quit boolean, not entirely necessary but just in case
string loc = "shell>"; //location for current location, modifiable later on
int s; //size of args[] kept global no not needed as func. param.

int sz(char** a){//return number of non-NULL elements in char** "a"
	int size;
	for(size=0; a[size]!=NULL; size++);
	return size;
}

void dopwd(){
	system("pwd"); //prints working directory
}

//changes directory
void cd(char* ar[]){
	int suc;//indicates whether chdir could find and change a dir by name given
	if(ar[1]!=NULL){//meaning an argument that may be a name of dir exists
		suc = chdir(ar[1]);//will be 0 if successful, -1 if not
	}
	else{
		suc = chdir(getenv("HOME"));
	}
	if(suc!=0){//chdir couldn't find the directory
		string finerr = "";//using string because dynamic and easier concatenation
		finerr += "Could not chdir to";
		for(int i=1; i<s; i++){//putting string together
			finerr += "/";
			finerr += ar[i];
		}
		finerr += ": No such file or directory";
		const char* f = finerr.c_str();//converting string to char array
		perror(f);//finally prints error using char* form
	}
}

//sets environment variable
void sete(char* ar[]){
	int suc;
	if(ar[1]!=NULL && ar[2]!=NULL){//if 2 more arguments exist
		suc = setenv(ar[1],ar[2],true);//assumes 1st is var. name and 2nd is value to set
	}
	else if(ar[1]!=NULL){//name of var. was specified but no value was
		suc = unsetenv(ar[1]);//meaning unset variable if it exists
	}//if it doesn't, then suc will be -1
	else
		perror("Not enough arguments to setenv");
	if(suc!=0)
		perror("Could not setenv: Invalid argument");
}

//changes boolean so main while wont repeat
void ex(){
	qt = true;//sets quit boolean to true just in case
	exit(0);//exit status 0 means successful termination
}

//for external commands and redirection
void extcom(char* ar[]){
	//fork();
}

//prints within shell
void echo(char* ar[]){
	for(int i=1; i<s; i++){
		cout << ar[i];//printing each substring separated by a space
		if(i!=(s-1))
			cout << " ";
	}
	cout << endl;
}

//determines command
void detcom(command* com){

	s = sz(com->args);//updating global variable "s" (size of command args)
	if(com->args[0] == NULL){
		//empty command line
	}
	else if(strcmp(com->args[0],"cd")==0){
		cd(com->args);
	}

	else if(strcmp(com->args[0],"setenv")==0){
		sete(com->args);
	}

	else if(strcmp(com->args[0],"echo")==0){
		echo(com->args);
	}
	else if(strcmp(com->args[0],"exit")==0){
		ex();
	}
	else if(strcmp(com->args[0],"pwd")==0){
		dopwd();
	}
	else{//must be external command or gibberish at this point
		perror("Command returned 1");
	}

}

int main(int argc, char * argv[]){
	string userin;
	if(argv[1]== NULL){//argv[1] would be an input file in this case
		while(!qt){//this loop is for keyboard input for the shell
			cout << loc;
			getline(cin, userin);
			const char* uscom = userin.c_str();
			struct command* co = parse_command(uscom);
			detcom(co);
			free_command(co);
		}
	}
	else{//this loop is for when an input file is present as an argument
		ifstream file;
		file.open(argv[1]);
		if(!(file.is_open()))
			exit(1);
		while(!qt){
			string line;
			getline(file, line);
			const char* uscom = line.c_str();
			struct command* co = parse_command(uscom);
			detcom(co);
			free_command(co);
		}

	}

	return 0;
}
