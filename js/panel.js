$(function () {
    $('.tabs button').click(function () {
      const tab = $(this).data('tab');
      $('.tab-content').removeClass('active');
      $('#' + tab).addClass('active');
    });
  });
  
  function applyEase() {
    const amount = parseInt(document.getElementById('easeAmount').value);
    const script = `
      var easeAmt = ${amount} / 100;
      var comp = app.project.activeItem;
      if (comp && comp.selectedProperties.length > 0) {
        app.beginUndoGroup("Apply Ease");
        for (var i = 0; i < comp.selectedProperties.length; i++) {
          var prop = comp.selectedProperties[i];
          if (prop.isTimeVarying) {
            for (var j = 1; j <= prop.numKeys; j++) {
              prop.setTemporalEaseAtKey(j, [new KeyframeEase(easeAmt * 33, 0)], [new KeyframeEase(easeAmt * 33, 0)]);
            }
          }
        }
        app.endUndoGroup();
      }
    `;
    new CSInterface().evalScript(script);
  }
  
  function applyElastic() {
    const amp = parseFloat(document.getElementById('elasticAmplitude').value);
    const freq = parseFloat(document.getElementById('elasticFrequency').value);
    const decay = parseFloat(document.getElementById('elasticDecay').value);
    const script = `
      var amp = ${amp};
      var freq = ${freq};
      var decay = ${decay};
      var comp = app.project.activeItem;
      if (comp && comp.selectedProperties.length > 0) {
        app.beginUndoGroup("Apply Elastic");
        for (var i = 0; i < comp.selectedProperties.length; i++) {
          var prop = comp.selectedProperties[i];
          if (prop.isTimeVarying) {
            prop.expression = "e = amp * Math.sin(freq*time*2*Math.PI) / Math.exp(decay*time); value + e";
          }
        }
        app.endUndoGroup();
      }
    `;
    new CSInterface().evalScript(script);
  }
  
  function applyBounce() {
    const height = parseFloat(document.getElementById('bounceHeight').value);
    const speed = parseFloat(document.getElementById('bounceSpeed').value);
    const decay = parseFloat(document.getElementById('bounceDecay').value);
    const script = `
      var h = ${height};
      var s = ${speed};
      var d = ${decay};
      var comp = app.project.activeItem;
      if (comp && comp.selectedProperties.length > 0) {
        app.beginUndoGroup("Apply Bounce");
        for (var i = 0; i < comp.selectedProperties.length; i++) {
          var prop = comp.selectedProperties[i];
          if (prop.isTimeVarying) {
            prop.expression = "t = time - key(1).time; e = Math.abs(Math.sin(t*s)*h / Math.exp(d*t)); value + e";
          }
        }
        app.endUndoGroup();
      }
    `;
    new CSInterface().evalScript(script);
  }
  
  function applyDistribute() {
    const frames = parseInt(document.getElementById('distributeFrames').value);
    const script = `
      var f = ${frames};
      var comp = app.project.activeItem;
      if (comp && comp.selectedLayers.length > 0) {
        app.beginUndoGroup("Distribute Layers");
        for (var i = 0; i < comp.selectedLayers.length; i++) {
          comp.selectedLayers[i].startTime = i * (f / comp.frameRate);
        }
        app.endUndoGroup();
      }
    `;
    new CSInterface().evalScript(script);
  }
  