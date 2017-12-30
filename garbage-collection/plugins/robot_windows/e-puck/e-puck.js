window.onload = function() {
  window.robotWindow = webots.window("e-puck");
  window.robotWindow.receive = function(value, robot) {
    if (value.indexOf("configure ") === 0) {
      try {
        var configure = JSON.parse(value.substring(10));
      } catch(e) {
        console.log(e);
        console.log("In: " + value);
      }
      robotLayout(configure);
    } else if (value.indexOf("ports") === 0) {
      updateDropDownMenu(document.getElementById("mode"), value);
      updateDropDownMenu(document.getElementById("upload"), value);
    } else { // sensor values
      var values = value.split(" ");
      document.getElementById("ps0").innerHTML = values[0];
      document.getElementById("ps1").innerHTML = values[1];
      document.getElementById("ps2").innerHTML = values[2];
      document.getElementById("ps3").innerHTML = values[3];
      document.getElementById("ps4").innerHTML = values[4];
      document.getElementById("ps5").innerHTML = values[5];
      document.getElementById("ps6").innerHTML = values[6];
      document.getElementById("ps7").innerHTML = values[7];
      document.getElementById("ls0").innerHTML = values[8];
      document.getElementById("ls1").innerHTML = values[9];
      document.getElementById("ls2").innerHTML = values[10];
      document.getElementById("ls3").innerHTML = values[11];
      document.getElementById("ls4").innerHTML = values[12];
      document.getElementById("ls5").innerHTML = values[13];
      document.getElementById("ls6").innerHTML = values[14];
      document.getElementById("ls7").innerHTML = values[15];
      document.getElementById("left speed").innerHTML = values[16];
      document.getElementById("right speed").innerHTML = values[17];
      document.getElementById("left encoder").innerHTML = values[18];
      document.getElementById("right encoder").innerHTML = values[19];
      document.getElementById("accelerometer x").innerHTML = values[20];
      document.getElementById("accelerometer y").innerHTML = values[21];
      document.getElementById("accelerometer z").innerHTML = values[22];
      document.getElementById("camera").src = values[23] + "#" + new Date().getTime();
      if (values.length > 24) {
        // optional ground sensors available
        setGroundSensorValue("gs0", values[24]);
        setGroundSensorValue("gs1", values[25]);
        setGroundSensorValue("gs2", values[26]);
      }
    }
  }
  window.robotWindow.send("configure");
  document.getElementById("file-selector").onchange = function(e) {
    var filename = e.target.files[0].name;
    if (filename.lastIndexOf(".hex") != filename.length - 4) {
      console.log(filename + ": Unsupported file type, should end with a .hex prefix");
      return;
    }
    var reader = new FileReader();
    reader.readAsText(e.target.files[0]);
    reader.onloadend = function(event) {
      window.robotWindow.send("upload " + selected_port + " " + event.target.result);
    }
  }
  onchange="onFileUpload();"
}

function setGroundSensorValue(id, valueString) {
  var value = parseInt(valueString);
  var elem = document.getElementById(id);
  var elemLabel = document.getElementById(id + " label");
  if (value < 0) {
    // show label
    elem.style.backgroundColor = "inherit";
    elem.style.borderWidth = "0px";
    elemLabel.innerHTML = id;
  } else {
    // show value
    var hexString = value.toString(16);
    elem.style.backgroundColor = "#" + hexString + hexString + hexString;
    elem.style.borderWidth = "1px";
    elemLabel.innerHTML = "";
  }
}

function updateDropDownMenu(menu, value) {
  var values = value.split(" ");
  while (menu.childNodes.length)
    menu.removeChild(menu.firstChild);
  for(var i = 1; i < values.length; i++) {
    var v = values[i];
    if (v.indexOf("\\\\.\\") == 0)
      v = v.substring(4);
    var a = document.createElement("A");
    a.innerHTML = v;
    a.href = '#';
    a.setAttribute("name", values[i]);
    menu.appendChild(a);
  }
  var a = document.createElement("A");
  a.innerHTML = "Refresh";
  a.href = "#";
  a.setAttribute("name", "refresh");
  menu.appendChild(a);
}

// Close the dropdown if the user clicks outside of it
var selected_port = "simulation";

window.onclick = function(event) {
  if (!event.target.classList.contains('dropdown-button'))
    hideAllDropDownMenus();
  if (event.target.nodeName == 'A') {
    var menu = event.target.parentNode.id;
    var action = event.target.getAttribute("name");
    if (action != 'refresh') {
      if (menu == 'mode') {
        var button = document.getElementById("mode-button");
        var previousValue = button.innerHTML;
        var previousName = button.getAttribute("name");
        button.innerHTML = event.target.innerHTML + " &#x2BC6;";
        button.setAttribute("name", event.target.getAttribute("name"));
        var mode = event.target.parentNode;
        mode.removeChild(event.target);
        var a = document.createElement("A");
        a.innerHTML = previousValue.substring(0, previousValue.length - 2);
        a.href = '#';
        a.setAttribute("name", previousName);
        mode.insertBefore(a, mode.childNodes[0]); // prepend
        if (action == "simulation") {
          a = document.createElement("A");
          a.innerHTML = "Refresh";
          a.href = "#";
          a.setAttribute("name", "refresh");
          mode.appendChild(a)
          selected_port = "simulation"
        } else {
          mode.removeChild(mode.lastChild); // Refresh
          selected_port = "remote control";
          action = "remote control " + action;
        }
      } else if (menu == 'upload') {
        document.getElementById("file-selector").click();
        selected_port = action;
        action = "";
      }
    }
    if (action)
      window.robotWindow.send(action);
  }
}

function hideAllDropDownMenus() {
  var dropdowns = document.getElementsByClassName("dropdown-content");
  var i;
  for (i = 0; i < dropdowns.length; i++) {
    var openDropdown = dropdowns[i];
    if (openDropdown.classList.contains('show')) {
      openDropdown.classList.remove('show');
    }
  }
}

function dropDownMenu(id) {
  hideAllDropDownMenus();
  if (selected_port=="remote control" && id=="upload") {
    console.log("Please revert to simulation before uploading HEX to the e-puck robot.")
    return;
  }
  document.getElementById(id).classList.toggle("show");
}

function robotLayout(configure) {
  window.robotWindow.setTitle("Robot: " + configure.name, configure.name);
}

function onEnableAll() {
  window.robotWindow.send("enable");
}
