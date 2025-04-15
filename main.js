(function() {
    'use strict';
  
    // Initialize CSInterface for communication with After Effects
    const csInterface = new CSInterface();
  
    /**
     * CurveEditor manages the drawing and interactive editing of a cubic Bézier curve.
     */
    class CurveEditor {
      constructor(canvasId) {
        this.canvas = document.getElementById(canvasId);
        if (!this.canvas) {
          console.error(`Canvas with id "${canvasId}" not found.`);
          return;
        }
        this.ctx = this.canvas.getContext("2d");
        this.pointRadius = 6;
  
        // Initialize default points for a cubic Bézier curve
        this.defaultPoints = {
          p0: { x: 50, y: this.canvas.height - 50 },
          p1: { x: 150, y: 50 },
          p2: { x: 210, y: 50 },
          p3: { x: this.canvas.width - 50, y: this.canvas.height - 50 }
        };
  
        // Clone defaultPoints into this.points
        this.points = JSON.parse(JSON.stringify(this.defaultPoints));
        this.selectedPoint = null;
  
        this.attachEvents();
        this.draw();
      }
  
      attachEvents() {
        this.canvas.addEventListener("mousedown", (e) => this.onMouseDown(e));
        this.canvas.addEventListener("mousemove", (e) => this.onMouseMove(e));
        // Listen for mouseup on the entire document to ensure proper release
        document.addEventListener("mouseup", () => this.onMouseUp());
        this.canvas.addEventListener("mouseleave", () => this.onMouseUp());
      }
  
      getMousePos(event) {
        const rect = this.canvas.getBoundingClientRect();
        return {
          x: event.clientX - rect.left,
          y: event.clientY - rect.top
        };
      }
  
      onMouseDown(event) {
        const pos = this.getMousePos(event);
        for (let key in this.points) {
          if (this.isInsidePoint(pos, this.points[key])) {
            this.selectedPoint = key;
            return;
          }
        }
      }
  
      onMouseMove(event) {
        if (!this.selectedPoint) return;
        const pos = this.getMousePos(event);
        this.points[this.selectedPoint] = { x: pos.x, y: pos.y };
        this.draw();
      }
  
      onMouseUp() {
        this.selectedPoint = null;
      }
  
      isInsidePoint(pos, pt) {
        const dx = pos.x - pt.x;
        const dy = pos.y - pt.y;
        return dx * dx + dy * dy <= this.pointRadius * this.pointRadius;
      }
  
      reset() {
        // Restore points to their default positions
        this.points = JSON.parse(JSON.stringify(this.defaultPoints));
        this.draw();
      }
  
      clearCanvas() {
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
      }
  
      draw() {
        this.clearCanvas();
  
        // Draw control lines connecting the points
        this.ctx.beginPath();
        this.ctx.moveTo(this.points.p0.x, this.points.p0.y);
        this.ctx.lineTo(this.points.p1.x, this.points.p1.y);
        this.ctx.lineTo(this.points.p2.x, this.points.p2.y);
        this.ctx.lineTo(this.points.p3.x, this.points.p3.y);
        this.ctx.lineWidth = 1;
        this.ctx.strokeStyle = "#aaa";
        this.ctx.stroke();
  
        // Draw the Bézier curve
        this.ctx.beginPath();
        this.ctx.moveTo(this.points.p0.x, this.points.p0.y);
        this.ctx.bezierCurveTo(
          this.points.p1.x, this.points.p1.y,
          this.points.p2.x, this.points.p2.y,
          this.points.p3.x, this.points.p3.y
        );
        this.ctx.lineWidth = 3;
        this.ctx.strokeStyle = "#0077fe";
        this.ctx.stroke();
  
        // Draw each control point
        for (let key in this.points) {
          this.drawPoint(this.points[key]);
        }
      }
  
      drawPoint(pt) {
        this.ctx.beginPath();
        this.ctx.arc(pt.x, pt.y, this.pointRadius, 0, Math.PI * 2);
        this.ctx.fillStyle = "#ff6600";
        this.ctx.fill();
        this.ctx.lineWidth = 1;
        this.ctx.strokeStyle = "#333";
        this.ctx.stroke();
      }
    }
  
    // Wait until the DOM is ready before initializing
    document.addEventListener("DOMContentLoaded", () => {
      const curveEditor = new CurveEditor("curveCanvas");
  
      // Reset button functionality
      const resetBtn = document.getElementById("resetBtn");
      if (resetBtn) {
        resetBtn.addEventListener("click", () => {
          curveEditor.reset();
        });
      }
  
      console.log("Crazy Curves Plugin Production Version Initialized");
    });
  })();
  