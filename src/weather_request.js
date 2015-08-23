var API_KEY = "{YOUR-API-KEY}";

var locationOptions = {
  enableHighAccuracy: false, 
  maximumAge: 60000, 
  timeout: 15000
};

function generateUrl(lat, lon) {
  return "http://api.wunderground.com/api/" + API_KEY + "/q/" + lat + "," + lon + ".json";
}

function retrieveWeather(position) {
  console.log('lat= ' + position.coords.latitude + ' lon= ' + position.coords.longitude);
}

function locationError(error) {
  console.log('Error code while fetching location {' + error.code + '}.  ' + error.message);
}

Pebble.addEventListener('ready',
  function(e) {
    console.log('JavaScript app ready and running!');
    navigator.geolocation.getCurrentPosition(retrieveWeather, locationError, locationOptions);
    console.log('Generated url [' + generateUrl(37.776289, -122.395234) + ']');
  }
);

Pebble.addEventListener('fetch_weather', 
  function(e) {
    console.log('Got request to fetch weather');
  }
);
// http://api.wunderground.com/api/{YOUR-API-KEY}/hourly/q/37.776289,-122.395234.json