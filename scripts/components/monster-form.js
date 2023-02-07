// Attributes: name

import "./base/checkbox-input.js"
import "./base/checkbox-list-input.js"
import "./base/controlled-select-list-input.js"
import "./base/select-input.js"
import "./species-input.js"
import "./species-output.js"
import StateTrackingForm from "./StateTrackingForm.js"
import damagecalc from "./wasm/damagecalc.js"

class MonsterForm extends StateTrackingForm {
    connectedCallback() {
        super.connectedCallback();

        this.monsterInput = this.querySelector(`#${this.transformId("monster-form-species-input")}`);
        this.monsterInput.populateOptions(damagecalc.getSpecies(), " (secondary)");
        const overrideTypesInput = this.querySelector(".override-types-input");
        overrideTypesInput.populateOptions(damagecalc.getTypes());
        const overrideAbilitiesInput = this.querySelector(".override-abilities-input");
        overrideAbilitiesInput.populateOptions(MonsterForm.getAbilities());
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

        this.statPresetMessage = this.querySelector(".stat-preset-message");
        this.statPresetMessageTimeoutID = null;
        this.hpInput = this.querySelector("number-input[name='hp']");
        this.maxHpInput = this.querySelector("number-input[name='max_hp']");
        this.atkInput = this.querySelector("number-input[name='atk']");
        this.spAtkInput = this.querySelector("number-input[name='sp_atk']");
        this.defInput = this.querySelector("number-input[name='def']");
        this.spDefInput = this.querySelector("number-input[name='sp_def']");
        const statPresetButton = this.querySelector("button.stat-preset-button");
        statPresetButton.onclick = this.fetchStatPreset.bind(this);
    }
    inputs() {
        const defaultInputs = Array.from(super.inputs());
        // Include species-input and exclude any of its children, since it controls its own inputs
        // Similarly for controlled-select-list-input elements
        const compositeInputs = Array.from(this.querySelectorAll("species-input, controlled-select-list-input"));
        return compositeInputs.concat(defaultInputs.filter(input =>
            compositeInputs.every(composite => !composite.contains(input))
        ));
    }
    static getAbilities() {
        const abilities = damagecalc.getAbilities();
        const firstAbility = abilities[0]; // "Unknown"
        const alphabetizeWithExceptions = (a, b) => {
            // Keep "Unknown" as the first item
            if (a === firstAbility) {
                return -1;
            } else if (b === firstAbility) {
                return 1;
            }
            // Otherwise, compare strings case-insensitively to alphabetize the array
            const aLower = a.toLowerCase();
            const bLower = b.toLowerCase();
            if (aLower < bLower) {
                return -1;
            } else if (bLower < aLower) {
                return 1;
            } else {
                return 0;
            }
        }
        abilities.sort(alphabetizeWithExceptions);
        return abilities;
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

    fetchStatPreset() {
        let message = "";
        const species = this.monsterInput.baseSpecies();
        let level = this.state.level
        if (level < 1) {
            level = 1;
            message = "Using level 1 stats";
        } else if (level > 100) {
            level = 100;
            message = "Using level 100 stats";
        }
        fetch(this.statPresetDataURI(species))
            .then(response => {
                if (!response.ok) {
                    throw new Error(`Could not fetch level-up stats for ${species} [code ${response.status}]`);
                }
                return response.json();
            })
            .then(data => {
                this.reportStatPresetMessage(message);
                this.applyStatPreset(data[level - 1]);
            })
            .catch(error => this.reportStatPresetMessage(error));
    }
    statPresetDataURI(species) {
        // Deal with forms
        if (species.includes("Unown")) {
            species = "Unown";
        } else if (species.includes("Celebi")) {
            species = "Celebi";
        } else if (species.includes("Castform")) {
            species = "Castform";
        } else if (species.includes("Kecleon")) {
            species = "Kecleon";
        } else if (species.includes("Deoxys")) {
            species = "Deoxys";
        } else if (species.includes("Burmy")) {
            species = "Burmy";
        } else if (species.includes("Wormadam")) {
            species = "Wormadam";
        } else if (species.includes("Cherrim")) {
            species = "Cherrim";
        } else if (species.includes("Shellos")) {
            species = "Shellos";
        } else if (species.includes("Gastrodon")) {
            species = "Gastrodon";
        } else if (species.includes("Dialga")) {
            species = "Dialga";
        } else if (species.includes("Giratina")) {
            species = "Giratina";
        } else if (species.includes("Shaymin")) {
            species = "Shaymin";
        } else if (species.includes("Arceus")) {
            species = "Arceus";
        }
        return encodeURIComponent(`levelup-stats/${species}.json`);
    }
    applyStatPreset(preset) {
        this.hpInput.setValue(preset.hp);
        this.maxHpInput.setValue(preset.hp);
        this.atkInput.setValue(preset.atk);
        this.spAtkInput.setValue(preset.spa);
        this.defInput.setValue(preset.def);
        this.spDefInput.setValue(preset.spd);
    }
    reportStatPresetMessage(message) {
        this.setStatPresetMessage(message);
        if (message !== "") {
            this.statPresetMessageTimeoutID = setTimeout(this.setStatPresetMessage.bind(this), 5000);
        }
    }
    setStatPresetMessage(newMessage) {
        if (this.statPresetMessageTimeoutID !== null) {
            clearTimeout(this.statPresetMessageTimeoutID);
            this.statPresetMessageTimeoutID = null;
        }
        this.statPresetMessage.innerText = newMessage ?? "";
    }
}

customElements.define("monster-form", MonsterForm);
