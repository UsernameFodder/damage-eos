// Attributes: name, width
// Slots: label (unnamed), details

import "./checkbox-input.js"
import InputStateTracker from "./InputStateTracker.js";

class CheckboxListInput extends InputStateTracker {
    constructor() {
        super();

        let template = document.getElementById("checkbox-list-input");
        const shadow = this.attachShadow({mode: "open"});
        shadow.appendChild(template.content.cloneNode(true));

        this.ulElement = shadow.querySelector(".items");
        this.itemElements = [];
        this.itemNameToIdx = {};
        this.values = new Set();
    }

    connectedCallback() {
        const name = this.getAttribute("name");
        const width = this.getAttribute("width") ?? 95;

        // Size the element dynamically
        const styleElement = document.createElement("style");
        styleElement.innerText = `.items {max-width: ${width}%}`;
        const linkElement = this.shadowRoot.querySelector("[rel='stylesheet']");
        this.shadowRoot.insertBefore(styleElement, linkElement);

        if (name !== null) {
            const ulId = `${name}-items`;
            this.ulElement.setAttribute("id", ulId);
            const labelElement = this.shadowRoot.querySelector(".checkbox-list-input-header");
            labelElement.setAttribute("for", ulId);
        }
    }
    
    initialState() {
        return new Set()
    }
    callbackArg() {
        // sort the value array in list order
        return Array.from(this.state).sort((a, b) => {
            const aIdx = this.itemNameToIdx[a];
            const bIdx = this.itemNameToIdx[b];
            if (aIdx !== undefined && bIdx === undefined) {
                return -1;
            } else if (aIdx === undefined && bIdx !== undefined) {
                return 1;
            } else if (aIdx === undefined && bIdx === undefined) {
                return 0;
            }
            return aIdx - bIdx;
        });
    }
    setState(name, value) {
        let changed = false;
        if (value) {
            changed = !this.state.has(name);
            this.state.add(name);
        } else {
            changed = this.state.delete(name);
        }
        return changed;
    }

    populateItems(items) {
        for (const item of items) {
            const checkbox = document.createElement("checkbox-input");
            checkbox.setAttribute("name", item.name);
            if (Object.hasOwn(item, "checked")) {
                checkbox.setAttribute("checked", "");
            }
            if (Object.hasOwn(item, "label")) {
                const label = document.createTextNode(item.label);
                checkbox.appendChild(label);
            }
            // Making popups overflow outside of a scroll box is hard;
            // use a <details> tag instead (see below)
            // if (Object.hasOwn(item, "details")) {
            //     const details = document.createElement("popup-info");
            //     details.innerText = item.details;
            //     details.setAttribute("slot", "details");
            //     checkbox.appendChild(details);
            // }
            for (const [key, value] of Object.entries(item)) {
                if (key === "label" || key === "details") {
                    continue;
                }
                checkbox.setAttribute(key, value);
            }
            const newLen = this.itemElements.push(checkbox);
            this.itemNameToIdx[item.name] = newLen - 1;
            
            const liElement = document.createElement("li");
            liElement.appendChild(checkbox);
            if (Object.hasOwn(item, "details")) {
                const details = document.createElement("details");
                const summary = document.createElement("summary");
                summary.textContent = "Details";
                const text = document.createTextNode(item.details);
                details.appendChild(summary);
                details.appendChild(text);
                liElement.appendChild(details);
            }
            this.ulElement.appendChild(liElement);

            this.registerCallbackFor(checkbox);
        }
    }
}

customElements.define("checkbox-list-input", CheckboxListInput);
