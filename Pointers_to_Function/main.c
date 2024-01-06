#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void die(char *message){//programi sonlandiran fonksiyon
        if(errno){
                perror(message);
        }else{
                printf("ERROR %s",message);
        }
        exit(1);
}
int sorted_order(int a,int b){
        return a-b;
}
int reverse_order(int a,int b){
        return b-a;
}

int* bubble_sort(int *numbers,int count,int (*fun)(int,int)){
        int *target = malloc(count*sizeof(int)); //yeni dizi olusturulur
        if(!target){ //dizi bossa hata verir
                die("Memory Error");
        }
        memcpy(target,numbers,count*sizeof(int));//orjinal dizimizi yeni olusturdugumuz diziye kopyaliyoruz
        int i,j,tmp;
        for(i=0;i<count;i++){
                for(j=0;j<count-1;j++){
                        if(fun(target[j],target[j+1])>0){ //bubble sort algoritmasi
                                tmp=target[j];
                                target[j]=target[j+1];
                                target[j+1]=tmp;
                        }
                }
        }
        return target;//olusturdugumuz diziyi gonderir
}
void test_sorting(int *numbers,int count,int (*fun)(int,int)){
        int *sorted=bubble_sort(numbers,count,fun); //siralanmis dizi sorted dizisine yazilir
        if(!sorted){ // Eger dizi bossa program sonlanir
                die("Sorting Error");
        }
        int i;
        for(i=0;i<count;i++){
                printf("%d  ",sorted[i]); //siralanmis dizi ekrana yazdirilir
        }
        printf("\n");
        free(sorted); //olusturulan dizi serbest birakilir
}
int main(int argc,char *argv[]){
        if(argc<2){
                die("Usage main.c 1 2 3 4 5 ");//Girilen arguman 2 den kucukse hata verir
        }
        int count = argc-1; // arguman sayisini aliyoruz
        char **arr = argv+1; // girilen parametre dizisini alıyoruz
        int *numbers=malloc(count*sizeof(int)); //Girdi boyutunda int dizi olusturuyoruz
        if(!numbers){ // Eger  dizi olusmazsa program sonlanir
                die("Memory Error");
        }
        int i;
        for(i=0;i<count;i++){
                numbers[i]=atoi(arr[i]); //Kulanici girdileri int e cevrilir
        }
        test_sorting(numbers,count,sorted_order);
        test_sorting(numbers,count,reverse_order);
        free(numbers);//numbers dizisi serbest birakilir
        return 0;
}
