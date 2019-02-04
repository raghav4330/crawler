#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<time.h>

#define keyword "chitkara"
//#define adef "ahref=\""
#define HASH_SIZE 100
#define MAX_DEPTH 3
#define MAX_URL_LENGTH 500

#define SEED_URL "https://www.chitkara.edu.in"

#define MAX_URL_PER_PAGE 100

#define INTERVAL_TIME 10      //time in ms

#define CHECK_MALLOC(s) if ((s) == NULL) { \
printf("memory allocation failed in %s: line%d", _FILE, __LINE_); \
perror(":"); \
exit(0); \
}

#define SET_MEM(start,size) memset(start,0,size)

//  https://www.chitkara.edu.in C:\Users\Raghav\Desktop\crfolder\new 2

struct URL
{
	char *url;
	int depth;
	int url_key;
};
struct node
{
	struct URL u;
	int isvisited;
	struct node *next;
	struct node *tail;
};

struct hash
{
	struct node *hhead;
	struct node *htail;
	int count;
};

void set_initials(int argc,char **argv,char *url,char *dir,int *depth);
int testDir(char *dir);
int valid_depth(int depth);
int valid_url(char *url);
int right_link(char *s,char *url,char **links,int *length);
struct node *create(char *s,int d,int key);
int find_key(char *str);

void delay(int ms)
{
	clock_t stime=clock();
	while(clock()<stime+ms)
	{if(kbhit())
	break;
	}

}


int testDir(char *dir)
{
  struct stat statbuf;
  if ( stat(dir, &statbuf) == -1 ) 
  {
    fprintf(stderr, "-----------------\n");
    fprintf(stderr, "Invalid directory\n");
    fprintf(stderr, "-----------------\n");
    return 0;
  }

  //Both check if there's a directory and if it's writable
  if ( !S_ISDIR(statbuf.st_mode) ) 
  {
    fprintf(stderr, "-----------------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
    fprintf(stderr, "-----------------------------------------------------\n");
    return 0;
  }

  if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR ) 
  {
    fprintf(stderr, "------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. It isn't writable\n");
    fprintf(stderr, "------------------------------------------\n");
    return 0;
  }
  
  return 1;
}



int valid_depth(int depth)
{
	///printf("\nChecking valid depth...\n");
    if(depth<=0)
    {   printf("Enter non zero positive depth\n");
        return 0;
    }
    if(depth>MAX_DEPTH)
    {   printf("Enter depth should not be greater than %d\n",MAX_DEPTH);
        return 0;
    }
    return 1;
}

int valid_url(char *url)
{
	int u1,u2;
	u1=strlen(SEED_URL);
	u2=strlen(url);
	
	if(u2>MAX_URL_LENGTH)
	{
		printf("url lemgth should be maximum upto %d",MAX_URL_LENGTH);
		return 0;
	}
	
	if(u1>u2)
    {
        printf("Smaller url! URL should start with %s\n",SEED_URL);
        return 0;
    }
    
    int i=0;
    while(i<u1)
    {
        if(SEED_URL[i]!=url[i])
        {
        	printf("incorrect url! url must start with seed url:%s",SEED_URL);
        	return 0;
		}
      i++;
    }
    
    //const int u3=u2+100;
    char s[MAX_URL_LENGTH]="wget --no-check-certificate --spider ";
    strcat(s,url);
    
    
     if(!system(s))
     {
     	printf("Valid URL on Internet");
     	return 1;
	 }
       else
       {
     	printf("Invalid URL on Internet");
     	return 0;
	 }
    
}



void set_initials(int argc,char **argv,char *url,char *dir,int *depth)
{
	strcpy(url,argv[1]);
    strcpy(dir,argv[2]);
    sscanf(argv[3],"%d",depth);
}


int check_parameters(int argc,char **argv,char *url,char *dir,int *depth,struct node **head,struct node **tail,struct hash *h)
{
	if(argc!=4)
	{
		printf("incorrect number of parameters passed in program");
		return 0;
	}
	set_initials(argc,argv,url,dir,depth);
	
	 if( testDir(dir) && valid_depth(*depth) && valid_url(url))
    {   
      printf("\nCrawler have been started");
      int key=find_key(url);
      struct node *temp=create(url,1,key);
      *head=temp;
      *tail=temp;
      h[key].hhead=temp;
      h[key].htail=temp;
      h[key].count++;
	  return 1;
	}
	else
	  return 0;
}
	
void getpage(char *url,char *dir)
{

char urlbuffer[MAX_URL_LENGTH+300]={0}; 
strcat(urlbuffer, "wget --no-check-certificate -O ");

strcat(urlbuffer,dir);
strcat(urlbuffer,"/temp.txt ");

strcat(urlbuffer, url);


system(urlbuffer);
}	 





char * create_file(char *dir,int num,char *url,int depth,int key)
{
	char tempath[180]="",perm_filepath[180]="",itostring[MAX_URL_PER_PAGE]="";
	char ss[]="/temp.txt";
	sprintf(itostring,"%d",num);
	strcpy(tempath,dir);
	strcat(tempath,ss);
	
	strcpy(perm_filepath,dir);
	strcat(perm_filepath,"/FILE");
	strcat(perm_filepath,itostring);
	strcat(perm_filepath,".txt");
	
	struct stat st; //variable which will count length of file.
stat(tempath,&st); // temp.txt is the file where wget fetch the html 
int file_size=st.st_size+1+50;  //+50 is for adding extra details in top of file as given below // +1 for '\0'

char *str=(char *)malloc(sizeof(char)*file_size);
int i=0;
char c;

    FILE *fp=fopen(tempath,"r");
    FILE *fp2=fopen(perm_filepath,"w");
    fprintf(fp2,"url = %s  |  depth = %d  |  key = %d\n",url,depth,key);
    char ch=fgetc(fp);
    while(ch!=EOF)
    {
    	str[i++]=ch;
    	fputc(ch,fp2);
    	ch=fgetc(fp);
	}
	fclose(fp);
	fclose(fp2);
	
    str[i]='\0';
    //puts(str);
    return str;
}	




 int right_link(char *s,char *url,char **links,int *length)
 {
		//printf("kk");
	//puts(s);
    int flag=0,i=0;
	if(s[0]=='\0' || s[0]=='#')
	return 0;
	//if(!strstr(s,keyword))
	//return 0;
	
	i=0;
	//printf("#");
	
	
	while(SEED_URL[i]!='\0')
	{
		if(SEED_URL[i]!=s[i])
         return 0;
	  i++;
	}
	
	if(strcmp(SEED_URL,s)==0)
	return 0;
	
	if(strcmp(url,s)==0)
	return 0;
	
	for(i=0;i<*length;i++)
	{
		if(strcmp(s,links[i])==0)
		return 0;
	}
	


	
	
	//puts(s);
	return 1;  
}




void find_links(char *str,char **links,int *length,char *url)
{
	int i=0,flag=0,j=0,m=0;
	char anchor[MAX_URL_LENGTH]="";
	char adef1[]="<a"; ///adef[6]='\"';
	char adef2[]="href=\"";
	//int flag=0;
//	puts(adef);
;
	char *anchor_link,temp;
	
	while(str[i]!='\0')
	{
	if(str[i]==' ')
	{}
	else
	 {
	 	temp=tolower(str[i]);
		 //printf("%c",str[i]); 
		if(flag==0 && temp==adef1[j])
         {
         	j++;
         	//i++;
         	if(adef1[j]=='\0')
         	flag=1;
         }
         else
         j=0;
         
         if(flag==1 && temp=='>')
         {
         	flag=0;
         	j=0;
		 }
		 
         if(flag==1 && adef2[m]==temp)
        {
		  m++;
         	if(adef2[m]=='\0')
         	{
         		int k=0;
         		i++;
         		while(str[i]==' ')
         		i++;
         		while(str[i]!='\"' && str[i]!=' ')
         		{
         			anchor[k++]=str[i];
         			i++;
				}
				anchor[k]='\0';
				m=0;
				j=0;
				flag=0;
				
				 //puts(anchor);
				if(right_link(anchor,url,links,length))
				{
					//puts("hello");
					k=k+1;
					anchor_link=(char *)malloc(sizeof(char)*k);
					k=0;
					//puts("hello");
					strcpy(anchor_link,anchor);
					links[(*length)++]=anchor_link;
					//puts(links[0]);
					if(*length==MAX_URL_PER_PAGE)
					return;
				}
				
			}
		 }
		 else
		 m=0;
	 }
	 i++;
	}
}

int link_not_exist(char *s,struct node *head)
{
	struct node *t=head;
	while(t!=NULL)
	{
		if(strcmp(t->u.url,s)==0)
		return 0;
		
		t=t->next;
	}
	return 1;
}


int find_key(char *str)
{
	int i=0;
	int sum=0;
	while(str[i]!='\0')
	{
		sum=sum+str[i];
		i++;
	}
	return (sum%100);
}



struct node *add_inlist(struct node *head,struct node **tail,char **links,int length,int d,struct hash *h)
{
	struct node *ftail=*tail;   ///tail in function
	struct node *t=head,*temp;
	int i=0;
	int key;
	if(head==NULL)
	{ 
	key=find_key(links[i]);
    temp=create(links[i],d,key);
	h[key].hhead=temp;
	h[key].htail=temp;
	h[key].count++;
	head=temp;
	ftail=temp;
	i++;
	}
	while(i<length)
   {
   	
	int key=find_key(links[i]);
	if(h[key].hhead==NULL)     
   {
	temp=create(links[i],d,key);
	h[key].hhead=temp;
	h[key].htail=temp;
	h[key].count++;

	ftail->next=temp;      ///if head at that key is null then its obvious temp is added in last pf linked list
	ftail=temp;
	    
   }
   else
   {
   	struct node *tt=h[key].hhead;
   	int c=h[key].count;
   	int flag=0;
   	while(c>0)
   	{
   		if(strcmp(tt->u.url,links[i])==0)
   		{
   			flag=1;
   			break;
	    }
	    tt=tt->next;
	    c--;
	}
	if(flag==0)
	{
		tt=h[key].htail;
		
		temp=create(links[i],d,key);
		temp->next=tt->next;
		tt->next=temp;
		
		if(tt==ftail)  // or temp ->next==NULL
		 ftail=temp;
		
		h[key].htail=temp;
		h[key].count++;
		
	}
   }

	i++;
  }

   *tail=ftail;
   return head;
	
}

struct node *create(char *s,int d,int key)
{
	int n=strlen(s);
	n=n+1;
	struct node *temp=(struct  node *)malloc(sizeof(struct node));
	temp->u.url=(char *)malloc(sizeof(char)*n);
	strcpy(temp->u.url,s);
	temp->u.depth=d;
	temp->u.url_key=key;
	temp->isvisited=0;
	temp->next=NULL;
	return temp;
}


void print_node(struct node *head)
{
	printf("\n\n\n");
	struct node *t=head;
	int i=1;
	while(t!=NULL)
	{
		printf("%d %s %d %d %d\n",i++,t->u.url,t->u.depth,t->isvisited,t->u.url_key);
		t=t->next;
	}
}


void freed(char **links,int *length)
{
	int i=0;
	for(i=0;i<*length;i++)
	free(links[i]);
	*length=0;
	//free(links);
}
void savecrawling(struct node *head,char *url,char *dir,int depth)
{
	FILE *fp;
	fp=fopen("savedcrawling.txt","w");
	struct node *t=head;
	fprintf(fp,"%s %s %d\n",url,dir,depth);/// url here is seed url and required depth for program
	while(t!=NULL)
	{
		fprintf(fp,"%s %d %d %d\n",t->u.url,t->u.depth,t->u.url_key,t->isvisited);
		t=t->next;
	}
	fprintf(fp,'\0');
	fclose(fp);
	printf("crawling saved");
	getch();
	
}

struct node *loadcrawler(struct node *head,struct node **tail,struct hash *h,char *seedurl,char *directory,int *depth)
{
	printf("hello");
	FILE *fp=fopen("savedcrawling.txt","r");
	struct node *ftail=*tail;   ///tail in function
	struct node *t=head,*temp;
	int key,d,v;
	char s[500];
	
	char url[500];
	char dir[300];
	int flag=0;
	
	while(!feof(fp))
	{
		printf("hello");
		
		if(flag==0)
		{
		fscanf(fp,"%s",url);
		fscanf(fp,"%s",dir);
		fscanf(fp,"%d",&d);
		
		strcpy(seedurl,url);
		strcpy(directory,dir);
		*depth=d;
		flag=1;
		}
		
		fscanf(fp,"%s",s);
		fscanf(fp,"%d",&d);
		fscanf(fp,"%d",&key);
		fscanf(fp,"%d",&v);
		printf("%s %d %d %d\n",s,d,key,v);
			
	if(head==NULL)
	{ 
    temp=create(s,d,key);
    temp->isvisited=v;
	h[key].hhead=temp;
	h[key].htail=temp;
	h[key].count++;
	head=temp;
	ftail=temp;
	}
	else
   	{
   		
	if(h[key].hhead==NULL)     
   {
	temp=create(s,d,key);
	temp->isvisited=v;
	h[key].hhead=temp;
	h[key].htail=temp;
	h[key].count++;

	ftail->next=temp;      ///if head at that key is null then its obvious temp is added in last pf linked list
	ftail=temp;
	    
   }
   else
   {
   	struct node *tt=h[key].hhead;

		tt=h[key].htail;
		
		temp=create(s,d,key);
		temp->isvisited=v;
		temp->next=tt->next;
		tt->next=temp;
		
		if(tt==ftail)  // or temp ->next==NULL
		 ftail=temp;
		
		h[key].htail=temp;
		h[key].count++;
		
    }

   }	
  }

   *tail=ftail;
   return head;	
}


void pause_menu(struct node *head,char *url,char *dir,int d)
{
	char c='s';
	do
	{
	printf("\npress s. to save\n press any other button to resume\n");
	c=getch();
	if(c=='s')
	  {
	  	printf("hello");
	  	savecrawling(head,url,dir,d);
	  }
    }
    while(c=='s');
}


int start_menu()
{
	
    int n,i=1;
	FILE *fp=fopen("savedcrawling.txt","r");
   do
    {
        system("cls");
    printf("%d.New Game\n",i++);
    if(fp!=NULL)
    printf("%d.Load crawler\n",i++);
    printf("%d.Exit\n",i++);

 fclose(fp);
    scanf("%d",&n);
    if(n==i-1)
    exit(1);
    else if(n==1 || (n==2 && fp!=NULL))
        return n;
    else
      printf("\nselect right option:\n");
      i=1;
    getch();
    }
	while(1);
    
}


int main(int argc,char *argv[])
{
	char url[MAX_URL_LENGTH];
	char directory[150];
	int depth;
	char *str;
	int length=0,i=0;
	struct node *head=NULL,*tail=NULL,*t;
	int seed_depth=1,prev_depth=0;
	int fileno=1;
	struct hash h[HASH_SIZE];
	for(i=0;i<HASH_SIZE;i++)
	{
		h[i].hhead=NULL;
		h[i].htail=NULL;
		h[i].count=0;
	}
	
	int nn=start_menu();
	
	if(nn==3)
	exit(1);
	else if(nn==1)
	{
	int c=check_parameters(argc,argv,url,directory,&depth,&head,&tail,h);
	//print_node(head);
	if(c==-1)
	  exit(1);
    }
    else if(nn==2)
    {
    	head=loadcrawler(head,&tail,h,url,directory,&depth);
    	//print_node(head);
    	//exit(1);
	}
    
    
	printf("\nwelcome\n");

	
t=head;
   while(t!=NULL)
  {
	                                                                     	//delay(1000);
		char c;
    if(kbhit())
        {
            c=getch();
            if(c==' ')
                pause_menu(head,url,directory,depth);

        }
        
	if(t->u.depth<=depth && t->isvisited==0)
    {
    	getpage(t->u.url,directory);
	    str=create_file(directory,fileno++,t->u.url,t->u.depth,t->u.url_key);


       length=0;
       char *links[MAX_URL_PER_PAGE];
	   find_links(str,links,&length,t->u.url);
      free(str);

//	printf("\n\n\n\n\n");
	
//	int i;
//	for (i=0;i<length;i++)
//	{
//		puts(links[i]);
//	}
//	printf("\n\n\n\n\n");

	

    int d=t->u.depth+1;
	head=add_inlist(head,&tail,links,length,d,h);

	//print_node(head);
	

	freed(links,&length);

	
	t->isvisited=1;
	
	t=head;

    }
     else
     t=t->next;
 }

	print_node(head);
	
	
}


