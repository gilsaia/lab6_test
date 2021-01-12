#include<cstdlib>
#include<cstdio>
#include<unistd.h>
#include<getopt.h>
#include<string>
#include<vector>
#include<fstream>
using namespace std;
extern int optind,opterr,optopt;
extern char *optarg;
static struct option long_options[] = 
{
    {"target",required_argument,NULL,'t'},
    {"type",no_argument,NULL,'y'},
    {"level",required_argument,NULL,'v'},
    {"different",no_argument,NULL,'d'},
    {"gilsaia",no_argument,NULL,'g'},
    {"compiler",required_argument,NULL,'c'},
    {"nolog",no_argument,NULL,'e'},
    {"diffcheck",no_argument,NULL,'f'},
};
int private_mode=0;
int target=0;
int level=0;
int different=0;
int type_check=0;
int err_log=1;
int diff_check=0;
FILE *err_file;
string opt_lev,opt_tar,opt_compiler("./main.out"),std_compiler("gcc -m32"),std_qemu("qemu-i386"),opt_err(" 2>>err.log"),std_asmopt(" -O0 -fno-asynchronous-unwind-tables -fno-builtin -fno-common -fno-ident -finhibit-size-directive -fno-pie -march=i386");
void getarg(int argc,char *argv[]){
    int c = 0;
    int index;
    while(EOF != (c = getopt_long(argc,argv,"t:v:defc:",long_options,&index)))
    {
        switch(c)
        {
            case 'g':
                printf("private mode open!\n");
                private_mode=1;
                opt_compiler=string("gcc -m32 -S");
                break;
            case 't':
                opt_tar=string(optarg);
                if(!opt_tar.compare("x86")){
                    target=0;
                }else if(!opt_tar.compare("arm")){
                    std_compiler=string("arm-linux-gnueabi-gcc -static");
                    std_qemu=string("qemu-arm");
                    std_asmopt=string("");
                    target=1;
                }else{
                    printf("Wrong Target!Use Default Target\n");
                }
                break;
            case 'v':
                opt_lev=string(optarg);
                if(opt_lev.length()>1||opt_lev.length()==0||opt_lev[0]>'4'||opt_lev[0]<'0'){
                    printf("Wrong Level!Use Default Level\n");
                }else{
                    level=opt_lev[0]-'0';
                }
                break;
            case 'd':
                different=1;
                break;
            case 'c':
                opt_compiler=string(optarg);
                break;
            case 'y':
                type_check=1;
                break;
            case 'e':
                opt_err=string();
                err_log=0;
                break;
            case 'f':
                diff_check=1;
                break;
            case '?':
                // printf("unknow option:%c\n",optopt);
                break;
            default:
                break;
        }   
    }
}
void printInfo(){
    printf("------ Test Info ------\n");
    printf("Target\t\t%s\n",target?"arm":"x86");
    if(diff_check){
        printf("Diff Check\t True\n");
    }else if(type_check){
        printf("Type Check\tTrue\n");
    }else{
        printf("Personal Code\t%s\n",different?"True":"False");
    }
    printf("Level\t\t%d\n",level);
    printf("Compiler\t%s\n",opt_compiler.c_str());
    printf("Error Log\t%s\n",err_log?"True":"False");
    printf("------ Info End  ------\n");
    system("rm -rf err.log");
    return;
}
vector<string> testlist;
void addTestfile(){
    FILE *fp;
    char file_name[120];
    if(type_check){
        fp=popen("find test/type_check/*.c","r");
        while(fscanf(fp,"%s",file_name)!=EOF){
            testlist.push_back(string(file_name));
        }
        pclose(fp);
    }else if(level){
        char tmp[120];
        sprintf(tmp,"find test/%d/*.c",level);
        fp=popen(tmp,"r");
        while(fscanf(fp,"%s",file_name)!=EOF){
            testlist.push_back(string(file_name));
        }
        pclose(fp);
    }else{
        fp=popen("find test/?/*.c","r");
        while(fscanf(fp,"%s",file_name)!=EOF){
            testlist.push_back(string(file_name));
        }
        pclose(fp);        
    }
    for(auto &i:testlist){
        i=i.substr(0,i.length()-2);
    }
}
void check_type(){
    printf("---Begin Type Check---\n");
    for(auto i:testlist){
        printf("%s......",i.substr(i.rfind("/")+1).c_str());
        string cmd=opt_compiler+(private_mode?" ":" <")+i+".c"+(private_mode?" -o "+i+".res":" >"+i+".res")+opt_err;
        int err;
        if(err=system(cmd.c_str())){
            if(err_log){
                printf("Failed\n");
                fprintf(err_file,"Wrong Cmd in %s Err %d\n",cmd.c_str(),err);
                fflush(err_file);
            }else{
                printf("Failed\nWrong Cmd in %s Err %d\n",cmd.c_str(),err);
            }
        }else{
            printf("Done!\n");
        }
    }
    printf("---End Type Check---\n");
}
void genstdasm(){
    printf("---Begin Generate Standard Assembly---\n");
    for(auto i:testlist){
        printf("%s......",i.substr(i.rfind("/")+1).c_str());
        string cmd=std_compiler+" -w -S "+i+".c"+" -o "+i+"_std.s"+std_asmopt+opt_err;
        int err;
        if(err=system(cmd.c_str())){
            if(err_log){
                printf("Failed\n");
                fprintf(err_file,"Wrong Cmd in %s Err %d\n",cmd.c_str(),err);
                fflush(err_file);
            }else{
                printf("Failed\nWrong Cmd in %s Err %d\n",cmd.c_str(),err);
            }
        }else{
            printf("Done!\n");
        }
    }
    printf("---End Generate Standard Assembly---\n");

}
void genstdout(){
    printf("---Begin Generate Standard Output---\n");
    for(auto i:testlist){
        printf("%s......",i.substr(i.rfind("/")+1).c_str());
        string cmd=std_compiler+" -w "+i+".c"+" -o "+i+"_std.out"+opt_err;
        int err;
        if(err=system(cmd.c_str())){
            if(err_log){
                printf("Failed\n");
                fprintf(err_file,"Wrong Cmd in %s Err %d\n",cmd.c_str(),err);
                fflush(err_file);
            }else{
                printf("Failed\nWrong Cmd in %s Err %d\n",cmd.c_str(),err);
            }
            continue;
        }
        cmd=std_qemu+" "+i+"_std.out"+" <"+i+".in"+" >"+i+"_std.output"+opt_err;
        err=system(cmd.c_str());
        printf("Done!\n");
    }
    printf("---End Generate Standard Output---\n");
}
void genasm(){
    printf("---Begin Generate Assembly---\n");
    for(auto i:testlist){
        printf("%s......",i.substr(i.rfind("/")+1).c_str());
        string cmd=opt_compiler+(private_mode?" -w -x c ":" <")+i+(different?".sy":".c")+(private_mode?" -o ":" >")+i+".s"+opt_err;
        int err;
        if(err=system(cmd.c_str())){
            if(err_log){
                printf("Failed\n");
                fprintf(err_file,"Wrong Cmd in %s Err %d\n",cmd.c_str(),err);
                fflush(err_file);
            }else{
                printf("Failed\nWrong Cmd in %s Err %d\n",cmd.c_str(),err);
            }
            continue;
        }
        printf("Done!\n");
    }
    printf("---End Generate Assembly---\n");
}
void genout(){
    printf("---Begin Generate Output---\n");
    for(auto i:testlist){
        printf("%s......",i.substr(i.rfind("/")+1).c_str());
        string cmd=std_compiler+" "+i+".s"+" -o "+i+".out"+opt_err;
        int err;
        if(err=system(cmd.c_str())){
            if(err_log){
                printf("Failed\n");
                fprintf(err_file,"Wrong Cmd in %s Err %d\n",cmd.c_str(),err);
                fflush(err_file);
            }else{
                printf("Failed\nWrong Cmd in %s Err %d\n",cmd.c_str(),err);
            }
            continue;
        }
        cmd=std_qemu+" "+i+".out"+" <"+i+".in"+" >"+i+".output"+opt_err;
        err=system(cmd.c_str());
        printf("Done!\n");
    }
    printf("---End Generate Output---\n");
}
void cmpout(){
    printf("---Begin Compare Output---\n");
    for(auto i:testlist){
        printf("%s......",i.substr(i.rfind("/")+1).c_str());
        FILE *fp=fopen((i+".output").c_str(),"r");
        if(!fp){
            printf("Failed!\n");
            continue;
        }else{
            fclose(fp);
        }
        string cmd=string("diff")+" -q "+i+".output"+" "+i+"_std.output";
        fp=popen(cmd.c_str(),"r");
        char tmp[120];
        if(fscanf(fp,"%s",tmp)!=EOF){
            printf("Failed!\n");
        }else{
            printf("OK!\n");
        }
    }
    printf("---End Compare Output---\n");
}
int main(int argc,char *argv[])
{
    getarg(argc,argv);
    printInfo();
    err_file=fopen("err.log","a");
    addTestfile();
    if(diff_check){
        genstdasm();
    }else if(type_check){
        check_type();
    }else{
        genstdout();
        genasm();
        genout();
        cmpout();
    }
    fclose(err_file);
    return 0;
}