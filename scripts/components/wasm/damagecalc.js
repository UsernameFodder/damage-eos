// Convenience wrapper to synchronously import libdamage
import createWasmModule from "./libdamage.js"

const damagecalc = await createWasmModule({
    print: function(text) { console.log("[libdamage] " + text) },
    printErr: function(text) { console.warn("[libdamage] " + text) },
});
export default damagecalc;
