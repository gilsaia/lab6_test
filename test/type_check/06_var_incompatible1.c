/*
Contributor: Sakura
Date: 12/19/2020
Comment: it will cause error at " a = a % b; ", because an int var can't mod string.
*/
int main(){
	int a = 10;
	string b = "100";
	a = a % 100;
	a = a % b;
}