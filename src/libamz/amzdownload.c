/*
 * libamz: library for accessing, manipulating and decrypting amz files.
 * amzdownload.c: utility functions for downloading files.
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

SoupSession *
amzdownload_session_new(void)
{
	return soup_session_async_new();
}

static void
amzdownload_session_got_headers(SoupMessage *msg, AMZDownloadContext *ctx)
{
	ctx->length = soup_message_headers_get_content_length(msg->response_headers);
}

static void
amzdownload_session_got_chunk(SoupMessage *msg, SoupBuffer *chunk, AMZDownloadContext *ctx)
{
	ctx->bytes = msg->response_body->length;
	ctx->progress = ((float) ctx->bytes / (float) ctx->length) * 100.;

	if (ctx->progress_notify != NULL)
		ctx->progress_notify(msg, ctx);
}

bool
amzdownload_session_download_url(SoupSession *session, const gchar *url, const gchar *path,
				 void (*progress_notify)(SoupMessage *msg, AMZDownloadContext *ctx))
{
	AMZDownloadContext ctx;
	GError *error = NULL;

	ctx.msg = soup_message_new(SOUP_METHOD_GET, url);
	ctx.progress_notify = progress_notify;

	g_signal_connect(ctx.msg, "got-headers", G_CALLBACK(amzdownload_session_got_headers), &ctx);
	g_signal_connect(ctx.msg, "got-chunk", G_CALLBACK(amzdownload_session_got_chunk), &ctx);

	soup_session_send_message(session, ctx.msg);

	if (!SOUP_STATUS_IS_SUCCESSFUL(ctx.msg->status_code))
	{
		g_warning("%s: %d %s\n", url, ctx.msg->status_code, ctx.msg->reason_phrase);
		return false;
	}

	g_file_set_contents(path, ctx.msg->response_body->data, ctx.msg->response_body->length, &error);
	if (error != NULL)
	{
		g_warning("%s: %s", url, error->message);
		return false;
	}

	g_object_unref(ctx.msg);

	return true;
}
