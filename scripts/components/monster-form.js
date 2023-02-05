// Attributes: name

import "./base/checkbox-input.js"
import "./base/checkbox-list-input.js"
import "./base/select-input.js"
import "./species-input.js"
import "./species-output.js"
import StateTrackingForm from "./StateTrackingForm.js"
import damagecalc from "./wasm/damagecalc.js"

class MonsterForm extends StateTrackingForm {
    connectedCallback() {
        super.connectedCallback();

        const monsterInput = this.querySelector(`#${this.transformId("monster-form-species-input")}`);
        monsterInput.populateOptions(damagecalc.getSpecies(), " (secondary)");
        const hiddenPowerTypeInput = this.querySelector("select-input[name='hidden_power_type']");
        hiddenPowerTypeInput.populateOptions(damagecalc.getTypes());
        const heldItemInput = this.querySelector("select-input[name='held_item.id']");
        heldItemInput.populateOptions(damagecalc.getHeldItems());

        const iqSkillsInput = this.querySelector("checkbox-list-input[name='iq_skills']");
        iqSkillsInput.populateItems(MonsterForm.getIqSkills());
        const exclusiveItemEffectsInput = this.querySelector("checkbox-list-input[name='exclusive_items.effects']");
        exclusiveItemEffectsInput.populateItems(MonsterForm.getExclusiveItemEffects());
        const statusesInput = this.querySelector("checkbox-list-input[name='statuses']");
        statusesInput.populateItems(MonsterForm.getStatuses());

        if (this.hasAttribute("default-team")) {
            const teamMemberInput = this.querySelector("checkbox-input[name='is_team_member']");
            teamMemberInput.setChecked(true);
        }
    }
    inputSelector() {
        const defaultSelector = super.inputSelector();
        // Include species-input and exclude any of its children, since it controls its own inputs
        const selectors = defaultSelector.split(",").map((s) => {
            let base = s.trim();
            return `${base}:not(species-input ${base})`;
        });
        selectors.push("species-input");
        return selectors.join(", ");
    }
    static getIqSkills() {
        return damagecalc.getIqSkills().map((iq) => ({name: iq, label: iq}));
    }
    static getExclusiveItemEffects() {
        return damagecalc.getExclusiveItemEffects().map((eff) => {
            const obj = {name: eff.name, label: eff.name};
            const altNames = eff.alternateNames ?? [];
            if (altNames.length > 0) {
                obj.details = `Items: ${altNames.join(", ")}`;
            } else {
                obj.details = "Unused in-game";
            }
            return obj;
        });
    }
    static getStatuses() {
        return damagecalc.getStatuses().map((status) => {
            const obj = {name: status.name, label: status.name};
            const altNames = status.alternateNames ?? [];
            if (altNames.length > 0) {
                if (altNames.length > 1) {
                    // Unexpected
                    console.warn(`warning: more than one alternate name for status '${status.name}': ${altNames}`);
                }
                obj.details = `Full name: ${altNames[0]}`
            }
            return obj;
        });
    }

    formTemplate() {
        return "monster-form";
    }
}

customElements.define("monster-form", MonsterForm);
