import "./base/checkbox-input.js"
import "./base/checkbox-list-input.js"
import "./base/number-input.js"
import "./base/select-input.js"
import InputStateTracker from "./base/InputStateTracker.js";

// Abstract base class
class StateTrackingForm extends InputStateTracker {
    formTemplate() {
        throw new Error("not implemented");
    }
    connectedCallback() {
        let template = document.getElementById(this.formTemplate());
        this.appendChild(this.transformTemplateIds(template.content.cloneNode(true)));

        this.querySelectorAll(this.inputSelector()).forEach(
            input => this.registerCallbackFor(input)
        );
    }
    inputSelector() {
        return "checkbox-input, checkbox-list-input, number-input, select-input";
    }
    transformId(id) {
        // Prefer the parent id as a base, but fall back to name if there's no id attribute
        const base = this.getAttribute("id") ?? this.getAttribute("name");
        return `${base}-${id}`;
    }
    transformTemplateIds(contentRoot) {
        for (const attr of ["id", "for"]) {
            contentRoot.querySelectorAll(`[${attr}]`).forEach(
                element => {
                    const templateId = element.getAttribute(attr);
                    element.setAttribute(attr, this.transformId(templateId));
                }
            )
        }
        return contentRoot
    }
    static arraysEquals(a, b) {
        if (a.length !== b.length) {
            return false;
        }
        for (let i = 0; i < a.length; i++) {
            if (a[i] !== b[i]) {
                return false;
            }
        }
        return true;
    }
    setState(name, value) {
        // Resolve "." subobject notation
        const fieldPath = name.split(".");
        const innerField = fieldPath.pop();
        let currentObj = this.state;
        let changed = false;
        for (const field of fieldPath) {
            if (!Object.hasOwn(currentObj, field)) {
                currentObj[field] = {};
                changed = true;
            }
            currentObj = currentObj[field];
        }
        let oldValue = currentObj[innerField];
        currentObj[innerField] = value;
        if (Array.isArray(value)) {
            // Standard arrays should only come from checkbox-list-input
            // Assume they're consistenty ordered
            return !StateTrackingForm.arraysEquals(value ?? [], oldValue ?? []);
        }
        return value !== oldValue;
    }
}

export default StateTrackingForm;
