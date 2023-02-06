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
            const details = damagecalc.getMoveDetails(move, false);
            details.move = move;
            return details;
        });
        // Put projectile moves in their own section
        const projectileMoves = new Set(damagecalc.getProjectileMoves());
        const supported = moves.filter(
            move => !move.unsupported && !projectileMoves.has(move.move)
        ).map(move => move.move);
        const projectile = moves.filter(
            move => !move.unsupported && projectileMoves.has(move.move)
        ).map(move => move.move);
        const unsupported = moves.filter(move => move.unsupported).map(move => move.move);

        const firstMove = supported[0]; // "Nothing"
        const alphabetizeWithExceptions = (a, b) => {
            // Keep "Nothing" as the first item
            if (a === firstMove) {
                return -1;
            } else if (b === firstMove) {
                return 1;
            }
            // The dummy moves that have "0x" in them will compare less than everything else
            // normally, but that's ugly, so kick them to the end of the list
            if (a.includes("0x") && !b.includes("0x")) {
                return 1;
            } else if (b.includes("0x") && !a.includes("0x")) {
                return -1;
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
        supported.sort(alphabetizeWithExceptions)
        unsupported.sort(alphabetizeWithExceptions);
        return [
            {group: "Normal Moves", options: supported},
            {group: "Projectile Moves", options: projectile},
            {group: "Unsupported", options: unsupported},
        ];
    }

    formTemplate() {
        return "move-form";
    }
}

customElements.define("move-form", MoveForm);
