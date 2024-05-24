#include <stdio.h>
int cnt = 0;
int flag = 0; //标识没有超过循环数量限制
//edi为输入的小于等于14的值, edx = 14, esi = 0
int fun_4(int esi, int edx, int edi)
{
    cnt ++;
    if(cnt >= 10000) {
        flag = 1;
        return 10086;
    }
    int eax = edx;
    eax -= esi;
    //eax = edx - esi;
    int ecx = eax;
    //ecx >>= 31;
    for(int i = 1; i <=31; i ++) ecx /= 2;
    eax += ecx;
    eax >>= 1;
    //eax = (eax + eax>>31)>>1;
    ecx = eax + esi;
    if(ecx > edi){
        edx = ecx - 1;
        return 2 * fun_4(esi, edx, edi);

    }else{
        eax = 0;
        if(ecx >= edi){
            return eax;
        }else{
            esi = ecx - 1;
            return 2 * fun_4(esi, edx, edi) + 1;
        }
    }
}
int main()
{
    for(int i = 0; i <= 14; i ++){
        cnt = 0;
        flag = 0;
        int res = fun_4(0, 14, i);
        if(!flag)
            printf("i=%d %d\n", i , res);
        else
            printf("i=%d No result\n", i);
    }
        

    return 0;
}