var slotid1 = 0;
var slotid2 = 0;
var slotid3 = 0;
var slotid4 = 0;

var slot1key = 1;
var slot2key = 2;
var slot3key = 3;
var slot4key = 4;
var showsecondskey = 5;

var showsecondsbool = 0;

var options = {};

function loadLocalVariables() {
  slotid1 = parseInt(localStorage.getItem("slot1key"));
  slotid2 = parseInt(localStorage.getItem("slot2key"));
  slotid3 = parseInt(localStorage.getItem("slot3key"));
  slotid4 = parseInt(localStorage.getItem("slot4key"));
  showsecondsbool = parseInt(localStorage.getItem("showsecondskey"));
  console.log("option 1: " + slotid1);
  console.log("option 2: " + slotid2);
  console.log("option 3: " + slotid3);
  console.log("option 4: " + slotid4);
  console.log("show seconds: " + showsecondsbool);
  
}

Pebble.addEventListener("ready",
  function(e) {
    console.log("PebbleKit JS ready!");
    loadLocalVariables();
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    loadLocalVariables();
    console.log("Showing config page");
    var url = ('http://www.googledrive.com/host/0B9MK2_flKmy7N1RRVkVkYXRoUjg?slotid1='+slotid1+'&slotid2='+slotid2+'&slotid3='+slotid3+'&slotid4='+slotid4+"&showsecondsint="+showsecondsbool);
    Pebble.openURL(url);
  }
); 

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
    
    var rawconfig = JSON.stringify(configuration);
    var option1 = parseInt(rawconfig.substring(13,14));
    var option2 = parseInt(rawconfig.substring(27,28));
    var option3 = parseInt(rawconfig.substring(41,42));
    var option4 = parseInt(rawconfig.substring(55,56));
    var showseconds = parseInt(rawconfig.substring(75,76));
    
    options.KEY_SLOT1 = option1;
    options.KEY_SLOT2 = option2;
    options.KEY_SLOT3 = option3;
    options.KEY_SLOT4 = option4;
    options.KEY_SHOWSECONDS = showseconds;
    
    if(!isNaN(options.KEY_SLOT1) && options.KEY_SLOT1 != slotid1) {
      slotid1 = options.KEY_SLOT1;
      localStorage.setItem("slot1key", slotid1);
    }
    if(!isNaN(options.KEY_SLOT2) && options.KEY_SLOT2 != slotid2) {
      slotid2 = options.KEY_SLOT2;
      localStorage.setItem("slot2key", slotid2);
    }    
    if(!isNaN(options.KEY_SLOT3) && options.KEY_SLOT3 != slotid3) {
      slotid3 = options.KEY_SLOT3;
      localStorage.setItem("slot3key", slotid3);
    }    
    if(!isNaN(options.KEY_SLOT4) && options.KEY_SLOT4 != slotid4) {
      slotid4 = options.KEY_SLOT4;
      localStorage.setItem("slot4key", slotid4);
    }
    if(!isNaN(options.KEY_SHOWSECONDS) && options.KEY_SHOWSECONDS != showsecondsbool) {
      showsecondsbool = options.KEY_SHOWSECONDS;
      localStorage.setItem("showsecondskey", showsecondsbool);
    }
    
    console.log("option 1 = " + options.KEY_SLOT1);
    console.log("option 2 = " + options.KEY_SLOT2);
    console.log("option 3 = " + options.KEY_SLOT3);
    console.log("option 4 = " + options.KEY_SLOT4);
    console.log("show seconds: " + options.KEY_SHOWSECONDS);
    
    //Send to Pebble, persist there
    Pebble.sendAppMessage(
      options,
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);