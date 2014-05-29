#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>


#define WIDTH 800
#define HEIGHT 500

#define BWIDTH 120
#define BHEIGHT 45
#define X1 15



typedef struct{
  int NUMBER;
   // The basic window
  GtkApplication *app;

  // the main window
  GtkWidget *window;
  GtkWidget *fixed;

  // VARtester
  GtkWidget **VAR;

}GTKwrapper;


static void generateSWEEPMODE (GSimpleAction *action, GVariant *parameter, GTKwrapper state)
{ 
  /* state->VAR[0] = gtk_combo_box_text_new (); */
  /* gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[0]),NULL, "VAR1"); */
  /* gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (state->VAR[0]),NULL, "VAR2"); */
  /* gtk_combo_box_set_active (GTK_COMBO_BOX (state->VAR[0]), 0); */
  /* gtk_widget_set_size_request(state->VAR[0], BWIDTH, BHEIGHT); */
  /* gtk_fixed_put(GTK_FIXED(state->fixed), state->VAR[0], X1, 270); */
  g_print ("This generates sweepmode.\n"); 
}

static void generateSAMPLINGMODE(GSimpleAction *action, GVariant *parameter, GTKwrapper state)
{
  g_print ("This generates sampling mode.\n");
}

static void quit (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
  GApplication *application = user_data;
  g_application_quit (application);
}

static void startup (GTKwrapper* state)
{
  static const GActionEntry actions[] = {
    {"sweepmode",  generateSWEEPMODE},
    {"samplingmode", generateSAMPLINGMODE},
    {"quit", quit}
  };
 
  GMenu *menu;
  GMenu *measMENU;
 
  menu = g_menu_new ();
  measMENU = g_menu_new ();

  g_menu_append (measMENU, "Sweep", "app.sweepmode");
  g_menu_append (measMENU, "Sampling", "app.samplingmode");
  g_menu_append_submenu (menu,"Measure",G_MENU_MODEL(measMENU));
  g_menu_append (menu, "Quit", "app.quit");
 
  g_action_map_add_action_entries (G_ACTION_MAP (state->app), actions, G_N_ELEMENTS(actions),NULL);
  gtk_application_set_menubar(state->app, G_MENU_MODEL (menu));
  g_object_unref (menu);
}

static void activate (GTKwrapper* state)
{

  g_print("HELLOOOO");
  state->window = gtk_application_window_new (state->app);
  gtk_window_set_application (GTK_WINDOW (state->window), GTK_APPLICATION (state->app));
  gtk_window_set_title (GTK_WINDOW (state->window), "HP4156B Control");
  gtk_window_set_default_size(GTK_WINDOW(state->window), WIDTH, HEIGHT);
  gtk_widget_show_all (GTK_WIDGET (state->window));
  //state->fixed = gtk_fixed_new();
  //gtk_container_add(GTK_CONTAINER(state->window), state->fixed); 
}

int main (int argc, char **argv)
{
  //GTKwrapper* state = malloc(sizeof(GTKwrapper));
 
  GtkApplication *app;
  

  app = gtk_application_new("application",G_APPLICATION_FLAGS_NONE);
  //g_signal_connect (state,"startup",G_CALLBACK (startup), state);
  //g_signal_connect (state->app,"startup",G_CALLBACK (activate), NULL);
  
  //state->window = gtk_application_window_new (state->app);
  //gtk_window_set_application (GTK_WINDOW (state->window), GTK_APPLICATION (state->app));
  //gtk_window_set_title (GTK_WINDOW (state->window), "HP4156B Control");
  //gtk_window_set_default_size(GTK_WINDOW(state->window), WIDTH, HEIGHT);
  //gtk_widget_show_all (GTK_WIDGET (state->window));


  g_application_run (G_APPLICATION (app), argc, argv);
  //g_object_unref (state->app);
  return 0;
}
