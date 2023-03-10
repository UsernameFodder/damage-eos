// Attributes: name, max, min, value, step, width, list-options
// Slots: label (unnamed), details

class NumberInput extends HTMLElement {
    constructor() {
        super();

        let template = document.getElementById("number-input");
        const shadow = this.attachShadow({mode: "open"});
        shadow.appendChild(template.content.cloneNode(true));

        this.inputElement = shadow.querySelector("input");
        this.previousValue = "";
        this.minValue = 0;
        this.restorePreviousValue = this.restorePreviousValue.bind(this);
    }

    connectedCallback() {
        const name = this.getAttribute("name");
        const max = this.getAttribute("max");
        const min = this.getAttribute("min") ?? 0;
        let defaultValue = this.getAttribute("value") ?? min;
        const step = this.getAttribute("step");
        const width = this.getAttribute("width") ?? 6;
        const listOptions = this.getAttribute("list-options") ?? "";

        // Size the input element dynamically
        const styleElement = document.createElement("style");
        styleElement.innerText = `input {max-width: ${width}em}`;
        const linkElement = this.shadowRoot.querySelector("[rel='stylesheet']");
        this.shadowRoot.insertBefore(styleElement, linkElement);

        if (name !== null) {
            const id = `number-input-${name}`;
            this.inputElement.setAttribute("id", id);
            const labelElement = this.shadowRoot.querySelector("label");
            labelElement.setAttribute("for", id);

            if (listOptions !== "") {
                // Create a datalist element dynamically
                const datalistId = `${id}-datalist`;
                const datalistElement = document.createElement("datalist");
                datalistElement.setAttribute("id", datalistId);
                for (const opt of listOptions.split(" ")) {
                    if (!opt) {
                        continue;
                    }
                    const optElement = document.createElement("option");
                    optElement.setAttribute("value", opt);
                    datalistElement.appendChild(optElement);
                }
                this.shadowRoot.insertBefore(datalistElement, this.inputElement);
                this.inputElement.setAttribute("list", datalistId);
            }
        }
        this.inputElement.setAttribute("min", min);
        if (max !== null) {
            this.inputElement.setAttribute("max", max);
        }
        if (step !== null) {
            this.inputElement.setAttribute("step", step);
        }

        this.inputElement.setAttribute("value", defaultValue);
        if (!this.inputElement.validity.valid) {
            defaultValue = min;
            this.inputElement.setAttribute("value", defaultValue);
        }
        this.previousValue = defaultValue;
        this.minValue = Number(min);
        this.inputElement.addEventListener("invalid", this.restorePreviousValue);

        this.inputElement.addEventListener("input", (event) => {
            if (event.currentTarget.checkValidity()) {
                this.previousValue = event.currentTarget.value;
            }
        });
    }

    restorePreviousValue() {
        this.inputElement.value = this.previousValue;
    }
    getInputValue() {
        if (this.inputElement.validity.valid) {
            // Treat empty as the minimum value. This allows users to delete the
            // input fully before entering a new value
            if (this.inputElement.value === "") {
                return this.minValue;
            }
            return this.inputElement.valueAsNumber;
        }
        return null;
    }
    setValue(value) {
        this.inputElement.value = value;
        // trigger any callbacks
        this.inputElement.dispatchEvent(new Event("input"));
    }

    registerCallback(valueCallback) {
        const inputValue = this.getInputValue();
        if (inputValue !== null) {
            valueCallback(this.getAttribute("name"), inputValue);
        }
        this.inputElement.addEventListener("input", (event) => {
            const inputValue = this.getInputValue();
            if (inputValue !== null) {
                valueCallback(this.getAttribute("name"), inputValue);
            }
        });
    }
}

customElements.define("number-input", NumberInput);
