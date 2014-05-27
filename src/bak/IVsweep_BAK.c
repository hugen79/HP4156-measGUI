/*** write GPIB order ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpib/ib.h"

/*** EXTERNAL FUNCTIONS ***/
char** str_split(char* a_str, const char a_delim);

/*** GPIB INPUT OUTPUT COMMANDS ***/
int write(int ud, char* command)
{
  // WRITE A COMMAND
  // (device, data, len(data))

  ibwrt(ud, command, strlen(command));
  usleep(100000);
  _status(command);
  return 0;
}

char* _read(int ud, char* command, int max_num_bytes)        
{     
  // Write the command
  char *buffer;
  int buffer_size = max_num_bytes + 1;
  buffer = malloc(buffer_size);
  int is_string;
  int i;
  if(buffer == NULL){
    fprintf(stderr, "%s: failed to allocate buffer.\n", __FUNCTION__);
    return ((char*)0);
  }
  memset(buffer, 0, buffer_size);
  
  ibwrt(ud, command, strlen(command));
  sleep(3);
  _status(command);
  printf("trying to read %i bytes from device...\n", max_num_bytes);
  ibrd(ud, buffer, buffer_size - 1);
  sleep(1);
  is_string = 1;
  for(i = 0; i < ThreadIbcntl(); ++i){
    if(isascii(buffer[i]) == 0){
      is_string = 0;
      break;
    }
  }
  if(is_string){
    printf("received string: '%s'\n", buffer);
  }
  // Set up a buffer to recieve data and initialize a 
  // Pointer to the first element of the string
  return buffer;
}

int _status(char* command)
{
  // Prints GPIB status info (for debug)
  printf("--------------------------\n");
  printf("cmd = %s\n",command);
  printf("ibsta = 0x%x\n", ThreadIbsta());
  printf("iberr = %i\n", ThreadIberr());
  printf("ibcnt = %i\n", ThreadIbcnt());
  return 0;
}

int initialize(int gpib)
{ 
  // OPEN A DEVICE 
  // ibdev(board index, primary address, secondary address, timeout, err, err)  
  // T1s = 1 second timeout
  int ud; 
  ud = ibdev(0, gpib, 0, T3s, 1, 0); 
  return ud;   
}

int close(int ud)
{
  // CLOSE THE DEVICE  ibonl(device, number)
  // number == 0 ----> deallocates everything associated with device
  // number != 0 ----> resets to 'default values' 
  ibonl(ud, 1);
  return 0;
}

/*** HP4156 SPECIFIC FUNCTIONS ***/

//SMU CONTROL
int setSMU(int ud, const char *vars[])
{

  //VNAME
  char cmd1[32] = ":PAGE:CHAN:";
  strcat(cmd1,vars[0]);
  strcat(cmd1,":VNAME ");
  write(ud,strcat(cmd1,vars[1]));

  //INAME
  char cmd2[32] = ":PAGE:CHAN:";
  strcat(cmd2,vars[0]);
  strcat(cmd2,":INAME ");
  write(ud,strcat(cmd2,vars[2]));

  //MODE
  char cmd3[32] = ":PAGE:CHAN:";
  strcat(cmd3,vars[0]);
  strcat(cmd3,":MODE ");
  write(ud,strcat(cmd3,vars[3]));
    
  //FUNC
  char cmd4[32] = ":PAGE:CHAN:";
  strcat(cmd4,vars[0]);
  strcat(cmd4,":FUNC ");
  write(ud,strcat(cmd4,vars[4]));
  return 0;
}
//SMU DISABLE
int disableSMU(int ud, const char *var)
{
  char cmd[32] = ":PAGE:CHAN:";
  strcat(cmd,var);
  write(ud,strcat(cmd,":DIS"));
  return 0;
}

//SET VAR1 or VAR2
int setVAR(int ud, int num, const char *vars[])
{

  char cmd[32] = ":PAGE:MEAS:";
  if (num == 1){
    strcat(cmd,"VAR1:");
  }
  else if (num == 2){
    strcat(cmd,"VAR2:");
  }   
  else{
    return -1;
  }
  
  // Linear or Log
  char cmd1[32]; 
  strcpy(cmd1,cmd); 
  strcat(cmd1,"SPAC ");
  write(ud, strcat(cmd1, vars[0]));

  // Start 
  char cmd2[32]; 
  strcpy(cmd2,cmd); 
  strcat(cmd2,"STAR ");
  write(ud, strcat(cmd2, vars[1]));

  // Stop
  char cmd3[32]; 
  strcpy(cmd3,cmd); 
  strcat(cmd3,"STOP ");
  write(ud, strcat(cmd3, vars[2]));

  // Step
  char cmd4[32]; 
  strcpy(cmd4,cmd); 
  strcat(cmd4,"STEP ");
  write(ud, strcat(cmd4, vars[3]));

  // Compliance
  char cmd5[32]; 
  strcpy(cmd5,cmd); 
  strcat(cmd5,"COMP ");
  write(ud, strcat(cmd5, vars[4]));
  return 0;
}

int dispLIST(int ud, char* var)
{
  char cmd[32] = ":PAGE:DISP:LIST ";
  write(ud, strcat(cmd, var));
  return 0;
}


int main(void)
{
  int gpib = 1; 
  int HP4156B = initialize(gpib);

  // Setup device
  write(HP4156B,"*CLS");
  write(HP4156B,"*RST");
  write(HP4156B,":FORM:DATA ASCii");
  
  // Sweep mode variables
  write(HP4156B,":PAGE:CHAN:MODE SWE");
  write(HP4156B,":PAGE:MEAS:MSET:ITIM SHORT");

  // Set up the SMU channels
  const char *SMU1[] = {"SMU1","'VS'","'IS'","COMM","CONS"};
  setSMU(HP4156B,SMU1);
  const char *SMU2[] = {"SMU2","'VD'","'ID'","V","VAR1"};
  setSMU(HP4156B,SMU2);
  const char *SMU3[] = {"SMU3","'VG'","'IG'","COMM","CONS"};
  setSMU(HP4156B,SMU3);
  disableSMU(HP4156B,"SMU4");
  disableSMU(HP4156B,"SMU5");

  // Set up VAR1 
  const char *VAR1[] = {"LIN","-0.1","1.5","0.1","100mA"};
  setVAR(HP4156B,1,VAR1);

  // Perform Measurement
  write(HP4156B,":PAGE:SCON:SING");
  write(HP4156B,"*WAI");
  sleep(2);

  // Read Measurement Data
  int buffer_size = 8192;

  char* vd_data;
  vd_data = malloc(buffer_size);

  char* id_data;
  vd_data = malloc(buffer_size);

  vd_data = _read(HP4156B, ":DATA? 'VD'", buffer_size);
  id_data = _read(HP4156B, ":DATA? 'ID'", buffer_size);

  // WRITE TO FILE
  // Format output data (first remove newlines at end)
  FILE *file; 
  file = fopen("test.dat","w+"); 
 
  vd_data[strlen(vd_data)-1] = 0; 
  id_data[strlen(id_data)-1] = 0;

  // Populate buf_1, buf_2, and buf_3
  char *save_ptr1, *save_ptr2;
  char *vd_tok, *id_tok;

  // get the initial tokens
  vd_tok = strtok_r(vd_data, ",", &save_ptr1);
  id_tok = strtok_r(id_data, ",", &save_ptr2);

  fprintf(file, "VD              ID\n");
  while(vd_tok && id_tok) {
    // get next tokens
    fprintf(file, "%s\t\%s\n", vd_tok, id_tok);
    vd_tok = strtok_r(NULL, ",", &save_ptr1);
    id_tok = strtok_r(NULL, ",", &save_ptr2);
  }

  fclose(file); /*done!*/ 

  // Format and dump data to file
  close(HP4156B);
  return 0;
}

