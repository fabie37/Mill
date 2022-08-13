var timerOn = false;
var prevTime = 0;

function updateTimer(startTime) {
    if (timerOn) {
        const currentTime = new Date();
        const differenceTimeMillis = currentTime - startTime + prevTime;
        const newTime = new Date(differenceTimeMillis);
        var hours = newTime.getHours() - 1;
        var minutes = newTime.getMinutes();
        var seconds = newTime.getSeconds();
        const timeString =
            formatTime(hours) +
            ':' +
            formatTime(minutes) +
            ':' +
            formatTime(seconds);
        document.getElementById('Timer').innerHTML = timeString;

        setTimeout(()=>{updateTimer(startTime)}, 200);
    }
}

function turnOnTimer() {
    timerOn = true;
    var startTime = new Date();
    updateTimer(startTime);
}

function turnOffTimer() {
    timerOn = false;
    prevTime = 0;
    document.getElementById('Timer').innerHTML = '00:00:00';
}

function formatTime(time) {
    var output = '';
    if (time >= 0 && time <= 9) {
        output = '0';
    }

    return output.concat(time.toString());
}
