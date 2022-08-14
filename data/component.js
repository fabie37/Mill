export class Component {
    static id = 'component';

    static show() {
        document.getElementById(this.id).style.display = 'block';
    }

    static hide() {
        document.getElementById(this.id).style.display = 'none';
    }

    static setEvent(action, method) {
        document.getElementById(this.id).addEventListener(action, method);
    }
}
