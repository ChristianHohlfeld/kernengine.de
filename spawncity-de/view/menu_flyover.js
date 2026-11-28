// Unchanged from the original SpawnCity (spawncity.de, drone_simulator.html inline script, commit
// 9108537): the title screen's flyover over a neon wireframe city on #gameMenuBg.
(function(){
  if(/[?&]dev=1/.test(location.search))document.body.classList.add("dev-ui");
  // Menu flyover: neon wireframe city. Black plate, green ground grid and
  // green wireframe blocks; the world stays fixed, only the camera moves.
  // It is drawn in a Web Worker on an OffscreenCanvas, so it keeps flying
  // perfectly smoothly while the main thread is busy loading the game
  // (script start-up, WebGL context, Box3D, shader compilation). Without
  // OffscreenCanvas it falls back to the same code on the main thread.
  var c=document.getElementById("gameMenuBg"),menu=document.getElementById("gameMenu"),DPR=Math.min(1.5,window.devicePixelRatio||1);
  function FLY(g,canvasRef,DPR,RAF){var W=0,H=0,HIDDEN=false,NEED_SIZE=true,GLOW=false,blocks=[],last=performance.now();
  function size(){W=canvasRef.w;H=canvasRef.h;canvasRef.c.width=Math.max(1,Math.round(W*DPR));canvasRef.c.height=Math.max(1,Math.round(H*DPR));g.setTransform(DPR,0,0,DPR,0,0);NEED_SIZE=false;}
  var STREET=10,FAR=520,GRID=16,BG="#0c0f13",LINE="rgba(255,190,120,.10)";
  var cam={x:0,y:12,z:0,yaw:0,roll:0,pitch:0};
  function rnd(a,b){return a+Math.random()*(b-a);}
  function shapeBlock(b,z){b.z0=z;b.z1=z+rnd(14,34);b.depth=rnd(12,22);b.h=Math.random()<.2?rnd(40,78):rnd(14,30);b.floors=Math.random()<.5;return b;}
  function addBlock(side,z){var b=shapeBlock({side:side},z);blocks.push(b);return b.z1+rnd(2,7);}
  [-1,1].forEach(function(side){var z=-10;while(z<FAR+60)z=addBlock(side,z);});
  var cy=1,sy=0;
  // world (x right, y up, z forward) -> screen
  function P(x,y,z,f){var dx=x-cam.x,dz=z-cam.z,rx=dx*cy-dz*sy,rz=dx*sy+dz*cy;if(rz<1.2)return null;return[W/2+rx/rz*f,H*(.55+cam.pitch)-(y-cam.y)/rz*f];}
  function path(p){for(var i=0;i<p.length;i++)if(!p[i])return false;g.beginPath();g.moveTo(p[0][0],p[0][1]);for(var k=1;k<p.length;k++)g.lineTo(p[k][0],p[k][1]);g.closePath();return true;}
  function face(p,fill){if(!path(p))return false;g.fillStyle=fill||BG;g.fill();g.stroke();return true;}
  function seg(a,b){if(!a||!b)return;g.beginPath();g.moveTo(a[0],a[1]);g.lineTo(b[0],b[1]);g.stroke();}
  function frame(now){
    if(HIDDEN){RAF(frame);last=now;return;}
    var dt=Math.min(.05,(now-last)/1000);last=now;if(NEED_SIZE)size();
    var t=now/1000,speed=26+6*Math.sin(t*.11),prevX=cam.x;cam.z+=speed*dt;cam.x=Math.sin(t*.23)*2.6;cam.y=12+Math.sin(t*.37)*2.2;
    var lateral=(cam.x-prevX)/Math.max(dt,1e-3);cam.yaw+=((lateral/speed)*.6-cam.yaw)*Math.min(1,dt*2);cy=Math.cos(cam.yaw);sy=Math.sin(cam.yaw);cam.roll+=(-(lateral/speed)*.35-cam.roll)*Math.min(1,dt*2.5);cam.pitch=-.02+Math.sin(t*.29)*.012;
    // recycle what is behind the camera to the far end (world stays put)
    for(var k=0;k<blocks.length;k++){var b=blocks[k];if(b.z1<cam.z-8){var far=-1e9;for(var m=0;m<blocks.length;m++)if(blocks[m].side===b.side)far=Math.max(far,blocks[m].z1);shapeBlock(b,far+rnd(2,7));}}
    var f=H*.95,hz=H*(.55+cam.pitch);g.setTransform(DPR,0,0,DPR,0,0);
    // dusk sky: deep blue zenith -> burning orange horizon, sun glow at the vanishing point
    var sk=g.createLinearGradient(0,0,0,hz+2);sk.addColorStop(0,"#0b1322");sk.addColorStop(.55,"#33303a");sk.addColorStop(.86,"#a4532a");sk.addColorStop(1,"#e88a3c");g.fillStyle=sk;g.fillRect(0,0,W,hz+2);
    var sg=g.createRadialGradient(W/2,hz,0,W/2,hz,H*.7);sg.addColorStop(0,"rgba(255,190,110,.55)");sg.addColorStop(.25,"rgba(255,140,60,.18)");sg.addColorStop(1,"rgba(255,120,40,0)");g.fillStyle=sg;g.fillRect(0,0,W,hz+2);
    var gr=g.createLinearGradient(0,hz,0,H);gr.addColorStop(0,"#4a3428");gr.addColorStop(.18,"#1d1a19");gr.addColorStop(1,"#0d0e10");g.fillStyle=gr;g.fillRect(0,hz,W,H-hz);
    g.translate(W/2,hz);g.rotate(cam.roll);g.translate(-W/2,-hz);
    g.lineJoin="round";
    var zn=cam.z+1.5,zf=cam.z+FAR;
    // road: lane dashes and kerbs (world-fixed, so they stream past)
    g.strokeStyle="rgba(235,225,200,.55)";g.lineWidth=1.4;
    for(var dz=Math.ceil(zn/12)*12;dz<zf;dz+=12){g.globalAlpha=Math.max(.05,1-(dz-cam.z)/260);seg(P(0,0,dz,f),P(0,0,dz+5,f));}
    g.globalAlpha=1;g.strokeStyle="rgba(160,150,140,.45)";seg(P(-STREET+1.5,0,zn,f),P(-STREET+1.5,0,zf,f));seg(P(STREET-1.5,0,zn,f),P(STREET-1.5,0,zf,f));
    function fogged(r,gg,bb,d){var k=Math.min(1,Math.max(0,(d-30)/420));return"rgb("+Math.round(r+(170-r)*k*.75)+","+Math.round(gg+(95-gg)*k*.75)+","+Math.round(bb+(60-bb)*k*.75)+")";}
    function hash(a,b,c){var x=Math.sin(a*127.1+b*311.7+c*74.7)*43758.5453;return x-Math.floor(x);}
    // buildings: far to near, dark concrete with lit windows
    var list=blocks.filter(function(b){return b.z1>cam.z+1.5&&b.z0<cam.z+FAR;}).sort(function(a,b){return b.z0-a.z0;});
    g.lineWidth=1;
    for(var i=0;i<list.length;i++){var b=list[i],x=b.side*STREET,xb=b.side*(STREET+b.depth),z0=Math.max(b.z0,cam.z+1.6),z1=b.z1,dist=b.z0-cam.z;
      if(!b.seed)b.seed=Math.random()*1000;
      g.strokeStyle="rgba(0,0,0,.55)";
      face([P(x,0,z0,f),P(xb,0,z0,f),P(xb,b.h,z0,f),P(x,b.h,z0,f)],fogged(30,30,34,dist));
      var fac=[P(x,0,z0,f),P(x,0,z1,f),P(x,b.h,z1,f),P(x,b.h,z0,f)];
      if(face(fac,fogged(44,42,44,dist))){
        var near=fac[0]&&fac[1]?Math.abs(fac[0][0]-fac[1][0]):0;
        if(near>16){var cols=Math.floor((b.z1-b.z0)/3.4),rows=Math.floor(b.h/3.3);
          for(var r=0;r<rows;r++)for(var q=0;q<cols;q++){var lit=hash(b.seed,r,q);if(lit<.6)continue;var wz=b.z0+1+q*3.4;if(wz<z0||wz+1.6>z1)continue;var wy=2+r*3.3;
            var A=P(x,wy,wz,f),B=P(x,wy,wz+1.5,f),C=P(x,wy+1.6,wz+1.5,f),D=P(x,wy+1.6,wz,f);if(!A||!B||!C||!D)continue;
            g.beginPath();g.moveTo(A[0],A[1]);g.lineTo(B[0],B[1]);g.lineTo(C[0],C[1]);g.lineTo(D[0],D[1]);g.closePath();
            g.fillStyle=lit>.93?"rgba(255,214,150,.95)":lit>.8?"rgba(255,180,100,.75)":"rgba(150,170,190,.35)";g.fill();}}
        if(cam.y>b.h)face([P(x,b.h,z0,f),P(x,b.h,z1,f),P(xb,b.h,z1,f),P(xb,b.h,z0,f)],fogged(26,26,28,dist));}}
    // sodium street lamps
    for(var lz=Math.ceil(zn/26)*26;lz<zf;lz+=26)for(var sd=-1;sd<=1;sd+=2){var L=P(sd*(STREET-1),7,lz,f),Lb=P(sd*(STREET-1),0,lz,f);if(!L||!Lb)continue;var dd=lz-cam.z,rr=Math.max(1,30/Math.max(dd,1)*6);
      g.globalAlpha=Math.max(.15,1-dd/420);g.strokeStyle="rgba(20,20,22,.9)";g.lineWidth=Math.max(1,rr*.25);seg(L,Lb);
      var lg=g.createRadialGradient(L[0],L[1],0,L[0],L[1],rr*5);lg.addColorStop(0,"rgba(255,190,110,.95)");lg.addColorStop(.25,"rgba(255,150,60,.35)");lg.addColorStop(1,"rgba(255,140,50,0)");g.fillStyle=lg;g.fillRect(L[0]-rr*5,L[1]-rr*5,rr*10,rr*10);}
    g.globalAlpha=1;
    GLOW=false;g.globalAlpha=1;
    g.setTransform(DPR,0,0,DPR,0,0);var vg=g.createRadialGradient(W/2,H/2,H*.25,W/2,H/2,H*.95);vg.addColorStop(0,"rgba(0,0,0,0)");vg.addColorStop(1,"rgba(0,0,0,.6)");g.fillStyle=vg;g.fillRect(0,0,W,H);RAF(frame);
  }
  RAF(frame);
  return{resize:function(w,h){canvasRef.w=w;canvasRef.h=h;NEED_SIZE=true;},hidden:function(v){HIDDEN=v;}};}
  var api=null;
  function report(){var w=c.clientWidth,h=c.clientHeight;if(api)api.resize(w,h);else if(worker)worker.postMessage({type:"size",w:w,h:h});}
  function hiddenState(){var v=menu.hidden;if(api)api.hidden(v);else if(worker)worker.postMessage({type:"hidden",v:v});}
  var worker=null;
  try{
    if(c.transferControlToOffscreen&&window.Worker&&!/[?&]menuMain=1/.test(location.search)){
      var off=c.transferControlToOffscreen();
      var src="var FLY="+FLY.toString()+";var f=null,ref=null;onmessage=function(e){var d=e.data;if(d.type==='init'){ref={c:d.canvas,w:d.w,h:d.h};var RAF=self.requestAnimationFrame?self.requestAnimationFrame.bind(self):function(cb){return setTimeout(function(){cb(performance.now());},16);};f=FLY(d.canvas.getContext('2d'),ref,d.dpr,RAF);}else if(f&&d.type==='size')f.resize(d.w,d.h);else if(f&&d.type==='hidden')f.hidden(d.v);};";
      worker=new Worker(URL.createObjectURL(new Blob([src],{type:"text/javascript"})));
      worker.postMessage({type:"init",canvas:off,w:c.clientWidth,h:c.clientHeight,dpr:DPR},[off]);
      document.documentElement.dataset.menuFlyover="offscreen-worker";
    }
  }catch(e){worker=null;}
  if(!worker){api=FLY(c.getContext("2d"),{c:c,w:c.clientWidth,h:c.clientHeight},DPR,function(cb){return requestAnimationFrame(cb);});document.documentElement.dataset.menuFlyover="main-thread";}
  window.addEventListener("resize",report,{passive:true});
  new MutationObserver(hiddenState).observe(menu,{attributes:true,attributeFilter:["hidden"]});
})();
