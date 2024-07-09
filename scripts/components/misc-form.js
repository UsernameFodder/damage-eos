// Attributes: name

import "./base/select-input.js"
import StateTrackingForm from "./StateTrackingForm.js"
import damagecalc from "./wasm/damagecalc.js"

class MiscForm extends StateTrackingForm {
    connectedCallback() {
        super.connectedCallback();

        const versionInput = this.querySelector("select-input[name='version']");
        versionInput.populateOptions(damagecalc.getVersions());
    }

    formTemplate() {
        return "misc-form";
    }
}

customElements.define("misc-form", MiscForm);
