/*  cssed vte terminal plugin (c) Iago Rubio 2004
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <vte/vte.h>
#include <gmodule.h>
#include <plugin.h>

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

/*
	You can use the gmodule provided functions:
	const gchar* g_module_check_init (GModule *module);
	void g_module_unload  (GModule *module);
	
	for preinitialization and post unload steps
	respectively.
*/

// module entry point. This must return a pointer to the plugin. Imported by
// cssed to load the plugin pointer.
G_MODULE_EXPORT CssedPlugin* init_plugin(void);
// cssed plugin creation function member of the plugin structure.
// Will be called by cssed through the plugin structure.
gboolean load_vte_plugin ( CssedPlugin* );
// cssed plugin cleaning function member of the plugin's structure to destroy 
// the UI. You must free all memory used here.
// Will be called by cssed through the plugin structure.
void clean_vte_plugin ( CssedPlugin* );

// One struct to save all UI to be destroyed on clean_plugin()
typedef struct _plugin_ui {
	GtkWidget* term_window;
	GtkWidget* hbox_terminal;
	GtkWidget* frame_terminal;
	GtkWidget* sb_terminal;
} PluginUI;

// the plugin structure
CssedPlugin vte_plugin;

// this will return the plugin to the caller
G_MODULE_EXPORT CssedPlugin* init_plugin()
{
	vte_plugin.name = _("Vte terminal");								// the plugin name	
	vte_plugin.description = _("Adds a console to the footer panel"); 	// the plugin description
	vte_plugin.load_plugin = &load_vte_plugin;						// load plugin function, will build the UI
	vte_plugin.clean_plugin = &clean_vte_plugin;					// clean plugin function, will destroy the UI
	vte_plugin.user_data = NULL;										// User data
	vte_plugin.priv = NULL;											// Private data, this is opaque and should be ignored		

	return &vte_plugin;
}

// util
GtkWidget*
create_single_button_from_stock(gchar *stock_id)
{
  GtkWidget* button;
  GtkWidget* image;

  button = gtk_button_new();
  image = gtk_image_new_from_stock(stock_id,GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image);
  gtk_container_add(GTK_CONTAINER(button),image);

  return button;
}

// callbacks
void
on_vte_plugin_terminal_child_exited (VteTerminal *vteterminal, gpointer user_data)
{
	gchar* shell;
	
	shell = getenv("SHELL");
	if( shell == NULL){
		vte_terminal_reset (vteterminal, TRUE, TRUE);
		vte_terminal_fork_command (vteterminal, "/bin/sh", NULL, NULL, "~/",
				   0, 0, 0);
	}else{
		if( g_file_test(shell, G_FILE_TEST_EXISTS) && g_file_test(shell,G_FILE_TEST_IS_EXECUTABLE) ){
			vte_terminal_reset (vteterminal, TRUE, TRUE);
			vte_terminal_fork_command (vteterminal, shell, NULL, NULL, "~/",
				   0, 0, 0);
		}else{
			vte_terminal_reset (vteterminal, TRUE, TRUE);
			vte_terminal_fork_command (vteterminal, "/bin/sh", NULL, NULL, "~/",
				   0, 0, 0);
		}
	}
}

//  load plugin, build the UI and attach it to cssed. Set callbacks for UI events
gboolean
load_vte_plugin (CssedPlugin* plugin)
{
	GtkWidget *term_window;
	GtkWidget* hbox_terminal;
	GtkWidget* frame_terminal;
	GtkWidget* sb_terminal;
	GdkColor terminal_fore_color;
	GdkColor terminal_back_color;
	GdkVisual* visual;
	GdkColormap* colormap;
	gchar* shell;	
	PluginUI* user_interface;
	
	gtk_set_locale ();

#ifdef ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif

	user_interface = g_malloc0(sizeof(PluginUI));
	
	visual = gdk_visual_get_system();
	colormap = gdk_colormap_new(visual, TRUE);
	gdk_color_parse ("black", &terminal_fore_color);
	gdk_color_parse ("white", &terminal_back_color);
	gdk_colormap_alloc_color(colormap, &terminal_fore_color, FALSE,TRUE);
	gdk_colormap_alloc_color(colormap, &terminal_back_color, FALSE,TRUE);

	term_window = vte_terminal_new ();
	gtk_widget_set_size_request (term_window, 50, 50);
	vte_terminal_reset (VTE_TERMINAL(term_window), TRUE, TRUE);
	vte_terminal_set_emulation (VTE_TERMINAL(term_window), "xterm");
	vte_terminal_set_mouse_autohide (VTE_TERMINAL(term_window), TRUE);
	vte_terminal_set_size (VTE_TERMINAL(term_window),50,7);// with a bigger size the app doesn't fit in 800x600
	vte_terminal_set_colors(VTE_TERMINAL(term_window), &terminal_fore_color, &terminal_back_color, NULL,0);
	vte_terminal_set_cursor_blinks (VTE_TERMINAL(term_window), TRUE);
	vte_terminal_set_scroll_on_output (VTE_TERMINAL(term_window), TRUE);
	vte_terminal_set_scroll_on_keystroke (VTE_TERMINAL(term_window), TRUE);

	shell = getenv("SHELL");
	if( shell == NULL){
		vte_terminal_fork_command (VTE_TERMINAL(term_window), "/bin/sh", NULL, NULL, "~/", 0, 0, 0);
	}else{
		if( g_file_test(shell, G_FILE_TEST_EXISTS) && g_file_test(shell,G_FILE_TEST_IS_EXECUTABLE) ){
			vte_terminal_fork_command (VTE_TERMINAL(term_window), shell, NULL, NULL, "~/", 0, 0, 0);
		}else{
			vte_terminal_fork_command (VTE_TERMINAL(term_window), "/bin/sh", NULL, NULL, "~/", 0, 0, 0);
		}
	}

	gtk_widget_show (term_window);

	sb_terminal = gtk_vscrollbar_new (GTK_ADJUSTMENT (VTE_TERMINAL (term_window)->adjustment));
	GTK_WIDGET_UNSET_FLAGS (sb_terminal, GTK_CAN_FOCUS);

	frame_terminal = gtk_frame_new (NULL);
	gtk_widget_show (frame_terminal);
	gtk_frame_set_shadow_type (GTK_FRAME (frame_terminal), GTK_SHADOW_IN);

	hbox_terminal = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_terminal), term_window, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_terminal), sb_terminal, FALSE, TRUE, 0);
	gtk_widget_show_all (hbox_terminal);
	gtk_container_add( GTK_CONTAINER( frame_terminal ), hbox_terminal);

	user_interface->term_window = term_window;
	user_interface->hbox_terminal = hbox_terminal;
	user_interface->frame_terminal = frame_terminal;
	user_interface->sb_terminal = sb_terminal;
	plugin->user_data = user_interface;

	if( cssed_plugin_add_page_with_widget_to_footer(&vte_plugin, frame_terminal, _("Vte plugin")) ){
		g_signal_connect ((gpointer) term_window, "child-exited", G_CALLBACK (on_vte_plugin_terminal_child_exited), NULL);
	}else{
		cssed_plugin_error_message(_("Cannot build plugin's controls"), _("There was an error while loading the plugin\nIt may fail at any point, please unload it"));
		return FALSE;
	}

	return TRUE;
}

/* could be used to post UI destroy
void g_module_unload (GModule *module)
{
	g_print(_("** Vte plugin unloaded\n"));	
}
*/

// to destroy UI and stuff. called by cssed.
void clean_vte_plugin ( CssedPlugin* p )
{
	PluginUI* user_interface;
	GtkWidget* button;

	user_interface = p->user_data;
	cssed_plugin_remove_page_with_widget_in_footer (&vte_plugin, user_interface->frame_terminal);
	g_free( p->user_data );
}






