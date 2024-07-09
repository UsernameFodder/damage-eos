import "./components/calc-output.js"
import "./components/dungeon-form.js"
import "./components/monster-form.js"
import "./components/move-form.js"
import "./components/rng-form.js"
import "./components/misc-form.js"

const moveForm = document.getElementById("move-ui");
const monster1Form = document.getElementById("monster1-ui");
const monster2Form = document.getElementById("monster2-ui");
const dungeonForm = document.getElementById("dungeon-ui");
const rngForm = document.getElementById("rng-ui");
const miscForm = document.getElementById("misc-ui");
const calcOutput = document.getElementById("calc-results");

calcOutput.registerInputs(moveForm, monster1Form, monster2Form, dungeonForm, rngForm, miscForm);

const swapMonstersButton = document.getElementById("swap-monsters-toggle");
swapMonstersButton.onclick = () => {
    const monster1Section = document.querySelector("#monster1-section");
    const monster2Section = document.querySelector("#monster2-section");
    const monster1Name = monster1Section.getAttribute("name");
    const monster2Name = monster2Section.getAttribute("name");

    const monster1Heading = monster1Section.querySelector("h2");
    const monster2Heading = monster2Section.querySelector("h2");
    const monster1Title = monster1Heading.textContent;
    const monster2Title = monster2Heading.textContent;

    monster1Heading.textContent = monster2Title;
    monster2Heading.textContent = monster1Title;
    monster1Section.setAttribute("name", monster2Name);
    monster2Section.setAttribute("name", monster1Name);
    calcOutput.swapMonsters();
}

finishLoading();
