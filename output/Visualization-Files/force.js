// Force.js V0.9  17 Sep 2017



// SVG
var svg = d3.select("svg"),
  width = +svg.attr("width"),
  height = +svg.attr("height"),
  radius = 150; // boundaries



// RECT & GROUP
var rect = svg.append("rect")
  .attr("width", width)
  .attr("height", height)
  .on("click", releaseHighlight)
  .on("mouseout", showInfo),

  g = svg.append("g");



// ZOOM
var zoomState = true,
  zoomExtent = 3,
  zoomScale,
  width_LT = -(zoomExtent - 1) / 2 * width + radius,
  height_LT = -(zoomExtent - 1) / 2 * height + radius,
  width_RD = (zoomExtent + 1) / 2 * width - radius,
  height_RD = (zoomExtent + 1) / 2 * height - radius,

  zoom = d3.zoom()
  .scaleExtent([1 / zoomExtent, zoomExtent])
  .translateExtent([
    [width_LT - radius, height_LT - radius],
    [width_RD + radius, height_RD + radius]
  ])
  .on("zoom", redrawOnZoom);

if (zoomState) {
  document.getElementById("zoomButton").classList.add("blue-active");
  rect.call(zoom);
}



// FISHEYE
var fisheyeState = false,
  fisheye = d3.fisheye.circular().radius(height / 5);



// HIGHLIGHT
var highlightState = false,
  LinkMap = {};



// COLORS
var fill = {
  1: '#d9d9d9' // main graph color
};



// DEFALUT VALUES
var nNodeCharge = -40,
  nLinkStr = 0.4,
  nNodeRad = 5,
  nLinkWid = 1,
  nVelDecay = 0.25,
  nCenterGrav = 0,
  nAlphaRestart = 0.4,
  forceState = true,
  pinState = false;

var info = {
  zoomButton: {
    title: "Zoom Function",
    def: `Zooming is performed by either by scrolling the mouse-wheel or double clicking on an empty part of the SVG. To Zoom out, hold SHIFT when double-clicking. You may click-and-drag the SVG to pan (translate).\nZoom scale: [1 \\ ${zoomExtent},  ${zoomExtent}]`
  },
  fisheyeButton: {
    title: "FishEye Distortion",
    def: `Magnifies the local region around the mouse, while leaving the larger graph unaffected for context. This type of distortion is particularly useful for disambiguating edge-crossings in static network layouts: edges between distant nodes are distorted more strongly than local ones.`
  },
  pinButton: {
    title: "Pin On Drop",
    def: `Drag and drop a node to have it pinned down. This prevents the force layout from subsequently changing the position of the node. Hold SHIFT and click to release a node.`
  },
  forceLabel: {
    title: "Force/Static Switch",
    def: `Switch between the Force and Static layouts. In the Static layout, simulation is stopped but the drag-and-drop functionality would still work.`
  },
  highlight: {
    title: "Highlighting",
    def: `By double clicking on a node, it's links and it's neighbours will be highlighted by fading out all other nodes. To release, double click again or click on an empty part of the SVG.`
  },
  chargeBlock: {
    title: "Charge Strength",
    def: `A positive value causes nodes to attract each other, similar to gravity, while a negative value causes nodes to repel each other, similar to electrostatic charge.`
  },
  linkStrBlock: {
    title: "Link Strength",
    def: `This property specifies the strength of the link between any two connected nodes. The distance of links vary due to stronger forces around heavily-connected nodes or layout's attempt to prevent link crossings or node collisions.`
  },
  veloDecayBlock: {
    title: "Velocity Decay",
    def: `The decay factor is akin to atmospheric friction; after the application of any forces during a tick, each node’s velocity is multiplied by <span style="font-style: italic;">1 - decay</span>.`
  },
  centerGravityBlock: {
    title: "Center Gravity",
    def: `Configures the Center Gravity force's strength. The force pushes nodes towards the center of the layout. The strength of the force is proportional to the one-dimensional distance between the node’s position and the center.`
  },
  about: {
    title: "About",
    def: `This visualization tool is part of the "Motifs In Biological Networks" project implementation.\n\n<span style="font-weight: bold;">Copyright © 2017 Nima Sarang</span>`
  }
};


// CALLBACKS
d3.select("#chargeINPUT")
  .on("input", nodeCharge)
d3.select("#linkStrINPUT")
  .on("input", linkStr);
d3.select("#nodeRadINPUT")
  .on("input", nodeRad);
d3.select("#linkWidthINPUT")
  .on("input", linkWidth);
d3.select("#veloDecayINPUT")
  .on("input", veloDecay);
d3.select("#centerGravityINPUT")
  .on("input", centerGravity);
d3.select("#zoomButton")
  .on("click", toggleZoom)
  .on("mouseover mouseout", showInfo);
d3.select("#fisheyeButton")
  .on("click", toggleFisheye)
  .on("mouseover mouseout", showInfo);
d3.select("#pinButton")
  .on("click", togglePin)
  .on("mouseover mouseout", showInfo);
d3.select("#toggleForce")
  .on("click", toggleForce);
d3.select("#forceLabel")
  .on("mouseover mouseout", showInfo);
d3.select("#forceHandle")
  .on("mouseover", showInfo);
d3.select("#chargeBlock")
  .on("mouseover mouseout", showInfo);
d3.select("#linkStrBlock")
  .on("mouseover mouseout", showInfo);
d3.select("#veloDecayBlock")
  .on("mouseover mouseout", showInfo);
d3.select("#centerGravityBlock")
  .on("mouseover mouseout", showInfo);


// READ DATA
var graph, link, node, simulation;

d3.json("motif.json", function (error, read) {
  if (error) throw error;
  graph = read;

  d3.select("#graphOrder").text(graph.specs.graphOrder);
  d3.select("#graphSize").text(graph.specs.graphSize);
  d3.select("#graphComps").text(graph.specs.graphComps);
  d3.select("#patternOrder").text(graph.specs.patternOrder);
  d3.select("#patternSize").text(graph.specs.patternSize);
  d3.select("#patternFreq").text(graph.specs.patternFreq);

  Visualize();
});



// SIMULATION INIT
function Visualize() {
  simulation = d3.forceSimulation()
    .velocityDecay(nVelDecay) // Friction
    // .alphaMin(0.01)
    .force("link", d3.forceLink().id(function (d) {
      return d.id;
    }).strength(nLinkWid))
    .force("charge", d3.forceManyBody().strength(nNodeCharge))
    .force("center", d3.forceCenter(width / 2, height / 2))
    .force("x", d3.forceX(width / 2).strength(nCenterGrav))
    .force("y", d3.forceY(height / 2).strength(nCenterGrav));
  // .force("collide", d3.forceCollide().radius(function(d) { return nNodeRad + 0.5; }).iterations(2));


  link = g.append("g")
    .attr("class", "links")
    .selectAll("line")
    .data(graph.links)
    .enter().append("line")
    .style("stroke", function (d) {
      return getColor(d.value);
    })

  node = g.append("g")
    .attr("class", "nodes")
    .selectAll("circle")
    .data(graph.nodes)
    .enter().append("circle")
    .attr("r", nNodeRad)
    .attr("fill", function (d) {
      return getColor(d.color);
    })
    .on("dblclick", toggleHighlight)
    .on("click", releasePin)
    .on("mouseover", showInfo)
    .call(d3.drag()
      .on("start", dragstarted)
      .on("drag", dragged)
      .on("end", dragended));


  node.append("title")
    .text(function (d) {
      return d.id;
    });

  simulation
    .nodes(graph.nodes)
    .force("link").links(graph.links);

  // smooth initialization
  var coef = Math.pow(graph.specs.graphSize, 1 / 3);

  requestAnimationFrame(function render() {
    for (var i = 0; i < coef * simulation.alpha(); ++i) {
      simulation.tick();
    }

    force_tick();

    if (simulation.alpha() > 0.1) {
      requestAnimationFrame(render);
    } else {
      simulation
        .on("tick", force_tick);
    }
  })

}



// FUNCTIONS
function force_tick() {
  link
    .attr("x1", function (d) {
      return d.source.x;
    })
    .attr("y1", function (d) {
      return d.source.y;
    })
    .attr("x2", function (d) {
      return d.target.x;
    })
    .attr("y2", function (d) {
      return d.target.y;
    });

  node
    .attr("cx", function (d) {
      return d.x = Math.max(width_LT, Math.min(width_RD, d.x));
    })
    .attr("cy", function (d) {
      return d.y = Math.max(height_LT, Math.min(height_RD, d.y));
    })
}

function fisheye_distortion() {
  fisheye.focus(d3.zoomTransform(this).invert(d3.mouse(this)));
  var r = parseFloat(d3.select("#nodeRadVALUE").text());

  node
    .each(function (d) {
      d.fisheye = fisheye(d);
    })
    .attr("cx", function (d) {
      return d.fisheye.x;
    })
    .attr("cy", function (d) {
      return d.fisheye.y;
    })
    .attr("r", function (d) {
      return d.fisheye.z * r;
    });

  link
    .attr("x1", function (d) {
      return d.source.fisheye.x;
    })
    .attr("y1", function (d) {
      return d.source.fisheye.y;
    })
    .attr("x2", function (d) {
      return d.target.fisheye.x;
    })
    .attr("y2", function (d) {
      return d.target.fisheye.y;
    });
};


function dragstarted(d) {
  if (!forceState)
    return;
  if (!d3.event.active)
    simulation.alphaTarget(0.3).restart();
  d.fx = d.x;
  d.fy = d.y;
}

function dragged(d) {
  if (forceState) {
    d.fx = d3.event.x;
    d.fy = d3.event.y;
  } else {
    d.x = d3.event.x;
    d.y = d3.event.y;
    force_tick();
  }
}

function dragended(d) {
  if (pinState) {
    d.fx = d.x;
    d.fy = d.y;
  } else {
    d.fx = null;
    d.fy = null;
  }
  if (!d3.event.active & forceState)
    simulation.alphaTarget(0);
}

function linkStr() {
  d3.select("#linkStrVALUE").text(parseFloat(+this.value).toFixed(2));
  simulation.force("link").strength(+this.value);
  if (forceState)
    simulation.alpha(nAlphaRestart).restart();
}

function nodeCharge() {
  d3.select("#chargeVALUE").text(+this.value);
  simulation.force("charge").strength(+this.value);
  if (forceState)
    simulation.alpha(nAlphaRestart).restart();
}

function nodeRad() {
  d3.select("#nodeRadVALUE").text(+this.value);
  g.selectAll('circle').transition().duration(200).attr('r', +this.value);
}

function linkWidth() {
  d3.select("#linkWidthVALUE").text(+this.value);
  g.selectAll('line').transition().duration(200).style('stroke-width', +this.value);
}

function veloDecay() {
  d3.select("#veloDecayVALUE").text(+this.value);
  simulation.velocityDecay(+this.value);
}

function centerGravity() {
  d3.select("#centerGravityVALUE").text(+this.value);
  simulation.force("x").strength((+this.value));
  simulation.force("y").strength((+this.value));
  if (forceState)
    simulation.alpha(nAlphaRestart).restart();
}

function redrawOnZoom() {
  zoomScale = [d3.event.transform.x, d3.event.transform.y, d3.event.transform.k];
  g.attr("transform", d3.event.transform);
}

function toggleZoom() {
  if (zoomState) {
    this.classList.remove('blue-active');
    rect.on('.zoom', null);
    rect.transition().duration(750).call(zoom.transform, d3.zoomIdentity);
  } else {
    this.classList.add('blue-active');
    rect.call(zoom);
  }
  zoomState = !zoomState;
}

function toggleFisheye() {
  if (fisheyeState) {
    this.classList.remove('black-active');
    rect.on("mousemove", null);
  } else {
    this.classList.add('black-active');
    rect.on("mousemove", fisheye_distortion);
    if (forceState)
      document.getElementById("forceLabel").click();
  }
  fisheyeState = !fisheyeState;
}

function togglePin() {
  if (pinState) {
    this.classList.remove('violet-active');
  } else {
    this.classList.add('violet-active');
  }
  pinState = !pinState;
}

function releasePin(d) {
  if (d3.event.shiftKey) {
    d.fx = null;
    d.fy = null;
    if (forceState)
      simulation.alpha(0.2).restart();
  }
}

function toggleForce() {
  if (forceState)
    simulation.stop();
  else {
    simulation.alpha(nAlphaRestart).restart();
    if (fisheyeState)
      document.getElementById("fisheyeButton").click();
  }
  forceState = !forceState;
}

function toggleHighlight() {
  if (Object.keys(LinkMap).length == 0) {
    for (let link of graph.links)
      LinkMap[link.source.index + "#" + link.target.index] = 1;

    for (let node of graph.nodes)
      LinkMap[node.index + "#" + node.index] = 1;
  }

  if (!highlightState) {
    //Reduce the opacity of all but the neighbouring nodes
    d = d3.select(this).node().__data__;
    node.style("opacity", function (o) {
      return LinkMap[d.index + "#" + o.index] | LinkMap[o.index + "#" + d.index] ? 1 : 0.1;
    });
    link.style("opacity", function (o) {
      return d.index == o.source.index | d.index == o.target.index ? 1 : 0.1;
    });
  } else {
    node.style("opacity", 1);
    link.style("opacity", 1);
  }

  highlightState = !highlightState;
}

function releaseHighlight() {
  if (highlightState)
    toggleHighlight();
}

function getColor(d) {
  if (!(d in fill))
    return fill[d] = getRandomColor();
  else
    return fill[d];
}

function getRandomColor() {
  var letters = '0123456789ABCDEF';
  var color = '#';
  for (var i = 0; i < 6; i++) {
    color += letters[Math.floor(Math.random() * 16)];
  }
  return color;
}

function showInfo() {
  var id = (d3.event.type == "mouseout") ? "about" :
    (this.tagName == "circle") ? "highlight" :
    (this.id == "forceHandle") ? "forceLabel" : this.id;

  if (showInfo.id == id)
    return;

  showInfo.id = id;

  d3.select("#infoTitle").text(info[id].title);
  d3.select("#infoDef").html(info[id].def);
}