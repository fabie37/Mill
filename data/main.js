import { Motor } from './motor.js';
import { Spinner } from './spinner.js';
import { Actions } from './actions.js';
import { Info } from './info.js';
import { makeRequest } from './requests.js';
import { MotorControlButton } from './motor_control_button.js';
import { Timer } from './timer.js';
import { Slider } from './slider.js';

const timer = new Timer();
const motor = new Motor(timer);
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// 1. Setup websockets
function initWebsockets() {
    console.log("Attempting a new websocket connection...");
    websocket = new WebSocket(gateway);
    websocket.onopen = wsOpen;
    websocket.onclose = wsClose;
    websocket.onmessage = wsMessage;
}

function wsOpen(event) {
    console.log("Websocket connection opened.");
}

function wsClose(event) {
    console.log("Websocket connection closed.");
    setTimeout(initWebSocket, 2000);
}

function wsMessage(event) {
    console.log("Websocket received a message");
}


// 2. Get state of motor
async function requestState() {
    Spinner.show();
    await makeRequest('GET', '/state')
        .then(
            (res) => {
                return resolveStateRequest(res);
            },
            (res) => {
                return rejectStateRequest(res);
            }
        )
        .finally(() => {
            Spinner.hide();
            Actions.show();
            Info.show();
        });
}

function resolveStateRequest(result) {
    const motorMode = result.motorMode;
    const runTime = result.runTime;
    const motorSpeed = result.speed;
    motor.updateSpeedFrontend(motorSpeed);
    motor.updateModeFrontend(motorMode);
    timer.setPrevTime(runTime * 1000);
    if (motorMode == motor.states.on) {
        timer.turnOn();
    } else {
        timer.turnOff();
    }
}

function rejectStateRequest(result) {
    motor.updateSpeedFrontend(0);
    motor.updateModeFrontend(motor.states.off);
    timer.turnOff();
}

window.onload = () => {
    initWebsockets();
    requestState();
};

// 2. Bind actions to action components
MotorControlButton.setEvent('click', () => {
    motor.invertMode();
});

Slider.setEvent('input', () => {
    websocket.send(JSON.stringify({speed: Slider.getValue()}));
    motor.setSpeed(Slider.getValue());
});
