/*
 * libamz: library for accessing, manipulating and decrypting amz files.
 * amzfile.c: Raw I/O access and decryption of AMZ files.
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

/*
 * LOL.
 *
 * These guys use DES encryption which has been broken since the 1970s.  Keys
 * cracked in 37 seconds on a Core i7 processor running at 2.7GHz.
 */
static const guchar amazon_key[8] = { 0x29, 0xAB, 0x9D, 0x18, 0xB2, 0x44, 0x9E, 0x31 };
static const guchar amazon_iv[8]  = { 0x5E, 0x72, 0xD7, 0x9A, 0x11, 0xB3, 0x4F, 0xEE };

/*
 * decrypts the underlying XSPF playlist.
 * does not *parse* the XSPF playlist.
 */
bool
amzfile_decrypt_blob(gchar *indata, gsize inlen, guchar **outdata, gsize *outlen)
{
	gcry_cipher_hd_t hd;
	gcry_error_t err;
	guchar *unpackdata, *decryptdata;
	gsize unpacklen;
	gint i;

	unpackdata = g_base64_decode(indata, &unpacklen);
	if (unpackdata == NULL)
	{
		g_print("g_base64_decode failed\n");
		return false;
	}

	if (unpacklen % 8)
		unpacklen -= (unpacklen % 8);

	decryptdata = g_malloc0(unpacklen + 1);

	if ((err = gcry_cipher_open(&hd, GCRY_CIPHER_DES, GCRY_CIPHER_MODE_CBC, 0)))
	{
		g_print("unable to initialise gcrypt: %s", gcry_strerror(err));
		free(unpackdata);
		g_free(decryptdata);
		return false;
	}

	if ((err = gcry_cipher_setkey(hd, amazon_key, 8)))
	{
		g_print("unable to set key for DES block cipher: %s", gcry_strerror(err));
		gcry_cipher_close(hd);
		free(unpackdata);
		g_free(decryptdata);
		return false;
	}

	if ((err = gcry_cipher_setiv(hd, amazon_iv, 8)))
	{
		g_print("unable to set initialisation vector for DES block cipher: %s", gcry_strerror(err));
		gcry_cipher_close(hd);
		free(unpackdata);
		g_free(decryptdata);
		return false;
	}

	if ((err = gcry_cipher_decrypt(hd, decryptdata, unpacklen, unpackdata, unpacklen)))
	{
		g_print("unable to decrypt embedded DES-encrypted XSPF document: %s", gcry_strerror(err));
		gcry_cipher_close(hd);
		free(unpackdata);
		g_free(decryptdata);
		return false;
	}

	free(unpackdata);
	gcry_cipher_close(hd);

	/* remove padding from XSPF document */
	for (i = unpacklen; i > 0; i--)
	{
		if (decryptdata[i - 1] == '\n' || decryptdata[i] == '\r' || decryptdata[i - 1] >= ' ')
			break;
	}
	decryptdata[i] = 0;

	*outdata = decryptdata;
	return true;
}

/*
 * Decrypt a file returning the XML data as outdata.
 * Returns true on success, false on failure.
 */
bool
amzfile_decrypt_file(const gchar *file, guchar **outdata, gsize *outlen)
{
	gchar *b64data;
	gsize b64len;
	GError *error = NULL;
	bool ret;

	if (!g_file_get_contents(file, &b64data, &b64len, &error))
	{
		g_print("cannot open %s: %s", file, error->message);
		return false;
	}

	ret = amzfile_decrypt_blob(b64data, b64len, outdata, outlen);

	g_free(b64data);

	return ret;
}
