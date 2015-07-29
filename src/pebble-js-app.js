var slotid1 = 0;
var slotid2 = 0;
var slotid3 = 0;
var slotid4 = 0;

var slot1key = 0;
var slot2key = 1;
var slot3key = 2;
var slot4key = 3;
var showsecondskey = 4;
var showhourskey = 5;
var showminuteskey = 6;
var showlogokey = 7;
var colorskey = 8;
var firsttimekey = 9;

var colorstorage = "555555FF0000555555555555FFFFFF";

var showsecondsbool = 1;

var showlogoint = 1;
var showminutesint = 1;
var showhoursint = 1;

var options = {};

var firsttime = 1;

function loadLocalVariables() {
  firsttime = parseInt(localStorage.getItem("firsttimekey"));
  
  if(firsttime == 1) {
    localStorage.setItem("showlogokey", showlogoint);
    localStorage.setItem("showminuteskey", showminutesint);
    localStorage.setItem("showhourskey", showhoursint);
    localStorage.setItem("showsecondskey", showsecondsbool);
    localStorage.setItem("colorskey", colorstorage);
  }
  slotid1 = parseInt(localStorage.getItem("slot1key"));
  slotid2 = parseInt(localStorage.getItem("slot2key"));
  slotid3 = parseInt(localStorage.getItem("slot3key"));
  slotid4 = parseInt(localStorage.getItem("slot4key"));
  showsecondsbool = parseInt(localStorage.getItem("showsecondskey"));
  showlogoint = parseInt(localStorage.getItem("showlogokey"));
  showhoursint = parseInt(localStorage.getItem("showhourskey"));
  showminutesint = parseInt(localStorage.getItem("showminuteskey"));
  colorstorage = localStorage.getItem("colorskey");
  console.log("option 1: " + slotid1);
  console.log("option 2: " + slotid2);
  console.log("option 3: " + slotid3);
  console.log("option 4: " + slotid4);
  console.log("show seconds: " + showsecondsbool);
  console.log("show logo: " + showlogoint);
  console.log("show minute ticks: " + showminutesint);
  console.log("show hour ticks: " + showhoursint);
  console.log("colors: "+colorstorage);
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
    var url = ('https://75f9e15011f94786195199c92d48a7c152f31f4c.googledrive.com/host/0B9MK2_flKmy7QnJNRFlOYnVSSE0?slotid1='+slotid1+'&slotid2='+slotid2+'&slotid3='+slotid3+'&slotid4='+slotid4+"&showsecondsint="+showsecondsbool+"&showlogoint="+showlogoint+"&showminutes="+showminutesint+"&showhours="+showhoursint+"&colors="+colorstorage);
    console.log("opening: "+url);
    Pebble.openURL(url);
  }
); 

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
    var rawconfig = JSON.stringify(configuration);
    /*var option1 = parseInt(rawconfig.substring(14,16));
    var option2 = parseInt(rawconfig.substring(31,33));
    var option3 = parseInt(rawconfig.substring(48,50));
    var option4 = parseInt(rawconfig.substring(65,67));
    var showlogo = parseInt(rawconfig.substring(142,143));
    var showseconds = parseInt(rawconfig.substring(87,88));
    var showminutes = parseInt(rawconfig.substring(107,108));
    var showhours = parseInt(rawconfig.substring(125,126));
    var backgroundcolor = rawconfig.substring(168,174);
    var handcolor = rawconfig.substring(194,200);
    var accentcolor = rawconfig.substring(222,228);*/
    
    var entries = rawconfig.split(',');
    
    var rawoption1 = entries[0].split(':');
    var option1split1 = rawoption1[1].replace('"', '');
    var option1 = parseInt(option1split1.replace('"', ''));
    
    var rawoption2 = entries[1].split(':');
    var option2split1 = rawoption2[1].replace('"', '');
    var option2 = parseInt(option2split1.replace('"', ''));
    
    var rawoption3 = entries[2].split(':');
    var option3split1 = rawoption3[1].replace('"', '');
    var option3 = parseInt(option3split1.replace('"', ''));
    
    var rawoption4 = entries[3].split(':');
    var option4split1 = rawoption4[1].replace('"', '');
    var option4 = parseInt(option4split1.replace('"', ''));
    
    var rawshowlogo = entries[7].split(':');
    var showlogo = parseInt(rawshowlogo[1].replace('"', ''));
    
    var rawshowseconds = entries[4].split(':');
    var showseconds = parseInt(rawshowseconds[1].replace('"', ''));
    
    var rawshowminutes = entries[5].split(':');
    var showminutes = parseInt(rawshowminutes[1].replace('"', ''));
    
    var rawshowhours = entries[6].split(':');
    var showhours = parseInt(rawshowhours[1].replace('"', ''));
    
    var rawbackgroundcolor = entries[8].split(':');
    var backgroundcolorsplit1 = rawbackgroundcolor[1].replace('"', '');
    var backgroundcolor = backgroundcolorsplit1.replace('"', '');
    
    var rawhandcolor = entries[9].split(':');
    var handcolorsplit1 = rawhandcolor[1].replace('"', '');
    var handcolor = handcolorsplit1.replace('"', '');
    
    var rawaccentcolor = entries[10].split(':');
    var accentcolorsplit1 = rawaccentcolor[1].replace('"', '');
    var accentcolorsplit2 = accentcolorsplit1.replace('"', '');
    var accentcolor = accentcolorsplit2.replace('}', '');
    
    var rawboldtickcolor = entries[11].split(':');
    var boldcolorsplit1 = rawboldtickcolor[1].replace('"', '');
    var boldcolorsplit2 = boldcolorsplit1.replace('"', '');
    var boldtickcolor = boldcolorsplit2.replace('}', '');
    console.log("bold tick color: "+boldtickcolor);
    
    var rawfacecolor = entries[12].split(':');
    var facecolorsplit1 = rawfacecolor[1].replace('"', '');
    var facecolorsplit2 = facecolorsplit1.replace('"', '');
    var facecolor = facecolorsplit2.replace('}', '');
    console.log("face color: "+facecolor);
    
    
    options.KEY_SLOT1 = option1;
    options.KEY_SLOT2 = option2;
    options.KEY_SLOT3 = option3;
    options.KEY_SLOT4 = option4;
    options.KEY_SHOWSECONDS = showseconds;
    options.KEY_SHOWLOGO = showlogo;
    options.KEY_SHOWMINUTES = showminutes;
    options.KEY_SHOWHOURS = showhours;
    
    /*options.KEY_BACKGROUNDR = backgroundr;
    options.KEY_BACKGROUNDG = backgroundg;
    options.KEY_BACKGROUNDB = backgroundb;
    options.KEY_HANDR = handr;
    options.KEY_HANDG = handg;
    options.KEY_HANDB = handb;
    options.KEY_ACCENTR = accentr;
    options.KEY_ACCENTG = accentg;
    options.KEY_ACCENTB = accentb;*/
    
    /*options.KEY_BACKGROUNDCOLOR = backgroundcolor;
    options.KEY_HANDCOLOR = handcolor;
    options.KEY_ACCENTCOLOR = accentcolor;*/
    
    options.KEY_BASALTCOLORS = (backgroundcolor+handcolor+accentcolor+boldtickcolor+facecolor);
    
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
    if(!isNaN(options.KEY_SHOWHOURS) && options.KEY_SHOWHOURS != showhoursint) {
      showhoursint = options.KEY_SHOWHOURS;
      localStorage.setItem("showhourskey", showhoursint);
    }
    if(!isNaN(options.KEY_SHOWMINUTES) && options.KEY_SHOWMINUTES != showminutesint) {
      showminutesint = options.KEY_SHOWMINUTES;
      localStorage.setItem("showminuteskey", showminutesint);
    }
    if(!isNaN(options.KEY_SHOWLOGO) && options.KEY_SHOWLOGO != showlogoint) {
      showlogoint = options.KEY_SHOWLOGO;
      localStorage.setItem("showlogokey", showlogoint);
    }
    /*if(!isNaN(options.KEY_BACKGROUNDCOLOR) && options.KEY_BACKGROUNDCOLOR != backgroundcolorstorage){
      backgroundcolorstorage = options.KEY_BACKGROUNDR;
      localStorage.setItem("backgroundrkey", backgroundcolorstorage);
    }
    if(!isNaN(options.KEY_HANDRKEY) && options.KEY_HANDR != handcolorstorage){
      handcolorstorage = options.KEY_HANDR;
      localStorage.setItem("handrkey", handcolorstorage);
    }
    if(!isNaN(options.KEY_ACCENTCOLOR) && options.KEY_ACCENTCOLOR != accentcolorstorage){
      accentcolorstorage = options.KEY_ACCENTCOLOR;
      localStorage.setItem("accentcolorkey", accentcolorstorage);
    }*/
    
    firsttime = 0;
    localStorage.setItem("firsttimekey", firsttime);
    
    colorstorage = options.KEY_BASALTCOLORS;
    localStorage.setItem("colorskey", colorstorage);
    
    console.log("option1: "+options.KEY_SLOT1+"\noption2: "+options.KEY_SLOT2+"\noption3: "+options.KEY_SLOT3+"\noption4: "+options.KEY_SLOT4+"\nshowlogo: "+options.KEY_SHOWLOGO+"\nshowseconds: "+options.KEY_SHOWSECONDS+"\nshowminutes: "+options.KEY_SHOWMINTES+"\nshowhours: "+options.KEY_SHOWHOURS+"\ncolors: "+options.KEY_BASALTCOLORS);
    
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