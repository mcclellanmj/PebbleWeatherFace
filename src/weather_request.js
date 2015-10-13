var API_KEY = "%WEATHER_UNDERGROUND_KEY%";

var HTTP_REQUEST_TIMEOUT = 45000;

var locationOptions = {
  enableHighAccuracy: false, 
  maximumAge: 60000, 
  timeout: 15000
};

function makeRequest(method, url, callback, errCallback) {
	var req = new XMLHttpRequest();
  
  req.onerror = errCallback;
  req.onreadystatechange = function(e) {
    if(this.readyState == 4 && this.status >= 200 && this.status < 300) {
      callback(this);
    } else if(this.readyState == 4) {
      console.log("Request for weather failed status code is [" + this.status + "]");
      errCallback(this);
    }
  };
  
  // If after the timeout request hasn't finished we need to abort it
  setTimeout(function() {
    console.log("Checking up on HTTP request");
    if(this.readyState != 4) {
      this.abort();
      this.onerror();
      console.log("Aborting request since its still not done");
    }
  }.bind(req), HTTP_REQUEST_TIMEOUT);
  
  req.open(method, url, true);
  req.send();
}

var MessageTypes = {
  "PHONE_READY" : 0,
  "WEATHER_REPORT" : 1,
  "FETCH_WEATHER" : 2,
  "WEATHER_FAILED" : 3
};

var ByteConversions = {
  showBinaryForByte : function(byte) {
    var str = byte.toString(2);
    return ('00000000' + str).substring(str.length);
  },
  
  toInt8ByteArray : function(integer) {
    return [integer & 0x000000FF];
  },
  
  toInt16ByteArray : function(integer) {
    return [
      (integer & 0x000000FF),
      (integer & 0x0000FF00) >> 8
    ];
  }
};

var Weather = {
  extractTemp : function(longWeather) { return longWeather.temp.english; },
  extractPop : function(longWeather) { return longWeather.pop; },
  
  iconToIconIdMap : {
    "clear" : 0,
    "partlycloudy" : 1,
    "mostlycloudy" : 3,
    "cloudy" : 4,
    "hazy" : 9,
    "fog" : 9,
    "chancerain" : 23,
    "rain" : 10,
    "chancetstorms" : 19,
    "tstorms" : 20,
    "flurries" : 14,
    "chancesleet" : 17,
    "sleet" : 17,
    "chancesnow" : 15,
    "snow" : 15,
    "ERROR" : 24
  },
  
  getIconId : function(forecastCode) {
    console.log("Finding icon id for forecast code [" + forecastCode + "]");
    if(forecastCode in this.iconToIconIdMap) {
      return this.iconToIconIdMap[forecastCode];
    }
    
    console.log("Unable to find icon id for [" + forecastCode + "]");
    return this.iconToIconIdMap.ERROR;
  },
  
  apiModelToWatchModel : function (jsonObject) {
    var current = jsonObject.current_observation;

    var hourly = jsonObject.hourly_forecast;
    var forecastPieces = hourly.slice(0, 12);
    var forecastTemps = forecastPieces.map(this.extractTemp, this);
    var forecastPrecip = forecastPieces.map(this.extractPop, this);

    return {
      "MESSAGE_TYPE" : ByteConversions.toInt8ByteArray(MessageTypes.WEATHER_REPORT),
      "WEATHER_TEMP" : ByteConversions.toInt16ByteArray(Math.round(current.temp_f)),
      "WEATHER_ICON_OFFSET" : ByteConversions.toInt8ByteArray(this.getIconId(current.icon)),
      "WEATHER_FORECAST_START" : ByteConversions.toInt8ByteArray(forecastPieces[0].FCTTIME.hour),
      "WEATHER_FORECAST_PRECIP_CHANCE" : [].concat.apply([], forecastPrecip.map(ByteConversions.toInt16ByteArray)),
      "WEATHER_FORECAST_TEMPS" : [].concat.apply([], forecastTemps.map(ByteConversions.toInt16ByteArray))
    };
  },
  
  retrieveSuccess : function(req) {
    if(!(req.status >= 200 && req.status < 300)) {
      console.log("Status code [" + req.status + "] is considered a failure");
      this.retrieveFailure(req);
      return;
    }
    
    var responseJson = JSON.parse(req.responseText);
    if("error" in responseJson.response) {
      console.log("JSON response contained error [" + responseJson.response.error.description + "] marking request as a failure");
      this.retrieveFailure(req);
      return;
    }
    
    var weatherModel = this.apiModelToWatchModel(responseJson);
    console.log("weather model conversion finished");
    sendWeatherModel(weatherModel);
  },
  
  retrieveFailure : function(req) {
    console.log("[" + req + "] request failed.");
    Pebble.sendAppMessage({"MESSAGE_TYPE" : MessageTypes.WEATHER_FAILED});
  },
  
  retrieve : function(position, onSuccess, onError) {
    function generateUrl(lat, lon) {
      return "http://api.wunderground.com/api/" + API_KEY + "/hourly/conditions/q/" + lat + "," + lon + ".json";
    }

    var latitude = position.coords.latitude;
    var longitude = position.coords.longitude;
    console.log('lat=[' + latitude + '], lon=[' + longitude + ']');
  
    var url = generateUrl(latitude, longitude);
    console.log('Generated url [' + url + ']');
  
    makeRequest("GET", url, this.retrieveSuccess.bind(this), this.retrieveFailure.bind(this));
  },
  
  toShortWeather : function(longWeather) {
    return {
      "time" : longWeather.FCTTIME.hour,
      "temperature" : longWeather.temp.english,
      "forecastCode" : longWeather.fctcode,
      "pop" : longWeather.pop
    };
  },
};

function locationError(error) {
  console.log('Error code while fetching location [' + error.code + '].  [' + error.message + ']');
  Pebble.sendAppMessage({"MESSAGE_TYPE" : MessageTypes.WEATHER_FAILED});
}

function MessageHandler() {
  this.messageFunctions = {};
  
  this.messageFunctions[MessageTypes.FETCH_WEATHER] = function(message) {
    navigator.geolocation.getCurrentPosition(Weather.retrieve.bind(Weather), locationError, locationOptions);
  };
}

MessageHandler.prototype.handleMessage = function(message) {
  var messageType = message.MESSAGE_TYPE;
  console.log("Got message from watch with type [" + message.MESSAGE_TYPE + "]");
    
  this.messageFunctions[messageType](message);
};

function sendWeatherModel(weatherModel) {
  console.log("Sending weather model [" + weatherModel + "]");
  Pebble.sendAppMessage(weatherModel);
}

Pebble.addEventListener('ready',
  function(e) { 
    console.log('JavaScript app ready and running');
    Pebble.sendAppMessage({"MESSAGE_TYPE": MessageTypes.PHONE_READY});
    console.log('Phone ready message has been sent');
  }
);

var messageHandler = new MessageHandler();
Pebble.addEventListener('appmessage', 
  function(e) {
    var msg = e.payload;
    messageHandler.handleMessage(msg);
  }
);
