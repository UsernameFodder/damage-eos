// Attributes: name

import "./base/checkbox-list-input.js"
import "./base/select-input.js"
import StateTrackingForm from "./StateTrackingForm.js"
import damagecalc from "./wasm/damagecalc.js"

class DungeonForm extends StateTrackingForm {
    static FIXED_SUBSTITUTE_ROOM = 110;

    connectedCallback() {
        super.connectedCallback();

        const weatherInput = this.querySelector("select-input[name='weather']");
        weatherInput.populateOptions(damagecalc.getWeatherTypes());

        const otherMonstersIq = this.querySelector("checkbox-list-input[name='other_monsters.iq']");
        otherMonstersIq.populateItems([{name: "cheerleader", label: "Cheerleader", details: "Whether an ally with Cheerleader is next to the attacker"}]);

        const otherMonstersAbilities = this.querySelector("checkbox-list-input[name='other_monsters.abilities']");
        otherMonstersAbilities.populateItems([
            {name: "flower gift", label: "Flower Gift", details: "Whether Flower Gift is active for a Pokémon on the attacker's team"},
            {name: "lightningrod", label: "LightningRod", details: "Whether a Pokémon with LightningRod on the defender's team is in the room"},
            {name: "storm drain", label: "Storm Drain", details: "Whether a Pokémon with Storm Drain on the defender's team is in the room"},
        ]);
    }

    formTemplate() {
        return "dungeon-form";
    }
    setState(name, value) {
        switch (name) {
            case "substitute_room":
                name = "fixed_room_id";
                value = value ? DungeonForm.FIXED_SUBSTITUTE_ROOM : 0;
                break;
        }
        return super.setState(name, value);
    }
}

customElements.define("dungeon-form", DungeonForm);
