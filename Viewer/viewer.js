// *********************************************************************
// Global variables
// *********************************************************************

// Viewer state.
var gamePaused = true;
var gamePreview = false; // If true, render will be called for the next tick even if game is paused, and then will be set to false.
var gameAnim = true;
var gameDirection = 1;
var actRound = 0; // Current round index.

// Data.
var raw_data_str; // String for storing the raw data.
var dataLoaded = false; // Set to true when raw_data_str is ready to be parsed.
var data = {}; // Object for storing all the game data.

// Animation.
var speed = 20; // Ticks per second.
var FRAMES_PER_ROUND = 2;
var frames = 0; // Incremented each tick, when it reaches FRAMES_PER_ROUND, actRound is updated (acording to gameDirection).

// Visuals.
var unitSize = 0.6; // 1 = same size as tile.
var unitLineWidth = 2;
var grid_color = "#000000";

var cell_colors = {
  "0": "#ef9a9a",
  "1": "#9fa8da",
  "2": "#a5d6a7",
  "3": "#fff3b5",
  X: "#4a4b4e",
  R: "#000000",
  S: "#FF8000",
  W: "#32b7ff",
  ".": "#727872"
};

var player_colors = {
  0: "#ff1744",
  1: "#3d5afe",
  2: "#558b2f",
  3: "#ffb90a"
};

// *********************************************************************
// Utility functions
// *********************************************************************

function getURLParameter(name) {
  // http://stackoverflow.com/questions/1403888/get-url-parameter-with-jquery
  var a = (RegExp(name + "=" + "(.+?)(&|$)").exec(location.search) || [
    ,
    null
  ])[1];
  if (a != null) return decodeURI(a);
  return null;
}

// Callback has a single parameter with the file contents.
function loadFile(file, callback) {
  var xmlhttp;

  if (file == null || file == "") {
    alert("You must specify a file to load.");
    return;
  }

  if (window.XMLHttpRequest) xmlhttp = new XMLHttpRequest();
  // Code for IE7+, Firefox, Chrome, Opera, Safari.
  else xmlhttp = new ActiveXObject("Microsoft.XMLHTTP"); // Code for IE6, IE5.

  // http://www.w3schools.com/ajax/ajax_xmlhttprequest_onreadystatechange.asp
  xmlhttp.onreadystatechange = function() {
    // Note: We cannot check xmlhttp.status != 200 for errors because status is not set when loading local files.
    if (xmlhttp.readyState == 4) callback(xmlhttp.responseText);
  };

  xmlhttp.open("GET", file, false);
  xmlhttp.send();
}

function int(s) {
  return parseInt(s);
}

function double(s) {
  return parseFloat(s);
}

function parse_assert(read_value, expected_value) {
  var correct = read_value == expected_value;
  if (!correct)
    alert(
      "Error parsing file, expected token: " +
        expected_value +
        ", read token: " +
        read_value +
        "."
    );
  return correct;
}

// *********************************************************************
// Initialization functions
// *********************************************************************

function parseData(raw_data_str) {
  if (raw_data_str == "") {
    alert("Could not load game file.");
    return false;
  }

  // Convert text to tokens.
  var st = raw_data_str + "";
  var t = st.replace("\n", " ").split(/\s+/);
  var p = 0;

  data.secgame = t[p++] == "SecGame";

  parse_assert(t[p++], "Seed");
  data.seed = int(t[p++]);

  // Game and version.
  if (t[p++] != "Mad_Max") {
    alert("Are you sure this is a Mad Max game file?");
    document.getElementById("file").value = "";
    document.getElementById("inputdiv").style.display = "";
    document.getElementById("loadingdiv").style.display = "none";
    return false;
  }

  data.version = t[p++];
  if (data.version != "1.6")
    alert("Unsupported game version! Trying to load it anyway.");

  parse_assert(t[p++], "nb_players");
  data.nb_players = int(t[p++]);

  parse_assert(t[p++], "nb_rounds");
  data.nb_rounds = int(t[p++]);

  parse_assert(t[p++], "nb_cities");
  data.nb_cities = int(t[p++]);

  parse_assert(t[p++], "nb_warriors");
  data.nb_warriors = int(t[p++]);

  parse_assert(t[p++], "nb_cars");
  data.nb_cars = int(t[p++]);

  parse_assert(t[p++], "warriors_health");
  data.warriors_health = int(t[p++]);

  parse_assert(t[p++], "cars_fuel");
  data.cars_fuel = int(t[p++]);

  parse_assert(t[p++], "damage");
  data.damage = int(t[p++]);

  parse_assert(t[p++], "rows");
  data.rows = int(t[p++]);

  parse_assert(t[p++], "cols");
  data.cols = int(t[p++]);

  data.nb_units = data.nb_players * (data.nb_cars + data.nb_warriors);

  parse_assert(t[p++], "names");
  data.names = new Array();
  for (var i = 0; i < data.nb_players; ++i) data.names[i] = t[p++];

  data.rounds = new Array();
  for (var round = 0; round <= data.nb_rounds; ++round) {
    // Maze.
    data.rounds[round] = new Object();
    data.rounds[round].rows = new Array();
    for (var i = 0; i < data.rows; ++i) data.rounds[round].rows[i] = t[p++];

    parse_assert(t[p++], "round");
    if (int(t[p++]) != round) alert("Wrong round number!");

    // Num cities.
    parse_assert(t[p++], "num_cities");
    data.rounds[round].num_cities = new Array();
    for (var i = 0; i < data.nb_players; ++i)
      data.rounds[round].num_cities[i] = int(t[p++]);

    // Total score.
    parse_assert(t[p++], "total_score");
    data.rounds[round].total_score = new Array();
    for (var i = 0; i < data.nb_players; ++i)
      data.rounds[round].total_score[i] = int(t[p++]);

    // Status.
    parse_assert(t[p++], "status");
    data.rounds[round].cpu = new Array();
    for (var i = 0; i < data.nb_players; ++i) {
      var cpu = int(double(t[p++]) * 100);
      data.rounds[round].cpu[i] = cpu == -100 ? "out" : cpu + "%";
    }

    // Units.
    data.rounds[round].units = [];
    data.rounds[round].alive_warriors = { 0: 0, 1: 0, 2: 0, 3: 0 };
    data.rounds[round].alive_cars = { 0: 0, 1: 0, 2: 0, 3: 0 };
    for (var i = 0; i < data.nb_units; ++i) {
      var unit = {
        type: t[p++],
        player: int(t[p++]),
        i: int(t[p++]),
        j: int(t[p++]),
        food: int(t[p++]),
        water: int(t[p++]),
        move: "" // Will be set when reading movements.
      };

      data.rounds[round].units.push(unit);
      if (unit.type == "c") ++data.rounds[round].alive_cars[unit.player];
      else if (unit.type == "w")
        ++data.rounds[round].alive_warriors[unit.player];
    }

    if (round != data.nb_rounds) {
      // Movements.
      parse_assert(t[p++], "movements");
      var code = int(t[p++]);
      while (code != -1) {
        data.rounds[round].units[code].move = t[p++];
        code = int(t[p++]);
      }
    }
  }
  return true;
}

function parseDebug(debug_data) {
  var debugs = Array(500)
    .fill()
    .map(function() {
      return Array(60)
        .fill()
        .map(function() {
          return Array(60).fill();
        });
    });

  debug_data
    .split("round")
    .filter(function(round) {
      return round.length;
    })
    .forEach(function(roundData, round) {
      var lines = roundData
        .split("\n")
        .slice(1, -1)
        .join("\n")
        .split("####\n")
        .slice(0, -1);
      lines.forEach(function(line) {
        var v = line.split(" ");
        var i = parseInt(v[0]);
        var j = parseInt(v[1]);

        var message = v.slice(2).join(" ");

        debugs[round][i][j] = message;
      });
    });

  return debugs;
}

// Initializing the game.
function initGame(raw_data, debug_data) {
  document.getElementById("loadingdiv").style.display = "";

  // TODO: Next two calls could run concurrently.
  if (parseData(raw_data) === false) return;
  preloadImages();

  gamePaused = false;
  gamePreview = true;

  var debugs = debug_data ? parseDebug(debug_data) : null;

  // Canvas element.
  canvas = document.getElementById("myCanvas");

  canvas.onmousemove = function(e) {
    // important: correct mouse position:
    var rect = this.getBoundingClientRect(),
      x = e.clientX - rect.left,
      y = e.clientY - rect.top;

    var j = Math.max(0, Math.floor((60 * x) / (rect.right - rect.left)));
    var i = Math.max(0, Math.floor((60 * y) / (rect.bottom - rect.top)));

    var position = "(" + i + "," + j + ")\n";
    document.getElementById("debug").innerText =
      position +
      (debugs ? (debugs[actRound][i][j] ? debugs[actRound][i][j] : "") : "");
  };

  context = canvas.getContext("2d");

  // Prepare the slider.
  $("#slider").slider({
    slide: function(event, ui) {
      var value = $("#slider").slider("option", "value");
      actRound = value;
      frames = 0;
      gamePaused = true;
      gamePreview = true;
    }
  });
  $("#slider").width(600);
  $("#slider").slider("option", { min: 0, max: data.nb_rounds });

  // Set the listerners for interaction.
  document.addEventListener("mousewheel", onDocumentMouseWheel, false);
  document.addEventListener("keydown", onDocumentKeyDown, false);
  document.addEventListener("keyup", onDocumentKeyUp, false);

  window.addEventListener("resize", onWindowResize, false);
  onWindowResize();

  document.getElementById("loadingdiv").style.display = "none";
  document.getElementById("gamediv").style.display = "";

  mainloop();
}

function preloadImages() {
  data.img = new Array();
}

// *********************************************************************
// Main loop functions
// *********************************************************************

function updateGame() {
  $("#slider").slider("option", "value", actRound);
}

function writeGameState() {
  // Write round.
  $("#round").html("Round: " + actRound);

  // Update scoreboard.
  var scoreboard = "";
  for (var i = 0; i < data.nb_players; ++i) {
    scoreboard +=
      "<span class='score'>" +
      "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:" +
      cell_colors[i] +
      "'></div>" +
      "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>" +
      data.names[i] +
      "</div>" +
      "<br/>" +
      "<div style='margin-left: 10px;'>" +
      "<div style='padding:2px;'>Cities: " +
      data.rounds[actRound].num_cities[i] +
      "</div>" +
      "<div style='padding:2px;'>Warriors: " +
      data.rounds[actRound].alive_warriors[i] +
      "</div>" +
      "<div style='padding:2px;'>Cars: " +
      data.rounds[actRound].alive_cars[i] +
      "</div>" +
      (data.secgame
        ? "<div style='padding:2px;'>CPU: " +
          data.rounds[actRound].cpu[i] +
          "</div>"
        : "") +
      "</div>" +
      "</span><br/><br/>";
  }
  $("#scores").html(scoreboard);

  var order = [0, 1, 2, 3];
  for (var i = 0; i < 3; ++i) {
    for (var j = i + 1; j < 4; ++j) {
      if (
        data.rounds[actRound].total_score[order[i]] <
        data.rounds[actRound].total_score[order[j]]
      ) {
        var k = order[i];
        order[i] = order[j];
        order[j] = k;
      }
    }
  }

  var totalboard = "";
  for (var i = 0; i < data.nb_players; ++i) {
    totalboard +=
      "<span class='total'>" +
      "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:" +
      cell_colors[order[i]] +
      "'></div>" +
      "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>" +
      data.rounds[actRound].total_score[order[i]] +
      "</div>" +
      "</span><br/><br/>";
  }
  $("#totals").html(totalboard);
}

function drawGame() {
  // Boundary check.
  if (actRound < 0) actRound = 0;
  if (actRound >= data.nb_rounds) actRound = data.nb_rounds;

  // Outter Rectangle.
  context.fillStyle = grid_color;
  context.fillRect(0, 0, tileSize * data.cols, tileSize * data.rows);

  // Draw maze.
  var rows = data.rounds[actRound].rows;
  for (var i = 0; i < data.rows; ++i) {
    var row = rows[i];
    for (var j = 0; j < data.cols; ++j) {
      var cell = row[j];
      if (cell == "S") drawStation(i, j, "#FFFFFF");
      else if (cell == "X") {
        var my_gradient = context.createLinearGradient(
          j * tileSize,
          i * tileSize,
          (j + 1) * tileSize,
          (i + 1) * tileSize
        );
        my_gradient.addColorStop(0, "grey");
        my_gradient.addColorStop(1, "white");
        context.fillStyle = my_gradient;
        var amplada = tileSize - 0.5;
        var incx = 0;
        if (row[j - 1] != "X") incx = 1;
        var decx = 0;
        if (row[j + 1] != "X") decx = 1;
        var incy = 0;
        if (rows[i - 1][j] != "X") incy = 1;
        var decy = 0;
        if (rows[i + 1][j] != "X") decy = 1;
        context.fillRect(
          j * tileSize + incx,
          i * tileSize + incy,
          amplada - incx - decx,
          amplada - incy - decy
        );
      } else {
        context.fillStyle = cell_colors[cell];
        var amplada = tileSize - 0.5;
        if (cell >= "0" && cell <= "3") amplada = tileSize - 1.5; // to show a grid
        context.fillRect(j * tileSize, i * tileSize, amplada, amplada);
      }
    }
  }

  // Draw units.
  context.lineWidth = unitLineWidth;
  var units = data.rounds[actRound].units;
  for (var un in units) {
    var u = units[un];
    context.strokeStyle = player_colors[u.player];
    context.fillStyle = player_colors[u.player];
    var i = u.i;
    var j = u.j;

    if (gameAnim) {
      if (frames >= FRAMES_PER_ROUND / 2) {
        if (u.move == "b") i += 0.5;
        else if (u.move == "w") {
          i += 0.5;
          j += 0.5;
        } else if (u.move == "r") j += 0.5;
        else if (u.move == "x") {
          i -= 0.5;
          j += 0.5;
        } else if (u.move == "t") i -= 0.5;
        else if (u.move == "y") {
          i -= 0.5;
          j -= 0.5;
        } else if (u.move == "l") j -= 0.5;
        else if (u.move == "z") {
          i += 0.5;
          j -= 0.5;
        }
      }
    }

    if (u.type == "c") drawCar(i, j, u, rows);
    else drawWarrior(i, j, u, rows);
  }

  if (gameAnim && frames >= FRAMES_PER_ROUND / 2) {
    for (var i = 0; i < data.rows; ++i) {
      var row = rows[i];
      for (var j = 0; j < data.cols; ++j) {
        if (row[j] == "S") drawStation(i, j, "#FF0000");
      }
    }
  }
}

function drawStation(i, j, col) {
  context.strokeStyle = cell_colors["S"];
  context.fillStyle = col;
  var size = unitSize * tileSize * 1.2;
  var offset = (tileSize - size) / 2;
  context.beginPath();
  context.arc(
    j * tileSize + size / 2 + offset,
    i * tileSize + size / 2 + offset,
    size / 2,
    0,
    Math.PI * 2,
    false
  );
  context.fill();
  context.stroke();
}

function drawCar(i, j, u, c) {
  var size = unitSize * tileSize * 0.6 + 6;
  var offset = (tileSize - size) / 2;
  //context.beginPath();
  //context.arc(j*tileSize + size/2 + offset, i*tileSize + size/2 + offset, size/2, 0, Math.PI*2, false);
  //context.fill();
  //context.stroke();
  context.beginPath();
  context.moveTo(
    j * tileSize + offset - 0.4 * size,
    i * tileSize + offset - 0.4 * size
  );
  context.lineTo(
    j * tileSize + offset + 1.4 * size,
    i * tileSize + offset + 1.4 * size
  );
  context.lineWidth = 5;
  context.stroke();
  context.beginPath();
  context.moveTo(
    j * tileSize + offset + 1.4 * size,
    i * tileSize + offset - 0.4 * size
  );
  context.lineTo(
    j * tileSize + offset - 0.4 * size,
    i * tileSize + offset + 1.4 * size
  );
  context.lineWidth = 5;
  context.stroke();
  context.beginPath();
  context.strokeStyle =
    c[i] && c[i][j + 1]
      ? c[i][j + 1] == "R"
        ? "#aeaeae"
        : "black"
      : "#aeaeae";
  context.lineWidth = 1;
  context.font = "12px Verdana";
  context.strokeText(
    u.food,
    j * tileSize + size / 2 + offset + 10,
    i * tileSize + size / 2 + offset
  );
  context.stroke();
}

function drawWarrior(i, j, u, c) {
  var size = unitSize * tileSize * 0.4 + (u.water / 40) * 10;
  var offset = (tileSize - size) / 2;
  context.beginPath();
  context.arc(
    j * tileSize + size / 2 + offset,
    i * tileSize + size / 2 + offset,
    size / 2,
    0,
    Math.PI * 4,
    false
  );
  context.fill();
  context.stroke();
  context.beginPath();
  context.strokeStyle =
    c[i] && c[i][j + 1]
      ? c[i][j + 1] == "R"
        ? "#aeaeae"
        : "black"
      : "#aeaeae";
  context.font = "12px Verdana";
  context.lineWidth = 1;
  context.strokeText(
    u.food,
    j * tileSize + size / 2 + offset + 10,
    i * tileSize + size / 2 + offset - 5
  );
  context.stroke();
  context.beginPath();
  context.strokeStyle =
    c[i] && c[i][j + 1]
      ? c[i][j + 1] == "R"
        ? "#aeaeae"
        : "black"
      : "#aeaeae";
  context.font = "12px Verdana";
  context.lineWidth = 1;
  context.strokeText(
    u.water,
    j * tileSize + size / 2 + offset + 10,
    i * tileSize + size / 2 + offset + 10
  );
  context.stroke();
}

// *********************************************************************
// Button events
// *********************************************************************

function playButton() {
  gamePaused = false;
}

function pauseButton() {
  gamePaused = true;
  gamePreview = true; // To call render again.
  frames = 0;
}

function startButton() {
  gamePaused = true;
  gamePreview = true;
  frames = 0;
  actRound = 0;
}

function endButton() {
  gamePreview = true;
  frames = 0;
  actRound = data.nb_rounds;
}

function animButton() {
  gameAnim = !gameAnim;
}

function closeButton() {
  window.close();
}

// *********************************************************************
// Keyboard and Mouse events
// *********************************************************************

function onDocumentMouseWheel(event) {}

function onDocumentKeyDown(event) {}

function onDocumentKeyUp(event) {
  // http://www.webonweboff.com/tips/js/event_key_codes.aspx
  switch (event.keyCode) {
    case 36: // Start.
      gamePreview = true;
      actRound = 0;
      frames = 0;
      break;

    case 35: // End.
      gamePreview = true;
      actRound = data.nb_rounds;
      frames = 0;
      break;

    case 33: // PageDown.
      gamePreview = true;
      actRound -= 10;
      frames = 0;
      break;

    case 34: // PageUp.
      gamePreview = true;
      actRound += 10;
      frames = 0;
      break;

    case 38: // ArrowUp.
    case 37: // ArrowLeft.
      gamePaused = true;
      gamePreview = true;
      --actRound;
      frames = 0;
      break;

    case 40: // ArrowDown.
    case 39: // ArrowRight.
      gamePaused = true;
      gamePreview = true;
      ++actRound;
      frames = 0;
      break;

    case 32: // Space.
      if (gamePaused) playButton();
      else pauseButton();
      break;

    case 72: // "h"
      help();
      break;

    default:
      // $("#debug").html(event.keyCode);
      break;
  }
}

function onWindowResize(event) {
  // Constants.
  var header_height = 150;
  var canvas_margin = 20;

  // Set canvas size.
  var size =
    Math.min(
      document.body.offsetWidth,
      document.body.offsetHeight - header_height
    ) -
    canvas_margin * 2;

  canvas.width = size;
  canvas.height = size;

  var max_dimension = Math.max(data.cols, data.rows);
  tileSize = size / max_dimension;

  drawGame();
}

function help() {
  // Opens a new popup with the help page.
  var win = window.open("help.html", "name", "height=400, width=300");
  if (window.focus) win.focus();
  return false;
}

// *********************************************************************
// This function is called periodically.
// *********************************************************************

function mainloop() {
  // Configure buttons.
  if (gamePaused) {
    $("#but_play").show();
    $("#but_pause").hide();
  } else {
    $("#but_play").hide();
    $("#but_pause").show();
  }

  if (actRound < 0) actRound = 0;

  if (actRound > data.nb_rounds) {
    actRound = data.nb_rounds;
    gamePaused = true;
    frames = 0;
  }

  if (!gamePaused || gamePreview) {
    updateGame();
    drawGame();
    writeGameState();

    if (gamePreview) {
      frames = 0;
      gamePreview = false;
    } else {
      ++frames;
      if (frames == FRAMES_PER_ROUND) {
        frames = 0;
        actRound += gameDirection;
      }
    }
  }

  // Periodically call mainloop.
  var frame_time = 1000 / speed;
  setTimeout(mainloop, frame_time);
}

// *********************************************************************
// Main function, it is called when the document is ready.
// *********************************************************************

function init() {
  // Get url parameters.
  var game;
  if (getURLParameter("sub") != null) {
    var domain = window.location.protocol + "//" + window.location.host;
    if (getURLParameter("nbr") != null)
      game =
        domain +
        "/?cmd=lliuraments&sub=" +
        getURLParameter("sub") +
        "&nbr=" +
        getURLParameter("nbr") +
        "&download=partida";
    else
      game =
        domain +
        "/?cmd=partida&sub=" +
        getURLParameter("sub") +
        "&download=partida";
  } else game = getURLParameter("game");

  if (game == null || game == "") {
    // Ask the user for a game input.
    var inputdiv = document.getElementById("inputdiv");
    inputdiv.style.display = "";
    document.getElementById("file").addEventListener(
      "change",
      function(evt) {
        //http://www.html5rocks.com/en/tutorials/file/dndfiles/
        var file = evt.target.files[0];
        var reader = new FileReader();
        reader.readAsText(file);
        reader.onloadend = function(evt) {
          if (evt.target.readyState != FileReader.DONE)
            alert("Error accessing file.");
          else {
            // DONE == 2.
            inputdiv.style.display = "none";
            document.getElementById("loadingdiv").style.display = "";
            initGame(reader.result);
          }
        };
      },
      false
    );
  } else {
    document.getElementById("loadingdiv").style.display = "";
    // Load the given game.
    loadFile(game, function(text) {
      loadFile("/debug.res", function(debugText) {
        initGame(text, debugText);
      });
    });
  }
}
