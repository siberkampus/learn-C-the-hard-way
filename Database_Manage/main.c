#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#define MAX_DATA 512 //Satırda bulunabilecek karakter sayısı
#define MAX_ROWS 100 //Saklanabilecek toplam kayıt sayisi
struct Address{ //Adres veritipi 
        int id;
        int set;
        char name[MAX_DATA];
        char email[MAX_DATA];
};
struct Database{ // Kayıtların saklandığı veritabanı
        struct Address rows[MAX_ROWS];
};

struct Connection{ // Kayıtların olduğu veritabanı dosyası
        FILE *file;
        struct Database *db;
};

void die(const char *message){ // Hata ile karşılaşınca programı sonlandırır
        if (errno){//Eğer hata kodu varsa
                perror(message); //Hata mesajını ekrana yazdırır
                printf("%s",strerror(errno));//Error NO değerini ekrana yazdırır
        }else{
                printf("ERROR:\t%s\n",message);//Hata mesajını ekrana yazdırır
        }
        exit(1); //Programı sonlandırır
}
void  Database_load(struct Connection *conn){
        int rc = fread(conn->db,sizeof(struct Database),1,conn->file); //Dosyanın ilk satırı okunur
        if (rc!=1){//Eğer okunmamışsa ya da boşsa hata verir
                die("Failed to load Database");
        }
}
struct Connection*  Database_open(char *filename, char action){
        struct Connection *conn= malloc(sizeof(struct Connection)); //Bellekte Connection değişkeni oluşturulur
        if(conn==NULL){//Eğer değişken oluşturulamamışsa hata verir
                die("Memory Error");
        }
        conn->db=malloc(sizeof(struct Database)); // Bellekte Database değişkeni oluşturulur
        if(conn->db==NULL){ // Eğer değişken oluşturulamamışsa hata verir
                die("Memory Error");
        }
        if(action=='c'){
                conn->file=fopen(filename,"w");
        }else{
                conn->file=fopen(filename,"r+");
                if (conn->file){
                        Database_load(conn);//Varolan dosya okunmak üzere gönderilir
                }
        }
        if (!conn->file){
                die("Failed to open file");
        }
        return conn; //Oluşturulan dosya döndürülür
}

void Database_create(struct Connection *conn){
        int i=0;
        for (i=0;i<MAX_ROWS;i++){
                struct Address adr={.id=i,.set=0}; //Database kayıtları oluşturulur ancak boştur
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
        printf("ID = %d Name = %s Email = %s\n",addr->id,addr->name,addr->email);//Kaydı ekrana yazar 
}

void Database_get(struct Connection *conn,int id){
        struct Address *addr = &conn->db->rows[id];//ID si belirtilen kaydı değişkene atar
        if (addr->set){
                Address_print(addr);
        }
}

void Database_set(struct Connection *conn, int id,char *name,char *email){
        struct Address *addr=&conn->db->rows[id];
        if (addr->set){//Eğer set değeri 1 ise zaten kaydın olduğuna dair hata mesajı verir
                die("Already set,delete it first");
        }
        addr->set=1;//Kaydın oluştuğunu belirtmek için set değeri 1 yapılır
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
        struct Address addr ={.id=id,.set=0}; //Kaydı silmek için set değeri 0 yapılır
         conn->db->rows[id]=addr;
}

void Database_list(struct Connection *conn){
        int i=0;
        struct Database *db = conn->db;
        for (i=0;i<MAX_ROWS;i++){
                struct Address *cur = &db->rows[i];
                if (cur->set){//set değeri 0 dan farklıysa ekrana yazdırma yapılır
                        Address_print(cur);
                }
        }
}

int main(int argc,char *argv[]){
        if (argc<3){//Komut satırı arguman sayısı kontrol edilir
                die("Usage main.c <dbfile> <action> [action parameters]");
        }
        char *filename=argv[1]; // Komut satırı argumanından dosya adı okunur
        char action = argv[2][0]; // Komut satırı argumanından aksiyon okunur
        struct Connection *conn = Database_open(filename,action); //Dosya açılır-oluşturulur
        int id=0;
        if (argc>3){
                id=atoi(argv[3]);//arguman sayısı 3 ten büyükse id değişkeni atanır
        }
        if (id >MAX_ROWS){
                die("There is not that many records");//Belirtilen sayıdan büyük ID'ye ulaşmak istenirse hata verir
        }
        switch(action){
                case 'c': //Database Dosyası oluşturur
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
        return 0;
}