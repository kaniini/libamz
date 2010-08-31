/*
 * gtkamzdl: gtk+ application for downloading amazon mp3 queues
 *
 * Copyright (c) 2010 William Pitcock <nenolod@dereferenced.org>.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include "libamz.h"

GtkWidget *window, *album, *song;
GtkWidget *albumprogress, *songprogress;

static void
handle_progress(SoupMessage *msg, AMZDownloadContext *ctx)
{
	static gint t = 0;
	gchar *progress;

	if (++t % 100 != 0 && ctx->progress > 1 && ctx->progress < 100)
		return;

	progress = g_strdup_printf("%d / %d bytes, %.2f percent complete",
				    ctx->bytes, ctx->length, ctx->progress);

	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(songprogress), ctx->progress / 100.);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(songprogress), progress);
}

gchar *
build_download_path(AMZPlaylistEntry *entry)
{
	gchar *ret, *dir, *filename;

	filename = g_strdup_printf("%02d - %s.mp3", entry->tracknum, entry->title);
	dir = g_build_filename(g_get_home_dir(), "Music", entry->creator, entry->album, NULL);
	ret = g_build_filename(dir, filename, NULL);

	g_mkdir_with_parents(dir, 0755);

	g_free(dir);

	return ret;
}

void
handle_amz_file(SoupSession *session, const gchar *file)
{
	GList *list, *node;
	guchar *data;
	gsize len, tracks;
	gint i = 1;

	g_return_if_fail(file != NULL);

	amzfile_decrypt_file(file, &data, &len);
	list = amzplaylist_parse(data);
	if (list == NULL)
	{
		fprintf(stderr, "failed to parse xspf file embedded in %s\n", file);
		exit(EXIT_FAILURE);
	}

	{
		gchar *albumtext, *creator, *markup;
		AMZPlaylistEntry *entry = list->data;

		albumtext = g_markup_escape_text(entry->album, -1);
		creator = g_markup_escape_text(entry->creator, -1);
		markup = g_strdup_printf("<big>Downloading <b>%s</b> by <b>%s</b>.</big>", albumtext, creator);
		gtk_label_set_markup(GTK_LABEL(album), markup);

		g_free(albumtext);
		g_free(creator);
		g_free(markup);

		markup = g_strdup_printf("gtkamzdl: %s by %s", entry->album, entry->creator);
		gtk_window_set_title(GTK_WINDOW(window), markup);
		g_free(markup);
	}

	tracks = g_list_length(list);
	for (node = list; node != NULL; node = node->next)
	{
		gchar *title, *markup, *status;
		gchar *path;
		AMZPlaylistEntry *entry = node->data;

		path = build_download_path(entry);

		title = g_markup_escape_text(entry->title, -1);
		markup = g_strdup_printf("<b>%s</b>", title);

		gtk_label_set_markup(GTK_LABEL(song), title);

		g_free(title);
		g_free(markup);

		status = g_strdup_printf("%d / %lu tracks", i, tracks);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(albumprogress), ((float) i / (float) tracks));
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(albumprogress), status);
		g_free(status);

		amzdownload_session_download_url(session, entry->location, path, handle_progress);

		g_free(path);

		i++;
	}
}

void
build_window(void)
{
	GtkWidget *vbox;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "gtkamzdl");

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	album = gtk_label_new("album");
	gtk_box_pack_start(GTK_BOX(vbox), album, TRUE, TRUE, 0);

	song = gtk_label_new("song");
	gtk_box_pack_start(GTK_BOX(vbox), song, TRUE, TRUE, 0);

	albumprogress = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), albumprogress, TRUE, TRUE, 0);

	songprogress = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), songprogress, TRUE, TRUE, 0);

	gtk_widget_show_all(window);
}

int
main(gint argc, gchar *argv[])
{
	SoupSession *session;
	gint i;

	gtk_init(&argc, &argv);

	session = amzdownload_session_new();

	if (argc < 2)
	{
		fprintf(stderr, "usage: %s file.amz\n", argv[0]);
		return EXIT_FAILURE;
	}

	build_window();
	for (i = 1; i < argc; i++)
		handle_amz_file(session, argv[i]);

	return EXIT_SUCCESS;
}
