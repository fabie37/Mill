export class Timer {
    constructor() {
        this.timerOn = false;
        this.prevTime = 0;
    }

    static formatTime(time) {
        var output = '';
        if (time >= 0 && time <= 9) {
            output = '0';
        }

        return output.concat(time.toString());
    }

    setPrevTime(prevTime) {
        this.prevTime = prevTime;
    }

    update(startTime) {
        if (this.timerOn) {
            const currentTime = new Date();
            const differenceTimeMillis =
                currentTime - startTime + this.prevTime;
            const newTime = new Date(differenceTimeMillis);
            var hours = newTime.getHours() - 1;
            var minutes = newTime.getMinutes();
            var seconds = newTime.getSeconds();
            const timeString =
                Timer.formatTime(hours) +
                ':' +
                Timer.formatTime(minutes) +
                ':' +
                Timer.formatTime(seconds);
            document.getElementById('Timer').innerHTML = timeString;

            setTimeout(() => {
                this.update(startTime);
            }, 200);
        }
    }

    turnOn() {
        this.timerOn = true;
        var startTime = new Date();
        this.update(startTime);
    }

    turnOff() {
        this.timerOn = false;
        this.prevTime = 0;
        document.getElementById('Timer').innerHTML = '00:00:00';
    }
}
