function getBoardState() {
    const xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/state', true);
    xhttp.setRequestHeader('Content-Type', 'application/json; charset=UTF-8');
    xhttp.send();
    xhttp.onerror = function () {
        alert('Error: Could not send request to server!');
    };
    xhttp.onload = function () {
        // 1. Convert Response into JSON obj
        const response = JSON.parse(this.response);

        // 2. Check reponse was actually given
        if (!response || response.status != 'success') {
            alert('Failed to get board state!');
            return;
        }

        // 3. Get motor mode
        const motorMode = !response.motorMode ? None : response.motorMode;
        if (!motorMode) {
            alert('Error: No motor mode was returned');
            return;
        }

        // 4. Change status
        document.getElementById('Status').innerHTML = motorMode;

        // 5. Change button
        document
            .getElementById('motor-control')
            .setAttribute('value', motorMode);
        document
            .getElementById('motor-control')
            .setAttribute('class', 'btn-' + motorMode);
        document.getElementById('motor-control').innerHTML = motorMode;

        // 6. Remove Spiiner and display buttons and status
        document.getElementById("loader").style.display = "none";
        document.getElementById("Actions").style.display = "block";
        document.getElementById("Info").style.display = "block";

        // 7. Running timer
        if (!response.runTime) {
            prevTime = 0;
        } else {
            prevTime = response.runTime * 1000;
        }
        if (motorMode == motor['on']) {
            turnOnTimer();
        } else {
            turnOffTimer();
        }
    };
}

window.onload = () => {getBoardState();};