import { Component } from './component.js';

export class MotorControlButton extends Component {
    constructor() {}

    static id = 'motor-control';
    static testingClass = 'testing';

    static setMode(mode) {
        document.getElementById(this.id).setAttribute('value', mode);
        document.getElementById(this.id).setAttribute('class', 'btn-' + mode);
        document.getElementById(this.id).innerHTML = mode;
    }

    static setNormal() {
        if (
            document
                .getElementById(this.id)
                .classList.contains(this.testingClass)
        ) {
            document
                .getElementById(this.id)
                .classList.remove(this.testingClass);
        }
    }

    static setTesting() {
        if (
            !document
                .getElementById(this.id)
                .classList.contains(this.testingClass)
        ) {
            document.getElementById(this.id).classList.add(this.testingClass);
        }
    }
}
