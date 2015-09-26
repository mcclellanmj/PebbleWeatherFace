var API_KEY = "%WEATHER_UNDERGROUND_KEY%";
function emptyState(statusCode) {
  return {
    "statusCode" : statusCode,
    "weather" : []
  };
}

var initialState = emptyState(1);
var failureState = emptyState(2);
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
    function generateUrl(lat, lon) {
      return "http://api.wunderground.com/api/" + API_KEY + "/hourly/q/" + lat + "," + lon + ".json";
    }

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
    sendWeatherStatus(weatherState);
  },
  
  retrieveFailure : function(req) {
    weatherState = failureState;
    sendWeatherStatus(weatherState);
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

function apiResponseToState(jsonObject) {
  var hourly = jsonObject.hourly_forecast;
  
  return {
    "statusCode" : 0,
    "weather" : hourly.slice(0, 13).map(Weather.toShortWeather, Weather)
  };
}

function sendWeatherStatus(weatherState) {
  Pebble.sendAppMessage({
    "WEATHER_STATUS_CODE" : weatherState.statusCode
  });
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

Pebble.addEventListener('appmessage', 
  function(e) {
    console.log('Got request to fetch weather');
    navigator.geolocation.getCurrentPosition(Weather.retrieve.bind(Weather), locationError, locationOptions);
  }
);