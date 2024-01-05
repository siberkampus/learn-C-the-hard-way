#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct Person{ //Kişi veri tipi oluşturuyoruz
        char *name;
        int age;
        int height;
        int weight;
};
struct Person* person_create(char *name,int age,int height,int weight){
        struct Person *who=malloc(sizeof(struct Person)); //Bellekte Person değişkeni kadar alan ayırıyoruz
        assert(who!=NULL); 
        /* Eğer bellekte yer yoksa malloc fonksiyonu NULL dönderecektir.
         Eğer Null ise assert hata verecektir*/
        who->name=strdup(name);// karakter dizisini who->name kısmına yazıyoruz.
        who->age=age;
        who->height=height;
        who->weight=weight;
        return who; // Oluşturduğumuz Kişiyi gönderiyoruz

}
void print_person(struct Person *who){//Parametre olarak  Person değişkeni alıyoruz
        printf("Name = %s\n",who->name);
        printf("Age = %d\n",who->age);
        printf("Height = %d\n",who->height);
        printf("Weight = %d\n",who->weight);
        //Aldığımız Person değişkeninin değerlerini yazdırdık
}
void destroy_person(struct Person *who){
        assert(who!=NULL);//Gelen Person boş mu diye kontrol ediyoruz.Eğer boş ise hata verecektir.
        free(who->name);//oluşturduğumuz char dizisini serbest bırakıyoruz
        free(who);//Oluşturduğumuz person veri tipini serbest bırakıyoruz

}
int main(){
        struct Person *joe = person_create("Joe Alex",32,180,78); 
        struct Person *frank = person_create("Frank Blank",35,190,87);

        print_person(joe);
        print_person(frank);

        destroy_person(joe);
        destroy_person(frank);
        return 0;
}
