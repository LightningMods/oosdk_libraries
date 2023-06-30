/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2018, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include "curl_setup.h"

/***********************************************************************
 * Only for plain IPv4 builds
 **********************************************************************/
#ifdef CURLRES_IPV4 /* plain IPv4 code coming up */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#ifdef HAVE_PROCESS_H
#include <process.h>
#endif

#include "urldata.h"
#include "sendf.h"
#include "hostip.h"
#include "hash.h"
#include "share.h"
#include "strerror.h"
#include "url.h"
#include "inet_pton.h"
/* The last 3 #include files should be in this order */
#include "curl_printf.h"
#include "curl_memory.h"
#include "memdebug.h"

/*
 * Curl_ipvalid() checks what CURL_IPRESOLVE_* requirements that might've
 * been set and returns TRUE if they are OK.
 */
bool Curl_ipvalid(struct connectdata *conn)
{
  if(conn->ip_version == CURL_IPRESOLVE_V6)
    /* An IPv6 address was requested and we can't get/use one */
    return FALSE;

  return TRUE; /* OK, proceed */
}

#ifdef CURLRES_SYNCH

/*
 * Curl_getaddrinfo() - the IPv4 synchronous version.
 *
 * The original code to this function was from the Dancer source code, written
 * by Bjorn Reese, it has since been patched and modified considerably.
 *
 * gethostbyname_r() is the thread-safe version of the gethostbyname()
 * function. When we build for plain IPv4, we attempt to use this
 * function. There are _three_ different gethostbyname_r() versions, and we
 * detect which one this platform supports in the configure script and set up
 * the HAVE_GETHOSTBYNAME_R_3, HAVE_GETHOSTBYNAME_R_5 or
 * HAVE_GETHOSTBYNAME_R_6 defines accordingly. Note that HAVE_GETADDRBYNAME
 * has the corresponding rules. This is primarily on *nix. Note that some unix
 * flavours have thread-safe versions of the plain gethostbyname() etc.
 *
 */
Curl_addrinfo *Curl_getaddrinfo(struct connectdata *conn,
                                const char *hostname,
                                int port,
                                int *waitp)
{
  Curl_addrinfo *ai = NULL;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (void)conn;
#endif

  *waitp = 0; /* synchronous response only */

  ai = Curl_ipv4_resolve_r(hostname, port);
  if(!ai)
    infof(conn->data, "Curl_ipv4_resolve_r failed for %s\n", hostname);

  return ai;
}
#endif /* CURLRES_SYNCH */
#endif /* CURLRES_IPV4 */

#if defined(CURLRES_IPV4) && !defined(CURLRES_ARES)

/*
 * Curl_ipv4_resolve_r() - ipv4 threadsafe resolver function.
 *
 * This is used for both synchronous and asynchronous resolver builds,
 * implying that only threadsafe code and function calls may be used.
 *
 */
Curl_addrinfo *Curl_ipv4_resolve_r(const char *hostname,
                                   int port)
{
#if !defined(HAVE_GETADDRINFO_THREADSAFE) && defined(HAVE_GETHOSTBYNAME_R_3)
  int res;
#endif
  Curl_addrinfo *ai = NULL;
  struct hostent *h = NULL;
  struct in_addr in;
  struct hostent *buf = NULL;

  if(Curl_inet_pton(AF_INET, hostname, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    return Curl_ip2addr(AF_INET, &in, hostname, port);
  else {
    struct addrinfo hints;
    char sbuf[12];
    char *sbufptr = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if(port) {
      msnprintf(sbuf, sizeof(sbuf), "%d", port);
      sbufptr = sbuf;
    }

  (void)Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &ai);

  if(h) {
    ai = Curl_he2ai(h, port);

    if(buf) /* used a *_r() function */
      free(buf);
  }

  return ai;
}
#endif /* defined(CURLRES_IPV4) && !defined(CURLRES_ARES) */
