YAHOO.widget.ImageCropper=function(v,A){var K=YAHOO.util.Dom;var w=YAHOO.util.Event;var x=YAHOO.lang;var j=this;var W;var P;var V=false;var b;function h(L,Q){var e,I,D;if(YAHOO.env.ua.ie){switch(Q){case "top":D=L.clientTop;break;case "right":D=L.offsetWidth-L.clientWidth-L.clientLeft;break;case "bottom":D=L.offsetHeight-L.clientHeight-L.clientTop;break;case "left":D=L.clientLeft;break;default:throw new Error("Invalid border: "+Q);}}else{switch(Q){case "top":e="border-top-width";break;case "right":e="border-right-width";break;case "bottom":e="border-bottom-width";break;case "left":e="border-left-width";break;default:throw new Error("Invalid border: "+Q);}I=K.getStyle(L,e);D=parseInt(I,10);}return D;}function r(D){var L,C,I,a,e,Q;I=h(D,"left");a=h(D,"top");L=D.offsetLeft+I;C=D.offsetTop+a;if(YAHOO.env.ua.gecko||YAHOO.env.ua.opera){e=h(D.offsetParent,"left");Q=h(D.offsetParent,"top");if(YAHOO.env.ua.gecko){L+=e;C+=Q;}else{if(YAHOO.env.ua.opera){L-=e;C-=Q;}}}return {x:L,y:C,w:D.clientWidth,h:D.clientHeight};}function T(D,Q,C,I,L){var e,a;D.style.position="absolute";D.style.left=Q+"px";D.style.top=C+"px";D.style.width=I+"px";D.style.height=L+"px";e=r(D);a=e.x-Q;if(a!==0){D.style.left=(Q-a)+"px";}a=e.y-C;if(a!==0){D.style.top=(C-a)+"px";}a=e.w-I;if(a!==0){D.style.width=(I-a)+"px";}a=e.h-L;if(a!==0){D.style.height=(L-a)+"px";}}function S(){var D,I,f,L,e,a,C,Q;if(V){return ;}v.style.width="auto";v.style.height="auto";W=document.createElement("DIV");W.className="image-cropper";v.parentNode.replaceChild(W,v);W.appendChild(v);D=document.createElement("DIV");D.className="mask";D.style.width=v.clientWidth+"px";D.style.height=v.clientHeight+"px";W.appendChild(D);P=document.createElement("DIV");P.className="cropper";P.style.background="url("+v.src+")";W.appendChild(P);if(A){L=A.w;e=A.h;I=A.x;f=A.y;a=A.xyratio;}if(!x.isNumber(L)||L<0||L>v.clientWidth){L=Math.floor(v.clientWidth/3);}if(x.isNumber(a)&&a>0){e=a*L;}if(!x.isNumber(e)||e<0||e>v.clientHeight){e=Math.floor(v.clientHeight/3);}if(!x.isNumber(I)||I<0||I+L>v.clientWidth){I=(v.clientWidth-L)/2;}if(!x.isNumber(f)||f<0||f+e>v.clientHeight){f=(v.clientHeight-e)/2;}T(P,I,f,L,e);C=r(P);P.style.backgroundPosition=(-C.x)+"px "+(-C.y)+"px";if(!A||A.noresize!==true){J();}Q=new YAHOO.util.DD(P);Q.startDrag=function(){C=r(P);this.resetConstraints();this.setXConstraint(C.x,v.clientWidth-C.x-C.w);this.setYConstraint(C.y,v.clientHeight-C.y-C.h);};Q.onDrag=function(G){C=r(P);P.style.backgroundPosition=(-C.x)+"px "+(-C.y)+"px";};Q.endDrag=function(G){j.onChangeEvent.fire();};V=true;}function J(){var D,e,Q,L,I,a;for(D=0;D<4||(!A||!x.isNumber(A.xyratio)||A.xyratio<=0)&&D<8;D++){e=document.createElement("DIV");P.appendChild(e);switch(D){case 0:K.addClass(e,"t");K.addClass(e,"l");break;case 1:K.addClass(e,"t");K.addClass(e,"r");break;case 2:K.addClass(e,"b");K.addClass(e,"l");break;case 3:K.addClass(e,"b");K.addClass(e,"r");break;case 4:K.addClass(e,"t");K.addClass(e,"c");break;case 5:K.addClass(e,"m");K.addClass(e,"r");break;case 6:K.addClass(e,"b");K.addClass(e,"c");break;case 7:K.addClass(e,"m");K.addClass(e,"l");break;}Q=new YAHOO.util.DD(e);Q.alignElWithMouse=function(C,G,f){};Q.startDrag=function(C,f){a=r(P);L=C;I=f;};Q.onDrag=function(O){var n,z,l,t,R,C,G,f;n=w.getPageX(O)-L;z=w.getPageY(O)-I;e=this.getEl();if(K.hasClass(e,"l")){if(n<0){n=Math.max(-a.x,n);}else{n=Math.min(a.w,n);}l=a.x+n;R=a.w-n;}else{if(K.hasClass(e,"r")){if(n<0){n=Math.max(-a.w,n);}else{n=Math.min(v.clientWidth-a.x-a.w,n);}l=a.x;R=a.w+n;}else{n=0;l=a.x;R=a.w;}}if(K.hasClass(e,"t")){if(z<0){z=Math.max(-a.y,z);}else{z=Math.min(a.h,z);}t=a.y+z;G=a.h-z;}else{if(K.hasClass(e,"b")){if(z<0){z=Math.max(-a.h,z);}else{z=Math.min(v.clientHeight-a.y-a.h,z);}t=a.y;G=a.h+z;}else{z=0;t=a.y;G=a.h;}}if(A&&x.isNumber(A.xyratio)&&A.xyratio>0){C=A.xyratio;G=Math.floor(R*C);if(K.hasClass(e,"t")){t=a.y+a.h-G;if(t<0){t=0;z=-a.y;G=a.h-z;R=Math.floor(G/C);if(K.hasClass(e,"l")){n=Math.floor(-z/C);l=a.x-n;}}}else{if(t+G>v.clientHeight){G=v.clientHeight-t;R=Math.floor(G/C);if(K.hasClass(e,"l")){z=G-a.h;n=Math.floor(z/C);l=a.x-n;}}}}T(P,l,t,R,G);f=r(P);P.style.backgroundPosition=(-f.x)+"px "+(-f.y)+"px";};Q.endDrag=function(C){j.onChangeEvent.fire();};}}this.onChangeEvent=new YAHOO.util.CustomEvent("onChange");this.getCropRegion=function(){return r(P);};v=K.get(v);if(!v||v.tagName!=="IMG"||A&&!x.isObject(A)){throw new Error("Invalid argument");}if(YAHOO.env.ua.webkit){b=setInterval(function(){if(v.width!==0||v.height!==0){clearInterval(b);S();}},100);}else{if(!v.complete||v.naturalWidth===0){w.addListener(v,"load",S);}else{S();}}};
