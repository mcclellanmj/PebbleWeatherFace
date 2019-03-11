var API_KEY = "%DARKSKY_KEY%";

var HTTP_REQUEST_TIMEOUT = 45000;

var locationOptions = {
  enableHighAccuracy: false, 
  maximumAge: 240000,
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

var SunPhases = (function() {
  var isDay = function(sunPhaseEpochs, currentEpoch) {
    console.log("Is day calculation: riseEpoch [" + sunPhaseEpochs.riseEpoch + "]; currentEpoch [" + currentEpoch + "]; setEpoch [" + sunPhaseEpochs.setEpoch + "]");
    return currentEpoch >= sunPhaseEpochs.riseEpoch && currentEpoch <= sunPhaseEpochs.setEpoch;
  };
  
  var isNight = function(sunPhaseEpochs, currentEpoch) {
    return !isDay(sunPhaseEpochs, currentEpoch);
  };
  
  return {
    isDay : isDay,
    isNight : isNight
  };
})();

var Time = (function() {
  var convertToEpoch = function(hour, minute) {
    var date = new Date();
    date.setHours(hour);
    date.setMinutes(minute);
    return date.getTime();
  };
  
  var getCurrentEpoch = function() {
    return new Date().getTime();
  };
  
  return {
    convertToEpoch : convertToEpoch,
    getCurrentEpoch : getCurrentEpoch
  };
})();

var ByteConversions = (function() {
  var showBinaryForByte = function(byte) {
    var str = byte.toString(2);
    return ('00000000' + str).substring(str.length);
  };
  
  var toInt8ByteArray = function(integer) {
    return [integer & 0x000000FF];
  };
  
  var toInt16ByteArray = function(integer) {
    return [
      (integer & 0x000000FF),
      (integer & 0x0000FF00) >> 8
    ];
  };
  
  var toInt32ByteArray = function(integer) {
    return [
      (integer & 0x000000FF),
      (integer & 0x0000FF00) >> 8,
      (integer & 0x00FF0000) >> 16,
      (integer & 0xFF000000) >> 24
    ];
  };
  
  return {
    showBinaryForByte : showBinaryForByte,
    toInt8ByteArray : toInt8ByteArray,
    toInt16ByteArray : toInt16ByteArray,
    toInt32ByteArray : toInt32ByteArray,
  };
})();

var Weather = (function() {
  var self = this;
  
  self.extractTemp = function(longWeather) { return Math.round(longWeather.temperature); };
  self.extractPop = function(longWeather) { return Math.round(longWeather.precipProbability * 100); };
  self.extractSunPhases = function(rise, set) {
    return {
      riseEpoch : rise,
      setEpoch : set
    };
  };

  self.alwaysIcon = function(id) {
    return function(sunPhaseEpochs) {
      return id;
    };
  };
  
  self.dayNightIcons = function(dayId, nightId) {
    return function(sunPhaseEpochs) {
      if(SunPhases.isNight(sunPhaseEpochs, Time.getCurrentEpoch())) {
        return nightId;
      }

      return dayId;
    };
  };
 
  self.iconToIconIdMap = {
    "clear-day": self.alwaysIcon(0),
    "clear-night": self.alwaysIcon(5),
    "rain": self.alwaysIcon(10),
    "snow": self.alwaysIcon(15),
    "sleet": self.alwaysIcon(17),
    "wind": self.alwaysIcon("??"),
    "fog" : self.alwaysIcon(9),
    "cloudy" : self.alwaysIcon(4),
    "partly-cloudy-day": self.alwaysIcon(1),
    "partly-cloudy-night": self.alwaysIcon(6),
    /*
    Old weather undergound icons
    "partlycloudy" : self.dayNightIcons(1, 6),
    "mostlycloudy" : self.dayNightIcons(2, 7),
    "hazy" : self.alwaysIcon(9),
    "chancerain" : self.alwaysIcon(23),
    "rain" : self.alwaysIcon(10),
    "chancetstorms" : self.alwaysIcon(19),
    "tstorms" : self.alwaysIcon(20),
    "flurries" : self.alwaysIcon(14),
    "chancesleet" : self.alwaysIcon(17),
    "sleet" : self.alwaysIcon(17),
    "chancesnow" : self.alwaysIcon(15),
    "snow" : self.alwaysIcon(15),
    */
    "ERROR" : self.alwaysIcon(24)
  };
  
  self.getIconId = function(forecastCode, state) {
    console.log("Finding icon id for forecast code [" + forecastCode + "]");
    if(forecastCode in self.iconToIconIdMap) {
      return self.iconToIconIdMap[forecastCode](state);
    }
    
    console.log("Unable to find icon id for [" + forecastCode + "]");
    return self.iconToIconIdMap.ERROR(state);
  };
  
  self.apiModelToWatchModel = function (jsonObject) {
    var current = jsonObject.currently;

    var hourly = jsonObject.hourly.data;
    var daily = jsonObject.daily.data[0];
    var forecastPieces = hourly.slice(1, 13);
    var forecastTemps = forecastPieces.map(self.extractTemp, self);
    var forecastPrecip = forecastPieces.map(self.extractPop, self);
    var sunPhaseEpochs = extractSunPhases(daily.sunriseTime, daily.sunsetTime);

    return {
      "MESSAGE_TYPE" : ByteConversions.toInt8ByteArray(MessageTypes.WEATHER_REPORT),
      "WEATHER_TEMP" : ByteConversions.toInt16ByteArray(Math.round(current.temperature)),
      "WEATHER_ICON_OFFSET" : ByteConversions.toInt8ByteArray(self.getIconId(current.icon, sunPhaseEpochs)),
      "WEATHER_FORECAST_START" : ByteConversions.toInt8ByteArray(new Date(hourly[1].time * 1000).getHours()),
      "WEATHER_FORECAST_PRECIP_CHANCE" : [].concat.apply([], forecastPrecip.map(ByteConversions.toInt8ByteArray)),
      "WEATHER_FORECAST_TEMPS" : [].concat.apply([], forecastTemps.map(ByteConversions.toInt16ByteArray)),
      "WEATHER_UV" : ByteConversions.toInt8ByteArray(current.uvIndex),
      "WEATHER_HUMIDITY" : ByteConversions.toInt8ByteArray(Math.round(current.humidity * 100)),
      "WEATHER_WIND" : ByteConversions.toInt8ByteArray(Math.round(current.windSpeed)),
      "SUNRISE_TIME" : ByteConversions.toInt32ByteArray(daily.sunriseTime),
      "SUNSET_TIME" : ByteConversions.toInt32ByteArray(daily.sunsetTime)
    };
  };
  
  self.retrieveSuccess = function(req) {
    if(!(req.status >= 200 && req.status < 300)) {
      console.log("Status code [" + req.status + "] is considered a failure");
      self.retrieveFailure(req);
      return;
    }
    
    var responseJson = JSON.parse(req.responseText);
    var weatherModel = self.apiModelToWatchModel(responseJson);
    console.log("weather model conversion finished");
    sendWeatherModel(weatherModel);
  };
  
  self.retrieveFailure = function(req) {
    console.log("[" + req + "] request failed.");
    Pebble.sendAppMessage({"MESSAGE_TYPE" : MessageTypes.WEATHER_FAILED});
  };
  
  self.retrieve = function(position, onSuccess, onError) {
    function generateUrl(lat, lon) {
      return "https://api.darksky.net/forecast/" + API_KEY + "/" + lat + "," + lon;
    }

    var latitude = position.coords.latitude;
    var longitude = position.coords.longitude;
    console.log('lat=[' + latitude + '], lon=[' + longitude + ']');
  
    var url = generateUrl(latitude, longitude);
    console.log('Generated url [' + url + ']');
  
    makeRequest("GET", url, self.retrieveSuccess.bind(self), self.retrieveFailure.bind(self));
  };
  
  return {
    retrieve : self.retrieve
  };
})();

function locationError(error) {
  if(error !== undefined) {
    console.log('Error code while fetching location [' + error.code + '].  [' + error.message + ']');
  }
  
  Pebble.sendAppMessage({"MESSAGE_TYPE" : MessageTypes.WEATHER_FAILED});
}

function MessageHandler() {
  this.messageFunctions = {};
  
  this.messageFunctions[MessageTypes.FETCH_WEATHER] = function(message) {
    navigator.geolocation.getCurrentPosition(Weather.retrieve, locationError, locationOptions);
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
