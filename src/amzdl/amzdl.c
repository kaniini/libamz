/*
 * amzls: decrypt and output the contents of an AMZ file.
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

#include "libamz.h"

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
	gsize len;

	g_return_if_fail(file != NULL);

	amzfile_decrypt_file(file, &data, &len);
	list = amzplaylist_parse(data);
	if (list == NULL)
	{
		fprintf(stderr, "failed to parse xspf file embedded in %s\n", file);
		exit(EXIT_FAILURE);
	}

	{
		AMZPlaylistEntry *entry = list->data;

		g_print("Downloading album %s by %s.\n\n", entry->album, entry->creator);
	}

	for (node = list; node != NULL; node = node->next)
	{
		gchar *path;
		AMZPlaylistEntry *entry = node->data;

		path = build_download_path(entry);

		g_print("Downloading %s as %s.\n", entry->title, path);
		amzdownload_session_download_url(session, entry->location, path);
	}
}

int
main(gint argc, const gchar *argv[])
{
	GMainLoop *loop;
	SoupSession *session;
	gint i;

	g_type_init();

	session = amzdownload_session_new();
	loop = g_main_loop_new(NULL, TRUE);

	if (argc < 2)
	{
		fprintf(stderr, "usage: %s file.amz\n", argv[0]);
		return EXIT_FAILURE;
	}

	for (i = 1; i < argc; i++)
		handle_amz_file(session, argv[i]);

	g_main_loop_unref(loop);
	g_object_unref(session);

	return EXIT_SUCCESS;
}
