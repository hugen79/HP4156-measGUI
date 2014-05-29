#include <stdio.h>
#include <gtk/gtk.h>

static void destroy(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}

int main( int  argc, char *argv[] )
{
    GtkWidget *window;
    
    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    

    /* // This creates a menu */
    /* GtkWidget *menu; */
    /* menu = gtk_menu_new(); */
   
    /* // this creates a list of options within the menu */
    /* GtkWidget* option; */
    /* option = gtk_menu_item_new_with_label("sweep"); */
    /* gtk_menu_shell_append (GTK_MENU_SHELL (menu), option); */
    /* g_signal_connect_swapped (option, "activate",G_CALLBACK (destroy), NULL); */
    /* gtk_widget_show (option); */

    /* // now we need to pack this list of options into a menu (e.g. we have */
    /* // a list inside a list. First create a menu bar */

    /* GtkWidget *menu_bar; */
    /* menu_bar = gtk_menu_bar_new(); */
    /* gtk_container_add (GTK_CONTAINER (window), menu_bar); */
    /* gtk_widget_show (menu_bar); */

    /* // Next add a "file" thing to the menu bar */
    /* GtkWidget *fileMENU; */
    /* fileMENU = gtk_menu_item_new_with_label ("File"); */
    /* gtk_widget_show (fileMENU); */
    /* gtk_menu_item_set_submenu (GTK_MENU_ITEM (fileMENU), menu); */
    /* gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), fileMENU); */


    GMenu* menu;
    menu = g_menu_new();
    g_menu_insert(menu,1,"TEST",NULL);
     
    GtkWidget* mainMENU; 
    mainMENU = gtk_menu_bar_new_from_model((GMenuModel*)menu);
    gtk_widget_show (mainMENU);
    
    gtk_widget_show_all(window);
    gtk_main ();
    
    return 0;
}
