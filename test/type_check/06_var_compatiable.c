/*
Contributor: Sakura
Date: 12/20/2020
Comment: var may cause error at lineno 9 (a = a + '5';) , 
			 must cause error at lineno 10 (a = a + "5";) ,
*/
int main(){
	int a;
	a = a + 5;
	a = a + '5';
	a = a + "5";
	return 0;
}