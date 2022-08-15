import { MotorControlButton } from './motor_control_button.js';
import { makeRequest } from './requests.js';
import { MotorStatusLabel } from './motor_status_label.js';
import { Slider } from './slider.js';

export class Motor {
    constructor(timer) {
        this.states = { on: 'on', off: 'off' };
        this.invert = { on: this.states.off, off: this.states.on };
        this.mode = this.states.off;
        this.speed = 0;
        this.timer = timer;
    }

    async setSpeed(speed) {
        this.speed = speed;
        // await makeRequest('POST', '/speed-control', { speed: speed });
        Slider.setValue(speed);
        return speed;
    }

    async setMode(mode) {
        await makeRequest('POST', '/motor-control', {
            mode: mode,
        })
            .then(
                (res) => {
                    return this.resolveMode(res);
                },
                (res) => {
                    return this.rejectMode(res);
                }
            )
            .then(
                (res) => {
                    return this.updateModeFrontend(res);
                },
                (res) => {
                    return this.updateModeFrontend(res);
                }
            );

        return mode;
    }

    resolveMode(result) {
        const motorMode = result.motorMode;
        return motorMode;
    }

    rejectMode(result) {
        const motorMode = result.payload.mode;
        return motorMode;
    }

    updateModeFrontend(motorMode) {
        MotorControlButton.setMode(motorMode);
        MotorStatusLabel.setLabel(motorMode);
        this.mode = motorMode;

        if (motorMode == this.states.on) {
            this.timer.turnOn();
            Slider.show();
        } else {
            this.timer.turnOff();
            this.setSpeed(0);
            Slider.hide();
        }
    }

    updateSpeedFrontend(speed) {
        console.log(speed);
        Slider.setValue(speed);
        this.speed = speed;
    }

    invertMode() {
        this.setMode(this.invert[this.mode]);
    }
}
