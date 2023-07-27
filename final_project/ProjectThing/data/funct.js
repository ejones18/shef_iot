/**
 * @file funct.js
 * @brief JavaScript functionality.
 * 
 * @author Ethan Jones
 * @date 2023-05-12
 */


// Constant declaration
const url = "https://ejjones18uossem2iot-prediction.cognitiveservices.azure.com/customvision/v3.0/Prediction/fcf81951-588b-4228-8f22-54600b2f7907/detect/iterations/Iteration2/image"
const pred_key = "93787c9c9a4548558cfd5cbd98d6051f"
const img_url = "http://192.168.1.248/saved-photo"

/**
* @brief Captures an image from the ESP-EYE.
*/
function capture_photo() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();
    setTimeout(add_img, 1000); //Wait a second for the photo to be uploaded to the server...
}

/**
* @brief Checks if an image is already on the webpage - allows for continuous pictures without spamming the page.
*
* @returns {JSON object} - contains depend on whether or not an image is present.
*/
function check_img() {
    return document.getElementById("img")
}

/**
* @brief Checks if the query_model button is already on the webpage
*
* @returns {JSON object} - contains depend on whether or not the button is present.
*/
function check_btn() {
    return document.getElementById("myButton")
}

/**
* @brief Clears the web-page of the image if one is displayed.
*/
function clear_img() {
    if (document.getElementById("img")) {
        document.getElementById("img").remove()
    }
}

/**
* @brief HTTP request to fetch the newest, saved image on the server.
*/
function fetch_new_img() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/saved-photo");
    xhr.send();
}

/**
* @brief Adds the latest image to the web-page.
*/
function add_img() {
    add_btn()
    clear_img()
    fetch_new_img()
    var img = document.createElement("img");
    img.src = img_url;
    img.id = "img"; 
    document.getElementById("photo").appendChild(img);
}

/**
* @brief Adds the query_model button to the web-page.
*/
function add_btn() {
    // Create a new button element
    if (!check_btn()) {
        var btn = document.createElement("button");
        // Set the button's text
        btn.innerHTML = "Query model with image!";
        btn.id = "myButton";
        // Set the button's class attribute
        btn.setAttribute("class", "my-button");
        // Add an event listener to the button
        btn.addEventListener("click", function() {
            query_model();
        });
        // Append the button to a parent element
        document.getElementById("container").appendChild(btn);
    }
}

/**
* @brief Queries the Azure Custom Vision endpoint using the image as the payload. Draws on the predictions and displays it on the web-page.
*/
function query_model() {
    var img = new Image();
    img.src = img_url;
    // Wait for the image to load
    img.onload = function() {
      fetch(img.src)
        .then(response => response.blob())
        .then(blob => {
          // Read the image data as a byte array
          var reader = new FileReader();
          reader.onloadend = function() {
            var byteArray = new Uint8Array(reader.result);
            // Send the byte array to the Custom Vision endpoint using fetch()
            fetch(url, {
                method: 'POST',
                headers: {
                  'Prediction-Key': pred_key,
                  'Content-Type': 'application/octet-stream'
                },
                body: byteArray
              })
              .then(response => response.json())
              .then(data => {
                // Process the prediction results returned by the Custom Vision endpoint
                var canvas = document.createElement('canvas');
                var ctx = canvas.getContext('2d');
                canvas.width = img.width;
                canvas.height = img.height;
                ctx.drawImage(img, 0, 0, img.width, img.height);

                var predictions = data.predictions;
                // Draw on predicitions if above 85% probability
                for (var i = 0; i < predictions.length; i++) {
                  var prediction = predictions[i];
                  if (prediction.probability >= 0.85) {
                    var bbox = prediction.boundingBox;
                    var x = bbox.left * img.width;
                    var y = bbox.top * img.height;
                    var width = bbox.width * img.width;
                    var height = bbox.height * img.height;

                    ctx.beginPath();
                    ctx.rect(x, y, width, height);
                    ctx.lineWidth = 2;
                    ctx.strokeStyle = 'red';
                    ctx.stroke();

                    // Add the label and probability as text
                    var label = prediction.tagName;
                    var probability = Math.round(prediction.probability * 100) + '%';
                    ctx.font = '16px Arial';
                    ctx.fillStyle = 'red';
                    ctx.fillText(label + ' (' + probability + ')', x, y - 5);
                  }
                }
                // Replace image source param to the edited image
                var dataUrl = canvas.toDataURL('image/jpeg');
                document.getElementById('img').src = dataUrl;
              })
              .catch(error => {
                console.error(error);
              });
            }
            reader.readAsArrayBuffer(blob);
          })
          .catch(error => {
            console.error(error);
          });
      }
}