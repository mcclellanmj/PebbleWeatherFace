var API_KEY = "%WEATHER_UNDERGROUND_KEY%";

var locationOptions = {
  enableHighAccuracy: false, 
  maximumAge: 60000, 
  timeout: 15000
};

function makeRequest(method, url, callback, errCallback) {
	var req = new XMLHttpRequest();
  
	req.open(method, url, true);
	req.onload = function(e) {
		if(req.readyState == 4) { callback(req); }
	};
  req.onerror = errCallback;
  
	req.send();
}

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
  
  iconToOffsetMap : {
    "Clear" : 0,
    "Partly Cloudy" : 1,
    "Mostly Cloudy" : 3,
    "Cloudy" : 4,
    "Hazy" : 9,
    "Foggy" : 9,
    "Very Hot" : 21,
    "Very Cold" : 22,
    "Blowing Snow" : 15,
    "Chance of Showers" : 23,
    "Showers" : 10,
    "Chance of Rain" : 23,
    "Rain" : 11,
    "Chance of a Thunderstorm" : 19,
    "Thunderstorm" : 20,
    "Flurries" : 14,
    "OMITTED" : 24,
    "Chance of Snow Showers" : 17,
    "Snow Showers" : 17,
    "Chance of Snow" : 15,
    "Snow" : 15,
    "Chance of Ice Pellets" : 13,
    "Ice Pellets" : 13,
    "Blizzard" : 16,
    "ERROR" : 24
  },
  
  getIconOffset : function(iconLabel) {
    console.log("Finding icon offset for [" + iconLabel + "]");
    if(iconLabel in this.iconToOffsetMap) {
      return this.iconToOffsetMap[iconLabel];
    }
    
    return this.iconToOffsetMap.ERROR;
  },
  
  apiModelToWatchModel : function (jsonObject) {
    var hourly = jsonObject.hourly_forecast;
    var currentWeather = this.toShortWeather(hourly[0]);

    var iconOffset = this.getIconOffset(currentWeather.condition);
    var forecastPieces = hourly.slice(1, 13);
    var forecastTemps = forecastPieces.map(this.extractTemp, this);
    var forecastPrecip = forecastPieces.map(this.extractPop, this);

    return {
      // TODO: MESSAGE_TYPE is needlessly wasting space, converting to an uint8 would be better
      "MESSAGE_TYPE" : "WEATHER_REPORT",
      "WEATHER_TEMP" : ByteConversions.toInt16ByteArray(currentWeather.temperature),
      "WEATHER_WIND_DIRECTION" : ByteConversions.toInt16ByteArray(currentWeather.windDirection),
      "WEATHER_WIND_SPEED" : ByteConversions.toInt16ByteArray(currentWeather.windSpeed),
      "WEATHER_ICON_OFFSET" : ByteConversions.toInt8ByteArray(iconOffset),
      "WEATHER_FORECAST_PRECIP_CHANCE" : [].concat.apply([], forecastPrecip.map(ByteConversions.toInt16ByteArray)),
      "WEATHER_FORECAST_TEMPS" : [].concat.apply([], forecastTemps.map(ByteConversions.toInt16ByteArray))
    };
  },
  
  retrieveSuccess : function(req) {
    // TODO: Check req status code for error
    console.log("Request for weather succeeded");
    var weatherModel = this.apiModelToWatchModel(JSON.parse(req.responseText));
    console.log("weather model conversion finished");
    sendWeatherModel(weatherModel);
  },
  
  retrieveFailure : function(req) {
    console.log("[" + req + "] request failed.");
  },
  
  retrieve : function(position, onSuccess, onError) {
    function generateUrl(lat, lon) {
      return "http://api.wunderground.com/api/" + API_KEY + "/hourly/q/" + lat + "," + lon + ".json";
    }

    var latitude = position.coords.latitude;
    var longitude = position.coords.longitude;
    console.log('lat=[' + latitude + '], lon=[' + longitude + ']');
  
    var url = generateUrl(latitude, longitude);
    console.log('Generated url [' + url + ']');
  
    makeRequest("GET", url, this.retrieveSuccess.bind(this), this.retrieveFailure.bind(this));
  },
  
  iconToOffset : function(icon) {
    // TODO: Convert from text into the appropriate image offset
    return 0;
  },
  
  toShortWeather : function(longWeather) {
    return {
      "time" : longWeather.FCTTIME.hour,
      "temperature" : longWeather.temp.english,
      "condition" : longWeather.condition,
      "windSpeed" : longWeather.wspd.english,
      "windDirection" : longWeather.wdir.degrees,
      "pop" : longWeather.pop
    };
  },
};

function locationError(error) {
  console.log('Error code while fetching location [' + error.code + '].  [' + error.message + ']');
}

var MessageHandler = {
  messageFunctions : {
    "FETCH_WEATHER" : function(message) {
      navigator.geolocation.getCurrentPosition(Weather.retrieve.bind(Weather), locationError, locationOptions);
    }
  },
  handleMessage : function(message) {
    var messageType = message.MESSAGE_TYPE;
    console.log("Got message from watch with type [" + message.MESSAGE_TYPE + "]");
    
    var messageFn = this.messageFunctions[messageType];
    messageFn(message);
  }
};


function sendWeatherModel(weatherModel) {
  console.log("Sending weather model [" + weatherModel + "]");
  Pebble.sendAppMessage(weatherModel);
}

Pebble.addEventListener('ready',
  function(e) { 
    console.log('JavaScript app ready and running!');
    
    var numbersToTest = [128, -127, 0, 4, 8];
    numbersToTest.forEach(function(number) {
      var array = ByteConversions.toInt16ByteArray(number);
      array = array.map(ByteConversions.showBinaryForByte.bind(ByteConversions));
      console.log("Result for [" + number + "] is [" + array[1].toString(2) + array[0].toString(2) + "]");
    });
    Pebble.sendAppMessage({"MESSAGE_TYPE": "PHONE_READY"});
  }
);

Pebble.addEventListener('appmessage', 
  function(e) {
    var msg = e.payload;
    MessageHandler.handleMessage.bind(MessageHandler)(msg);
  }
);