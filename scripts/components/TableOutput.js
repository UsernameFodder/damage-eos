import "./base/checkbox-input.js"
import "./base/select-input.js"

// Abstract base class
class TableOutput extends HTMLElement {
    constructor() {
        super();

        this.stateChangeHandler = this.stateChangeHandler.bind(this);
        this.setDefaults();
    }
    tableType() {
        throw new Error("not implemented");
    }
    propertyList() {
        throw new Error("not implemented");
    }
    setDefaults() {
        throw new Error("not implemented");
    }
    getContentDetails(contentId) {
        throw new Error("not implemented");
    }
    connectedCallback() {
        const templateId = `${this.tableType()}-output`;
        let template = document.getElementById(templateId);
        this.appendChild(template.content.cloneNode(true));
        this.theadElement = this.querySelector("thead");

        // Append the cloned unsupported template to a dummy node to turn the
        // DocumentFragment into a real Node, then store it for later
        let dummyNode = document.createElement("thead");
        let unsupportedTemplate = document.getElementById("table-output-unsupported");
        dummyNode.appendChild(unsupportedTemplate.content.cloneNode(true));
        this.unsupportedElement = dummyNode.querySelector(".unsupported");
        this.unsupportedElement.remove();

        const inputIds = this.getAttribute("for").split(" ");
        for (const inputId of inputIds) {
            if (!inputId) {
                continue;
            }
            const inputElement = document.getElementById(inputId);
            // needed in case the input element is after the output and hasn't been upgraded yet
            customElements.upgrade(inputElement);
            inputElement.registerCallback(this.stateChangeHandler);
        }
    }
    stateChangeHandler(source, contentId) {
        this.updateContent(source, contentId);
        this.render();
    }
    updateContent(source, contentId) {
        this[this.tableType()] = contentId;
        if (contentId) {
            Object.assign(this, this.getContentDetails(contentId));
        } else {
            this.setDefaults();
        }
    }
    render() {
        if (this.unsupported) {
            this.theadElement.appendChild(this.unsupportedElement);
        } else {
            this.unsupportedElement.remove();
        }

        const tableProps = [this.tableType()].concat(this.propertyList());
        for (const prop of tableProps) {
            const element = this.querySelector(`.tcell-${prop}`);
            element.innerText = this[prop];
        }
    }
}

export default TableOutput;
