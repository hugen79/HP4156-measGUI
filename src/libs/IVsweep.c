/*** write GPIB order ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpib/ib.h"
#include "stringutils.h"
#include "linkedlist.h"
#include "gpib_io.h"
#include "IVsweep.h"

/*** HP4156 SPECIFIC FUNCTIONS ***/
///////////////////////////////////
//    MODE SELECT FUNCTIONS      //
///////////////////////////////////
int _setSweepMode(int ud)
{
  _write(ud,":PAGE:CHAN:MODE SWE");
}

int _setSamplingMode(int ud)
{
  _write(ud,":PAGE:CHAN:MODE SAMP");
}

///////////////////////////////////
//     SWEEP MODE FUNCTIONS      //
///////////////////////////////////
//SMU CONTROL
int setSweepSMU(int ud, const char *vars[])
{
  char cmd1[32];

  //VNAME
  strcpy(cmd1,":PAGE:CHAN:");
  strcat(cmd1,vars[0]);
  strcat(cmd1,":VNAME ");
  _write(ud,strcat(cmd1,vars[1]));

  //INAME
  if (strlen(vars[2]) < 8){ 
    char cmd2[32] = ":PAGE:CHAN:";
    strcat(cmd2,vars[0]);
    strcat(cmd2,":INAME ");
    _write(ud,strcat(cmd2,vars[2]));
  }

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
    
  if (vars[5]!= NULL && vars[6]!= NULL){
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

//SET VAR1 or VAR2
int setSweepVAR(int ud, const char *vars[])
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

///////////////////////////////////
//     SWEEP MODE FUNCTIONS      //
///////////////////////////////////
//SMU CONTROL
int setSamplingSMU(int ud, const char *vars[]){

  char cmd1[32];
    
  //VNAME
  strcpy(cmd1,":PAGE:CHAN:");
  strcat(cmd1,vars[0]);
  strcat(cmd1,":VNAME ");
  _write(ud,strcat(cmd1,vars[1]));

  //INAME
  if (strlen(vars[2]) < 8){ 
    char cmd2[32] = ":PAGE:CHAN:";
    strcat(cmd2,vars[0]);
    strcat(cmd2,":INAME ");
    _write(ud,strcat(cmd2,vars[2]));
  }

  //MODE
  char cmd3[32] = ":PAGE:CHAN:";
  strcat(cmd3,vars[0]);
  strcat(cmd3,":MODE ");
  _write(ud,strcat(cmd3,vars[3]));

  char cmd4[32] = ":PAGE:CHAN:";
  strcat(cmd4,vars[0]);
  strcat(cmd4,":FUNC CONS");
  _write(ud,cmd4);
  
  if ((vars[4] != NULL) && (vars[5]!=NULL)){
    char cmd5[32] = ":PAGE:MEAS:SAMP:CONS:";
    strcat(cmd5,vars[0]);
    strcat(cmd5," ");
    strcat(cmd5,vars[4]);
    _write(ud,cmd5);
    
    if (strlen(vars[5]) < 8){ 

      char cmd6[32] = ":PAGE:MEAS:SAMP:CONS:";
      strcat(cmd6,vars[0]);
      strcat(cmd6,":COMP ");
      strcat(cmd6,vars[5]);
      _write(ud,cmd6);
    }
  }
  _write(ud,"*CLS");
}

////////////////////////////////////
// COMMON FUNCTIONS TO BOTH MODES //
////////////////////////////////////
//SMU DISABLE
int disableSMU(int ud, const char *var)
{
  char cmd[32] = ":PAGE:CHAN:";
  strcat(cmd,var);
  _write(ud,strcat(cmd,":DIS"));
  return 0;
}
int disableALLSMU(int ud){
  _write(ud, ":PAGE:CHAN:ALL:DIS");
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

int measureGRAPH(int ud, node_t* listVARS){

  _write(ud,":PAGE:DISP:MODE GRAP");

  // Set the x and y axis to the first two things in 
  // listVARS
  if (count_list(listVARS) > 2){

    listVARS = listVARS->head;
    char* cmdx; 
    cmdx = (char*)malloc(32);
    strcpy(cmdx,":PAGE:DISP:GRAP:X:NAME ");
    strcat(cmdx, strdup(stringify(listVARS->data)));
    _write(ud, cmdx);
    char* cmdy;
    cmdy = (char*)malloc(32);
    listVARS = listVARS->next;
    strcpy(cmdy,":PAGE:DISP:GRAP:Y1:NAME ");
    strcat(cmdy, strdup(stringify(listVARS->data)));
    _write(ud, cmdy);
    listVARS = listVARS->head;
    
    free(cmdx);
    free(cmdy);
  }
  // Perform Measurement and autoscale
  _write(ud,":PAGE:SCON:SING");
  _write(ud,"*WAI");
  _write(ud,":PAGE:GLIS:SCAL:AUTO ONCE");
}

int measureLIST(int ud){
  _write(ud,":PAGE:DISP:MODE LIST");
  _write(ud,":PAGE:SCON:SING");
  _write(ud,"*WAI");
  _write(ud,":PAGE:GLIS:SCAL:AUTO ONCE");
}

int setSamplingVar(int ud, const char* data[]){

  char cmd1[32];
  strcpy(cmd1,":PAGE:MEAS:SAMP:IINT ");
  _write(ud, strcat(cmd1,data[0]));

  char cmd2[32];
  strcpy(cmd2,":PAGE:MEAS:SAMP:POIN ");
  _write(ud, strcat(cmd2,data[1]));
}

int setUserFunction(int ud, const char* data[]){
  char cmd[32] = ":PAGE:CHAN:UFUN:DEF ";
  strcat(cmd,data[0]);
  strcat(cmd,",");
  strcat(cmd,data[1]);
  strcat(cmd,",");
  strcat(cmd,data[2]);
  _write(ud, cmd);
}

int writeToFile(char *filename, node_t *listVARS, char **DATA, int buffersize){ 
  
  // Get a file pointer
  FILE *file; 
  file = fopen(filename,"w+"); 
 
  int size;
  size = count_list(listVARS);

  /* // WRITE HEADER TO FILE */
  char *header = malloc(sizeof(header));
  strcpy(header,""); 

 // while (listVARS->next != NULL){
  while (listVARS->next){
    strcat(header, strdup(listVARS->data));
    strcat(header, "\t\t");
    listVARS=listVARS->next;
  }
  strcat(header,"\n");
  fprintf(file, remove_char(header,'\''));

  node_t **DATALIST;
  DATALIST = (node_t**)malloc(8*sizeof(node_t*));

  int j;
  for (j = 0; j<8; j++){
    DATALIST[j] = (node_t*)malloc(buffersize*sizeof(DATALIST[j]));
    DATALIST[j] = initialize_list();
  }
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
  char line[256];
  int npoints = count_list(DATALIST[0]);
 
  // Actually write the data
  j = 0;
  k = 0;
  for (j = 0; j < npoints; j++){
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
  }

  // destroy the lists
  j = 0;
  for (j = 0; j<size; j++)
    destroy_list(DATALIST[j]);

  free(DATALIST);
  fclose(file); 
  return 0; 
}


int savedata(int ud, char* filename, node_t *listVARS, int buffersize){

 
  // DATA[i] needs to be read/write so it must 
  // be an array. If data is char** then it is 
  // read only and we cannot use strtok later!!

  int count; 
  count = count_list(listVARS); 
  if (count == 0){
    return;
  }

  node_t *cp_listVARS = initialize_list();

  char **DATA;
  DATA = (char**)malloc(count*sizeof(char*));
  
  int j;
  for (j = 0; j<10; j++){
    DATA[j] = malloc(buffersize*sizeof(char));
  }

  int i = 0;
  while (listVARS->next != NULL){
    char* cmd = malloc(sizeof(char*));
    strcpy(cmd,":DATA? ");
    DATA[i]=_read(ud, strcat(cmd, strdup(listVARS->data)), buffersize);
    add_to_list(cp_listVARS, strdup(listVARS->data));
    
    // If there is no data ... then just quit. 
    // Otherwise write will issue SEGFAULT
    if (!(strcmp(DATA[i],"")))
      return 0;

    listVARS    = listVARS->next;
    free(cmd);
    i++;
  }

  // Rewind the linked list. Otherwise we will get a
  // a segfault.
  writeToFile(filename, cp_listVARS, DATA, buffersize);
}









