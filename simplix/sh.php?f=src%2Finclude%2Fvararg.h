<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <title>Simplix - vararg.h</title>
    <link type="text/css" rel="stylesheet" href="sh.css">
  </head>
  <body>
    <p id="wait-msg">Please wait...</p>
    <textarea id="code" name="code" class="c" cols="60" rows="10">/*===========================================================================
 *
 * vararg.h
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
 *===========================================================================
 *
 * Support for variadic functions.
 *
 *===========================================================================*/

#ifndef _VARARG_H_
#define _VARARG_H_

typedef char* va_list;

/* Number of bytes used by the specified type in the stack.
   This is equivalent to sizeof(long) * ceil(sizeof(type) / sizeof(long)) */
#define __va_size(type) \
    (((sizeof(type) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))

/* Initializes the specified pointer (ap) to point to the first parameter
   following the specified parameter (last) */
#define va_start(ap, last) \
    ((ap) = (va_list) &(last) + __va_size(last))

/* Make the specified pointer (ap) point to the next parameter in the stack. */
#define va_arg(ap, type) \
    (*(type *)((ap) += __va_size(type), (ap) - __va_size(type)))

#endif /* _VARARG_H_ */
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