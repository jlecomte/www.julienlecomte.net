<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <title>Simplix - string.h</title>
    <link type="text/css" rel="stylesheet" href="sh.css">
  </head>
  <body>
    <p id="wait-msg">Please wait...</p>
    <textarea id="code" name="code" class="c" cols="60" rows="10">/*===========================================================================
 *
 * string.h
 *
 * Copyright (C) 2007 - Julien Lecomte
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *===========================================================================*/

#ifndef _STRING_H_
#define _STRING_H_

#include <vararg.h>

#include <simplix/types.h>

void *memset(void *s, int c, size_t size);
void *memcpy(void *dest, const void *src, size_t n);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int snprintf(char *str, size_t size, const char *format, ...);
size_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);

#endif /* _STRING_H_ */
</textarea>
    <script type="text/javascript" src="sh.js"></script>
    <script type="text/javascript">
setTimeout(function () {
    var el = document.createElement('p');
    el.id = 'wait-msg';
    el.innerHTML = 'Please wait...';
    document.body.appendChild(el);
    // dp.SyntaxHighlighter.ClipboardSwf = 'clipboard.swf';
    dp.SyntaxHighlighter.HighlightAll('code');
    document.body.removeChild(el);
}, 0);
    </script>
  </body>
</html>