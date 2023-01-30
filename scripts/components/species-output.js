// Attributes: for

import TableOutput from "./TableOutput.js"
import damagecalc from "./wasm/damagecalc.js"

class SpeciesOutput extends TableOutput {
    tableType() {
        return "species";
    }
    propertyList() {
        return [
            "gender",
            "type1",
            "type2",
            "ability1",
            "ability2",
            "weight",
        ];
    }
    setDefaults() {
        this.species = "";
        this.gender = "";
        this.type1 = "";
        this.type2 = "";
        this.ability1 = "";
        this.ability2 = "";
        this.weight = 0;
        this.size = 0;
    }
    getContentDetails(move) {
        return damagecalc.getSpeciesDetails(move);
    }
}

customElements.define("species-output", SpeciesOutput);
