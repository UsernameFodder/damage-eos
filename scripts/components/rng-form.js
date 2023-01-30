// Attributes: name

import StateTrackingForm from "./StateTrackingForm.js"

class RngForm extends StateTrackingForm {
    formTemplate() {
        return "rng-form";
    }
}

customElements.define("rng-form", RngForm);
