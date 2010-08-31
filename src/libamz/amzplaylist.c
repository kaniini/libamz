/*
 * libamz: library for accessing, manipulating and decrypting amz files.
 * amzplaylist.c: Parsing of XSPF playlists.
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

#include <glib.h>
#include <gcrypt.h>

#include "libamz.h"

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/uri.h>

#define XSPF_ROOT_NODE_NAME "playlist"
#define XSPF_XMLNS "http://xspf.org/ns/0/"

static AMZPlaylistEntry *
amzplaylist_parse_track(xmlNodePtr track, xmlChar *base)
{
	xmlNodePtr nptr;
	AMZPlaylistEntry *entry;

	entry = g_slice_new0(AMZPlaylistEntry);

	for (nptr = track->children; nptr != NULL; nptr = nptr->next)
	{
		if (nptr->type == XML_ELEMENT_NODE)
		{
			if (!xmlStrcmp(nptr->name, (xmlChar *)"location"))
				entry->location = g_strdup((gchar *) xmlNodeGetContent(nptr));
			else if (!xmlStrcmp(nptr->name, (xmlChar *)"creator"))
				entry->creator = g_strdup((gchar *) xmlNodeGetContent(nptr));
			else if (!xmlStrcmp(nptr->name, (xmlChar *)"album"))
				entry->album = g_strdup((gchar *) xmlNodeGetContent(nptr));
			else if (!xmlStrcmp(nptr->name, (xmlChar *)"title"))
				entry->title = g_strdup((gchar *) xmlNodeGetContent(nptr));
			else if (!xmlStrcmp(nptr->name, (xmlChar *)"trackNum"))
				entry->tracknum = atol((gchar *) xmlNodeGetContent(nptr));
			else if (!xmlStrcmp(nptr->name, (xmlChar *)"duration"))
				entry->duration = atol((gchar *) xmlNodeGetContent(nptr));
		}
	}

	return entry;	
}

static GList *
amzplaylist_parse_tracklist(GList *list, xmlNodePtr tracklist, xmlChar *base)
{
	xmlNodePtr nptr;
	AMZPlaylistEntry *entry;

	for (nptr = tracklist->children; nptr != NULL; nptr = nptr->next)
	{
		if (nptr->type == XML_ELEMENT_NODE && !xmlStrcmp (nptr->name, (xmlChar *) "track"))
		{
			entry = amzplaylist_parse_track(nptr, base);
			list = g_list_prepend(list, entry);
		}
	}

	return g_list_reverse(list);
}

GList *
amzplaylist_parse(const guchar *indata)
{
	xmlDocPtr doc;
	xmlNodePtr nptr, nptr2;
	GList *ret = NULL;

	doc = xmlRecoverDoc(indata);
	if (doc == NULL)
	{
		fprintf(stderr, "xmlRecoverDoc is NULL\n");
        	return NULL;
	}

	for (nptr = doc->children; nptr != NULL; nptr = nptr->next)
	{
		if (nptr->type == XML_ELEMENT_NODE && !xmlStrcmp(nptr->name, (xmlChar *) "playlist"))
		{
			xmlChar *base;

			base = xmlNodeGetBase(doc, nptr);
			for (nptr2 = nptr->children; nptr2 != NULL; nptr2 = nptr2->next)
			{
				if (nptr2->type == XML_ELEMENT_NODE && !xmlStrcmp(nptr2->name, (xmlChar *) "trackList"))
					ret = amzplaylist_parse_tracklist(ret, nptr2, base);
			}
		}
	}

	return ret;
}

static void
amzplaylist_free_entry(AMZPlaylistEntry *entry, gpointer userdata)
{
	g_free(entry->location);
	g_free(entry->creator);
	g_free(entry->album);
	g_free(entry->title);

	g_slice_free(AMZPlaylistEntry, entry);
}

void
amzplaylist_free(GList *playlist)
{
	g_return_if_fail(playlist != NULL);

	g_list_foreach(playlist, (GFunc) amzplaylist_free_entry, NULL);
	g_list_free(playlist);
}
