(() => {
    // You can call AE scripting via csInterface if needed:
    const csInterface = new CSInterface();
  
    const canvas = document.getElementById('curveCanvas');
    const ctx = canvas.getContext('2d');
    const r = 6;
    const defaults = {
      p0:{x:50,y:380},p1:{x:150,y:80},
      p2:{x:210,y:80},p3:{x:310,y:380}
    };
    let pts = JSON.parse(JSON.stringify(defaults)), sel=null;
  
    function draw() {
      ctx.clearRect(0,0,canvas.width,canvas.height);
      // control polyline
      ctx.beginPath();
      ctx.moveTo(pts.p0.x,pts.p0.y);
      [pts.p1,pts.p2,pts.p3].forEach(p=>ctx.lineTo(p.x,p.y));
      ctx.strokeStyle='#aaa'; ctx.lineWidth=1; ctx.stroke();
      // bezier
      ctx.beginPath();
      ctx.moveTo(pts.p0.x,pts.p0.y);
      ctx.bezierCurveTo(pts.p1.x,pts.p1.y,pts.p2.x,pts.p2.y,pts.p3.x,pts.p3.y);
      ctx.strokeStyle='#0077fe'; ctx.lineWidth=3; ctx.stroke();
      // handles
      Object.values(pts).forEach(p=>{
        ctx.beginPath(); ctx.arc(p.x,p.y,r,0,2*Math.PI);
        ctx.fillStyle='#ff6600'; ctx.fill();
        ctx.strokeStyle='#333'; ctx.stroke();
      });
    }
  
    function pos(e) {
      const b=canvas.getBoundingClientRect();
      return {x:e.clientX-b.left, y:e.clientY-b.top};
    }
    canvas.addEventListener('mousedown',e=>{
      const m=pos(e);
      for(let k in pts) {
        const d=(m.x-pts[k].x)**2+(m.y-pts[k].y)**2;
        if(d<=r*r) { sel=k; break; }
      }
    });
    canvas.addEventListener('mousemove',e=>{
      if(!sel) return;
      pts[sel]=pos(e);
      draw();
    });
    ['mouseup','mouseleave'].forEach(evt=>
      canvas.addEventListener(evt,()=>sel=null)
    );
    document.getElementById('resetBtn').addEventListener('click',()=>{
      pts=JSON.parse(JSON.stringify(defaults)); draw();
    });
    window.addEventListener('DOMContentLoaded',draw);
  })();
  