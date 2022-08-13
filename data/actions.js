const motor = {
    on: 'on',
    off: 'off',
};

const motorModeFlip = {
    on: motor['off'],
    off: motor['on'],
};

function controlMotor() {
    const mode = document.getElementById('motor-control').getAttribute('value');

    let payload = {
        mode: motorModeFlip[mode],
    };

    const xhttp = new XMLHttpRequest();
    xhttp.open('POST', '/motor-control', true);
    xhttp.setRequestHeader('Content-Type', 'application/json; charset=UTF-8');
    xhttp.send(JSON.stringify(payload));
    xhttp.onerror = function () {
        alert('Error: Could not send request to server!');
    };
    xhttp.onload = function () {
        // 1. Convert Response into JSON obj
        const response = JSON.parse(this.response);

        // 2. Check reponse was actually given
        if (!response || response.status != 'success') {
            alert('Failed to control motor!');
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

        // 6. Running timer
        if (motorMode == motor['on']) {
            turnOnTimer();
        } else {
            turnOffTimer();
        }
    };
}
