<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <title>Simplix - Source Code</title>
    <style>

/* YUI reset.css */

html{color:#000;background:#FFF;}
body,div,dl,dt,dd,ul,ol,li,h1,h2,h3,h4,h5,h6,pre,code,form,fieldset,legend,input,textarea,p,blockquote,th,td{margin:0;padding:0;}
table{border-collapse:collapse;border-spacing:0;}
fieldset,img{border:0;}
address,caption,cite,code,dfn,em,strong,th,var{font-style:normal;font-weight:normal;}
li{list-style:none;}
caption,th{text-align:left;}
h1,h2,h3,h4,h5,h6{font-size:100%;font-weight:normal;}
q:before,q:after{content:'';}
abbr,acronym {border:0;font-variant:normal;}
/* to preserve line-height and selector appearance */
sup {vertical-align:text-top;}
sub {vertical-align:text-bottom;}
input,textarea,select{font-family:inherit;font-size:inherit;font-weight:inherit;}
/* because legend doesn't inherit in IE */
legend{color:#000;}

/* YUI fonts.css */

body {font:13px/1.231 arial,helvetica,clean,sans-serif;*font-size:small;*font:x-small;}
table {font-size:inherit;font:100%;}
pre,code,kbd,samp,tt {font-family:monospace;*font-size:108%;line-height:100%;}

/* TOC styles */

body {padding:1em;}
h1 {font-size:1.5em;font-weight:bold;margin-bottom:.5em;}
ul {padding-left:24px;}
li {line-height:16px;*line-height:19px;*padding-left:6px;}
li.dir {list-style-image:url(dir-icon.png);}
li.unknown {list-style-image:url(unknown-icon.png);}
li.as {list-style-image:url(as-icon.png);}
li.c {list-style-image:url(c-icon.png);}
li.h {list-style-image:url(h-icon.png);}
#sourcetree {padding-left:24px;*padding-left:16px;}
a, a:link, a:visited, a:active {color:blue;}
a:hover {color:red;}

    </style>
  </head>
  <body>
    <ul id="sourcetree">
<li class="dir">boot<ul>
<li class="as"><a href="sh.php?f=src%252Fboot%252Fbootsect.S" target="sourceframe">bootsect.S</a></li>
</ul></li>
<li class="dir">drivers<ul>
<li class="c"><a href="sh.php?f=src%252Fdrivers%252Fgfx.c" target="sourceframe">gfx.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fdrivers%252Fide.c" target="sourceframe">ide.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fdrivers%252Fkbd.c" target="sourceframe">kbd.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fdrivers%252Framdisk.c" target="sourceframe">ramdisk.c</a></li>
</ul></li>
<li class="dir">include<ul>
<li class="dir">simplix<ul>
<li class="dir">keymaps<ul>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fkeymaps%252Fus-std.h" target="sourceframe">us-std.h</a></li>
</ul></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fassert.h" target="sourceframe">assert.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fconsts.h" target="sourceframe">consts.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fcontext.h" target="sourceframe">context.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fglobals.h" target="sourceframe">globals.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fio.h" target="sourceframe">io.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Flist.h" target="sourceframe">list.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fmacros.h" target="sourceframe">macros.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fproto.h" target="sourceframe">proto.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Fsegment.h" target="sourceframe">segment.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Ftask.h" target="sourceframe">task.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Ftss.h" target="sourceframe">tss.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsimplix%252Ftypes.h" target="sourceframe">types.h</a></li>
</ul></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fstdlib.h" target="sourceframe">stdlib.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fstring.h" target="sourceframe">string.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fsyscalls.h" target="sourceframe">syscalls.h</a></li>
<li class="h"><a href="sh.php?f=src%252Finclude%252Fvararg.h" target="sourceframe">vararg.h</a></li>
</ul></li>
<li class="dir">kernel<ul>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fblkdev.c" target="sourceframe">blkdev.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fexception.c" target="sourceframe">exception.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fgdt.c" target="sourceframe">gdt.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fidt.c" target="sourceframe">idt.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Firq.c" target="sourceframe">irq.c</a></li>
<li class="as"><a href="sh.php?f=src%252Fkernel%252Fisr.S" target="sourceframe">isr.S</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fkmem.c" target="sourceframe">kmem.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fksync.c" target="sourceframe">ksync.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fmain.c" target="sourceframe">main.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fphysmem.c" target="sourceframe">physmem.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fsched.c" target="sourceframe">sched.c</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Fsys.c" target="sourceframe">sys.c</a></li>
<li class="as"><a href="sh.php?f=src%252Fkernel%252Fsyscall.S" target="sourceframe">syscall.S</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Ftask.c" target="sourceframe">task.c</a></li>
<li class="as"><a href="sh.php?f=src%252Fkernel%252Ftask_switch.S" target="sourceframe">task_switch.S</a></li>
<li class="c"><a href="sh.php?f=src%252Fkernel%252Ftimer.c" target="sourceframe">timer.c</a></li>
</ul></li>
<li class="dir">lib<ul>
<li class="c"><a href="sh.php?f=src%252Flib%252Fstdlib.c" target="sourceframe">stdlib.c</a></li>
<li class="c"><a href="sh.php?f=src%252Flib%252Fstring.c" target="sourceframe">string.c</a></li>
</ul></li>
<li class="unknown"><a href="sh.php?f=src%252F.bochsrc" target="sourceframe">.bochsrc</a></li>
<li class="unknown"><a href="sh.php?f=src%252FMakefile" target="sourceframe">Makefile</a></li>
<li class="unknown"><a href="sh.php?f=src%252Fsimplix.lds" target="sourceframe">simplix.lds</a></li>
    </ul>
  </body>
</html>