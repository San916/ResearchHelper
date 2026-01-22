document.addEventListener("DOMContentLoaded", function() {
    const mainPage = document.getElementById("main-page");
    const footer = document.getElementById("footer");
    mainPage.style.display = "none";

    const mainPanel = document.getElementById("main-panel");
    mainPanel.addEventListener("wheel", function (event) {
        const inner = event.target.closest(".result-content");
        if (inner) return;

        const response = document.getElementById("response");
        const delta = event.deltaY * 0.5;
        const maxScroll = response.scrollHeight - response.clientHeight;

        response.scrollTop = Math.min(Math.max(response.scrollTop + delta, 0), maxScroll);
        event.preventDefault();
    }, { passive: false });
});

document.addEventListener("DOMContentLoaded", function() {
    const userInputForms = document.querySelectorAll(".user-input-form");
    const responseDiv = document.getElementById("response");
    const mainPage = document.getElementById("main-page");
    const titlePage = document.getElementById("title-page");
    const footer = document.getElementById("footer");

    const exampleResults = "{\"results\":[{\"title\":\"Question about realloc : r\/cprogramming\",\"link\":\"https:\/\/www.reddit.com\/r\/cprogramming\/comments\/1lrkzjb\/question_about_realloc\/\", \"id\":\"result_1\"},{\"title\":\"Proper usage of realloc()\",\"link\":\"https:\/\/stackoverflow.com/questions\/21006707\/proper-usage-of-realloc\", \"id\":\"result_2\"},{\"title\":\"OpenGL - Picking (fastest way)\",\"link\":\"https:\/\/stackoverflow.com\/questions\/28032910\/opengl-picking-fastest-way\", \"id\":\"result_3\"},{\"title\":\"How to handle realloc when it fails due to memory?\",\"link\":\"https:\/\/stackoverflow.com\/questions\/1986538\/how-to-handle-realloc-when-it-fails-due-to-memory\"},{\"title\":\"How do birds fly? We explain how they get - and stay - airborne ...\",\"link\":\"https:\/\/www.discoverwildlife.com\/animal-facts\/birds\/how-do-birds-fly\", \"id\":\"result_5\"}]}"; 

    userInputForms.forEach((userInputForm) => {
        userInputForm.addEventListener("submit", async function (event) {
            event.preventDefault();

            if (mainPage.style.display === "none") {
                titlePage.style.display = "none";
                mainPage.style.display = "flex";
                footer.style.display = "block";
                footer.style.position = "fixed";
            }

            displayResults(JSON.parse(exampleResults), "Example query");
            return;

            const userInput = document.getElementById("user-input").value;

            if (!userInput.trim()) {
                return;
            }

            try {
                const response = await fetch("/submit", {
                    method: "POST",
                    headers: {
                        "Content-Type": "application/x-www-form-urlencoded",
                    },
                    body: `user_input=${encodeURIComponent(userInput)}`
                });
                
                if (response.ok) {
                    const data = await response.text();
                    displayResults(JSON.parse(data), userInput);
                } else {
                    responseDiv.textContent = `Error: ${response.status}`;
                }
            } catch (error) {
                responseDiv.textContent = `Error: ${error.message}`;
                console.error("Error:", error);
            }

            document.querySelectorAll(".user-input").forEach((user_input) => {user_input.value = ""});
        });
    });

    function displayResults(data, query) {
        responseDiv.innerHTML = "";

        const queryElement = document.createElement("div");
        queryElement.className = "response-header";
        queryElement.innerHTML = `<h1>Results for: "${query}"</h2>`;
        responseDiv.appendChild(queryElement);
        
        if (!data.results || data.results.length === 0) {
            responseDiv.innerHTML += `<div class="no-results">No results!</div>`;
            return;
        }
        
        const responseContent = document.createElement("div");
        responseContent.className = "response-content";

        data.results.forEach((result, index) => {
            const resultElement = document.createElement("div");
            resultElement.className = "result-item";
            
            const resultExpander = document.createElement("div");
            resultExpander.className = "result-expander";
            resultExpander.textContent = "▼";

            const resultHeader = document.createElement("div");
            resultHeader.className = "result-header";
            resultHeader.appendChild(resultExpander);
            resultHeader.innerHTML += `
                <a href="${result.link}" target="_blank" rel="noopener noreferrer">
                    <h3 class="result-title">${result.title}</h3>
                </a>
            `;

            const resultContent = document.createElement("div");
            resultContent.className = "result-content";
            resultContent.style.display = "none";
            
            resultHeader.addEventListener("click", async function() {
                const visible = resultContent.style.display !== "none";
                resultContent.style.display = "none";
                if (visible) {
                    resultContent.style.display = "none";
                    resultExpander.textContent = "▼";
                } else {
                    resultContent.style.display = "block";
                    resultExpander.textContent = "▲";
                    
                    if (!resultContent.dataset.loaded) {
                        await fetchAndDisplayContent(result.link, resultContent);
                    }
                }
            });

            resultElement.appendChild(resultHeader);
            resultElement.appendChild(resultContent);
            responseContent.appendChild(resultElement);
        });
        
        responseDiv.appendChild(responseContent);
    }

    async function fetchAndDisplayContent(resultLink, contentArea) {
        try {
            const response = await fetch(`/content?url=${encodeURIComponent(resultLink)}`);
            
            if (response.ok) {
                const escaped = response.headers.get("Html-Escaped") === "true";
                const data = await response.text();
                const jsonData = JSON.parse(data);
                jsonData.content.forEach((item) => {
                    let html = item.content_body;

                    if (escaped) {
                        const txt = document.createElement("textarea");
                        txt.innerHTML = html;
                        html = txt.value;
                    }
                    console.log(item);

                    const contentItem = document.createElement("div");
                    contentItem.className = "content-item";

                    const contentText = document.createElement("div");
                    contentText.className = "content-text";
                    contentText.innerHTML = html;

                    const contentHeader = document.createElement("div");
                    contentHeader.className = "content-header";
                    contentHeader.innerHTML = `Score: ${item.score >= 0 ? item.score : "N/A"}`;

                    const commentExpander = document.createElement("div");
                    commentExpander.className = "comment-expander";
                    commentExpander.textContent = "[close]";
                    contentHeader.addEventListener("click", function() {
                        const closing = commentExpander.textContent === "[close]";
                        if (closing) {
                            commentExpander.textContent = "[expand]";
                            contentText.style.display = "none";
                        } else {
                            commentExpander.textContent = "[close]";
                            contentText.style.display = "block";
                        }
                    });

                    const invisibleBorder = document.createElement("div");
                    invisibleBorder.className = "invisible-border";
                    invisibleBorder.addEventListener("click", function() {
                        contentHeader.click();
                    })

                    contentHeader.appendChild(commentExpander);

                    contentItem.appendChild(contentHeader);
                    contentItem.appendChild(contentText);
                    contentItem.appendChild(invisibleBorder);

                    contentArea.appendChild(contentItem);
                });
                contentArea.dataset.loaded = true;
            } else {
                contentArea.textContent = `Error: ${response.status}`;
            }
        } catch (error) {
            contentArea.textContent = `Error: ${error.message}`;
            console.error("Error:", error);
        }
    }
});

function expandComment() {
    document.get
}

function onCloseMenuEnd(event) {
    document.getElementById("expand-button").style.display = "block";
    document.getElementById("menu").removeEventListener('transitionend', onCloseMenuEnd);
}

function expandMenu() {
    document.getElementById("expand-button").style.display = "none";
    document.getElementById("close-button").style.display = "block";
    document.getElementById("footer").style.left = "125px";
    document.getElementById("menu").classList.add('open');
}

function closeMenu() {
    document.getElementById("footer").style.left = "0px";
    document.getElementById("menu").classList.remove('open');
    document.getElementById("menu").addEventListener('transitionend', onCloseMenuEnd);
}