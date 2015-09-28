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

var Weather = {
  extractTemp : function(longWeather) { return longWeather.temp.english; },
  extractPop : function(longWeather) { return longWeather.pop; },
  
  apiModelToWatchModel : function (jsonObject) {
    var hourly = jsonObject.hourly_forecast;
    var currentWeather = this.toShortWeather(hourly[0]);

    var forecastPieces = hourly.slice(1, 13);
    var forecastTemps = forecastPieces.map(this.extractTemp, this);
    var forecastPrecip = forecastPieces.map(this.extractPop, this);

    return {
      "MESSAGE_TYPE" : "WEATHER_REPORT",
      "WEATHER_TEMP" : currentWeather.temperature,
      "WEATHER_WIND_DIRECTION" : currentWeather.windDirection,
      "WEATHER_WIND_SPEED" : currentWeather.windSpeed,
      "WEATHER_ICON_OFFSET" : currentWeather.icon,
      "WEATHER_FORECAST_PRECIP_CHANCE" : forecastPrecip,
      "WEATHER_FORECAST_TEMPS" : forecastTemps
    };
  },
  
  retrieveSuccess : function(req) {
    // TODO: Check req status code for error
    var weatherModel = this.apiModelToWatchModel(JSON.parse(req.responseText));
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
      "icon" : this.iconToOffset(longWeather.icon),
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
    Pebble.sendAppMessage({"MESSAGE_TYPE": "PHONE_READY"});
  }
);

Pebble.addEventListener('appmessage', 
  function(e) {
    var msg = e.payload;
    MessageHandler.handleMessage.bind(MessageHandler)(msg);
  }
);