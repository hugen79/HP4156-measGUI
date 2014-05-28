/*** write GPIB order ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpib/ib.h"
#include "../inc/stringutils.h"
#include "../inc/linkedlist.h"
#include "../inc/gpib_io.h"
#include "../inc/IVsweep.h"

/*** HP4156 SPECIFIC FUNCTIONS ***/

//SMU CONTROL
int setSMU(int ud, const char *vars[])
{

  //VNAME
  char cmd1[32] = ":PAGE:CHAN:";
  strcat(cmd1,vars[0]);
  strcat(cmd1,":VNAME ");
  _write(ud,strcat(cmd1,vars[1]));

  if (strcmp(vars[0], "GNDU")){
      //INAME
      char cmd2[32] = ":PAGE:CHAN:";
      strcat(cmd2,vars[0]);
      strcat(cmd2,":INAME ");
      _write(ud,strcat(cmd2,vars[2]));

      //MODE
      char cmd3[32] = ":PAGE:CHAN:";
      strcat(cmd3,vars[0]);
      strcat(cmd3,":MODE ");
      _write(ud,strcat(cmd3,vars[3]));
    
      //FUNC
      char cmd4[32] = ":PAGE:CHAN:";
      strcat(cmd4,vars[0]);
      strcat(cmd4,":FUNC ");
      _write(ud,strcat(cmd4,vars[4]));

  }
   
  if (vars[5] && vars[6]){
    char cmd5[32] = "PAGE:MEAS:CONS:";
    strcat(cmd5,vars[0]);
    strcat(cmd5," ");
    _write(ud,strcat(cmd5, vars[5]));
   
    char cmd6[32] = "PAGE:MEAS:CONS:";
    strcat(cmd6,vars[0]);
    strcat(cmd6,":COMP");
    strcat(cmd6," ");
    _write(ud,strcat(cmd6, vars[6]));
  }
  return 0;
}

//SMU DISABLE
int disableSMU(int ud, const char *var)
{
  char cmd[32] = ":PAGE:CHAN:";
  strcat(cmd,var);
  _write(ud,strcat(cmd,":DIS"));
  return 0;
}

//SET VAR1 or VAR2
int setVAR(int ud, const char *vars[])
{

  char cmd[32] = ":PAGE:MEAS:";
  strcat(cmd, vars[0]);
  strcat(cmd, ":");
 
  // Linear or Log. Do not need to issue this 
  // command for var2
  if (strcmp(vars[0],"VAR2")){
    char cmd1[32]; 
    strcpy(cmd1,cmd); 
    strcat(cmd1,"SPAC ");
    _write(ud, strcat(cmd1, vars[1]));
  }

  // Start
  char cmd2[32];
  strcpy(cmd2,cmd);
  strcat(cmd2,"STAR ");
  _write(ud, strcat(cmd2, vars[2]));

  // Stop or nPoints
  char cmd3[32];
  strcpy(cmd3,cmd);
  if (strcmp(vars[0],"VAR1")==0){
    strcat(cmd3,"STOP ");
    _write(ud, strcat(cmd3, vars[3]));
  }
  else{
    printf("%s", vars[0]);
    strcat(cmd3,"POIN ");
    _write(ud, strcat(cmd3, vars[3]));

  }


  if (strncmp(vars[1],"LIN",3) == 0)
  {
    // Step
    char cmd4[32];
    strcpy(cmd4,cmd);
    strcat(cmd4,"STEP ");
    _write(ud, strcat(cmd4, vars[4]));
  }
    
  // Compliance
  char cmd5[32];
  strcpy(cmd5,cmd);
  strcat(cmd5,"COMP ");
  _write(ud, strcat(cmd5, vars[5]));
  return 0;
}

int addToList(int ud, char* variable){ 
  char cmd[32] = ":PAGE:DISP:LIST ";
  _write(ud, cmd);
  _write(ud, strcat(cmd, variable));

  char cmd2[32] = ":PAGE:DISP:MODE LIST";
  _write(ud,cmd2); 
}

int remFromList(int ud, char* variable){
  char cmd[32] = ":PAGE:DISP:LIST";
  _write(ud, cmd);
  
  char cmd1[32];
  strcpy(cmd1,cmd); 
  strcat(cmd1, ":DEL ");
  _write(ud, strcat(cmd1, variable));
  
  char cmd2[32] = ":PAGE:DISP:MODE LIST";
  _write(ud,cmd2); 
}

int remFromListAll(int ud){
  char cmd[32] = ":PAGE:DISP:LIST:DEL:ALL";
  _write(ud,cmd); 

}

int measure(int ud){
  // Perform Measurement and autoscale
  _write(ud,":PAGE:DISP:MODE GRAP");
  _write(ud,":PAGE:SCON:SING");
  _write(ud,"*WAI");
  _write(ud,":PAGE:GLIS:SCAL:AUTO ONCE");
}

int writeToFile(char *filename, node_t *listVARS, char **DATA, int size){ 
  
  // Get a file pointer
  FILE *file; 
  file = fopen(filename,"w+"); 

  //  print_list(listVARS);

  /* // WRITE HEADER TO FILE */
  char *header;
  header = (char*)malloc(256);
  strcpy(header,""); 

 // while (listVARS->next != NULL){
  while (listVARS->next){
    strcat(header, strdup(listVARS->data));
    strcat(header, "\t\t");
    listVARS=listVARS->next;
  }
  strcat(header,"\n");
  fprintf(file, remove_char(header,'\''));
  free(header);

  int j;
  node_t *DATALIST[size];
  // need to initialize all the linked lists
  for (j = 0; j<size; j++)
    DATALIST[j]=initialize_list();
  j=0;

  // Populate the linked lists with our data
  for (j = 0; j< size; j++){
    char* point = strtok(DATA[j],",\n");
    add_to_list(DATALIST[j], point);

    while (point != NULL){
      add_to_list(DATALIST[j], point);
      point = strtok(NULL, ",\n");
    }
  }

  // Calculate the total number of data points
  int k;
  int npoints = count_list(DATALIST[0]);

  
  // Actually write the data
  j = 0;
  k = 0;
  for (j = 0; j < npoints; j++){

    char* line;
    line = (char*)malloc(256);
    strcpy(line,"");
    // Build the data line by line
    for (k = 0; k< size; k++){
      strcat(line, DATALIST[k]->data);
      strcat(line, "\t");
      DATALIST[k] = DATALIST[k]->next;
    }
    // Add a newline to each row
    strcat(line, "\n");
    fprintf(file, line);
    free(line);
  }

  // destroy the lists
  j = 0;
  for (j = 0; j<size; j++)
    destroy_list(DATALIST[j]);

  fclose(file);
  return 0;
}

int savedata(int ud, char* filename, node_t *listVARS, int buffersize){

  

  int i = 0;
  // DATA[i] needs to be read/write so it must 
  // be an array. If data is char** then it is 
  // read only and we cannot use strtok later!!
  char *DATA[count_list(listVARS)];   
  
  while (listVARS->next != NULL){
    char* cmd;
    cmd = (char*)malloc(16);
    strcpy(cmd,":DATA? ");
    DATA[i]=_read(ud, strcat(cmd, strdup(listVARS->data)), buffersize);
    
    // If there is no data ... then just quit. 
    // Otherwise write will issue SEGFAULT
    if (!(strcmp(DATA[i], "")))
      return 0;

    listVARS = listVARS->next;
    free(cmd);
    i++;
  }

  // Rewind the linked list. Otherwise we will get a
  // a segfault.
  listVARS = listVARS->head;
  writeToFile(filename,listVARS , DATA, i);
}
