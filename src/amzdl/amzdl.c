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
	gchar *ret, *filename;

	filename = g_strdup_printf("%02d - %s.mp3", entry->tracknum, entry->title);
	ret = g_build_filename(g_get_home_dir(), "Music", entry->creator, entry->album, filename, NULL);

	return ret;
}

int
main(gint argc, const gchar *argv[])
{
	GList *list, *node;
	GMainLoop *loop;
	SoupSession *session;

	guchar *data;
	gsize len;

	g_type_init();

	if (!argv[1])
	{
		fprintf(stderr, "usage: %s file.amz\n", argv[0]);
		return EXIT_FAILURE;
	}

	amzfile_decrypt_file(argv[1], &data, &len);
	list = amzplaylist_parse(data);
	if (list == NULL)
	{
		fprintf(stderr, "failed to parse xspf file embedded in %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	{
		AMZPlaylistEntry *entry = list->data;

		g_print("Downloading album %s by %s.\n\n", entry->album, entry->creator);
	}

	session = amzdownload_session_new();
	loop = g_main_loop_new(NULL, TRUE);

	for (node = list; node != NULL; node = node->next)
	{
		gchar *path;
		AMZPlaylistEntry *entry = node->data;

		path = build_download_path(entry);

		g_print("Downloading %s as %s.\n", entry->title, path);
		amzdownload_session_download_url(session, entry->location, path);
	}

	g_main_loop_unref(loop);
	g_object_unref(session);

	return EXIT_SUCCESS;
}
