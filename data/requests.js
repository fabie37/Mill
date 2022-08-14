import { ConnectionLabel } from './connection_label.js';

export function makeRequest(method, url, payload = null) {
    return new Promise(function (resolve, reject) {
        const xhttp = new XMLHttpRequest();
        xhttp.open(method, url, true);
        xhttp.setRequestHeader(
            'Content-Type',
            'application/json; charset=UTF-8'
        );
        xhttp.onerror = function () {
            ConnectionLabel.setDisconnected();
            reject({
                status: this.status,
                statusText: this.statusText,
                payload: payload,
            });
        };
        xhttp.onload = function () {
            if (this.status >= 200 && this.status <= 300) {
                const response = JSON.parse(this.response);
                ConnectionLabel.setConnected();
                resolve(response);
            } else {
                ConnectionLabel.setDisconnected();
                reject({
                    status: this.status,
                    statusText: this.statusText,
                    payload: payload,
                });
            }
        };
        xhttp.send(payload ? JSON.stringify(payload) : null);
    });
}
