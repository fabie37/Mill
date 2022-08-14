import { Component } from './component.js';

export class MotorStatusLabel extends Component {
    constructor() {}

    static id = 'Status';

    static setLabel(status) {
        document.getElementById(this.id).innerHTML = status;
    }
}
