#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif
#include <fuse.h> 		
#include <stdio.h>				
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>	// ENOMEM
#include <unistd.h> 
#include <pwd.h>	// Due to getUsername()
#include <stdarg.h>
#include <time.h>

#define CMD_TAM_MAX   (1024)

char *_fileName = "File";
char *fileName = "/File";
char *pathOfBashrc;

char *getUsername() {
	register struct passwd *pw;
  	register uid_t uid;
  	uid = geteuid();
  	pw = getpwuid(uid);
	return pw->pw_name;
}

char* getTheFileText() {

			FILE *stream;
			char *line = NULL;	// The Data Of That line, null terminated
			size_t len = 0;	
			ssize_t nread; 		// The Length of the line 
			int x = 0;
			stream = fopen(pathOfBashrc, "r");
			if (stream == NULL) 
				return NULL;

			while ((nread = getline(&line, &len, stream)) != -1) {
				//fwrite(line, nread, 1, stdout);
				if (strncmp("cowsay", line, strlen("cowsay")) == 0) {
					x = 1;
					break;
				}
			}
			fclose(stream);
			
			if (x == 0) {
				return NULL;
			} else {
				int repace = strlen("cowsay -e \"o0\" ");
				char *x = malloc(nread - repace + 1);
				memcpy(x, line + repace, nread - repace);
				memset(x + nread - repace, 0, 1);
				free(line);
				return x;
			}
			return line;
}
			

int getLengthOfTheFileText() { 
	char *j = getTheFileText();
	if (j == NULL) 
		return 0;
	int c = strlen(j);
	free(j);
	return c;
}

void setTheFileText(const char *x, size_t size) {	
	/*
	// Prepare Memory Stream ...
	*/
	FILE *stream;
	char *buf /* Buffer Data */ ; size_t lenOfChar; /* Len Of Data */
	stream = open_memstream(&buf, &lenOfChar);

	/*
	// Read .bashrc File And Process Out The Content ...
	*/
	FILE *file;
	char *line = NULL;	
	size_t len = 0;
	ssize_t nread; 	
	file = fopen(pathOfBashrc, "r");
	if (file == NULL) 
		 return;

	size_t y = strlen("cowsay");
	while (nread = getline(&line, &len, file) != -1) 
		if (strncmp("cowsay", line, y) != 0) 
			fprintf(stream, "%s", line);


	/*
	// Close The Data, Prepae The Files ...
	*/
	fclose(file);
	free(line);
	if (size >= 3)
		fprintf(stream, "cowsay -e \"o0\" %s", x);
	fclose(stream);


	/*
	// Write The New Text 
	*/
	file = fopen(pathOfBashrc, "w");
	if (file == NULL) 
		return;
	fprintf(file, "%s", buf);
	fclose(file);
	free(buf);
}

// will be executed when the system asks for attributes of a file that was stored in the mount point
int fun_getattr( const char *path, struct stat *stateBuff ) {
		
	stateBuff->st_uid = getuid();
	stateBuff->st_gid = getgid();
	stateBuff->st_atime = stateBuff->st_mtime = stateBuff->st_ctime = time(NULL);

	if ( strcmp( path, "/" ) == 0 ) { // Info For The mount point
		stateBuff->st_mode = S_IFDIR | 0755;
		stateBuff->st_nlink = 2;
	}
	else if (strcmp( path, fileName ) == 0){ // Info For The req file
		stateBuff->st_mode = S_IFREG | 0644;
		stateBuff->st_nlink = 1;
		stateBuff->st_size = getLengthOfTheFileText();
	} else 
		return -ENOENT;

	return 0;
}



//  will be executed when the system asks for a list of files that were stored in the mount point
int fun_readdir( const char *path, void *buff, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ) {
	filler( buff, ".", NULL, 0 ); 	// Current Directory
	filler( buff, "..", NULL, 0 ); 	// Parent Directory
	
	// Get the files/directories which are exist in the mount point
	if ( strcmp( path, "/" ) == 0 )
		filler( buff, _fileName , NULL, 0 );
	else 
		return -ENOENT;

	return 0;
}

int fun_release(const char *path, struct fuse_file_info *fi) {
	printf("Release Function\n");
	return 0;
}

int fun_flush(const char *path, struct fuse_file_info *fi) { 
	printf("Flush Function\n");
	return 0;
}

int fun_open(const char *path, struct fuse_file_info *fi) {

	if (strcmp(path, fileName) == 0) 
	{
	printf("Open Done successfully\n");
	}
	
	else
		return -ENOENT;

	return 0;
}

int fun_truncate(const char *path, off_t newsize)
{
	return 0;
}

int fun_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
	return 0;
}

static struct fuse_operations operations = {
    .getattr	= fun_getattr,
    .readdir	= fun_readdir,
	.open		= fun_open,
	.release	= fun_release,  
	.flush		= fun_flush,  
	.truncate 	= fun_truncate,
	.ftruncate 	= fun_ftruncate,
};

int systemf( const char * fmt, ... )
{
    va_list args;
    char cmd[ CMD_TAM_MAX + 1 ];

    va_start( args, fmt );
    vsnprintf( cmd, CMD_TAM_MAX + 1, fmt, args );
    va_end(args);

    return system(cmd);
}

// gcc Trabalho3.c -o myFUSE `pkg-config fuse --cflags --libs`
int main( int argc, char *argv[] ) {

	int opcao;
	char mail[100];
	char mailPass[100];	
	int NewPassword;
	int PasswordUtilizador;
	char PasswordUtilizadorString[100];
	time_t start, stop; 

	do
	{
		printf("Possui password(1)/Não possui password(2)/Sair(3): ");
		scanf("%d", &opcao);

	}while(opcao != 1 && opcao != 2 && opcao != 3);
	
	if (opcao == 1)
	{
		int Validacao;	//0-nao	1-sim
		printf("Introduza mail e pass (Mail/Pass): ");
		scanf("%s", &mailPass);

		FILE *f;
      	int  i;
      	int ultimo;
      	char nomes[100][100];
      
     	f = fopen("registo.txt", "r");
      	if( f == NULL )
        	printf("Erro");
    
      	i = 0;
      	while( fgets(nomes[i], 100, f) != NULL && (i<100) ) 
        {
          ultimo = strlen( nomes[i] );
          nomes[i][ultimo-1] = '\0';       /* para mandar fora o '\n' */
          i++;
        }
    
      	fclose( f );

      	
      	for (int i = 0; i < 100; ++i)
      	{
      		if (mailPass == nomes[i])
      		{
      			Validacao = 1;
      			break;
      		}

      		else
      		{
      			Validacao = 0;
      		}
      	}

      	//printf("%s \n", nomes[1]);

      	if(Validacao == 0)
      	{
      		printf("Erro, não existe correspondência");
      	}

      	else
      	{
      		return fuse_main( argc, argv, &operations, NULL );

      	}	
		
	}

	else if (opcao == 2)
	{	
		NewPassword = rand() % 9000000000;	

		printf("Introduza o seu email: ");
		scanf("%s", &mail);

	    systemf("echo '%d' | mail -s 'Password De Acesso' %s", NewPassword, mail);

		// Get .bashrc File Path
		size_t s = strlen("/home/") + strlen(getUsername()) + strlen("/.bashrc") + 1;
		pathOfBashrc = malloc(s);
		memset(pathOfBashrc, 0, s);

		sprintf(pathOfBashrc + strlen(pathOfBashrc), "/home/");
		sprintf(pathOfBashrc + strlen(pathOfBashrc), "%s", getUsername());
		sprintf(pathOfBashrc + strlen(pathOfBashrc), "/.bashrc");

		start = time(NULL);

		printf("Introduza a pass enviada para o email: ");
		scanf("%d", &PasswordUtilizador);

		stop = time(NULL);  

		printf("O tempo que levou a escrever a password foi de: %ld\n", stop - start);

		if (NewPassword==PasswordUtilizador && stop - start < 30)
		{
			//ESCREVER NO FICHEIRO

			// creating file pointer to work with files
    		FILE *fptr;

    		// opening file in writing mode
    		fptr = fopen("registo.txt", "a");

    		// exiting program 
    		if (fptr == NULL) {
        		printf("Error!");
        		exit(1);
    		}
			
			sprintf(PasswordUtilizadorString, "%i", PasswordUtilizador);
    		strcpy(mailPass, mail);
    		strcat(mail, "/");
    		strcat(mail, PasswordUtilizadorString);
			strcat(mail, "\n");
    		
    		fprintf(fptr, "%s", mail);
    		fclose(fptr);
			
			//FIM ESCREVER NO FICHEIRO

			return fuse_main( argc, argv, &operations, NULL );
		}

		else if(stop - start > 30)
		{
			printf("ERRO - PASSOU OS 30 SEGUNDOS");	
		}

		else if(stop - start > 30 && NewPassword != PasswordUtilizador)
		{
			printf("ERRO - PASSOU OS 30 SEGUNDOS E ERROU A PASSWORD");	
		}

		else
		{
			printf("ERRO - ERROU A PASSWORD");
		}
	}

	else
	{
		printf("Obrigado, volte sempre");
	}
}