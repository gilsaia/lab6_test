/*
Contributor: Sakura
Date: 12/19/2020
Comment: it will cause error at "p = p + 15;" , string shall support add to a char, but not add to a number
*/
int main(){
    string p = "123";
    p = p + 'a';
	p = p + 15;
    return 0;
}