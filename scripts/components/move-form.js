// Attributes: name

import "./base/select-input.js"
import "./move-output.js"
import StateTrackingForm from "./StateTrackingForm.js"
import damagecalc from "./wasm/damagecalc.js"

class MoveForm extends StateTrackingForm {
    connectedCallback() {
        super.connectedCallback();

        const moveInput = this.querySelector(`#${this.transformId("move-form-id-input")}`);
        moveInput.populateOptions(MoveForm.getMoves());
    }
    static getMoves() {
        const moves = damagecalc.getMoves().map(move => {
            const details = damagecalc.getMoveDetails(move);
            details.move = move;
            return details;
        });
        const supported = moves.filter(move => !move.unsupported).map(move => move.move);
        const unsupported = moves.filter(move => move.unsupported).map(move => move.move);
        return [
            {group: "Supported", options: supported},
            {group: "Unsupported", options: unsupported},
        ];
    }

    formTemplate() {
        return "move-form";
    }
}

customElements.define("move-form", MoveForm);
