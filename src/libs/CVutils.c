/*** write GPIB order ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpib/ib.h"
#include "stringutils.h"
#include "linkedlist.h"
#include "gpib_io.h"
#include "CVutils.h"


/*** HP4285A SPECIFIC FUNCTIONS ***/
int setDefaults(int ud){
  _write(ud, "BIAS:STATE OFF");
  _write(ud, "OUTP:BMON:VOLT ON");
  _write(ud, "OUTP:BMON:CURR ON");
  _write(ud, "APER MED");
  _write(ud, "FREQ 1MHZ");
  _write(ud, "VOLT 1V");
}

///////////////////////////////////
//      OSCILLATOR CONTROL       //
///////////////////////////////////
int setFrequency(int ud, const char *var){
  char cmd1[32]; 
  strcpy(cmd1, "FREQ "); 
  strcat(cmd1, var);
  strcat(cmd1, "MHZ");
  _write(ud, cmd1);   
  return 0;
}
int setAmplitude(int ud, const char *var){
  char cmd1[32]; 
  strcpy(cmd1, "VOLT "); 
  strcat(cmd1, var); 
  strcat(cmd1, "V"); 
  _write(ud, cmd1);   
  return 0;
}

//////////////////////////////
//   MEASUREMENT CONTOL     //
//////////////////////////////
int setMode(int ud, const char *var){
  char cmd1[32];
  strcpy(cmd1, "FUNC:IMP:TYPE "); 
  strcat(cmd1, var); 
  _write(ud, cmd1);   
  return 0;
}
int calOpen(int ud){
  _write(ud, "CORR:OPEN");
  return 0;
}
int calShort(int ud){
  _write(ud, "CORR:SHOR");
  return 0;
}

/////////////////////////////
//      BIAS CONTROL       //
/////////////////////////////
int setBias(int ud, const char *var){
  char cmd1[32]; 
  strcpy(cmd1, "BIAS:VOLT:LEVEL ");
  strcat(cmd1, var); 
  _write(ud, cmd1);   
  return 0;
}

int setBiasON(int ud){
  _write(ud, "BIAS:STATE ON");
  return 0;
}
int setBiasOFF(int ud){
  _write(ud, "BIAS:STATE OFF");
  return 0;
}


///////////////////////////////
// MEASUREMENT FUNCTIONALITY //
///////////////////////////////
<<<<<<< HEAD
char** measure(int ud, double* swp, int len, char** freq, int lenf,  int buffersize){
 
=======
char** measure(int ud, double* swp, int len, int buffersize){
  
>>>>>>> 53b06224f894b3c0fe780e7e190cf754f9cb5fd3
  if (len !=0 ){
    char *swps  = malloc(32*sizeof(char));
    char *READ  = malloc(32*sizeof(char));

<<<<<<< HEAD
    int i,j;
    char** DATA = malloc(sizeof(char*));
=======
    int i;
    char** DATA = malloc(sizeof(char*)); 
>>>>>>> 53b06224f894b3c0fe780e7e190cf754f9cb5fd3
    DATA = (char**)malloc(len*sizeof(char*));
    for(i = 0; i<len; i++){
      DATA[i] = (char*)malloc(buffersize*sizeof(char));
    }

    for(i=0; i<len; i++){
      strcpy(READ, "\0");
      sprintf(swps, "%.3f", swp[i]);
      setBias(ud, swps);
<<<<<<< HEAD
      strcpy(DATA[i], swps);

      for(j = 0; j<lenf; j++){

	setFrequency(ud, freq[j]);
	_write(ud, "TRIG:SOUR BUS");
	_write(ud, "TRIG");
	READ = _read(ud, "FETC?", buffersize);


	strcat(DATA[i], "\t\t");
	strcat(DATA[i], strtok(READ, ","));
	strcat(DATA[i], "\t\t");
	strcat(DATA[i], strtok(NULL, ","));
      }
      strcat(DATA[i], "\n");
      printf("%s\n", DATA[i]);

=======
      _write(ud, "TRIG:SOUR BUS");
      _write(ud, "TRIG");
      READ = _read(ud, "FETC?", buffersize);

      strcpy(DATA[i], swps);
      strcat(DATA[i], "\t\t");
      strcat(DATA[i], strtok(READ, ","));
      strcat(DATA[i], "\t\t");
      strcat(DATA[i], strtok(NULL, ","));
      strcat(DATA[i], "\n");
>>>>>>> 53b06224f894b3c0fe780e7e190cf754f9cb5fd3
    }
    _write(ud, "TRIG:SOUR INT");
    _write(ud, "INIT:CONT ON");
    _write(ud, "INIT:IMM");
    setBias(ud, "0.00V");
    return DATA;
<<<<<<< HEAD
  }
}

int savedata(int ud, char* filename, char* hdata, char* oscdata, char** DATA, int len, int lenf){

  // Construct a header. Probaby not the most efficient method
  char* header = malloc(64*sizeof(char));
  strcpy(header, "Vb");
  strcat(header, "\t\t");

  int i,j,k;
  int split = (strlen(hdata)+1)/2;
  

  char* tmp = malloc(64*sizeof(char));
  tmp[0]='\0';
  for(k = 0; k<lenf; k++){

    j=0;
    for(i=0; i<strlen(hdata); i++){
      if (i != split){ 
	tmp[j]= hdata[i];j++;
      }
      else{
	tmp[j]= k+0x30; j++;
	tmp[j]='\t'; j++;
	tmp[j]='\t'; j++;
	tmp[j]='\t'; j++;
	tmp[j]= hdata[i];j++;
	tmp[j]= k+0x30; j++;
	tmp[j]='\t'; j++;
	tmp[j]='\t'; j++;
	tmp[j]='\t'; j++;
      }
    } 
    tmp[j] = '\0';
    strcat(header, tmp);
  }
  free(tmp);
=======

  }
  else{
    return NULL;
  }
}

int savedata(int ud, char* filename, char* hdata, char* oscdata, char** DATA, int len){

  // Construct a header. Probaby not the most efficient method
  char* header = malloc(16*sizeof(char));
  strcpy(header, "Vb");
  strcat(header, "\t\t");

  int i,j;
  int split = (strlen(hdata)+1)/2;
  char* tmp = malloc(16*sizeof(char));
  tmp[0]='\0';

  j=0;
  for(i=0; i<strlen(hdata); i++){
    if (i == split){ 
      tmp[j]='\t'; j++;
      tmp[j]='\t'; j++;
      tmp[j]='\t'; j++;
      tmp[j]= hdata[i]; j++;
    }
    else{
      tmp[j]= hdata[i]; j++;
    }
  } 
  tmp[j] = '\0';
  strcat(header, tmp);
>>>>>>> 53b06224f894b3c0fe780e7e190cf754f9cb5fd3
  strcat(header, "\n");

  FILE *file;  
  file = fopen(filename,"w+"); 
<<<<<<< HEAD
=======

>>>>>>> 53b06224f894b3c0fe780e7e190cf754f9cb5fd3
  fprintf(file,oscdata);
  fprintf(file,"*\n");
  fprintf(file,header); 

  // Dump data in 
  for(i = 0; i<len; i++){fprintf(file, DATA[i]);}
  fclose(file); 
}

<<<<<<< HEAD
=======






>>>>>>> 53b06224f894b3c0fe780e7e190cf754f9cb5fd3
/* int savedata(int ud, char* filename, char liststr[], int buffersize){ */

/*   // DO nothing if liststr is empty */
/*   if(liststr == NULL) */
/*     return 0; */
/*   if(!(strcmp(liststr,""))) */
/*     return 0; */
/*   if(!(strcmp(liststr,"[EMPTY LIST]")))  */
/*     return 0; */

/*   // First allocate some memory for the data  */
/*   // strings. These are LONG so they need to  */
/*   // be allocated on the HEAP to prevent a  */
/*   // STAAAACCCKKKK OVERFLOOWWWW */
/*   int j,k,count; */
/*   char **DATA; */
/*   DATA = (char**)malloc(8*sizeof(char*)); */
/*   for (j = 0; j<8; j++){ */
/*     DATA[j] = (char*)malloc(2*buffersize*sizeof(char)); */
/*   } */

/*   // Construct the header first from our copy  */
/*   // of liststr and write to file. Note that we  */
/*   // are simultaneously constructing the file  */
/*   // header to save a few lines of code.  */
/*   j = 0; */
/*   count = 0; */
/*   char* col = strtok(liststr," ");  */
/*   char header[32] = ""; */
/*   while (col != NULL){ */
/*     // construct the data header  */
/*     strcat(header,col); */
/*     strcat(header,"\t\t"); */
/*     // construct the data command and request the data. */
/*     // If there is no data then just return.   */
/*     char cmd[16] = ":DATA? "; */
/*     DATA[j]= _read(ud, strcat(cmd, col), buffersize); */
/*     if (!(strcmp(DATA[j],""))) */
/*       return -1; */
  
/*     // strtok again to get the next thing */
/*     col = strtok(NULL," "); */
/*     count++; */
/*     j++; */
/*   } */
/*   strcat(header, "\n"); */

/*   // Now we have the data AND the header constructed.  */
/*   // At this point we need to write out the file.  */
/*   // First we get a file pointer and print the header. */
/*   FILE *file;  */
/*   file = fopen(filename,"w+");   */
/*   fprintf(file,header); */

/*   // Now we will write out the actual file. For this we */
/*   // will use a rather advanced application of strtok.  */
/*   // this is advanagous because one only passes through  */
/*   // each data list ONCE ... rather than twice (i.e. the */
/*   // list method). First we need an array of test and  */
/*   // save pointers.  */
/*   char *testpointer[count];  */
/*   char *savepointer[count]; */

/*   // And an array for each line. Now for stktok, we need  */
/*   // to get the starting line.  */
/*   char *line; */
/*   strcpy(line, ""); */
/*   for ( j=0;j<count; j++){ */
/*     testpointer[j] = strtok_r( DATA[j],",\n", &savepointer[j]);                          */
/*     strcat(line, testpointer[j]); */
/*     strcat(line,"\t"); */
/*   } */
/*   strcat(line,"\n"); */
/*   fprintf(file, line); */
/*   // Once we have thw initial pointers ... we just loop through  */
/*   // all of the lines. */
/*   int bool = 1; */
/*   while (bool){ */
/*     strcpy(line,""); */
/*     for ( j=0;j<count; j++){ */
/*       testpointer[j] = strtok_r(NULL,",\n", &savepointer[j]);                          */
/*       if (testpointer[j] == NULL){ */
/* 	fclose(file); */
/* 	for (j =0; j<count; j++) */
/* 	  free(DATA[j]); */
/* 	free(DATA); */
/* 	return 0; */
/*       } */
/*       else{ */
/* 	strcat(line, testpointer[j]); */
/* 	strcat(line,"\t"); */
/*       } */
/*     } */
/*     strcat(line,"\n"); */
/*     fprintf(file, line); */
/*   } */
/* } */









