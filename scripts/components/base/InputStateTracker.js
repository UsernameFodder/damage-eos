// Abstract base class
class InputStateTracker extends HTMLElement {
    constructor() {
        super();

        this.state = this.initialState();
        this.callbacks = [];
        this.stateChangeHandler = this.stateChangeHandler.bind(this);
    }
    initialState() {
        return {};
    }
    setState(name, value) {
        throw new Error("not implemented");
    }
    callbackArg() {
        return this.state;
    }
    registerCallbackFor(inputElement) {
        inputElement.registerCallback(this.stateChangeHandler);
    }
    executeCallbacks() {
        const value = this.callbackArg();
        for (const callback of this.callbacks) {
            callback(this.getAttribute("name"), value);
        }
    }
    stateChangeHandler(name, value) {
        const changed = this.setState(name, value);
        if (changed) {
            this.executeCallbacks();
        }
    }
    registerCallback(valueCallback) {
        this.callbacks.push(valueCallback);
        this.executeCallbacks();
    }
}

export default InputStateTracker;
