/** ***********************************************************************************
  *                                                                                   *
  * mmp (Micro Music Player) a very simple play a folder or files music player.       *
  * Copyright (C) 2015, 2016  Brüggemann Eddie.                                       *
  *                                                                                   *
  * This file is part of mmp (Micro Music Player).                                    *
  * Micro Music Player is free software: you can redistribute it and/or modify        *
  * it under the terms of the GNU General Public License as published by              *
  * the Free Software Foundation, either version 3 of the License, or                 *
  * (at your option) any later version.                                               *
  *                                                                                   *
  * mmp (Micro Music Player) is distributed in the hope that it will be useful,       *
  * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                      *
  * GNU General Public License for more details.                                      *
  *                                                                                   *
  * You should have received a copy of the GNU General Public License                 *
  * along with mmp (Micro Music Player). If not, see <http://www.gnu.org/licenses/>   *
  *                                                                                   *
  ************************************************************************************/

#include "includes.h"

#include "structures.h"

#include "global_vars.h"

#include "defines.h"

#include "Resizer.h"

#include "global_vars.h"

#include "dialogs.h"

#include "folder_parsing.h"

#include "generate_menu.h"

#include "music_hooks.h"

#include "playing_controls_callback.h"

/** Micro Music Player -> A very simple music player. A select folder or files music player.
  *
  * The program is written in C++ but in a C-Like style (Procedural) not Object Oriented with severals global variables,
  *
  * because the program was originally written in C and extended to C++ for a better container support.
  *
  */

static gboolean timeline_change_value(GtkRange     *range, GtkScrollType scroll, gdouble       value, gpointer      user_data) {



  if (libvlc_media_player_is_playing(media_player)) {

    if ( (value <= 0.0) || (value >= 1.0)) {

      return TRUE ;

    }

    libvlc_media_player_set_position(media_player, static_cast<float>(value)) ;

  }

  return TRUE ;

}

static gchar* timeline_format_value(GtkScale *scale, gdouble   value, gpointer  user_data) {

  return g_strdup_printf("%02li:%02li", static_cast<int64_t>(value *  factor) / 60, static_cast<int64_t>(value *  factor) % 60);
                  
}

// Quit application.
static gboolean delete_event(GtkWidget *widget,GdkEvent *event,gpointer pointer) ;

int main(int argc, char *argv[]) {

  if ( realpath(PATH_TO_CONF_FILE, NULL) == NULL ) {

    GKeyFile *conf_file = g_key_file_new() ;

    // Set configuration settings to configuration file buffer.
    g_key_file_set_string(conf_file,   "Config",  "Buttons_Icons_Path",   PATH_TO_OXYGEN_BUTTON_ICONS) ;
    g_key_file_set_string(conf_file,   "Config",  "Menu_Icons_Path",      PATH_TO_OXYGEN_MENU_ICONS)   ;

    const char *user_music_folder = g_get_user_special_dir(G_USER_DIRECTORY_MUSIC) ;

    g_key_file_set_string(conf_file,   "Config",  "Music_Folder",         (user_music_folder != NULL) ? g_strdup(user_music_folder) : g_strdup(g_get_home_dir()) )   ;

    g_key_file_set_boolean(conf_file,  "Config",  "Repeat_all",           TRUE)  ;
    g_key_file_set_boolean(conf_file,  "Config",  "Shuffle",              FALSE) ;

    g_key_file_set_double(conf_file,   "Config",  "Volume",               50.0) ;

    g_key_file_set_uint64(conf_file,   "Config",  "Buttons_space",        SPACE_BUTTONS_LITTLE) ;

    g_key_file_set_boolean(conf_file,  "Config",  "Is_Oxygen",            TRUE) ;

    g_key_file_set_uint64(conf_file,   "Config",  "Display_Size",         DISPLAY_SIZE_LITTLE) ;

    g_key_file_set_uint64(conf_file,   "Config",  "Image_Resizing",       IMAGE_RESIZED_SIZE_LITTLE) ;

    g_key_file_set_string(conf_file,   "Config",  "Sized_Default_Image",  PATH_TO_IMAGE "Micro_Music_Player_256.png")   ;

    // Write to configuration file:
    g_key_file_save_to_file(conf_file, PATH_TO_CONF_FILE, NULL);

  }


  PRG_START_MSG  // Display Micro Music Player start message.


  /** ***** [START] loading settings from configuration file or default. [START] ***** **/

  bool is_config_loaded = false ;

  GKeyFile *conf_file = g_key_file_new() ;

  GError *error = NULL ;

  g_key_file_load_from_file(conf_file, PATH_TO_CONF_FILE, G_KEY_FILE_NONE, &error) ;

  if (error != NULL) {

     /** Error by loading configuration file: **/

     /** Reset settings to default. **/

     rewrite_default_to_conf_file :

     // Default icons set.
     settings.path_to_button_icons =  PATH_TO_OXYGEN_BUTTON_ICONS ;
     settings.path_to_menu_icons   =  PATH_TO_OXYGEN_MENU_ICONS  ;
     //settings.path_to_button_icons =  PATH_TO_HIGH_CONTRAST_BUTTON_ICONS ;
     //settings.path_to_menu_icons   =  PATH_TO_HIGH_CONTRAST_MENU_ICONS  ;

     const char *user_music_folder = g_get_user_special_dir(G_USER_DIRECTORY_MUSIC) ;

     // Default folder path.
     settings.path_to_music_folder =  (user_music_folder != NULL) ? g_strdup(user_music_folder) : g_strdup(g_get_home_dir()) ;
                                          

     // Boolean configuration settings.
     settings.icon_set_oxygen      = TRUE     ;
     settings.is_repeat_all        = TRUE     ;
     settings.is_shuffle           = FALSE    ;


     // Default volume.
     settings.volume               = 50.0     ;


     // Start configuration little.
     settings.space_buttons       = SPACE_BUTTONS_LITTLE      ;
     settings.display_size        = DISPLAY_SIZE_LITTLE       ;
     settings.image_resized_size  = IMAGE_RESIZED_SIZE_LITTLE ;


     // Start sized default image.
     settings.path_to_default_image =  PATH_TO_IMAGE "Micro_Music_Player_256.png" ;


     // Setting global variables.
     cover_image = settings.path_to_default_image ;
     current_folder = settings.path_to_music_folder ;


     // Set configuration settings to configuration file buffer.
     g_key_file_set_string(conf_file,   "Config",  "Buttons_Icons_Path",   PATH_TO_OXYGEN_BUTTON_ICONS) ;
     g_key_file_set_string(conf_file,   "Config",  "Menu_Icons_Path",      PATH_TO_OXYGEN_MENU_ICONS)   ;

     g_key_file_set_string(conf_file,   "Config",  "Music_Folder",         g_get_home_dir())   ;

     g_key_file_set_boolean(conf_file,  "Config",  "Repeat_all",           TRUE)  ;
     g_key_file_set_boolean(conf_file,  "Config",  "Shuffle",              FALSE) ;

     g_key_file_set_double(conf_file,   "Config",  "Volume",               50.0) ;

     g_key_file_set_uint64(conf_file,   "Config",  "Buttons_space",        SPACE_BUTTONS_LITTLE) ;

     g_key_file_set_boolean(conf_file,  "Config",  "Is_Oxygen",            TRUE) ;

     g_key_file_set_uint64(conf_file,   "Config",  "Display_Size",         DISPLAY_SIZE_LITTLE) ;

     g_key_file_set_uint64(conf_file,   "Config",  "Image_Resizing",       IMAGE_RESIZED_SIZE_LITTLE) ;

     g_key_file_set_string(conf_file,   "Config",  "Sized_Default_Image",   PATH_TO_IMAGE "Micro_Music_Player_256.png")   ;

     // Write to configuration file:
     g_key_file_save_to_file(conf_file, PATH_TO_CONF_FILE, &error);

   }
   else {

     // Getting configuration settings:
     settings.path_to_button_icons  = g_key_file_get_string(conf_file,  "Config",  "Buttons_Icons_Path",   NULL) ;
     settings.path_to_menu_icons    = g_key_file_get_string(conf_file,  "Config",  "Menu_Icons_Path",      NULL) ;

     settings.path_to_music_folder  = g_key_file_get_string(conf_file,  "Config",  "Music_Folder",         NULL) ;

     settings.icon_set_oxygen       = g_key_file_get_boolean(conf_file, "Config",  "Is_Oxygen",            NULL) ;

     settings.is_repeat_all         = g_key_file_get_boolean(conf_file, "Config",  "Repeat_all",           NULL) ;
     settings.is_shuffle            = g_key_file_get_boolean(conf_file, "Config",  "Shuffle",              NULL) ;

     settings.volume                = g_key_file_get_double(conf_file,  "Config",   "Volume",              NULL) ;

     // Start configuration little.
     settings.space_buttons         = g_key_file_get_int64(conf_file,   "Config",   "Buttons_space",       NULL) ;
     settings.display_size          = g_key_file_get_int64(conf_file,   "Config",   "Display_Size",        NULL) ;
     settings.image_resized_size    = g_key_file_get_int64(conf_file,   "Config",   "Image_Resizing",      NULL) ;

     // Start sized default image.
     settings.path_to_default_image = g_key_file_get_string(conf_file,  "Config",   "Sized_Default_Image", NULL) ;

     if (realpath(settings.path_to_button_icons.c_str(),    NULL) == NULL) { goto rewrite_default_to_conf_file ; }

     if (realpath(settings.path_to_menu_icons .c_str(),     NULL) == NULL) { goto rewrite_default_to_conf_file ; }

     if (realpath(settings.path_to_music_folder.c_str(),    NULL) == NULL) { goto rewrite_default_to_conf_file ; }

     if (realpath(settings.path_to_default_image.c_str(),   NULL) == NULL) { goto rewrite_default_to_conf_file ; }

     // Setting global variables.
     cover_image = settings.path_to_default_image ;
     current_folder = settings.path_to_music_folder ;

     is_config_loaded = true ;

  }

  /** ***** [END] Loading settings from configuration file or default. [END] ***** **/



  /** [START] command line argument parsing [START] **/

  bool is_arg_dir = true ;

  char *path = NULL ;

  if (argc == 1) {

    gchar *prgname = g_path_get_basename(argv[0]) ;

    fprintf(stdout,"\nusage cmdline: %s [dir|files]path\n",  prgname) ;

    g_free(prgname) ;

  }
  if (argc > 1) {

    // First argument should be a valid folderpath or a valid filepath.

    path = realpath(argv[1], NULL) ;

    if (path == NULL) {
      fprintf(stderr,"Given argument(s) are not a valid filepath(s) or a directory path:\n%s\n", argv[1]) ;
      exit(EXIT_FAILURE) ;
    }

    // Checking if the given argument(s) are a directory or a file(s) path.
    if (g_file_test(path, G_FILE_TEST_IS_DIR) ) {
      is_arg_dir = true ;
    }
    else if (g_file_test(path, G_FILE_TEST_IS_REGULAR) ) {
      is_arg_dir = false ;
    }



  }

  /** [END] command line argument parsing [END] **/


  //setup vlc
  vlc_inst = libvlc_new(0, NULL);
  media_player = libvlc_media_player_new(vlc_inst);

  libvlc_event_manager_t *media_event_manager = libvlc_media_player_event_manager(media_player) ;


  // Maybe libvlc_MediaParsedChanged event to add ???
  libvlc_event_attach(media_event_manager, libvlc_MediaPlayerEndReached, &music_finished_hook, NULL) ;



  /** Start settings **/
  set_pause(false)   ;
  set_playing(false) ;
  set_stop(true)     ;
  set_order(false)   ;

  set_repeat(settings.is_repeat_all)   ;
  set_shuffle(settings.is_shuffle)     ;
  /** ************** **/


  /** ***** [START] Main GUI generating, configurating and connect callbacks [START] ***** **/

  gtk_init(&argc, &argv) ;


  gui->window=gtk_window_new(GTK_WINDOW_TOPLEVEL) ;

  gtk_window_set_title(GTK_WINDOW(gui->window), prgname.c_str()) ;
  gtk_window_set_position(GTK_WINDOW(gui->window), GTK_WIN_POS_CENTER_ALWAYS) ;
  gtk_window_set_resizable(GTK_WINDOW(gui->window), FALSE);
  gtk_window_set_default_icon_from_file(PATH_TO_PRG_WIN_ICON, NULL) ;

  gtk_widget_set_size_request(gui->window,-1,-1) ;



  gui->main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0) ;

  gui->controls_buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0) ;

  gtk_box_set_homogeneous(GTK_BOX(gui->controls_buttonbox), TRUE) ;


  Menu pmenu ;

  gui->menu = &pmenu ;

  Playlist pplaylist ;

  gui->playlist = &pplaylist ;

  // Construct the menu of the program.
  initialize_menu(gui->menu) ;

  // Controls:
  gui->button_prev    = gtk_button_new() ;
  gui->button_play    = gtk_button_new() ;
  gui->button_pause   = gtk_button_new() ;
  gui->button_next    = gtk_button_new() ;
  gui->button_stop    = gtk_button_new() ;
  // Play modes:
  gui->button_repeat  = gtk_toggle_button_new() ;
  gui->button_shuffle = gtk_toggle_button_new() ;
  // Settings:
  gui->button_volume  = gtk_volume_button_new() ;


  gtk_button_set_relief(GTK_BUTTON(gui->button_volume), GTK_RELIEF_NORMAL) ;
  gtk_scale_button_set_value(GTK_SCALE_BUTTON(gui->button_volume), 1.0 / 100.0 * settings.volume ) ;


  // Controls:
  gui->button_prev_image       = gtk_image_new_from_file((settings.path_to_button_icons +  "media-skip-backward.png").c_str())    ;
  gui->button_play_image       = gtk_image_new_from_file((settings.path_to_button_icons +  "media-playback-start.png").c_str())   ;
  gui->button_pause_image      = gtk_image_new_from_file((settings.path_to_button_icons +  "media-playback-pause.png").c_str())   ;
  gui->button_next_image       = gtk_image_new_from_file((settings.path_to_button_icons +  "media-skip-forward.png").c_str())     ;
  gui->button_stop_image       = gtk_image_new_from_file((settings.path_to_button_icons +  "media-playback-stop.png").c_str())    ;
  // Play modes:
  gui->button_repeat_image     = gtk_image_new_from_file((settings.path_to_button_icons +  "media-playlist-repeat.png").c_str())  ;
  gui->button_shuffle_image    = gtk_image_new_from_file((settings.path_to_button_icons +  "media-playlist-shuffle.png").c_str()) ;
  // Settings:
  gui->button_volume_image     = gtk_image_new_from_file((settings.path_to_button_icons +  "audio-volume-medium.png").c_str())    ;

  //Configure volume button plus & minus button:
  gui->button_volume_plus  = gtk_scale_button_get_plus_button(GTK_SCALE_BUTTON(gui->button_volume))  ;
  gui->button_volume_minus = gtk_scale_button_get_minus_button(GTK_SCALE_BUTTON(gui->button_volume)) ;
  gui->button_volume_plus_image  = gtk_image_new_from_file((settings.path_to_menu_icons +  "audio-volume-high.png").c_str()) ;
  gui->button_volume_minus_image = gtk_image_new_from_file((settings.path_to_menu_icons +  "audio-volume-low.png").c_str())  ;
  gtk_button_set_image(GTK_BUTTON(gui->button_volume_plus),  gui->button_volume_plus_image)   ;
  gtk_button_set_image(GTK_BUTTON(gui->button_volume_minus), gui->button_volume_minus_image)  ;

  // Set tooltips:
  gtk_widget_set_tooltip_markup(gui->button_prev,  "<b>Play</b> the <b>previous song</b> in the playlist [ <b>Ctrl + B</b> ]")          ;
  gtk_widget_set_tooltip_markup(gui->button_play,  "<b>Pause</b> the <b>current song</b> playing in the playlist [ <b>Ctrl + P</b> ]")  ;
  gtk_widget_set_tooltip_markup(gui->button_pause, "<b>Resume</b> the <b>current song</b> playing in the playlist [ <b>Ctrl + P</b> ]") ;
  gtk_widget_set_tooltip_markup(gui->button_next,  "<b>Play</b> the <b>next song</b> in the playlist [ <b>Ctrl + N</b> ]")              ;
  gtk_widget_set_tooltip_markup(gui->button_stop,  "<b>Stop</b> the playlist playing [ <b>Ctrl + S</b> ]")                              ;

  gtk_widget_set_tooltip_markup(gui->button_repeat,  "(Enable | Disable) <b>Repeat</b> playlist [ <b>Ctrl + Maj + R</b> ]")             ;
  gtk_widget_set_tooltip_markup(gui->button_shuffle, "(Enable | Disable) <b>shuffle</b> playing mode [ <b>Ctrl + Maj + S</b> ]")        ;

  // Set images on buttons:
  gtk_button_set_image(GTK_BUTTON(gui->button_prev),    gui->button_prev_image)    ;
  gtk_button_set_image(GTK_BUTTON(gui->button_play),    gui->button_play_image)    ;
  gtk_button_set_image(GTK_BUTTON(gui->button_pause),   gui->button_pause_image)   ;
  gtk_button_set_image(GTK_BUTTON(gui->button_next),    gui->button_next_image)    ;
  gtk_button_set_image(GTK_BUTTON(gui->button_stop),    gui->button_stop_image)    ;
  gtk_button_set_image(GTK_BUTTON(gui->button_repeat),  gui->button_repeat_image)  ;
  gtk_button_set_image(GTK_BUTTON(gui->button_shuffle), gui->button_shuffle_image) ;
  gtk_button_set_image(GTK_BUTTON(gui->button_volume),  gui->button_volume_image)  ;

  #ifdef BUTTON_SIZE
  // Set buttons size.
  gtk_widget_set_size_request(gui->button_prev,    BUTTON_SIZE, BUTTON_SIZE) ;
  gtk_widget_set_size_request(gui->button_play,    BUTTON_SIZE, BUTTON_SIZE) ;
  gtk_widget_set_size_request(gui->button_pause,   BUTTON_SIZE, BUTTON_SIZE) ;
  gtk_widget_set_size_request(gui->button_next,    BUTTON_SIZE, BUTTON_SIZE) ;
  gtk_widget_set_size_request(gui->button_stop,    BUTTON_SIZE, BUTTON_SIZE) ;
  gtk_widget_set_size_request(gui->button_repeat,  BUTTON_SIZE, BUTTON_SIZE) ;
  gtk_widget_set_size_request(gui->button_volume,  BUTTON_SIZE, BUTTON_SIZE) ;
  #endif

  // Connecting buttons callbacks:
  g_signal_connect(G_OBJECT(gui->button_play),    "clicked",        G_CALLBACK(play_music),         NULL) ;
  g_signal_connect(G_OBJECT(gui->button_pause),   "clicked",        G_CALLBACK(pause_music),        NULL) ;
  g_signal_connect(G_OBJECT(gui->button_prev),    "clicked",        G_CALLBACK(prev_music),         NULL) ;
  g_signal_connect(G_OBJECT(gui->button_next),    "clicked",        G_CALLBACK(next_music),         NULL) ;
  g_signal_connect(G_OBJECT(gui->button_stop),    "clicked",        G_CALLBACK(stop_music),         NULL) ;
  g_signal_connect(G_OBJECT(gui->button_repeat),  "toggled",        G_CALLBACK(repeat_all_music),   NULL) ;
  g_signal_connect(G_OBJECT(gui->button_shuffle), "toggled",        G_CALLBACK(shuffle_music),      NULL) ;
  g_signal_connect(G_OBJECT(gui->button_volume),  "value-changed",  G_CALLBACK(change_volume),      NULL) ;

  gtk_box_pack_start(GTK_BOX(gui->controls_buttonbox), gui->button_prev,    TRUE, TRUE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->controls_buttonbox), gui->button_play,    TRUE, TRUE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->controls_buttonbox), gui->button_pause,   TRUE, TRUE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->controls_buttonbox), gui->button_next,    TRUE, TRUE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->controls_buttonbox), gui->button_stop,    TRUE, TRUE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->controls_buttonbox), gui->button_repeat,  TRUE, TRUE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->controls_buttonbox), gui->button_shuffle, TRUE, TRUE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->controls_buttonbox), gui->button_volume,  TRUE, TRUE, 0) ;




  gtk_box_set_spacing(GTK_BOX(gui->controls_buttonbox), settings.space_buttons) ;

  // Activate or not menu checkboxes according user configuration:
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gui->menu->button_repeat->menuitem), is_repeat_all) ;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->button_repeat), is_repeat_all) ;
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gui->menu->button_shuffle->menuitem), is_shuffle) ;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->button_shuffle), is_shuffle) ;


  // Configure current song displaying display.
  gui->song_name_entry = gtk_entry_new() ;
  gtk_entry_set_alignment(GTK_ENTRY(gui->song_name_entry), 0.5);
  g_object_set(G_OBJECT(gui->song_name_entry), "editable", FALSE, NULL) ;
  gtk_widget_set_can_focus(gui->song_name_entry, TRUE);


  // Configure folder chooser button.
  gui->folder_chooser_button = gtk_button_new_with_label("Select a folder to play content") ;
  gtk_widget_set_tooltip_markup(gui->folder_chooser_button,  "Select the folder to play music files from.")      ;
  gtk_widget_set_hexpand(gui->folder_chooser_button, FALSE) ;
  g_signal_connect(G_OBJECT(gui->folder_chooser_button), "clicked", G_CALLBACK(get_folder_to_play), NULL) ;


  // Configure cover image container.
  gui->image_container  = gtk_fixed_new() ;
  gui->image_widget = gtk_image_new_from_file(settings.path_to_default_image.c_str()) ;
  gtk_fixed_put(GTK_FIXED(gui->image_container), gui->image_widget, (settings.display_size - settings.image_resized_size)  / 2 , (settings.display_size - settings.image_resized_size) / 2 ) ;
  gtk_widget_set_size_request(gui->image_container, settings.display_size, settings.display_size ) ;


  GtkWidget *label_margin_left  = gtk_label_new("") ;
  GtkWidget *label_margin_right = gtk_label_new("") ;

  gui->image_container_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0) ;

  gui->timeline_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 1.0, 0.01)  ;

  gtk_scale_set_draw_value(GTK_SCALE(gui->timeline_scale), TRUE) ;
  gtk_scale_set_has_origin(GTK_SCALE(gui->timeline_scale), TRUE) ;

  gtk_range_set_slider_size_fixed(GTK_RANGE(gui->timeline_scale), TRUE) ;

  gtk_scale_set_value_pos(GTK_SCALE(gui->timeline_scale), GTK_POS_RIGHT);

  gtk_scale_add_mark(GTK_SCALE(gui->timeline_scale), 0.0, GTK_POS_TOP, NULL);

  g_object_set(gui->timeline_scale, "lower-stepper-sensitivity", GTK_SENSITIVITY_OFF, NULL) ;
  g_object_set(gui->timeline_scale, "upper-stepper-sensitivity", GTK_SENSITIVITY_OFF, NULL) ;

  g_signal_connect(G_OBJECT(gui->timeline_scale), "change-value", G_CALLBACK(timeline_change_value), NULL) ;
  g_signal_connect(G_OBJECT(gui->timeline_scale), "format-value", G_CALLBACK(timeline_format_value), NULL) ;

  gtk_box_pack_start(GTK_BOX(gui->image_container_hbox), label_margin_left,     TRUE,  TRUE,  0) ;
  gtk_box_pack_start(GTK_BOX(gui->image_container_hbox), gui->image_container,  FALSE, FALSE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->image_container_hbox), label_margin_right,    TRUE,  TRUE,  0) ;

  gtk_box_pack_start(GTK_BOX(gui->main_vbox), gui->menu->menu_bar,              FALSE, FALSE, 5) ;
  gtk_box_pack_start(GTK_BOX(gui->main_vbox), gui->controls_buttonbox,          FALSE, FALSE, 5) ;
  gtk_box_pack_start(GTK_BOX(gui->main_vbox), gui->image_container_hbox,        FALSE, FALSE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->main_vbox), gui->timeline_scale,              FALSE, FALSE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->main_vbox), gui->song_name_entry,             FALSE, FALSE, 0) ;
  gtk_box_pack_start(GTK_BOX(gui->main_vbox), gui->folder_chooser_button,       FALSE, FALSE, 5) ;


  gtk_window_add_accel_group(GTK_WINDOW(gui->window), GTK_ACCEL_GROUP(gui->menu->accel_group) );

  gtk_container_add(GTK_CONTAINER(gui->window), gui->main_vbox) ;

  gtk_container_set_border_width(GTK_CONTAINER(gui->window), 16) ;

  g_signal_connect(G_OBJECT(gui->window),"destroy",G_CALLBACK(destroy),NULL) ;

  g_signal_connect(G_OBJECT(gui->window),"delete-event",G_CALLBACK(delete_event),NULL) ;

  /** ***** [END] Main GUI generating, configurating and connect callbacks [END ***** **/

  // stdout output:
  if (argc > 1) {

    #ifdef INFO
    #if (INFO >= 1)
    if (is_arg_dir) {

      gchar *dirname = g_path_get_basename(path) ;

      fprintf(stdout,"\n%s: Read music files from directory:\n%s\n", prgname.c_str(), dirname) ;

      g_free(dirname) ;

    }
    else {
      fprintf(stdout,"\n%s: Read music files...", prgname.c_str()) ;
    }
    #endif
    #endif

  }


  // Print out others infos about configuration.
  if (is_config_loaded) {
  #if (INFO >= 2)
  fprintf(stdout, "\n%s: Settings successfull from configuration file loaded.\n", prgname.c_str() ) ;
  fprintf(stdout,"\n%s: current configuration:\n\n%s", prgname.c_str(),   g_key_file_to_data(conf_file, NULL, NULL) ) ;
  #endif
  }
  else {
  #if (INFO >= 2)
  fprintf(stdout, "\n%s: Error by loading configuration file\nReset default settings...\n", prgname.c_str() ) ;
  #endif
  }
  #if (INFO >= 1)
  fprintf(stdout,"\n%s: Volume set at start to value: %.0lf\n", prgname.c_str(), settings.volume) ; fflush(stdout) ;
  #endif


  gtk_widget_show_all(gui->window) ;

  gtk_widget_hide(gui->button_pause) ;

  gtk_window_set_focus_on_map(GTK_WINDOW(gui->window), TRUE) ;

  // Launching application lifetime timeouts.
  g_timeout_add(75, (GSourceFunc) timeout_update,   NULL) ;
  g_timeout_add(75, (GSourceFunc) timeout_timeline, NULL) ;

  // Setting volume according user configuration.
  libvlc_audio_set_volume(media_player, static_cast<int>(settings.volume)) ;


  /** Checking given argumenty on the cmdline. **/
  if (argc > 1) {

    if (is_arg_dir) {

      if (! parse_folder(path) ) {


        #ifdef INFO
        #if (INFO >= 1)
        fprintf(stderr,"%s: Failed to parse folder\n -> %s\nHave not detected any playable compatible music file !!!\n\nSupported filetypes:\n", prgname.c_str(), path) ;
        for (int c=0 ; c < static_cast<int>(extension_list.size()) ; c++) {

          if (c < static_cast<int>(extension_list.size()) - 1) {
            fprintf(stderr,"%s, ",extension_list.at(c).c_str()) ;
          }
          else {
            fprintf(stderr,"%s.",extension_list.at(c).c_str()) ;
          }
        }
        #endif
        #endif
  
        free(path) ;
  
        exit(EXIT_FAILURE) ;

      }
    }
    else {

      // Setting control variables:
      music_song_counter = 0 ;

      int c = 1 ;

      while (c < argc) {
 
        char *filepath = NULL ;
  
        static bool dirpath_found = false ;
  
        if ( (filepath = realpath(argv[c++], NULL)) != NULL) {
   
          check_file_selection(filepath) ;
      
          if (! dirpath_found) {
    
            gchar *dirname = g_path_get_dirname(filepath) ;
   
            current_folder = dirname ;
   
            g_free(dirname) ;
      
            dirpath_found = true ;
    
          }
   
        }

      }


      if (imaging_cover_container.size() == 0) {
  
        search_and_set_cover_image(current_folder.c_str()) ;
      }
 
      // Shrink containers size.
      playlist_shuffle_indexes.shrink_to_fit() ;
      playlist_shuffle_back_up.shrink_to_fit() ;
      folder_music_playlist_container.shrink_to_fit() ;

      imaging_cover_container.shrink_to_fit() ;
      title_author_container.shrink_to_fit()  ;
      album_names_container.shrink_to_fit()   ;

      // Reset controls variables:
      total_song_counter = music_song_counter ;
      music_song_counter = 0 ;

      // Shuffle feature support.
      playlist_shuffle_back_up = playlist_shuffle_indexes ;

      /*
      // Reset some control variables.
      set_pause(false) ;
      set_playing(false) ;
      set_stop(false) ;
      */

      // Show play button and hide pause because they take the same place in the GUI.
      gtk_widget_hide(gui->button_pause) ;
      gtk_widget_show(gui->button_play)  ;

      at_start = true ;
 
      // Start playing music.
      play_music(NULL, NULL) ;

    }

  }


  // Enter GUI mainloop.
  gtk_main() ;

  /** Free libvlc pointers **/
  libvlc_media_player_release(media_player) ;
  libvlc_release(vlc_inst) ;

  exit(EXIT_SUCCESS) ;

}


void destroy(GtkWidget *widget, gpointer pointer) {

  stop_music(NULL, NULL) ;

  gtk_main_quit() ;

}

static gboolean delete_event(GtkWidget *widget,GdkEvent *event,gpointer pointer) {

  if (event->type == GDK_DELETE) {

    stop_music(NULL, NULL) ;

    #ifdef DEBUG
    fprintf(stdout,"%s delete_event catch\n",     __func__) ;
    #endif

    gtk_main_quit() ;

    return TRUE ;
  }
  else {

    #ifdef DEBUG
    fprintf(stdout,"%s delete_event not catch\n", __func__) ;
    #endif

    return FALSE ;
  }
}