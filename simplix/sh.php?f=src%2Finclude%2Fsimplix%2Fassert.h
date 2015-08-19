<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <title>Simplix - assert.h</title>
    <link type="text/css" rel="stylesheet" href="sh.css">
  </head>
  <body>
    <p id="wait-msg">Please wait...</p>
    <textarea id="code" name="code" class="c" cols="60" rows="10">/*===========================================================================
 *
 * assert.h
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

#ifndef _SIMPLIX_ASSERT_H_
#define _SIMPLIX_ASSERT_H_

#include <simplix/macros.h>
#include <simplix/proto.h>

#define ASSERT(expr)                                                          \
    if (!(expr)) {                                                            \
        cli();                                                                \
        printk("Assertion failed in file %s at line %u", __FILE__, __LINE__); \
        hlt();                                                                \
    }

#endif /* _SIMPLIX_ASSERT_H_ */
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
