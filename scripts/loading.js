const observer = new MutationObserver((mutations) => {
    if (document.body) {
        document.body.classList.add("loading");
        observer.disconnect();
    }
})
observer.observe(document.documentElement, {childList: true});

function finishLoading() {
    document.body.classList.remove("loading");
}
