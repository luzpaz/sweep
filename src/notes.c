/*
 * Sweep, a sound wave editor.
 *
 * Copyright (C) 2000 Conrad Parker
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "callbacks.h"

#include <sweep/sweep_i18n.h>

#include <sweep/sweep_types.h>
#include <sweep/sweep_undo.h>
#include <sweep/sweep_sample.h>

#include "head.h"
#include "interface.h"
#include "edit.h"
#include "sample-display.h"
#include "play.h"

#include "notes.h"

typedef struct _sw_noteplay sw_noteplay;

struct _sw_noteplay {
  char name [5];
  float pitch;
  guint	 accel_key;
};

static void play_view_note_cb (GtkWidget * widget, gpointer data);

static sw_noteplay notes [] =
{
  { N_("C3") , 0.500000, GDK_z },
  { N_("C#3"), 0.529732, GDK_s },
  { N_("D3") , 0.561231, GDK_x },
  { N_("Eb3"), 0.594604, GDK_d },
  { N_("E3") , 0.629961, GDK_c },
  { N_("F3") , 0.667420, GDK_v },
  { N_("F#3"), 0.707107, GDK_g },
  { N_("G3") , 0.749154, GDK_b },
  { N_("G#3"), 0.793701, GDK_h },
  { N_("A3") , 0.793701, GDK_n },
  { N_("Bb3"), 0.890899, GDK_j },
  { N_("B3") , 0.943874, GDK_m },

  { N_("C4") , 1.000000, GDK_q },

  { N_("C#4"), 1.059463, GDK_2 },
  { N_("D4") , 1.122462, GDK_w },
  { N_("Eb4"), 1.189207, GDK_3 },
  { N_("E4") , 1.259921, GDK_e },
  { N_("F4") , 1.334840, GDK_r },
  { N_("F#4"), 1.414214, GDK_5 },
  { N_("G4") , 1.498307, GDK_t },
  { N_("G#4"), 1.587401, GDK_6 },
  { N_("A4") , 1.681793, GDK_y },
  { N_("Bb4"), 1.781797, GDK_7 },
  { N_("B4") , 1.887749, GDK_u },

  { N_("C5") , 2.000000, GDK_i },
  { N_("C#5"), 2.118926, GDK_9 },
  { N_("D5") , 2.244924, GDK_o },
  { N_("D#5"), 2.378414, GDK_0 },
  { N_("E5") , 2.519842, GDK_p },
#if 0
  { N_("F5") , 2.669680, GDK_bracketleft },
  { N_("F#5"), 2.828427, GDK_None },
  { N_("G5") , 2.996614, GDK_None },
  { N_("G#5"), 3.174802, GDK_None },
  { N_("A5") , 3.363586, GDK_None },
  { N_("Bb5"), 3.563595, GDK_None },
  { N_("B5") , 3.775497, GDK_None },
#endif

} ;


void
noteplay_setup (GtkWidget *subsubmenu, sw_view * view,
		GtkAccelGroup *accel_group)
{
  GtkWidget *menuitem;
  int k;

  for (k = 0 ; k < sizeof (notes) / sizeof (notes [0]) ; k++) {
    menuitem = gtk_menu_item_new_with_label (_(notes [k].name));
    gtk_menu_append (GTK_MENU(subsubmenu), menuitem);
    g_signal_connect (G_OBJECT(menuitem), "activate",
			G_CALLBACK(play_view_note_cb), view);
    gtk_widget_show (menuitem);
    gtk_widget_add_accelerator (menuitem, "activate", accel_group,
notes [k].accel_key, 0,
				GTK_ACCEL_VISIBLE);

    g_object_set_data (G_OBJECT(menuitem), "default", 
			      GINT_TO_POINTER(k));
  }

  return;
}

static void
play_view_note_cb (GtkWidget * widget, gpointer data)
{
  sw_view * view = (sw_view*) data;
  sw_head * head = view->sample->play_head;
  sw_framecount_t mouse_offset;
  int k;
  float pitch;
  
  /* Retrieve the pitch. */
  k = GPOINTER_TO_INT(g_object_get_data (G_OBJECT(widget), "default"));
  pitch = notes[k].pitch;

  if (head->going) {
    head_set_rate (head, pitch);
    mouse_offset =
      sample_display_get_mouse_offset (SAMPLE_DISPLAY(view->display));
    head_set_offset (head, mouse_offset);
  } else {
    play_view_all_pitch (view, pitch);
  }

}

