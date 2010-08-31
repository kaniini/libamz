/*
 * libamz: library for accessing, manipulating and decrypting amz files.
 * libamz.h: API declarations.
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

#include <libsoup/soup.h>

#include <stddef.h>
#include <stdbool.h>

#ifndef __LIBAMZ_H__
#define __LIBAMZ_H__

/* amzfile */
extern bool amzfile_decrypt_blob(gchar *indata, gsize inlen, guchar **outdata, gsize *outlen);
extern bool amzfile_decrypt_file(const gchar *file, guchar **outdata, gsize *outlen);

/* amzplaylist */
typedef struct {
	gchar *location;
	gchar *title;
	gchar *creator;
	gchar *album;
	gint tracknum;
	gint64 duration;
	GHashTable *meta;
} AMZPlaylistEntry;

extern GList *amzplaylist_parse(const guchar *indata);
extern void amzplaylist_free(GList *playlist);

/* amzdownload */
SoupSession *amzdownload_session_new(void);
bool amzdownload_session_download_url(SoupSession *session, const gchar *url, const gchar *path);

#endif
