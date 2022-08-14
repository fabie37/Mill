import { Component } from './component.js';

export class ConnectionLabel extends Component {
    constructor() {}

    static id = 'Connection';

    static setConnected() {
        this.setLabel('Connected');
    }

    static setDisconnected() {
        this.setLabel('Disconnected');
    }

    static setLabel(status) {
        document.getElementById(this.id).innerHTML = status;
    }
}
