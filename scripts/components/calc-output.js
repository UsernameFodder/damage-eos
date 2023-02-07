import "./base/checkbox-input.js"
import "./base/popup-info.js"
import damagecalc from "./wasm/damagecalc.js"

class CalcOutput extends HTMLElement {
    constructor() {
        super();

        this.state = {};
        this.registered = false;
        this.setState = this.setState.bind(this);
        this.monster1 = null;
        this.monster2 = null;
        this.showUnsetModifers = false;
    }
    connectedCallback() {
        let template = document.getElementById("calc-output");
        this.appendChild(template.content.cloneNode(true));

        const modifiersToggle = this.querySelector("checkbox-input[name='modifiers-toggle']");
        modifiersToggle.registerCallback((source, value) => {
            this.showUnsetModifers = value;
            this.updateModifierDetailsVisibility();
        });
    }
    setState(name, value) {
        this.state[name] = value;
        if (this.registered) {
            this.render();
        }
    }
    registerInputs(move, attacker, defender, dungeon, rng) {
        for (const input of [move, attacker, defender, dungeon, rng]) {
            input.registerCallback(this.setState);
        }
        this.monster1 = attacker;
        this.monster2 = defender;
        this.registered = true;
        this.render();
    }
    swapMonsters() {
        const name1 = this.monster1.getAttribute("name");
        const name2 = this.monster2.getAttribute("name");
        const state1 = this.state[name1];
        const state2 = this.state[name2];

        this.monster1.setAttribute("name", name2);
        this.monster2.setAttribute("name", name1);
        this.state[name1] = state2;
        this.state[name2] = state1;
        this.render();
    }

    setData(selectorValuePairs) {
        for (const [selector, value] of selectorValuePairs) {
            const element = this.querySelector(selector);
            element.textContent = value;
        }
    }
    render() {
        const rawInput = this.querySelector(".calc-json-input .raw-json");
        rawInput.innerText = JSON.stringify(this.state, null, 4);
        const result = this.calcDamage(this.state);
        const rawOutput = this.querySelector(".calc-json-output .raw-json");
        rawOutput.innerText = JSON.stringify(result, null, 4);

        this.setData([
            [".calc-min-damage", result.minDamage],
            [".calc-max-damage", result.maxDamage],
            // Since random variation is symmetric, the pre-variance damage calc
            // is equivalent to the average
            [".calc-avg-damage", result.details.calc.damageCalc],
            [".calc-healed", result.healed ? " (healed)" : ""],
            [".calc-hit-chance", result.guaranteedMiss ? "guaranteed miss" : result.hitChance],
            [".calc-crit-chance", result.guaranteedMiss ? 0 : result.critChance],
        ]);
        const resultDetails = this.querySelector(".calc-result-details");
        const calcDetails = this.querySelector(".calc-calculation-details");
        const modifierDetails = this.querySelector(".calc-modifier-details");
        if (result.guaranteedMiss) {
            for (const detailsElement of [resultDetails, calcDetails, modifierDetails]) {
                detailsElement.removeAttribute("open");
                detailsElement.setAttribute("disabled", ""); // functionality implemented with CSS
                // detailsElement.setAttribute("hidden", "");
            }
            return;
        } else {
            for (const detailsElement of [resultDetails, calcDetails, modifierDetails]) {
                detailsElement.removeAttribute("disabled");
                // detailsElement.removeAttribute("hidden");
            }
        }

        const details = result.details;
        const calc = details.calc;
        this.setData([
            // Result details
            [".details-message", details.damageMessage],
            [".details-matchup", details.typeMatchup],
            [".details-type", details.moveType],
            [".details-category", details.moveCategory],
            [".details-critical", details.criticalHit],
            [".details-immunity", details.fullTypeImmunity],
            [".details-no-damage", details.noDamage],
            // Calculation details
            [".details-offensive-stage", calc.offensiveStatStage],
            [".details-defensive-stage", calc.defensiveStatStage],
            [".details-offensive-stat", calc.offensiveStat],
            [".details-defensive-stat", calc.defensiveStat],
            [".details-offense", calc.offenseCalc],
            [".details-defense", calc.defenseCalc],
            [".details-at", calc.damageCalcAt],
            [".details-def", calc.damageCalcDef],
            [".details-flv", calc.damageCalcFlv],
            [".details-base", calc.damageCalcBase],
            [".details-static-mult", calc.staticDamageMult],
            [".details-damage", calc.damageCalc],
            [".details-min-random-mult", calc.minRandomDamageMultPct],
            [".details-max-random-mult", calc.maxRandomDamageMultPct],
        ]);

        const modifiers = calc.modifiers;
        this.setData([
            [".details-item-atk", modifiers.itemAtk],
            [".details-item-spatk", modifiers.itemSpAtk],
            [".details-item-def", modifiers.itemDef],
            [".details-item-spdef", modifiers.itemSpDef],
            [".details-ability-offense", modifiers.abilityOffense],
            [".details-ability-defense", modifiers.abilityDefense],
            [".details-iq-offense", modifiers.iqSkillOffense],
            [".details-iq-defense", modifiers.iqSkillDefense],
            [".details-scope-lens-sharpshooter", modifiers.scopeLensOrSharpshooter],
            [".details-patsy-band", modifiers.patsyBand],
            [".details-half-physical", modifiers.halfPhysicalDamage],
            [".details-half-special", modifiers.halfSpecialDamage],
            [".details-focus-energy", modifiers.focusEnergy],
            [".details-type-advantage-master", modifiers.typeAdvantageMaster],
            [".details-cloudy", modifiers.cloudyDrop],
            [".details-rain", modifiers.rainMultiplier],
            [".details-sunny", modifiers.sunnyMultiplier],
            [".details-thick-fat-heatproof", modifiers.thickFatHeatproof],
            [".details-flash-fire", modifiers.flashFire],
            [".details-levitate", modifiers.levitate],
            [".details-overgrow", modifiers.overgrow],
            [".details-swarm", modifiers.swarm],
            [".details-blaze-dry-skin", modifiers.blazeDrySkin],
            [".details-scrappy", modifiers.scrappy],
            [".details-super-luck", modifiers.superLuck],
            [".details-sniper", modifiers.sniper],
            [".details-stab", modifiers.stab],
            [".details-mud-sport-fog", modifiers.mudSportFog],
            [".details-water-sport", modifiers.waterSport],
            [".details-charge", modifiers.charge],
            [".details-ghost-immunity", modifiers.ghostImmunity],
            [".details-skull-bash", modifiers.skullBash],
        ]);
        this.updateModifierDetailsVisibility();
    }
    updateModifierDetailsVisibility() {
        this.querySelectorAll(".calc-modifier-details td").forEach((td) => {
            const tr = td.parentElement;
            const unset = (td.textContent === "false" || Number(td.textContent) === 0);
            if (!this.showUnsetModifers && unset) {
                tr.setAttribute("hidden", "");
            } else {
                tr.removeAttribute("hidden");
            }
        })
    }

    calcDamage(config) {
        return damagecalc.calcDamage(JSON.stringify(config));
    }
}

customElements.define("calc-output", CalcOutput);
