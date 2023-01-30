// Attributes: name

import "./base/checkbox-input.js"
import "./base/select-input.js"
import StateTrackingForm from "./StateTrackingForm.js";

class SpeciesInput extends StateTrackingForm {
    connectedCallback() {
        super.connectedCallback();

        this.speciesSelector = this.querySelector("select-input[name='species']");
        this.genderCheckbox = this.querySelector("checkbox-input[name='secondary']");
        this.secondarySuffix = "";
        this.secondaryAllowed = new Set()
    }
    updateGenderCheckbox(species) {
        if (!this.genderCheckbox) {
            // This is the initial call from connectedCallback.
            // We haven't properly set up state yet, so just return
            return;
        }

        // Disable the gender checkbox if the new species doesn't have a secondary gender
        if (this.secondaryAllowed.has(species)) {
            this.genderCheckbox.removeAttribute("disabled");
        } else {
            this.genderCheckbox.setAttribute("disabled", ""); // functionality implemented with CSS
        }
    }
    setState(name, value) {
        if (name === "species") {
            this.updateGenderCheckbox(value);
        }
        return super.setState(name, value);
    }
    callbackArg() {
        let species = this.state.species;
        if (this.state.secondary && this.secondaryAllowed.has(this.state.species)) {
            species += this.secondarySuffix;
        }
        return species;
    }

    populateOptions(options, secondarySuffix) {
        const primary = options.filter(opt => !opt.endsWith(secondarySuffix));
        this.secondaryAllowed = new Set(
            options.filter(opt => opt.endsWith(secondarySuffix)).map(opt => opt.slice(0, -secondarySuffix.length))
        );
        this.secondarySuffix = secondarySuffix;
        // As a sanity check, ensure that all items in secondaryAllowed also exist in primary
        // The opposite need not be true; not all items in primary must be in secondaryAllowed
        const primarySet = new Set(primary);
        for (const secondary of this.secondaryAllowed) {
            if (!primarySet.has(secondary)) {
                console.warn(`"${secondary}" has secondary gender without default gender?`);
            }
        }
        this.speciesSelector.populateOptions(primary);
    }

    formTemplate() {
        return "species-input";
    }
}

customElements.define("species-input", SpeciesInput);
