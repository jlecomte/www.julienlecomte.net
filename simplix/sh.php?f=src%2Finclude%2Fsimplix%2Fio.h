<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <title>Simplix - io.h</title>
    <link type="text/css" rel="stylesheet" href="sh.css">
  </head>
  <body>
    <p id="wait-msg">Please wait...</p>
    <textarea id="code" name="code" class="c" cols="60" rows="10">/*===========================================================================
 *
 * io.h
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

#ifndef _SIMPLIX_IO_H_
#define _SIMPLIX_IO_H_

#include <simplix/types.h>

/*
 * Busy loop for a small amount of time. Note: this function is not calibrated
 * (accurate calibration is not trivial, but required in a real OS) Here, we'll
 * just assume that calling udelay(1) busy loops for at least 1 microsecond.
 */
static inline void udelay(unsigned long n)
{
    if (!n)
        return;
    asm("1: dec %%eax; jne 1b;"
        : : "a" (n * 1000));
}

static inline void mdelay(unsigned long n)
{
    while (--n)
        udelay(1000);
}

static inline void outb(int port, byte_t data)
{
    asm volatile("outb %0, %w1" : : "a" (data), "d" (port));
}

static inline byte_t inb(int port)
{
    byte_t data;
    asm volatile("inb %w1, %0" : "=a" (data) : "d" (port));
    return data;
}

static inline void outw(int port, uint16_t data)
{
    asm volatile("outw %0, %w1" : : "a" (data), "d" (port));
}

static inline uint16_t inw(int port)
{
    uint16_t data;
    asm volatile("inw %w1, %0" : "=a" (data) : "d" (port));
    return data;
}

#endif /* _SIMPLIX_IO_H_ */
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