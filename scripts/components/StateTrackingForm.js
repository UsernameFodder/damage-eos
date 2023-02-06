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

        this.inputs().forEach(
            input => this.registerCallbackFor(input)
        );
    }
    inputs() {
        // controlled-select-list-input isn't here by default since it's a composite element with
        // select-input children in the light DOM; override inputs() if it's needed
        return this.querySelectorAll("checkbox-input, checkbox-list-input, number-input, select-input");
    }
    transformId(id) {
        // Prefer the parent id as a base, but fall back to name if there's no id attribute
        const base = this.getAttribute("id") ?? this.getAttribute("name");
        return `${base}-${id}`;
    }
    transformTemplateIds(contentRoot) {
        contentRoot.querySelectorAll(`[id]`).forEach(
            element => {
                const templateId = element.getAttribute("id");
                element.setAttribute("id", this.transformId(templateId));
            }
        )
        contentRoot.querySelectorAll(`[for]`).forEach(
            element => {
                // for attributes can have space-separated lists
                const transformed = element.getAttribute("for").split(" ").map(templateId => {
                    if (!templateId) {
                        return templateId;
                    }
                    return this.transformId(templateId);
                });
                element.setAttribute("for", transformed.join(" "));
            }
        )
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

        if (value === null) {
            // Interpret null to mean delete the property
            const hadProperty = Object.hasOwn(currentObj, innerField);
            delete currentObj[innerField];
            return changed || hadProperty;
        }

        let oldValue = currentObj[innerField];
        currentObj[innerField] = value;
        if (Array.isArray(value)) {
            // Standard arrays should only come from checkbox-list-input
            // Assume they're consistenty ordered
            return !StateTrackingForm.arraysEquals(value ?? [], oldValue ?? []);
        }
        return changed || (value !== oldValue);
    }
}

export default StateTrackingForm;
