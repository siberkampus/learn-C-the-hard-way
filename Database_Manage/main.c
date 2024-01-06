#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#define MAX_DATA 512 //Satirda bulunabilecek karakter sayisi
#define MAX_ROWS 100 //Saklanabilecek toplam kayit sayisi
struct Address{ //Adres veritipi 
        int id;
        int set;
        char name[MAX_DATA];
        char email[MAX_DATA];
};
struct Database{ // Kayitlarin saklandigi veritabani
        struct Address rows[MAX_ROWS];
};

struct Connection{ // Kayitlarin oldugu veritabani dosyasi
        FILE *file;
        struct Database *db;
};

void die(const char *message){ // Hata ile karsilasinca programi sonlandirir
        if (errno){//Eger hata kodu varsa
                perror(message); //Hata mesajini ekrana yazdirir
                printf("%s",strerror(errno));//Error NO degerini ekrana yazdirir
        }else{
                printf("ERROR:\t%s\n",message);//Hata mesajini ekrana yazdirir
        }
        exit(1); //Programi sonlandirir
}
void  Database_load(struct Connection *conn){
        int rc = fread(conn->db,sizeof(struct Database),1,conn->file); //Dosyanin ilk satiri okunur
        if (rc!=1){//Eger okunmamissa ya da bossa hata verir
                die("Failed to load Database");
        }
}
struct Connection*  Database_open(char *filename, char action){
        struct Connection *conn= malloc(sizeof(struct Connection)); //Bellekte Connection degiskeni olusturulur
        if(conn==NULL){//Eger degisken olusturulamamissa hata verir
                die("Memory Error");
        }
        conn->db=malloc(sizeof(struct Database)); // Bellekte Database degiskeni olusturulur
        if(conn->db==NULL){ // Eger degisken olusturulamamissa hata verir
                die("Memory Error");
        }
        if(action=='c'){
                conn->file=fopen(filename,"w");
        }else{
                conn->file=fopen(filename,"r+");
                if (conn->file){
                        Database_load(conn);//Varolan dosya okunmak uzere gonderilir
                }
        }
        if (!conn->file){
                die("Failed to open file");
        }
        return conn; //Olusturulan dosya dondurulur
}

void Database_create(struct Connection *conn){
        int i=0;
        for (i=0;i<MAX_ROWS;i++){
                struct Address adr={.id=i,.set=0}; //Database kayitlari olusturulur ancak bostur
                conn->db->rows[i]=adr;
        }
}

void Database_write(struct Connection *conn){
        rewind(conn->file);
        int rc = fwrite(conn->db,sizeof(struct Database),1,conn->file);
        if (rc!=1){
                die("Failed to write Database");
        }
        rc=fflush(conn->file);
        if (rc==-1){
                die("Cannot flush database");
        }
}

void Address_print(struct Address *addr){
        printf("ID = %d Name = %s Email = %s\n",addr->id,addr->name,addr->email);//Kaydi ekrana yazar 
}

void Database_get(struct Connection *conn,int id){
        struct Address *addr = &conn->db->rows[id];//ID si belirtilen kaydi degiskene atar
        if (addr->set){
                Address_print(addr);
        }
}

void Database_set(struct Connection *conn, int id,char *name,char *email){
        struct Address *addr=&conn->db->rows[id];
        if (addr->set){//Eger set degeri 1 ise zaten kaydin olduguna dair hata mesaji verir
                die("Already set,delete it first");
        }
        addr->set=1;//Kaydin oludugunu belirtmek icin set degeri 1 yapilir
        char *res=strncpy(addr->name,name,MAX_DATA); 
        if(!res){
                die("Name copy failed");
        }
        res=strncpy(addr->email,email,MAX_DATA);
        if(!res){
                die("Email copy failed");
        }
}
void Database_delete(struct Connection *conn,int id){
        struct Address addr ={.id=id,.set=0}; //Kaydi silmek icin set degeri 0 yapilir
         conn->db->rows[id]=addr;
}

void Database_list(struct Connection *conn){
        int i=0;
        struct Database *db = conn->db;
        for (i=0;i<MAX_ROWS;i++){
                struct Address *cur = &db->rows[i];
                if (cur->set){//set degeri 0 dan farkliysa ekrana yazdirma yapilir
                        Address_print(cur);
                }
        }
}

void Database_close(struct Connection *conn){
		if (conn){
			if(conn->db){
				free(conn->db);
			}
			if(conn->file){
				fclose(conn->file);
			}
			free(conn);
		}

}
int main(int argc,char *argv[]){
        if (argc<3){//Komut satiri arguman sayisi kontrol edilir
                die("Usage main.c <dbfile> <action> [action parameters]");
        }
        char *filename=argv[1]; // Komut satiri argumanindan dosya adi okunur
        char action = argv[2][0]; // Komut satiri argumanindan aksiyon okunur
        struct Connection *conn = Database_open(filename,action); 
        int id=0;
        if (argc>3){
                id=atoi(argv[3]);//arguman sayisi 3 ten buyukse id degiskeni atanir
        }
        if (id >MAX_ROWS){
                die("There is not that many records");//Belirtilen sayidan buyuk ID'ye ulasmak istenirse hata verir
        }
        switch(action){
                case 'c': //Database Dosyasi olusturur
                        Database_create(conn);
                        Database_write(conn);
                        break;
                case 'g':
                        if (argc!=4){
                                die("Need an id to get");
                        }
                        Database_get(conn,id);
                        break;
                case 's':
                        if (argc!=6){
                                die("Need id, name, email to set");
                        }
                        Database_set(conn,id,argv[4],argv[5]);
                        Database_write(conn);
                        break;
                case 'd':
                        if (argc!=4){
                                die("Need id to Delete");
                        }
                        Database_delete(conn,id);
                        Database_write(conn);
                        break;
                case 'l':
                        Database_list(conn);
                        break;
                default:
                        printf("Invalid  Action c=create g=get s=set d=del l=list");
        }
        Database_close(conn);
        return 0;
}
