// Slots: label (unnamed), details, items

import "./checkbox-input.js"
import "./select-input.js"
import InputStateTracker from "./InputStateTracker.js";

class ControlledSelectListInput extends InputStateTracker {
    constructor() {
        super();

        let template = document.getElementById("controlled-select-list-input");
        const shadow = this.attachShadow({mode: "open"});
        shadow.appendChild(template.content.cloneNode(true));

        this.itemSlot = this.shadowRoot.querySelector("slot[name='items']");
        const controlElement = shadow.querySelector("checkbox-input[name='control']");
        this.registerCallbackFor(controlElement);
    }
    connectedCallback() {
        this.selectElements = this.querySelectorAll("select-input");
        this.selectElements.forEach(input => {
            // needed in case the input element hasn't been upgraded yet
            customElements.upgrade(input);
            this.registerCallbackFor(input)
        });
    }

    executeCallbacks() {
        for (const [key, value] of Object.entries(this.state)) {
            if (key === "control") {
                continue;
            }
            const controlledValue = this.state.control ? value : null;
            for (const callback of this.callbacks) {
                callback(key, controlledValue);
            }
        }
    }
    render() {
        if (this.state.control) {
            this.itemSlot.removeAttribute("hidden");
        } else {
            this.itemSlot.setAttribute("hidden", "");
        }
    }
    setState(name, value) {
        const oldValue = this.state[name];
        this.state[name] = value;
        if (name === "control") {
            this.render();
        }
        return value !== oldValue;
    }

    populateOptions(options) {
        this.selectElements.forEach(input =>input.populateOptions(options));
    }
}

customElements.define("controlled-select-list-input", ControlledSelectListInput);
