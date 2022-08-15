import { Component } from './component.js';

export class Slider extends Component {
    static id = 'slider';
    static input_id = 'slider-input';

    static setValue(val) {
        document.getElementById(this.input_id).value = val;
    }

    static getValue() {
        return document.getElementById(this.input_id).value;
    }

    static show() {
        document.getElementById(this.id).style.display = 'grid';
    }

    static setEvent(action, method) {
        document.getElementById(this.input_id).addEventListener(action, method);
    }
}
