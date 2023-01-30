// Attributes: for

import TableOutput from "./TableOutput.js"
import damagecalc from "./wasm/damagecalc.js"

class MoveOutput extends TableOutput {
    tableType() {
        return "move";
    }
    propertyList() {
        return [
            "basePower",
            "type",
            "category",
            "pp",
            "accuracy1",
            "accuracy2",
            "strikes",
            "critChance",
            "specialNotes",
        ];
    }
    setDefaults() {
        this.move = "";
        this.basePower = 0;
        this.type = "";
        this.category = "";
        this.pp = 0;
        this.accuracy1 = 0;
        this.accuracy2 = 0;
        this.strikes = 0;
        this.critChance = 0;
        this.unsupported = false;
        this.specialNotes = "";
    }
    getContentDetails(move) {
        return damagecalc.getMoveDetails(move);
    }
}

customElements.define("move-output", MoveOutput);
