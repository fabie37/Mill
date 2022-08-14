import { Component } from './component.js';

export class Spinner extends Component {
    constructor() {}

    static id = 'spinner';

    static showSpinner() {
        document.getElementById(this.id).style.display = 'block';
    }

    static removeSpinner() {
        document.getElementById(this.id).style.display = 'none';
    }
}
