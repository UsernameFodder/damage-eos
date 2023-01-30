// Slots: text (unnamed)
// Based on https://github.com/mdn/web-components-examples popup-info-box

class PopUpInfo extends HTMLElement {
    constructor() {
        super();

        let template = document.getElementById("popup-info");
        const shadow = this.attachShadow({mode: "open"});
        shadow.appendChild(template.content.cloneNode(true));
    }
    connectedCallback() {
        // Size the info element dynamically
        const width = this.getAttribute("width") ?? 200;
        const styleElement = document.createElement("style");
        styleElement.innerText = `.info {max-width: ${width}px}`;
        const linkElement = this.shadowRoot.querySelector("[rel='stylesheet']");
        this.shadowRoot.insertBefore(styleElement, linkElement);
    }
}

// Define the new element
customElements.define("popup-info", PopUpInfo);
