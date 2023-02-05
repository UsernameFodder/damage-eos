// Attributes: name, checked
// Slots: label (unnamed), details

class CheckboxInput extends HTMLElement {
    constructor() {
        super();

        let template = document.getElementById("checkbox-input");
        const shadow = this.attachShadow({mode: "open"});
        shadow.appendChild(template.content.cloneNode(true));

        this.inputElement = shadow.querySelector("input");
    }

    connectedCallback() {
        const name = this.getAttribute("name");
        const defaultChecked = this.hasAttribute("checked");

        if (name !== null) {
            const id = `checkbox-input-${name}`;
            this.inputElement.setAttribute("id", id);
            const labelElement = this.shadowRoot.querySelector("label");
            labelElement.setAttribute("for", id);
        }
        if (defaultChecked) {
            this.inputElement.setAttribute("checked", "");
        }
    }

    registerCallback(valueCallback) {
        valueCallback(this.getAttribute("name"), this.inputElement.checked);
        this.inputElement.addEventListener("change", (event) => {
            valueCallback(this.getAttribute("name"), event.currentTarget.checked);
        });
    }

    setChecked(value) {
        this.inputElement.checked = value;
        // trigger any callbacks that were already registered
        this.inputElement.dispatchEvent(new Event("change"));
    }
}

customElements.define("checkbox-input", CheckboxInput);
