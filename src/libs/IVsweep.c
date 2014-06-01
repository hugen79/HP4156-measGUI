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

int setSingleDouble(int ud, char* tmp){
  char cmd[32]= ":PAGE:MEAS:MSET:ITIM:MODE "; 
  strcat(cmd, tmp); 
  _write(ud, cmd);
  return 0; 
}

int setIntegrationTime(int ud, char* tmp){ 
  char cmd[32]= ":PAGE:MEAS:VAR1:MODE "; 
  strcat(cmd, tmp); 
  _write(ud, cmd);
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

int savedata(int ud, char* filename, char liststr[], int buffersize){

  // DO nothing if liststr is empty
  if(liststr == NULL)
    return 0;
  if(!(strcmp(liststr,"")))
    return 0;
  if(!(strcmp(liststr,"[EMPTY LIST]"))) 
    return 0;

  // First allocate some memory for the data 
  // strings. These are LONG so they need to 
  // be allocated on the HEAP to prevent a 
  // STAAAACCCKKKK OVERFLOOWWWW
  int j,k,count;
  char **DATA;
  DATA = (char**)malloc(8*sizeof(char*));
  for (j = 0; j<8; j++)
    DATA[j] = (char*)malloc(buffersize*sizeof(int));
    
  // Construct the header first from our copy 
  // of liststr and write to file. Note that we 
  // are simultaneously constructing the file 
  // header to save a few lines of code. 
  j = 0;
  count = 0;
  char header[32] = "";
  char* col = strtok(liststr," "); 
  while (col != NULL){
    // construct the data header 
    strcat(header,col);
    strcat(header,"\t\t");
    // construct the data command and request the data.
    // If there is no data then just return.  
    char cmd[16] = ":DATA? ";
    DATA[j] = _read(ud, strcat(cmd, col), buffersize);
    if (!(strcmp(DATA[j],"")))
      return -1;
  
    // strtok again to get the next thing
    col = strtok(NULL," ");
    count++;
    j++;
  }
  strcat(header, "\n");
 
  printf("%s", DATA[0]);

  // Now we have the data AND the header constructed. 
  // At this point we need to write out the file. 
  // First we get a file pointer and print the header.
  FILE *file; 
  file = fopen(filename,"w+");  
  fprintf(file,header);

  // Now we will write out the actual file. For this we
  // will use a rather advanced application of strtok. 
  // this is advanagous because one only passes through 
  // each data list ONCE ... rather than twice (i.e. the
  // list method). First we need an array of test and 
  // save pointers. 
  char *testpointer[count]; 
  char *savepointer[count];

  // And an array for each line. Now for stktok, we need 
  // to get the starting line. 
  char *line;
  strcpy(line, "");
  for ( j=0;j<count; j++){
    testpointer[j] = strtok_r( DATA[j],",\n", &savepointer[j]);                         
    strcat(line, testpointer[j]);
    strcat(line,"\t");
  }
  strcat(line,"\n");
  fprintf(file, line);
  // Once we have thw initial pointers ... we just loop through 
  // all of the lines.
  // while (testpointer[0]){

  int bool = 1;
  while (bool){
    strcpy(line,"");
    for ( j=0;j<count; j++){
      testpointer[j] = strtok_r(NULL,",\n", &savepointer[j]);                         
      if (testpointer[j] == NULL){
	fclose(file);
	for (j =0; j<count; j++)
	  free(DATA[j]);
	free(DATA);
	return 0;
      }
      else{
	strcat(line, testpointer[j]);
	strcat(line,"\t");
      }
    }
    strcat(line,"\n");
    fprintf(file, line);
  }
}









