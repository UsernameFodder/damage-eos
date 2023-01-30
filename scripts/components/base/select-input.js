// Attributes: name, width
// Slots: label (unnamed), details

class SelectInput extends HTMLElement {
    constructor() {
        super();

        let template = document.getElementById("select-input");
        const shadow = this.attachShadow({mode: "open"});
        shadow.appendChild(template.content.cloneNode(true));

        this.selectElement = shadow.querySelector("select");
    }

    connectedCallback() {
        const name = this.getAttribute("name");
        const width = this.getAttribute("width");

        // Size the select element dynamically
        if (width !== null) {
            const styleElement = document.createElement("style");
            styleElement.innerText = `select {max-width: ${width}em}`;
            const linkElement = this.shadowRoot.querySelector("[rel='stylesheet']");
            this.shadowRoot.insertBefore(styleElement, linkElement);
        }

        if (name !== null) {
            const id = `select-input-${name}`;
            this.selectElement.setAttribute("id", id);
            const labelElement = document.createElement("label");
            labelElement.setAttribute("for", id);
        }
    }

    static createOption(option) {
        const optionElement = document.createElement("option");
        if (Object.hasOwn(option, "name")) {
            optionElement.setAttribute("value", option.name);
            optionElement.textContent = option.label;
        } else {
            optionElement.setAttribute("value", option);
            optionElement.textContent = option;
        }
        return optionElement;
    }
    populateOptions(options) {
        for (const option of options) {
            if (Object.hasOwn(option, "group")) {
                // Group element
                const groupElement = document.createElement("optgroup");
                groupElement.setAttribute("label", option.group);
                for (const suboption of option.options) {
                    groupElement.appendChild(SelectInput.createOption(suboption));
                }
                this.selectElement.appendChild(groupElement);
            } else {
                this.selectElement.appendChild(SelectInput.createOption(option));
            }
        }
        // trigger any callbacks that were already registered
        this.selectElement.dispatchEvent(new Event("change"));
    }

    registerCallback(valueCallback) {
        valueCallback(this.getAttribute("name"), this.selectElement.value);
        this.selectElement.addEventListener("change", (event) => {
            valueCallback(this.getAttribute("name"), event.currentTarget.value);
        });
    }
}

customElements.define("select-input", SelectInput);
