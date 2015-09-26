var API_KEY = "%WEATHER_UNDERGROUND_KEY%";
function emptyState(statusCode) {
  return {
    "statusCode" : statusCode,
    "weather" : {
      "currentWeather" : {},
      "forecastWeather" : []
    }
  };
}

var initialState = emptyState(-1);
var failureState = emptyState(-1);
var weatherState = initialState;

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
  retrieve : function(position) {
    var latitude = position.coords.latitude;
    var longitude = position.coords.longitude;
    console.log('lat=[' + latitude + '], lon=[' + longitude + ']');
  
    var url = generateUrl(latitude, longitude);
    console.log('Generated url [' + url + ']');
  
    makeRequest("GET", url, this.retrieveSuccess, this.retrieveError);
  },
  
  retrieveSuccess : function(req) {
    // TODO: Check req status code for error
    weatherState = apiResponseToState(JSON.parse(req.responseText));
    sendWeatherState(weatherState);
  },
  
  retrieveFailure : function(req) {
    weatherState = failureState;
    sendWeatherState(weatherState);
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
      "windSpeed" : longWeather.wspd,
      "windDirection" : longWeather.wdir.degrees,
      "pop" : longWeather.pop
    };
  }
};

function generateUrl(lat, lon) {
  return "http://api.wunderground.com/api/" + API_KEY + "/hourly/q/" + lat + "," + lon + ".json";
}

function apiResponseToState(jsonObject) {
  var hourly = jsonObject.hourly_forecast;
  var currentWeather = Weather.toShortWeather(hourly[0]);
  var forecast = hourly.slice(1, 13).map(Weather.toShortWeather, Weather);
  
  return {
    "statusCode" : 0,
    "weather" : {
      "currentWeather" : currentWeather,
      "forecastWeather" : forecast
    }
  };
}

function sendMessage(type, message) {
  message.type = type;
  Pebble.sendAppMessage(message);
  console.log("Sent request of type [" + type + "] to watch, full message is [" + message + "]");
}

function sendWeatherState(weatherState) {
  sendMessage("WeatherStatus", {"statusCode" : weatherState.statusCode});
}

function locationError(error) {
  console.log('Error code while fetching location [' + error.code + '].  [' + error.message + ']');
}

Pebble.addEventListener('ready',
  function(e) { 
    console.log('JavaScript app ready and running!'); 
    navigator.geolocation.getCurrentPosition(Weather.retrieve.bind(Weather), locationError, locationOptions);
  }
);

Pebble.addEventListener('fetch_weather', 
  function(e) {
    console.log('Got request to fetch weather');
    navigator.geolocation.getCurrentPosition(Weather.retrieve.bind(Weather), locationError, locationOptions);
  }
);