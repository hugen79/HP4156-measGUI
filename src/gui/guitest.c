#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../inc/stringutils.h"
#include "../inc/linkedlist.h"
#include "../inc/IVsweep.h"
#include "../inc/gpib_io.h"

#define WIDTH 850
#define HEIGHT 500

#define BWIDTH  120
#define BHEIGHT 45
#define BUFFERSIZE 32000

#define X1 15
#define X2 155
#define X3 295
#define X4 435
#define X5 575
#define X6 715

int gpibADDR;
int gpibHANDLE;

//////////////////////////////////////////
// A STRUCT TO HOLD ALL THE GUI OBJECTS //
//////////////////////////////////////////
typedef struct
{ 
  int NUMBER;

  // The basic window
  GtkWidget *window;
  GtkWidget *fixed; 

  // Initialize and Measure
  GtkWidget *initBUTTON; 
  GtkWidget *measBUTTON;
  GtkWidget *gpibBUTTON;

  // SMU control
  GtkWidget *smuBUTTON; 
  GtkWidget *disBUTTON;  
  GtkWidget *smuLABEL1;
  GtkWidget *smuLABEL2;
  GtkWidget *smuLABEL3;
  GtkWidget *smuLABEL4;
  GtkWidget *smuLABEL5;
  GtkWidget *smuLABEL6;
  GtkWidget **SMU;

  // VAR control
  GtkWidget *varBUTTON;
  GtkWidget *varLABEL1;
  GtkWidget *varLABEL2;
  GtkWidget *varLABEL3;
  GtkWidget *varLABEL4;
  GtkWidget *varLABEL5;
  GtkWidget **VAR;

  // List Control
  GtkWidget *listENTRY; 
  GtkWidget *listCOMBO; 
  GtkWidget *listADD; 
  GtkWidget *listREM;
  GtkWidget *listALL;
  node_t *comboVARS;
  node_t *listVARS;
  char* listSTR;

  // Save Data 
  GtkWidget *saveWINDOW;
  GtkWidget *saveBUTTON;
  GtkWidget *saveENTRY;
  GtkWidget *saveDATA;
  GtkWidget *saveINC; 
  GtkWidget *saveLABEL;

  char* filename;
  int increment; 

} GTKwrapper; 

////////////////////////////////////
// Destroy callback for main loop //
////////////////////////////////////
static void destroy(GtkWidget *widget, gpointer data)
{
  _close(gpibHANDLE);
  gtk_main_quit();
}

////////////////////////////
// MAIN WINDOW GENERATION //
////////////////////////////
static void generateWindow(GTKwrapper* state){
  /* Create a new window, give it a title and display it to the user. */
  state->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  state->saveWINDOW = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (state->window), "HP-4156B Sweep Control");
  gtk_window_set_default_size(GTK_WINDOW(state->window), WIDTH, HEIGHT);
  
  state->fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(state->window), state->fixed); 
}


//////////////////////////////////////////
// INITIALIZATION AND MEASURE CALLBACKS //
//////////////////////////////////////////
static void SETGPIB(GtkWidget *gpibBUTTON, gpointer data)
{ 
  gpibADDR = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gpibBUTTON));

}
static int INITIALIZE_GPIB(GtkWidget *initBUTTON)
{  
  gpibHANDLE = _initialize(gpibADDR);
  disableSMU(gpibHANDLE,"SMU1");
  disableSMU(gpibHANDLE,"SMU2");
  disableSMU(gpibHANDLE,"SMU3");
  disableSMU(gpibHANDLE,"SMU4");
  disableSMU(gpibHANDLE,"SMU5");
}
static void MEASURE(GtkWidget *measBUTTON)
{
  measure(gpibHANDLE);
}

///////////////////////////////////////////
// INITIALIZATION AND MEASURE GENERATION //
///////////////////////////////////////////
static void generateINIT(GTKwrapper* state){
   /* Initialize GPIB button */
  state->initBUTTON = gtk_button_new_with_label("Initialize GPIB");
  g_signal_connect(state->initBUTTON,"clicked", G_CALLBACK(INITIALIZE_GPIB),NULL);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->initBUTTON, X1, 15);
  gtk_widget_set_size_request(state->initBUTTON, BWIDTH, BHEIGHT);
  
  /* GPIB address selector */
  GtkWidget* adj = (GtkWidget*)gtk_adjustment_new(0,1,30,1,1,0);
  state->gpibBUTTON = gtk_spin_button_new(GTK_ADJUSTMENT(adj),1,2);
  g_signal_connect(state->gpibBUTTON,"value-changed", G_CALLBACK(SETGPIB), NULL);
  gtk_spin_button_set_digits(GTK_SPIN_BUTTON (state->gpibBUTTON),0);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->gpibBUTTON, 150, 15);
  gtk_widget_set_size_request(state->gpibBUTTON, 100 , BHEIGHT);
    
  /* Measure Button */
  state->measBUTTON = gtk_button_new_with_label("Measure");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->measBUTTON, X6, 15);
  g_signal_connect(state->measBUTTON,"clicked", G_CALLBACK(MEASURE),NULL);
  gtk_widget_set_size_request(state->measBUTTON, BWIDTH, BHEIGHT);
}


///////////////////////////
// SMU CONTROL CALLBACKS //
///////////////////////////
static int SETSMU(GtkWidget *smuBUTTON,  GTKwrapper *state)
{
  // read everything
  char* _smu    = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->SMU[0]);
  char* vtmp    = (char*)gtk_entry_get_text((GtkEntry*)state->SMU[1]); 
  char* itmp    = (char*)gtk_entry_get_text((GtkEntry*)state->SMU[2]); 
  char* mode    = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->SMU[3]); 
  char* _var    = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->SMU[4]);
  char* cons    = (char*)gtk_entry_get_text((GtkEntry*)state->SMU[5]);
  char* comp    = (char*)gtk_entry_get_text((GtkEntry*)state->SMU[6]);      
 
  gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT (state->listCOMBO));   
  add_to_list_unique(state->comboVARS,strdup(vtmp));
  add_to_list_unique(state->comboVARS,strdup(itmp));
  while (state->comboVARS->next != NULL){
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->listCOMBO),NULL,state->comboVARS->data);
    state->comboVARS = state->comboVARS->next;
    gtk_combo_box_set_active (GTK_COMBO_BOX (state->listCOMBO), 0);
  }
  // rewind the pointer
  state->comboVARS = state->comboVARS->head;

  // format vname and iname for the gpib
  char* vname;
  char* iname;

  vname = stringify(vtmp);
  iname = stringify(itmp);

  if (! ( (int)strcmp(_var,"CONS") )){
    // Black magick bitwise operations for mode selection
    if (! ((int)strcmp("V", mode) & (int)strcmp(mode,"I"))){
      const char *data[7] = {_smu, vname, iname, mode, _var, cons, comp};
      setSMU(gpibHANDLE, data);
    }
    else {
      const char *data[7] = {_smu, vname, iname, mode, _var, NULL, NULL};
      setSMU(gpibHANDLE, data);
    }
  }
  else {
    const char *data[7] = {_smu, vname, iname, mode, _var, NULL, NULL};
    setSMU(gpibHANDLE, data);
  }
}
static void DISSMU(GtkWidget *disBUTTON,  GTKwrapper *state)
{
  gtk_entry_set_text((GtkEntry*)state->SMU[1],"");
  gtk_entry_set_text((GtkEntry*)state->SMU[2],"");
  char* SMUX = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->SMU[0]);
  disableSMU(gpibHANDLE, SMUX);
}


static void smuChanger(GtkWidget *widget, GTKwrapper* state){
  char *tmp1;
  char *tmp2;
  tmp1 = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->SMU[3]);
  tmp2 = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->SMU[4]);
  
  // If in common constant mode ... turn off everything
  if ((strcmp(tmp1,"COMM") == 0) & (strcmp(tmp2,"CONS") == 0)) {
    gtk_entry_set_text(GTK_ENTRY(state->SMU[5]), "------------------");
    gtk_entry_set_text(GTK_ENTRY(state->SMU[6]), "------------------");
    gtk_editable_set_editable(GTK_EDITABLE(state->SMU[5]), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(state->SMU[6]), FALSE);
  }

  else if(strcmp(tmp2,"CONS") == 0  ){
    gtk_entry_set_text(GTK_ENTRY(state->SMU[5]), "");
    gtk_editable_set_editable(GTK_EDITABLE(state->SMU[5]), TRUE);
    gtk_entry_set_text(GTK_ENTRY(state->SMU[6]), "");
    gtk_editable_set_editable(GTK_EDITABLE(state->SMU[6]), TRUE);
  }
  else {
    gtk_entry_set_text(GTK_ENTRY(state->SMU[5]), "------------------");
    gtk_entry_set_text(GTK_ENTRY(state->SMU[6]), "------------------");
    gtk_editable_set_editable(GTK_EDITABLE(state->SMU[5]), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(state->SMU[6]), FALSE);
  }
}


///////////////////////////
// SMU CONTROL GENRATION //
///////////////////////////
static void generateSMU(GTKwrapper* state){
  
  /* set SMU control */
  state->smuBUTTON = gtk_button_new_with_label("OK");
  g_signal_connect(state->smuBUTTON,"clicked", G_CALLBACK(SETSMU), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->smuBUTTON, X6, 110);
  gtk_widget_set_size_request(state->smuBUTTON, BWIDTH, BHEIGHT);

  state->disBUTTON = gtk_button_new_with_label("!DISABLE!");
  g_signal_connect(state->disBUTTON,"clicked", G_CALLBACK(DISSMU), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->disBUTTON, X6, 180);
  gtk_widget_set_size_request(state->disBUTTON, BWIDTH, BHEIGHT);

  state->SMU = g_new(GtkWidget*, 8);
      
  //GtkListStore *SMULIST = gtk_list_store_new(5, G_TYPE_STRING,G_TYPE_INT,G_TYPE_BOOLEAN);
  state->SMU[0] = gtk_combo_box_text_new();
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->SMU[0]),NULL, "SMU1");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->SMU[0]),NULL, "SMU2");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->SMU[0]),NULL, "SMU3");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->SMU[0]),NULL, "SMU4");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->SMU[0]),NULL, "SMU5");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->SMU[0]),NULL, "GNDU");
  gtk_combo_box_set_active(GTK_COMBO_BOX(state->SMU[0]),0);
  gtk_widget_set_size_request(state->SMU[0], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SMU[0], X1, 110);
 
  // VNAME 
  state->SMU[1] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->SMU[1]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->SMU[1],14);
  gtk_widget_set_size_request(state->SMU[1], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SMU[1], X2, 110);
  state->smuLABEL1 = gtk_label_new("(V) name");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->smuLABEL1, X2, 90);
    
  // INAME
  state->SMU[2] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->SMU[2]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->SMU[2],14);
  gtk_widget_set_size_request(state->SMU[2], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SMU[2], X3, 110);
  state->smuLABEL2 = gtk_label_new("(I) name");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->smuLABEL2, X3, 90);

  // MODE
  state->SMU[3] = gtk_combo_box_text_new ();
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->SMU[3]),NULL, "V");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->SMU[3]),NULL, "I");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->SMU[3]),NULL, "COMM");
  gtk_combo_box_set_active (GTK_COMBO_BOX (state->SMU[3]), 0);
  gtk_widget_set_size_request(state->SMU[3], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SMU[3], X4, 110);
  state->smuLABEL3 = gtk_label_new("Mode");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->smuLABEL3, X4, 90);
  g_signal_connect(state->SMU[3],"changed", G_CALLBACK(smuChanger), state);

  // CONSTANT OR VARIABLE
  state->SMU[4] = gtk_combo_box_text_new ();
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->SMU[4]),NULL, "CONS");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->SMU[4]),NULL, "VAR1");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->SMU[4]),NULL, "VAR2");
  gtk_combo_box_set_active (GTK_COMBO_BOX (state->SMU[4]), 0);
  gtk_widget_set_size_request(state->SMU[4], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SMU[4], X5, 110);
  state->smuLABEL4 = gtk_label_new("VAR Select");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->smuLABEL4, X5, 90);
  g_signal_connect(state->SMU[4],"changed", G_CALLBACK(smuChanger), state);
 

  // CONST VALUE
  state->SMU[5] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->SMU[5]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->SMU[5],14);
  gtk_widget_set_size_request(state->SMU[5], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SMU[5], X4, 180);
  state->smuLABEL5 = gtk_label_new("Const Value");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->smuLABEL5, X4, 160);

  // CONST COMP
  state->SMU[6] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->SMU[6]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->SMU[6],14);
  gtk_widget_set_size_request(state->SMU[6], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SMU[6], X5, 180);
  state->smuLABEL6 = gtk_label_new("Const Compliance");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->smuLABEL6, X5, 160);
}


////////////////////////////////
// VARIABLE CONTROL CALLBACKS //
////////////////////////////////
static void SETVAR(GtkWidget *smuBUTTON,  GTKwrapper *state)
{
  char* _var = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->VAR[0]);
  char* mode = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->VAR[1]);
  char* star = (char*)gtk_entry_get_text((GtkEntry*)state->VAR[2]);
  char* stop = (char*)gtk_entry_get_text((GtkEntry*)state->VAR[3]);
  char* step = (char*)gtk_entry_get_text((GtkEntry*)state->VAR[4]);
  char* comp = (char*)gtk_entry_get_text((GtkEntry*)state->VAR[5]);
  
  const char *data[6] = {_var, mode, star, stop, step, comp};
  setVAR(gpibHANDLE, data);
}

static void varChanger(GtkWidget *widget, GTKwrapper* state){
  char *tmp;
  tmp = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->VAR[0]);
  if (strcmp(tmp,"VAR2") == 0){
    gtk_label_set_text((GtkLabel*)state->varLABEL3,"Npoints");
    gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT (state->VAR[1]),1);
    gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT (state->VAR[1]),1);
    gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT (state->VAR[1]),1);
    gtk_combo_box_set_active (GTK_COMBO_BOX (state->VAR[1]),0);
  }
  else{
    gtk_label_set_text((GtkLabel*)state->varLABEL3,"Stop");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[1]),NULL, "L10");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[1]),NULL, "L25");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[1]),NULL, "L50");
    gtk_combo_box_set_active (GTK_COMBO_BOX (state->VAR[1]), 0);
  }
}

static void modeChanger(GtkWidget *widget, GTKwrapper* state){
  char *tmp;
  tmp = (char*)gtk_combo_box_get_active((GtkComboBox*)state->VAR[1]);
  if (strcmp(tmp,"LIN") != 0){
    gtk_entry_set_text(GTK_ENTRY(state->VAR[4]), "------------------");
    gtk_editable_set_editable(GTK_EDITABLE(state->VAR[4]), FALSE);
  }
  else{
    gtk_entry_set_text(GTK_ENTRY(state->VAR[4]), "");
    gtk_editable_set_editable(GTK_EDITABLE(state->VAR[4]), TRUE);
  }
}

/////////////////////////////////
// VARIABLE CONTROL GENERATION //
/////////////////////////////////
static void generateVAR(GTKwrapper* state){
  
  /* set VAR control */
  state->varBUTTON = gtk_button_new_with_label("OK");
  g_signal_connect(state->varBUTTON,"clicked", G_CALLBACK(SETVAR), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->varBUTTON, X6, 320);
  gtk_widget_set_size_request(state->varBUTTON, BWIDTH, BHEIGHT);
  state->VAR = g_new(GtkWidget*, 6);

  // START
  state->VAR[2] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->VAR[2]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->VAR[2],14);
  gtk_widget_set_size_request(state->VAR[2], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->VAR[2], X3, 270);
  state->varLABEL2 = gtk_label_new("Start");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->varLABEL2, X3, 250);

  // STOP
  state->VAR[3] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->VAR[3]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->VAR[3],14);
  gtk_widget_set_size_request(state->VAR[3], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->VAR[3], X4, 270);
  state->varLABEL3 = gtk_label_new("Stop");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->varLABEL3, X4, 250);

  // STEP
  state->VAR[4] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->VAR[4]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->VAR[4],14);
  gtk_widget_set_size_request(state->VAR[4], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->VAR[4], X5, 270);
  state->varLABEL4 = gtk_label_new("Step");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->varLABEL4, X5, 250);

  // COMP
  state->VAR[5] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->VAR[5]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->VAR[5],14);
  gtk_widget_set_size_request(state->VAR[5], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->VAR[5], X6, 270);
  state->varLABEL5 = gtk_label_new("Compliance");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->varLABEL5, X6, 250);

  // VARIABLE SELECTOR
  state->VAR[0] = gtk_combo_box_text_new ();
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[0]),NULL, "VAR1");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[0]),NULL, "VAR2");
  gtk_combo_box_set_active (GTK_COMBO_BOX (state->VAR[0]), 0);
  gtk_widget_set_size_request(state->VAR[0], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->VAR[0], X1, 270);
  g_signal_connect(state->VAR[0],"changed", G_CALLBACK(varChanger), state);

  // MODE SELECTOR
  state->VAR[1] = gtk_combo_box_text_new ();
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[1]),NULL, "LIN");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[1]),NULL, "L10");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[1]),NULL, "L25");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[1]),NULL, "L50");
  gtk_combo_box_set_active (GTK_COMBO_BOX (state->VAR[1]), 0);
  gtk_widget_set_size_request(state->VAR[1], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->VAR[1], X2, 270);
  g_signal_connect(state->VAR[1],"changed", G_CALLBACK(modeChanger), state);
  state->varLABEL1 = gtk_label_new("Mode");

}


////////////////////////////
// LIST CONTROL CALLBACKS //
////////////////////////////
static void LISTADD(GtkWidget* listADD, GTKwrapper* state){
  char* var;
  var = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->listCOMBO);
  add_to_list_unique( state->listVARS,strdup(var));
  strcpy(state->listSTR,(char*)print_list_to_string(state->listVARS));
  gtk_entry_set_text((GtkEntry*)state->listENTRY,state->listSTR);
  addToList(gpibHANDLE, stringify(var));
}

static void LISTREM(GtkWidget* listREM, GTKwrapper* state){

  char* var;
  var = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->listCOMBO);
  if (var != NULL)
    remove_from_list(&(state->listVARS),strdup(var));

  strcpy(state->listSTR,(char*)print_list_to_string(state->listVARS));
  gtk_entry_set_text((GtkEntry*)state->listENTRY,state->listSTR);
}

static void LISTALL(GtkWidget* listREMALL, GTKwrapper* state){
  char* var;
  var = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->listCOMBO);
  remove_all_from_list(&(state)->listVARS );
  strcpy(state->listSTR,(char*)print_list_to_string(state->listVARS));
  gtk_entry_set_text((GtkEntry*)state->listENTRY,state->listSTR);
}

/////////////////////////////
// LIST CONTROL GENERATION //
/////////////////////////////
static void generateListControl(GTKwrapper *state){


  state->listCOMBO = gtk_combo_box_text_new();
  gtk_combo_box_set_active(GTK_COMBO_BOX(state->listCOMBO),0);
  gtk_widget_set_size_request(state->listCOMBO, BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->listCOMBO, X1, 395);

  state->listENTRY = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->listENTRY), FALSE);
  gtk_entry_set_width_chars((GtkEntry*)state->listENTRY,14);
  gtk_widget_set_size_request(state->VAR[5], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->listENTRY,X4,395);
  gtk_widget_set_size_request(state->listENTRY, BWIDTH, BHEIGHT);

  state->listADD = gtk_button_new_with_label("Add DATA");
  g_signal_connect(state->listADD,"clicked", G_CALLBACK(LISTADD), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->listADD, X2, 395);
  gtk_widget_set_size_request(state->listADD, BWIDTH, BHEIGHT);

  state->listREM = gtk_button_new_with_label("Remove DATA");
  g_signal_connect(state->listREM,"clicked", G_CALLBACK(LISTREM), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->listREM, X3, 395);
  gtk_widget_set_size_request(state->listREM, BWIDTH, BHEIGHT);

  state->listALL = gtk_button_new_with_label("Remove ALL");
  g_signal_connect(state->listALL,"clicked", G_CALLBACK(LISTALL), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->listALL, X6, 395);
  gtk_widget_set_size_request(state->listALL, BWIDTH, BHEIGHT);
}


////////////////////////////
// SAVE CONTROL CALLBACKS //
////////////////////////////
static void SAVEPATH(GtkWidget* saveBUTTON, GTKwrapper* state){

  GtkWidget *chooser;
  chooser = gtk_file_chooser_dialog_new ("Open File...",
					 (GtkWindow*)state->window,
					 GTK_FILE_CHOOSER_ACTION_SAVE,
					 (gchar*)"_Cancel", 
					 GTK_RESPONSE_CANCEL,
					 (gchar*)"_Open", 
					 GTK_RESPONSE_OK,
					 NULL);
	
  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (chooser), TRUE);
  if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
  {
      state->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
      gtk_entry_set_text(GTK_ENTRY(state->saveENTRY), state->filename);
  }
  gtk_widget_destroy (chooser);
}

static void SAVEDATA(GtkWidget* saveBUTTON, GTKwrapper* state){

  if (!strcmp(state->filename,"")){
    gtk_entry_set_text(GTK_ENTRY(state->saveENTRY),"ERROR .. FILENAME NOT SET!!");
    return;
  }

  int INC = (int)gtk_switch_get_active ((GtkSwitch*)state->saveINC);
  if (INC) {

    // Append the incrementor to the filename
    // e.g. you will get <path.dat.0> etc. We 
    // need some string manipulation for this. 
    char tmpPath[100]; 
    char incPath[3];

    strcpy(tmpPath,state->filename);
    int len = strlen(tmpPath);
    sprintf(incPath, "%d", state->increment);

    // add a . to the pathname
    tmpPath[len]   = '.';
    tmpPath[len+1] = '\0';
    strcat(tmpPath, incPath);

    // update the file name in the entry field
    gtk_entry_set_text(GTK_ENTRY(state->saveENTRY), tmpPath);
    state->increment++;
    savedata(gpibHANDLE, tmpPath, state->listVARS, (int)BUFFERSIZE);
  }
  else {
    // if the incrementor is deselected then reset.
    state->increment = 0;
    savedata(gpibHANDLE, state->filename, state->listVARS, (int)BUFFERSIZE);
  }
}

/////////////////////////////
// SAVE CONTROL GENERATION //
/////////////////////////////
static void generateFileSelector(GTKwrapper *state)
{
  /* set SMU control */
  state->saveBUTTON = gtk_button_new_with_label("<filename>");
  g_signal_connect(state->saveBUTTON,"clicked", G_CALLBACK(SAVEPATH), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveBUTTON, X1, 450);
  gtk_widget_set_size_request(state->saveBUTTON, BWIDTH, BHEIGHT);

  // entry box to display filename
  state->saveENTRY = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->saveENTRY), FALSE);
  gtk_widget_set_size_request(state->saveENTRY, 405, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveENTRY,X2,450);
  
  // Incrementor. This allows data to be saved in rapid sucsession 
  // e.g. data.dat.0 .... data.dat.n. If toggled, it will increment 
  // the save path ... otherwise it will overwrite the data
  state->saveINC =  gtk_switch_new();
  gtk_widget_set_size_request(state->saveINC, BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveINC, X5, 450);
  gtk_switch_set_active ((GtkSwitch*)state->saveINC, TRUE);
  state->saveLABEL = gtk_label_new("File Incrementor");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveLABEL, X5, 430);

  // Save Button
  state->saveDATA = gtk_button_new_with_label("SAVE");
  g_signal_connect(state->saveDATA,"clicked", G_CALLBACK(SAVEDATA), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveDATA, X6, 450);
  gtk_widget_set_size_request(state->saveDATA, BWIDTH, BHEIGHT);
}


////////////////////////
// WINDOW SEPARATORS  //
////////////////////////
static void generateWindowSeparators(GTKwrapper *state)
{

  GtkWidget* label1; 
  GtkWidget* label2; 
  GtkWidget* label3; 

  GtkWidget* separator1; 
  GtkWidget* separator2;
  GtkWidget* separator3;

  label1 = gtk_label_new("SMU Control");
  separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_size_request(separator1,720, 2);
  gtk_fixed_put(GTK_FIXED(state->fixed), label1, 15, 74);
  gtk_fixed_put(GTK_FIXED(state->fixed), separator1, BWIDTH, 80);

  label2 = gtk_label_new("VAR Control");
  separator2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_size_request(separator2,720, 2);
  gtk_fixed_put(GTK_FIXED(state->fixed), label2, 15, 234);
  gtk_fixed_put(GTK_FIXED(state->fixed), separator2, BWIDTH, 240);

  label3 = gtk_label_new("DATA Control");
  separator3 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_size_request(separator3,650, 2);
  gtk_fixed_put(GTK_FIXED(state->fixed), label3, 15, 374);
  gtk_fixed_put(GTK_FIXED(state->fixed), separator3, BWIDTH,380);
}

/////////////////////////////
//       MAIN LOOP         //
/////////////////////////////
int main (int argc, char *argv[])
{
  GTKwrapper* state = malloc(sizeof(GTKwrapper));
  state->filename   = malloc(100);
  state->listSTR    = malloc(100);
  state->increment  = 0;

  state->comboVARS = initialize_list();
  state->listVARS  = initialize_list();
  gtk_init (&argc, &argv); 
  generateWindow(state);

  // Generate INIT and SMU controls
  generateINIT(state);
  generateSMU(state);
  generateVAR(state);
  
  // Generate list control and Save stuff
  generateListControl(state);
  generateFileSelector(state);
  generateWindowSeparators(state);

  /* Hand control over to the main loop. */
  gtk_widget_show_all(state->window); 
  gtk_main ();

  free(state);
  return 0;    
}








