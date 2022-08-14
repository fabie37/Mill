import { Component } from './component.js';

export class Actions extends Component {
    static id = 'Actions';

    static show() {
        document.getElementById(this.id).style.display = 'grid';
    }
}
