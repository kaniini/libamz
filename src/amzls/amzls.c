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

int
main(gint argc, const gchar *argv[])
{
	GList *list, *node;
	gint i = 1;

	guchar *data;
	gsize len, tracks;

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

	tracks = g_list_length(list);
	g_print("%s: %lu track%s\n\n", argv[1], tracks, tracks != 1 ? "s" : "");

	for (node = list; node != NULL; node = node->next)
	{
		AMZPlaylistEntry *entry = node->data;

		g_print("%5d. %s - %s\n", i, entry->creator, entry->title);
		g_print("       %s\n", entry->location);
		i++;
	}

	return EXIT_SUCCESS;
}
