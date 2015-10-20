#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "gpib_io.h"
#include "CVutils.h"

// GPIB BUFFER SIZE 32kb 
#define BUFFERSIZE 512

// GUI Window SIZE
#define WIDTH 600
#define HEIGHT 400

// BASIC Widget SIZE
#define BWIDTH 120
#define BHEIGHT 45


// WIDGET PLACEMENT 
#define XSPACE 150
#define YSPACE 70

#define X1 15
#define X2 X1+XSPACE
#define X3 X2+XSPACE
#define X4 X3+XSPACE
#define X5 X4+XSPACE
#define X6 X5+XSPACE

#define Y1 15
#define Y2 Y1+YSPACE+25
#define Y3 Y2+YSPACE
#define Y4 Y3+YSPACE+25
#define Y5 Y4+YSPACE
#define Y6 Y5+YSPACE+25
#define Y7 Y6+YSPACE

// Some global variables
int gpibADDR;      //<- set by SETGPIB
int gpibHANDLE;    //<- set by INITIALIZE_GPIB

typedef struct{
  ////////////////////////////
  // THE APPLICATION ITSELF //
  ////////////////////////////
  GtkApplication *app;
  // the main window
  GtkWidget *window;
  GtkWidget *fixed;
  GdkWindow *gd;

  ////////////////////////////
  // INITIALIZATION WIDGETS //
  ////////////////////////////
  // Initialize and Measure
  GtkWidget *initBUTTON; 
  GtkWidget *measBUTTON;
  GtkWidget *gpibBUTTON;

  // MODE/BIAS/OSCILLATOR CONTROLS
  GtkWidget *M_LABEL;
  GtkWidget **MOD;
  GtkWidget **OSC;
  GtkWidget **SWP;

  // VARIABLES FOR SWEEP
  double *swp;
  int     len;

  // VARIABLES FOR OSCILLATOR CONTROL 
  char** FLST;
  char*  FREQ;
  char*  AMPL;
  int    lenf;

  // VARIABLES FOR SAVING DATA
  char** DATA;

  // MODE/BIAS/OSCILLATOR CONTROLS
  GtkWidget **oscLABELS;
  GtkWidget **swpLABELS;

  // MASTER BIAS SWITCH 
  GtkWidget *B_LABEL;
  GtkWidget *B_SWITCH;

  // SAVE FUNCTIONALITY
  GtkWidget *saveWINDOW;
  GtkWidget *saveBUTTON;
  GtkWidget *saveENTRY;
  GtkWidget *saveDATA;
  GtkWidget *saveINC; 
  GtkWidget *saveLABEL;
  char* filename;
  int increment; 

  // PRETTY LABEL
  GtkWidget *MODELABEL;  

}GTKwrapper;

//////////////////////////////////////////
// INITIALIZATION AND MEASURE CALLBACKS //
//////////////////////////////////////////
static void SETGPIB(GtkWidget *gpibBUTTON, gpointer data)
{ 
  gpibADDR = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gpibBUTTON));
}
static int INITIALIZE_GPIB(GtkWidget *initBUTTON, GTKwrapper* state)
{
  gpibHANDLE = _initialize(gpibADDR);
  setDefaults(gpibHANDLE);
}

static int MEASURE(GtkWidget *widget, GTKwrapper *state){
  // Only measure if everything is set
  if (state->len != 0 && state->lenf != 0){
    state->DATA = measure(gpibHANDLE, state->swp, state->len, state->FLST, state->lenf, BUFFERSIZE);
  }
}

///////////////////////////////////////////
// INITIALIZATION AND MEASURE GENERATION //
///////////////////////////////////////////
static void generateINIT(GTKwrapper* state){
   /* Initialize GPIB button */
  state->initBUTTON = gtk_button_new_with_label("Initialize GPIB");
  g_signal_connect(state->initBUTTON,"clicked", G_CALLBACK(INITIALIZE_GPIB),state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->initBUTTON, X1, Y1);
  gtk_widget_set_size_request(state->initBUTTON, BWIDTH, BHEIGHT);
  
  /* GPIB address selector */
  GtkWidget* adj = (GtkWidget*)gtk_adjustment_new(0,1,30,1,1,0);
  state->gpibBUTTON = gtk_spin_button_new(GTK_ADJUSTMENT(adj),1,2);
  g_signal_connect(state->gpibBUTTON,"value-changed", G_CALLBACK(SETGPIB), NULL);
  gtk_spin_button_set_digits(GTK_SPIN_BUTTON (state->gpibBUTTON),0);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->gpibBUTTON, X2, Y1);
  gtk_widget_set_size_request(state->gpibBUTTON, 100 , BHEIGHT);
    
  /* Measure Button */
  state->measBUTTON = gtk_button_new_with_label("Measure");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->measBUTTON, X4, Y1);
  g_signal_connect(state->measBUTTON,"clicked", G_CALLBACK(MEASURE),state);
  gtk_widget_set_size_request(state->measBUTTON, BWIDTH, BHEIGHT);
}


////////////////////////////////////
//         MODE CONTROL           //
////////////////////////////////////
static void SETMODE(GtkWidget *widget, GTKwrapper *state){
  char* MODE = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->MOD[0]);
  setMode(gpibHANDLE, MODE);
}

static void CALOPEN(GtkWidget *widget, GTKwrapper *state){
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(state->MOD[1]))){
    calOpen(gpibHANDLE);
  }
}
static void CALSHOR(GtkWidget *widget, GTKwrapper *state){
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(state->MOD[2]))){
    calShort(gpibHANDLE);
  }
}
static void BSWITCH(GtkSwitch *sw, GTKwrapper *state){

  int BOOL = (int)gtk_switch_get_active((GtkSwitch*)sw);
  if (BOOL){ 
    setBiasON(gpibHANDLE);
  }
  else {
    setBiasOFF(gpibHANDLE); 
  } 
}

static void generateCONTROL(GTKwrapper* state){ 

  state->MOD = g_new(GtkWidget*, 8);
  // Mode Selector
  state->M_LABEL = gtk_label_new("Mode Selection");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->M_LABEL, X1, (int)Y2-20);

  state->MOD[0] = gtk_combo_box_text_new();
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "CPD");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "CPQ");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "CPG");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "CPRP");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "CSD");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "CSQ");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "CSRS");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "LPQ");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "LPD");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "LPC");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "LPRP");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "LSD");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "LSQ");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "LSRS");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "RX");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT(state->MOD[0]),NULL, "GB");
  gtk_combo_box_set_active(GTK_COMBO_BOX(state->MOD[0]),0);
  gtk_widget_set_size_request(state->MOD[0], BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->MOD[0], X1, Y2);
  g_signal_connect(state->MOD[0],"changed", G_CALLBACK(SETMODE), state);

  // Calibration Selectors
  const gchar *calopen; 
  calopen = "open calibration";
  state->MOD[1] = gtk_check_button_new_with_label(calopen);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->MOD[1], X2, Y2-5);
  g_signal_connect(state->MOD[1],"toggled", G_CALLBACK(CALOPEN), state);

  const gchar *calshort; 
  calshort = "short calibration";
  state->MOD[2] = gtk_check_button_new_with_label(calshort);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->MOD[2], X2, Y2+20);
  g_signal_connect(state->MOD[2],"toggled", G_CALLBACK(CALSHOR), state);

 
  state->B_LABEL = gtk_label_new("BIAS");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->B_LABEL, X4, (int)Y2-20);

  state->B_SWITCH = gtk_switch_new();
  gtk_widget_set_size_request(state->B_SWITCH, BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->B_SWITCH, X4, Y2);
  g_signal_connect((GtkSwitch*)state->B_SWITCH, "notify::active" , G_CALLBACK(BSWITCH), state);

}

static void SETOSC(GtkWidget *widget, GTKwrapper *state){
  state->FREQ =  (char*)gtk_entry_get_text((GtkEntry*)state->OSC[0]);
  state->AMPL =  (char*)gtk_entry_get_text((GtkEntry*)state->OSC[1]);
  setAmplitude(gpibHANDLE,state->AMPL); // Amplitude is a measurement global

  // Get ready for strtok. Strtok is destructive, 
  // so we need to make a copy of the text string
  state->FLST = (char**)malloc(8*sizeof(char*));
  char* tmp   = (char*)malloc(32*sizeof(char)); 
  strcpy(tmp,  state->FREQ);

  // Go ahead and do the strtoking. 
  // FLST is what we will pass to measure
  char *f;
  int i = 0;
  const char delim[2] = " ";
  f = strtok(tmp, delim);
  while( f != NULL ) {
    state->FLST[i] = (char*)malloc(8*sizeof(char));
    strcpy(state->FLST[i], f); 
    f = strtok(NULL, delim);
    i++;
  }
  state->lenf = i;
}

////////////////////////////////////////
//       OSCILLATOR CONTROL           //
////////////////////////////////////////
static void generateOSCILLATOR(GTKwrapper* state){
  
  state->OSC = g_new(GtkWidget*, 3);
  
  // Frequency Selector
  state->OSC[0] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->OSC[0]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->OSC[0],14);
  gtk_widget_set_size_request(state->OSC[0], BWIDTH, BHEIGHT);
  gtk_entry_set_text(GTK_ENTRY(state->OSC[0]),"0.1");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->OSC[0], X1, Y4);
  
  state->OSC[1] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->OSC[1]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->OSC[1],14);
  gtk_widget_set_size_request(state->OSC[1], BWIDTH, BHEIGHT);
  gtk_entry_set_text(GTK_ENTRY(state->OSC[1]),"0.1");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->OSC[1], X2, Y4);

  state->OSC[2] = gtk_button_new_with_label("Set OSC");
  g_signal_connect(state->OSC[2],"clicked", G_CALLBACK(SETOSC), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->OSC[2], X3, Y4);
  gtk_widget_set_size_request(state->OSC[2], BWIDTH, BHEIGHT);
  
  state->oscLABELS = g_new(GtkWidget*, 3);
  state->oscLABELS[0] = gtk_label_new("Frequencies (MHz)");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->oscLABELS[0], X1, (int)Y4-20);

  state->oscLABELS[1] = gtk_label_new("Amplitude (V)");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->oscLABELS[1], X2, (int)Y4-20);
}

///////////////////////////////////////
//           SWEEP CONTROL           //
///////////////////////////////////////
static void SETSWP(GtkWidget *widget, GTKwrapper *state){
  double MIN =  atof( (char*)gtk_entry_get_text((GtkEntry*)state->SWP[0]) );
  double MAX =  atof( (char*)gtk_entry_get_text((GtkEntry*)state->SWP[1]) );
  int    NPT =  atoi( (char*)gtk_entry_get_text((GtkEntry*)state->SWP[2]) );
  
  if (MIN < -40.0){MIN = -40;}
  if (MAX >  40.0){MAX =  40;}

  // Hysteresis Mode
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(state->SWP[4]))){
    double delta = (MAX-MIN)/(double)NPT;
    state->swp  = malloc((2*NPT+1)*sizeof(double));
    state->len  = 2*NPT+1;
    
    int i;
    for ( i = 0; i <= NPT; i++ ) {
      memcpy(&state->swp[i],&MIN, sizeof(double));
      MIN+=delta;
    }
    MIN-=delta;
    for ( i = 1; i < NPT+1; i++ ){
      MIN-=delta;
      memcpy(&state->swp[NPT+i],&MIN, sizeof(double));
    }
  }

  // Regular Mode
  else{
    double delta = (MAX-MIN)/(double)NPT;
    state->swp  = malloc((NPT+1)*sizeof(double));
    state->len  = NPT+1;
    
    int i;
    for ( i = 0; i <= NPT; i++ ) {
      memcpy(&state->swp[i],&MIN, sizeof(double));
      MIN+=delta;
    }
  }
}

static void generateSWEEP(GTKwrapper* state){
  state->SWP = g_new(GtkWidget*, 5);
 
  // Frequency Selector
  state->SWP[0] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->SWP[0]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->SWP[0],14);
  gtk_widget_set_size_request(state->SWP[0], BWIDTH, BHEIGHT);
  gtk_entry_set_text(GTK_ENTRY(state->SWP[0]),"0.0");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SWP[0], X1, Y3+20);
  
  state->SWP[1] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->SWP[1]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->SWP[1],14);
  gtk_widget_set_size_request(state->SWP[1], BWIDTH, BHEIGHT);
  gtk_entry_set_text(GTK_ENTRY(state->SWP[1]),"5.0");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SWP[1], X2, Y3+20);

  state->SWP[2] = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->SWP[2]), TRUE);
  gtk_entry_set_width_chars((GtkEntry*)state->SWP[2],14);
  gtk_widget_set_size_request(state->SWP[2], BWIDTH, BHEIGHT);
  gtk_entry_set_text(GTK_ENTRY(state->SWP[2]),"10");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SWP[2], X3, Y3+20);
  
  state->SWP[3] = gtk_button_new_with_label("Set SWP");
  g_signal_connect(state->SWP[3],"clicked", G_CALLBACK(SETSWP), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SWP[3], X4, Y3+20);
  gtk_widget_set_size_request(state->SWP[3], BWIDTH, BHEIGHT);

  const gchar *hist; 
  hist = "Hysteresis";
  state->SWP[4] = gtk_check_button_new_with_label(hist);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->SWP[4], X4, Y3-10);

  state->swpLABELS = g_new(GtkWidget*, 3);
  state->swpLABELS[0] = gtk_label_new("Min (V)");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->swpLABELS[0], X1, Y3);
  state->swpLABELS[1] = gtk_label_new("Max (V)");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->swpLABELS[1], X2, Y3);
  state->swpLABELS[2] = gtk_label_new("Npoints");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->swpLABELS[2], X3, Y3);
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
  /* if (state->DATA == NULL){ */
  /*   gtk_entry_set_text(GTK_ENTRY(state->saveENTRY),"ERROR ... NO DATA !"); */
  /*   return; */
  /* } */
  
  // Copy the mode of save vars into a string. We will
  // strtok this later to generate a data header
  char* MODE = (char*)gtk_combo_box_text_get_active_text((GtkComboBoxText*)state->MOD[0]);  
  char oscdata[64] = "\0"; 
  strcpy(oscdata,"*\n");
  strcat(oscdata,"* FREQ = ");
  strcat(oscdata, state->FREQ);
  strcat(oscdata," MHz\n* AMPL = ");
  strcat(oscdata, state->AMPL);
  strcat(oscdata," V\n");
  
  //strcpy(tmpString, state->listSTR);
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
    savedata(gpibHANDLE, tmpPath, MODE, oscdata, state->DATA, state->len, state->lenf);
  }
  else {
    // if the incrementor is deselected then reset.
    state->increment = 0;
    savedata(gpibHANDLE, state->filename, MODE, oscdata, state->DATA, state->len, state->lenf);
  }
}

/////////////////////////////
// SAVE CONTROL GENERATION //
/////////////////////////////
static void generateSaveControl(GTKwrapper *state)
{
  /* set SMU control */
  state->saveBUTTON = gtk_button_new_with_label("<filename>");
  g_signal_connect(state->saveBUTTON,"clicked", G_CALLBACK(SAVEPATH), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveBUTTON, X1, Y5);
  gtk_widget_set_size_request(state->saveBUTTON, BWIDTH, BHEIGHT);

  // entry box to display filename
  state->saveENTRY = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(state->saveENTRY), FALSE);
  gtk_widget_set_size_request(state->saveENTRY, 2*XSPACE-25, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveENTRY,X2,Y5);
  
  // Incrementor. This allows data to be saved in rapid sucsession 
  // e.g. data.dat.0 .... data.dat.n. If toggled, it will increment 
  // the save path ... otherwise it will overwrite the data
  state->saveINC =  gtk_switch_new();
  gtk_widget_set_size_request(state->saveINC, BWIDTH, BHEIGHT);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveINC, X4, Y4);
  gtk_switch_set_active ((GtkSwitch*)state->saveINC, TRUE);
  state->saveLABEL = gtk_label_new("File Incrementor");
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveLABEL, X4, (Y4-20));

  // Save Button
  state->saveDATA = gtk_button_new_with_label("SAVE");
  g_signal_connect(state->saveDATA,"clicked", G_CALLBACK(SAVEDATA), state);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->saveDATA, X4, Y5);
  gtk_widget_set_size_request(state->saveDATA, BWIDTH, BHEIGHT);
}

static void generateMODELABEL(GTKwrapper* state, char* str){
  state->MODELABEL = gtk_label_new(NULL);
  gtk_label_set_use_markup (GTK_LABEL (state->MODELABEL),TRUE);
  gtk_fixed_put(GTK_FIXED(state->fixed), state->MODELABEL,(int)X3,  (int)Y1+10);

  const char* format;
  format = "<span font=\"16.0\" weight=\"bold\" foreground=\"#66D966\">%s</span>";
  char *markup;
  markup = g_markup_printf_escaped (format, str);
  gtk_label_set_markup (GTK_LABEL (state->MODELABEL),markup);
  g_free (markup);
}

///////////////////////////////////////////
//    APPLICATION INITIALIZE AND QUIT    //
/////////////////////////////////////////// 
static void quit (GSimpleAction *action, GVariant *parameter, void* gui_state)
{
  GTKwrapper* _state = (GTKwrapper*)malloc(sizeof(GTKwrapper));
  _state = gui_state; 
  g_application_quit((GApplication*)_state->app);
}

static void activate (GtkApplication *app, GTKwrapper* state)
{
  state->window = (GtkWidget*)gtk_application_window_new (app);
  gtk_window_set_application (GTK_WINDOW (state->window), GTK_APPLICATION (app));
  gtk_window_set_default_size(GTK_WINDOW(state->window), WIDTH, HEIGHT);
  gtk_window_set_title (GTK_WINDOW (state->window), "CV Control");
  gtk_window_set_position(GTK_WINDOW(state->window), GTK_WIN_POS_CENTER);
  gtk_widget_set_app_paintable(state->window, TRUE);
  gtk_window_set_decorated(GTK_WINDOW(state->window), TRUE);
  gtk_widget_set_opacity(GTK_WIDGET(state->window),0.9);

  state->fixed = gtk_fixed_new();
  gtk_widget_set_size_request (state->fixed, WIDTH, HEIGHT);
  gtk_container_add(GTK_CONTAINER(state->window), state->fixed);

  generateMODELABEL(state,"CV \n  Control \n\t v1.1");
  generateINIT(state);
  generateCONTROL(state);
  generateOSCILLATOR(state);
  generateSWEEP(state);
  generateSaveControl(state);
  state->len  = 0;
  state->DATA = NULL;
  gtk_widget_show_all (GTK_WIDGET(state->window));
}

///////////////////////////////////////////////////////////////////////////
//                    ------- MAIN LOOP -------                          //
///////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
  GTKwrapper* state = (GTKwrapper*)malloc(sizeof(GTKwrapper));
  state->app = gtk_application_new ("org.gtk.example",G_APPLICATION_FLAGS_NONE);
  g_signal_connect (state->app, "activate", G_CALLBACK (activate), state);
  g_application_run (G_APPLICATION (state->app), argc, argv);
  g_object_unref (state->app);
  return 0;
}
